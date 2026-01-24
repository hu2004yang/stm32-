#include "Dino.h"

struct Object_Postion
{
	uint8_t MinX,MinY,MaxX,MaxY;
};


int Score;
uint16_t Ground_Pos; 
uint8_t Barrier_Pos;
uint8_t Barrier_Flag;
uint8_t Cloud_Pos;
uint8_t dino_jump_flag=0;//0:奔跑，1:跳跃
uint8_t Jump_Pos;

uint16_t jump_t;
void Dino_Tick(void)
{
	static uint16_t Score_Count,Ground_Count,Cloud_Count;
	Score_Count++;
	Ground_Count++;	
	Cloud_Count++;

	if(Score_Count>=1000)
	{
		Score_Count=0;
		Score++;
	}
	if(Ground_Count>=20)
	{
		Ground_Count=0;
		Ground_Pos++;
		Barrier_Pos++;
		if(Ground_Pos>255) Ground_Pos=0;
		if(Barrier_Pos>143) Barrier_Pos=0;
	}
	if(Cloud_Count>=50)
	{
		Cloud_Count=0;
		Cloud_Pos++;
		if(Cloud_Pos>200) Cloud_Pos=0;
	}
	if(dino_jump_flag==1)
	{
		jump_t++;
		if(jump_t>1000)
		{
			jump_t=0;
			dino_jump_flag=0;
		}
	}
}

void Show_Score(void)
{
	OLED_ShowNum(98,0,Score,5,OLED_6X8);
}

void Show_Ground(void)
{
	uint8_t i, j;
	
	if(Ground_Pos < 128)
	{
		for(i=0; i<128; i++)
		{
			// 逐位绘制Ground数组中对应位置的图案
			uint8_t pattern = Ground[i + Ground_Pos];
			for(j = 0; j < 8; j++) {
				if(pattern & (1 << j)) {
					OLED_DrawPoint(i, 56 + j);  // 在屏幕底部绘制点，Y坐标为56-63
				}
			}
		}
	}
	else
	{
		for(i=0; i<255-Ground_Pos; i++)
		{
			// 逐位绘制Ground数组中对应位置的图案
			uint8_t pattern = Ground[i + Ground_Pos];
			for(j = 0; j < 8; j++) {
				if(pattern & (1 << j)) {
					OLED_DrawPoint(i, 56 + j);  // 在屏幕底部绘制点
				}
			}
		}
		for(uint8_t i=255-Ground_Pos; i<128; i++)
		{
			// 逐位绘制Ground数组中从开头开始的图案（循环）
			uint8_t pattern = Ground[i - (255-Ground_Pos)];
			for(j = 0; j < 8; j++) {
				if(pattern & (1 << j)) {
					OLED_DrawPoint(i, 56 + j);  // 在屏幕底部绘制点
				}
			}
		}
	}
}

struct Object_Postion barrier;
void Show_Barrier(void)
{
	if(Barrier_Pos>142) 
	{
		Barrier_Flag=rand()%3;
	}

	OLED_ShowImage(127-Barrier_Pos,44,16,18,Barrier[Barrier_Flag]);
	barrier.MinX=127-Barrier_Pos;
	barrier.MaxX=143-Barrier_Pos;
	barrier.MinY=44;
	barrier.MaxY=62;
}

void Show_Cloud(void)
{
	OLED_ShowImage(127-Cloud_Pos,9,16,8,Cloud);
}


double pi=3.1415927;
struct Object_Postion dino;
void Show_Dino(void)
{
	uint8_t CurrentKey=Key_GetNum();
	if(CurrentKey==1&& dino_jump_flag==0) dino_jump_flag=1;
	Jump_Pos = 28 * sin((float)(pi * jump_t / 1000));
	if(dino_jump_flag==0)
	{
		if(Cloud_Pos%2==0)OLED_ShowImage(0,40,16,18,Dino[0]);
		else OLED_ShowImage(0,40,16,18,Dino[1]);
	}
	else{
		OLED_ShowImage(0,40-Jump_Pos,16,18,Dino[2]);
	}
	dino.MinX=0;
	dino.MaxX=16;
	dino.MinY=40-Jump_Pos;
	dino.MaxY=60-Jump_Pos;
}

int IsColliding(struct Object_Postion *a,struct Object_Postion *b)
{
	if(a->MaxX>b->MinX&&a->MinX<b->MaxX&&a->MaxY>b->MinY&&a->MinY<b->MaxY)
	{
		OLED_Clear();
		OLED_ShowString(28,24,"Game Over",OLED_8X16);
		OLED_Update();
		Delay_s(1);
		OLED_Clear();
		OLED_Update();
		return 1;
	}
	return 0;
}

int Dino_Game(void)
{
	while(1)
	{
		OLED_Clear();
		Show_Score();
		Show_Ground();
		Show_Barrier();
		Show_Cloud();
		Show_Dino();
		OLED_Update();
		int return_flag=IsColliding(&dino,&barrier);
		if(return_flag==1)
		{
			return 0;
		} 
	}	
}

void Dino_Init(void)
{
	Score=Ground_Pos=Barrier_Pos=Cloud_Pos=Jump_Pos=0;
}
