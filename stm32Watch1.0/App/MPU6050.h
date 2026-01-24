#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f10x.h"
#include "i2c.h"
#include "MPU6050_Reg.h"
#include "OLED.h"
#include "key.h"

#define MPU6050_ADDRESS		0xD0

extern double pi;
extern  float Roll,Pitch,Yaw;

void MPU6050_Write(uint8_t RegAddress, uint8_t Data);
uint8_t MPU6050_Read(uint8_t RegAddress);
void MPU6050_Init(void);
uint8_t MPU6050_GetID(void);
void MPU6050_GetData(int16_t *Accel_X, int16_t *Accel_Y, int16_t *Accel_Z, int16_t *Gyro_X, int16_t *Gyro_Y, int16_t *Gyro_Z);

void MPU6050_Calculation(void);

int MPU6050_Page(void);

#endif
