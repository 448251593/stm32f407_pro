#include "stdint.h"
#include "stdio.h"
#include  "spi_adc_driver.h"


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
	addata1 = sADC_ReadByte();
	ittt = addata1 << 2;
	ittt = ittt >> 4;
	ittt = (ittt * 3300 / 4096);
	return addata1;
}
