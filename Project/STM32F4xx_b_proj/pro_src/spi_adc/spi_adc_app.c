#include "stdint.h"
#include "stdio.h"
#include  "spi_adc_driver.h"
#include  "spi_adc_app.h"
#include "main.h"
#include "tim9_driver.h"
#include "usart_app.h"
run_ctrl_struct     run_status_g = {
	.status_s = 0,
	.min_period = 1,
	.time_sustain = 100,//ms 
};
static uint32_t last_time_ticks = 0;
static uint32_t all_time_ticks = 0;
uint32_t   sample_nums_count_all = 0;
void   spi_adc_init(void)
{

}

uint32_t ittt;
uint16_t spi_adc_read(void)
{

	uint16_t addata1 = 0;
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
	sample_nums_count_all = 0;
	dma_start_times = 0;
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
uint32_t  sample_interval_count = 0;
uint8_t  get_adc_interval_check(void)
{
	sample_interval_count++;
	if(sample_interval_count > 5)
	{
		sample_interval_count = 0;
		
		return 1;
	}
	// __NOP();


	
	return 0;
}
void   get_adc_data_200khz(void)
{

	//uint16_t   adc_t = 0;
	uint8_t   sdt_ch[3] = {0};
	if (run_status_g.status_s == 1)
	{
		if (get_adc_interval_check() == 0)
		{
			return;
		}
		// sdt_ch[2] = ',0'?
		temp_tick = get_global_tick();
		// if(temp_tick - last_time_ticks >= run_status_g.min_period)
		// {
		// 	last_time_ticks = temp_tick;
			*((uint16_t *)sdt_ch) = spi_adc_read();
			sdt_ch[2] = sdt_ch[0];//add by bcg,2020-12-20 11:09:38 把高位调换到低位,改为大端发送
			Usart3SendData((char*)&sdt_ch[1], 2);

			//add by bcg,2020-12-15 22:54:03 大于等于BUFFERSIZE/2个数据,启动dma
			sample_nums_count++;
			sample_nums_count_all++;
			if(sample_nums_count>=BUFFERSIZE/2)
			{
				sample_nums_count = 0;
				usart3send_flush();
			}
		// }


		if(temp_tick - all_time_ticks >= run_status_g.time_sustain)
		{
			run_status_g.status_s = 0;
			sample_nums_count = 0;
			Usart3SendData((char*)&sample_nums_count_all, 4);
			usart3send_flush();
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


