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
Ӳ������
LED
��  PB15
��  PB14
��  PB13
��  PB12

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


/* �û�����ǰ�豸״̬�ṹ��*/
//dataPoint_t currentDataPoint;

//WIFI����״̬
//wifi_sta 0: �Ͽ�
//         1: ������
u8 wifi_sta=0;

//Э���ʼ��
void Gizwits_Init(void)
{	
	TIM3_Int_Init(9,7199);//1MSϵͳ��ʱ
    usart3_init(9600);//WIFI��ʼ��
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//�豸״̬�ṹ���ʼ��
	gizwitsInit();//��������ʼ��
}




//���ݲɼ�
void userHandle(void)
{
    static u8 t=0;
	static u8 temp,hum;
    static u8 temp_num=0;
	static u8 humi_num=0;
	if(wifi_sta)
	{
		 if(t==10)//ÿ2S��ȡһ��
		 {
			 t=0;
			 DHT11_Read_Data(&temp,&hum);//��ȡDHT11������
			 OLED_ShowNum(80,0,temp,3,16);           //��ʾCPUʹ���ʵ��������� 
 		     OLED_ShowNum(74,2,hum,3,16);           //��ʾCPUʹ���ʵ�С������ 
//			 printf("�¶� %d C \n",temp);
//			 delay_ms(100);
//			 printf("ʪ�� %d %% \n",hum);
			 delay_ms(100);
			 currentDataPoint.valuetemperature = temp ;//�¶�����
			 currentDataPoint.valuehumidity = hum;//ʪ������ 		
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
//������
 int main(void)
 {		
	 
	u8 wifi_con=0;//��¼wifi����״̬ 1:���� 0:�Ͽ�
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	    //���ڳ�ʼ��Ϊ115200
 	LED_Init();			    //LED�˿ڳ�ʼ��
	//KEY_Init();             //������ʼ��  
	Gizwits_Init();         //Э���ʼ��
	OLED_Init();		//��ʼ��OLED
	while(DHT11_Init())	//DHT11��ʼ��	
	{
		LED0=0;
		delay_ms(200);
		LED0=1;
 		delay_ms(200);
	}
	LED0=0;
	OLED_ShowString(0,0,"Temperature");        //���ڹ̶�λ����ʾTemperature
	OLED_ShowString(110,0,"C");        //���ڹ̶�λ����ʾC 
	OLED_ShowString(0,2,"Humidity");        //���ڹ̶�λ����ʾHumidity
	OLED_ShowString(100,2,"%RH");        //���ڹ̶�λ����ʾ%RH 
	 
	
	printf("--------������IOT-DHT11��ʪ�ȼ�ⱨ��ʵ��----------\r\n");
	printf("KEY1:AirLink����ģʽ\t KEY_UP:��λ\r\n\r\n");
	
	printf("WIFI��λ����������������\r\n");
	gizwitsSetMode(WIFI_RESET_MODE);//WIFI��λ	
	wifi_sta=0;//��־wifi�ѶϿ�
	printf("WIFI����AirLink����ģʽ\r\n");
	gizwitsSetMode(WIFI_AIRLINK_MODE);//Air-linkģʽ����
//	delay_ms(200);
	
   	while(1)
	{
	   if(wifi_con!=wifi_sta)
	   {
		   wifi_con=wifi_sta;
		   
	   }
	   userHandle();//�û��ɼ�
         
       gizwitsHandle((dataPoint_t *)&currentDataPoint);//Э�鴦��
 		
	   delay_ms(200);
	   LED0!=LED0;
	}	 

} 
 
