#include "Weather.h"
#include "bsp_esp8266.h"
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "stm32f10x.h"

// 发起 HTTP GET 请求并将响应正文写入 outBody
bool Weather_GetWeather(const char *host, int port, const char *path, char *outBody, int outBodyLen)
{
	char cmd[256];
	char httpReq[512];
	char *recv = NULL;

	if (!host || !path || !outBody || outBodyLen <= 0) return false;

	printf(">>> Weather: Establishing TCP connection %s:%d\r\n", host, port);
	// Establish TCP connection
	snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d", host, port);
	if (!ESP8266_Cmd(cmd, "OK", "ALREAY CONNECT", 4000)) {
		printf(">>> Weather: TCP connection failed\r\n");
		return false;
	}
	printf(">>> Weather: TCP connection successful\r\n");

	// Prepare HTTP GET request
	snprintf(httpReq, sizeof(httpReq), "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, host);
	int len = strlen(httpReq);

	printf(">>> Weather: Sending HTTP request, length=%d\r\n", len);
	//printf(">>> Weather: HTTP request content: %s\r\n", httpReq);
	
	// Declare extern variables before use
	extern char Esp8266_rx_buf[];
	extern __IO u16 Esp8266_rx_length;
	extern __IO u16 Esp8266_rx_flag;
	
	// Notify upcoming data length
	snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d", len);
	if (!ESP8266_Cmd(cmd, ">", NULL, 2000)) {
		// 不关闭连接,避免ESP8266_Cmd中的delay_ms导致卡死
		printf(">>> Weather: CIPSEND failed\r\n");
		return false;
	}
	
	// ESP8266_Cmd has cleared the buffer internally, now ready to send HTTP body
	printf(">>> Weather: Sending HTTP request body...\r\n");
	Usart3_SendStr(httpReq);  // Send directly without ESP8266_Cmd
	printf(">>> Weather: HTTP request sent, waiting for SEND OK...\r\n");
	
	// Wait for SEND OK confirmation (max 2s) - use busy wait
	int send_waited = 0;
	const int send_timeout = 2000;
	while (send_waited < send_timeout) {
		if (strstr(Esp8266_rx_buf, "SEND OK") || strstr(Esp8266_rx_buf, "ERROR")) {
			break;
		}
		// 使用忙等待替代 delay_ms(100)
		volatile uint32_t i;
		for (i = 0; i < 720000; i++) {  // 100ms
			__NOP();
		}
		send_waited += 100;
	}
	
	if (!strstr(Esp8266_rx_buf, "SEND OK")) {
		printf(">>> Weather: SEND OK not received! Buffer: %.200s\r\n", Esp8266_rx_buf);
		// 不关闭连接,避免ESP8266_Cmd中的delay_ms导致卡死
		return false;
	}
	printf(">>> Weather: SEND OK confirmed\r\n");
	
	// Clear buffer before waiting for server response
	printf(">>> Weather: clearing buffer...\r\n");
	Esp8266_rx_length = 0;
	Esp8266_rx_flag = 0;
	// Don't clear Esp8266_rx_buf[0] to avoid potential issues
	printf(">>> Weather: buffer cleared\r\n");

	// Wait for server response - use busy wait that directly uses SysTick
	printf(">>> Weather: waiting for server response...\r\n");

	// Direct SysTick-based busy wait to avoid delay_ms issues
	int waited = 0;
	const int total_timeout = 6000;

	// Use direct SysTick register access for timing
	extern volatile uint32_t uwTick;  // Usually defined in main.c or similar

	//uint32_t start_time = 0;
	// Try to get a timer value - use SysTick->VAL if uwTick not available
	// For simplicity, we'll use a simple counter-based approach

	while (waited < total_timeout && Esp8266_rx_length == 0) {
		// Simple busy wait - approximately 100ms
		// Based on system clock (72MHz), 100ms = 7,200,000 cycles
		volatile uint32_t i;
		for (i = 0; i < 720000; i++) {
			__NOP();  // No operation, just burn cycles
			// Check periodically during the loop
			if (i % 10000 == 0 && Esp8266_rx_length > 0) {
				break;
			}
		}
		waited += 100;

		if (waited % 1000 == 0) {
			printf(">>> Weather: waited %dms\r\n", waited);
		}
	}

	if (Esp8266_rx_length == 0) {
		printf(">>> Weather: TIMEOUT! No data in %dms\r\n", total_timeout);
		// 不关闭连接,避免ESP8266_Cmd中的delay_ms导致卡死
		return false;
	}

	printf(">>> Weather: Data received! rx_len=%d\r\n", Esp8266_rx_length);

	// Wait a bit more to ensure all data received - use busy wait
	// 500ms = 5 * 100ms, each 100ms = 720,000 cycles at 72MHz
	for(int k = 0; k < 5; k++) {
		volatile uint32_t i;
		for (i = 0; i < 720000; i++) {
			__NOP();
		}
	}

	printf(">>> Weather: Buffer length=%d\r\n", Esp8266_rx_length);
	// protect against overflow when terminating buffer
	int cap = Esp8266_rx_length;
	if (cap < 0) cap = 0;
	// assume rx buffer max >= 1024, clamp defensively
	if (cap > 4095) cap = 4095;
	Esp8266_rx_buf[cap] = '\0';
	printf(">>> Weather: Buffer content: %.400s\r\n", Esp8266_rx_buf);

	// Check if we have any data
	if (Esp8266_rx_length > 0) {
		// Try to find HTTP response
		char *http_start = strstr(Esp8266_rx_buf, "HTTP/1.1");
		if (http_start) {
			printf(">>> Weather: Found HTTP response start\r\n");
			recv = http_start;
		} else {
			printf(">>> Weather: HTTP response marker not found, using full buffer\r\n");
			recv = Esp8266_rx_buf;
		}
	} else {
		printf(">>> Weather: NO data received at all!\r\n");
		// 不关闭连接,直接返回,避免ESP8266_Cmd中的delay_ms导致卡死
		return false;
	}

	// Check if we have enough data for HTTP response
	if (recv == NULL || strlen(recv) < 20) {
		printf(">>> Weather: Insufficient data received\r\n");
		// 不关闭连接,直接返回,避免ESP8266_Cmd中的delay_ms导致卡死
		return false;
	}

	printf(">>> Weather: Data received successfully\r\n");

	// 提取 body 并拷贝到 outBody
	if (!Weather_ParseHTTPBody(recv, outBody, outBodyLen)) {
		// 若解析失败，直接返回全部接收到的内容（裁剪）
		strncpy(outBody, recv, outBodyLen - 1);
		outBody[outBodyLen - 1] = '\0';
	}

	// 不关闭连接,避免ESP8266_Cmd中的delay_ms导致卡死
	// ESP8266下次连接时会自动关闭旧连接

	return true;
}

// 从 HTTP 响应中提取 body（简单查找双 CRLF）
bool Weather_ParseHTTPBody(const char *httpResp, char *outBody, int outBodyLen)
{
	const char *p;
	if (!httpResp || !outBody || outBodyLen <= 0) return false;
	p = strstr(httpResp, "\r\n\r\n");
	if (!p) return false;
	p += 4; // 跳过分隔符
	strncpy(outBody, p, outBodyLen - 1);
	outBody[outBodyLen - 1] = '\0';
	return true;
}

// 调用 Seniverse API 获取当前天气（固定 key 与城市 Jinzhong）
bool Weather_GetSeniverseNow(char *outBody, int outBodyLen)
{
	const char *host = "api.seniverse.com";
	const int port = 80;
	const char *path = "/v3/weather/now.json?key=SJYmRQbUrO0G-fGX5&location=Jinzhong&language=zh-Hans&unit=c";

	return Weather_GetWeather(host, port, path, outBody, outBodyLen);
}

// 简单解析 Seniverse 返回的 JSON body，提取 temperature 与 text
bool Weather_ParseSeniverseNow(const char *body, char *temperature, int tempLen, char *text, int textLen)
{
	const char *p = NULL;
	int i;

	if (!body) return false;
	if (temperature && tempLen > 0) temperature[0] = '\0';
	if (text && textLen > 0) text[0] = '\0';

	// 查找 temperature 字段
	p = strstr(body, "\"temperature\"");
	if (!p) p = strstr(body, "temperature");
	if (p && temperature && tempLen > 0) {
		p = strchr(p, ':');
		if (p) {
			p++;
			while (*p == ' ' || *p == '\"') p++;
			for (i = 0; i < tempLen - 1 && *p; ++i) {
				if ((*p >= '0' && *p <= '9') || *p == '-' || *p == '.') {
					temperature[i] = *p++;
				} else break;
			}
			temperature[i] = '\0';
		}
	}

	// 查找 text 字段
	p = strstr(body, "\"text\"");
	if (!p) p = strstr(body, "text");
	if (p && text && textLen > 0) {
		p = strchr(p, ':');
		if (p) {
			p++;
			while (*p == ' ' || *p == '\"') p++;
			for (i = 0; i < textLen - 1 && *p; ++i) {
				if (*p == '\"' || *p == '\\' || *p == '\r' || *p == '\n') break;
				text[i] = *p++;
			}
			text[i] = '\0';
		}
	}

	return ((temperature && temperature[0]) || (text && text[0]));
}
