#ifndef _XWF_DEF_H_
#define _XWF_DEF_H_
#include "xwf_pin_map.h"


#define _DEBUG
#ifdef _DEBUG
  #define debug  debug0
#else
  #define debug   //
#endif
#define NODATA 0x55aa

#pragma pack(1) /*ָ����1�ֽڶ���*/

#define  spi_flash   SpiHandle[2] 
#define  spi_sram    SpiHandle[2]

#define  spi_adc     SpiHandle[0] 
#define  spi_net     SpiHandle[1] 

typedef struct
{
    unsigned protocal:1;  //10ms����
    unsigned run_led:1; //500ms����
    unsigned run_ad:1; //5��
    unsigned scan_key:1;  //100ms
    unsigned set_idle:1;  
    unsigned run_lldp:1;
    unsigned ex_led_check:1;
    unsigned can_to_send:1;
}_systick_task;


#define protocol_over_time     50u//100ms
#define run_led_time           1000u
#define run_ad_time            5000u
#define scan_key_time          500
#define test_overtime          10000u   //ÿ10������flash��ָ���Ĳ�������.
#define verify_ex_led          20   //


typedef struct
{
	uint8_t tx_head[4];
	uint8_t tx_data[256];
}_flash_tx;

typedef struct
{
  uint8_t   rx[4096];  //��ȡһ��������С
  _flash_tx tx;  //ҳ���
  uint16_t rxnum;
  uint16_t txnum;
  uint16_t current_sector;  //ÿ������һ����������
  uint16_t flash_Fsm;
  uint16_t delay_time;	//д������Ҫ����ȴ���ʱ��
  uint16_t time_out;
}_doflash;



#endif

