#ifndef UARTP3_H
#define UARTP3_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define DEBOUNCE_US 150000
#define state       1
#define MAX_CHAR    25

#define UART            UART_NUM_0
#define BUFFER_SIZE     1024
#define UART_TX_PIN     1
#define UART_RX_PIN     3


typedef struct{
    char texto[MAX_CHAR+1]; 
} mensaje_t;


void uart_init(uart_port_t num, int tx_pin, int rx_pin);

char uart_getchar(uart_port_t uart_num);
void uart_getNum(uart_port_t uart_num, char * str, uint8_t max_len);
void uart_getAlpha(uart_port_t uart_num, char * str, uint8_t max_len);
void uart_gets(uart_port_t uart_num, char * str, uint8_t max_len);
void uart_putchar(uart_port_t uart_num, char c);
void uart_puts(uart_port_t uart_num, char * str);

#endif 