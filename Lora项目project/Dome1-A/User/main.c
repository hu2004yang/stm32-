#include "stm32f10x.h"  
#include "string.h"  
#include "stdio.h" 
#include "delay.h"   
#include "bsp_usart.h"
#include "oled.h"
#include "bsp_key.h"
#include "bsp_GeneralTim.h"
#include "cs_lora.h"
#include "bsp_led.h"  
#include "bsp_dht11.h"
#include "bsp_adc.h"

#define SDATA "ToggleLED\r\n"	


/* lora 信道 */
uint8_t lora_channel = 12;
/* lora 地址 */
uint16_t lora_addr = 12;

unsigned char cscxReg[3]={0xC1,0x00,0x09};	//发送查询数据
unsigned char csrevReg[12]={0xC1 ,0x00 ,0x09 ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF, 0xFF ,0xFF ,0xFF};
unsigned char key=0;
char disOLED[24];
char LORA_Send_buf[64] = {0};
uint16_t ADC_Value[2];


uint16_t timer3_count = 0;
int main(void)
{
	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_2);
	SysTick_Init(72);                          /* 系统时钟初始化 */
	usart1_init(115200);                       /* 初始化串口1-debug */
	printf("\r\n ############ http://www.csgsm.com/ ############\r\n ############("__DATE__ " - " __TIME__ ")############\r\n");
	printf("LORA Board LED OK\r\n");	
	usart2_init(9600);                         /* 串口2初始化-lora */
	CS_OLED_Init();                            /* oled初始化 */
	OLED_Clear();                              /* 清空屏幕 */
	OLED_ShowString(16,0,"LoRa开发板");       /* OLED显示 */
	printf("LORA Board LCD OK\r\n");           /* 打印数据 */ 
	OLED_ShowString(100,0,"[A]");              /* OELD显示 */
	CSTX_Lora_M0_M1_Init();                    /* 初始化M0 M1 */
	Set_Lora_Config_Mode();                    /* 配置LORA为配置模式	*/
	delay_ms(10);                              /* 延时 */
	
	if(LORA_M0_STATE == 0 && LORA_M1_STATE == 1)/* 检测是否为配置模式 */
	{
		printf("\r\n M1 = 1 ; M0 = 0 Register mode \r\n");/* 打印 */
		CSTX_Lora_Receive_Buf_Clear();                    /* 清空LORA接收buf */
		CSTX_Lora_Send(cscxReg,3);                        /* 发送获取配置信息指令 */
		cstx_reg_Receive_Data(&lora_channel,&lora_addr);  /* 解析LORA反馈配置信息 */
		printf("lora 地址:%#x 信道:%#x\r\n",lora_addr,lora_channel);/* 打印 */
		sprintf((char *)disOLED,"Addr:%02X Ch:%02X",lora_addr,lora_channel);/* 编辑字符串 */
		OLED_ShowString(0,2,disOLED);                     /* OELD显示 */
	}
	else                                                /* 如果不是配置模式，不进行发送询问数据 */
	{
		printf("\r\n M1 = 0; M0 = 0 Transparent mode \r\n");/* 打印 */
	}
	delay_ms(100);                              /* 延迟 */
	Set_Lora_Transmit_Mode();                   /* 将LORA配置为发送模式 */
	delay_ms(100);                              /* 延迟 */
	DHT11_Init();																/* 温湿度初始化 */
	MYADC_Init();																/* ADC初始化 */
	LED1_Init();															  /* LED初始化 */
	if(DHT11_Read_TempAndHumidity(&DHT11_Data) == SUCCESS)
	{
		printf("\r\n ***Read DHT11 Succeed*** \r\n");
		OLED_ShowString(0,6,"Read DHT11 [OK].");
	}
	else
	{
		printf("\r\n ***Read DHT11 failure*** \r\n");
		OLED_ShowString(0,6,"Read DHT11 [NO].");
	}
	TIM3_Init(7199,1000-1);                     /* 100ms定时器3初始化 */
	while(1)                                    /* 主循环 */
	{
		/* 大概1000ms进入一次 */
		if(timer3_count > 100)
		{
			/* 计算 PA1引脚 光照 */
			ADC_Value[0] = ADC_Read[0];
			/* 计算 PA6引脚 光照 */
			ADC_Value[1] = ADC_Read[1];
			memset(disOLED,0,24);
			sprintf((char *)disOLED,"PA1:%dPA6:%d",ADC_Value[0],ADC_Value[1]);
			/* 清屏 */
			OLED_ShowString(0,6,"                "); 
			OLED_ShowString(0,6,disOLED);
			printf("\r\n-->ADC PA1=%d , ADC PA6=%d<--\r\n",ADC_Value[0],ADC_Value[1]);

			/*延时500ms */			
			delay_ms(500);
			
			/* 读取温湿度 如果成功就发送数据到主机 */
			if(DHT11_Read_TempAndHumidity(&DHT11_Data) == SUCCESS)
			{
				/* 打印温湿度获取成功 */
				printf("\r\n Read DHT11 Succeed \r\n");
				/* 编辑LORA发送数据 */
				memset(LORA_Send_buf,0,sizeof(LORA_Send_buf));
				sprintf(LORA_Send_buf,"%d,%d,%d.%d,%d",ADC_Value[0],ADC_Value[1],DHT11_Data.temp_int,DHT11_Data.temp_deci,DHT11_Data.humi_int);
				/* LORA发送 */
				CSXT_Lora_Send_String(LORA_Send_buf);
				/* 编辑OELD显示数据 */
				memset(disOLED,0,24);
				sprintf((char *)disOLED,"T:%d.%dC H:%d.%dR ",DHT11_Data.temp_int,DHT11_Data.temp_deci,DHT11_Data.humi_int,DHT11_Data.humi_deci);
				/* OLED显示 */
				OLED_ShowString(0,6,disOLED);
				printf("\r\n ***Send DHT11: %s *** \r\n", disOLED);
				OLED_ShowString(0,4,"Send data [OK]..");	
				delay_ms(500);	
				OLED_ShowString(0,4,"Send data ......");
			}			
			
			/* 将计数变量置零 */
			timer3_count = 0;
			
		}
		
	}
}


/* 100ms进入一次 */
void  TIM3_IRQHandler(void)
{
	if ( TIM_GetITStatus( TIM3, TIM_IT_Update) != RESET ) 
	{
		TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);	
		
		timer3_count++;
		
	}
}
