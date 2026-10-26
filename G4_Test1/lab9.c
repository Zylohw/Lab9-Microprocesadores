#include "lab9.h"
#include "stm32g431xx.h"

void gpio_init(){

  // Activando los clocks del GPIO A y B 
  RCC -> AHB2ENR |= (1<<0);
  RCC -> AHB2ENR |= (1<<1);
  // seteando los pines de los leds PA8, PA12, PB5, PB6
  // PA8
  GPIOA -> MODER &= ~(1<<17);
  GPIOA -> MODER |= (1<<16);
  // PA12
  GPIOA -> MODER &= ~(1<<25);
  GPIOA -> MODER |= (1<<24);
  // PB5
  GPIOB -> MODER &= ~(1<<11);
  GPIOB -> MODER |= (1<<10);
  // PB6
  GPIOB -> MODER &= ~(1<<13);
  GPIOB -> MODER |= (1<<12);

}

void adc_init(){
  // configurar el reloj para suministrar la señal al reloj ADC
  RCC -> AHB2ENR |= (1<< 13);
  // Reloj del GPIOA habilitado
  RCC -> AHB2ENR |= (1<<0);
  // seteando PA0 y PA1 como entrada analogica para los potenciometros 
  //PA0
  GPIOA->MODER |= (1<<0);
  GPIOA->MODER |= (1<<1);
  //PA1
  GPIOA->MODER |= (1<<3);
  GPIOA->MODER |= (1<<2);

  ADC12_COMMON ->CCR |= (1<<17);
  ADC12_COMMON ->CCR |= (1<<16);
  
  ADC1->CR &=~(1<<29);
  ADC1->CR |=(1<<28);

  for (volatile int i = 0; i < 2000; i++);
  ADC1->CR |= ADC_CR_ADCAL;
  while(ADC1->CR & ADC_CR_ADCAL);

  // haciendo el continuo el adc
  // configurando la resolución 
  ADC1->CFGR &=~(1<<4);
  ADC1->CFGR &=~(1<<3);

  // configurando el modo continuo
  ADC1->CFGR |= (1<<13);

  // configurando el overrun que sobre-escrib el dato anterior con el nuvo en lugar de detener el adc 
  ADC1->CFGR |= (1<<12);

  // INDICANDO AL ADC QUE CANALES LEER
  // definiendo cantidad de conversiones
  ADC1->SQR1 &= ~(1<<3);
  ADC1->SQR1 &= ~(1<<2);
  ADC1->SQR1 &= ~(1<<1);
  ADC1->SQR1 |=  (1<<0);

  //definiendo secuencia1
  ADC1->SQR1 &= ~(1<<10);
  ADC1->SQR1 &= ~(1<<9);
  ADC1->SQR1 &= ~(1<<8);
  ADC1->SQR1 &= ~(1<<7);
  ADC1->SQR1 |=  (1<<6);

  // definiendo secuencia 2
  ADC1->SQR1 &= ~(1<<16);
  ADC1->SQR1 &= ~(1<<15);
  ADC1->SQR1 &= ~(1<<14);
  ADC1->SQR1 |= (1<<13);
  ADC1->SQR1 &= ~(1<<12);

  // interrupciones 

  // configurnando para que el ADC avise cuando terminó una conversión
  ADC1->IER |= (1<<2);

  // levantando rutina de interrupción
  NVIC_EnableIRQ(ADC1_2_IRQn);

  ADC1->CR |= (1<<0);
  while(!(ADC1->ISR & ADC_ISR_ADRDY));
  ADC1->CR |= (1<<2);

}


void tim3_init(){
  //habilitar el reloj de de TIM3
  RCC->APB1ENR1 |= (1<<1);
  // setear el PB0 como función alterna
  GPIOB->MODER |= (1<<1);
  GPIOB->MODER &= ~(1<<0);
  //función alterna
  GPIOB->AFR[0] &=~(1<<3);
  GPIOB->AFR[0] &=~(1<<2);
  GPIOB->AFR[0] |=(1<<1);
  GPIOB->AFR[0] &=~(1<<0);

  TIM3->PSC = 0;
  TIM3->ARR = 4095;


  //configurando CC3S canal funcionando como output
  TIM3->CCMR2 &=~(1<<1);
  TIM3->CCMR2 &=~(1<<0);
  //la salida cambio CCR3 el cambio se aplica en un nuevo periodo
  TIM3->CCMR2 |= (1<<3);

  TIM3->CCMR2 &= ~(1<<16);
  TIM3->CCMR2 |= (1<<6);
  TIM3->CCMR2 |= (1<<5);
  TIM3->CCMR2 &= ~(1<<4);

  TIM3->CCER |= (1<<8);
  TIM3->CCER &= ~(1<<9);


  // arrancando el timer 
  TIM3->CCR3 = 0;
  TIM3->CR1 |= (1<<7);
  TIM3->EGR |= (1<<0);
  TIM3->CR1 |= (1<<0);

}