#ifndef PRACTICA3_H
#define PRACTICA3_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define cnt_elementos(arr) (sizeof(arr)) / sizeof((arr)[0])
#define DEBOUNCE_US 150000
#define state       1
#define MAX_CHAR    25

#define UART            UART_NUM_0
#define BUFFER_SIZE     1024
#define UART_TX_PIN     1
#define UART_RX_PIN     3

#define rol_emisor      1
#define rol_receptor    2

#define SF  '#'
#define EF  '$'

typedef struct{
    char texto[MAX_CHAR+1]; 
} mensaje_t;

typedef struct{
    bool formato_ok;
    char mensaje[MAX_CHAR+1];
    uint8_t checksum;
} trama_t;

typedef enum{
    FILTRO_NUM,
    FILTRO_ALPHA,
    FILTRO_TODO
}tipo_filtro_t;

extern QueueHandle_t cola_mensajes, cola_tramas;
extern SemaphoreHandle_t listo_binSem;

void configurar_gpios(const gpio_num_t *pines, int cnt);
void uart_init(uart_port_t num, int tx_pin, int rx_pin);
void establecer_rol(int rol);

char uart_getchar(uart_port_t uart_num);
void uart_getNum(uart_port_t uart_num, char * str, uint8_t max_len);
void uart_getAlpha(uart_port_t uart_num, char * str, uint8_t max_len);
void uart_gets(uart_port_t uart_num, char * str, uint8_t max_len);
void uart_putchar(uart_port_t uart_num, char c);
void uart_puts(uart_port_t uart_num, char * str);

#endif 