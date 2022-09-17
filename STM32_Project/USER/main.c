#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "dht11.h" 
#include "adc.h"
#include "lsens.h"
#include "beep.h"
 
 extern u8 res;
 int main(void)
 {	 
	u8 t=0;			    
	u8 temperature;  	    
	u8 humidity; 
	u8 adcx ; 

	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	LED_Init();		  		//初始化与LED
	BEEP_Init();   //初始化蜂鸣器
	LCD_Init();		 //初始化LCD  
	Lsens_Init();  //初始化光敏传感器
	
 	POINT_COLOR=RED;		//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"GreenHouse_IoT");	
	LCD_ShowString(30,70,200,16,16,"Du Kang");	
	LCD_ShowString(30,90,200,16,16,"180690203");
	LCD_ShowString(30,110,200,16,16,"2022/5/01");		 
	 
 	while(DHT11_Init())	//DHT11初始化	
	{
		LCD_ShowString(30,130,200,16,16,"DHT11 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}		
	
	LCD_ShowString(30,130,200,16,16,"DHT11 OK");
	POINT_COLOR=BLUE;//设置字体为蓝色 
 	LCD_ShowString(30,150,200,16,16,"Temp:  C");	 
 	LCD_ShowString(30,170,200,16,16,"Humi:  %");
	LCD_ShowString(30,190,200,16,16,"LSENS_VAL:  Lux");
	
	while(1)
	{	    	    
 		if(t%10==0)			//每100ms读取一次
		{			
			
			char env[60];
			
			DHT11_Read_Data(&temperature,&humidity);	//读取温湿度值	
			adcx=Lsens_Get_Val();
			
			LCD_ShowNum(30+40,150,temperature,2,16);	//显示温度			
			LCD_ShowNum(30+40,170,humidity,2,16);		//显示湿度				
			LCD_ShowxNum(30+10*8,190,adcx,2,16,0);//显示ADC的值
			
			sprintf(env,"Temperature:%dC\r\n",temperature);
			USART_SendString(USART1,env);					
			sprintf(env,"Humidity:%d%%\r\n",humidity);
			USART_SendString(USART1,env);					
			sprintf(env,"LSENS_VAL:%dLux \r\n",adcx);
			USART_SendString(USART1,env);
		

		}
		delay_ms(100);
		t++;
		if(t==10)
		{
			t=0;
		}
	}
}




