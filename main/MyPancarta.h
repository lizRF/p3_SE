#ifndef MYPANCARTA_H
#define MYPANCARTA_H

#include <stdint.h>
#include "driver/uart.h"

#define FONT_ROWS 7
#define FONT_COLS 5

void imprimir_pancarta(uart_port_t uart_num, const char *texto);

#endif