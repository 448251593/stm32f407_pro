#include "stdint.h"
#include "stdio.h"
#include  "spi_adc_driver.h"
#include  "spi_adc_app.h"
#include "main.h"
#include "tim9_driver.h"
#include "usart_app.h"
run_ctrl_struct     run_status_g = {0};
static uint32_t last_time_ticks = 0;
static uint32_t all_time_ticks = 0;
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
	all_time_ticks = get_global_tick();
}
void  adc_read_deal(void)
{
	if(run_status_g.status_s == 1)
	{
		run_status_g.status_s = 0;
		ittt = spi_adc_read();
		LOG_INFO("adc read =%d\n", ittt);
	}

}
#define      ADC_V_MAX_NUM    10
uint16_t    adc_v[ADC_V_MAX_NUM] = {0};
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
			LOG_INFO("end=%d\n", get_global_tick());
		}
		
	}
}

uint32_t  temp_tick;
uint16_t  sample_nums_count = 0;
void   get_adc_data_200khz(void)
{

	//uint16_t   adc_t = 0;
	uint8_t   sdt_ch[3] = {0};
	if (run_status_g.status_s == 1)
	{
		// sdt_ch[2] = ',0'?
		temp_tick = get_global_tick();
		if(temp_tick - last_time_ticks >= 5)
		{
			last_time_ticks = temp_tick;
			*((uint16_t *)sdt_ch) = spi_adc_read();
			Usart3SendData((char*)sdt_ch, 3);

			//add by bcg,2020-12-15 22:54:03 大于等于3000个数据,启动dma
			sample_nums_count++;
			if(sample_nums_count>=BUFFERSIZE/2)
			{
				sample_nums_count = 0;
				usart3send_flush();
				//run_status_g.status_s = 0;
			}
		}


		if(temp_tick - all_time_ticks >= 1000000)
		{
			run_status_g.status_s = 0;
		}
		
	}
}
void   print_adc_data(void)
{
	int i = 0;
	LOG_INFO("---\n");
	// printf("%04d,", adc_v[i]);
	for (i = 1; i < ADC_V_MAX_NUM+1; i++)
	{
		LOG_INFO("%04d,", adc_v[i-1]);
		if (i % 20 == 0)
		{
			LOG_INFO("\n");
		}
	}
	LOG_INFO("---\n");
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


