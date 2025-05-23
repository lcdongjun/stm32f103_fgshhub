#include "oled.h"
#include "oledfont.h"  	 
#include "spi.h"

uint8_t OLED_GRAM[2048];
volatile uint8_t spi_dma_busy = 0;

//���Ժ���
void OLED_ColorTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//������ʾ
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//��ɫ��ʾ
		}
}

//��Ļ��ת180��
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//������ʾ
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//��ת��ʾ
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

// DMA������ɻص�
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1)
    {
        spi_dma_busy = 0;
    }
}

// DMA ���������DMA������������ã������ֽڣ�
static void OLED_Write_Cmd(uint8_t cmd)
{
    OLED_DC_Clr();         // ����
    OLED_CS_Clr();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    OLED_CS_Set();
}


// DMA ����һ��ҳ���ݣ�128�ֽڣ�
static void OLED_Write_Data_DMA(uint8_t* pData, uint16_t len)
{
    OLED_DC_Set();         // ����
    OLED_CS_Clr();

    spi_dma_busy = 1;
    HAL_SPI_Transmit_DMA(&hspi1, pData, len);

    while (spi_dma_busy);  // �ȴ�������ɣ�������

    OLED_CS_Set();
}

void OLED_WR_Byte(u8 dat,u8 cmd)
{	
		if (cmd == OLED_DATA)
        OLED_DC_Set();  // ����
    else
        OLED_DC_Clr();  // ����

    OLED_CS_Clr();  // ѡ��OLED

    HAL_SPI_Transmit(&hspi1, &dat, 1, HAL_MAX_DELAY);  // SPI����һ���ֽ�

    OLED_CS_Set();  // ȡ��ѡ��
}

//����OLED��ʾ 
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//��ɱ�ʹ��
	OLED_WR_Byte(0x14,OLED_CMD);//������ɱ�
	OLED_WR_Byte(0xAF,OLED_CMD);//������Ļ
}

//�ر�OLED��ʾ 
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//��ɱ�ʹ��
	OLED_WR_Byte(0x10,OLED_CMD);//�رյ�ɱ�
	OLED_WR_Byte(0xAE,OLED_CMD);//�ر���Ļ
}

//�����Դ浽OLED	
void OLED_Refresh(void)
{
    for (uint8_t page = 0; page < 8; page++)
    {
        OLED_Write_Cmd(0xB0 + page);  // ҳ��ַ
        OLED_Write_Cmd(0x00);         // �е�
        OLED_Write_Cmd(0x10);         // �и�

        OLED_Write_Data_DMA(&OLED_GRAM[(page+4) * 128], 128);  // һҳ����
    }
}

//��������
void OLED_Clear(void)
{
		for (uint16_t i = 0; i < 2048; i++)
    {
        OLED_GRAM[i] = 0x00; // ��������Դ�
    }

    OLED_Refresh(); // ������ʾ
}

void OLED_Clear_GRAM(void)
{
		for (uint16_t i = 0; i < 2048; i++)
    {
        OLED_GRAM[i] = 0x00; // ��������Դ�
    }
}

void OLED_Clear_Area(uint8_t x_start, uint8_t y_start, uint8_t width, uint8_t height)
{
    if (x_start >= 128 || y_start >= 128 || width == 0 || height == 0)
        return;

    uint8_t x_end = x_start + width;
    uint8_t y_end = y_start + height;

    if (x_end > 128) x_end = 128;
    if (y_end > 128) y_end = 128;

    for (uint8_t y = y_start; y < y_end; y++)
    {
        uint8_t page = y / 8;
        uint8_t bit_mask = 1 << (y % 8);

        for (uint8_t x = x_start; x < x_end; x++)
        {
            OLED_GRAM[x + (page+4) * 128] &= ~bit_mask;
        }
    }
}

//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���	
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
		if (x > 127 || y > 127) return;  // ��ֹԽ��

    uint16_t page = y / 8;
    uint8_t bit_offset = y % 8;
    uint16_t index = page * 128 + x;

    if (t)
        OLED_GRAM[index] |= (1 << bit_offset);  // ��λ
    else
        OLED_GRAM[index] &= ~(1 << bit_offset); // ����
}

//����
//x1,y1:�������
//x2,y2:��������
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1;
	uRow=x1;//�����������
	uCol=y1;
	if(delta_x>0)incx=1; //���õ������� 
	else if (delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//ˮƽ�� 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint(uRow,uCol,mode);//����
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}
//x,y:Բ������
//r:Բ�İ뾶
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b,1);
        OLED_DrawPoint(x - a, y - b,1);
        OLED_DrawPoint(x - a, y + b,1);
        OLED_DrawPoint(x + a, y + b,1);
 
        OLED_DrawPoint(x + b, y + a,1);
        OLED_DrawPoint(x + b, y - a,1);
        OLED_DrawPoint(x - b, y - a,1);
        OLED_DrawPoint(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;//���㻭�ĵ���Բ�ĵľ���
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}



//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//size1:ѡ������ 6x8/6x12/8x16/12x24
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1, u8 mode) {
    u8 i, m, temp, size2, chr1;
    u8 x0 = x, y0 = y;
    
    if (size1 == 8) {
        size2 = 6;
    } else {
        size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2);
    }
    
    chr1 = chr - ' ';
    
    for (i = 0; i < size2; i++) {
        switch (size1) {
            case 8:
                temp = asc2_0806[chr1][i];
                break;
            case 12:
                temp = asc2_1206[chr1][i];
                break;
            case 16:
                temp = asc2_1608[chr1][i];
                break;
//            case 24:
//                temp = asc2_2412[chr1][i];
//                break;
            default:
                return;
        }
        
        for (m = 0; m < 8; m++) {
            if (temp & 0x01) {
                OLED_DrawPoint(x, y, mode);
            } else {
                OLED_DrawPoint(x, y, !mode);
            }
            temp >>= 1;
            y++;
        }
        
        x++;
        if ((size1 != 8) && ((x - x0) == size1 / 2)) {
            x = x0;
            y0 += 8;
        }
        y = y0;
    }
}


//��ʾ�ַ���
//x,y:�������  
//size1:�����С 
//*chr:�ַ�����ʼ��ַ 
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode)
{
	while((*chr>=' ')&&(*chr<='~'))//�ж��ǲ��ǷǷ��ַ�!
	{
		OLED_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
  }
}

//m^n
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}

//��ʾ����
//x,y :�������
//num :Ҫ��ʾ������
//len :���ֵ�λ��
//size:�����С
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode)
{
	u8 t,temp,m=0;
	if(size1==8)m=2;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
			}
  }
}

//��ʾ����
//x,y:�������
//num:���ֶ�Ӧ�����
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode)
{
//	u8 m,temp;
//	u8 x0=x,y0=y;
//	u16 i,size3=(size1/8+((size1%8)?1:0))*size1;  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
//	for(i=0;i<size3;i++)
//	{
//		if(size1==16)
//				{temp=Hzk1[num][i];}//����16*16����
//		else if(size1==24)
//				{temp=Hzk2[num][i];}//����24*24����
//		else if(size1==32)       
//				{temp=Hzk3[num][i];}//����32*32����
//		else if(size1==64)
//				{temp=Hzk4[num][i];}//����64*64����
//		else return;
//		for(m=0;m<8;m++)
//		{
//			if(temp&0x01)OLED_DrawPoint(x,y,mode);
//			else OLED_DrawPoint(x,y,!mode);
//			temp>>=1;
//			y++;
//		}
//		x++;
//		if((x-x0)==size1)
//		{x=x0;y0=y0+8;}
//		y=y0;
//	}
}

//num ��ʾ���ֵĸ���
//space ÿһ����ʾ�ļ��
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode)
{
    uint8_t t = 0;
    uint8_t m = 0;
    uint16_t r;

    while (1)
    {
        if (m == 0)
        {
            OLED_ShowChinese(128, 24, t, 16, mode);  // ��GRAM��д����һ���֣����ң�
            t++;
        }

        if (t == num)
        {
            // �հ׹�����
            for (r = 0; r < 16 * space; r++)
            {
                // ���������Դ�
                for (uint16_t page = 0; page < 8; page++)
                {
                    for (uint16_t col = 1; col < 128; col++)
                    {
                        OLED_GRAM[page * 128 + col - 1] = OLED_GRAM[page * 128 + col];
                    }
                    OLED_GRAM[page * 128 + 127] = 0x00;  // ���������
                }
                OLED_Refresh();
            }
            t = 0;
        }

        m++;
        if (m == 16) m = 0;

        // ����������ʾ
        for (uint16_t page = 0; page < 8; page++)
        {
            for (uint16_t col = 1; col < 128; col++)
            {
                OLED_GRAM[page * 128 + col - 1] = OLED_GRAM[page * 128 + col];
            }
            OLED_GRAM[page * 128 + 127] = 0x00;
        }

        OLED_Refresh();
    }
}

//x,y���������
//sizex,sizey,ͼƬ����
//BMP[]��Ҫд���ͼƬ����
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,const unsigned char *BMP,u8 mode)
{
	u16 j=0;
	u8 i,n,temp,m;
	u8 x0=x,y0=y;
	sizey=sizey/8+((sizey%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		 for(i=0;i<sizex;i++)
		 {
				temp=BMP[j];
				j++;
				for(m=0;m<8;m++)
				{
					if(temp&0x01)OLED_DrawPoint(x,y,mode);
					else OLED_DrawPoint(x,y,!mode);
					temp>>=1;
					y++;
				}
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y0=y0+8;
				}
				y=y0;
     }
	 }
}
//OLED�ĳ�ʼ��
void OLED_Init(void)
{
	OLED_RES_Clr();
	HAL_Delay(200);
	OLED_RES_Set();
	
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_Clear();
	OLED_WR_Byte(0xAF,OLED_CMD);
}

