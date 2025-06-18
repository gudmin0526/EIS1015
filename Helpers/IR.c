/*
 * IR.c
 *
 *  Created on: 2025. 6. 4.
 *      Author: devkim
 */

#include "IR.h"

void IR_Init(void) {
    P5->SEL0 &= ~0x0F; // 0, 2, 4, 6
    P5->SEL1 &= ~0x0F; // 1, 3, 5, 7
    P5->DIR |= 0x0F;
    P5->OUT &= ~0x0F;

    P9->SEL0 &= ~0x0F;
    P9->SEL1 &= ~0x0F;
    P9->DIR |= 0x0F;
    P9->OUT &= ~0x0F;

    P7->SEL0 &= ~0xFF;
    P7->SEL1 &= ~0xFF;
    P7->DIR &= ~0xFF;
}

void On_IR_Emitter(uint8_t mask_P5, uint8_t mask_P9) {
    P5->OUT |= mask_P5;
    P9->OUT |= mask_P9;
}

void Off_IR_Emitter(uint8_t mask_P5, uint8_t mask_P9) {
    P5->OUT &= ~mask_P5;
    P9->OUT &= ~mask_P9;
}

void Read_IR_Sensor(uint8_t sensors[8]) {
    On_IR_Emitter(0x0F, 0x0F);
    P7->DIR = 0xFF;
    P7->OUT = 0xFF;
    Clock_Delay1us(10);
    P7->DIR = 0x00;
    Clock_Delay1us(1000);
    Off_IR_Emitter(0x0F, 0x0F);
    int i;
    for (i = 0; i < 8; i++)
        sensors[i] = (P7->IN & (1 << i)) ? 1 : 0;
}

uint8_t Get_IR_Sensor_Value(void) {
    On_IR_Emitter(0x0F, 0x0F);
    P7->DIR = 0xFF;
    P7->OUT = 0xFF;
    Clock_Delay1us(10);
    P7->DIR = 0x00;
    Clock_Delay1us(1000);
    Off_IR_Emitter(0x0F, 0x0F);
    return P7->IN & 0xFF;

}

void Print_IR_Sensor(uint8_t sensors[8]) {
    int i;
    printf("sensors: [");
    for (i = 0; i < 7; i++)
        printf("%d, ", sensors[i]);
    printf("%d]\n", sensors[7]);
}
