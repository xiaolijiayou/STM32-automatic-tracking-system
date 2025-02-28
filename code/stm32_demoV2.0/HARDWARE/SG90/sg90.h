#ifndef __SG90_H
#define __SG90_H	 
#include "sys.h"

// ������ƽǶȶ�Ӧ��PWMռ�ձ�
#define SERVO_0_DEGREE 500   // 0�� ��Ӧ��PWM������Ϊ0.5ms����ʱ������Ϊ500�����趨ʱ��ʱ��Ϊ1MHz����������Ϊ1us��
#define SERVO_45_DEGREE 1000   // 45�� ��Ӧ��PWM������Ϊ1ms����ʱ������Ϊ1000
#define SERVO_90_DEGREE 1500 // 90�� ��Ӧ��PWM������Ϊ1.5ms����ʱ������Ϊ1500
#define SERVO_135_DEGREE 2000 // 135�� ��Ӧ��PWM������Ϊ2ms����ʱ������Ϊ2000
#define SERVO_180_DEGREE 2500// 180�� ��Ӧ��PWM������Ϊ2.5ms����ʱ������Ϊ2500

#define MIN_PULSE           500      // 0.5ms��Ӧ0��
#define MAX_PULSE           1800     // 2.5ms��Ӧ180��

void TIM3_PWM_Init(u16 arr, u16 psc);
void smooth_rotate(TIM_TypeDef* TIMx, uint8_t channel, uint16_t start, uint16_t end, uint16_t step, uint16_t delay_time);
void UpdatePulseWidth(uint16_t angle);
#endif

