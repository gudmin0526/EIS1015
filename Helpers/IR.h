/*
 * IR.h
 *
 *  Created on: 2025. 6. 4.
 *      Author: devkim
 */

#ifndef IR_IR_H_
#define IR_IR_H_

#include "msp.h"
#include ".\Clock.h"
#include <stdio.h>

void IR_Init(void);
void On_IR_Emitter(uint8_t mask_P5, uint8_t mask_P9);
void Off_IR_Emitter(uint8_t mask_P5, uint8_t mask_P9);
void Read_IR_Sensor(uint8_t sensors[8]);
void Print_IR_Sensor(uint8_t sensors[8]);
uint8_t Get_IR_Sensor_Value(void);

#endif /* IR_IR_H_ */
