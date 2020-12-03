#ifndef  _TIME9_DRIVER_H_
#define  _TIME9_DRIVER_H_



uint16_t   get_adc_data_finish(void);

void TIM9_IRQHandler_s(void);

int timer9_driver_init(void);
uint32_t get_global_tick(void);

#endif



