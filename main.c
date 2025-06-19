#include "msp.h"
#include ".\Helpers\Clock.h"
#include ".\Helpers\Led.h"
#include ".\Helpers\SysTick.h"
#include ".\Helpers\Motor.h"
#include ".\Helpers\Timer.h"
#include ".\Helpers\IR.h"
#include ".\Helpers\Switch.h"
#include <stdio.h>

#define MAX_STACK_SIZE 128

/**
 * main.c
 */


typedef enum {
    Dir_S = 0,
    Dir_L,
    Dir_R,
} Direction;


typedef enum {
    Node_Straight = 0,
    Node_Wall,
    Node_End,
    Node_Branch,
    Node_Right,
    Node_Left,
} NodeType;


NodeType node_map[256] = {
    [0b00000000] = Node_Wall,
    [0b10000000] = Node_Wall,
    [0b11000000] = Node_Wall,
    [0b11100000] = Node_Wall,
    [0b00000001] = Node_Wall,
    [0b00000011] = Node_Wall,
    [0b00000111] = Node_Wall,
    [0b10000001] = Node_Wall,
    [0b10000010] = Node_Wall,
    [0b01000001] = Node_Wall,
    [0b11000011] = Node_Wall,
    [0b00001101] = Node_Right,   //  13
    [0b00001111] = Node_Right,   //  15
    [0b00011101] = Node_Right,   //  29
    [0b00011110] = Node_Right,   //  30
    [0b00011111] = Node_Right,   //  31
    [0b00101111] = Node_Right,   //  47
    [0b00111101] = Node_Right,   //  61
    [0b00111110] = Node_Right,   //  62
    [0b00111111] = Node_Right,   //  63
    [0b11011111] = Node_Right,   // 223
    [0b11001111] = Node_Right,   // 207
    [0b10001111] = Node_Right,   // 143
    [0b10011111] = Node_Right,   // 159
    [0b01011111] = Node_Right,   //  95
    [0b01110000] = Node_Left,    // 112
    [0b01110100] = Node_Left,    // 116
    [0b01111000] = Node_Left,
    [0b01111100] = Node_Left,    // 124
    [0b11110100] = Node_Left,    // 244
    [0b11111000] = Node_Left,    // 248
    [0b11111100] = Node_Left,    // 252
    [0b11101100] = Node_Left,    // 236
    [0b11110000] = Node_Left,    // 240
    [0b01111101] = Node_Branch,  // 125
    [0b01111111] = Node_Branch,  // 127
    [0b11111001] = Node_Branch,  // 249
    [0b11111010] = Node_Branch,  // 250
    [0b11111011] = Node_Branch,  // 251
    [0b11111101] = Node_Branch,  // 253
    [0b11111110] = Node_Branch,  // 254
    [0b11111111] = Node_Branch,  // 255
    [0b01111110] = Node_End,     // 126
};

uint8_t sensors[8];
NodeType type;

int top = -1;
Direction dir_stack[MAX_STACK_SIZE];
NodeType node_stack[MAX_STACK_SIZE];
uint8_t backtrack_count = 0;


NodeType Detect_NodeType(uint8_t sensors[8]) {
    return node_map[Get_IR_Sensor_Value()];
}


void Toggle_Led_By_NodeType(uint8_t sensors[8]) {
    type = Detect_NodeType(sensors);
    uint8_t color = 0;
    if (type == Node_Branch)
        color = LED_RED;
    else if (type == Node_Left)
        color = LED_BLUE;
    else if (type == Node_Right)
        color = LED_GREEN;
    else
        color = LED_RED | LED_BLUE;

    int i;
    for (i = 0; i < 8; i++) {
        if (sensors[i])
            TurnOn_Led(color);
        else
            TurnOn_Led(LED_WHITE);
        Clock_Delay1ms(500);
        TurnOff_Led();
        Clock_Delay1ms(500);
    }
    Clock_Delay1ms(1000);
}


NodeType Move_Backward_Straight(void) {
    Read_IR_Sensor(sensors);
    type = Detect_NodeType(sensors);

    if (type == Node_Straight)
        Align_And_Move_Backward(sensors);
    else
        Move_Backward(BASE_SPEED, BASE_SPEED, 30);

    Read_IR_Sensor(sensors);
    type = Detect_NodeType(sensors);

    return type;
}


NodeType Move_Forward_Straight(void) {
    Read_IR_Sensor(sensors);
    type = Detect_NodeType(sensors);

    if (type == Node_Straight)
        Align_And_Move_Forward(sensors);
    else
        Move_Forward(BASE_SPEED, BASE_SPEED, 30);

    Read_IR_Sensor(sensors);
    type = Detect_NodeType(sensors);

    return type;
}


void Process_Backtrack_Branch(void) {
    Direction _dir = dir_stack[top];
    Toggle_Led(LED_RED, backtrack_count);
    if (_dir == Dir_L) {
        while (type != Node_Left && type != Node_Branch)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        Rotate_Left90(sensors);
        Rotate_Left90(sensors);
        Motor_Stop(500);

        while (type == Node_Wall)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        while (type != Node_Right)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        while (type == Node_Right)
            type = Move_Forward_Straight();
        Motor_Stop(500);

        dir_stack[top] = Dir_R;
    }
    else if (_dir == Dir_R) {
        while (type != Node_Right && type != Node_Branch)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        Rotate_Left90(sensors);
        Motor_Stop(500);

        while (type == Node_Wall)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        while (type != Node_Branch)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        while (type == Node_Branch)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        top--;
    }
}


void Process_Backtrack_Left(void) {
    Direction _dir = dir_stack[top];
    Toggle_Led(LED_BLUE, backtrack_count);
    if (_dir == Dir_L) {
        while (type != Node_Branch && type != Node_Left)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        Move_Forward(JUMP_SPEED, JUMP_SPEED, 750);
        Motor_Stop(500);

        Rotate_Right90(sensors);
        Motor_Stop(500);

        while (type == Node_Wall)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        dir_stack[top] = Dir_S;
    } else if (_dir == Dir_S) {
        while (type != Node_Left)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        while (type == Node_Left)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        top--;
    }
}


void Process_Backtrack_Right(void) {
    Direction _dir = dir_stack[top];
    Toggle_Led(LED_GREEN, backtrack_count);
    if (_dir == Dir_R) {
        while (type != Node_Branch && type != Node_Right)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        Move_Forward(JUMP_SPEED, JUMP_SPEED, 750);
        Motor_Stop(500);

        Rotate_Left90(sensors);
        Motor_Stop(500);



        while (type != Node_Right)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        while (type == Node_Right)
            type = Move_Backward_Straight();
        Motor_Stop(500);
        top--;
    } else if (_dir == Dir_S) {
        while (type != Node_Right)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        while (type == Node_Right)
            type = Move_Backward_Straight();
        Motor_Stop(500);

        Move_Forward(JUMP_SPEED, JUMP_SPEED, 600);
        Motor_Stop(500);

        Rotate_Right90(sensors);
        Motor_Stop(500);

        while (type == Node_Wall)
            type = Move_Backward_Straight();
        dir_stack[top] = Dir_R;
    }
}


int Escape_And_Memorize_Maze(uint8_t sensors[8]) {
    while (backtrack_count > 0) {
        if (node_stack[top] == Node_Branch)
            Process_Backtrack_Branch();
        else if (node_stack[top] == Node_Left)
            Process_Backtrack_Left();
        else if (node_stack[top] == Node_Right)
            Process_Backtrack_Right();
        backtrack_count--;
    }
    type = Detect_NodeType(sensors);

    int cur;
    switch (type) {
    case Node_Branch:
        TurnOn_Led(LED_WHITE);
        while (type == Node_Branch)
            Move_Backward_Straight();
        Move_Forward(JUMP_SPEED, JUMP_SPEED, 750);
        Motor_Stop(500);

        Rotate_Left90(sensors);
        Motor_Stop(500);

        while (type == Node_Wall)
            type = Move_Backward_Straight();

        node_stack[++top] = Node_Branch;
        dir_stack[top] = Dir_L;
        break;
    case Node_Left:
        TurnOn_Led(LED_RED);
        while (type == Node_Left)
            type = Move_Backward_Straight();
        Move_Forward(JUMP_SPEED, JUMP_SPEED, 750);
        Motor_Stop(500);

        Rotate_Left90(sensors);
        Motor_Stop(500);

        while (type == Node_Wall)
            type = Move_Backward_Straight();

        node_stack[++top] = Node_Left;
        dir_stack[top] = Dir_L;
        break;
    case Node_Right:
        TurnOn_Led(LED_BLUE);
        Move_Forward(JUMP_SPEED, JUMP_SPEED, 300);
        Motor_Stop(500);

        node_stack[++top] = Node_Right;
        dir_stack[top] = Dir_S;
        break;
    case Node_Straight:
        TurnOn_Led(LED_GREEN);
        while (type == Node_Straight)
            type = Move_Forward_Straight();
        Motor_Stop(500);
        break;
    case Node_Wall:
        cur = top;
        backtrack_count++;
        while (cur >= 0 &&
                !((node_stack[cur] == Node_Left && dir_stack[cur] == Dir_L)   ||
                  (node_stack[cur] == Node_Branch && dir_stack[cur] == Dir_L) ||
                  (node_stack[cur] == Node_Right && dir_stack[cur] == Dir_S))) {
            backtrack_count++;
            cur--;
        }

        while (type == Node_Wall)
            type = Move_Backward_Straight();
        Motor_Stop(500);
        break;
    case Node_End:
        TurnOff_Led();
        return 1;
    default:
        TurnOff_Led();
        break;
    }
    return 0;
}


void Phase_One_Two(void) {
    Read_IR_Sensor(sensors);
    type = Detect_NodeType(sensors);

    /* --- Maze Escape and Memorize --- */
    int escape_flag = 1;
    while (!escape_flag) {
        Read_IR_Sensor(sensors);
        escape_flag = Escape_And_Memorize_Maze(sensors);
        Clock_Delay1ms(10);
    }
}


void Phase_Three(void) {
    int i;
    for (i = 0; i <= top; i++) {
        Read_IR_Sensor(sensors);
        type = Detect_NodeType(sensors);

        while (type != node_stack[i]) {
            if (type == Node_Straight) {
                Align_And_Move_Forward(sensors);
            } else if (type == Node_Wall) {
                Move_Backward(1000, 1000, 1000);
            } else {
                Move_Forward(BASE_SPEED, BASE_SPEED, 30);
            }
            Read_IR_Sensor(sensors);
            type = Detect_NodeType(sensors);
        }
        Motor_Stop(500);

        switch (dir_stack[i]) {
        case Dir_L:
            TurnOn_Led(LED_RED);
            while (type == node_stack[i])
                Move_Backward_Straight();
            Motor_Stop(500);

            Move_Forward(JUMP_SPEED, JUMP_SPEED, 750);
            Motor_Stop(500);

            Rotate_Left90(sensors);
            Motor_Stop(500);

            while (type == Node_Wall)
                type = Move_Backward_Straight();
            Motor_Stop(500);
            break;
        case Dir_R:
            TurnOn_Led(LED_BLUE);
            while (type == node_stack[i])
                Move_Backward_Straight();
            Motor_Stop(500);

            Move_Forward(JUMP_SPEED, JUMP_SPEED, 750);
            Motor_Stop(500);

            Rotate_Right90(sensors);
            Motor_Stop(500);

            while (type == Node_Wall)
                type = Move_Backward_Straight();
            Motor_Stop(500);
            break;
        case Dir_S:
            TurnOn_Led(LED_GREEN);
            while (type == node_stack[i]) {
                Move_Backward(BASE_SPEED, BASE_SPEED, 30);
                Read_IR_Sensor(sensors);
                type = Detect_NodeType(sensors);
            }
            Move_Forward(JUMP_SPEED, JUMP_SPEED, 750);
            Motor_Stop(500);
            break;
        default:
            TurnOn_Led(LED_WHITE);
            break;
        }
    }
}


int main(void) {
    Clock_Init48MHz();
    Motor_Init();
    Led_Init();
    IR_Init();
    Switch_Init();

    int sw_left, sw_right, i;
    sw_left = sw_right = 1;

    // Phase 1 & 2: Escape and Memorize
    while (sw_left)
        sw_left = Read_Switch_Left();

    for (i = 0; i < 3; i++) {
        TurnOn_Led(LED_GREEN);
        Clock_Delay1ms(500);
        TurnOff_Led();
        Clock_Delay1ms(500);
    }
    Phase_One_Two();

    // Phase 3: Escape via the shortest path
    while (sw_right)
        sw_right = Read_Switch_Right();

    for (i = 0; i < 3; i++) {
        TurnOn_Led(LED_GREEN);
        Clock_Delay1ms(500);
        TurnOff_Led();
        Clock_Delay1ms(500);
    }
    Phase_Three();
}
