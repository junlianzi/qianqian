#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "usart3.h"
#include "dht11.h"
#include "oled.h"
#include "gizwits_product.h" 
/************************************************
硬件连接
LED
红  PB15
黄  PB14
绿  PB13
白  PB12

WIFI
TXD  PB11
RXD  PB10
RST  PB1
TO_O PB0

OLED
D0   PA7 
D1   PA6
RES  PA5
DC   PA4

DHT11
COUT PA0
************************************************/


/* 用户区当前设备状态结构体*/
//dataPoint_t currentDataPoint;

//WIFI连接状态
//wifi_sta 0: 断开
//         1: 已连接
u8 wifi_sta=0;

//协议初始化
void Gizwits_Init(void)
{	
	TIM3_Int_Init(9,7199);//1MS系统定时
    usart3_init(9600);//WIFI初始化
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//设备状态结构体初始化
	gizwitsInit();//缓冲区初始化
}




//数据采集
void userHandle(void)
{
    static u8 t=0;
	static u8 temp,hum;
    static u8 temp_num=0;
	static u8 humi_num=0;
	if(wifi_sta)
	{
		 if(t==10)//每2S读取一次
		 {
			 t=0;
			 DHT11_Read_Data(&temp,&hum);//读取DHT11传感器
			 OLED_ShowNum(80,0,temp,3,16);           //显示CPU使用率的整数部分 
 		     OLED_ShowNum(74,2,hum,3,16);           //显示CPU使用率的小数部分 
//			 printf("温度 %d C \n",temp);
//			 delay_ms(100);
//			 printf("湿度 %d %% \n",hum);
			 delay_ms(100);
			 currentDataPoint.valuetemperature = temp ;//温度数据
			 currentDataPoint.valuehumidity = hum;//湿度数据 		
		 }
		  t++;
	}
	else
	{
		if(temp_num!=0||humi_num!=0) 
		{
			temp_num=0;humi_num=0;
		}
	}
	
	
}
//主函数
 int main(void)
 {		
	 
	u8 wifi_con=0;//记录wifi连接状态 1:连接 0:断开
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	    //串口初始化为115200
 	LED_Init();			    //LED端口初始化
	//KEY_Init();             //按键初始化  
	Gizwits_Init();         //协议初始化
	OLED_Init();		//初始化OLED
	while(DHT11_Init())	//DHT11初始化	
	{
		LED0=0;
		delay_ms(200);
		LED0=1;
 		delay_ms(200);
	}
	LED0=0;
	OLED_ShowString(0,0,"Temperature");        //先在固定位置显示Temperature
	OLED_ShowString(110,0,"C");        //先在固定位置显示C 
	OLED_ShowString(0,2,"Humidity");        //先在固定位置显示Humidity
	OLED_ShowString(100,2,"%RH");        //先在固定位置显示%RH 
	 
	
	printf("--------机智云IOT-DHT11温湿度检测报警实验----------\r\n");
	printf("KEY1:AirLink连接模式\t KEY_UP:复位\r\n\r\n");
	
	printf("WIFI复位，请重新配置连接\r\n");
	gizwitsSetMode(WIFI_RESET_MODE);//WIFI复位	
	wifi_sta=0;//标志wifi已断开
	printf("WIFI进入AirLink连接模式\r\n");
	gizwitsSetMode(WIFI_AIRLINK_MODE);//Air-link模式接入
//	delay_ms(200);
	
   	while(1)
	{
	   if(wifi_con!=wifi_sta)
	   {
		   wifi_con=wifi_sta;
		   
	   }
	   userHandle();//用户采集
         
       gizwitsHandle((dataPoint_t *)&currentDataPoint);//协议处理
 		
	   delay_ms(200);
	   LED0!=LED0;
	}	 

} 
 
