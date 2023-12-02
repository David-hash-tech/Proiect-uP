#include "Uart.h"
#define LED_PIN12 (12) // PORT A12
#define LED_PIN4 (4) // PORT A4
#define LED_PIN5 (5) // PORT A5

int led_state_pin12 = 0;
int led_state_pin4 = 0;
int led_state_pin5 = 0;

char c; 
char buffer[32];

int write, read;
int print_buffer;
int full;

void UART0_Transmit(uint8_t data)
{
	//Punem in asteptare pana cand registrul de transmisie a datelor nu este gol
	while(!(UART0->S1 & UART0_S1_TDRE_MASK));
				UART0->D = data;
	
}

uint8_t UART0_receive(void)
{
	//Punem in asteptare pana cand registrul de receptie nu este plin
	while(!(UART0->S1 & UART0_S1_RDRF_MASK));
				return UART0->D;
	
}

void UART0_Init(uint32_t baud_rate)
{
	
	//Setarea sursei de ceas pentru modulul UART
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(01);
	
	//Activarea semnalului de ceas pentru modulul UART
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	
	//Activarea semnalului de ceas pentru portul A
	//intrucat dorim sa folosim pinii PTA1, respectiv PTA2 pentru comunicarea UART
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	//Fiecare pin pune la dispozitie mai multe functionalitati 
	//la care avem acces prin intermediul multiplexarii
	PORTA->PCR[1] = ~PORT_PCR_MUX_MASK;
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Configurare RX pentru UART0
	PORTA->PCR[2] = ~PORT_PCR_MUX_MASK;
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Configurare TX pentru UART0
	
	
	
	UART0->C2 &= ~((UART0_C2_RE_MASK) | (UART0_C2_TE_MASK)); 
	
	//Configurare Baud Rate
	//uint32_t osr = 8; // Over-Sampling Rate (numarul de esantioane luate per bit-time)
	
	//SBR - vom retine valoarea baud rate-ului calculat pe baza frecventei ceasului de sistem
	// 	 SBR  -		b16 b15 b14 [b13 b12 b11 b10 b09		b08 b07 b06 b05 b04 b03 b02 b01] &
	// 0x1F00 -		0		0   0    1   1   1   1   1      0   0   0   0   0   0   0   0
	//            0   0   0    b13 b12 b11 b10 b09    0   0   0   0   0   0   0   0 >> 8
	//   BDH  -   0   0   0    b13 b12 b11 b10 b09
	//   BDL  -   b08 b07 b06  b05 b04 b03 b02 b01
	
//  // calculeaza Baud Rate Divisor(sbr) pe baza osr dat
//	uint16_t sbr = (uint16_t)((DEFAULT_SYSTEM_CLOCK)/(baud_rate * (osr+1)));

//  // pune in temp valoare lui UART0_BDH cu ultimii 5 biti pe 0 (care reprezinta sbr) 
//	uint8_t temp = UART0->BDH & ~(UART0_BDH_SBR(0x1F));

//  // seteaza ultimii 5 biti din UART0_BDH cu bitii 9-14 din sbr
//	UART0->BDH = temp | UART0_BDH_SBR(((sbr & 0x1F00)>> 8));

//  // pune in UART0_BDL ultimii 8 biti din sbr
//	UART0->BDL = (uint8_t)(sbr & UART_BDL_SBR_MASK);

//  // seteaza ultimii 5 biti din registrul UART0_C4(care tin de osr) cu osr ales
//	UART0->C4 |= UART0_C4_OSR(osr);



//-------SET OVER SAMPLING RATE TO 8--------
uint32_t osr = 8; // Over-Sampling Rate (numarul de esantioane luate per bit-time)

// calculeaza Baud Rate Divisor(sbr) pe baza osr dat
uint16_t sbr = (uint16_t)((DEFAULT_SYSTEM_CLOCK) / (baud_rate * (osr + 1)));

// extrage bitii high si low din sbr
uint8_t sbr_high = (uint8_t)((sbr & 0xFF00) >> 8);
uint8_t sbr_low = (uint8_t)(sbr & 0xFF);

// seteaza UART0_BDH cu bitii high din sbr
UART0->BDH = UART0_BDH_SBR(sbr_high);

// seteaza UART0_BDL cu bitii low din sbr
UART0->BDL = sbr_low;

// seteaza UART0_C4 cu osr ales
UART0->C4 = (UART0->C4 & ~UART0_C4_OSR_MASK) | UART0_C4_OSR(osr);
//-------END SET OVER SAMPLING RATE TO 8--------

//-------SET RX INVERTED BIT------
//UART0->S2 |= UART0_S2_RXINV_MASK;
//-------END SET RX INVERTED BIT-----


//-------SET DATA FRAME ORDER TO LSB FIRST----------
// seteaza LSB bit in UART0_S2 (putea fi lasata si default, e lsb first by default)
  UART0->S2 &= ~UART0_S2_MSBF_MASK;
//-------END SET DATA FRAME ORDER TO LSB FIRST----------
	
	//Setare numarul de biti de date la 8 si fara bit de paritate
	UART0->C1 = 0;
	
	//Dezactivare intreruperi la transmisie
	UART0->C2 |= UART0_C2_TIE(0);
	UART0->C2 |= UART0_C2_TCIE(0);
	
	//Activare intreruperi la receptie
	UART0->C2 |= UART0_C2_RIE(1);
	UART0->C2 |= ((UART_C2_RE_MASK) | (UART_C2_TE_MASK));
	
	NVIC_EnableIRQ(UART0_IRQn);
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

// handler de intrerupere la uart0
void UART0_IRQHandler(void) 
{
		if(UART0->S1 & UART0_S1_RDRF_MASK) 
		{
			c = UART0->D;
			
			buffer[write] = c;
			write++;
			write = write % 32;
			
			if(c == '0')
			{
				closeLed(LED_PIN4);
				closeLed(LED_PIN5);
				closeLed(LED_PIN12);
			}
			
			if(c > '0' && c < '4')
			{
				openLed(LED_PIN4);
				closeLed(LED_PIN5);
				closeLed(LED_PIN12);
			}
			
			if(c >= '4' && c < '7')
			{	
				openLed(LED_PIN4);
				openLed(LED_PIN5);
				closeLed(LED_PIN12);
			}
			
			if(c >= '7' && c <= '9')
			{	
				openLed(LED_PIN4);
				openLed(LED_PIN5);
				openLed(LED_PIN12);
			}
		}
}
