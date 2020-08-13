#include "stm8s.h"
#include "main.h"

void sys_del_us(char del_us)	
{
char i;
for(i=0;i<del_us;i++){nop();}
}


char start_iic (char adr, char read_write_bit)
{
	//configuration pins (sda,clk) 
	PORT_iic ->CR2&=~bit_clk; //PA_clk_pin  - low speed (interrupt disabled)
	PORT_iic ->CR1&=~bit_clk; //PA_clk_pin  - open drain /float in inp. direct
	PORT_iic ->ODR|=bit_clk; //PA_clk_pin  - 1 (CLOCK)
	PORT_iic ->DDR|=bit_clk; 
	PORT_iic ->CR2&=~bit_sda; //PA_sda_pin  - low speed (interrupt disabled)
	PORT_iic ->CR1&=~bit_sda; //PA_sda_pin - open drain /float in inp. direct
	PORT_iic ->ODR|=bit_sda; //PA_sda_pin  - 1 (SDA)
	PORT_iic ->DDR|=bit_sda; 
	
	//(SDA) "START!!!!" 
	PORT_iic ->ODR&=~bit_sda; 											//set sda=0
	nop(); //>0.1us
	return send_byte ((adr<<1)+read_write_bit);
}	
	
char send_byte (char data_byte)
{
	//send start byte
	char i;
	for(i=8;i>0;i--)
	{
	 PORT_iic ->ODR&=~bit_clk;											//clock=0
	 PORT_iic ->ODR&=~bit_sda;                     //set data=0
	 nop();nop();nop(); nop();nop();nop();//>2us
	 PORT_iic ->ODR|=((data_byte>>(i-1))&1)<<pin_sda; //set data
	 nop(); //>0.1us
	 PORT_iic ->ODR|=bit_clk;											//clock=1
	 nop();nop(); nop();nop();//>1us
	 }
	// assert acknowledge
	PORT_iic ->ODR&=~bit_clk;											//clock=0
	nop();nop();nop(); nop();nop();nop();//>2us
	PORT_iic ->ODR|=bit_sda;                     //set data=1
	nop(); //>0.1us
	PORT_iic ->ODR|=bit_clk;											//clock=1
	nop();nop(); nop();nop();//>1us
	if ((PORT_iic ->IDR&bit_sda)==0) 
	return 1; 
	else 
	return 0;
}
unsigned char receive_byte (char acknowledge)
{
	//send start byte
	char i;
	char receive_b=0;
	for(i=8;i>0;i--)
	 {
	 PORT_iic ->ODR&=~bit_clk;											//clock=0
	 nop();nop();nop();nop();nop();nop(); //>0.1us//>2us
	 nop();nop();nop();nop();nop();nop(); //>0.1us//>2us
	 PORT_iic ->ODR|=bit_clk;											//clock=1
	 nop();nop();nop();nop();//>1us
	 nop();nop();nop();nop();nop();nop(); //>0.1us//>2us
	 PORT_iic ->DDR&=~bit_sda;
	 receive_b|=(((PORT_iic ->IDR)&bit_sda)>>pin_sda)<<(i-1);
	 PORT_iic ->DDR|=bit_sda;
	 }
	// send acknowledge
	PORT_iic ->ODR&=~bit_clk;										//clock=0
	if(acknowledge) PORT_iic ->ODR&=~bit_sda;     //set data=0	
	nop();nop();nop();nop();nop();nop();//>2us
	PORT_iic ->ODR|=bit_clk;											//clock=1
	nop();nop();nop();nop();//>1us
	PORT_iic ->ODR&=~bit_clk;											//clock=0
	PORT_iic ->ODR|=bit_sda; 
	
	return receive_b;
}	
	
void stop_iic (void)
{	 
  PORT_iic ->ODR&=~bit_clk;											//clock=0
	nop();nop();nop(); nop();nop();nop();//>2us
	PORT_iic ->ODR&=~bit_sda; 											//set sda=0
	nop(); //>0.1us
	PORT_iic ->ODR|=bit_clk;											//clock=1
	nop(); //>0.1us
	PORT_iic ->ODR|=bit_sda; 											//set sda=1
}



