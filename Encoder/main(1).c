/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 */
#include "stm8s.h"
#include "main.h"
encoder_t encoder;

main()
{
	init_encoder();
	while (1)
	{
		encoder_handler(&encoder);
		if (encoder.enc_data>=10)
	{nop();}
		if (encoder.enc_data<=-10)
	{nop();}
	
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
