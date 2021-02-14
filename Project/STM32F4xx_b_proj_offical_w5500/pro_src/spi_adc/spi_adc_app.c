#include "stdint.h"
#include "stdio.h"
#include  "spi_adc_driver.h"
#include  "spi_adc_app.h"
#include "main.h"
#include "tim9_driver.h"
#include "usart_app.h"
#include "socket.h"
#include "loopback.h"
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
void  adc_read_start(void)
{
	run_status_g.status_s = 1;
	run_status_g.start_time_ticks = get_global_tick();
	sample_nums_count_all = 0;
	sample_nums_count = 0;
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
	sample_interval_count++;
	if(sample_interval_count > 2)
	{
		sample_interval_count = 0;
		
		return 1;
	}
	for(i = 0; i <  5; i++)
	{
		__NOP();
	}
	
	return 0;
}
	static uint16_t   addata1 = 0;
void   get_adc_data_200khz(void)
{
#if 1

	if (run_status_g.status_s == 1)
	{
		// if (get_adc_interval_check() == 0)
		// {
		// 	return;
		// }
		addata1 = sADC_ReadByte();
		// addata1 = (addata1 << 2) >> 4;
		// addata1 = (addata1>>8)+(addata1<<8);
		// addata1++;
		w5500_send_put((char*)&addata1, 2);

		// sample_nums_count++;
		sample_nums_count_all++;
		// __NOP();
		// if(sample_nums_count >= DATA_BUF_SIZE / 2)
		// {
		// 	sample_nums_count = 0;
		// 	w5500_send_flush();
		// }
		// if(sample_nums_count >= NET_SEND_BUF_SIZE / 2)
		// {
		// 	run_status_g.status_s = 0;
		// 	sample_nums_count = 0;
		// 	w5500_send_flush();
		// 	LOG_INFO("sample_nums_count_all=%d\n",sample_nums_count_all);
		// 	LOG_INFO ("end=%d\n",run_status_g.time_tick_ms);
		// }


		if(run_status_g.time_tick_ms - run_status_g.start_time_ticks >= run_status_g.time_sustain)
		{
			run_status_g.status_s = 0;
			w5500_send_flush();
			LOG_INFO("sample_nums_count_all=%d\n",sample_nums_count_all);
			LOG_INFO("sample_nums put fifo ok=%d\n",sample_nums_count);

			LOG_INFO ("starts->end (ms)=,%d-%d\n",run_status_g.start_time_ticks, run_status_g.time_tick_ms);
		}

	}
	#endif
}
void   print_run_param(void)
{
	char   tmp_buf[100];
	tmp_buf[0] = 0x7e;
	tmp_buf[1] = 0x7f;
	tmp_buf[2] = 0x7f;
	tmp_buf[3] = 0x7e;
	Usart3SendData((char*)tmp_buf, 4);
	Usart3SendData((char*)&run_status_g, sizeof(run_status_g));
	Usart3SendData((char*)tmp_buf, 4);

}

uint16_t   get_adc_data_finish(void)
{

		return 0;
	

}


