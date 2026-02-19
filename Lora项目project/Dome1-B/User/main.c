#include "stm32f10x.h"  
#include "string.h"  
#include "stdio.h" 
#include "delay.h"   
#include "bsp_usart.h"
#include "oled.h"
#include "bsp_GeneralTim.h"
#include "cs_lora.h"
#include "bsp_led.h"  
#include "bsp_io.h"   
#include "Weather.h"
#include <stdlib.h>
#include "bsp_esp8266.h"


#define SDATA "ToggleLED\r\n"	


/* lora 信道 */
uint8_t lora_channel = 12;
/* lora 地址 */
uint16_t lora_addr = 12;

unsigned char cscxReg[3]={0xC1,0x00,0x09};	//发送查询数据
unsigned char csrevReg[12]={0xC1 ,0x00 ,0x09 ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF, 0xFF ,0xFF ,0xFF};
unsigned char key=0;
char disOLED[24];

static void turn_on_led1(void)
{
	// 直接使用库函数点亮LED1 (PA15, 低电平点亮)
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
}

static void turn_off_led1(void)
{
	// 直接使用库函数熄灭LED1 (PA15, 高电平熄灭)
	GPIO_SetBits(GPIOA, GPIO_Pin_15);
}

int main(void)
{
	char *CSTXREVData[4];
	uint8_t j = 0;
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
	OLED_ShowString(100,0,"[B]");              /* OELD显示 */
	LED1_Init();                              /* LED1初始化 */
	printf("LORA Board LED OK\r\n");
	
	CSTX_Lora_M0_M1_Init();                    /* 初始化M0 M1 */
	Set_Lora_Config_Mode();                    /* 配置LORA为配置模式	*/
	delay_ms(10);                              /* 延时 */
	
	if(LORA_M0_STATE == 0 && LORA_M1_STATE == 1)/* 检测是否为配置模式 */
	{
		printf("\r\n M1 = 1 ; M0 = 0 Register mode \r\n");/* 打印 */
		CSTX_Lora_Receive_Buf_Clear();                    /* 清空LORA接收buf */
		CSTX_Lora_Send(cscxReg,3);                        /* 发送获取配置信息指令 */
		cstx_reg_Receive_Data(&lora_channel,&lora_addr);  /* 解析LORA反馈配置信息 */
		printf("lora site:%#x channel:%#x\r\n",lora_addr,lora_channel);/* 打印 */
		sprintf((char *)disOLED,"Addr:%02X Ch:%02X",lora_addr,lora_channel);/* 编辑字符串 */
		OLED_ShowString(0,2,disOLED);                     /* OELD显示 */
	}
	else                                                /* 如果不是配置模式，不进行发送询问数据 */
	{
		printf("\r\n M1 = 0; M0 = 0 Transparent mode \r\n");/* 打印 */
	}
	CSTX_Lora_Receive_Buf_Clear();
	delay_ms(100);                              /* 延迟 */
	Set_Lora_Transmit_Mode();                   /* 将LORA配置为发送模式 */
	delay_ms(100);                              /* 延迟 */
	Beep_Init();                                /* 蜂鸣器初始化 */
	TIM3_Init(7199,1000-1);                     /* 100ms定时器3初始化 */

	// 测试LED是否能正常工作 - 闪烁3次
	printf("Testing LED...\r\n");
	for(int i=0; i<3; i++) {
		GPIO_ResetBits(GPIOA, GPIO_Pin_15);  // 点亮
		delay_ms(200);
		GPIO_SetBits(GPIOA, GPIO_Pin_15);    // 熄灭
		delay_ms(200);
	}
	printf("LED test completed\r\n");

	/* 初始化 ESP8266 并尝试连接指定 WiFi */
	ESP8266_Init();
	delay_ms(1000);

	// 测试ESP8266模块
	if (ESP8266_AT_Test()) {
		printf("ESP8266 AT OK\r\n");

		// 设置为Station模式
		printf("Setting ESP8266 to Station mode...\r\n");
		while(!ESP8266_Net_Mode_Choose(STA)) {
			printf("Mode set failed, retrying...\r\n");
			delay_ms(1000);
		}
		printf("Station mode set successfully\r\n");
		delay_ms(500);

		// 启用DHCP
		printf("Enabling DHCP...\r\n");
		ESP8266_DHCP_CUR();
		delay_ms(500);

		// 在连接前增加更多调试信息
		printf("Attempting to connect to WiFi network: Xiaomi12SPro\r\n");
		printf("Password: 25005454hu\r\n");

		if (ESP8266_JoinAP("Xiaomi12SPro","25005454hu")) {
			printf("Successfully joined WiFi: Xiaomi12SPro\r\n");
			delay_ms(1000);

			// 添加一个简单的指示，表明WiFi已连接
			OLED_ShowString(0,7,"WiFi: Connected   ");
		} else {
			printf("ESP8266 join AP failed\r\n");
			printf("Error: Could not connect to WiFi network\r\n");

			// 打印ESP8266返回的原始数据用于调试
			extern char Esp8266_rx_buf[];
			extern __IO u16 Esp8266_rx_length;
			printf("ESP8266 response: %s\r\n", Esp8266_rx_buf);
			printf("Response length: %d\r\n", Esp8266_rx_length);

			// 在OLED上显示错误信息
			OLED_ShowString(0,7,"WiFi: Connect Err");

			// 可能的原因：
			// 1. WiFi密码错误
			// 2. 热点名称错误
			// 3. 信号强度不够
			// 4. ESP8266模块故障
			printf("Possible reasons:\r\n");
			printf("- Incorrect password\r\n");
			printf("- Wrong network name\r\n");
			printf("- Weak signal strength\r\n");
			printf("- ESP8266 module issue\r\n");
		}
	} else {
		printf("ESP8266 not responding\r\n");
		// 这表示ESP8266模块没有正常工作
		OLED_ShowString(0,7,"WiFi: Module Err  ");
		printf("ESP8266 module communication error\r\n");
		printf("Check hardware connections\r\n");
	}
	while(1)                                    /* 主循环 */
	{
		// 空闲时打印心跳信息，确保程序在运行
		static uint32_t loop_count = 0;
		loop_count++;
		if(loop_count % 500000 == 0) {
			printf(">>> Main loop running, waiting for LORA data...\r\n");
		}

		/* LORA发送过来数据 */
		if(buf_lora.rx_flag == 1)
		{
			/* 先保存rx_flag状态，然后立即清零，避免重复处理 */
			buf_lora.rx_flag = 0;

			// 使用忙等待替代 delay_ms(20)
			volatile uint32_t i;
			for (i = 0; i < 144000; i++) {  // 20ms = 144,000 cycles at 72MHz
				__NOP();
			}

			/* Print received raw data for debugging */
			printf("\r\n===Raw received data[%d bytes]: ",buf_lora.index);
			for(uint8_t i=0; i<buf_lora.index; i++)
			{
				printf("%c",buf_lora.buf[i]);
			}
			printf("===\r\n");

			/* Find the last complete data packet (search backwards for last comma-separated complete data) */
			char *last_data = NULL;
			uint8_t comma_count = 0;
			
			// Traverse from back to front to find the last complete data packet (format: ADC1,ADC2,temperature,humidity)
			for(int i = buf_lora.index - 1; i >= 0; i--)
			{
				if(buf_lora.buf[i] == ',')
				{
					comma_count++;
					// If 4 commas found, there is complete data after
					if(comma_count == 4)
					{
						// Find start position of complete data packet (skip preceding comma)
						if(i + 1 < buf_lora.index)
						{
							last_data = (char*)&buf_lora.buf[i + 1];
						}
						break;
					}
				}
			}
			
			// If 4-comma complete data not found, try using original strtok method
			if(last_data == NULL)
			{
				printf("\r\nWarning: Complete data packet not found, using original data\r\n");
				// Check if original data starts with non-digit (error data)
				if(buf_lora.buf[0] >= '0' && buf_lora.buf[0] <= '9')
				{
					last_data = (char*)buf_lora.buf;
				}
				else
				{
					printf("\r\nError data detected (first byte not digit), discarding this reception\r\n");
					CSTX_Lora_Receive_Buf_Clear();
					continue;
				}
			}
			
			printf("\r\nUsing data: %s\r\n", last_data);

			/* 使用strtok解析从机发送过来的数据 */
			CSTXREVData[0] = strtok(last_data,","); /* ADC1数据 */
			CSTXREVData[1] = strtok(NULL,",");                /* ADC2数据 */
			CSTXREVData[2] = strtok(NULL,",");                /* 温度数据(带小数) */
			CSTXREVData[3] = strtok(NULL,",");                /* 湿度数据 */

			/* Data integrity check: ensure all fields are successfully parsed */
			if(CSTXREVData[0] == NULL || CSTXREVData[1] == NULL ||
			   CSTXREVData[2] == NULL || CSTXREVData[3] == NULL)
			{
				printf("\r\nData parse failed - fields: %s %s %s %s, discarding this reception\r\n",
				       CSTXREVData[0] ? CSTXREVData[0] : "NULL",
				       CSTXREVData[1] ? CSTXREVData[1] : "NULL",
				       CSTXREVData[2] ? CSTXREVData[2] : "NULL",
				       CSTXREVData[3] ? CSTXREVData[3] : "NULL");
				CSTX_Lora_Receive_Buf_Clear();
				continue;
			}

			/* Additional check: ensure first field is digit, not 'A' or other error data */
			if(CSTXREVData[0][0] < '0' || CSTXREVData[0][0] > '9')
			{
				printf("\r\nError data detected (first byte not digit: %c 0x%02X), discarding this reception\r\n",
				       CSTXREVData[0][0], CSTXREVData[0][0]);
				CSTX_Lora_Receive_Buf_Clear();
				continue;
			}

			/* Parse temperature and humidity data */
			float temp_val = atof(CSTXREVData[2]);
			float humi_val = atof(CSTXREVData[3]);

			/* Print received data */
			printf("\r\nReceived LORA PA1 ADC:%s , PA6 ADC:%s , humidity:%.1fRH , temperature:%.1fC\r\n",
			       CSTXREVData[0], CSTXREVData[1], humi_val, temp_val);
			memset(disOLED,0,24);
			if(j==0)
			{
				sprintf((char *)disOLED,"ADC1:%.2fV,%s",(float)atoi(CSTXREVData[0])*3.3f/4096,CSTXREVData[0]);//显示PA1的adc采集的数据
				j=1;
			}
			else
			{
				sprintf((char *)disOLED,"ADC2:%.2fV,%s",(float)atoi(CSTXREVData[1])*3.3f/4096,CSTXREVData[1]);//显示PA6的ADC采集的数据
				j=0;
			}

			/* 温度和湿度阈值 */
			#define TEMP_HIGH_THRESHOLD  35.0f    // 温度过高阈值
			#define TEMP_LOW_THRESHOLD   15.0f    // 温度过低阈值
			#define HUMI_HIGH_THRESHOLD 80.0f      // 湿度过高阈值
			#define HUMI_LOW_THRESHOLD  4.0f      // 湿度过低阈值

			// Check data validity (float range)
			if(humi_val > 100.0f || humi_val < 0.0f) {
				printf("Warning: Humidity invalid (%.3f), skipping alarm\r\n", humi_val);
				BEEP_OFF;
			}
			/* If temperature too high, buzzer alarm */
			else if(temp_val >= TEMP_HIGH_THRESHOLD)
			{
				/* Turn on buzzer */
				BEEP_ON;
				printf("\r\nTemperature too high (%.1fC)! Buzzer alarm started\r\n", temp_val);
			}
			/* If temperature too low, buzzer alarm */
			else if(temp_val <= TEMP_LOW_THRESHOLD)
			{
				/* Turn on buzzer */
				BEEP_ON;
				printf("\r\nTemperature too low (%.1fC)! Buzzer alarm started\r\n", temp_val);
			}
			/* If humidity too high, buzzer alarm */
			else if(humi_val >= HUMI_HIGH_THRESHOLD)
			{
				/* Turn on buzzer */
				BEEP_ON;
				printf("\r\nHumidity too high (%.1f%%)! Buzzer alarm started\r\n", humi_val);
			}
			/* If humidity too low, buzzer alarm */
			else if(humi_val <= HUMI_LOW_THRESHOLD)
			{
				/* Turn on buzzer */
				BEEP_ON;
				printf("\r\nHumidity too low (%.1f%%)! Buzzer alarm started\r\n", humi_val);
			}
			/* If normal, turn off buzzer */
			else
			{
				/* Turn off buzzer */
				BEEP_OFF;
			}
			/* Clear screen */
			OLED_ShowString(0,6,"                ");
			/* OLED display ADC data */
			OLED_ShowString(0,6,disOLED);
			/* Edit OLED display data */
			memset(disOLED,0,sizeof(disOLED));
			sprintf(disOLED,"Temp:%.1fC Humi:%.1f%%", temp_val, humi_val);
			OLED_ShowString(0,4,disOLED);
			/* Clear LORA receive buffer */
			CSTX_Lora_Receive_Buf_Clear();

			/* --- Get weather and compare temperature (light up LED1 if diff > 4C) --- */
			{
				char weatherBody[512] = {0};
				char weatherTemp[32] = {0};
				char weatherText[64] = {0};
				float recv_temp = temp_val;

				// Only call weather API when temperature data is valid
				if(recv_temp >= -50.0f && recv_temp <= 100.0f)
				{
					printf("\r\n>>> Starting to get weather data...\r\n");
					if (Weather_GetSeniverseNow(weatherBody, sizeof(weatherBody))) {
						printf(">>> Weather API call successful, starting to parse...\r\n");
						if (Weather_ParseSeniverseNow(weatherBody, weatherTemp, sizeof(weatherTemp), weatherText, sizeof(weatherText))) {
							float wtemp = (float)atof(weatherTemp);
							float diff = wtemp - recv_temp;
							if (diff < 0.0f) diff = -diff;

							// Debug info: print temperature and difference
							printf("Local Temp: %.1fC, Weather Temp: %.1fC, Diff: %.1fC, Text: %s\r\n",
							       recv_temp, wtemp, diff, weatherText);

							if (diff > 4.0f) {
								turn_on_led1();
								OLED_ShowString(0,5,"LED1:OVER DIFF");
								printf("LED1 ON - Temperature difference exceeded 4C\r\n");
							} else {
								turn_off_led1();  // Turn off LED
								OLED_ShowString(0,5,"LED1:Normal    ");
								printf("LED1 OFF - Temperature difference normal\r\n");
							}
						}
						else
						{
							printf(">>> Weather data parse failed\r\n");
						}
					}
					else
					{
						printf(">>> Weather API call failed\r\n");
					}
				}
				else
				{
					printf("\r\n>>> Temperature data invalid (%.1fC), skipping weather API call\r\n", recv_temp);
				}
			}
		}
	}
}



void  TIM3_IRQHandler(void)
{
	if ( TIM_GetITStatus( TIM3, TIM_IT_Update) != RESET ) 
	{
		TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);		
	}
}
