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

volatile uint16_t current_angle = 90; // 当前角度（0-180）
volatile uint8_t key_pressed = 0;// 定义状态标志位
volatile uint16_t step;
volatile uint16_t delay_time;

#define LIGHT_DIFFERENCE_THRESHOLD 100  // 定义光照差值阈值，当差值小于该值时不调整舵机

// 检查舵机限位并报警函数
void check_and_alarm(void)
{
    if (x_current_pwm == SERVO_0_DEGREE || x_current_pwm == SERVO_180_DEGREE ||
        y_current_pwm == SERVO_0_DEGREE || y_current_pwm == MAX_PULSE )
    {
      Buzzer_On();
			GPIO_ResetBits(GPIOB, GPIO_Pin_11);//led亮
			printf("最大限度警告\r\n");
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

//		printf("左边电压为：%d\n\r",Left_value);
//		printf("右边电压为：%d\n\r",Right_value);
//		printf("上边的电压为：%d\n\r", up_value);
//    printf("下边的电压为：%d\n\r", down_value);
	
		// 计算X轴左右差值
		x_diff = Left_value > Right_value ? Left_value - Right_value : Right_value - Left_value;
	
		// 计算 Y 轴上下光照差值
    uint16_t y_diff = up_value > down_value ? up_value - down_value : down_value - up_value;
	
//		printf("X 轴差值为：%d\n\r", x_diff);
//    printf("Y 轴差值为：%d\n\r", y_diff);
	
		// 根据差值动态调整步长和延时
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
     //处理 X 轴舵机
		if(x_diff > LIGHT_DIFFERENCE_THRESHOLD)
		{
			if (Left_value > Right_value && x_current_pwm < SERVO_180_DEGREE) 
			{
					// 光敏电阻 1 光照更强，舵机右转
					new_pwm=x_current_pwm +step;// 加大每次调整的步长
				
					if (new_pwm > SERVO_180_DEGREE) 
					{
							new_pwm = SERVO_180_DEGREE;
					}
					smooth_rotate(TIM3, 2, x_current_pwm, new_pwm, step, delay_time);// 加大步长并减少延时
					x_current_pwm=new_pwm;
			} 
			else if (Left_value < Right_value && x_current_pwm > SERVO_0_DEGREE) 
			{
					// 光敏电阻 2 光照更强，舵机左转
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
		check_and_alarm(); // 检查限位并报警
		// 处理 Y 轴舵机
    if (y_diff > LIGHT_DIFFERENCE_THRESHOLD)
    {
        if (up_value > down_value && y_current_pwm < MAX_PULSE ) 
        {
            // 情况 1 Y 轴顺时针转动
            uint16_t new_pwm = y_current_pwm + step;
            if (new_pwm > MAX_PULSE ) 
            {
                new_pwm = MAX_PULSE ;
            }
            smooth_rotate(TIM3, 1, y_current_pwm, new_pwm, step, delay_time); // 平滑转动并设置延时
            y_current_pwm = new_pwm;
        } 
        else if (up_value < down_value && y_current_pwm > SERVO_0_DEGREE) 
        {
            // 情况 2 Y 轴逆时针转动
            uint16_t new_pwm = y_current_pwm - step;
            if (new_pwm < SERVO_0_DEGREE) 
            {
                new_pwm = SERVO_0_DEGREE;
            }
            smooth_rotate(TIM3, 1, y_current_pwm, new_pwm, step, delay_time);
            y_current_pwm = new_pwm;
        }
    }
		check_and_alarm(); // 检查限位并报警
		printf("y_current_pwm%d\r\n",y_current_pwm);

	delay_ms(20); // 适当延时，避免频繁采样和调整

}


int main(void)
{		
		delay_init();	    	 //延时函数初始化	  
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
		LED_Init();			     //LED端口初始化
		KEY_Init();          //初始化与按键连接的硬件接
		TIM3_PWM_Init(19999, 71); // 定时器时钟为72MHz / (71 + 1) = 1MHz，计数周期为1us，自动重装载值为19999，PWM周期为20ms
		uart1_init(115200); 
		Adc_Init();
		Buzzer_Init(); // 初始化蜂鸣器
		printf("舵机demo\n\r");
		while(1)
		{

			adc_XY_date();
			
		}	 
		
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		switch(Res)
		{
		  case 1:
					TIM_SetCompare2(TIM3, SERVO_45_DEGREE);
					printf("45度\n\r");
					break;
			case 2:
					TIM_SetCompare2(TIM3, SERVO_90_DEGREE);
					printf("90度\n\r");
					break;
			case 3:
					TIM_SetCompare2(TIM3, SERVO_135_DEGREE);
					printf("135度\n\r");
					break;
			case 4:
					TIM_SetCompare2(TIM3, SERVO_180_DEGREE);
					printf("180度\n\r");
					break;
			case 5:
					TIM_SetCompare2(TIM3, SERVO_0_DEGREE);
					printf("0度\n\r");
					break;
		
		}
  } 

}

// 外部中断3服务函数
void EXTI3_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
			 // 按键防抖处理
			delay_ms(10);
			if (EXTI_GetITStatus(EXTI_Line3) != RESET)
			{
				printf("按键1被按下\n\r");
				// 按键 1，X 轴舵机左转
				if (x_current_pwm > SERVO_0_DEGREE) 
				{
					x_current_pwm -= step_key;
					smooth_rotate(TIM3, 2, x_current_pwm + step_key, x_current_pwm, step_key , delay_time_k);
				}
				
				check_and_alarm(); // 检查限位并报警
			}
			// 清除中断标志位
			EXTI_ClearITPendingBit(EXTI_Line3);
    }
}

// 外部中断4服务函数
void EXTI4_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line4) != RESET)
    {

				printf("按键2被按下\n\r");
				// 按键 2，X 轴舵机右转
				if (x_current_pwm < SERVO_180_DEGREE) 
				{
					x_current_pwm += step_key;
					smooth_rotate(TIM3, 2, x_current_pwm - step_key, x_current_pwm, step_key , delay_time_k);
				}
				check_and_alarm(); // 检查限位并报警
        // 清除中断标志位
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

// 外部中断5 - 9服务函数
void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
   
				printf("按键3被按下\n\r");
			// 按键 3，Y 轴舵机上转
					if (y_current_pwm > SERVO_0_DEGREE) 
				{
						y_current_pwm -= step_key;
						smooth_rotate(TIM3, 1, y_current_pwm + step_key, y_current_pwm, step_key , delay_time_k);
				}
				check_and_alarm(); // 检查限位并报警
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
    if (EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
       
				printf("按键4被按下\n\r");
				// 按键 4，Y 轴舵机下转
				if (y_current_pwm < SERVO_180_DEGREE) 
				{
						y_current_pwm += step_key;
						smooth_rotate(TIM3, 1, y_current_pwm - step_key, y_current_pwm, step_key , delay_time_k);
				}
				check_and_alarm(); // 检查限位并报警
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
    if (EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
     
				printf("按键5被按下\n\r");
				// 按键 5，一键复位
				x_current_pwm = SERVO_90_DEGREE;
				y_current_pwm=SERVO_45_DEGREE;
				TIM_SetCompare2(TIM3, SERVO_90_DEGREE);//X轴90度
				TIM_SetCompare1(TIM3, SERVO_45_DEGREE);//y轴45度

				Buzzer_Off(); // 关闭蜂鸣器
				GPIO_SetBits(GPIOB, GPIO_Pin_11);
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
}
