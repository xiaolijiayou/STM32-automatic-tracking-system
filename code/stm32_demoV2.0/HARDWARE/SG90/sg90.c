#include "sg90.h"
#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
/**
PA6----TIM3 CH1 舵机1——x轴
PA7----TIM3 CH2 舵机2——y轴
**/
void TIM3_PWM_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 使能GPIOA和TIM3时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 配置PA6为复用推挽输出，用于控制第一个舵机
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

		// 配置PA7（TIM3_CH2）为复用推挽输出，用于控制第二个舵机
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    // 初始化TIM3
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 初始化TIM3通道1的PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
		
		// 初始化TIM3通道2的PWM模式，用于控制第二个舵机
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);

    // 使能TIM3
    TIM_Cmd(TIM3, ENABLE);
}


/*****平滑转动函数******

	该函数用于实现舵机的平滑转动。
	TIMx：指定要操作的定时器，这里是TIM3。
	channel：指定定时器的通道，2 或 3。
	start：起始的 PWM 占空比。
	end：目标的 PWM 占空比。
	step：每次增加或减少的 PWM 占空比步长。
	delay_time：每次改变 PWM 占空比后的延时时间，用于控制转动速度。
	注意：step和delay_time的值会影响舵机的转动速度和平滑度。可以根据实际需求调整这两个参数
************************/
void smooth_rotate(TIM_TypeDef* TIMx, uint8_t channel, uint16_t start, uint16_t end, uint16_t step, uint16_t delay_time)
{
 int16_t direction = (start < end) ? 1 : -1;
    uint16_t current = start;

    // 避免步长过大导致越过目标值
    if (step > (start > end ? start - end : end - start)) {
        step = (start > end ? start - end : end - start);
    }
		printf("Start: %d, End: %d, Step: %d\n", start, end, step);
    while ((direction == 1 && current < end) || (direction == -1 && current > end)) {
        switch (channel) {
            case 1:
                TIM_SetCompare1(TIMx, current);
                break;
            case 2:
                TIM_SetCompare2(TIMx, current);
                break;
            default:
                break;
        }
        delay_ms(delay_time);
        current += direction * step;

        // 防止越界
        if (direction == 1 && current > end) {
            current = end;
        } else if (direction == -1 && current < end) {
            current = end;
        }
				printf("Current: %d\n", current);
    }

    // 确保到达目标角度
    switch (channel) {
        case 1:
            TIM_SetCompare1(TIMx, end);
            break;
        case 2:
            TIM_SetCompare2(TIMx, end);
            break;
        default:
            break;
    }
    delay_ms(delay_time);

}

void UpdatePulseWidth(uint16_t angle) 
{
    // 角度转脉冲宽度计算
    uint16_t pulse = MIN_PULSE + (angle * (MAX_PULSE - MIN_PULSE)) / 180;
    TIM_SetCompare2(TIM3, pulse);
}
