// Ivan Jimenez Pineda
// ijimenezpineda@g.hmc.edu
// October 5, 2026
// main.c measures motor revolutions/sec and determines spin direction of a quadrature encoder using interrupts.

#include "STM32L432KC.h"
#include "encoder.h"
#include <stdio.h>

#define DELAY_TIM  TIM2   // Use TIM2 clock
#define PPR 408           // Pulses per revolution

// Function Prototypes
void setup(void);
void compute_velocity(void);
int _write(int file, char *ptr, int len);

// Setup clock, pins, and encoder
void setup(void) {
    configureFlash();
    configureClock(); // 80 MHz system clock

    // Setup TIM2 for delay
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;   
    initTIM(DELAY_TIM);                     

    gpioEnable(GPIO_PORT_A);

    // Initialize the encoder driver
    init_encoder();
}

// Computes and prints velocity based on data from encoder.c
void compute_velocity(void) {
    // Retrieve data using drivers from encoder.c
    int32_t current_count = reset_counter();
    int current_dir = return_direction();

    if (current_count == 0) {
         printf("RPS: 0.000 rev/s \n");
    } else {
        float velocity = ((float)current_count) / (PPR * 4);
        float signed_velocity = (current_dir == 0) ? velocity : -velocity;
        printf("RPS: %.3f rev/s\n", signed_velocity);
    }
}

// Function used by printf to show print statements in the debug terminal
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        ITM_SendChar((*ptr++));
    }
    return len;
}

// Main
int main(void) {
    setup();
    printf("Starting\n");

    while (1) {
        compute_velocity();
        delay_millis(DELAY_TIM, 1000);
    }
}