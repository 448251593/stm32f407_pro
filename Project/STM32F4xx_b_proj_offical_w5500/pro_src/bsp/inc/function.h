#ifndef _USER_FUNCTION_H_
#define _USER_FUNCTION_H_

typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned       __int64 uint64_t;


void gpio_init(void);
void free_spi_cs(uint32_t Pin_cs);
void selected_spi_cs(uint32_t Pin_cs);
void led_on(uint16_t led_num);
void led_off(uint16_t led_num);
void led_toggle(uint16_t led_num);

void SpiAdcDma_init(void);

uint16_t swapword(uint16_t x);

void uart3_init(void);
void debug0( const char *string,uint16_t data);
void UART_Send(uint8_t* pdata, uint32_t size);
void stop_uart3_rx(void);
void en_uart3_rx(void);

void user_systick_task(void);
void process_recv(uint16_t len);

void SpiFlashDma_init(void);
void flash_init(void);
void NOTDMAreadflash(uint32_t addr,uint16_t len);
void earse_flash(uint16_t sector,uint16_t num);
#endif
