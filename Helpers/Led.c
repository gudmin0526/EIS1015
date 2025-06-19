#include "Led.h"

void Led_Init(void) {
    P2->SEL0 &= ~0x07;
    P2->SEL1 &= ~0x07;
    P2->DIR  |=  0x07;
    P2->OUT  &= ~0x07;
}

void TurnOn_Led(int color) {
    P2->OUT &= ~0x07;
    P2->OUT |= color;
}

void TurnOff_Led(void) {
    P2->OUT &= ~0x07;
}

void Toggle_Led(int color, int count) {
    int i;
    for (i = 0; i < count; i++) {
        TurnOn_Led(color);
        Clock_Delay1ms(1000);
        TurnOff_Led();
        Clock_Delay1ms(1000);
    }
}
