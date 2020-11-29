#ifndef _xwf_PIN_H_
#define _xwf_PIN_H_
 
#define LED_Y            GPIO_PIN_15
#define LED_O            GPIO_PIN_14
#define LED_PORT         GPIOH

#define mx_cs            GPIO_PIN_15
#define mx_cs_port       GPIOA
#define mx_hold          GPIO_PIN_9
#define mx_hold_port     GPIOC

#define sram_cs          GPIO_PIN_8
#define sram_cs_port     GPIOC

#define eth_rst          GPIO_PIN_4
#define eth_rst_port     GPIOI
#define eth_cs           GPIO_PIN_0
#define eth_cs_port      GPIOI

#define ad_cs            GPIO_PIN_4
#define ad_cs_port       GPIOA

//spi pin
#define adc_spiclk       GPIO_PIN_5
#define adc_spimiso      GPIO_PIN_6
#define adc_spiport      GPIOA


#endif
