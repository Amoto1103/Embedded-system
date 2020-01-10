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
#define SNAKE_Max_Long 60//�ߵ���󳤶�
static void Delay ( __IO uint32_t nCount );
//�߽ṹ��
struct Snake
{
	uint16_t X[SNAKE_Max_Long];
	uint16_t Y[SNAKE_Max_Long];
	u8 Long;//�ߵĳ���
	u8 Life;//�ߵ����� 0���� 1����
	u8 Direction;//���ƶ��ķ���
}snake1,snake2;

//ʳ��ṹ��
struct Food
{
	u8 X;//ʳ�������
	u8 Y;//ʳ��������
	u8 Yes;//�ж��Ƿ�Ҫ����ʳ��ı��� 0��ʳ�� 1��Ҫ����ʳ��
}food;

//��Ϸ�ȼ�����
struct Game
{
	u16 Score;//����
	u8 Life;//��Ϸ�ȼ�	
}game;

void KEY1_IRQHandler(void)
{
  //ȷ���Ƿ������EXTI Line�ж�
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
  //ȷ���Ƿ������EXTI Line�ж�
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

void USART1_IRQHandler(u8 GetData)//�����ж�
{

if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		GetData=USART1->DR;
		mk=GetData;
		printf("�յ������ݣ�\n");
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

//��Ϸ����
void gameover()
	{
		   LCD_SetTextColor								(RED )	;
       ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	
		   sprintf(dispBuff,"        FUCK FOOD : %d",game.Score);
		   ILI9341_DispStringLine_EN(LINE(10),dispBuff);
			 ILI9341_DispStringLine_EN(LINE(8),"       YOU ARE FUCKED!");
		   
		}
		
//����Ϸ
void play()
{
	u16 i;	
	u16 ChooseWords;
	char *Words[6];
	ChooseWords=5;
	snake1.Long=2;//�����ߵĳ���
	snake1.Life=0;//�߻�����
	snake1.Direction=1;//�ߵ���ʼ������Ϊ��
	snake2.Long=2;//�����ߵĳ���
	snake2.Life=0;//�߻�����
	snake2.Direction=2;
	
	game.Score=0;//����Ϊ0
	game.Life=4;//�ߵ�����ֵ
	food.Yes=1;//������ʳ��
	
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
			if(food.Yes==1)//�����µ�ʳ��
			{
				//���趨����������ʾʳ��		

			    food.X=rand()%(220/10)*10+10;
					food.Y=rand()%(250/10)*10+10;

					food.Yes=0;
			}
				if(food.Yes==0)//��ʳ���Ҫ��ʾ
				{	
		
					 LCD_SetColors(GREEN,BLACK);
					 ILI9341_DrawRectangle  (food.X,food.Y,10,10,1);
				
				}
				
				//snake1
				//ȡ����Ҫ���»����ߵĽ���
				for(i=snake1.Long-1;i>0;i--)
				{
					snake1.X[i]=snake1.X[i-1];
					snake1.Y[i]=snake1.Y[i-1];
				}

				
	
				switch(snake1.Direction)
				{
					case 1:snake1.X[0]+=10;break;//�����˶�
					case 2:snake1.X[0]-=10;break;//�����˶�
					case 3:snake1.Y[0]-=10;break;//�����˶�
				
					case 4:snake1.Y[0]+=10;break;//�����˶�
					
					
				}
				LCD_SetColors(YELLOW,BLACK);
					for(i=0;i<snake1.Long;i++)//������	
						
				ILI9341_DrawRectangle(snake1.X[i],snake1.Y[i],10,10,1);//��������
				
				//snake2
				//ȡ����Ҫ���»����ߵĽ���
				for(i=snake2.Long-1;i>0;i--)
				{
					snake2.X[i]=snake2.X[i-1];
					snake2.Y[i]=snake2.Y[i-1];
				}

				
	
				switch(snake2.Direction)
				{
					case 1:snake2.X[0]+=10;break;//�����˶�
					case 2:snake2.X[0]-=10;break;//�����˶�
					case 3:snake2.Y[0]-=10;break;//�����˶�
				
					case 4:snake2.Y[0]+=10;break;//�����˶�
					
					
				}
				LCD_SetColors(BLUE,BLACK);
					for(i=0;i<snake2.Long;i++)//������	
						
				ILI9341_DrawRectangle(snake2.X[i],snake2.Y[i],10,10,1);//��������
				
				

				
					 Delay(0xFFFFF);
				
				  
				    LCD_SetTextColor								(BLACK )	;
						ILI9341_DrawRectangle(snake1.X[snake1.Long-1],snake1.Y[snake1.Long-1],10,10,1);//��������	
            ILI9341_DrawRectangle(snake2.X[snake2.Long-1],snake2.Y[snake2.Long-1],10,10,1);//��������						
				
				
				
				
				
					
					 LCD_SetTextColor								(GREEN  )	;
					//�ж��Ƿ�ײǽ
					if(snake1.X[0]<0||snake1.X[0]>230||snake1.Y[0]<0||snake1.Y[0]>260)

						snake1.Life=1;//��������
		
					//���ߵ����峬��3�ں��ж����������ײ
					for(i=3;i<snake1.Long;i++)
					{
						if(snake1.X[i]==snake1.X[0]&&snake1.Y[i]==snake1.Y[0])//�������һ����ֵ����ͷ������Ⱦ���Ϊ��������ײ
						{
							game.Life-=1;
						}
					}
					
					//�ж��Ƿ�ײǽ
					if(snake2.X[0]<0||snake2.X[0]>230||snake2.Y[0]<0||snake2.Y[0]>260)

						snake2.Life=1;//��������
		
					//���ߵ����峬��3�ں��ж����������ײ
					for(i=3;i<snake2.Long;i++)
					{
						if(snake2.X[i]==snake2.X[0]&&snake2.Y[i]==snake2.Y[0])//�������һ����ֵ����ͷ������Ⱦ���Ϊ��������ײ
						{
							game.Life-=1;
						}
					}
					
					
					
					
					if(snake1.Life==1||snake2.Life==1 || game.Life==0)//���������ж��Ժ������������������ѭ�������¿�ʼ
					{
					gameover();
						
					Delay(0xFFFFFF);
					Delay(0xFFFFFF);//��ʱ��������¿�ʼ��Ϸ
				  
						
					ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	
					break;
					}	
					
					
					
					
					//�ж����Ƿ�Ե���ʳ��
					if(snake1.X[0]==food.X&&snake1.Y[0]==food.Y)
					{ 
						
						LCD_SetTextColor								(BLACK  )	;
					//	ILI9341_DrawRectangle(food.X,food.Y,food.X+1,food.Y+1,1);//�ѳԵ���ʳ������
						
						ILI9341_DrawRectangle(food.X,food.Y,10,10,1);//�ѳԵ���ʳ������
						LCD_SetTextColor								(GREEN  )	;
						snake1.Long++;//�ߵ����峤һ��
						game.Score+=1;
						ChooseWords=rand()%5;
						//LCD_ShowNum(40,165,game.Score,3,16);//��ʾ�ɼ�	
						food.Yes=1;//��Ҫ������ʾʳ��
					}
						//LCD_ShowNum(224,165,game.Life,1,16);//��ʾ����ֵ	
					
					
					//�ж����Ƿ�Ե���ʳ��
					if(snake2.X[0]==food.X&&snake2.Y[0]==food.Y)
					{ 
						
						LCD_SetTextColor								(BLACK  )	;
					//	ILI9341_DrawRectangle(food.X,food.Y,food.X+1,food.Y+1,1);//�ѳԵ���ʳ������
						
						ILI9341_DrawRectangle(food.X,food.Y,10,10,1);//�ѳԵ���ʳ������
						LCD_SetTextColor								(GREEN  )	;
						snake2.Long++;//�ߵ����峤һ��
						game.Score+=1;
						ChooseWords=rand()%5;
						//LCD_ShowNum(40,165,game.Score,3,16);//��ʾ�ɼ�	
						food.Yes=1;//��Ҫ������ʾʳ��
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
