#if 0
// #include "app/fpga.h"
// #include "app/def.h"
// #include "app/data.h"
// #include "app/function.h"
#include <stdio.h>
#include <string.h>
#include "Mx25L6406e.h"
#include "spi_flash_driver.h"

extern _doflash doflash,*do_flash;
extern spi_instance_t _flash_core_spi;

void earse_flash(uint16_t sector,uint16_t num) //sector=0xffff block earse
{
uint8_t cmd[4];
uint32_t add,i;
wait_WEL_Reset();
//read_status_reg();
do_flash->flash_Fsm=flash_writeing;  //准备写

if(sector==0xffff)
    {
     enable_WR_flash();
	 cmd[0]=MX25_ERASE_CHIP;
     Active_flash();
#if OLD_CODE_EN
     SPI_transfer_block (flash_core_spi,
	                 cmd,
                     1,
                     0,
                     0);
#else
     SPI_transfer_block (
	                 cmd,
                     1,
                     0,
                     0);
#endif
     Deact_flash();
	 wait_WEL_Reset();
    }
else
   {
     for(i=0;i<num;i++)
     {
      enable_WR_flash();
	  add = (sector+i)*4096;
      cmd[0]=MX25_ERASE_SECTOR;
      cmd[1]= add>>16;
      cmd[2]= add>>8;
      cmd[3]= add;
      Active_flash();
#if OLD_CODE_EN
      SPI_transfer_block (flash_core_spi,
	                 cmd,
                     4,
                     0,
                     0);
#else
      SPI_transfer_block (
	                 cmd,
                     4,
                     0,
                     0);
                     #endif
      Deact_flash();
      wait_WEL_Reset();
     }
   }

 do_flash->flash_Fsm=flash_wait_writed;  //等待写完
 do_flash->flash_Fsm=flash_idle;
 debug("earse ok",NODATA);
}



//void write_status_reg()
//{
//uint8_t cmd[2];
//cmd[1]=read_status_reg()|0x02;
//cmd[0]=MX25_WRITE_STATUS;
//enable_WR_flash();
//read_status_reg();
//Active_flash();
//HAL_SPI_Transmit(&SpiHandle[1],cmd,2,100);
//Deact_flash();
//read_status_reg();
//}


/*读取指定位置的数据*/
void NOTDMAreadflash(uint32_t addr,uint16_t rd_len)
{
 do_flash->txnum=put_cmdhead(MX25_READ_DATA,addr);  //4
 do_flash->rxnum=rd_len;
 wait_WEL_Reset();
 Active_flash();
 #if OLD_CODE_EN
// HAL_SPI_Transmit(&SpiHandle[1], (uint8_t *)&(fptr->tx),fptr->txnum,1000);
// HAL_SPI_Receive(&SpiHandle[1], fptr->rx, fptr->rxnum,1000);
 SPI_transfer_block (flash_core_spi,
	                 (uint8_t *)&do_flash->tx,
                     do_flash->txnum,
                     do_flash->rx,
                     rd_len);
#else
 SPI_transfer_block (
	                 (uint8_t *)&do_flash->tx,
                     do_flash->txnum,
                     do_flash->rx,
                     rd_len);
#endif
 Deact_flash();
}

void NOTDMA_PAGEWR(void) //len==256+4
{
 wait_WEL_Reset();
 enable_WR_flash();
 Active_flash();
#if OLD_CODE_EN
SPI_transfer_block (flash_core_spi,
                  (uint8_t *)&do_flash->tx,
                    do_flash->txnum,
                    0,
                    0);

#else
SPI_transfer_block (
                  (uint8_t *)&do_flash->tx,
                    do_flash->txnum,
                    0,
                    0);

#endif
Deact_flash();
//delay_ms(2);
}


/*修改某扇区中某一页*/
void ModifyPageProgram(uint32_t addr,uint8_t* pdata)//addr=页的起点地址
{
uint32_t page,i;
uint16_t sector=addr/4096;
NOTDMAreadflash(sector*4096,4096); //程序中执行put_head
earse_flash(sector,1);//擦除当前扇区
page=addr%4096;
for(i=0;i<256;i++)
  {
  do_flash->rx[page+i]=*pdata++;
  }
for(i=0;i<8;i++)
  {
  PageProgram((sector*4096+i*256),&do_flash->rx[0+i*256]);
  }
}

void PageProgram(uint32_t addr,uint8_t* pdata)
{
  uint32_t i;
 for(i=0;i<256;i++)
  {
  do_flash->tx.tx_data[i]=*pdata++;
  }
 do_flash->rxnum=0;
 do_flash->txnum=put_cmdhead(MX25_PAGE_PROGRAM,addr)+256;
 //UART_Send((uint8_t*)&fptr->tx.tx_head[0],sizeof(_flash_tx));
 NOTDMA_PAGEWR();
}


void copy_task_to_flash(uint8_t *string,uint16_t len)
{
//uint16_t len=strlen((char *)string);
uint8_t j,i=len/256;
uint32_t addr;
if(search_task_from_flash(string))
{
debug("该任务已存在，配置失败",NODATA);
return;
}
do_flash->current_sector=search_current_sector();  //随机查找一个空扇区
if(do_flash->current_sector!=0)
  {
addr=do_flash->current_sector*4096;
if(len%256!=0)i=i+1;
for(j=0;j<i;j++)
  {
  PageProgram((addr+j*256),(string+j*256));
  }
debug("任务配置成功",NODATA);
  }
}

void copy_ram_to_page(uint8_t *buf,uint32_t addr) //addr必须是256整数倍
{
uint16_t j;
NOTDMAreadflash(addr,256);  //0扇区第一页 RJ45CONFIG
for(j=0;j<256;j++)
  {
   if(do_flash->rx[j]!=0xff)
   {
   ModifyPageProgram(addr,buf);
   return;
   }
  }
PageProgram((addr),(buf));
}



void test_flash(unsigned char cmd)
{
uint8_t read_buf[2];
switch(cmd)
 {
 case MX25_READ_ID:
	  Active_flash();
#if OLD_CODE_EN
      SPI_transfer_block (flash_core_spi,
	                      &cmd,
                          1,
                           read_buf,
                           sizeof(read_buf));

#else
      SPI_transfer_block (
	                      &cmd,
                          1,
                           read_buf,
                           sizeof(read_buf));
#endif
     Deact_flash();
   //  debug("ID",read_buf[0]);
    break;
 }
}

void upload_task_all()
{
uint32_t i=task_sd;
for(;i<2048;i++)
   {
   NOTDMAreadflash(i*4096,4);
   if(!strncmp(do_flash->rx,"task",4))  //不相等=1
      {
	  irq_send_uart(do_flash->rx,0);
	  NOTDMAreadflash(i*4096,4096);
	  irq_send_uart(do_flash->rx,strlen(do_flash->rx));
      }
   }
}
void upload_task_name(uint8_t* name)  //将改变ftp->rx的值，影响_result
{
if(search_task_from_flash(name))
irq_send_uart(do_flash->rx,strlen(do_flash->rx));
}

#endif
