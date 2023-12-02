#include "MKL25Z4.h"

void ADC0_Init(void);
int ADC0_Calibrate(void);
void ADC0_IRQHandler(void);
uint16_t getRotationSensorValue(void);
void openLed(int pin);
void closeLed(int pin);
