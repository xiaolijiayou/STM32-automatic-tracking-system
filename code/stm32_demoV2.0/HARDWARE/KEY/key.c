#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "usart.h"
///////////////	 
/*
�������
key0  һ����λ
key1  ��
key2  ��
key3  ��
key4  ��
*/
///////////////  

//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTBʱ��
	
	 // ʹ��AFIOʱ�ӣ������ⲿ�ж�
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// ���� JTAG ���Խӿڣ��ͷ� PB3 ����
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//KEY0-KEY4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //���ó���������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB4,3��5��6��7
	
	// ����PB3 - PB7����Ӧ���ⲿ�ж���
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource6);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7);

	// �����ⲿ�ж���3 - 7
	EXTI_InitStructure.EXTI_Line = EXTI_Line3 | EXTI_Line4 | EXTI_Line5 | EXTI_Line6 | EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // �����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

  // ����NVIC,�ⲿ�ж���3 - 4��������5 - 9�ϲ�����
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY3���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		else if(WK_UP==1)return WKUP_PRES;
	}else if(KEY0==1&&KEY1==1&&WK_UP==0)key_up=1; 	    
 	return 0;// �ް�������
}

// ����ɨ�躯��
uint8_t KEY_Scan2(void) {
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
        delay_ms(20); // ����
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0); // �ȴ������ͷ�
            return 1; // ���� 1 ����
        }
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
        delay_ms(20); // ����
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0); // �ȴ������ͷ�
            return 2; // ���� 2 ����
        }
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 0) {
        delay_ms(20); // ����
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 0) {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 0); // �ȴ������ͷ�
            return 3; // ���� 3 ����
        }
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0) {
        delay_ms(20); // ����
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0) {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0); // �ȴ������ͷ�
            return 4; // ���� 4 ����
        }
    }
    return 0; // �ް�������
}
