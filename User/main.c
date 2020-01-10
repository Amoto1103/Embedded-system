#include "stm32f10x.h"
 
#include <stdio.h>

#include "./led/bsp_led.h" 
#include "./key/bsp_exti.h"
#include "./i2c/bsp_i2c_gpio.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./usart/bsp_usart.h"
#include "./SysTick/bsp_SysTick.h"
  
#include "./key/bsp_key.h"  
static void LCD_Test(void);	
uint32_t time = 0;
char str[20];
uint16_t  key_flag=0;
extern mk;
char dispBuff[100];
#define SNAKE_Max_Long 60//蛇的最大长度
static void Delay ( __IO uint32_t nCount );
//蛇结构体
struct Snake
{
	uint16_t X[SNAKE_Max_Long];
	uint16_t Y[SNAKE_Max_Long];
	u8 Long;//蛇的长度
	u8 Life;//蛇的生命 0活着 1死亡
	u8 Direction;//蛇移动的方向
}snake1,snake2;

//食物结构体
struct Food
{
	u8 X;//食物横坐标
	u8 Y;//食物纵坐标
	u8 Yes;//判断是否要出现食物的变量 0有食物 1需要出现食物
}food;

//游戏等级分数
struct Game
{
	u16 Score;//分数
	u8 Life;//游戏等级	
}game;

void KEY1_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{   key_flag=3;
		printf("key 1\n");
		if(snake1.Direction==1){
			snake1.Direction=3;
		}
		else if(snake1.Direction==2){
			snake1.Direction=4;
		}
		else if(snake1.Direction==3){
			snake1.Direction=2;
		}
		else if(snake1.Direction==4){
			snake1.Direction=1;
		}
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
}

void KEY2_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
		key_flag=4;
		printf("key 2\n");
		if(snake2.Direction==1){
			snake2.Direction=4;
		}
		else if(snake2.Direction==2){
			snake2.Direction=3;
		}
		else if(snake2.Direction==3){
			snake2.Direction=1;
	  }
		else if(snake2.Direction==4){
			snake2.Direction=2;
		}
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  

}

void USART1_IRQHandler(u8 GetData)//串口中断
{

if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		GetData=USART1->DR;
		mk=GetData;
		printf("收到的数据：\n");
	  printf("%d\n",GetData);
		printf("0x%x\n",mk);
		if(mk==0x1)
		{	
		snake1.Direction=1;
	  snake2.Direction=2;
			
		}
		if(GetData==0x2)
		{
			
    	snake1.Direction=2;
			snake2.Direction=1;
			
		}
		
	
	}
    
}

//游戏结束
void gameover()
	{
		   LCD_SetTextColor								(RED )	;
       ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	
		   sprintf(dispBuff,"        FUCK FOOD : %d",game.Score);
		   ILI9341_DispStringLine_EN(LINE(10),dispBuff);
			 ILI9341_DispStringLine_EN(LINE(8),"       YOU ARE FUCKED!");
		   
		}
		
//玩游戏
void play()
{
	u16 i;	
	u16 ChooseWords;
	char *Words[6];
	ChooseWords=5;
	snake1.Long=2;//定义蛇的长度
	snake1.Life=0;//蛇还活着
	snake1.Direction=1;//蛇的起始方向定义为右
	snake2.Long=2;//定义蛇的长度
	snake2.Life=0;//蛇还活着
	snake2.Direction=2;
	
	game.Score=0;//分数为0
	game.Life=4;//蛇的生命值
	food.Yes=1;//出现新食物
	
	snake1.X[0]=0;snake1.Y[0]=50;
	snake1.X[1]=10;snake1.Y[1]=50;
	snake2.X[0]=240;snake2.Y[0]=50;
	snake2.X[1]=230;snake2.Y[1]=50;
	
	Words[0]="GREAT!       ";
	Words[1]="GOOD!        ";
	Words[2]="BRILLIANT!   ";
	Words[3]="EXCELLENT!   ";
	Words[4]="UNBELIEVABLE!";
	Words[5]="             ";


		while(1)
	{
		
		   ILI9341_DrawRectangle(0,0,240,270,0);
        

      sprintf(dispBuff,"FUCK FOOD : %d  %s",game.Score,Words[ChooseWords]);
		
		  ILI9341_DispStringLine_EN(LINE(18),dispBuff);
			if(food.Yes==1)//出现新的食物
			{
				//在设定的区域内显示食物		

			    food.X=rand()%(220/10)*10+10;
					food.Y=rand()%(250/10)*10+10;

					food.Yes=0;
			}
				if(food.Yes==0)//有食物就要显示
				{	
		
					 LCD_SetColors(GREEN,BLACK);
					 ILI9341_DrawRectangle  (food.X,food.Y,10,10,1);
				
				}
				
				//snake1
				//取得需要重新画的蛇的节数
				for(i=snake1.Long-1;i>0;i--)
				{
					snake1.X[i]=snake1.X[i-1];
					snake1.Y[i]=snake1.Y[i-1];
				}

				
	
				switch(snake1.Direction)
				{
					case 1:snake1.X[0]+=10;break;//向右运动
					case 2:snake1.X[0]-=10;break;//向左运动
					case 3:snake1.Y[0]-=10;break;//向上运动
				
					case 4:snake1.Y[0]+=10;break;//向下运动
					
					
				}
				LCD_SetColors(YELLOW,BLACK);
					for(i=0;i<snake1.Long;i++)//画出蛇	
						
				ILI9341_DrawRectangle(snake1.X[i],snake1.Y[i],10,10,1);//画蛇身体
				
				//snake2
				//取得需要重新画的蛇的节数
				for(i=snake2.Long-1;i>0;i--)
				{
					snake2.X[i]=snake2.X[i-1];
					snake2.Y[i]=snake2.Y[i-1];
				}

				
	
				switch(snake2.Direction)
				{
					case 1:snake2.X[0]+=10;break;//向右运动
					case 2:snake2.X[0]-=10;break;//向左运动
					case 3:snake2.Y[0]-=10;break;//向上运动
				
					case 4:snake2.Y[0]+=10;break;//向下运动
					
					
				}
				LCD_SetColors(BLUE,BLACK);
					for(i=0;i<snake2.Long;i++)//画出蛇	
						
				ILI9341_DrawRectangle(snake2.X[i],snake2.Y[i],10,10,1);//画蛇身体
				
				

				
					 Delay(0xFFFFF);
				
				  
				    LCD_SetTextColor								(BLACK )	;
						ILI9341_DrawRectangle(snake1.X[snake1.Long-1],snake1.Y[snake1.Long-1],10,10,1);//消除蛇身	
            ILI9341_DrawRectangle(snake2.X[snake2.Long-1],snake2.Y[snake2.Long-1],10,10,1);//消除蛇身						
				
				
				
				
				
					
					 LCD_SetTextColor								(GREEN  )	;
					//判断是否撞墙
					if(snake1.X[0]<0||snake1.X[0]>230||snake1.Y[0]<0||snake1.Y[0]>260)

						snake1.Life=1;//蛇死掉了
		
					//当蛇的身体超过3节后判断蛇自身的碰撞
					for(i=3;i<snake1.Long;i++)
					{
						if(snake1.X[i]==snake1.X[0]&&snake1.Y[i]==snake1.Y[0])//自身的任一坐标值与蛇头坐标相等就认为是自身碰撞
						{
							game.Life-=1;
						}
					}
					
					//判断是否撞墙
					if(snake2.X[0]<0||snake2.X[0]>230||snake2.Y[0]<0||snake2.Y[0]>260)

						snake2.Life=1;//蛇死掉了
		
					//当蛇的身体超过3节后判断蛇自身的碰撞
					for(i=3;i<snake2.Long;i++)
					{
						if(snake2.X[i]==snake2.X[0]&&snake2.Y[i]==snake2.Y[0])//自身的任一坐标值与蛇头坐标相等就认为是自身碰撞
						{
							game.Life-=1;
						}
					}
					
					
					
					
					if(snake1.Life==1||snake2.Life==1 || game.Life==0)//以上两种判断以后如果设死掉了跳出内循环，重新开始
					{
					gameover();
						
					Delay(0xFFFFFF);
					Delay(0xFFFFFF);//延时两秒后重新开始游戏
				  
						
					ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	
					break;
					}	
					
					
					
					
					//判断蛇是否吃到了食物
					if(snake1.X[0]==food.X&&snake1.Y[0]==food.Y)
					{ 
						
						LCD_SetTextColor								(BLACK  )	;
					//	ILI9341_DrawRectangle(food.X,food.Y,food.X+1,food.Y+1,1);//把吃到的食物消除
						
						ILI9341_DrawRectangle(food.X,food.Y,10,10,1);//把吃到的食物消除
						LCD_SetTextColor								(GREEN  )	;
						snake1.Long++;//蛇的身体长一节
						game.Score+=1;
						ChooseWords=rand()%5;
						//LCD_ShowNum(40,165,game.Score,3,16);//显示成绩	
						food.Yes=1;//需要重新显示食物
					}
						//LCD_ShowNum(224,165,game.Life,1,16);//显示生命值	
					
					
					//判断蛇是否吃到了食物
					if(snake2.X[0]==food.X&&snake2.Y[0]==food.Y)
					{ 
						
						LCD_SetTextColor								(BLACK  )	;
					//	ILI9341_DrawRectangle(food.X,food.Y,food.X+1,food.Y+1,1);//把吃到的食物消除
						
						ILI9341_DrawRectangle(food.X,food.Y,10,10,1);//把吃到的食物消除
						LCD_SetTextColor								(GREEN  )	;
						snake2.Long++;//蛇的身体长一节
						game.Score+=1;
						ChooseWords=rand()%5;
						//LCD_ShowNum(40,165,game.Score,3,16);//显示成绩	
						food.Yes=1;//需要重新显示食物
					}
	}	
}


int main ( void )
{

	
	
	ILI9341_GramScan ( 6 );
    
	
  ILI9341_Init ();
  LED_GPIO_Config();
	USART_Config();	
  Key_GPIO_Config();
  SysTick_Init();
  LCD_SetFont(&Font8x16);
	
	LCD_SetColors(RED,BLACK);
  EXTI_Key_Config();
  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	while ( 1 )
	{ 
	 
	   
  ILI9341_DrawRectangle(0,0,240,270,0);
   
		
	 printf("test\n");
   play();


	}

}


static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}
