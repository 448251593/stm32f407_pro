#include "stdint.h"
#include "stdio.h"
#include  "spi_adc_driver.h"
#include  "spi_adc_app.h"
#include "main.h"
#include "tim9_driver.h"
#include "usart_app.h"
#include "socket.h"
#include "loopback.h"
#include "app_fifo.h"
run_ctrl_struct     run_status_g = {
	.status_s = 0,
	.min_period = 1,
	.time_sustain = 1000,//ms 
	.time_tick_ms = 0,
	.sample_nums_count_all = 0,
	.start_time_ticks = 0,
};

uint32_t  sample_nums_count = 0;
uint32_t   sample_nums_count_all = 0;
void   spi_adc_init(void)
{

}

uint32_t ittt;
//提高速度不在使用这个函数
uint16_t spi_adc_read(void)
{

uint16_t addata1 = 0;
	// uint32_t i;
	// 0000 0000- 0000 0000
	addata1 = sADC_ReadByte();
	
	addata1 = (addata1 << 2) >> 4;
	return addata1;

	// ittt = addata1 << 2;
	// ittt = ittt >> 4;
	// // ittt = (ittt * 3300 / 4096);
	// return (uint16_t)ittt;
}

void  adc_read_stop(void)
{
	run_status_g.status_s = 2;

}
static uint16_t addata1 = 0;
static uint32_t ad_data_average = 0;

void  adc_read_start(void)
{
	run_status_g.status_s = 1;
	run_status_g.start_time_ticks = get_global_tick();
	sample_nums_count_all = 0;
	sample_nums_count = 0;

	sADC_CS_LOW();
	ad_data_average = sADC_SendByte(sADC_DUMMY_BYTE);
	sADC_CS_HIGH();
	if (ad_data_average > 2048)
	{
		ad_data_average = ad_data_average - 2048;
	}
	else
	{
		ad_data_average = 2048 - ad_data_average;
	}
#if USART_DMA_TX_ENABEL
	dma_start_times = 0;
#endif
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


uint32_t  temp_tick;

uint32_t  sample_interval_count = 0;
uint8_t  get_adc_interval_check(void)
{
	
	int i;

	for(i = 0; i <  95; i++)
	{
		__NOP();
	}
	
	return 1;
}




void get_adc_data_200khz(void)
{
#if 1
	uint16_t templen = 0;

	if (run_status_g.status_s == 1)
	{
		if (get_adc_interval_check() == 0)
		{
			return;
		}
		//不调用函数.直接通过 spi 读取数据
		sADC_CS_LOW();
		addata1 = sADC_SendByte(sADC_DUMMY_BYTE);
		sADC_CS_HIGH();
		addata1 = (addata1 << 2) >> 4;
		// addata1 = (addata1 >> 8) + (addata1 << 8);
		if(addata1 > 2048)
		{
			addata1 = addata1 - 2048;
		}
		else
		{
			addata1 =  2048 - addata1;
		}
		

		ad_data_average = (ad_data_average + addata1);
		sample_nums_count_all++;
#if FIFO_SELECT
		// 使用优化过的接口存放数据
		if (sample_nums_count_all % 20000 == 0)
		{
			ad_data_average = ad_data_average / 20000;
			ft_fifo_put_ext(&spi_net_send_Fifo, (uint8_t *)&ad_data_average);
			#if 0
			templen = ft_fifo_put(&spi_net_send_Fifo, (char *)&addata1, 2);
			if (templen > 0)
			{
			sample_nums_count++;
			}
			#endif
		}
#else

		app_fifo_put(&spi_net_send_ff, (addata1));
		// app_fifo_put(&spi_net_send_ff, (uint8_t)(addata1 >> 8));
#endif

		// __NOP();
		// 调试校对 1s采样数据用
		// if ( run_status_g.time_tick_ms - run_status_g.start_time_ticks > 1000)
		// {
		// 	run_status_g.status_s = 2;
		// }
	}
	else if (run_status_g.status_s == 2 )
	{
		run_status_g.status_s = 0;//停止后状态修改为0
		run_status_g.end_time_ticks = run_status_g.time_tick_ms;
		run_status_g.sample_nums_count_all = sample_nums_count_all;
		LOG_INFO("sample_nums_count_all=%d\n",sample_nums_count_all);
		LOG_INFO ("starts->end (ms)=,%d-%d=%d\n",
		run_status_g.start_time_ticks, run_status_g.time_tick_ms,
		run_status_g.end_time_ticks - run_status_g.start_time_ticks);
		sample_nums_count_all = 0;
		w5500_send_flush();
	}
	#endif
}


uint16_t   get_adc_data_finish(void)
{

		return 0;
	

}


