#include "stm8s.h"


#define PORT_iic       GPIOB
#define pin_sda        4
#define bit_sda                           (1<<pin_sda)
#define pin_clk        5
#define bit_clk                           (1<<pin_clk)

#define WRITE 0
#define READ  1

#define PORT_enc GPIOA
#define pin_enc_l    1
#define bit_enc_l                       (1<<pin_enc_l)
#define pin_enc_r    2
#define bit_enc_r                       (1<<pin_enc_r)


#define address_iic_oled 0x3c // 3c 
#define oled_32_128_adr  0x3c // 3c 
#define address_iic_t    0x76 //

#define bit_read    1
#define bit_write   0
#define ACK 0 //acknowledge
#define NOT_ACK 1 //not acknowledge

#define SMALL 0
#define MIDDLE 1
#define MEGA 2
#define GIGA 3
#define TERRA 4

typedef struct {
	char cnt;
	char f_push:1;
	char button :1;
	signed char enc_data;
} encoder_t;

void init_encoder(void);
void encoder_handler(encoder_t* enc);

//iic
void sys_del_us(char del_us);
char start_iic (char adr, char read_write_bit);
char send_byte (char data_byte);
unsigned char receive_byte (char acknowledge);
void stop_iic (void);

void init_tim1(void);
@far @interrupt void TIM1Interrupt (void);



void init_ssd1306_2(char adr);
void delay(unsigned int del);
void sendCmd(char adr,char cmd);
void sendData(char adr,char* data, char count); //send [count] data byts 
void blankScreen(char adr,char n);
void oled_Clear_Screen(void);
void set_cursor(char adr, char x,char y );
void ff_string(char adr,char n);
void send_repeat_data(char adr,char data_byte,int n);
void send_single_data(char adr,char data_byte);
void oled_blink1(char left, char right, char width, char count); //cursor right-left effect
void oled_print_small_digit(char d,char x,char y);  //3*5
void oled_print_middle_digit(char d,char x,char y); //6*10
void oled_print_mega_digit(char d,char x,char y);   //9*15
char oled_print_mega_char(char c,char x,char y);
void oled_print_giga_digit(char d,char x,char y);   //12*20
char oled_print_giga_char(char c,char x,char y); //12*20
char oled_print_char(char c,char x,char y,char font);   
void oled_print_terra_digit(char d,char x,char y);  //18*30
char oled_print_graphic(char* data,char n,char x,char width);
char oled_print_nomber(char f_zero,int n,char pos,char y, char font );
