#ifndef PRACTICA3_H
#define PRACTICA3_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

//macro para que los valorecs no se salgan del rango mínimo y máximo. 
#define LIMITAR(valor, min, max) ((valor)< (min) ? (min): ((valor)> (max) ? (max) : (valor)))
#define cnt_elementos(arr) (sizeof(arr)) / sizeof((arr)[0])

#define PIN_BOTON_IZQ GPIO_NUM_14
#define PIN_BOTON_DER GPIO_NUM_13
#define PIN_BOTON_DISPARO GPIO_NUM_26
#define PIN_BTN_TERMINAR_JUEGO GPIO_NUM_25

//Leds para representaciones de tiempo
#define PIN_LED0			GPIO_NUM_19
#define PIN_LED1			GPIO_NUM_18
#define PIN_LED2			GPIO_NUM_4
#define PIN_LED3			GPIO_NUM_22
#define PIN_LED4			GPIO_NUM_23

#define DEBOUNCE_US 150000
#define state 1

#define ALTO_TABLERO 11
#define ANCHO_TABLERO 46

#define MAX_ENEMIGOS 5
#define MAX_DISPAROS_ENEMIGO 5
#define MAX_DISPAROS_PLAYER 5
#define VIDAS 3

//Datos del enemigo, posición (disparos) y actividad
typedef struct{
    bool activo;
    int x;
    int y;
}entidad_t;

typedef struct{
    int x;
}jugador_t;

typedef struct{
    jugador_t nave;
    entidad_t enemigos[MAX_ENEMIGOS];
    entidad_t disparos_jugador[MAX_DISPAROS_PLAYER]; //SUBEN
    entidad_t disparos_enemigo [MAX_DISPAROS_ENEMIGO]; //BAJAN

    int score;
    int vidas;
    unsigned long segs_transc;

    bool juego_iniciado;
    bool final_juego;
    bool jugador_quit;
    int ultima_puntuacion;
    int max_puntuacion;
}estado_juego_t;

//estado global del juego se comparte
extern estado_juego_t st_estado_juego;
extern SemaphoreHandle_t st_estado_juego_mutex;
extern SemaphoreHandle_t st_estado_juego_countSem;

void configurar_btns(const gpio_num_t *pines, int cnt);
void configurar_leds(const gpio_num_t *pines, int cnt);
void generar_posicion_random(int *x, int *y);
void crear_Tareas_enemigos();
void pintar_entidades(const entidad_t entidades[], int cnt, char grid[ALTO_TABLERO][ANCHO_TABLERO],char caracter);
void mover_disparos(entidad_t disparos[],int cnt,int direccion);
void elegir_enemigo_disparo();
void colision_enemigo_jugador();
void colision_disparo_enemigo();
void colision_disparo_jugador();
void detectar_colisiones();
//Detecta los botones y genera eventos de movimiento o disparo
void tarea_entrada(void *pvParameters); 
void tarea_juego(void *pvParameters);
void tarea_enemigos(void *pvParameters);
void tarea_pantalla(void *pvParameters);
void tarea_leds (void *pvParameters);
#endif 