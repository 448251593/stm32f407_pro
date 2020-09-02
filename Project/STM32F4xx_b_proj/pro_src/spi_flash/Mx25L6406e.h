
#ifndef __MX25_L6406E__
#define __MX25_L6406E__

#define CORESPI_flash_ADDR   0x7D000000U
#define flash_core_spi  &_flash_core_spi
#define flash_cs        SPI_SLAVE_0

/******************
 Function MARCO Defined Start
*******************/
typedef enum
{
firmware_sd=             0,  //升级APP首扇区
firmware_ed=             24, //升级APP末扇区  //程序最大100K
updata_flag_addr=        (firmware_ed*4096+15*256), //end page 升级标志位
sys_record =             (firmware_ed+1),
dev_para_addr=           (sys_record*4096+256),
adjust_factor_addr=      (dev_para_addr+256),
task_sd=                 (sys_record+1),
task_ed=                 2047,
}_flash_record;

//#define MX25_WRITE_REG_READ_BACK 
#define recordhead_startADD  4096
#define recordhead_endADD    (4096*2)

#define MX25_SECTOR_COUNTER    (2048)  //扇区个数
#define MX25_SECTOR_SIZE           (0x1000)  //扇区大小
#define MX25_BANK_COUNTER        (128)
#define MX25_SECTORS_PER_BANK (16)
#define MX25_BANK_SIZE 			((MX25_SECTOR_COUNTER*MX25_SECTOR_SIZE)/MX25_BANK_COUNTER)



/****Status Register define***/
#define MX25_STATUS_SRWD_MASK 		 (0x01<<0x07)
#define MX25_STATUS_SRWD_OFFSET		 (0x07)
#define MX25_STATUS_PROTECT_MASK 	 (0x0f<<0x02)
#define MX25_STATUS_PROTECT_OFFSET  	 (0x02)
#define MX25_STATUS_WEL_MASK     		 (0x01<<0x01)
#define MX25_STATUS_WEL_OFFSET             (0x01)
#define MX25_STATUS_IDLE_MASK    		 (0x01<<0x00)

/*******Spi Command define**************/

typedef enum
{
	MX25_COMMAND_START,
	MX25_WRITE_STATUS = 0x01,
	MX25_WRITE_ENABLE = 0x06,
	MX25_WRITE_DISABLE = 0x04,
	MX25_READ_ID = 0x9F,
	MX25_READ_STATUS = 0x05,
	MX25_READ_DATA = 0x03,
	MX25_FAST_READ_DATA = 0x0B,
	MX25_READ_SFDP = 0x5A,
	MX25_READ_EID = 0xAB,
	MX25_READ_EMD_ID = 0x90,
	MX25_READ_DOUBLE_OUTPUT = 0x3B,
	MX25_ERASE_SECTOR = 0x20,
	MX25_ERASE_BLOCK = 0x52,
	MX25_ERASE_BLOCK1 = 0xD8,
	MX25_ERASE_CHIP = 0x60,
	MX25_ERASE_CHIP1 = 0xC7,
	MX25_PAGE_PROGRAM = 0x02,
	MX25_READ_SECURITY = 0x2B,
	MX25_WRITE_SECURITY = 0x2F,
	MX25_ENTER_S_OTP = 0xB1,
	MX25_EXIT_S_OTP = 0xC1,
	MX25_ENTER_SLEEP = 0xB9,
	MX25_WAKE_UP = 0xAB,
	MX25_COMMAND_END
}Mx25CommandType;

typedef struct
{
  uint8_t  taskID;
	uint8_t  cmd;
  uint32_t Txp;
	uint32_t Txnum;
	uint32_t Rxnum;
	uint16_t TxTimeout;
	uint16_t RxTimeout;
        uint16_t flash_Fsm;
        uint16_t delay_time;	
}_flash_task;

//uint16_t token_sector;
/*function*/
uint8_t put_cmdhead(uint8_t cmd,uint32_t addr);
void Deact_flash(void);
void Active_flash(void);
void NOTDMAreadflash(uint32_t addr,uint16_t rd_len);
void enable_WR_flash(void);
void disable_WR_flash(void);
uint8_t read_status_reg(void);
void wait_WEL_Reset(void);
void PageProgram(uint32_t addr,uint8_t* pdata);
void earse_flash(uint16_t sector,uint16_t num);
uint16_t search_current_sector(void);
void set_rj45_config(uint8_t *config);

void NOTDMA_PAGEWR(void);
void flash_self_check(void);

#endif

