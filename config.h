//********config file***********
//LCD1602
//DS18B02

#ifndef __CONFIG_H_
#define __CONFIG_H_

#include <REG51.H>
#include <intrins.h>

//*** redefine key words ***//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

#ifndef bit
#define bit bool
#endif

//typedef unsigned int uint;
//typedef unsigned char uchar;
//typedef bit bool;

//*** LCD 1602 ***//
//Data port set to P0
//RS P2.0   data/command select
//RW P2.1   read/write select
//EN P2.2   enable Pin
#define LCD P0
sbit RS = P2^0;
sbit RW = P2^1;
sbit EN = P2^2;

//***ds18b20***//
//DQ P2.3
sbit DQ = P2^3;	

//*** declare global function **//
//*** LCD 1602 ***//
void lcd_init();                                    //LCD1602 Init Function
void lcd_busy();                                    //Read LCD1602 busy flag
void lcd_write_cmd(uchar cmd);                      //LCD1602 write command RS=0, RW=0, EN=1->0
void lcd_read_cmd(uchar cmd);                       //LCD1602 read command RS=0, RW=1, EN=1
void lcd_write_data(uchar dat);                     //LCD1602 write dataRS=1, RW=0, EN=1->0
void lcd_read_data(uchar dat);                      //LCD1602 read data RS=1, RW=1, EN=1
void lcd_set_cursor(uchar x,uchar y);               //LCD1602 set cursor or first place
void lcd_printf(uchar x,uchar y,uchar *str);        //LCD1602 display str at (x,y)
void lcd_display(uchar dat);                        //LCD1602 display data

void delay_ten_us(uchar t);                 //delay t*10 us

//*** DS18B20 ***//
bool ds18b20_get_ack();                     //reset ds18b20 bus
void ds18b20_write_data(uchar dat);         //write one bit to ds18b20
uchar ds18b20_read_data();                  //read one byte from ds18b20,return it
bool ds18b20_start();                       //start convert temperature,return true if ok
bool ds18b20_get_temp(int *temp);           //read temperature from ds18b20,return true if ok
uchar convert(uchar *str,int dat);          //convert int to str,return len(str)

//*** From Six Axis ***//
void Delay1ms(uint );
uchar Ds18b20Init();
void Ds18b20WriteByte(uchar com);
uchar Ds18b20ReadByte();
void  Ds18b20ChangTemp();
void  Ds18b20ReadTempCom();
int Ds18b20ReadTemp();

#endif
