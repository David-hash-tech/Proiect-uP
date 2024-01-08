#include "Adc.h"
#include "Uart.h"

#define ROTATION_SENSOR (11) // PTC2

#define LED_PIN12 (12) // PORT A12
#define LED_PIN4 (4) // PORT A4
#define LED_PIN5 (5) // PORT A5

int led_state_pin12 = 0;
int led_state_pin4 = 0;
int led_state_pin5 = 0;

void ADC0_Init() {
	
	// Activarea semnalului de ceas pentru modulul periferic ADC
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
	
	// Functia de calibrare luata din reference manual -> pag 494
	ADC0_Calibrate();
	
	ADC0->CFG1 = 0x00; // dupa setarea unor biti mai jos raman pe 0 (Normal Power configuration) si (Short sample time)
	ADC0->CFG1 |= ADC_CFG1_MODE(3) | ADC_CFG1_ADICLK(0) | ADC_CFG1_ADIV(2);
//reg config	| conv pe 16 biti | fol sursa de ceas MCU | imparte sursa de ceas la 4 (pentru a obtine o frecventa <= 4 Mz -> pag 494)
	
	// DIFF = 0 -> selectam conversia sa fie single-ended (pag 464)
	ADC0->SC1[0] = 0x00;
	ADC0->SC3 = 0x00;

	// dorim ca, conversia sa fie continua pentru a putea folosi si intreruperi in acelasi timp
	ADC0->SC3 |= ADC_SC3_ADCO_MASK;
	
	// selecteaza canalul de input ca fiind PTC2 (la care am conectat senzorul)
	ADC0->SC1[0] |= ADC_SC1_ADCH(ROTATION_SENSOR);
	
	
	// Apeleaza o intrerupere dupa ce o conversie completa a fost realizata
	ADC0->SC1[0] |= ADC_SC1_AIEN_MASK;
	
	NVIC_SetPriority(ADC0_IRQn,6); // setam prioritate mai mica decat la modulul de PIT pentru nu a interfera intre ele
	NVIC_EnableIRQ(ADC0_IRQn);	
}


// functia de calibrare descrisa in reference manual -> pag 494
int ADC0_Calibrate() 
{
		
	// The calibration will automatically begin if the SC2[ADTRG] is 0. (pag 495)
	ADC0->SC2 &= ~ADC_SC2_ADTRG_MASK;
	
	// Set hardware averaging to maximum, that is, SC3[AVGE]=1 and SC3[AVGS]=0x11 for an average of 32 (pag 494)
	ADC0->SC3 |= (ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3));
	
	// To initiate calibration, the user sets SC3[CAL] (pag 472)
	ADC0->SC3 |= ADC_SC3_CAL_MASK;
	
	
	// ====== FUNCTIA DE CALIBRARE DESCRISA IN REFERENCE MANUAL (pag 495) =====
	
	// 1. Initialize or clear a 16-bit variable in RAM.
	uint16_t calValue = 0x0000;
	
	// 2. Add the plus-side calibration results CLP0, CLP1, CLP2, CLP3, CLP4, and CLPS to the variable.
	calValue += ADC0->CLP0;
	calValue += ADC0->CLP1;
	calValue += ADC0->CLP2;
	calValue += ADC0->CLP3;
	calValue += ADC0->CLP4;
	calValue += ADC0->CLPS;
	
	// 3. Divide the variable by two.
	calValue /= 2;
	
	// 4. Set the MSB of the variable. 
	calValue |= 0x8000;
	
	// 5. Store the value in the plus-side gain calibration register PG.
	ADC0->PG = ADC_PG_PG(calValue);
	
	// 6. Repeat the procedure for the minus-side gain calibration value.
	calValue = 0x0000;
	
	calValue += ADC0->CLM0;
	calValue += ADC0->CLM1;
	calValue += ADC0->CLM2;
	calValue += ADC0->CLM3;
	calValue += ADC0->CLM4;
	calValue += ADC0->CLMS;
	
	calValue /= 2;
	
	calValue |= 0x8000;
	
	ADC0->MG = ADC_MG_MG(calValue);
	
	// Incheierea calibrarii prin setarea pe 0 a bitului CAL -> pag 472
	ADC0->SC3 &= ~ADC_SC3_CAL_MASK;
	
	return (0);
}

uint16_t getRotationSensorValue()
{
	uint16_t rez = 0;
	int index = 0;
	int i;
	char charValue[16];
	uint16_t analogValue;
	
	//configurare canal senzor de rotatie
	ADC0->SC1[0] = 0x00;
	ADC0->SC3 = 0x00;
	ADC0->SC3 |= ADC_SC3_ADCO_MASK;
	ADC0->SC1[0] |= ADC_SC1_ADCH(ROTATION_SENSOR);
	ADC0->SC1[0] |= ADC_SC1_AIEN_MASK;
	
	//citirea valorii transmise de senzor
	analogValue = (uint16_t)ADC0->R[0];
	
	analogValue = analogValue*300/65535;
	
	rez = analogValue;
	
	//afisarea valorii obtinute
	while(analogValue!=0)
	{
		charValue[index] = analogValue%10 + 0x30;
		analogValue = analogValue/10;
		index = index + 1;
	}
	
	for(i = 0; i < index ; i++)
		UART0_Transmit(charValue[index - i - 1]);

	UART0_Transmit(0x0A);
	UART0_Transmit(0x0D);
	
	return rez;
}

// aprinde led-ul de pe pinul pin
void openLed(int pin)
{
	if(pin == LED_PIN12)
	{
		led_state_pin12 = 1;
		GPIOA->PSOR |= (1<<pin);
	}
	else if(pin == LED_PIN4)
	{	
		led_state_pin4 = 1;
		GPIOA->PSOR |= (1<<pin);
	}
	else
	{	
		led_state_pin5 = 1;
		GPIOA->PSOR |= (1<<pin);
	}
}

// stinge led-ul de pe pinul pin
void closeLed(int pin)
{
	if(pin == LED_PIN12)
	{
		led_state_pin12 = 0;
		GPIOA->PCOR |= (1<<pin);
	}
	else if(pin == LED_PIN4)
	{	
		led_state_pin4 = 0;
		GPIOA->PCOR |= (1<<pin);
	}
	else
	{	
		led_state_pin5 = 0;
		GPIOA->PCOR |= (1<<pin);
	}
}

void ADC0_IRQHandler()
{
	  uint16_t value; 
	  value = getRotationSensorValue();

		ADC0->SC1[0] = 0x00;
	  ADC0->SC1[0] |= ADC_SC1_AIEN_MASK;
	
		if(value == 0)
		{
			closeLed(LED_PIN4);
			closeLed(LED_PIN5);
			closeLed(LED_PIN12);
		}
		
		if(value > 0 && value < 100)
		{
			openLed(LED_PIN4);
			closeLed(LED_PIN5);
			closeLed(LED_PIN12);
		}
		
		if(value >= 100 && value < 200)
		{	
			openLed(LED_PIN4);
			openLed(LED_PIN5);
			closeLed(LED_PIN12);
		}
		
		if(value >= 200 && value <= 300)
		{	
			openLed(LED_PIN4);
			openLed(LED_PIN5);
			openLed(LED_PIN12);
		}
}
