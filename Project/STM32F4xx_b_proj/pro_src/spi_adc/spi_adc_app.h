#ifndef  __SPI_ADC_APP_H__
#define  __SPI_ADC_APP_H__

typedef struct
{
	uint8_t     status_s;//add by bcg,2020-11-30 20:27:18 运行控制
	uint32_t    min_period;//add by bcg,2020-11-30 20:27:35 采样周期
	uint32_t    time_sustain;//add by bcg,2020-11-30 20:28:06 持续多长时间 us
	uint32_t 	time_tick_ms;//add by bcg,2020-12-20 19:31:39  sysytem ticks ms
}run_ctrl_struct;
extern run_ctrl_struct     run_status_g;
uint16_t spi_adc_read(void);
void  adc_read_start(void);

void  adc_read_deal(void);

void   get_adc_data(void);

void   print_adc_data(void);
void   get_adc_data_200khz(void);


#endif
