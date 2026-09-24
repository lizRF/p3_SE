#ifndef PRACTICA3_H
#define PRACTICA3_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

char uart_getchar(uart_port_t uart_num);
void uart_gets(uart_port_t uart_num, char * str, uint8_t max_len);
void uart_getNum(uart_port_t uart_num, char * str, uint8_t max_len);
void uart_getAlpha(uart_port_t uart_num, char * str, uint8_t max_len);
void uart_putchar(uart_port_t uart_num, char c);
void puts(uart_port_t uart_num, char * str);

#endif 