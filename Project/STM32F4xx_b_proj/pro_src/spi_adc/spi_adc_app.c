#include "stdint.h"
#include "stdio.h"
#include  "spi_adc_driver.h"
typedef struct 
{
	uint8_t     status_s;//add by bcg,2020-11-30 20:27:18 运行控制
	uint32_t    min_period;//add by bcg,2020-11-30 20:27:35 采样周期
	uint32_t    time_sustain;//add by bcg,2020-11-30 20:28:06 持续多长时间

}run_ctrl_struct;
run_ctrl_struct     run_status_g = {0};

void   spi_adc_init(void)
{

}

uint16_t addata1 = 0;
uint32_t ittt;
uint16_t spi_adc_read(void)
{

	// uint32_t i;
	addata1 = sADC_ReadByte();
	ittt = addata1 << 2;
	ittt = ittt >> 4;
	ittt = (ittt * 3300 / 4096);
	// addata1 = sADC_ReadByte();
	// ittt = addata1 << 2;
	// ittt = ittt >> 4;
	// ittt = (ittt * 3300 / 4096);
	return (uint16_t)ittt;
}
void  adc_read_start(void)
{
	run_status_g.status_s = 1;
}
void  adc_read_deal(void)
{
	if(run_status_g.status_s == 1)
	{
		run_status_g.status_s = 0;
		ittt = spi_adc_read();
		printf("adc read =%d\n", ittt);
	}

}





