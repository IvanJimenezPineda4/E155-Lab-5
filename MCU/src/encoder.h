// Ivan Jimenez Pineda
// ijimenezpineda@g.hmc.edu
// October 5, 2026
// encoder.h is the header for quadrature encoder using EXTI interrupts

#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include "STM32L432KC.h"
#include "stm32l432xx.h"

// Define Macros
#define ENCODER_A  PA8    // Encoder A input
#define ENCODER_B  PA6    // Encoder B input

// Function Prototypes
void init_encoder(void);
int32_t reset_counter(void);
int return_direction(void);

#endif