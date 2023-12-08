#include "MKL25Z4.h"
#include "Pit.h"
#include "Uart.h"
#include "Gpio.h"
#include "Adc.h"

int main()
{
	UART0_Init(9600);
	PIT_Init();
	OutputPIN_Init();
	ADC0_Init();
	
	while (1) 
	{		
	}
	
	return 0;
	
}