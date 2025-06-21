/*
 * Motor.h
 *
 *  Created on: 2025. 5. 29.
 *      Author: devkim
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#define ROTATION_TIME 370
#define BASE_SPEED 2000
#define JUMP_SPEED 1600

#include "msp.h"
#include ".\Timer.h"
#include ".\Clock.h"
#include ".\IR.h"
#include <math.h>

void Motor_Init(void);
void Motor_Move(uint16_t leftDuty, uint16_t rightDuty);

void Left_Forward(void);
void Left_Backward(void);
void Right_Forward(void);
void Right_Backward(void);

void Move_Forward(uint16_t leftDuty, uint16_t rightDuty, uint16_t time);
void Move_Backward(uint16_t leftDuty, uint16_t rightDuty, uint16_t time);
void Rotate_Left(uint16_t leftDuty, uint16_t rightDuty, uint16_t time);
void Rotate_Right(uint16_t leftDuty, uint16_t rightDuty, uint16_t time);
void Rotate_Left90(uint8_t sensors[8]);
void Rotate_Right90(uint8_t sensors[8]);

void Motor_Stop(uint16_t time);

void Align_And_Move_Forward(uint8_t sensors[8]);
void Align_And_Move_Backward(uint8_t sensors[8]);


#endif /* MOTOR_H_ */
