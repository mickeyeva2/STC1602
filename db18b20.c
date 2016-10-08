//DS18B20 Function
//DQ 4.7K~10K pullup resistor

#include "config.h"

/*******************************************************************************
* Function Name     : delay_ten_us
* Feature		    : Delay ten us
* Input             : None
* Output            : None
*******************************************************************************/
/* delay function, time=t*10 us*/
void delay_ten_us(uchar t)
{
	do
    {
		_nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    }
    while(--t);
}

/*******************************************************************************
* Function Name     : Ds18b20Init
* Feature		    : Initialization
* Input             : None
* Output            : return 1 while initialization succeed, else return 0
* From Six Axis
*******************************************************************************/
uchar Ds18b20Init()
{
	uchar i;
	DSPORT = 0;			//pull bus down about 480us~960us
	i = 70;	
	while(i--);         //wait 642us
	DSPORT = 1;			//pullup bus，if ds18b20 response, it will pull down after 15us~60us
	i = 0;
	while(DSPORT)	    //wait DS18B20 pull down the bus
    {
		Delay1ms(1);
		i++;
		if(i>5)         //wait at lease 5ms
		{
			return 0;   //init failed
		}
	
	}
	return 1;           //init success
}

/*reset DS18B20's DQ bus, get ack, after that start read and write one time*/
bool ds18b20_get_ack()
{
    bool ack;
	lcd_printf(0,0,"start get ack...");
    delay_ten_us(30000);	
    EA=0;               	//disable all interupt
    DQ=0;          			//start 500us reset pulse
    delay_ten_us(50);
    DQ=1;
    delay_ten_us(5);    	//delay 60us
    ack=DQ;        			//read the pulse
    delay_ten_us(50);
    while(!DQ);
    EA=1;               //enable all interput
	if(ack)
    {
		lcd_printf(0,0,"ack=1 fail");
	}
    return ack;
}

/*******************************************************************************
* Function Name     : ds18b20_write_data
* Feature		    : write a byte to ds18b20
* Input             : dat
* Output            : None
*******************************************************************************/
void ds18b20_write_data(unsigned char dat)
{
    unsigned char mask;
    EA=0;
    for(mask=0x01;mask!=0;mask<<=1)
    {        //low bit first, 8bit in all
        DQ=0;                          //2us low Voltage pusle
        _nop_();
        _nop_();
        if((mask&dat)==0)
        {                  //output dat
            DQ=0;
        }
        else
        {
            DQ=1;
        }
    }
    delay_ten_us(6);                        //delay 60us
    DQ=1;                              //pull up DS18B20's DQ
    EA=1;                                   //enable zhongduan
}

/*******************************************************************************
* Function Name     : ds18b20_read_data
* Feature		    : read a byte from ds18b20
* Input             : None
* Output            : dat
*******************************************************************************/
uchar ds18b20_read_data()
{
    uchar dat;
    uchar mask;
    EA=0;
    for(mask=0x01;mask!=0;mask<<=1)
    {        //low bit first,8bit in all
        DQ=0;                          //2us low voltage pusle
        _nop_();
        _nop_();
        DQ=1;                          //finish low voltage pusle,wait ds18b20 output data
        _nop_();
        _nop_();
        if(!DQ)
        {                       //read DQ
            dat&=~mask;
        }
        else
        {
            dat|=mask;
        }
        delay_ten_us(6);                    //delay 60us
    }
    EA=1;
    return dat;
}

//ds18b20 read a byte 2

uchar ds18b20_read_data()
{
    uchar i;
    uchar dat=0;
    for (i=0;i<8;i++)   //8 bit counter
    {
        dat >>=1;
        DQ=0;
        _nop_();
        _nop_();
        DQ=1;
        _nop_();
        _nop_();
        if(DQ) dat|=0x80;
        delay_ten_us(12);
    }
    return dat;
}

/*******************************************************************************
* Function Name     : ds18b20_start
* Feature		    : start temp convert
* Input             : None
* Output            : True or False
*******************************************************************************/
bool ds18b20_start()
{
    bool ack;
    ack=ds18b20_get_ack();                  //reset ds18b20'bus, get ds18b20's ack
    if(ack==0)
    {                             //if ds18b20 return 0, convert temp
        ds18b20_write_data(0xCC);           //skip rom
        ds18b20_write_data(0x44);           //covert temp
    }
    return ~ack;                            //ack==0 as success, return true value
}

/*******************************************************************************
* Function Name     : ds18b20_get_temp
* Feature		    : read temperature from ds18b20
* Input             : Temp
* Output            : True or False
*******************************************************************************/
bool ds18b20_get_temp(int *temp)
{
    bool ack;
    uchar LSB,MSB;                  //16bit temp,low byte and high byte
    ack=ds18b20_get_ack();                  //reset bus,get ack
	lcd_printf(0,0,"get ack");
    if(ack==0)
    {                             //read temp when ds18b20 return low bit
        ds18b20_write_data(0xCC);           //skip rom
        ds18b20_write_data(0xBE);           //send read cmd
        LSB=ds18b20_read_data();            //low byte
        MSB=ds18b20_read_data();            //high byte
        *temp=((int)MSB<<8)+LSB;            //16bit int
    }
    return ~ack;                            //ack==0 as success
}

//read temp
//
//
int ds18b20_read_temp()
{
    int temp=0;
    uchar tmh,tml;
    ds18b20_get_ack();
    ds18b20_write_data(0xCC);               //skip rom
    ds18b20_write_data(0x44);               //start temp convert
    while(!DQ);

    ds18b20_get_ack();
    ds18b20_write_data(0xCC);               //skip rom
    ds18b20_write_data(0xBE);               //start temp convert
    tml=ds18b20_read_data();
    tmh=ds18b20_read_data();
    temp=tmh;
    temp<<=8;
    temp|=tml;
    return temp;
}

void get_temp(int temp)
{
    float tp;
    if(temp<0)
    {
        datas[0]='-';
        temp=temp-1;
        temp=~temp;
        tp=temp;
        temp=tp*0.0625*100+5;
    }
    else
    {
        datas[0]='+';
        tp=temp;
        temp=tp*0.0625*100+5;
    }

    datas[1] = temp / 10000 + 48;
    datas[2] = temp % 10000 / 1000 + 48;
    datas[3] = temp % 1000 / 100 + 48;
    datas[4] = '.';
    datas[5] = temp % 100 / 10 + 48;
    datas[6] = temp % 10 + 48;
}

/*
 * convert int to str
 * input:str point,dat
 * output:return len(str)
 */
unsigned char convert(unsigned char *str,int dat)
{
	signed char i=0;
	unsigned char len=0;
	unsigned char buf[6];

	if(dat<0)
    {		//if fu shu,abs(dat),add '-' to str
		dat=-dat;
		*str++='-';
		len++;
	}
	do
    {			//convert it to shuzu, low bit first
		buf[i++]=dat%10;
		dat/=10;
	}
    while(dat>0);
	len+=i;			//len(str)=i
	while(i-->0)
    {		//convert shuzu to ascii
		*str++=buf[i]+'0';
	}
	*str='\0';		//add str end flag
	return len;		//return len(str)
}

/*******************************************************************************
* Function Name     : Ds18b20ChangTemp
* Feature		    : DS18B20 start convert temperature
* Input             : None
* Output            : None
*******************************************************************************/
void  Ds18b20ChangTemp()
{
	Ds18b20Init();
	Delay1ms(1);                //delay 1ms,
	Ds18b20WriteByte(0xcc);		//skip rom		 
	Ds18b20WriteByte(0x44);	    //start temperature convert
    //Delay1ms(1000);	        //等待转换成功，而如果你是一直刷着的话，就不用这个延时了 
}

/*******************************************************************************
* Function Name     : Ds18b20ReadTempCom
* Feature		    : send read temperature
* Input             : None
* Output            : None
*******************************************************************************/
void  Ds18b20ReadTempCom()
{	
	Ds18b20Init();
	Delay1ms(1);
	Ds18b20WriteByte(0xcc);	 //跳过ROM操作命令
	Ds18b20WriteByte(0xbe);	 //发送读取温度命令
}

/*******************************************************************************
* Function Name     : Ds18b20ReadTemp
* Feature		    : read temperature
* Input             : None
* Output            : temperature
*******************************************************************************/
int Ds18b20ReadTemp()
{
	int temp = 0;
	uchar tmh, tml;
	Ds18b20ChangTemp();			 	//先写入转换命令
	Ds18b20ReadTempCom();			//然后等待转换完后发送读取温度命令
	tml = Ds18b20ReadByte();		//读取温度值共16位，先读低字节
	tmh = Ds18b20ReadByte();		//再读高字节
	temp = tmh;
	temp <<= 8;
	temp |= tml;
	return temp;
}
