/*MCU:      STC12C5AS60S2
 *Senor:    DS18B20
 *Display:  LCD1602
 *Author:   Mickey
 *Date:     2016-09-04
 *Goal:     Display temperature on lcd1602.
 */

#include "config.h"

extern void lcd_init();
extern void lcd_printf(unsigned char x,unsigned char y,unsigned char *str);
extern unsigned char convert(unsigned char *str,int dat);
extern bool ds18b20_start();
extern bool ds18b20_get_temp(int *temp);

bool flag1s=0;		                        //1s timer flag
unsigned char T0RH=0;	                    //Timer0 reload high byte
unsigned char T0RL=0;	                    //Timer0 reload low byte

//software delay 100ms
//void delay_100ms(){
//    unsigned char i, j, k;

//	i = 5;
//	j = 144;
//	k = 71;
//	do{
//        do{
//		    while(--k);
//		}while(--j);
//	}while(--i);
//}

void delay1000ms()		//@12.000MHz
{
	unsigned char i, j, k;

	i = 46;
	j = 153;
	k = 245;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

/*
 * config Timer0
 * input:ms time
 */
void config_timer0(unsigned int ms)
{
	unsigned long tmp;	                    //temperature
	tmp=12000000/12;	                    //timer fosc
	tmp=(tmp*ms)/1000;	                    //counter no.
	tmp=65536-tmp;		                    //reload counter
	tmp=tmp+12;		                        //add delay error count
	T0RH=(unsigned char)(tmp>>8);	        //high byte
	T0RL=(unsigned char)tmp;            	//low byte
	TMOD&=0xF0;	                        	//clear Timer0 config
	TMOD|=0x01;		                        //config Timer0 mode 1
	TH0=T0RH;
	TL0=T0RL;
	ET0=1;		                        	//enable  Timer0 intrupt
	TR0=1;		                        	//start Timer0
}


/*
 * Timer0 interrupt 1
 * return flag1s=1
 */
void InterruptTimer() interrupt 1
{
    static unsigned char tmr1s=0;
    TH0=T0RH;   //reload
    TL0=T0RL;
    tmr1s++;
    if(tmr1s>100)
    {//1s
        tmr1s=0;
        flag1s=1;
    }
}

/*
 * main function
 * show tempture on lcd1602
 * refresh every 1s
 */
void main()
{
	unsigned char str[]="hello world!";
    bool res;
	int temp;	                            //temperature
	int tempInt,tempDec;                    //int and dec of temp
	unsigned char len;
	unsigned char str2[12];

//	EA=1;	                            	//open interupt
//	config_timer0(10);                      //timer0 10ms
	ds18b20_start();                        //start ds18b20
	lcd_init();	                            //init lcd1602

	lcd_printf(0,0,str);
	delay1000ms();		                //delay 1s

	while(1)
    {
		lcd_printf(0,0,"start 18b20");
        delay1000ms();
		res=ds18b20_get_temp(&temp);        //read temp now
		if(res)
        {		            	    //read temp and refresh
			lcd_printf(0,0,"get temp");
			tempInt=temp>>4;        	    //temp int
			tempDec=temp&0xF;	            //temp dec
			len=convert(str2,tempInt);      //temp int convert to str
			str2[len++]='.';	        	//add dot
			tempDec=(tempDec*10)/16;        //temp dec convert to 1bit
			str2[len++]=tempDec+'0';         //convert to ascii str
			while(len<6)
            {	        	    //add space to len =6
				str2[len++]=' ';
			}
			str2[len]='\0';	        	    //add str end flag
			lcd_printf(0,0,str2);    	    //show str on lcd1602
		}
        else
        {			            	    //read error,display error
			lcd_printf(0,0,"error!");
		}
		ds18b20_start();	        	    //restart next temp convert
	}
}

