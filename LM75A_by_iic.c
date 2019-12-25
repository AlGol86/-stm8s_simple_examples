#include "stm8s.h"
#define pin_sda     1
#define pin_clk     2
#define address_iic  (char)(7+72) //address=7 for LM75A temperature sensor

#define bit_sda    (1<<pin_sda)
#define bit_clk    (1<<pin_clk)
#define bit_read    1
#define bit_write   0
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

char temperatura=0;
char temperatura_1=0;
char n;

main()
{
if(start_iic (bit_read))
{while(1);} //not ack

temperatura=receive_byte (1);

temperatura_1=receive_byte (0)>>5;

stop_iic ();


while (1)
{ n++;}//set breakpoint
}

char start_iic (char read_write_bit)
{
	char i,j;
	//configuration pins (sda,clk) 
	PA->CR2&=~bit_clk; //PA_clk_pin  - low speed (interrupt disabled)
	PA->CR1&=~bit_clk; //PA_clk_pin  - open drain /float in inp. direct
	PA->ODR|=bit_clk; //PA_clk_pin  - 1 (CLOCK)
	PA->DDR|=bit_clk; 
	PA->CR2&=~bit_sda; //PA_sda_pin  - low speed (interrupt disabled)
	PA->CR1&=~bit_sda; //PA_sda_pin - open drain /float in inp. direct
	PA->ODR|=bit_sda; //PA_sda_pin  - 1 (SDA)
	PA->DDR|=bit_sda; 
	
	//(SDA) "START!!!!" 
	PA->ODR&=~bit_sda; 											//set sda=0
	for(j=0;j<10;j++){} //>0.1us
	return send_byte ((address_iic<<1)+read_write_bit);
}	
	
char send_byte (char data_byte)
{
	//send start byte
	char i,j;
	for(i=8;i>0;i--)
	 {
	 PA->ODR&=~bit_clk;											//clock=0
	 PA->ODR&=~bit_sda;                     //set data=0
	 for(j=0;j<10;j++){}//>2us
	 PA->ODR|=((data_byte>>(i-1))&1)<<pin_sda; //set data
	 for(j=0;j<10;j++){}//>0.1us
	 PA->ODR|=bit_clk;											//clock=1
	 for(j=0;j<10;j++){}//>1us
	 }
	// assert acknowledge
	PA->ODR&=~bit_clk;											//clock=0
	for(j=0;j<10;j++){}//>2us
	PA->ODR|=bit_sda;                     //set data=1
	for(j=0;j<10;j++){}//>0.1us
	PA->ODR|=bit_clk;											//clock=1
	for(j=0;j<10;j++){}//>1us
	if ((PA->IDR&bit_sda)==0) 
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
	 PA->ODR&=~bit_clk;											//clock=0
   for(j=0;j<10;j++){} //>0.1us//>2us
	 PA->ODR|=bit_clk;											//clock=1
	 for(j=0;j<10;j++){}  //>0.1us//>2us
	 PA->DDR&=~bit_sda;
	 receive_b|=(((PA->IDR)&bit_sda)>>pin_sda)<<(i-1);
	 PA->DDR|=bit_sda;
	 }
	// send acknowledge
	PA->ODR&=~bit_clk;										//clock=0
	if(acknowledge) PA->ODR&=~bit_sda;     //set data=0	
	for(j=0;j<10;j++){} //>2us
	PA->ODR|=bit_clk;											//clock=1
	for(j=0;j<10;j++){} //>1us
	PA->ODR&=~bit_clk;											//clock=0
	PA->ODR|=bit_sda; 
	
	return receive_b;
}	
	
void stop_iic (void)
{	
  char i,j;
  PA->ODR&=~bit_clk;											//clock=0
	for(j=0;j<10;j++){}//>2us
	PA->ODR&=~bit_sda; 											//set sda=0
	for(j=0;j<10;j++){} //>0.1us
	PA->ODR|=bit_clk;											//clock=1
	for(j=0;j<10;j++){} //>0.1us
	PA->ODR|=bit_sda; 											//set sda=1
}
