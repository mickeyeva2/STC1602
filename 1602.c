//********1602 Function**********//
#include "config.h"

void lcd_init();
//void lcd_display(unsigned char x,unsigned char y,unsigned char *str);

/*waiting lcd ready*/
void lcd_busy()
{
    unsigned char stat;
    LCD=0xFF;
    EN=0;
    RS=0;
    RW=1;
    do 
    {
        EN=1;
        stat=LCD; //Read busy flag of lcd1602
        EN=0;
    }while (stat&0x80); //7th bit equ 1 as lcd busy, read busy flay until bit7=0

}

/*write cmd to lcd1602*/
void lcd_write_cmd(unsigned char cmd)
{
    lcd_busy();
    RS=0;
    RW=0;
    LCD=cmd;
    EN=1;
    EN=0;
}

/*write data to lcd1602*/
void lcd_write_data(unsigned char dat)
{
    lcd_busy();
    RS=1;
    RW=0;
    LCD=dat;
    EN=1;
    EN=0;
}

//void lcd_read_cmd(unsigned char cmd){
//}

//void lcd_read_data(unsigned char dat){
//}

void lcd_set_cursor(unsigned char x,unsigned char y)
{
    unsigned char addr;
    if(y==0){
        addr=0x00+x;
    }else{
        addr=0x40+x;
    }
    lcd_write_cmd(addr|0x80);
}

/*show str at (x,y)*/
void lcd_printf(unsigned char x,unsigned char y,unsigned char *str)
{
	lcd_write_cmd(0x01);	//clear
    lcd_set_cursor(x,y);
    while(*str!='\0')
    {
        lcd_write_data(*str++);
    }
}

/*init*/
void lcd_init()
{
    lcd_write_cmd(0x38); //16*2,5*7,8bit dataport
    lcd_write_cmd(0x0C); //open ,close cursor
    lcd_write_cmd(0x06); //addr+1
    lcd_write_cmd(0x01); //clear
}

