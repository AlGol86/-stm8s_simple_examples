//this example is intended for 8 MHz clock frequency
#include "stm8s.h"
#define PIN_sda     1
#define PIN_clk     2
#define ADRESS_iic 0x38 //(7+72) paste the device address-if you need to connect several devices, replase this point to "char start_iic (char read_write_bit,....)"
#define BIT_sda    (1<<pin_sda)
#define BIT_clk    (1<<pin_clk)
#define BIT_read    1
#define BIT_write   0
#define ACK 0 //acknowledge
#define NOT_ACK 1 //not acknowledge
#define PA GPIOA
#define PB GPIOB
#define PD GPIOD
#define PE GPIOE

void sys_del_us(char del_us);
char start_iic (char read_write_bit);
char send_byte (char data_byte);
unsigned char receive_byte (char acknowledge);
void stop_iic (void);

void sys_del_us(char del_us)	
{
char i;
for(i=0;i<del_us;i++)
{nop();}
}

char start_iic (char read_write_bit)
{
	//configuration pins (sda,clk) 
	PA->CR2&=~BIT_clk; //PA_clk_pin  - low speed (interrupt disabled)
	PA->CR1&=~BIT_clk; //PA_clk_pin  - open drain /float in inp. direct
	PA->ODR|=BIT_clk; //PA_clk_pin  - 1 (CLOCK)
	PA->DDR|=BIT_clk; 
	PA->CR2&=~BIT_sda; //PA_sda_pin  - low speed (interrupt disabled)
	PA->CR1&=~BIT_sda; //PA_sda_pin - open drain /float in inp. direct
	PA->ODR|=BIT_sda; //PA_sda_pin  - 1 (SDA)
	PA->DDR|=BIT_sda; 
	
	//(SDA) "START!!!!" 
	PA->ODR&=~BIT_sda; 											//set sda=0
	nop(); //>0.1us
	return send_byte ((ADRESS_iic<<1)+read_write_bit);
}	
	
char send_byte (char data_byte)
{
	//send start byte
	char i;
	for(i=8;i>0;i--)
	 {
	 PA->ODR&=~BIT_clk;											//clock=0
	 PA->ODR&=~BIT_sda;                     //set data=0
	 sys_del_us(2);//>2us
	 PA->ODR|=((data_byte>>(i-1))&1)<<PIN_sda; //set data
	 nop();//>0.1us
	 PA->ODR|=BIT_clk;											//clock=1
	 nop();nop();//>1us
	 }
	// assert acknowledge
	PA->ODR&=~BIT_clk;											//clock=0
	nop();nop();nop();//>2us
	PA->ODR|=BIT_sda;                     //set data=1
	nop();//>0.1us
	PA->ODR|=BIT_clk;											//clock=1
	nop();nop();//>1us
	if ((PA->IDR&BIT_sda)==0) 
	return ACK; 
	else 
	return NOT_ACK;
}

unsigned char receive_byte (char acknowledge)
{
	//send start byte
	char i,j;
	char receive_b=0;
	for(i=8;i>0;i--)
	 {
	 PA->ODR&=~BIT_clk;											//clock=0
	 sys_del_us(4);//>5us
	 PA->ODR|=BIT_clk;											//clock=1
	 sys_del_us(4);//>5us
	 PA->DDR&=~BIT_sda;
	 receive_b|=(((PA->IDR)&BIT_sda)>>PIN_sda)<<(i-1);
	 PA->DDR|=BIT_sda;
	 }
	// send acknowledge
	PA->ODR&=~BIT_clk;										//clock=0
	if(acknowledge) PA->ODR&=~BIT_sda;     //set data=0	
	sys_del_us(2);//>2us
	PA->ODR|=BIT_clk;											//clock=1
	sys_del_us(2);//>1us
	PA->ODR&=~BIT_clk;											//clock=0
	PA->ODR|=BIT_sda; 
	
	return receive_b;
}	
	
void stop_iic (void)
{	 
  PA->ODR&=~BIT_clk;											//clock=0
	sys_del_us(2);//>2us
	PA->ODR&=~BIT_sda; 											//set sda=0
	nop(); //>0.1us
	PA->ODR|=BIT_clk;											//clock=1
	nop();//>0.1us
	PA->ODR|=BIT_sda; 											//set sda=1
}
