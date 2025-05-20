#ifndef __DS18B20_H
#define __DS18B20_H

#include "main.h"

//IO��������
#define DS18B20_IO_IN()  {GPIOA->CRH &= ~(0xFUL << 28); GPIOA->CRH |= (8UL << 28);}  // ��������
#define DS18B20_IO_OUT() {GPIOA->CRH &= ~(0xFUL << 28); GPIOA->CRH |= (3UL << 28);}  // �������

////IO��������											   
#define DS18B20_DQ_OUT PAout(15)  // ������� PA5
#define DS18B20_DQ_IN  PAin(15)   // �������� PA5
   	
u8 DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20 
#endif
