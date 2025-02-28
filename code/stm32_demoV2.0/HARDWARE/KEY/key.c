#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "usart.h"
///////////////	 
/*
五个按键
key0  一键复位
key1  上
key2  下
key3  左
key4  右
*/
///////////////  

//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PORTB时钟
	
	 // 使能AFIO时钟，用于外部中断
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// 禁用 JTAG 调试接口，释放 PB3 引脚
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//KEY0-KEY4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //设置成下拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB4,3，5，6，7
	
	// 连接PB3 - PB7到对应的外部中断线
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource6);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7);

	// 配置外部中断线3 - 7
	EXTI_InitStructure.EXTI_Line = EXTI_Line3 | EXTI_Line4 | EXTI_Line5 | EXTI_Line6 | EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

  // 配置NVIC,外部中断线3 - 4单独处理，5 - 9合并处理
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

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		else if(WK_UP==1)return WKUP_PRES;
	}else if(KEY0==1&&KEY1==1&&WK_UP==0)key_up=1; 	    
 	return 0;// 无按键按下
}

// 按键扫描函数
uint8_t KEY_Scan2(void) {
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
        delay_ms(20); // 消抖
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0); // 等待按键释放
            return 1; // 按键 1 按下
        }
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
        delay_ms(20); // 消抖
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0); // 等待按键释放
            return 2; // 按键 2 按下
        }
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 0) {
        delay_ms(20); // 消抖
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 0) {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 0); // 等待按键释放
            return 3; // 按键 3 按下
        }
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0) {
        delay_ms(20); // 消抖
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0) {
            while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0); // 等待按键释放
            return 4; // 按键 4 按下
        }
    }
    return 0; // 无按键按下
}
