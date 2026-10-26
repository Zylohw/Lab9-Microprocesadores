#include "RTE_Components.h"
#include "stm32g431xx.h"
#include <stdint.h>
#include CMSIS_device_header
#include "lab9.h"


volatile uint16_t turno = 0;
volatile uint16_t pot1 = 0;
volatile uint16_t pot2 = 0;

int main() {
    
    gpio_init();
    adc_init();
    tim3_init();      
    for (;;) {
        // verificando el ambiente
        if(pot2 < LED_OSCURO){
            TIM3->CCR3 = pot1 ;
        }else{
            TIM3->CCR3 = 0;
        }


        // PA8
        if (pot1>=819){
            GPIOA->ODR |=(1<<8);
        }else{
            GPIOA->ODR &=~(1<<8);
        }
        
        // PB6
        if (pot1>=1638){
            GPIOB->ODR |=(1<<6);
        }else{
            GPIOB->ODR &=~(1<<6);
        }

        // PB5
        if (pot1>=2457){
            GPIOB->ODR |=(1<<5);
        }else{
            GPIOB->ODR &=~(1<<5);
        }

        // PB12
        if (pot1>=3276){
            GPIOA->ODR |=(1<<12);
        }else{
            GPIOA->ODR &=~(1<<12);
        }
    }
    

}


void ADC1_2_IRQHandler(){
    
    uint16_t data = ADC1->DR;
    if(turno == 0){
        pot1 = data;
        turno = 1; 
    }else{
        pot2 = data;
        turno = 0; 
    }
    
}
