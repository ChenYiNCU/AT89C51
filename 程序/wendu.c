#include "reg52.h"  
//---重定义关键词---//
#define uchar unsigned char
#define uint unsigned int


//--定义使用的IO口--//
sbit DQ =P3^6;
sbit BEEP=P3^7;               //定义蜂鸣器   
sbit ALAM=P1^3;                //定义灯光报警
sbit SET=P3^1;                 //定义调整键
sbit ADD=P3^3;
sbit DEC=P3^2;

bit shanshuo_st;                            //闪烁间隔标志
bit beep_st;                                     //蜂鸣器间隔标志
char shangxian=38;                  //上限报警温度，默认值为38
char xiaxian=-6;                   //下限报警温度，默认值为-6
uchar set_st=0;                             //状态标志

uchar m;                    //温度值全局变量（整数）
char Signed_Current_temp;
uchar n;                             //温度值全局变量（小数）

char num=0;
uchar DisplayData[8];

uchar code DSY_CODE[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
uchar data DSY_IDX[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
uchar code df_table[]={0,1,1,2,3,3,4,4,5,6,6,7,8,8,9,9};
void Alarm();

/*******************************************************************************
* 函 数 名         : delay
* 函数功能		   : 延时函数，i=1时，大约延时10us
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

/*****延时子程序*****/
void Delay_DS18B20(int num)
{
  while(num--) ;
}
/*****初始化DS18B20*****/
void Init_DS18B20(void)
{
  unsigned char x=0;
  DQ = 1;         //DQ复位
  Delay_DS18B20(28);    //稍做延时
  DQ = 0;         //单片机将DQ拉低
  Delay_DS18B20(80);   //精确延时，大于480us
  DQ = 1;         //拉高总线
  Delay_DS18B20(14);
  x = DQ;           //稍做延时后，如果x=0则初始化成功，x=1则初始化失败
  Delay_DS18B20(20);
}
/*****读一个字节*****/
unsigned char ReadOneChar(void)
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;     // 给脉冲信号
    dat>>=1;
    DQ = 1;     // 给脉冲信号
    if(DQ)
    dat|=0x80;
    Delay_DS18B20(4);
  }
  return(dat);
}
/*****写一个字节*****/
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
/*****读取温度*****/
void ReadTemperature(void)
{
  unsigned char a=0;
  unsigned char b=0;
  //unsigned int t=0;

  uchar ng=0;
  //float tt=0;
  Init_DS18B20();
  WriteOneChar(0xCC);  //跳过读序号列号的操作
  WriteOneChar(0x44);  //启动温度转换
  	Delay1ms(15);

  Init_DS18B20();
  WriteOneChar(0xCC);  //跳过读序号列号的操作
  WriteOneChar(0xBE);  //读取温度寄存器
  a=ReadOneChar();     //读低8位
  b=ReadOneChar();    //读高8位
    if((b&0XF8)==0XF8)
   {
      b=~b;
      a=~a+1;
      if(a==0X00)b++;
      ng=1;               //为负数
     
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
  

  //t=b;                                   //高8位转移到t
 // t<<=8;                           //t数据左移8位
 // t=t|a;                           //将t和a按位或，得到一个16位的数
  //tt=t*0.0625;                   //将t乘以0.0625得到实际温度值（温度传感器设置12位精度，最小分辨率是0.0625）
 // t= tt*10+0.5;     //放大10倍（将小数点后一位显示出来）输出并四舍五入
  //return(t);                   //返回温度值
}

/*****读取温度*****/
void check_wendu(void)
{
    //uint a,b,c,i;
     uint i;
    for( i=2;i>0;i--){          //重复3次
	     ReadTemperature();                          //获取温度值
        //a=c/100;                                                     //计算得到十位数字
        //b=c/10-a*10;                                            //计算得到个位数字
       // m=c/10;                                                      //计算得到整数位
       // n=c-a*100-b*10;                                    //计算得到小数位
       // if(m<0){m=0;n=0;}                                   //设置温度显示上限
        if(m>99){m=99;n=9;}                                   //设置温度显示上限    
	 } 
}

/*******************************************************************************
* 函数名         :DigDisplay()
* 函数功能		 :数码管显示函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void DigDisplay()
{

	uint i;
	P0=0xff;
	P2=0x00;
	for(i=30;i>0;i--){
	
				P2=DSY_IDX[0];
					P0=DisplayData[0];//发送数据
		delay(150); //间隔一段时间扫描	
		P0=0xff;//消隐
	
			P2=DSY_IDX[1]; 
				P0=DisplayData[1];//发送数据
		delay(150); //间隔一段时间扫描	
		P0=0xff;//消隐
		
				P2=DSY_IDX[2];
					P0=DisplayData[2];//发送数据
		delay(150); //间隔一段时间扫描	
		P0=0xff;//消隐
			
				P2=DSY_IDX[3];
					P0=DisplayData[3];//发送数据
		delay(250); //间隔一段时间扫描	
		P0=0xff;//消隐
	
	 Alarm();   //报警检测
	}
 

}

/*****报警子程序*****/
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

/*****显示报警温度子程序*****/
void Disp_alarm(char baojing)
{
	uint i;
	P0=0xff;
	P2=0x00;

   for(i=2;i>0;i--){
       P2=DSY_IDX[0];
      if(set_st==1)P0=0x89;       //上限H
        else if(set_st==2)P0=0xC7; //下限L标示
        
        delay(100);
    	P0=0xff;//消隐

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
	 P0=0xff;//消隐

     P2=DSY_IDX[2];
     P0=DSY_CODE[baojing/10];
     delay(100);
	 P0=0xff;//消隐

     P2=DSY_IDX[3];
     P0=DSY_CODE[baojing%10];
     delay(200);
	 P0=0xff;//消隐

    }

}



/*******************************************************************************
* 函 数 名       : main
* 函数功能		 : 主函数
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void main()
{	
   EA=1;
  
  IT0=1;     //外部中断的触发方式，下降沿触发      
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
   
          EX0=0;    //关闭外部中断0
          EX1=0;    //关闭外部中断1
       	   check_wendu();
	    datapros();
    	DigDisplay();
        //Alarm();   //报警检测
       }
       else if(set_st==1)
      {
          BEEP=1;    //关闭蜂鸣器
          ALAM=1;
          EX0=1;    //开启外部中断0
          EX1=1;    //开启外部中断1
         
         if(shanshuo_st) {Disp_alarm(shangxian);}
      }
      else if(set_st==2)
      {
          BEEP=1;    //关闭蜂鸣器
          ALAM=1;
          EX0=1;    //开启外部中断0
          EX1=1;    //开启外部中断1
         
          if(shanshuo_st) {Disp_alarm(xiaxian);}
      }

   }

}



/*****外部中断0服务程序*****/
void int0(void) interrupt 0
{

       EX0=0;      //关外部中断0
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
/*****外部中断1服务程序*****/
void int1(void) interrupt 2
{
       EX1=0;      //关外部中断1
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
