//// COMENTARIO LIZ 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "esp_random.h"
#include "esp_log.h"

#include "P3.h"
#include "MyUART.h"
#include "MyPancarta.h"

QueueHandle_t cola_mensajes,cola_tramas;
SemaphoreHandle_t listo_binSem, respuesta_binSem;

static void rx_mensaje_task(void *args);
static void tx_trama_task(void *args);
static void rx_trama_task(void *args);
static void tx_pancarta_task(void *args);

void app_main(void){
	uart_init(UART_PC, 1,3);
	uart_init(UART_COM, UART_COM_TX_PIN,UART_COM_RX_PIN);
	establecer_rol(rol_emisor);
	//establecer_rol(rol_receptor);

}

void establecer_rol(int rol){
	if(rol==rol_emisor){
		cola_mensajes = xQueueCreate(1,sizeof(mensaje_t));
		listo_binSem = xSemaphoreCreateBinary();
		respuesta_binSem = xSemaphoreCreateBinary();
		xTaskCreate(rx_mensaje_task, "rx_mensaje_task", 2048, NULL, 4, NULL);
		xTaskCreate(tx_trama_task, "tx_trama_task", 2048, NULL, 5, NULL);
	}
	else{
		cola_tramas = xQueueCreate(2,sizeof(trama_t));
		xTaskCreate(rx_trama_task, "rx_trama_task", 2048, NULL, 4, NULL);
		xTaskCreate(tx_pancarta_task, "tx_pancarta_task", 2048, NULL, 5, NULL);
	}
}

static void rx_mensaje_task(void *args){
	mensaje_t mensaje;
	while(1){
		uart_flush_input(UART_PC);
		uart_puts(UART_PC, "Ingrese mensaje: ");
		uart_gets(UART_PC, mensaje.texto,MAX_CHAR+1);

		//Mandar el mensaje a la cola
		xQueueSend(cola_mensajes, &mensaje, portMAX_DELAY);

		//Avisar si si ya hay un mensaje listo
		xSemaphoreGive(listo_binSem);

		//Esperar a que termine el envío
		xSemaphoreTake(respuesta_binSem, portMAX_DELAY);
	}
}

static void tx_trama_task(void *args){
	mensaje_t mensaje;
	uint8_t checksum;
	uint8_t i;
	char respuesta;

	while(1){
		//Esperar que haya un mensaje
		xSemaphoreTake(listo_binSem, portMAX_DELAY);

		//Obtener mensaje de la cola
		xQueueReceive(cola_mensajes, &mensaje, portMAX_DELAY);

		//Calcular chechsum
		checksum =0;
		for(i=0; mensaje.texto[i] != '\0'; i++){
			checksum ^=mensaje.texto[i];
		}

		//Enviar SF
		uart_putchar(UART_COM,SF);

		//Enviar mensaje
		uart_puts(UART_COM, mensaje.texto);

		//Enviar EF
		uart_putchar(UART_COM, EF);

		//Enviar checksum
		uart_putchar(UART_COM,checksum);

		//Esperar respuesta del ESP receptor
		respuesta = uart_getchar(UART_COM);

		if(respuesta == 'A'){
			uart_puts(UART_PC, "Mensaje enviado correctamente. \r\n");
		}
		else if(respuesta == 'N'){
			uart_puts(UART_PC, "Error al enviar mensaje. \r\n");
		}
		xSemaphoreGive(respuesta_binSem);
	}
}

static void rx_trama_task(void *args){
	trama_t trama;
	char c;
	uint8_t i;

	while(1){
		trama.formato_ok = true;
		i=0;
		//Esperar inicio de trama
		do{
			c=uart_getchar(UART_COM);
		}while(c!=SF);

		//Recibir mensaje hasta EF
		while(1){
			c=uart_getchar(UART_COM);
			if(c==EF){
				break;
			}

			//Verificar límite del mensaje
			if(i>=MAX_CHAR){
				trama.formato_ok=false;

				//Vaciar hasta encontar EF
				while(c!=EF){
					c = uart_getchar(UART_COM);
				}
				break;
			}
			trama.mensaje[i]=c;
			i++;
		}
		trama.mensaje[i]='\0';

		//Recibir checksum
		trama.checksum = uart_getchar(UART_COM);
		//Mandar trama a la cola
		xQueueSend(cola_tramas,&trama,portMAX_DELAY);
	}
}

static void tx_pancarta_task(void *args){
	trama_t trama;
	uint8_t checksum;
	uint8_t i;

	while(1){
		//Esperar una trama
		xQueueReceive(cola_tramas, &trama, portMAX_DELAY);

		//Si el formato es incorrecto
		if(trama.formato_ok==false){
			uart_putchar(UART_COM,'N');
			continue;
		}
		//Calcular checksum
		checksum=0;
		for(i=0;trama.mensaje[i]!='\0';i++){
			checksum ^= trama.mensaje[i];
		}
		//Comparar checksum
		if(checksum != trama.checksum){
			uart_putchar(UART_COM,'N');
			continue;
		}
		//Mensaje correcto
		uart_putchar(UART_COM, 'A');

		imprimir_pancarta(UART_PC, trama.mensaje);
	}
}


