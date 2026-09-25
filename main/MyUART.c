#include "MyUART.h"

void uart_init(uart_port_t numUart, int tx_pin, int rx_pin){
	uart_config_t uart_config={
		.baud_rate=115200,
		.data_bits=UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_DEFAULT,
	};
	//Instalar el driver de UART
	ESP_ERROR_CHECK(uart_driver_install(numUart, BUFFER_SIZE-1, BUFFER_SIZE-1,0,NULL,0));

	//Configuración de parametros de uart
	ESP_ERROR_CHECK(uart_param_config(numUart, &uart_config));

	//Setear pines del uart
	ESP_ERROR_CHECK(uart_set_pin(numUart, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));	
}


char uart_getchar(uart_port_t uart_num){
	char c;
	while(uart_read_bytes(uart_num, &c, 1, portMAX_DELAY)!=1){
		//Repetir hasta obtener 1 byte, la función read , regresa cuantos se leyeron
	}
	return c;
}

void uart_getNum(uart_port_t uart_num, char * str, uint8_t max_len){
	char c;
	int8_t i =0;
	while(i < (MAX_CHAR-1)){
		c = uart_getchar(UART);
		//Enter, terminar la cadena
		if(c=='\r' || c== '\n'){
			break;
		}
		//Back space
		if(c=='\b' || c==127){
			if(i>0){
				i--;
				str[i]='\0';
				uart_puts(uart_num,"\b \b"); //Borrar de la terminal
			}
			continue;
		}
		//Validar caracteres permitidos
		if(	(c >= '0' && c<= '9')){
				str[i] =c;
				i++;
				uart_putchar(UART,c); //Mostrar el caracter
			}
	}
	str[i]='\0';
	uart_puts(UART, "\r\n"); //Salto de linea despues del enter
}

void uart_getAlpha(uart_port_t uart_num, char * str, uint8_t max_len){
	char c;
	int8_t i =0;
	while(i < (MAX_CHAR-1)){
		c = uart_getchar(UART);
		//Enter, terminar la cadena
		if(c=='\r' || c== '\n'){
			break;
		}
		//Back space
		if(c=='\b' || c==127){
			if(i>0){
				i--;
				str[i]='\0';
				uart_puts(uart_num,"\b \b"); //Borrar de la terminal
			}
			continue;
		}
		//Validar caracteres permitidos
		if(	(c >= 'A' && c<= 'Z') || (c >= 'a' && c<= 'z')){
				str[i] =c;
				i++;
				uart_putchar(UART,c); //Mostrar el caracter
			}
	}
	str[i]='\0';
	uart_puts(UART, "\r\n"); //Salto de linea despues del enter
}

void uart_gets(uart_port_t uart_num, char * str, uint8_t max_len){
	char c;
	int8_t i =0;
	while(i < (MAX_CHAR-1)){
		c = uart_getchar(UART);
		//Enter, terminar la cadena
		if(c=='\r' || c== '\n'){
			break;
		}

		//Back space
		if(c=='\b' || c==127){
			if(i>0){
				i--;
				str[i]='\0';
				uart_puts(uart_num,"\b \b"); //Borrar de la terminal
			}
			continue;
		}

		//Validar caracteres permitidos
		if(	(c >= 'A' && c<= 'Z') ||
		   	(c >= 'a' && c<= 'z') ||
			(c >= '0' && c<= '9') ||
			 c == ' ' || c == '!' || c == '.' || c == '-' || c == '+'){
				str[i] =c;
				i++;
				uart_putchar(UART,c); //Mostrar el caracter
			}
	}
	str[i]='\0';
	uart_puts(UART, "\r\n"); //Salto de linea despues del enter
}

void uart_putchar(uart_port_t uart_num, char c){
	uart_write_bytes(uart_num, &c, 1);
}

void uart_puts(uart_port_t uart_num, char * str){
	uint8_t i =0;
	while(str[i] != '\0'){
		uart_putchar(UART, str[i]);
		i++;
	}
}