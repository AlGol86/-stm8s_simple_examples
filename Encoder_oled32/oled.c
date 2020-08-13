#include "stm8s.h"
#include "main.h"

void delay(unsigned int del)
{
	int j;
  for(j=0;j<=del;j++){}
}

void sendCmd(char adr,char cmd)
{start_iic(adr,WRITE);
send_byte (0x80);
send_byte (cmd);
stop_iic();
}

void sendData(char adr,char* data, char count)
{int j;
start_iic(adr,WRITE);
send_byte (0x40);
for(j=0;j<count;j++)
{
send_byte (*(data+j));
}
stop_iic();
}

void init_ssd1306_2(char adr) //init for oled128*64
{
	int j;
   delay(10000);

   sendCmd(adr,0xAE); //display off
   sendCmd(adr,0x20); //Set Memory Addressing Mode   
   sendCmd(adr,0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                // 10b,Page Addressing Mode (RESET); 11b,Invalid
   sendCmd(adr,0xC0); //Set COM Output Scan Direction    (0xC0)-- Mirror vertically
   sendCmd(adr,0xA0); //--set segment re-map 0 to 127 - CHECK (0xA0)--- Mirror horizontally
   sendCmd(adr,0xB0); //Set Page Start Address for Page Addressing Mode,0-7
   sendCmd(adr,0x00); //---set low column address
   sendCmd(adr,0x10); //---set high column address
   sendCmd(adr,0x40); //--set start line address - CHECK
   sendCmd(adr,0x81); //--set contrast control register - CHECK
   sendCmd(adr,0x7F); //--set contrast volume
   sendCmd(adr,0xA6); //--set normal color (0xA7)--set inverse color
   sendCmd(adr,0xA8); //--set multiplex ratio(1 to 64) - CHECK
   sendCmd(adr,0x1F); //0x3F for 128*64 0x1f for 128*32
   sendCmd(adr,0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
   sendCmd(adr,0xD3); //-set display offset - CHECK
   sendCmd(adr,0x00); //-not offset
   sendCmd(adr,0xD5); //--set display clock divide ratio/oscillator frequency
   sendCmd(adr,0xF0); //--set divide ratio
   sendCmd(adr,0xD9); //--set pre-charge period
   sendCmd(adr,0x22); //
   sendCmd(adr,0xDA); //--set com pins hardware configuration - CHECK
   sendCmd(adr,0x02); //0x12 for 128*64 0x02 for 128*32
   sendCmd(adr,0xDB); //--set vcomh
   sendCmd(adr,0x20); //0x20,0.77xVcc
   sendCmd(adr,0x8D); //--set DC-DC enable
   sendCmd(adr,0x14); //
   sendCmd(adr,0xAF); //--turn on SSD1306 panel
   for(j=0;j<=3;j++) blankScreen(0x3c,128);
}

void blankScreen(char adr,char n)
{
send_repeat_data(adr,0x00,n);
}
void oled_Clear_Screen(void)
{ 
char i;
for(i=0;i<8;i++) 
{set_cursor(oled_32_128_adr,0,i );send_repeat_data(oled_32_128_adr,0x00,128);}
}

void ff_string(char adr,char n)
{
send_repeat_data(adr,0xff,n);
}

void send_repeat_data(char adr,char data_byte,int n)
{
int j;	
start_iic(adr,WRITE);
send_byte (0x40);
for(j=1;j<=n;j++) send_byte (data_byte);
stop_iic();
}

void send_single_data(char adr,char data_byte)
{
start_iic(adr,WRITE);
send_byte (0xc0);
send_byte (data_byte);
stop_iic();
}

void set_cursor(char adr, char x,char y )
{   sendCmd(adr,0x0f&x); 
    sendCmd(adr,0x10|(0x0f&(x>>4)));
    sendCmd(adr,0xb0|y); 
}

void oled_blink1(char left, char right, char width,char count)
 {  char i=left;
    char sig=0; 
    const char y=7;
    count++;
     while(count)
     {
      set_cursor(0x3c, i,y );
      ff_string(0x3c,width);
      set_cursor(0x3c, i+width-1,y );
      blankScreen(0x3c,1);
      set_cursor(0x3c, i,y );
      blankScreen(0x3c,1);
      if(i==right+1-width) sig=1;
      if(i==left)   {sig=0;count--;}
      if (sig==0) i++; else i--;
     } 
     set_cursor(0x3c, left,y );
     blankScreen(0x3c,width); 
 }

void oled_print_small_digit(char d,char x,char y)
{
  char dig0[3]={0x1f,0x11,0x1f};
 set_cursor(0x3c,x,y );
  switch(d)
  {
  case 0:{sendData(0x3c,dig0,3);break;}
  }
}

void oled_print_middle_digit(char d,char x,char y)
{
  char c=6;
  char dig0[12]={0xff,0xff,0x03,0x03,0xff,0xff,0x03,0x03,0x03,0x03,0x03,0x03};
  set_cursor(0x3c,x,y );
  switch(d)
  {
  case 0:
    {sendData(0x3c,dig0,c);set_cursor(0x3c,x,y+1);sendData(0x3c,dig0+c,c);break;}
  } 
  }

void oled_print_mega_digit(char d,char x,char y)
{
	char i;
  const char c=3;
  const char const_dig[10][8]={{0xff,0x07,0xff,0,0x7f,0x70,0x7f,0}, //0
                                 {0,0,0xff,0,0,0,0x7f,0},             //1
                                 {0xc7,0xc7,0xff,0,0x7f,0x71,0x71,0}, //2
                                 {0xc7,0xc7,0xff,0,0x71,0x71,0x7f,0}, //3            
                                 {0xff,0xc0,0xff,0,0x01,0x01,0x7f,0}, //4
                                 {0xff,0xc7,0xc7,0,0x71,0x71,0x7f,0}, //5
                                 {0xff,0xc7,0xc7,0,0x7f,0x71,0x7f,0}, //6
                                 {0x07,0x07,0xff,0,0x00,0x00,0x7f,0}, //7
                                 {0xff,0xc7,0xff,0,0x7f,0x71,0x7f,0}, //8
                                 {0xff,0xc7,0xff,0,0x71,0x71,0x7f,0}};//9
  char dig[8];
  for (i=0;i<8;i++) dig[i]=const_dig[d][i]; 
  
  set_cursor(oled_32_128_adr,x,y );
  for (i=0;i<4;i++) send_repeat_data(oled_32_128_adr,dig[i],c);
  set_cursor(oled_32_128_adr,x,y+1);
  for (i=4;i<8;i++) send_repeat_data(oled_32_128_adr,dig[i],c);
}

void oled_print_giga_digit(char d,char x,char y)
{
  const char c=4;
	const char const_dig[10][9]={{0xff,0x0f,0xff,0xff,0x00,0xff,0x0f,0x0f,0x0f},  //0
                               {0x00,0x00,0xff,0x00,0x00,0xff,0x00,0x00,0x0f},  //1
                               {0x0f,0x0f,0xff,0xff,0x0f,0x0f,0x0f,0x0f,0x0f},  //2
                               {0x0f,0x0f,0xff,0x0f,0x0f,0xff,0x0f,0x0f,0x0f},  //3
                               {0xff,0x00,0xff,0x0f,0x0f,0xff,0x00,0x00,0x0f},  //4
                               {0xff,0x0f,0x0f,0x0f,0x0f,0xff,0x0f,0x0f,0x0f},  //5
                               {0xff,0x0f,0x0f,0xff,0x0f,0xff,0x0f,0x0f,0x0f},  //6
                               {0x0f,0x0f,0xff,0x00,0x00,0xff,0x00,0x00,0x0f},  //7
                               {0xff,0x0f,0xff,0xff,0x0f,0xff,0x0f,0x0f,0x0f},  //8
                               {0xff,0x0f,0xff,0x0f,0x0f,0xff,0x0f,0x0f,0x0f}}; //9
  char dig[9];
	char i=0;
  for (i=0;i<9;i++) 
	dig[i]=const_dig[d][i]; 
  
  set_cursor(oled_32_128_adr,x,y );
  for (i=0;i<3;i++) send_repeat_data(oled_32_128_adr,dig[i],c);
  send_repeat_data(oled_32_128_adr,0,4);
  set_cursor(oled_32_128_adr,x,y+1);
  for (i=3;i<6;i++) send_repeat_data(oled_32_128_adr,dig[i],c);
  send_repeat_data(oled_32_128_adr,0,4);
  set_cursor(oled_32_128_adr,x,y+2);
  for (i=6;i<9;i++) send_repeat_data(oled_32_128_adr,dig[i],c);
  send_repeat_data(oled_32_128_adr,0,4);
  }

char oled_print_char(char c,char x,char y,char font)
{ switch(font)
  {
  case GIGA:{x=oled_print_giga_char(c,x,y); break;}
  case MEGA:{x=oled_print_mega_char(c,x,y); break;}
  }
  return x;
}

char oled_print_mega_char(char c,char x,char y)
{
	char i;
  const char cc=3;
  const char const_ch[5][8]={{0x00,0x38,0x00,0x00, 0x00,0x0e,0x00,0x00},  //0   ':'
                             {0xc0,0xc0,0xc0,0x00, 0x01,0x01,0x01,0x00},  //1   '-'
                             {0x00,0x00,0x00,0x00, 0x70,0x00,0x00,0x00},  //2   '.'
                             {0x0f,0x8f,0xe0,0x38, 0x0e,0x03,0x78,0x78},  //3   '%'
                             {0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00}   //4   ' '
                              }; 
  char width_limit;
  char ch[8];
  switch(c)
  {
  case ':':{c=0;width_limit=3; break;}
  case '-':{c=1;width_limit=3; break;}
  case '.':{c=2;width_limit=1; break;}
  case '%':{c=3;width_limit=4; break;}
  case ' ':{c=4;width_limit=4; break;}
  }
  
  for (i=0;i<8;i++) ch[i]=const_ch[c][i]; 
  
  set_cursor(oled_32_128_adr,x,y );
  for (i=0;i<width_limit;i++) send_repeat_data(oled_32_128_adr,ch[i],cc);
  send_repeat_data(oled_32_128_adr,0x00,cc);
  set_cursor(oled_32_128_adr,x,y+1);
  for (i=4;i<(4+width_limit);i++) send_repeat_data(oled_32_128_adr,ch[i],cc);
  send_repeat_data(oled_32_128_adr,0x00,cc);
  return ((width_limit*cc)+cc+x);
}

char oled_print_giga_char(char c,char x,char y)
{
  const char cc=3;
  const char const_ch[5][12]={{0x00,0x38,0x38,0x00, 0x00,0xc0,0xc0,0x00, 0x00,0x01,0x01,0x00},  //0   ':'
                              {0x00,0x00,0x00,0x00, 0x06,0x06,0x06,0x06, 0x00,0x00,0x00,0x00},  //1   '-'
                              {0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x0e,0x00,0x00},  //2   '.'
                              {0x1f,0x1f,0x80,0xf0, 0xf0,0x1e,0x87,0x80, 0x00,0x00,0x0f,0x0f},  //3   '%'
                              {0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00}   //4   ' '
                              }; 
  char pos;
  char ch[12];
	char i;
  switch(c)
  {
  case ':':{c=0; pos=16; break;}
  case '-':{c=1; pos=16; break;}
  case '.':{c=2; pos=12; break;}
  case '%':{c=3; pos=16; break;}
  case ' ':{c=4; pos=16; break;}
  }
  
  
  for (i=0;i<12;i++) ch[i]=const_ch[c][i]; 
  
  set_cursor(oled_32_128_adr,x,y );
  for (i=0;i<4;i++) send_repeat_data(oled_32_128_adr,ch[i],cc);
  if (c!=2) send_repeat_data(oled_32_128_adr,0,4);
  set_cursor(oled_32_128_adr,x,y+1);
  for (i=4;i<8;i++) send_repeat_data(oled_32_128_adr,ch[i],cc);
  if (c!=2) send_repeat_data(oled_32_128_adr,0,4);
  set_cursor(oled_32_128_adr,x,y+2);
  for (i=8;i<12;i++) send_repeat_data(oled_32_128_adr,ch[i],cc);
  if (c!=2) send_repeat_data(oled_32_128_adr,0,4);
  return (pos+x);
  }

void oled_print_terra_digit(char d,char x,char y)
{
	char i;
  const char c=6;
  const char const_dig[10][12]={{0xff,0x3f,0xff,0xff,0x00,0xff,0xff,0x00,0xff,0x3f,0x3f,0x3f},   //0
                                {0x00,0x00,0xff,0x00,0x00,0xff,0x00,0x00,0xff,0x00,0x00,0x3f},   //1  
                                {0x3f,0x3f,0xff,0xf0,0xf0,0xff,0xff,0x03,0x03,0x3f,0x3f,0x3f},   //2   
                                {0x3f,0x3f,0xff,0xf0,0xf0,0xff,0x03,0x03,0xff,0x3f,0x3f,0x3f},   //3 
                                {0xff,0x00,0xff,0xff,0xf0,0xff,0x03,0x03,0xff,0x00,0x00,0x3f},   //4 
                                {0xff,0x3f,0x3f,0xff,0xf0,0xf0,0x03,0x03,0xff,0x3f,0x3f,0x3f},   //5 
                                {0xff,0x3f,0x3f,0xff,0xf0,0xf0,0xff,0x03,0xff,0x3f,0x3f,0x3f},   //6 
                                {0x3f,0x3f,0xff,0x00,0x00,0xff,0x00,0x00,0xff,0x00,0x00,0x3f},   //7
                                {0xff,0x3f,0xff,0xff,0xf0,0xff,0xff,0x03,0xff,0x3f,0x3f,0x3f},   //8 
                                {0xff,0x3f,0xff,0xff,0xf0,0xff,0x03,0x03,0xff,0x3f,0x3f,0x3f}};  //9 
  char dig[12];
  for (i=0;i<12;i++) dig[i]=const_dig[d][i]; 
  
  set_cursor(oled_32_128_adr,x,y );
  for (i=0;i<3;i++) send_repeat_data(oled_32_128_adr,dig[i],c);
  set_cursor(oled_32_128_adr,x,y+1);
  for (i=3;i<6;i++) send_repeat_data(oled_32_128_adr,dig[i],c);
  set_cursor(oled_32_128_adr,x,y+2);
  for (i=6;i<9;i++) send_repeat_data(oled_32_128_adr,dig[i],c);
  set_cursor(oled_32_128_adr,x,y+3);
  for (i=9;i<12;i++) send_repeat_data(oled_32_128_adr,dig[i],c);
  }
  
char oled_print_graphic(char* data,char n,char x,char width)
{
  char i;
	char scale=1;
  char y;
  char max_data=0;
  char data_print;

  
  sendCmd(oled_32_128_adr,0x0f&x); 
  sendCmd(oled_32_128_adr,0x10|(0x0f&(x>>4)));
  switch (width)
  {
  case 1: 
    {
		for (i=0;i<n;i++) if(*(data+i)<32) {nop();}
         else if(max_data<(*(data+i))){max_data=*(data+i);}
      scale=(max_data/32)+1;
      
      for (i=0;i<n;i++) 
         {
           data_print=(*(data+i))/scale;
           y=3-(data_print/8);
           sendCmd(oled_32_128_adr,0xb0|y); 
           send_single_data(oled_32_128_adr,1<<(7-data_print%8));
         }
      break;
    }
   case 2: 
     {
		for (i=0;i<n;i++) if(*(data+i)<31) {nop();}
         else if(max_data<(*(data+i))){max_data=*(data+i);}
      scale=(max_data/31)+1;
      
      for (i=0;i<n;i++) 
         {
           data_print=(*(data+i))/scale;
           y=3-(data_print/8);
           if(y==(3-((data_print+1)/8)))
              {
               sendCmd(oled_32_128_adr,0xb0|y); 
              nop();
               send_single_data(oled_32_128_adr,3<<(6-data_print%8));
              }
             else
             { 
               sendCmd(oled_32_128_adr,0xb0|y); 
               send_single_data(oled_32_128_adr,1<<(7-data_print%8));
               y=(3-((data_print+1)/8));
                sendCmd(oled_32_128_adr,0x0f&(x+i)); //
                sendCmd(oled_32_128_adr,0x10|(0x0f&((x+i)>>4)));//
                sendCmd(oled_32_128_adr,0xb0|y); 
               send_single_data(oled_32_128_adr,1<<(7-(data_print+1)%8));
              }
         }
      break;      
    }
  }
  return scale;
}

////////////////////////////////
/*
char print_n_01(unsigned long int n, char x, char y ) //n=0..65535 right align x-right position
{ 
do
{
print_char((0x30+(n%10)),x,y); 
n/=10;
x--;
}
while(n>0); 
return (x);
}
      
char print_n_10(unsigned int n, char x, char y ) //n=0..65535 left align x-left position
{ 
unsigned int tn=0;
char fz=0;
if ((n%10)==0)  {n++;fz=1;}
if(n==1) print_char(0x30,x,y); 
  else 
  {
while(n>0) {tn=(tn*10)+(n%10); n/=10;}
while(tn>9) {print_char(0x30+(tn%10),x,y); tn/=10;x++;}
if(fz) print_char(0x30,x,y); else print_char(0x30+tn,x,y);
  }
x++;
return (x);
}
void print_n_00(unsigned long int n, char nn, char x, char y ) //n=0..65535 fixed format - nn diges x-left pos
{
char pos;  
pos=print_n_01(n,x+nn-1,y);
while(pos>=x) { print_char(0x30,pos,y);pos--;}
}
  
*/
////////////////////////////////
char oled_print_nomber(char f_zero,int n,char pos,char y, char font )	//9*15
{
char cnt0=1;  
int nt=n;
unsigned long int dec=1;

while((nt>=10)||(nt<=-10))
       {
         dec*=10;
         nt/=10;
         cnt0++;
       }
switch(font)
  {
   case MEGA:{
     
      while(f_zero>cnt0) {oled_print_mega_digit(0,pos,y);pos+=12;f_zero--;}
      if(n<0) {oled_print_mega_char('-',pos,y);pos+=12;n=0-n;}
      while(dec>0) 
       {
         oled_print_mega_digit(n/dec,pos,y);
         n%=dec;
         dec/=10;
         pos+=12;
       }
     break;}
   case GIGA:{
      if(n<0) {oled_print_giga_char('-',pos,y);pos+=16;n=0-n;}
			else {oled_print_giga_char(' ',pos,y);pos+=16;}
      while(f_zero>cnt0) {oled_print_giga_digit(0,pos,y);pos+=16;f_zero--;}
      
      while(dec>0) 
       {
         oled_print_giga_digit(n/dec,pos,y);
         n%=dec;
         dec/=10;
         pos+=16;
       }    
      break;}
  }
return pos;
}                                       