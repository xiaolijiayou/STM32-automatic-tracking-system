#include "sg90.h"
#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
/**
PA6----TIM3 CH1 ���1����x��
PA7----TIM3 CH2 ���2����y��
**/
void TIM3_PWM_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // ʹ��GPIOA��TIM3ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // ����PA6Ϊ����������������ڿ��Ƶ�һ�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

		// ����PA7��TIM3_CH2��Ϊ����������������ڿ��Ƶڶ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    // ��ʼ��TIM3
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // ��ʼ��TIM3ͨ��1��PWMģʽ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
		
		// ��ʼ��TIM3ͨ��2��PWMģʽ�����ڿ��Ƶڶ������
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);

    // ʹ��TIM3
    TIM_Cmd(TIM3, ENABLE);
}


/*****ƽ��ת������******

	�ú�������ʵ�ֶ����ƽ��ת����
	TIMx��ָ��Ҫ�����Ķ�ʱ����������TIM3��
	channel��ָ����ʱ����ͨ����2 �� 3��
	start����ʼ�� PWM ռ�ձȡ�
	end��Ŀ��� PWM ռ�ձȡ�
	step��ÿ�����ӻ���ٵ� PWM ռ�ձȲ�����
	delay_time��ÿ�θı� PWM ռ�ձȺ����ʱʱ�䣬���ڿ���ת���ٶȡ�
	ע�⣺step��delay_time��ֵ��Ӱ������ת���ٶȺ�ƽ���ȡ����Ը���ʵ�������������������
************************/
void smooth_rotate(TIM_TypeDef* TIMx, uint8_t channel, uint16_t start, uint16_t end, uint16_t step, uint16_t delay_time)
{
 int16_t direction = (start < end) ? 1 : -1;
    uint16_t current = start;

    // ���ⲽ��������Խ��Ŀ��ֵ
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

        // ��ֹԽ��
        if (direction == 1 && current > end) {
            current = end;
        } else if (direction == -1 && current < end) {
            current = end;
        }
				printf("Current: %d\n", current);
    }

    // ȷ������Ŀ��Ƕ�
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
    // �Ƕ�ת�����ȼ���
    uint16_t pulse = MIN_PULSE + (angle * (MAX_PULSE - MIN_PULSE)) / 180;
    TIM_SetCompare2(TIM3, pulse);
}
