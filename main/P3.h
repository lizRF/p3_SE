#ifndef PRACTICA3_H
#define PRACTICA3_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "MyUART.h"
#define cnt_elementos(arr) (sizeof(arr)) / sizeof((arr)[0])
#define DEBOUNCE_US 150000
#define state       1


#define rol_emisor      1
#define rol_receptor    2

#define SF  '#'
#define EF  '$'


typedef struct{
    bool formato_ok;
    char mensaje[MAX_CHAR+1];
    uint8_t checksum;
} trama_t;


extern QueueHandle_t cola_mensajes, cola_tramas;
extern SemaphoreHandle_t listo_binSem;

void configurar_gpios(const gpio_num_t *pines, int cnt);
void establecer_rol(int rol);

#endif