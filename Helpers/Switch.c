/*
 * Switch.c
 *
 *  Created on: 2025. 6. 5.
 *      Author: devkim
 */

#include "Switch.h"

void Switch_Init(void) {
    // Setup  Switch as GPIO
    P1->SEL0 &= ~0x12;
    P1->SEL1 &= ~0x12;

    // Setup Switch as Input
    P1->DIR &= ~0x12;

    // Set-up pull-up registers
    P1->REN |= 0x12;
    P1->OUT |= 0x12;
}

int Read_Switch_Right(void) {
    return (P1->IN & 0x02);
}

int Read_Switch_Left(void) {
    return (P1->IN & 0x10);
}
