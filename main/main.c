#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "esp_random.h"

#include "p3.h"

//cola para recibir los eventos de los botones desde la ISR
static QueueHandle_t colaBotones;
static const gpio_num_t botones[]={PIN_BOTON_IZQ, PIN_BOTON_DER,PIN_BOTON_DISPARO,PIN_BTN_TERMINAR_JUEGO};
static const gpio_num_t leds[]={PIN_LED0,PIN_LED1,PIN_LED2,PIN_LED3,PIN_LED4};

static void gpio_interrupt_handler(void *args);
static void render_pantalla(const estado_juego_t *estado);
static void iniciar_juego();

//memoria de las variables globales. 
estado_juego_t st_estado_juego={.max_puntuacion=0};
SemaphoreHandle_t st_estado_juego_mutex, st_estado_juego_countSem;

int cnt_render_pantalla=0; //contador para saber cuántas veces renderizar pantalla, para saber si tener pantalla estatica
int posX, posY;
char disparo_jugador='|',disparo_enemigo = 'V', caracter_enemigo = '*';

void app_main(void){
	colaBotones = xQueueCreate(10, sizeof(int));
	gpio_install_isr_service(0);
	configurar_btns(botones,cnt_elementos(botones));
	configurar_leds(leds,cnt_elementos(leds));
	iniciar_juego();
	printf("------SPACE DEFENDER----- \n");
	printf("------PRESIONA UN BOTON PARA INICIAR----- \n");
	
	st_estado_juego_mutex = xSemaphoreCreateMutex();
	st_estado_juego_countSem = xSemaphoreCreateCounting(MAX_ENEMIGOS, MAX_ENEMIGOS);

	crear_Tareas_enemigos();
	xTaskCreate(tarea_pantalla, "tarea_pantalla", 4096, NULL, 1, NULL);
	xTaskCreate(tarea_entrada, "tarea_entrada", 2048, NULL, 2, NULL);
	xTaskCreate(tarea_juego, "tarea_juego", 4096, NULL, 2, NULL);
	xTaskCreate(tarea_leds, "tarea_leds", 4096, NULL, 1, NULL);
}

void configurar_btns(const gpio_num_t *pines, int cnt){
	for (int i=0;i<cnt;i++){
		gpio_set_direction(pines[i], GPIO_MODE_INPUT);
    gpio_set_pull_mode(pines[i], GPIO_PULLDOWN_ONLY);
    gpio_set_intr_type(pines[i], GPIO_INTR_NEGEDGE);
		gpio_isr_handler_add(pines[i], gpio_interrupt_handler, (void*)pines[i]);
	}
}

void configurar_leds(const gpio_num_t *pines, int cnt){
	for (int i=0;i<cnt;i++){
		gpio_set_direction(pines[i], GPIO_MODE_OUTPUT);
		gpio_set_level(pines[i],!state);
	}
}

static void IRAM_ATTR gpio_interrupt_handler(void *args){
    int pinNumber = (int) args;
    xQueueSendFromISR(colaBotones, &pinNumber, NULL);
}

static void iniciar_juego(){
	memset(&st_estado_juego,0,sizeof(st_estado_juego));
	st_estado_juego.nave.x= ANCHO_TABLERO / 2;
	st_estado_juego.vidas=VIDAS;

	st_estado_juego.juego_iniciado=false;
	st_estado_juego.final_juego=false;

	cnt_render_pantalla=1;

	st_estado_juego.ultima_puntuacion =0;
}

void generar_posicion_random(int *x, int *y){
	*x=esp_random() % ANCHO_TABLERO;
	*y=esp_random() % ALTO_TABLERO;
}

void pintar_entidades(const entidad_t entidades[], int cnt, char grid[ALTO_TABLERO][ANCHO_TABLERO],char caracter){
	for (int i=0; i<cnt; i++){
		if(entidades[i].activo){
			grid[entidades[i].y][entidades[i].x]=caracter;
		}
	}
}

void mover_disparos(entidad_t disparos[],int cnt,int direccion){
	for (int i=0;i<cnt;i++){
		if(disparos[i].activo){
			disparos[i].y+=direccion;

			//Si se sale del tablero se desactiva
			if(disparos[i].y<0 || disparos[i].y>=ALTO_TABLERO){
				disparos[i].activo=false;
			}
		}
	}
}

void elegir_enemigo_disparo(){
	//Elegir si enemigo ataca, si es 0 no se ataca al jugador
	if(esp_random() %2 ==0){
		return;
	}

	int cantidad = 0,elegido;
	int enemigos_activos[MAX_ENEMIGOS];

	for(int i=0;i<MAX_ENEMIGOS;i++){
		if(st_estado_juego.enemigos[i].activo){
			enemigos_activos[cantidad]=i;
			cantidad++;
		}
	}

	if(cantidad==0){
		return;
	}

	elegido=enemigos_activos[esp_random() % cantidad];

	for (int i=0;i<MAX_DISPAROS_ENEMIGO;i++){
		if(!st_estado_juego.disparos_enemigo[i].activo){
			st_estado_juego.disparos_enemigo[i]=(entidad_t){true,st_estado_juego.enemigos[elegido].x,st_estado_juego.enemigos[elegido].y};
			break;
		}
	}

}
void colision_enemigo_jugador(){
	for(int i=0;i<MAX_ENEMIGOS;i++){
		if(!st_estado_juego.enemigos[i].activo){
			continue;
		}
		if(st_estado_juego.enemigos[i].x== st_estado_juego.nave.x && st_estado_juego.enemigos[i].y==ALTO_TABLERO-1){
			st_estado_juego.enemigos[i].activo=false;
			st_estado_juego.vidas--;//Restar vida

			if(st_estado_juego.vidas<=0){
				st_estado_juego.final_juego=true;
				st_estado_juego.juego_iniciado=false;
				cnt_render_pantalla=1;
			}
		}
	}
}

void colision_disparo_enemigo(){
	for(int i=0;i<MAX_DISPAROS_PLAYER;i++){
		if(!st_estado_juego.disparos_jugador[i].activo){
			continue;
		}
		for(int j=0;j<MAX_ENEMIGOS;j++){
			if(!st_estado_juego.enemigos[j].activo){
				continue;
			}
			//Comprobar si el enemigo y el disparo están en la misma posición
			if(st_estado_juego.disparos_jugador[i].x==st_estado_juego.enemigos[j].x && st_estado_juego.disparos_jugador[i].y==st_estado_juego.enemigos[j].y){
				//Desaparecer disparo y enemigo
				st_estado_juego.disparos_jugador[i].activo=false;
				st_estado_juego.enemigos[j].activo=false;

				//Aumentar puntuación
				st_estado_juego.score++;
				break;//Disparo ya no existe
			}
		}
	}
}

void colision_disparo_jugador(){
	for(int i=0;i<MAX_DISPAROS_ENEMIGO;i++){
		if(!st_estado_juego.disparos_enemigo[i].activo){
			continue;
		}
		if(st_estado_juego.disparos_enemigo[i].x== st_estado_juego.nave.x && st_estado_juego.disparos_enemigo[i].y==ALTO_TABLERO-1){
			st_estado_juego.disparos_enemigo[i].activo=false;//DESAPARECE DISPARO 
			st_estado_juego.vidas--;//Restar vida

			if(st_estado_juego.vidas<=0){
				st_estado_juego.final_juego=true;
				st_estado_juego.juego_iniciado=false;
				cnt_render_pantalla=1;
			}
		}
	}
}

void detectar_colisiones(){
	colision_enemigo_jugador();
	colision_disparo_jugador();
	colision_disparo_enemigo();
}

void crear_Tareas_enemigos(){
	char nombreTarea[10];
	for(int i=0;i<MAX_ENEMIGOS;i++){
		snprintf(nombreTarea,sizeof(nombreTarea),"Enemigo_%d",i);
		xTaskCreate(tarea_enemigos, nombreTarea, 2048, (void *)i, 2, NULL);
	}
}

void tarea_entrada(void *pvParameters){
	int pinPresionado;
	int64_t last_time =0;
	while(1){
		if(xQueueReceive(colaBotones, &pinPresionado, portMAX_DELAY)){
			int64_t timeStamp_us= esp_timer_get_time();
			if ((timeStamp_us -last_time)>= DEBOUNCE_US){
				last_time=timeStamp_us;
				if (pinPresionado ==PIN_BTN_TERMINAR_JUEGO){
					st_estado_juego.final_juego=true;
					st_estado_juego.jugador_quit=true;
					st_estado_juego.juego_iniciado=false;
					cnt_render_pantalla=1;
				}
				else{
					if(!st_estado_juego.juego_iniciado){
						memset(&st_estado_juego.enemigos,0,sizeof(st_estado_juego.enemigos));
						memset(&st_estado_juego.disparos_jugador,0,sizeof(st_estado_juego.disparos_jugador));
						memset(&st_estado_juego.disparos_enemigo,0,sizeof(st_estado_juego.disparos_enemigo));
						st_estado_juego.nave.x= ANCHO_TABLERO / 2;
						st_estado_juego.vidas=VIDAS;
						st_estado_juego.score=0;
						st_estado_juego.segs_transc=0;

						st_estado_juego.juego_iniciado=true;
						st_estado_juego.jugador_quit=false;
						st_estado_juego.final_juego=false;
					}

					if(pinPresionado == PIN_BOTON_IZQ){
					st_estado_juego.nave.x=LIMITAR(st_estado_juego.nave.x-1,0,ANCHO_TABLERO-1);
					}
					else if (pinPresionado ==PIN_BOTON_DER){
						st_estado_juego.nave.x=LIMITAR(st_estado_juego.nave.x+1,0,ANCHO_TABLERO-1);
					}
					else if (pinPresionado==PIN_BOTON_DISPARO){
						for(int i=0;i<MAX_DISPAROS_PLAYER;i++){
							if(!st_estado_juego.disparos_jugador[i].activo){
								st_estado_juego.disparos_jugador[i]=(entidad_t){true,st_estado_juego.nave.x,ALTO_TABLERO-2};
								break;
							}
						}
					}
				}
			}
		}
	}
}

void tarea_juego(void *pvParameters){
	int contador_ms=0;
	while(1){
		if(xSemaphoreTake(st_estado_juego_mutex, portMAX_DELAY)== pdTRUE){
			if(st_estado_juego.juego_iniciado){
				//Actualizar pos de disparo de jugador y enemigo
				mover_disparos(st_estado_juego.disparos_jugador,cnt_elementos(st_estado_juego.disparos_jugador),-1);
				mover_disparos(st_estado_juego.disparos_enemigo,cnt_elementos(st_estado_juego.disparos_enemigo),1);
				detectar_colisiones();
				elegir_enemigo_disparo();
				contador_ms+=500;
				if(contador_ms >= 1000){
					st_estado_juego.segs_transc++;
					contador_ms=0;
				}
			}
			xSemaphoreGive(st_estado_juego_mutex);
		}
		vTaskDelay(pdMS_TO_TICKS(500)); //refrescar cada 500ms 
	}
}

void tarea_enemigos(void *pvParameters) {
	int task_id = (int)pvParameters;
	while (1) {
		if(st_estado_juego.juego_iniciado){
			generar_posicion_random(&posX, &posY);
			//posX=7+(task_id*7); posY=ALTO_TABLERO-1;//Pos estaticas para pruebas
			if (xSemaphoreTake(st_estado_juego_countSem, pdMS_TO_TICKS(1000)) == pdTRUE) {
				st_estado_juego.enemigos[task_id] = (entidad_t){true,posX,posY};
				xSemaphoreGive(st_estado_juego_countSem);
			}
		} 
		vTaskDelay(pdMS_TO_TICKS(500)); // Esperar antes de intentarlo otra vez
	}
}

void tarea_pantalla(void *pvParameters){
	while(1){
		if(xSemaphoreTake(st_estado_juego_mutex, portMAX_DELAY)== pdTRUE){
			estado_juego_t estado_local = st_estado_juego; //copia el estado 'local'
			xSemaphoreGive(st_estado_juego_mutex);

			if(!estado_local.juego_iniciado){
				if (cnt_render_pantalla){
					render_pantalla(&estado_local);
					cnt_render_pantalla=0;
				}
				
			}
			else{
				render_pantalla(&estado_local);
				cnt_render_pantalla=1;
			}

			if(estado_local.final_juego){
				if (cnt_render_pantalla){
					st_estado_juego.ultima_puntuacion=st_estado_juego.score;
					if(st_estado_juego.ultima_puntuacion>st_estado_juego.max_puntuacion){
						st_estado_juego.max_puntuacion = st_estado_juego.ultima_puntuacion;
					}
					render_pantalla(&estado_local);
					cnt_render_pantalla=0;
				}
			}
		}
		vTaskDelay(pdMS_TO_TICKS(500)); //refrescar cada 500ms 
	}
}

static void render_pantalla(const estado_juego_t *estado){
	static char grid[ALTO_TABLERO][ANCHO_TABLERO];
	for (int y=0; y < ALTO_TABLERO; y++){
		memset(grid[y], ' ', ANCHO_TABLERO);
	}

	pintar_entidades(estado->enemigos,MAX_ENEMIGOS, grid, caracter_enemigo);
	grid[ALTO_TABLERO-1][estado->nave.x]='^'; //Pintar JUGADOR -- Nave
	//disparos del jugador y enemigos
	pintar_entidades(estado->disparos_jugador,cnt_elementos(estado->disparos_jugador),grid,disparo_jugador);
	pintar_entidades(estado->disparos_enemigo,cnt_elementos(estado->disparos_enemigo),grid,disparo_enemigo);

	//formato de la pantalla 
	printf("\033[2J\033[H");
	printf("=================================================\n");
	printf("    		Espace Defender\n");
	printf("=================================================\n");
	printf("Puntuación: %-4d    Vidas: %d\n", estado->score, estado->vidas);
	//superior
	putchar('+');
	for (int x=0; x<ANCHO_TABLERO; x++) {
		putchar('-');
	}
	printf("+\n");

	//filas del tablero 
	for (int y=0; y<ALTO_TABLERO; y++){
		putchar('|');
		for (int x=0; x<ANCHO_TABLERO; x++){
				putchar(grid[y][x]);
		}
		printf("|\n");
	}
	//inferior
	putchar('+');
	for (int x=0; x<ANCHO_TABLERO; x++) {
			putchar('-');
	}
	printf("+\n");

	printf("Tiempo: %lus\n", estado->segs_transc);

	if (estado->final_juego){
		if(estado->jugador_quit){
				printf("\n Hasta lueguin\n");
		}
		else{
				printf("\nSuerte para la proxima\n");
		}
		printf("-- Puntuacion Obtenida: %d\n", estado->ultima_puntuacion);
		printf("-- Record: %d\n", estado->max_puntuacion);
	}
	printf("\n");
}

void tarea_leds (void *pvParameters){
	while(1){
		unsigned long segs= 0;

		if (xSemaphoreTake(st_estado_juego_mutex, portMAX_DELAY)== pdTRUE){
				segs = st_estado_juego.segs_transc; //se extrae el valor de segundos transcurridos
				xSemaphoreGive(st_estado_juego_mutex);
		}

		//recorrer los bits a la derecha y una mascara de AND 1
		//¿esta prendido el bit numero N, de este numero?
		gpio_set_level(PIN_LED0, (segs>>0) & 1);
		gpio_set_level(PIN_LED1, (segs>>1) & 1);
		gpio_set_level(PIN_LED2, (segs>>2) & 1);
		gpio_set_level(PIN_LED3, (segs>>3) & 1);
		gpio_set_level(PIN_LED4, (segs>>4) & 1);
		
		vTaskDelay(pdMS_TO_TICKS(200)); //refresca 5 veces por segundo 
	}
}