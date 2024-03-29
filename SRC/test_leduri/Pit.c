#include "Pit.h"
#include "Uart.h"

char order_leds = '0'; /* 0 inseamna ca ordinea e normala : verde(0), albastru(1), magenta(2), negru(3); si led_state = 3*/
												/* 1 inseamna ca ordinea e inversa : negru(0), magenta(1), albastru(2), verde(3); si led_state = 3, 0, ce vreau eu*/
char is_changed = '0';
uint8_t led_state = 3; 

#define RED_LED_PIN (18) /* PORT B */
#define GREEN_LED_PIN (19) /* PORT B*/
#define BLUE_LED_PIN (1) /* PORT D*/

void PIT_Init(void) {
	
	/* Activarea semnalului de ceas pentru perifericul */
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;  /* SIM->SCGC6 e registrul care contine bitul special pt PIT*/
	/* Utilizarea semnalului de ceas pentru tabloul de timere*/
	PIT_MCR &= ~PIT_MCR_MDIS_MASK;
	/* Oprirea decrementarii valorilor numaratoarelor in modul debug*/
	PIT->MCR |= PIT_MCR_FRZ_MASK;  /* FRZ e pt debug, il pun pe 1 => timere sunt oprite cand depanam*/
	
	/* Setarea valoarii numaratorului de pe canalul 0 la o perioada de 1350 milisecunde*/
	PIT->CHANNEL[0].LDVAL = 0x00D7FFFF;  /* valoarea calculata de noi pe foaie; e poza in documentul proiectului*/
	
  /* Activarea intreruperilor pe canalul 0*/
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	/* Activarea timerului de pe canalul 0*/
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	
	/* Setarea valoarea numaratorului de pe canalul 1 la o perioada de 0.5 secunde*/
	PIT->CHANNEL[1].LDVAL = 0x4FFFFF;
	
	/* Activara intreruperilor pe canalul 1*/
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;
	
	/* Activarea timerului de pe canalul 1*/
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	
	/* Activarea intreruperii mascabile si setarea prioritatii*/
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_SetPriority(PIT_IRQn,5);
	NVIC_EnableIRQ(PIT_IRQn);
}

void PIT_IRQHandler(void) {
	
	if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {

		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		
		if(is_changed == '1') /* ma ocup sa schimb ordinea*/
		{
			is_changed = '0'; /* restartez starea de change; adica o fac ca la inceput*/
			
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
		
		if(led_state == 0) /* daca ledul e verde*/
			{
				GPIOB->PSOR |= (1<<GREEN_LED_PIN); /* string ledul verde*/
				GPIOB->PSOR |= (1<<RED_LED_PIN); /* string ledul rosu*/
				GPIOD->PSOR |= (1<<BLUE_LED_PIN); /* sting ledul albastru*/
				
				GPIOB->PCOR |= (1<<GREEN_LED_PIN); /* pornesc ledul verde*/
			}
		
		if(led_state == 1) /* daca ledul e albastru*/
			{
				GPIOB->PSOR |= (1<<GREEN_LED_PIN); /* string ledul verde*/
				GPIOB->PSOR |= (1<<RED_LED_PIN); /* string ledul rosu*/
				GPIOD->PSOR |= (1<<BLUE_LED_PIN); /* sting ledul albastru*/
				
				GPIOD->PCOR |= (1<<BLUE_LED_PIN); /* pornesc doar ledul albastru*/
			}
	
		if(led_state == 2) /* daca ledul e magenta*/
		{
				GPIOB->PSOR |= (1<<GREEN_LED_PIN); /* string ledul verde*/
				GPIOB->PSOR |= (1<<RED_LED_PIN); /* string ledul rosu*/
				GPIOD->PSOR |= (1<<BLUE_LED_PIN); /* sting ledul albastru*/
			
				GPIOB->PCOR |= (1<<RED_LED_PIN); /* pornesc ledul rosu*/
				GPIOD->PCOR |= (1<<BLUE_LED_PIN); /* pornesc ledul albastru*/
		}
			
		if(led_state == 3) /* daca ledul e negru, adica toate ledurile sunt stinse*/
		{
				GPIOB->PSOR |= (1<<GREEN_LED_PIN); /* string ledul verde*/
				GPIOB->PSOR |= (1<<RED_LED_PIN); /* string ledul rosu*/
				GPIOD->PSOR |= (1<<BLUE_LED_PIN); /* sting ledul albastru*/
		}
	}
	
	if(PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		
		/* se activeaza intreruperea ADC*/
		ADC0->SC1[0] |= ADC_SC1_AIEN_MASK;
		
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
	}
}
