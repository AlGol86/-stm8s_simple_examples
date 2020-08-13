/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 */
#include "stm8s.h"
#include "main.h"
encoder_t encoder;

main()
{
	int i;
	char pos;
	
	init_encoder();
	init_tim1();
	init_ssd1306_2(address_iic_oled);	

	while (1)
	{
		
		
		
		if (encoder.enc_data>=10)
	{nop();}
		if (encoder.enc_data<=-10)
	{nop();}
  pos=oled_print_nomber(3,encoder.enc_data,1,1,GIGA );
	//oled_print_giga_char(' ',pos,1);
	}
	
	
}

void init_encoder(void)
{
	PORT_enc->CR2&=~(bit_enc_l|bit_enc_r);
	PORT_enc->DDR&=~(bit_enc_l|bit_enc_r);
	PORT_enc->CR1|=(bit_enc_l|bit_enc_r);
}

void encoder_handler(encoder_t* enc)
{ char cnt_lim=7;
	if(((PORT_enc->IDR&bit_enc_l)==0) && (enc->cnt<cnt_lim)) enc->cnt++;
else	if (enc->cnt>0) enc->cnt--;
  if ((enc->cnt>(cnt_lim-2))&&(enc->f_push==0)) {
	enc->f_push=1;
	if((PORT_enc->IDR&bit_enc_r)==0) enc->enc_data++; else enc->enc_data--;
  }
	if (enc->cnt==0)
	enc->f_push=0;
}

void init_tim1(void)
{
	TIM1->IER|=TIM1_IER_UIE;

  TIM1->PSCRH=0;
	TIM1->PSCRL=1; 
	TIM1->ARRH=0; 
  TIM1->ARRL=200;
	TIM1->CNTRH=0;
	TIM1->CNTRL=0;
	rim();
	TIM1->CR1|=TIM1_CR1_CEN;
}
