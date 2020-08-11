#include "stm8s.h"

#define PORT_enc GPIOA
#define pin_enc_l    1
#define bit_enc_l                       (1<<pin_enc_l)
#define pin_enc_r    2
#define bit_enc_r                       (1<<pin_enc_r)

typedef struct {
	char cnt;
	char f_push:1;
	char button :1;
	signed char enc_data;
} encoder_t;

void init_encoder(void);
void encoder_handler(encoder_t* enc);
