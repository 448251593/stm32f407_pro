#if 1

#define OLD_CODE_EN 0

#if OLD_CODE_EN
#include "app/fpga.h"
#include "app/def.h"
#include "app/data.h"
#include "app/function.h"
#endif
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "Mx25L6406e.h"
#include "spi_flash_driver.h"

// spi_instance_t _flash_core_spi;

void Mx2564_flash_init(void)
{
	const char first[] = "s";

	// SPI_init( flash_core_spi, CORESPI_flash_ADDR, 32 );
	// SPI_configure_master_mode( flash_core_spi );
	//SPI_set_slave_select( flash_core_spi, SPI_SLAVE );
	disable_WR_flash();
		#if 0
	test_flash(MX25_READ_ID);

	do_flash = &doflash;

	NOTDMAreadflash(sys_record * 4096, 256); //读取第0扇区配置信息

	if (strcmp((const char *)do_flash->rx, first) != 0) //第一次启动
	{
		debug("first poweron", NODATA);
		earse_flash(0xffff, 2048); //earse sector
		earse_flash(sys_record, 1);
		PageProgram((sys_record * 4096 + 0), (uint8_t *)first);
		delay_ms(100);
		NVIC_SystemReset();
	}
	else
	{
		do_flash->current_sector = search_current_sector();
		NOTDMAreadflash(dev_para_addr, 256); //0扇区第一页 RJ45CONFIG
		set_rj45_config(do_flash->rx);
		debug(first, NODATA);
	}
	#endif
}

void Active_flash(void)
{
#if OLD_CODE_EN
	SPI_set_slave_select(flash_core_spi, flash_cs);
#else
	//add by bcg,2020-09-02 10:00:17
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
#endif
}

void Deact_flash(void)
{
#if OLD_CODE_EN
// SPI_clear_slave_select( flash_core_spi, flash_cs );
#else
	//add by bcg,2020-09-02 10:02:17
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
#endif
}

void enable_WR_flash(void)
{
	uint8_t cmd = MX25_WRITE_ENABLE;

	Active_flash();

#if OLD_CODE_EN
	SPI_transfer_block(flash_core_spi,
					   &cmd,
					   1,
					   0,
					   0);
#else
	sFLASH_SendByte(cmd);
#endif

	Deact_flash();
}

void disable_WR_flash(void)
{
	uint8_t cmd = MX25_WRITE_DISABLE;
	Active_flash();
#if OLD_CODE_EN
	SPI_transfer_block(flash_core_spi,
					   &cmd,
					   1,
					   0,
					   0);
#else
	sFLASH_SendByte(cmd);
#endif
	Deact_flash();
}

uint8_t read_status_reg(void)
{
	uint8_t status;
	uint8_t cmd = MX25_READ_STATUS;
	Active_flash();

#if OLD_CODE_EN
	SPI_transfer_block(flash_core_spi,
					   &cmd,
					   1,
					   &status,
					   1);
#else
	sFLASH_SendByte(cmd);
#endif

	Deact_flash();
	//debug("flashRDSR",(uint16_t)status);
	return status;
}

void wait_WEL_Reset(void)
{
	uint8_t status = 0;
	while (1)
	{
#if OLD_CODE_EN
		status = read_status_reg();
#else
		status = sFLASH_ReadByte();
#endif
		status = status & 0x03;
		if (status == 0)
			break;
	}
}

#if 0
uint8_t put_cmdhead(uint8_t cmd, uint32_t addr)
{

	do_flash->tx.tx_head[0] = cmd;
	do_flash->tx.tx_head[1] = addr >> 16;
	do_flash->tx.tx_head[2] = addr >> 8;
	do_flash->tx.tx_head[3] = addr;

	return 4;
}

void del_flash_task(uint8_t *taskname)
{
	uint32_t i = task_sd;

	for (; i < 2048; i++)
	{
		NOTDMAreadflash(i * 4096, 20);
		if (!strncmp(do_flash->rx, taskname, strlen(taskname))) //不相等=1
		{
			earse_flash(i, 1);
			debug("该任务已擦除", NODATA);
			return;
		}
	}
	if (i == 2048)
	{
		debug("没有该任务", NODATA);
	}
}

uint16_t search_current_sector(void)
{
	uint32_t i = task_sd;

	for (; i < 2048; i++)
	{
		NOTDMAreadflash(i * 4096, 4);
		if (strncmp(do_flash->rx, "task", 4)) //不相等=1
		{
			return i;
		}
	}
	if (i == 2048)
	{
		debug("任务写满,请删除一些任务", NODATA);
		// earse_flash(1,2048-1);
		return 0;
	}
}

uint8_t *search_task_from_flash(uint8_t *name)
{
	uint32_t i = task_sd;
	for (; i < 2048; i++)
	{
		NOTDMAreadflash(i * 4096, (nameLen + 5));
		if (!strncmp(do_flash->rx, name, (nameLen + 5)))
		{
			NOTDMAreadflash(i * 4096, 4096);
			return do_flash->rx;
		}
	}
	if (i == 2048)
	{
		debug("该任务还没有配置", NODATA);
	}
	return 0;
}

void flash_self_check()
{
	uint32_t i, addr;
	addr = 3 * 4096;
	earse_flash(3, 1);
	NOTDMAreadflash(addr, 256);
	irq_send_uart(do_flash->rx, 256);
	for (i = 0; i < 256; i++)
		do_flash->tx.tx_data[i] = i;
	do_flash->rxnum = 0;
	do_flash->txnum = put_cmdhead(MX25_PAGE_PROGRAM, addr) + 256;
	NOTDMA_PAGEWR();
	NOTDMAreadflash(addr, 256);
	irq_send_uart(do_flash->rx, 256);
}
#endif

#endif
