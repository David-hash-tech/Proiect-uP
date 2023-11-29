#include "Gpio.h"

#define LED_PIN12 (12) // PORT A12
#define LED_PIN4 (4) // PORT A4
#define LED_PIN5 (5) // PORT A5

#define RED_LED_PIN (18) // PORT B
#define GREEN_LED_PIN (19) // PORT B
#define BLUE_LED_PIN (1) // PORT D

void OutputPIN_Init(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	PORTA->PCR[LED_PIN12] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[LED_PIN12] |= PORT_PCR_MUX(1);
	
	GPIOA->PDDR |= (1<<LED_PIN12);
	GPIOA->PCOR |= (1<<LED_PIN12);
	
	//----
	
	PORTA->PCR[LED_PIN4] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[LED_PIN4] |= PORT_PCR_MUX(1);
	
	GPIOA->PDDR |= (1<<LED_PIN4);
	GPIOA->PCOR |= (1<<LED_PIN4);
	
	//----
	
	PORTA->PCR[LED_PIN5] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[LED_PIN5] |= PORT_PCR_MUX(1);
	
	GPIOA->PDDR |= (1<<LED_PIN5);
	GPIOA->PCOR |= (1<<LED_PIN5);
	
	
	// punem alternativa 1(GPIO) -> nu se mai afiseaza la terminal
	//OBS: DACA DEZACTIVAM PINUL PTA2 NU SE MAI FACE AFISAREA IN TERMINAL 
//	PORTA->PCR[LED2_PIN] &= ~PORT_PCR_MUX_MASK;
//	PORTA->PCR[LED2_PIN] &= PORT_PCR_MUX(0);
	
	// pt ledul de pe placa nu mai folosim pta12, punem una din alea 3 ptb18,..
	// Activarea semnalului de ceas pentru pinii folositi în cadrul led-ului RGB
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
	
	// --- RED LED ---
	
	// Utilizare GPIO ca varianta de multiplexare ; fiecare pin are regirtul sau PCR
	PORTB->PCR[RED_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED_PIN] |= PORT_PCR_MUX(1);
	
	
	// Configurare pin pe post de output
	GPIOB_PDDR |= (1<<RED_LED_PIN);
	
	// Stingerea LED-ului (punerea pe 0 logic)
	GPIOB_PSOR |= (1<<RED_LED_PIN); // DE CE NU MERGE SA STING CU PCOR ??? 
	
	// --- GREEN LED ---
	
	// Utilizare GPIO ca varianta de multiplexare
	PORTB->PCR[GREEN_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED_PIN] |= PORT_PCR_MUX(1);
	
	// Configurare pin pe post de output
	GPIOB_PDDR |= (1<<GREEN_LED_PIN);
	
	// Stingerea LED-ului (punerea pe 0 logic)
	GPIOB_PSOR |= (1<<GREEN_LED_PIN);
	
	// --- BLUE LED ---
	
		// Utilizare GPIO ca varianta de multiplexare
	PORTD->PCR[BLUE_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED_PIN] |= PORT_PCR_MUX(1);
	
	// Configurare pin pe post de output
	GPIOD_PDDR |= (1<<BLUE_LED_PIN);
	
	// Stingerea LED-ului (punerea pe 0 logic)
	GPIOD_PSOR |= (1<<BLUE_LED_PIN);
}


