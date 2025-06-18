/*
 * Motor.c
 *
 *  Created on: 2025. 5. 29.
 *      Author: devkim
 */

#include ".\Motor.h"

void Motor_Init(void) {
    P3->SEL0 &= ~0xC0; // 67: nSLPR, nSLPL
    P3->SEL1 &= ~0xC0;
    P3->DIR  |=  0xC0;
    P3->OUT  &= ~0xC0;

    P5->SEL0 &= ~0x30; // 45: DIRR, DIRL
    P5->SEL1 &= ~0x30;
    P5->DIR  |=  0x30;
    P5->OUT  &= ~0x30;

    P2->SEL0 &= ~0xC0; // 67: PWMR, PWML
    P2->SEL1 &= ~0xC0;
    P2->DIR  |=  0xC0;
    P2->OUT  &= ~0xC0;

    // 주기 = 15'000 / 1'500'000 = 0.01s = 10ms
    // 초당 100개의 PWM 사이클 출력, 이후 듀티를 통해 조절
    PWM_Init34(15000, 0, 0);
}

void Motor_Move(uint16_t leftDuty, uint16_t rightDuty) {
    P3->OUT |= 0xC0;
    PWM_Duty3(rightDuty);
    PWM_Duty4(leftDuty);
}

void Left_Forward() {
    P5->OUT &= ~0x10; // P5.4 PH: 0
}

void Left_Backward() {
    P5->OUT |= 0x10;  // P5.4 PH: 1
}

void Right_Forward() {
    P5->OUT &= ~0x20; // P5.5 PH: 0
}

void Right_Backward() {
    P5->OUT |= 0x20;  // P5.5 PH: 1
}

void Move_Forward(uint16_t leftDuty, uint16_t rightDuty, uint16_t time) {
    Left_Forward();
    Right_Forward();
    Motor_Move(leftDuty, rightDuty);
    Clock_Delay1ms(time);
}

void Move_Backward(uint16_t leftDuty, uint16_t rightDuty, uint16_t time) {
    Left_Backward();
    Right_Backward();
    Motor_Move(leftDuty, rightDuty);
    Clock_Delay1ms(time);
}

void Rotate_Left(uint16_t leftDuty, uint16_t rightDuty, uint16_t time) {
    Left_Backward();
    Right_Forward();
    Motor_Move(leftDuty, rightDuty);
    Clock_Delay1ms(time);
}

void Rotate_Right(uint16_t leftDuty, uint16_t rightDuty, uint16_t time) {
    Left_Forward();
    Right_Backward();
    Motor_Move(leftDuty, rightDuty);
    Clock_Delay1ms(time);
}

void Rotate_Left90(uint8_t sensors[8]) {
    Left_Backward();
    Right_Forward();
    Motor_Move(3000, 3000);
    Clock_Delay1ms(ROTATION_TIME * 2);
}


void Rotate_Right90(uint8_t sensors[8]) {
    Left_Forward();
    Right_Backward();
    Motor_Move(3000, 3000);
    Clock_Delay1ms(ROTATION_TIME * 2);
}


void Motor_Stop(uint16_t time) {
    P3->OUT &= ~0xC0;
    Clock_Delay1ms(time);
}


int Compute_Forward_CenteringError(uint8_t sensors[8]) {
    // 0, 1: Right, 6, 7: Left
    int weights[8] = {0, 0, -142, -47, 47, 142, 0, 0};
    int sum, active;
    sum = active = 0;

    int i;
    for (i = 0; i < 8; i++) {
        if (sensors[i]) {
            sum += weights[i];
            active++;
        }
    }

    return (active > 0) ? (sum / active) : -1;
}


int Compute_Backward_CenteringError(uint8_t sensors[8]) {
    // 0, 1: Right, 6, 7: Left
    int weights[8] = {0, 0, 142, 47, -47, -142, 0, 0};
    int sum, active;
    sum = active = 0;

    int i;
    for (i = 0; i < 8; i++) {
        if (sensors[i]) {
            sum += weights[i];
            active++;
        }
    }

    return (active > 0) ? (sum / active) : -1;
}


void Align_And_Move_Forward(uint8_t sensors[8]) {
    int error = Compute_Forward_CenteringError(sensors);
    uint8_t abs_error = abs(error);

    if (error == -1)
        Move_Backward(900, 900, 90);
    else if (abs_error < 23)
        Move_Forward(1500, 1500, 30);
    else if (abs_error < 47) {
        if (error > 0)
            Rotate_Left(1000, 1000, 45);
        else
            Rotate_Right(1000, 1000, 45);
    } else if (abs_error < 142) {
        if (error > 0)
            Rotate_Left(1000, 1000, 30);
        else
            Rotate_Right(1000, 1000, 30);
    }
    Motor_Stop(0);
}


void Align_And_Move_Backward(uint8_t sensors[8]) {
    int error = Compute_Backward_CenteringError(sensors);
    uint8_t abs_error = abs(error);

    if (error == -1)
        Move_Forward(900, 900, 90);
    else if (abs_error < 23)
        Move_Backward(1500, 1500, 30);
    else if (abs_error < 47) {
        if (error > 0)
            Rotate_Right(1000, 1000, 45);
        else
            Rotate_Left(1000, 1000, 45);
    } else if (abs_error < 142) {
        if (error > 0)
            Rotate_Right(1000, 1000, 30);
        else
            Rotate_Left(1000, 1000, 30);
    }
    Motor_Stop(0);
}
