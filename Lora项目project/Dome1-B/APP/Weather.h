#ifndef __WEATHER_H
#define __WEATHER_H

#include <stdbool.h>

// 发起 HTTP GET 并将响应正文拷贝到 outBody（以 NUL 结尾）
// host: 主机名或 IP（不带协议和端口）
// port: 端口（通常 80）
// path: 请求路径，例如 "/data?city=..."
// outBody: 输出缓冲区
// outBodyLen: 输出缓冲区长度
bool Weather_GetWeather(const char *host, int port, const char *path, char *outBody, int outBodyLen);

// 从完整 HTTP 响应中提取 body 部分，拷贝到 outBody
bool Weather_ParseHTTPBody(const char *httpResp, char *outBody, int outBodyLen);

// 使用 Seniverse 实时天气 API（固定 key 与城市 Jinzhong）
// 示例 URL:
// https://api.seniverse.com/v3/weather/now.json?key=SJYmRQbUrO0G-fGX5&location=Jinzhong&language=zh-Hans&unit=c
bool Weather_GetSeniverseNow(char *outBody, int outBodyLen);

// 从 Seniverse 返回的 JSON body 中解析温度和文本（简单字符串解析）
// temperature: 输出温度字符串（例如 "5"）
// text: 输出天气文本（例如 "多云"）
bool Weather_ParseSeniverseNow(const char *body, char *temperature, int tempLen, char *text, int textLen);

#endif // __WEATHER_H
