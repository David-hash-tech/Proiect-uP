#include "Uart.h"

char c; 

extern char order_leds;
extern char is_changed;

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
UART0->S2 |= UART0_S2_RXINV_MASK;
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
	
	NVIC_SetPriority(UART0_IRQn, 1); // setam prioritate mai mica decat la modulul de PIT pentru nu a interfera intre ele
	NVIC_EnableIRQ(UART0_IRQn);
}






 //handler de intrerupere la uart0
void UART0_IRQHandler(void) 
{
		if(UART0->S1 & UART0_S1_RDRF_MASK) 
		{
			c = UART0->D;
			
			if(c == 'g')
			{
				is_changed = '1'; // 'g' imi spune ca s-a schimbat ordinea
			}
		}
}
