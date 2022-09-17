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

	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	LED_Init();		  		//��ʼ����LED
	BEEP_Init();   //��ʼ��������
	LCD_Init();		 //��ʼ��LCD  
	Lsens_Init();  //��ʼ������������
	
 	POINT_COLOR=RED;		//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"GreenHouse_IoT");	
	LCD_ShowString(30,70,200,16,16,"Du Kang");	
	LCD_ShowString(30,90,200,16,16,"180690203");
	LCD_ShowString(30,110,200,16,16,"2022/5/01");		 
	 
 	while(DHT11_Init())	//DHT11��ʼ��	
	{
		LCD_ShowString(30,130,200,16,16,"DHT11 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}		
	
	LCD_ShowString(30,130,200,16,16,"DHT11 OK");
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
 	LCD_ShowString(30,150,200,16,16,"Temp:  C");	 
 	LCD_ShowString(30,170,200,16,16,"Humi:  %");
	LCD_ShowString(30,190,200,16,16,"LSENS_VAL:  Lux");
	
	while(1)
	{	    	    
 		if(t%10==0)			//ÿ100ms��ȡһ��
		{			
			
			char env[60];
			
			DHT11_Read_Data(&temperature,&humidity);	//��ȡ��ʪ��ֵ	
			adcx=Lsens_Get_Val();
			
			LCD_ShowNum(30+40,150,temperature,2,16);	//��ʾ�¶�			
			LCD_ShowNum(30+40,170,humidity,2,16);		//��ʾʪ��				
			LCD_ShowxNum(30+10*8,190,adcx,2,16,0);//��ʾADC��ֵ
			
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




