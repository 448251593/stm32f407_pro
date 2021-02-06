#ifndef _DATA_H_
#define _DATA_H_

#ifdef ALLOCATE_EXTERN
  #define EXTERN
  #else
  #define EXTERN extern
#endif

//EXTERN  uint8_t coil[256],rx_tail;
//EXTERN  uint16_t send[65536];
//EXTERN  uint8_t protocol_overTime;
//EXTERN  uint16_t time_1ms;

////EXTERN  SPI_HandleTypeDef SpiHandle[3];

//EXTERN  uint16_t ad_data[65536],sample;

//EXTERN _systick_task systick_do_task;

//EXTERN _doflash doflash,*do_flash;

#endif
