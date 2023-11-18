#include "Uart.h"
#include "Gpio.h"

extern uint32_t timer_value;
extern uint8_t led_state;


extern char c;
extern int is_char;

int main() {
	
	UART0_Init(9600);
	OutputPIN_Init();
	while(1) 
	{
		if(is_char)
		{
			UART0_Transmit(c);
			is_char = 0;
		}
	}
	return 0;
}