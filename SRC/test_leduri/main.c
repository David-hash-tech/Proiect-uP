#include "MKL25Z4.h"
#include "Pit.h"
#include "Uart.h"
#include "Gpio.h"

extern char c;
extern char buffer[32];

extern int write, read;


int main()
{
	uint16_t i;
	UART0_Init(9600);
	OutputPIN_Init();
	PIT_Init();
	while (1) 
	{
		if (read != write)	
		{
	
			if (buffer[read] == 0x0D)
			{
				//pt enter
				UART0_Transmit(0x0D); //Carriage Return
				UART0_Transmit(0x0A); //Line Feed
			}
			else 
				UART0_Transmit(buffer[read]);
			
			read++;
			read = read % 32;
		}
				
	}
	
	return 0;
	
}