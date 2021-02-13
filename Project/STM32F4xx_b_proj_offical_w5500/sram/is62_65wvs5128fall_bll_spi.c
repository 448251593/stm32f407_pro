#include <stdio.h>
#include <string.h>
#include "main.h"
#include "def.h"
#include "global_data.h"
#include "function.h"
#include "is62_65wvs5128fall_bll_spi.h"

_doflash dosram, *p_dosram;
uint8_t is62_put_cmdhead(uint8_t cmd, uint32_t addr)
{
    do_flash->tx.tx_head[0] = cmd;
    do_flash->tx.tx_head[1] = addr >> 16;
    do_flash->tx.tx_head[2] = addr >> 8;
    do_flash->tx.tx_head[3] = addr;
    return 4;
}

uint8_t is62_set_mode(IS62_OPERATE_MODE_ENUM mode)
{
    uint8_t set_tx_data[2];
    set_tx_data[0] = IS62_65WVS5128_CMD_WRMR;
    set_tx_data[1] = mode;
    selected_spi_cs(sram_cs);
    HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 2, 1000);
    free_spi_cs(sram_cs);
    return 0;
}

uint8_t is62_get_mode(void)
{
    uint8_t set_tx_data[2], rx_data;
    set_tx_data[0] = IS62_65WVS5128_CMD_RDMR;
    selected_spi_cs(sram_cs);
    HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 1, 1000);
    HAL_SPI_Receive(&spi_sram, &rx_data, 1, 1000);
    free_spi_cs(sram_cs);
    return rx_data;
}
/**
* @Description:
* @Author: bcg
* @Date: 2020-09-12 11:41:46
* @param :
    rw:  1:read  ; 0:write
* @return:
*/
uint8_t is62_byte_read_write(uint32_t addr, uint8_t *pbuff, uint8_t rw)
{
    uint8_t set_tx_data[4];
    set_tx_data[0] = IS62_65WVS5128_CMD_READ;
    set_tx_data[1] = addr >> 16;
    set_tx_data[2] = addr >> 8;
    set_tx_data[3] = addr;

    if (addr > IS62_65WVS5128_DIE1_END_ADDR)
    {
        return 1; // page_index or word_offset err
    }

    selected_spi_cs(sram_cs);
    HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 4, 1000);
    if (rw == IS62_65WVS5128_READ_BYTE)
    {

        HAL_SPI_Receive(&spi_sram, pbuff, 1, 1000);
    }
    else
    {
        HAL_SPI_Transmit(&spi_sram, pbuff, 1, 1000);
    }
    free_spi_cs(sram_cs);
    return 0;
}

//add by bcg,2020-09-11 16:45:07
uint8_t is62_page_read(uint32_t page_index, uint32_t word_offset, uint8_t *pbuff, uint8_t p_size)
{
    uint8_t set_tx_data[4];
    set_tx_data[0] = IS62_65WVS5128_CMD_READ;
    set_tx_data[1] = page_index >> 16;
    set_tx_data[2] = page_index >> 8;
    set_tx_data[3] = word_offset;
    if (page_index > IS62_65WVS5128_MAX_PAGE_NUM || word_offset > IS62_65WVS5128_PAGE_SIZE)
    {
        return 1; // page_index or word_offset err
    }
    //add by bcg,2020-09-11 16:51:16 判断pbuff大小
    if (IS62_65WVS5128_PAGE_SIZE - word_offset > p_size)
    {
        return 2; // buf too short
    }

    selected_spi_cs(sram_cs);
    HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 4, 1000);
    HAL_SPI_Receive(&spi_sram, pbuff, p_size, 1000);
    free_spi_cs(sram_cs);
    return 0;
}
//add by bcg,2020-09-11 16:45:33
uint8_t is62_page_write(uint32_t page_index, uint32_t word_offset, uint8_t *pbuff, uint8_t p_size)
{
    uint8_t set_tx_data[4];
    set_tx_data[0] = IS62_65WVS5128_CMD_READ;
    set_tx_data[1] = page_index >> 16;
    set_tx_data[2] = page_index >> 8;
    set_tx_data[3] = word_offset;
    if (page_index > IS62_65WVS5128_MAX_PAGE_NUM || word_offset > IS62_65WVS5128_PAGE_SIZE)
    {
        return 1; // page_index or word_offset err
    }
    //add by bcg,2020-09-11 16:51:16 判断page剩余空间
    if (p_size > IS62_65WVS5128_PAGE_SIZE - word_offset)
    {
        return 2;
    }

    selected_spi_cs(sram_cs);
    HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 4, 1000);
    HAL_SPI_Transmit(&spi_sram, (uint8_t *)pbuff, p_size, 1000);
    free_spi_cs(sram_cs);
    return 0;
}

/**
* @Description:sequent page read
* @Author: bcg
* @Date: 2020-09-12 11:24:05
* @param :page_index 0 - 8191,8192-16383
* @return:
*/
uint8_t is62_page_sequent_read(uint32_t page_index, uint32_t word_offset, uint8_t *pbuff, uint8_t p_size)
{
    uint8_t set_tx_data[4];
    uint32_t die0_end_page, die0_bytes;

    set_tx_data[0] = IS62_65WVS5128_CMD_READ;
    set_tx_data[1] = page_index >> 16;
    set_tx_data[2] = page_index >> 8;
    set_tx_data[3] = word_offset;

    die0_end_page = (IS62_65WVS5128_MAX_PAGE_NUM / 2) - 1;
    die0_bytes = (die0_end_page - page_index) * 32;

    //add by bcg,2020-09-12 11:26:25
    if (p_size <= die0_bytes)
    {
        selected_spi_cs(sram_cs);
        HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 4, 1000);
        HAL_SPI_Receive(&spi_sram, pbuff, p_size, 1000);
        free_spi_cs(sram_cs);
    }
    else
    {
        /*
            //add by bcg,2020-09-12 11:27:38
            When the Address Pointer reaches the highest address of first die (3FFFFh), the address counter cannot cross to first
            address of 2nd die (40000h). Instead, it rolls over to (00000h). So the sequential operation must be terminated at the
            last address of first die (Die 0) by CS# HIGH, and begin new sequential operation from first address (40000h) of
            second die (Die 1) by CS# LOW.
        */

        selected_spi_cs(sram_cs);
        HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 4, 1000);
        HAL_SPI_Receive(&spi_sram, pbuff, die0_bytes, 1000);
        free_spi_cs(sram_cs);
        //delay some ticks
        set_tx_data[0] = IS62_65WVS5128_CMD_READ;
        set_tx_data[1] = (die0_end_page + 1) >> 16;
        set_tx_data[2] = (die0_end_page + 1) >> 8;
        set_tx_data[3] = 0;

        selected_spi_cs(sram_cs);
        HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 4, 1000);
        HAL_SPI_Receive(&spi_sram, (pbuff + die0_bytes), p_size - die0_bytes, 1000);
        free_spi_cs(sram_cs);
    }
    return 0;
}
/**
* @Description:sequent page write
* @Author: bcg
* @Date: 2020-09-12 11:45:32
* @param :
* @return:
*/
uint8_t is62_page_sequent_write(uint32_t page_index, uint32_t word_offset, uint8_t *pbuff, uint8_t p_size)
{
    uint8_t set_tx_data[4];
    uint32_t die0_end_page, die0_bytes;
    set_tx_data[0] = IS62_65WVS5128_CMD_WRITE;
    set_tx_data[1] = page_index >> 16;
    set_tx_data[2] = page_index >> 8;
    set_tx_data[3] = word_offset;

    die0_end_page = (IS62_65WVS5128_MAX_PAGE_NUM / 2) - 1;
    die0_bytes = (die0_end_page - page_index) * 32;

    //add by bcg,2020-09-12 11:26:25
    if (p_size <= die0_bytes)
    {
        selected_spi_cs(sram_cs);
        HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 4, 1000);
        HAL_SPI_Transmit(&spi_sram, pbuff, p_size, 1000);
        free_spi_cs(sram_cs);
    }
    else
    {
        /*
            //add by bcg,2020-09-12 11:27:38
            When the Address Pointer reaches the highest address of first die (3FFFFh), the address counter cannot cross to first
            address of 2nd die (40000h). Instead, it rolls over to (00000h). So the sequential operation must be terminated at the
            last address of first die (Die 0) by CS# HIGH, and begin new sequential operation from first address (40000h) of
            second die (Die 1) by CS# LOW.
        */

        selected_spi_cs(sram_cs);
        HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 4, 1000);
        HAL_SPI_Transmit(&spi_sram, pbuff, die0_bytes, 1000);
        free_spi_cs(sram_cs);
        //delay some ticks
        set_tx_data[0] = IS62_65WVS5128_CMD_WRITE;
        set_tx_data[1] = (die0_end_page + 1) >> 16;
        set_tx_data[2] = (die0_end_page + 1) >> 8;
        set_tx_data[3] = 0;

        selected_spi_cs(sram_cs);
        HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 4, 1000);
        HAL_SPI_Transmit(&spi_sram, (pbuff + die0_bytes), p_size - die0_bytes, 1000);
        free_spi_cs(sram_cs);
    }
		return 0;
}
