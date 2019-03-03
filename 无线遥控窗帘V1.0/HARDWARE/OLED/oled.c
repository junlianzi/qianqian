/********************************************************
  * @file        	oled.c
  * @author      	LGG
  * @version     	V1.0.0
  * @data        	2017��06��6��
  * @brief       	OELD�ļ�
  *
********************************************************/


/*includes----------------------------------------------*/
#include "oled.h"
#include "oledfont.h" 
/*definition--------------------------------------------*/


/*
	@brief			�ӳ�1us
	@param			��
	@retval			��
 */
static void delay(unsigned char num)
{
	uint8_t i = 10;
	while(num--)
	{
		while(i--);
	}
}


/*
	@brief			ms�ӳٺ���
	@param			��
	@retval			��
*/
static void delay_ms(unsigned int ms)//�ӳٺ�����MS����
{
	unsigned int x,y;
	for(x = ms;x>0;x--)
 {
		for(y = 12000;y>0;y--);
 }
}


/*
	@brief			��ʼ��OLED�뵥Ƭ����IO�ӿ�
	@param			��
	@retval			��
 */
static void OLED_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	//����һ��GPIO_InitTypeDef���͵Ľṹ��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��A�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //��ʼ��GPIOA
 	GPIO_SetBits(GPIOA, GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);	
	
	
}


/*
	@brief			ģ��SPI����һ���ֽ�
	@param			data�����͵�����
	@retval			��
 */
static void SPI_Write_Byte(unsigned char data)
{
	unsigned char i;	//�������
	for(i = 0; i < 8; i++)	//ѭ��8��
	{
		SPI_SCK_LOW;	//��ʱ��������
		delay(1);	//�ӳ�
		
		if(data & 0x80)	//���ݴӸ�λ-->��λ���η���
			SPI_MOSI_HIGH;	//����ΪΪ1
		else
			SPI_MOSI_LOW;	//����λΪ0
			
		data <<= 1;	//��������1λ
		
		delay(1);	//�ӳ�
		SPI_SCK_HIGH;	//ʱ�������ߣ������ݷ��ͳ�ȥ
		delay(1);	//�ӳ�
	}
	
}


/*
	@brief			��OLEDд��һ���ֽ�
	@param			dat������
					cmd��1��д�����ݣ�0��д������
	@retval			��
 */
void OLED_WR_Byte(unsigned char dat,unsigned char cmd)
{
	if(cmd)  //���cmdΪ�ߣ����͵�������
	  	OLED_DC_HIGH;	//��DC����
	else    //���cmdΪ�ͣ����͵�������
		OLED_DC_LOW;	//��DC����
		
	SPI_CS_LOW; //Ƭѡ���ͣ�ѡͨ����
		
	SPI_Write_Byte(dat); //��������
		
	SPI_CS_HIGH; //Ƭѡ���ߣ��ر�����
	OLED_DC_HIGH; //DC���ߣ�����ʱΪ�ߵ�ƽ
}


/*
	@brief			��������д�����ʼ�С���
	@param			x�� �е���ʼ�͵�ַ����ʼ�ߵ�ַ��0x00~0x0f��������ʼ�е͵�ַ����ҳѰַģʽ����
						0x10~0x1f��������ʼ�иߵ�ַ����ҳѰַģʽ��
					y����ʼҳ��ַ 0~7
	@retval			��
 */
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);//д��ҳ��ַ
	OLED_WR_Byte((x&0x0f),OLED_CMD);  //д���еĵ�ַ  �Ͱ��ֽ�
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);//д���еĵ�ַ �߰��ֽ�
}   	     	  


/*
	@brief			����ʾ
	@param			��
	@retval			��
 */ 
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //����OLED��ɱ�
	OLED_WR_Byte(0X14,OLED_CMD);  //ʹ�ܣ���
	OLED_WR_Byte(0XAF,OLED_CMD);  //����ʾ
}


/*
	@brief			����ʾ
	@param			��
	@retval			��
 */  
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0XAE,OLED_CMD);  //����ʾ
	OLED_WR_Byte(0X8D,OLED_CMD);  //����OLED��ɱ�
	OLED_WR_Byte(0X10,OLED_CMD);  //ʧ�ܣ���
}		   			 


/*
	@brief			����
	@param			��
	@retval			��
 */	  
void OLED_Clear(void)  
{  
	unsigned char i,n;		    //�������
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //��0~7ҳ����д��
		OLED_WR_Byte (0x00,OLED_CMD);      //�е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //�иߵ�ַ  
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); //д�� 0 ����
	}
}


/*
	@brief			��ʾһ���ַ�
	@param			x����ʼ��
					y����ʼҳ��SIZE = 16ռ��ҳ��SIZE = 12ռ1ҳ
					chr���ַ�
	@retval			��
 */
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr)
{      	
	unsigned char c=0,i=0;	
		c=chr-' '; //��ȡ�ַ���ƫ����	
		if(x>Max_Column-1){x=0;y=y+2;} //������������˷�Χ���ʹ���2ҳ�ĵ�0�п�ʼ

		if(SIZE ==16) //�ַ���С���Ϊ 16 = 8*16
			{
				OLED_Set_Pos(x,y);	//��x y ��ʼ����
				for(i=0;i<8;i++)  //ѭ��8�� ռ8��
				OLED_WR_Byte(F8X16[c*16+i],OLED_DATA); //�ҳ��ַ� c ������λ�ã����ڵ�һҳ���л���
				OLED_Set_Pos(x,y+1); //ҳ����1
				for(i=0;i<8;i++)  //ѭ��8��
				OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA); //�ѵڶ�ҳ����������
			}
		else 	//�ַ���СΪ 6 = 6*8
			{	
				OLED_Set_Pos(x,y+1); //һҳ�Ϳ��Ի���
				for(i=0;i<6;i++) //ѭ��6�� ��ռ6��
				OLED_WR_Byte(F6x8[c][i],OLED_DATA); //���ַ�����
			}
}


/*
	@brief			����m^n
	@param			m�������һ����
					n���������Ĵη�
	@retval			result��һ������n�η�
 */
unsigned int oled_pow(unsigned char m,unsigned char n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}				  


/*
	@brief			��ָ����λ�ã���ʾһ��ָ�����ȴ�С����
	@param			x����ʼ��
					y����ʼҳ
					num������
					len�����ֵĳ���
					size����ʾ���ֵĴ�С
	@retval			��
 */		  
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size)
{         	
	unsigned char t,temp;  //�������
	unsigned char enshow=0;		//�������

	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;//ȡ����������ÿ��λ���ɸߵ���
		if(enshow==0&&t<(len-1)) //enshow���Ƿ�Ϊ��һ������t<(len-1)���ж��Ƿ�Ϊ���һ����
		{
			if(temp==0) //�������Ϊ0 
			{
				OLED_ShowChar(x+(size/2)*t,y,' ');//��ʾ 0 ��x+(size2/2)*t���������Сƫ�Ƶ�������8��
				continue; //����ʣ����䣬�����ظ�ѭ���������ظ���ʾ��
			}else enshow=1; 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0'); //��ʾһ��λ��x+(size2/2)*t���������Сƫ�Ƶ�������8��
	}
} 


/*
	@brief			��ʾ�ַ���
	@param			x����ʼ��
					y����ʼҳ
					*chr����һ���ַ��׵�ַ
	@retval			��
 */
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr)
{
	unsigned char j=0; //�������

	while (chr[j]!='\0') //����������һ���ַ�
	{		
		OLED_ShowChar(x,y,chr[j]); //��ʾ�ַ�
		x+=8; //������8 ��һ���ַ�������ռ8
		if(x>=128){x=0;y+=2;} //���x���ڵ���128���л�ҳ���Ӹ�ҳ�ĵ�һ����ʾ
		j++; //��һ���ַ�
	}
}


/*
	@brief			��ʾ����
	@param			x����ʼ�У�һ������ռ16��
					y����ʼҳ��һ������ռ��ҳ
					no����������
	@retval			��
 */
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no)
{      			    
	unsigned char t,adder=0; //�������

	OLED_Set_Pos(x,y);	//�� x y ��ʼ���㣬�Ȼ���һҳ
    for(t=0;t<16;t++) //ѭ��16�Σ�����һҳ��16��
		{
			OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);//��no������λ�õĵ�һҳ16�еĵ�
			adder+=1; //�����ַ��1
     	}	
		OLED_Set_Pos(x,y+1); //���ڶ�ҳ
    for(t=0;t<16;t++)//ѭ��16�Σ����ڶ�ҳ��16��
		{	
			OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);//��no������λ�õĵڶ�ҳ16�еĵ�
			adder+=1;//�����ַ��1
        }					
}


/*
	@brief			��ʾͼƬ
	@param			x0����ʼ�е�ַ
					y0����ʼҳ��ַ
					x1����ֹ�е�ַ
					y1����ֹҳ��ַ
					BMP[]�����ͼƬ���������
	@retval			��
 */
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 	unsigned int j=0; //�������
 	unsigned char x,y; //�������
  
 	if(y1%8==0) y=y1/8;   //�ж���ֹҳ�Ƿ�Ϊ8��������
 	 else y=y1/8+1;

		for(y=y0;y<y1;y++) //����ʼҳ��ʼ��������ֹҳ
		{
			OLED_Set_Pos(x0,y); //��ҳ����ʼ�п�ʼ��
   			for(x=x0;x<x1;x++) //��x1 - x0 ��
	    		{
	    			OLED_WR_Byte(BMP[j++],OLED_DATA);	//��ͼƬ�ĵ�    	
	    		}
		}
} 


/*
	@brief			OLED��ʼ������
	@param			��
	@retval			��
 */				    
void OLED_Init(void)
{
	OLED_GPIO_Init();	//GPIO�ڳ�ʼ��
 
	OLED_RES_HIGH;
	delay_ms(100);
	OLED_RES_LOW;
	delay_ms(200);	//�ӳ٣����ڵ�Ƭ���ϵ��ʼ����OLED�죬���Ա�������ӳ٣��ȴ�OLED�ϵ��ʼ�����
	OLED_RES_HIGH;
	delay_ms(200);

	OLED_WR_Byte(0xAE,OLED_CMD);	//�ر���ʾ
	OLED_WR_Byte(0x2e,OLED_CMD);	//�رչ���

	OLED_WR_Byte(0x00,OLED_CMD);	//���õ��е�ַ
	OLED_WR_Byte(0x10,OLED_CMD);	//���ø��е�ַ
	OLED_WR_Byte(0x40,OLED_CMD);	//������ʼ�е�ַ
	OLED_WR_Byte(0xB0,OLED_CMD);	//����ҳ��ַ

	OLED_WR_Byte(0x81,OLED_CMD);	// �Աȶ����ã�����������
	OLED_WR_Byte(0xFF,OLED_CMD);	//  265  

	OLED_WR_Byte(0xA1,OLED_CMD);	//���öΣ�SEG������ʼӳ���ַ��column��127��ַ��SEG0�ĵ�ַ
	OLED_WR_Byte(0xA6,OLED_CMD);	//������ʾ��0xa7����ʾ

	OLED_WR_Byte(0xA8,OLED_CMD);	//��������·��
	OLED_WR_Byte(0x3F,OLED_CMD);	//1/64duty
	
	OLED_WR_Byte(0xC8,OLED_CMD);	//��ӳ��ģʽ��COM[N-1]~COM0ɨ��

	OLED_WR_Byte(0xD3,OLED_CMD);	//������ʾƫ��
	OLED_WR_Byte(0x00,OLED_CMD);	//��ƫ��
	
	OLED_WR_Byte(0xD5,OLED_CMD);	//����������Ƶ��Ĭ�ϣ�
	OLED_WR_Byte(0x80,OLED_CMD);	
	
	OLED_WR_Byte(0xD8,OLED_CMD);	//���� area color mode off��û�У�
	OLED_WR_Byte(0x05,OLED_CMD);
	
	OLED_WR_Byte(0xD6,OLED_CMD);	//�Ŵ���ʾ
	OLED_WR_Byte(0x00,OLED_CMD);
	
	OLED_WR_Byte(0xD9,OLED_CMD);	//���� Pre-Charge Period��Ĭ�ϣ�
	OLED_WR_Byte(0xF1,OLED_CMD);
	
	OLED_WR_Byte(0xDA,OLED_CMD);	//���� com pin configuartion��Ĭ�ϣ�
	OLED_WR_Byte(0x12,OLED_CMD);
	
	OLED_WR_Byte(0xDB,OLED_CMD);	//���� Vcomh���ɵ������ȣ�Ĭ�ϣ�
	OLED_WR_Byte(0x30,OLED_CMD);
	
	OLED_WR_Byte(0x8D,OLED_CMD);	//����OLED��ɱ�
	OLED_WR_Byte(0x14,OLED_CMD);	//����ʾ
	
	OLED_WR_Byte(0xA4,OLED_CMD);	// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);	// Disable Inverse Display On (0xa6/a7) 
	
	OLED_WR_Byte(0xAF,OLED_CMD);	//����OLED�����ʾ
	OLED_Clear();	//����
	OLED_Set_Pos(0,0); 	 //����
}  


/*
	@brief			OLED������������Χ0~1ҳ��ˮƽ����
	@param			��
	@retval			��
 */	
void OLED_Scroll(void)
{
	OLED_WR_Byte(0x2E,OLED_CMD);	//�رչ���
	OLED_WR_Byte(0x27,OLED_CMD);	//ˮƽ�������
	OLED_WR_Byte(0x00,OLED_CMD);	//�����ֽ�
	OLED_WR_Byte(0x00,OLED_CMD);	//��ʼҳ 0
	OLED_WR_Byte(0x00,OLED_CMD);	//����ʱ����
	OLED_WR_Byte(0x01,OLED_CMD);	//��ֹҳ 1
	OLED_WR_Byte(0x00,OLED_CMD);	//�����ֽ�
	OLED_WR_Byte(0xFF,OLED_CMD);	//�����ֽ�
	OLED_WR_Byte(0x2F,OLED_CMD);	//��������
}

