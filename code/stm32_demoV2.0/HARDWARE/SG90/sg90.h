#ifndef __SG90_H
#define __SG90_H	 
#include "sys.h"

// 舵机控制角度对应的PWM占空比
#define SERVO_0_DEGREE 500   // 0° 对应的PWM脉冲宽度为0.5ms，定时器计数为500（假设定时器时钟为1MHz，计数周期为1us）
#define SERVO_45_DEGREE 1000   // 45° 对应的PWM脉冲宽度为1ms，定时器计数为1000
#define SERVO_90_DEGREE 1500 // 90° 对应的PWM脉冲宽度为1.5ms，定时器计数为1500
#define SERVO_135_DEGREE 2000 // 135° 对应的PWM脉冲宽度为2ms，定时器计数为2000
#define SERVO_180_DEGREE 2500// 180° 对应的PWM脉冲宽度为2.5ms，定时器计数为2500

#define MIN_PULSE           500      // 0.5ms对应0度
#define MAX_PULSE           1800     // 2.5ms对应180度

void TIM3_PWM_Init(u16 arr, u16 psc);
void smooth_rotate(TIM_TypeDef* TIMx, uint8_t channel, uint16_t start, uint16_t end, uint16_t step, uint16_t delay_time);
void UpdatePulseWidth(uint16_t angle);
#endif

