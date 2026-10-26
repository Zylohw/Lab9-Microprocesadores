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


  // cambiando el clock del ADC a 4MHz para que convierta mas lento 
  ADC12_COMMON ->CCR |= (1<<17);
  ADC12_COMMON ->CCR |= (1<<16);
  
  // nos piden desactivar el DEEPPWD para activar el ADC por que lo vamos a usar y ADVGREGEN para activar el regulador
  ADC1->CR &=~(1<<29);
  ADC1->CR |=(1<<28);


  for (volatile int i = 0; i < 2000; i++);// permite estabilizar el regulador 20 micro segundos
  ADC1->CR |= ADC_CR_ADCAL; // le indicamos al ADC que debe calibrarse 
  while(ADC1->CR & ADC_CR_ADCAL); // este while nos permite indicar cuando termino de calibrarse revisa en bit 31 que es el indicador si esta en 1 sigue calibrandose si no entonces termino lac condición da falsa y sigue el programa

  // se esta configurando al resolución del ADC para este caso tenemos una resolución de 12 bit
  ADC1->CFGR &=~(1<<4);
  ADC1->CFGR &=~(1<<3);

  // configurando el modo continuo al poner en 1 el registro 13 CONT
  ADC1->CFGR |= (1<<13);

  // Activamos el OVRMOD en 1 para que cuando llegue un dato nuevo y no ha terminado de leer el anterior entonces  sobre-escriba el dato viejo con el nuevo 
  ADC1->CFGR |= (1<<12);

  // INDICANDO AL ADC QUE CANALES LEER
  // definiendo cantidad de conversiones en el canal regular
  ADC1->SQR1 &= ~(1<<3);
  ADC1->SQR1 &= ~(1<<2);
  ADC1->SQR1 &= ~(1<<1);
  ADC1->SQR1 |=  (1<<0);

  // para el potenciomaetro 1 definiendo canal 1 de lectura
  ADC1->SQR1 &= ~(1<<10);
  ADC1->SQR1 &= ~(1<<9);
  ADC1->SQR1 &= ~(1<<8);
  ADC1->SQR1 &= ~(1<<7);
  ADC1->SQR1 |=  (1<<6);

  //para el potenciometro 2 definiendo canal 2 de lectura
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

  // habilitando el control del ADC
  ADC1->CR |= (1<<0);
  
  // verifica si el ADC esta listo
  while(!(ADC1->ISR & ADC_ISR_ADRDY));
  // inicia una nueva conversión
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

  // definiendo Prescalado de 0 dividiendo en 0 todo y me permite ir junto al timer de la tarjeta
  TIM3->PSC = 0;
  // definiendo un auto reload de 4095 el tope de hasta donde cuenta el contador
  TIM3->ARR = 4095;


  //configurando CC3S canal funcionando como output
  TIM3->CCMR2 &=~(1<<1);
  TIM3->CCMR2 &=~(1<<0);

  TIM3->CCMR2 |= (1<<3); // para que la señal no de brinco

  // pone el canal en 0110 quiere decir que pone el canal 3 en PWM modo 1 donde la salida en alto mientras CNT < CCR3 y en bajo el resto del periodo
  TIM3->CCMR2 &= ~(1<<16);
  TIM3->CCMR2 |= (1<<6);
  TIM3->CCMR2 |= (1<<5);
  TIM3->CCMR2 &= ~(1<<4);

  TIM3->CCER |= (1<<8); //conecta la señal del canal 3 al pin PB0 Sin esto el PWM se genera solo por dentro
  TIM3->CCER &= ~(1<<9); //polaridad normal: “activo” es voltaje alto, o sea, LED encendido.



  // arrancando el timer 
  TIM3->CCR3 = 0; // seteamos que el led arranque en 0
  TIM3->CR1 |= (1<<7); // para configurar que los cambios de periodo se aplican al terminar el periodo actual
  TIM3->EGR |= (1<<0); //fuerza un “update” para que PSC y ARR se carguen de inmediato sin esperar al primer periodo
  TIM3->CR1 |= (1<<0); //enciende el contador. Desde aquí el timer empieza a generar el PWM

}