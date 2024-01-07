#include "Pit.h"
#include "Uart.h"

#define LED_PIN (12) // PORT A
uint32_t timer_value;
char order_leds = '0'; // 0 inseamna ca ordinea e normala : verde(0), albastru(1), magenta(2), negru(3); si led_state = 3
												// 1 inseamna ca ordinea e inversa : negru(0), magenta(1), albastru(2), verde(3); si led_state = 3, 0, ce vreau eu
char is_changed = '0';
uint8_t led_state = 3; 

#define RED_LED_PIN (18) // PORT B
#define GREEN_LED_PIN (19) // PORT B
#define BLUE_LED_PIN (1) // PORT D

char colors[] = "VAMN";
int order = 1;

void PIT_Init(void) {
	
	// Activarea semnalului de ceas pentru perifericul PIT
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;  // SIM->SCGC6 e registrul care contine bitul special pt PIT
	// Utilizarea semnalului de ceas pentru tabloul de timere
	PIT_MCR &= ~PIT_MCR_MDIS_MASK;
	// Oprirea decrementarii valorilor numaratoarelor in modul debug
	PIT->MCR |= PIT_MCR_FRZ_MASK;  // FRZ e pt debug, il pun pe 1 => timere sunt oprite cand depanam
	// Setarea valoarii numaratorului de pe canalul 0 la o perioada de 1 secunda, vedem mai jos de unde am scos valoarea 0x9FFFFF
	PIT->CHANNEL[0].LDVAL = 0x00D7FFFF;  // valoarea calculata de noi pe foaie; e poza in documentul proiectului
	
  // Activarea întreruperilor pe canalul 0
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	// Activarea timerului de pe canalul 0
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	// Activarea întreruperii mascabile si setarea prioritatiis
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_SetPriority(PIT_IRQn,5);
	NVIC_EnableIRQ(PIT_IRQn);
}

// afisare in terminal
uint8_t UTILS_PrintTimer(uint32_t value){
	uint8_t minutes = value/60;
	uint8_t seconds = value%60;
	
	UART0_Transmit(0x0D);
	UART0_Transmit(minutes/10 + 0x30);
	UART0_Transmit(minutes%10 + 0x30);
	UART0_Transmit(0x3A);
	UART0_Transmit(seconds/10 + 0x30);
	UART0_Transmit(seconds%10 + 0x30);
	
	if(timer_value == 99*60+59){
		timer_value = 0;
	}
}

void PIT_IRQHandler(void) {
	
	if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		//UTILS_PrintTimer(timer_value);
		timer_value++;
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		
		if(is_changed == '1') // ma ocup sa schimb ordinea
		{
			is_changed = '0'; // restartez starea de change; adica o fac ca la inceput
			
			if(order_leds == '0')
				order_leds = '1';
			else
				order_leds = '0';
		}
		
		if(order_leds == '0')
		{
			led_state = (led_state+1) % 4;
		}
		else if(order_leds == '1')
		{
			if((led_state - 1) < 0)
				led_state = 3;
			else
				led_state = (led_state - 1) % 4;
		}
		
		
		
		if(led_state == 0) // daca ledul e verde
			{
				GPIOB->PSOR |= (1<<GREEN_LED_PIN); // string ledul verde
				GPIOB->PSOR |= (1<<RED_LED_PIN); // string ledul rosu
				GPIOD->PSOR |= (1<<BLUE_LED_PIN); // sting ledul albastru
				
				GPIOB->PCOR |= (1<<GREEN_LED_PIN); // pornesc ledul verde
			}
		
		if(led_state == 1) // daca ledul e albastru
			{
				GPIOB->PSOR |= (1<<GREEN_LED_PIN); // string ledul verde
				GPIOB->PSOR |= (1<<RED_LED_PIN); // string ledul rosu
				GPIOD->PSOR |= (1<<BLUE_LED_PIN); // sting ledul albastru
				
				GPIOD->PCOR |= (1<<BLUE_LED_PIN); // pornesc doar ledul albastru
			}
	
		if(led_state == 2){ // daca ledul e magenta
				GPIOB->PSOR |= (1<<GREEN_LED_PIN); // string ledul verde
				GPIOB->PSOR |= (1<<RED_LED_PIN); // string ledul rosu
				GPIOD->PSOR |= (1<<BLUE_LED_PIN); // sting ledul albastru
			
				GPIOB->PCOR |= (1<<RED_LED_PIN); // pornesc ledul rosu
				GPIOD->PCOR |= (1<<BLUE_LED_PIN); // pornesc ledul albastru
			}
			
		if(led_state == 3){ // daca ledul e negru, adica toate ledurile sunt stinse
				GPIOB->PSOR |= (1<<GREEN_LED_PIN); // string ledul verde
				GPIOB->PSOR |= (1<<RED_LED_PIN); // string ledul rosu
				GPIOD->PSOR |= (1<<BLUE_LED_PIN); // sting ledul albastru
			}
		
		
	}
}