#include "stdint.h"
#include "stdio.h"
#include  "spi_adc_driver.h"
#include  "spi_adc_app.h"

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
#define      ADC_V_MAX_NUM    1000
uint16_t    adc_v[ADC_V_MAX_NUM] = 0;
uint16_t    adc_v_index = 0;
void   get_adc_data(void)
{
	if (run_status_g.status_s == 1)
	{
		if (adc_v_index < ADC_V_MAX_NUM)
		{
			adc_v_index++;
			adc_v[adc_v_index] = spi_adc_read();
		}
		else
		{
			run_status_g.status_s = 0;
			adc_v_index = 0;
			printf("end=%d\n", get_global_tick());
		}
		
	}
}
void   print_adc_data(void)
{
	int i;
	printf("---\n");
	for (i = 0; i < ADC_V_MAX_NUM; i++)
	{
		printf("%04d,", adc_v[i]);
		if(i%20==0)
		{
			printf("\n");
		}
	}
	printf("---\n");
}

uint16_t   get_adc_data_finish(void)
{
	if(adc_v_index == 1000)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}


