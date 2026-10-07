// Ivan Jimenez Pineda
// ijimenezpineda@g.hmc.edu
// October 5, 2026
// encoder.c is the driver source for the quadrature encoder

#include "encoder.h"

// Volatile Variables
volatile int32_t counter = 0;    
volatile int direction = 0;      // 0 = Clockwise, 1 = Counter-Clockwise
volatile int lastA = 0;
volatile int lastB = 0;
volatile int newA = 0;
volatile int newB = 0;

// Setup encoder GPIO pins and interrupts
void init_encoder(void) {
    gpioEnable(GPIO_PORT_A);
    pinMode(ENCODER_A, GPIO_INPUT);
    pinMode(ENCODER_B, GPIO_INPUT);
    
    // Initialize lastA/lastB
    lastA = digitalRead(ENCODER_A);
    lastB = digitalRead(ENCODER_B);

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG

    // Map external interrupt (EXTI) lines to Port A (0b000)
    SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI6; 
    SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR2_EXTI6, 0b000);   // EXTI6 connects to PA6
    
    SYSCFG->EXTICR[2] &= ~SYSCFG_EXTICR3_EXTI8;
    SYSCFG->EXTICR[2] |= _VAL2FLD(SYSCFG_EXTICR3_EXTI8, 0b000);   // EXTI8 connects to PA8

    // Unmask EXTI lines and enable both rising/falling edges
    EXTI->IMR1 |= (1 << gpioPinOffset(ENCODER_A)) | (1 << gpioPinOffset(ENCODER_B));
    EXTI->RTSR1 |= (1 << gpioPinOffset(ENCODER_A)) | (1 << gpioPinOffset(ENCODER_B));
    EXTI->FTSR1 |= (1 << gpioPinOffset(ENCODER_A)) | (1 << gpioPinOffset(ENCODER_B));

    NVIC_EnableIRQ(EXTI9_5_IRQn); // Enable EXTI lines 5-9 in Nested Vector Interrupt Controller (NVIC)
    __enable_irq(); 
}

// Interrupt Service Routine (ISR) for encoder edge
// Compare initial values of ENCODER_A and ENCODER_B to values when interrupt is taken.
void EXTI9_5_IRQHandler(void) {
    newA = digitalRead(ENCODER_A);
    newB = digitalRead(ENCODER_B);

    // Determine direction using logic gates
    if (!lastA && !lastB) {
        if (newA && !newB) direction = 0;       // CW        
        else if (!newA && newB) direction = 1;  // CCW
    } 
    else if (!lastA && lastB) {
        if (!newA && !newB) direction = 0;
        else if (newA && newB) direction = 1;
    }
    else if (lastA && !lastB) {
        if (newA && newB) direction = 0;
        else if (!newA && !newB) direction = 1;
    }
    else if (lastA && lastB) {
        if (!newA && newB) direction = 0;
        else if (newA && !newB) direction = 1;
    }

    lastA = newA;
    lastB = newB;
    counter++;

    // clear interrupt flags using direct assignment
    if (EXTI->PR1 & (1 << gpioPinOffset(ENCODER_B))) EXTI->PR1 = (1 << gpioPinOffset(ENCODER_B)); 
    if (EXTI->PR1 & (1 << gpioPinOffset(ENCODER_A))) EXTI->PR1 = (1 << gpioPinOffset(ENCODER_A)); 
}

// retrieves and resets the counter atomically
int32_t reset_counter(void) {
    NVIC_DisableIRQ(EXTI9_5_IRQn);
    int32_t current_count = counter;
    counter = 0;
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    return current_count;
}

// function for current direction
int return_direction(void) {
    return direction;
}