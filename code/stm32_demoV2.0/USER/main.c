#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "sg90.h"
#include "usart.h"
#include "adc.h"
#include "math.h"

uint16_t current_pwm = SERVO_90_DEGREE;
uint16_t step_key=50;
uint16_t delay_time_k=2;
uint16_t new_pwm;
uint16_t x_diff, y_diff;
uint16_t x_current_pwm = SERVO_90_DEGREE;
uint16_t y_current_pwm = SERVO_45_DEGREE;

volatile uint16_t current_angle = 90; // ��ǰ�Ƕȣ�0-180��
volatile uint8_t key_pressed = 0;// ����״̬��־λ
volatile uint16_t step;
volatile uint16_t delay_time;

#define LIGHT_DIFFERENCE_THRESHOLD 100  // ������ղ�ֵ��ֵ������ֵС�ڸ�ֵʱ���������

// �������λ����������
void check_and_alarm(void)
{
    if (x_current_pwm == SERVO_0_DEGREE || x_current_pwm == SERVO_180_DEGREE ||
        y_current_pwm == SERVO_0_DEGREE || y_current_pwm == MAX_PULSE )
    {
      Buzzer_On();
			GPIO_ResetBits(GPIOB, GPIO_Pin_11);//led��
			printf("����޶Ⱦ���\r\n");
    }
    else
    {
        Buzzer_Off();	
			GPIO_SetBits(GPIOB, GPIO_Pin_11);
    }
}

void adc_XY_date()
{
		uint16_t Left_value = Get_Adc_Average(0, 10);
    uint16_t Right_value = Get_Adc_Average(1, 10);
		uint16_t up_value = Get_Adc_Average(2, 10);
    uint16_t down_value = Get_Adc_Average(3, 10);

//		printf("��ߵ�ѹΪ��%d\n\r",Left_value);
//		printf("�ұߵ�ѹΪ��%d\n\r",Right_value);
//		printf("�ϱߵĵ�ѹΪ��%d\n\r", up_value);
//    printf("�±ߵĵ�ѹΪ��%d\n\r", down_value);
	
		// ����X�����Ҳ�ֵ
		x_diff = Left_value > Right_value ? Left_value - Right_value : Right_value - Left_value;
	
		// ���� Y �����¹��ղ�ֵ
    uint16_t y_diff = up_value > down_value ? up_value - down_value : down_value - up_value;
	
//		printf("X ���ֵΪ��%d\n\r", x_diff);
//    printf("Y ���ֵΪ��%d\n\r", y_diff);
	
		// ���ݲ�ֵ��̬������������ʱ
		if (x_diff > 500||y_diff > 500) {
				step = 50;
				delay_time = 2;
		} else if (x_diff > 200||y_diff > 200) {
				step = 20;
				delay_time = 3;
		} else {
				step = 5;
				delay_time = 4;
		}
     //���� X ����
		if(x_diff > LIGHT_DIFFERENCE_THRESHOLD)
		{
			if (Left_value > Right_value && x_current_pwm < SERVO_180_DEGREE) 
			{
					// �������� 1 ���ո�ǿ�������ת
					new_pwm=x_current_pwm +step;// �Ӵ�ÿ�ε����Ĳ���
				
					if (new_pwm > SERVO_180_DEGREE) 
					{
							new_pwm = SERVO_180_DEGREE;
					}
					smooth_rotate(TIM3, 2, x_current_pwm, new_pwm, step, delay_time);// �Ӵ󲽳���������ʱ
					x_current_pwm=new_pwm;
			} 
			else if (Left_value < Right_value && x_current_pwm > SERVO_0_DEGREE) 
			{
					// �������� 2 ���ո�ǿ�������ת
					new_pwm=x_current_pwm -step;
				
					if (new_pwm< SERVO_0_DEGREE) 
					{
							new_pwm = SERVO_0_DEGREE;
					}
					smooth_rotate(TIM3, 2, x_current_pwm, new_pwm, step, delay_time);
					x_current_pwm=new_pwm;
			}
		
		}
		printf("x_current_pwm%d\r\n",x_current_pwm);
		check_and_alarm(); // �����λ������
		// ���� Y ����
    if (y_diff > LIGHT_DIFFERENCE_THRESHOLD)
    {
        if (up_value > down_value && y_current_pwm < MAX_PULSE ) 
        {
            // ��� 1 Y ��˳ʱ��ת��
            uint16_t new_pwm = y_current_pwm + step;
            if (new_pwm > MAX_PULSE ) 
            {
                new_pwm = MAX_PULSE ;
            }
            smooth_rotate(TIM3, 1, y_current_pwm, new_pwm, step, delay_time); // ƽ��ת����������ʱ
            y_current_pwm = new_pwm;
        } 
        else if (up_value < down_value && y_current_pwm > SERVO_0_DEGREE) 
        {
            // ��� 2 Y ����ʱ��ת��
            uint16_t new_pwm = y_current_pwm - step;
            if (new_pwm < SERVO_0_DEGREE) 
            {
                new_pwm = SERVO_0_DEGREE;
            }
            smooth_rotate(TIM3, 1, y_current_pwm, new_pwm, step, delay_time);
            y_current_pwm = new_pwm;
        }
    }
		check_and_alarm(); // �����λ������
		printf("y_current_pwm%d\r\n",y_current_pwm);

	delay_ms(20); // �ʵ���ʱ������Ƶ�������͵���

}


int main(void)
{		
		delay_init();	    	 //��ʱ������ʼ��	  
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
		LED_Init();			     //LED�˿ڳ�ʼ��
		KEY_Init();          //��ʼ���밴�����ӵ�Ӳ����
		TIM3_PWM_Init(19999, 71); // ��ʱ��ʱ��Ϊ72MHz / (71 + 1) = 1MHz����������Ϊ1us���Զ���װ��ֵΪ19999��PWM����Ϊ20ms
		uart1_init(115200); 
		Adc_Init();
		Buzzer_Init(); // ��ʼ��������
		printf("���demo\n\r");
		while(1)
		{

			adc_XY_date();
			
		}	 
		
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		switch(Res)
		{
		  case 1:
					TIM_SetCompare2(TIM3, SERVO_45_DEGREE);
					printf("45��\n\r");
					break;
			case 2:
					TIM_SetCompare2(TIM3, SERVO_90_DEGREE);
					printf("90��\n\r");
					break;
			case 3:
					TIM_SetCompare2(TIM3, SERVO_135_DEGREE);
					printf("135��\n\r");
					break;
			case 4:
					TIM_SetCompare2(TIM3, SERVO_180_DEGREE);
					printf("180��\n\r");
					break;
			case 5:
					TIM_SetCompare2(TIM3, SERVO_0_DEGREE);
					printf("0��\n\r");
					break;
		
		}
  } 

}

// �ⲿ�ж�3������
void EXTI3_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
			 // ������������
			delay_ms(10);
			if (EXTI_GetITStatus(EXTI_Line3) != RESET)
			{
				printf("����1������\n\r");
				// ���� 1��X ������ת
				if (x_current_pwm > SERVO_0_DEGREE) 
				{
					x_current_pwm -= step_key;
					smooth_rotate(TIM3, 2, x_current_pwm + step_key, x_current_pwm, step_key , delay_time_k);
				}
				
				check_and_alarm(); // �����λ������
			}
			// ����жϱ�־λ
			EXTI_ClearITPendingBit(EXTI_Line3);
    }
}

// �ⲿ�ж�4������
void EXTI4_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line4) != RESET)
    {

				printf("����2������\n\r");
				// ���� 2��X ������ת
				if (x_current_pwm < SERVO_180_DEGREE) 
				{
					x_current_pwm += step_key;
					smooth_rotate(TIM3, 2, x_current_pwm - step_key, x_current_pwm, step_key , delay_time_k);
				}
				check_and_alarm(); // �����λ������
        // ����жϱ�־λ
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

// �ⲿ�ж�5 - 9������
void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
   
				printf("����3������\n\r");
			// ���� 3��Y ������ת
					if (y_current_pwm > SERVO_0_DEGREE) 
				{
						y_current_pwm -= step_key;
						smooth_rotate(TIM3, 1, y_current_pwm + step_key, y_current_pwm, step_key , delay_time_k);
				}
				check_and_alarm(); // �����λ������
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
    if (EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
       
				printf("����4������\n\r");
				// ���� 4��Y ������ת
				if (y_current_pwm < SERVO_180_DEGREE) 
				{
						y_current_pwm += step_key;
						smooth_rotate(TIM3, 1, y_current_pwm - step_key, y_current_pwm, step_key , delay_time_k);
				}
				check_and_alarm(); // �����λ������
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
    if (EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
     
				printf("����5������\n\r");
				// ���� 5��һ����λ
				x_current_pwm = SERVO_90_DEGREE;
				y_current_pwm=SERVO_45_DEGREE;
				TIM_SetCompare2(TIM3, SERVO_90_DEGREE);//X��90��
				TIM_SetCompare1(TIM3, SERVO_45_DEGREE);//y��45��

				Buzzer_Off(); // �رշ�����
				GPIO_SetBits(GPIOB, GPIO_Pin_11);
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
}
