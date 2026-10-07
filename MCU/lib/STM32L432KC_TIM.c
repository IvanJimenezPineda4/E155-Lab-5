// STM32F401RE_TIM.c
// TIM functions

#include "STM32L432KC_TIM.h"
#include "STM32L432KC_RCC.h"

void initTIM(TIM_TypeDef * TIMx){
  // Divide 80 MHz by 8000 to get a 10 kHz timer clock (0.1 ms per tick)
  // 8000 easily fits inside the 16-bit PSC register
  uint32_t psc_div = 8000; 

  // Set prescaler division factor
  TIMx->PSC = (psc_div - 1);
  
  // Generate an update event to update prescaler value
  TIMx->EGR |= 1;
  
  // Enable counter
  TIMx->CR1 |= 1; // Set CEN = 1
}

void delay_millis(TIM_TypeDef * TIMx, uint32_t ms){
  // Since the timer ticks every 0.1 ms, multiply ms by 10
  TIMx->ARR = ms * 10;
  
  TIMx->EGR |= 1;     // Force update
  TIMx->SR &= ~(0x1); // Clear UIF
  TIMx->CNT = 0;      // Reset count

  while(!(TIMx->SR & 1)); // Wait for UIF to go high
}