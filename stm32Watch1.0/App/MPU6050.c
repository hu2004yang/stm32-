#include "MPU6050.h"

void MPU6050_Write(uint8_t RegAddress,uint8_t Data)
{
    I2C_Start();
    I2C_SendByte(MPU6050_ADDRESS);
    I2C_Ack();
    I2C_SendByte(RegAddress);
    I2C_Ack();
    I2C_SendByte(Data);
    I2C_Ack();
    I2C_Stop();
}

uint8_t MPU6050_Read(uint8_t RegAddress)
{
    I2C_Start();
    I2C_SendByte(MPU6050_ADDRESS);
    I2C_Ack();
    I2C_SendByte(RegAddress);
    I2C_Ack();

    I2C_Start();
    I2C_SendByte(MPU6050_ADDRESS|0x01);
    I2C_Ack();
    uint8_t data=I2C_ReadByte();
    I2C_Nack();
    I2C_Stop();

    return data;
}

void MPU6050_Init(void)
{
    I2C_Init(); 
    MPU6050_Write(MPU6050_PWR_MGMT_1, 0x01);		//电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
	MPU6050_Write(MPU6050_PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机
	MPU6050_Write(MPU6050_SMPLRT_DIV, 0x04);		//采样率分频寄存器，配置采样率
	MPU6050_Write(MPU6050_CONFIG, 0x06);			//配置寄存器，配置DLPF
	MPU6050_Write(MPU6050_GYRO_CONFIG, 0x18);	//陀螺仪配置寄存器，选择满量程为±2000°/s
	MPU6050_Write(MPU6050_ACCEL_CONFIG, 0x18);	//加速度计配置寄存器，选择满量程为±16g
}

uint8_t MPU6050_GetID(void)
{
    return MPU6050_Read(MPU6050_WHO_AM_I);
}

void MPU6050_GetData(int16_t *Accel_X,int16_t *Accel_Y,int16_t *Accel_Z,int16_t *Gyro_X,int16_t *Gyro_Y,int16_t *Gyro_Z)
{
    *Accel_X=MPU6050_Read(MPU6050_ACCEL_XOUT_H)<<8 | MPU6050_Read(MPU6050_ACCEL_XOUT_L);
    *Accel_Y=MPU6050_Read(MPU6050_ACCEL_YOUT_H)<<8 | MPU6050_Read(MPU6050_ACCEL_YOUT_L);
    *Accel_Z=MPU6050_Read(MPU6050_ACCEL_ZOUT_H)<<8 | MPU6050_Read(MPU6050_ACCEL_ZOUT_L);
    *Gyro_X=MPU6050_Read(MPU6050_GYRO_XOUT_H)<<8 | MPU6050_Read(MPU6050_GYRO_XOUT_L);
    *Gyro_Y=MPU6050_Read(MPU6050_GYRO_YOUT_H)<<8 | MPU6050_Read(MPU6050_GYRO_YOUT_L);
    *Gyro_Z=MPU6050_Read(MPU6050_GYRO_ZOUT_H)<<8 | MPU6050_Read(MPU6050_GYRO_ZOUT_L);
}

int16_t ax,ay,az,gx,gy,gz;//MPU6050测得的三轴加速度和角速度
float roll_g,pitch_g,yaw_g;//陀螺仪解算的欧拉角
float roll_a,pitch_a;//加速度计解算的欧拉角
float Roll,Pitch,Yaw=0;//互补滤波后的欧拉角
float a=0.9;//互补滤波器系数
float Delta_t=0.005;//采样周期

void MPU6050_Calculation(void)
{
	Delay_ms(5);
	MPU6050_GetData(&ax,&ay,&az,&gx,&gy,&gz);
	
	//通过陀螺仪解算欧拉角
	roll_g=Roll+(float)gx*Delta_t;
	pitch_g=Pitch+(float)gy*Delta_t;
	yaw_g=Yaw+(float)gz*Delta_t;
	
	//通过加速度计解算欧拉角
	pitch_a=atan2((-1)*ax,az)*180/pi;
	roll_a=atan2(ay,az)*180/pi;
	
	//通过互补滤波器进行数据融合
	Roll=a*roll_g+(1-a)*roll_a;
	Pitch=a*pitch_g+(1-a)*pitch_a;
	Yaw=a*yaw_g;
	
}

void Show_MPU6050_UI(void)
{
    OLED_ShowImage(0,0,16,16,Return);
	OLED_Printf(0,16,OLED_8X16,"Roll: %.2f",Roll);
	OLED_Printf(0,32,OLED_8X16,"Pitch:%.2f",Pitch);
	OLED_Printf(0,48,OLED_8X16,"Yaw:  %.2f",Yaw);
}

int MPU6050_Page(void)
{
    while (1)
    {
        uint8_t currentKey=Key_GetNum();
        if(currentKey==3)
        {
            OLED_Clear();
            OLED_Update();
            return 0;
        }

        OLED_Clear();
        MPU6050_Calculation();        
        Show_MPU6050_UI();
        OLED_ReverseArea(0,0,16,16);
        OLED_Update();
    }    
}
