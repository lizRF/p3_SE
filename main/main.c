#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "esp_random.h"
#include "esp_log.h"

#include "MyUART.h"
#include "P3.h"

QueueHandle_t cola_mensajes,cola_tramas;
SemaphoreHandle_t listo_binSem;

void app_main(void){
	mensaje_t buffer;
	uart_init(UART, UART_TX_PIN, UART_RX_PIN);
	//establecer_rol(rol_emisor);
	uart_puts(UART, "\nIngresa un caracter: ");
	uart_gets(UART, buffer.texto, MAX_CHAR);
	uart_puts(UART, "Recibi esto: ");
	uart_puts(UART, buffer.texto);
}



static void rx_mensaje_task(){
	
}

static void tx_trama_task(){
	
}

static void rx_trama_task(){
	
}

static void tx_pancarta_task(){
	
}


