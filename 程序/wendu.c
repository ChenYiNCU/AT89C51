#include "reg52.h"  
//---�ض���ؼ���---//
#define uchar unsigned char
#define uint unsigned int


//--����ʹ�õ�IO��--//
sbit DQ =P3^6;
sbit BEEP=P3^7;               //���������   
sbit ALAM=P1^3;                //����ƹⱨ��
sbit SET=P3^1;                 //���������
sbit ADD=P3^3;
sbit DEC=P3^2;

bit shanshuo_st;                            //��˸�����־
bit beep_st;                                     //�����������־
char shangxian=38;                  //���ޱ����¶ȣ�Ĭ��ֵΪ38
char xiaxian=-6;                   //���ޱ����¶ȣ�Ĭ��ֵΪ-6
uchar set_st=0;                             //״̬��־

uchar m;                    //�¶�ֵȫ�ֱ�����������
char Signed_Current_temp;
uchar n;                             //�¶�ֵȫ�ֱ�����С����

char num=0;
uchar DisplayData[8];

uchar code DSY_CODE[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
uchar data DSY_IDX[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
uchar code df_table[]={0,1,1,2,3,3,4,4,5,6,6,7,8,8,9,9};
void Alarm();

/*******************************************************************************
* �� �� ��         : delay
* ��������		   : ��ʱ������i=1ʱ����Լ��ʱ10us
*******************************************************************************/
void delay(uint i)
{
	while(i--);	
}

void Delay1ms(uint y)
{
	uint x;
	for( ; y>0; y--)
	{
		for(x=110; x>0; x--);
	}
}

void datapros(void) 	 
{
   
	
	DisplayData[1] = DSY_CODE[m/10];
	DisplayData[2] = DSY_CODE[m%10]&0x7F;
	DisplayData[3] = DSY_CODE[n];

}
//=====================================================================================
//=====================================================================================
//=====================================================================================

/*****��ʱ�ӳ���*****/
void Delay_DS18B20(int num)
{
  while(num--) ;
}
/*****��ʼ��DS18B20*****/
void Init_DS18B20(void)
{
  unsigned char x=0;
  DQ = 1;         //DQ��λ
  Delay_DS18B20(28);    //������ʱ
  DQ = 0;         //��Ƭ����DQ����
  Delay_DS18B20(80);   //��ȷ��ʱ������480us
  DQ = 1;         //��������
  Delay_DS18B20(14);
  x = DQ;           //������ʱ�����x=0���ʼ���ɹ���x=1���ʼ��ʧ��
  Delay_DS18B20(20);
}
/*****��һ���ֽ�*****/
unsigned char ReadOneChar(void)
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;     // �������ź�
    dat>>=1;
    DQ = 1;     // �������ź�
    if(DQ)
    dat|=0x80;
    Delay_DS18B20(4);
  }
  return(dat);
}
/*****дһ���ֽ�*****/
void WriteOneChar(unsigned char dat)
{
  unsigned char i=0;
  for (i=8; i>0; i--)
  {
    DQ = 0;
    DQ = dat&0x01;
    Delay_DS18B20(5);
    DQ = 1;
    dat>>=1;
  }
}
/*****��ȡ�¶�*****/
void ReadTemperature(void)
{
  unsigned char a=0;
  unsigned char b=0;
  //unsigned int t=0;

  uchar ng=0;
  //float tt=0;
  Init_DS18B20();
  WriteOneChar(0xCC);  //����������кŵĲ���
  WriteOneChar(0x44);  //�����¶�ת��
  	Delay1ms(15);

  Init_DS18B20();
  WriteOneChar(0xCC);  //����������кŵĲ���
  WriteOneChar(0xBE);  //��ȡ�¶ȼĴ���
  a=ReadOneChar();     //����8λ
  b=ReadOneChar();    //����8λ
    if((b&0XF8)==0XF8)
   {
      b=~b;
      a=~a+1;
      if(a==0X00)b++;
      ng=1;               //Ϊ����
     
   }
    n=df_table[a&0X0F];
   if(ng==1)
   {
       DisplayData[0] = 0xbf;
   }
   else
   {
      DisplayData[0] = 0xff;
   }
   m=((a&0Xf0)>>4)|((b&0X07)<<4);
   Signed_Current_temp=ng?-m:m;
  

  //t=b;                                   //��8λת�Ƶ�t
 // t<<=8;                           //t��������8λ
 // t=t|a;                           //��t��a��λ�򣬵õ�һ��16λ����
  //tt=t*0.0625;                   //��t����0.0625�õ�ʵ���¶�ֵ���¶ȴ���������12λ���ȣ���С�ֱ�����0.0625��
 // t= tt*10+0.5;     //�Ŵ�10������С�����һλ��ʾ�������������������
  //return(t);                   //�����¶�ֵ
}

/*****��ȡ�¶�*****/
void check_wendu(void)
{
    //uint a,b,c,i;
     uint i;
    for( i=2;i>0;i--){          //�ظ�3��
	     ReadTemperature();                          //��ȡ�¶�ֵ
        //a=c/100;                                                     //����õ�ʮλ����
        //b=c/10-a*10;                                            //����õ���λ����
       // m=c/10;                                                      //����õ�����λ
       // n=c-a*100-b*10;                                    //����õ�С��λ
       // if(m<0){m=0;n=0;}                                   //�����¶���ʾ����
        if(m>99){m=99;n=9;}                                   //�����¶���ʾ����    
	 } 
}

/*******************************************************************************
* ������         :DigDisplay()
* ��������		 :�������ʾ����
* ����           : ��
* ���         	 : ��
*******************************************************************************/
void DigDisplay()
{

	uint i;
	P0=0xff;
	P2=0x00;
	for(i=30;i>0;i--){
	
				P2=DSY_IDX[0];
					P0=DisplayData[0];//��������
		delay(150); //���һ��ʱ��ɨ��	
		P0=0xff;//����
	
			P2=DSY_IDX[1]; 
				P0=DisplayData[1];//��������
		delay(150); //���һ��ʱ��ɨ��	
		P0=0xff;//����
		
				P2=DSY_IDX[2];
					P0=DisplayData[2];//��������
		delay(150); //���һ��ʱ��ɨ��	
		P0=0xff;//����
			
				P2=DSY_IDX[3];
					P0=DisplayData[3];//��������
		delay(250); //���һ��ʱ��ɨ��	
		P0=0xff;//����
	
	 Alarm();   //�������
	}
 

}

/*****�����ӳ���*****/
void Alarm()
{
        float tem,xiaoshu;
		  tem=Signed_Current_temp;
		  xiaoshu=n;
		  xiaoshu=xiaoshu*0.1;
        if(((tem+xiaoshu)>=shangxian)||((tem-xiaoshu)<xiaxian))
        {
		  
				 BEEP=!BEEP;
                ALAM=0;
               
        }
        else
        {
                BEEP=1;
                ALAM=1;
        }
}

/*****��ʾ�����¶��ӳ���*****/
void Disp_alarm(char baojing)
{
	uint i;
	P0=0xff;
	P2=0x00;

   for(i=2;i>0;i--){
       P2=DSY_IDX[0];
      if(set_st==1)P0=0x89;       //����H
        else if(set_st==2)P0=0xC7; //����L��ʾ
        
        delay(100);
    	P0=0xff;//����

 P2=DSY_IDX[1];
 if(baojing<0)
   {
       P0 = 0xbf;
		 baojing=-baojing;
   }
   else
   {
      P0 = 0xff;

   }
 delay(100);
	 P0=0xff;//����

     P2=DSY_IDX[2];
     P0=DSY_CODE[baojing/10];
     delay(100);
	 P0=0xff;//����

     P2=DSY_IDX[3];
     P0=DSY_CODE[baojing%10];
     delay(200);
	 P0=0xff;//����

    }

}



/*******************************************************************************
* �� �� ��       : main
* ��������		 : ������
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void main()
{	
   EA=1;
  
  IT0=1;     //�ⲿ�жϵĴ�����ʽ���½��ش���      
   IT1=1;
		check_wendu();
		Delay1ms(900); 
		check_wendu();
		datapros();
	 BEEP=1;
    ALAM=1;
	while(1)
	{
        if(SET==0)
      {
          delay(2000);
          do{}while(SET==0);
          set_st++;shanshuo_st=1;
          if(set_st>2)set_st=0;
      }
       if(set_st==0)
      {
		 BEEP=1;
   
          EX0=0;    //�ر��ⲿ�ж�0
          EX1=0;    //�ر��ⲿ�ж�1
       	   check_wendu();
	    datapros();
    	DigDisplay();
        //Alarm();   //�������
       }
       else if(set_st==1)
      {
          BEEP=1;    //�رշ�����
          ALAM=1;
          EX0=1;    //�����ⲿ�ж�0
          EX1=1;    //�����ⲿ�ж�1
         
         if(shanshuo_st) {Disp_alarm(shangxian);}
      }
      else if(set_st==2)
      {
          BEEP=1;    //�رշ�����
          ALAM=1;
          EX0=1;    //�����ⲿ�ж�0
          EX1=1;    //�����ⲿ�ж�1
         
          if(shanshuo_st) {Disp_alarm(xiaxian);}
      }

   }

}



/*****�ⲿ�ж�0�������*****/
void int0(void) interrupt 0
{

       EX0=0;      //���ⲿ�ж�0
     if(DEC==0&&set_st==1)
      {
          do{
                Disp_alarm(shangxian);
        }while(DEC==0);
         
          shangxian--;
          if(shangxian<xiaxian)shangxian=xiaxian;
      }
      else if(DEC==0&&set_st==2)
     {
          do{
                Disp_alarm(xiaxian);
        }while(DEC==0);
        
         xiaxian--;
          //if(xiaxian<0)xiaxian=0;
     }
}
/*****�ⲿ�ж�1�������*****/
void int1(void) interrupt 2
{
       EX1=0;      //���ⲿ�ж�1
     if(ADD==0&&set_st==1)
    {
         do{
                Disp_alarm(shangxian);
        }
         while(ADD==0);
          shangxian++;
          if(shangxian>99)shangxian=99;
      }
    else if(ADD==0&&set_st==2)
   {
          do{
                Disp_alarm(xiaxian);
          }
         while(ADD==0);
          xiaxian++;
          if(xiaxian>shangxian)xiaxian=shangxian;
    }  
}
