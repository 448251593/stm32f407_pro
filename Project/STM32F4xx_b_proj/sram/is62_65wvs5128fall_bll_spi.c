#include <stdio.h>
#include <string.h>
#include "main.h"
#include "def.h"
#include "global_data.h"
#include "function.h"
#include "is62-65wvs5128fall-bll_spi.h"

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
}

uint8_t is62_get_mode(void)
{
    uint8_t set_tx_data[2], rx_data;
    set_tx_data[0] = IS62_65WVS5128_CMD_RDMR;
    selected_spi_cs(sram_cs);
    HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 1, 1000);
    HAL_SPI_Receive(&spi_sram, rx_data, 1,1000);
    free_spi_cs(sram_cs);
    return rx_data;
}
//add by bcg,2020-09-11 16:45:07
uint8_t is62_page_read(uint32_t  page_index, uint32_t word_offset, uint8_t *pbuff, uint8_t p_size)
{
    uint8_t set_tx_data[4];
    set_tx_data[0] = IS62_65WVS5128_CMD_READ;
    set_tx_data[1] = page_index >> 16;
    set_tx_data[2] = page_index >> 8;
    set_tx_data[3] = word_offset;
    if ( page_index  > IS62_65WVS5128_MAX_PAGE_NUM || word_offset > IS62_65WVS5128_PAGE_SIZE)
    {
        return 1; // page_index or word_offset err
    }
     //add by bcg,2020-09-11 16:51:16 判断pbuff大小
    if( IS62_65WVS5128_PAGE_SIZE - word_offset > p_size)
    {
        return 2;// buf too short
    }


    selected_spi_cs(sram_cs);
    HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 4, 1000);
    HAL_SPI_Receive(&spi_sram, pbuff, p_size,1000);
    free_spi_cs(sram_cs);
    return 0;
}
//add by bcg,2020-09-11 16:45:33
uint8_t is62_page_write(uint32_t  page_index, uint32_t word_offset, uint8_t *pbuff, uint8_t p_size)
{
    uint8_t set_tx_data[4];
    set_tx_data[0] = IS62_65WVS5128_CMD_READ;
    set_tx_data[1] = page_index >> 16;
    set_tx_data[2] = page_index >> 8;
    set_tx_data[3] = word_offset;
    if ( page_index  > IS62_65WVS5128_MAX_PAGE_NUM || word_offset > IS62_65WVS5128_PAGE_SIZE)
    {
        return 1; // page_index or word_offset err
    }
    //add by bcg,2020-09-11 16:51:16 判断page剩余空间
    if(p_size > IS62_65WVS5128_PAGE_SIZE - word_offset)
    {
        return 2;
    }

    selected_spi_cs(sram_cs);
    HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 4, 1000);
    HAL_SPI_Transmit(&spi_sram, (uint8_t *)pbuff, p_size, 1000);
    free_spi_cs(sram_cs);
    return 0;
}

uint8_t is62_page_sequent_read(uint32_t  page_index, uint32_t word_offset, uint8_t *pbuff, uint8_t p_size)
{
    uint8_t set_tx_data[4];
    uint32_t page_index_tmp = 0, t,j,m;
    set_tx_data[0] = IS62_65WVS5128_CMD_READ;
    set_tx_data[1] = page_index >> 16;
    set_tx_data[2] = page_index >> 8;
    set_tx_data[3] = word_offset;


    page_index_tmp = page_index ;
    if(p_size >  (IS62_65WVS5128_PAGE_SIZE - word_offset))
    {
        t = p_size - (IS62_65WVS5128_PAGE_SIZE - word_offset);
        j = t / IS62_65WVS5128_PAGE_SIZE;
        if (t % IS62_65WVS5128_PAGE_SIZE > 0)
        {
            j = j + 1;
        }
        page_index_tmp = page_index_tmp + j;
    }



    if ((page_index <= (IS62_65WVS5128_MAX_PAGE_NUM/2)) && page_index_tmp > (IS62_65WVS5128_MAX_PAGE_NUM/2))
    {
        /* code */
    }

    if ( page_index  > IS62_65WVS5128_MAX_PAGE_NUM || word_offset > IS62_65WVS5128_PAGE_SIZE)
    {
        return 1; // page_index or word_offset err
    }
     //add by bcg,2020-09-11 16:51:16
    if( IS62_65WVS5128_PAGE_SIZE - word_offset > p_size)
    {
        return 2;// buf too short
    }


    selected_spi_cs(sram_cs);
    HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 4, 1000);
    HAL_SPI_Receive(&spi_sram, pbuff, p_size,1000);
    free_spi_cs(sram_cs);
    return 0;
}

uint8_t is62_page_sequent_write(uint32_t  page_index, uint32_t word_offset, uint8_t *pbuff, uint8_t p_size)
{
    uint8_t set_tx_data[4];
    set_tx_data[0] = IS62_65WVS5128_CMD_READ;
    set_tx_data[1] = page_index >> 16;
    set_tx_data[2] = page_index >> 8;
    set_tx_data[3] = word_offset;
    if ( page_index  > IS62_65WVS5128_MAX_PAGE_NUM || word_offset > IS62_65WVS5128_PAGE_SIZE)
    {
        return 1; // page_index or word_offset err
    }
    //add by bcg,2020-09-11 16:51:16 判断page剩余空间
    if(p_size > IS62_65WVS5128_PAGE_SIZE - word_offset)
    {
        return 2;
    }

    selected_spi_cs(sram_cs);
    HAL_SPI_Transmit(&spi_sram, (uint8_t *)set_tx_data, 4, 1000);
    HAL_SPI_Transmit(&spi_sram, (uint8_t *)pbuff, p_size, 1000);
    free_spi_cs(sram_cs);
    return 0;
}























