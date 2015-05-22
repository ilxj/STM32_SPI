#ifndef __PLATFORM_H
#define __PLATFORM_H

#include "stm32f2xx.h"
#include "mxchipWNET.h"

#ifdef EMW3162
#include "HAL_EMW3162.h"
#endif

#define APP_INFO     "GIZWITS_MXCHIP"

#define GAGENT_WITH_MXCHIP  1
#define GAGENT_CONNECT_WITH_ETHERNET    0
#define GAGENT_FEATURE_WXCHIP_EASYLINK  1
#define GAGENT_FEATURE_WXCHIP_AIRKISS   0
#define GAGENT_FEATURE_JINGDONGDISCOVER 0
#define GAGENT_FEATURE_SENDAPSTATUS2MCU 1
#define GAGENT_FEATURE_HTTP_OTA         1

//MXCHIP V3 һӉԃHTTP OTA ׽ʽ
#ifdef GAGENT_V3PROTOCOL
#define GAGENT_FEATURE_HTTP_OTA         0
#endif


#ifdef EABox
#include "HAL_EABox.h"
#endif

#ifdef EMW3161
#include "HAL_EMW3161.h"
#endif

#define PRODUCT_VERSION "XXXXXXX_XXXXXX"
#define GAGENT_FEATURE_OTA                  1

#ifdef EMW3161
#define WIFI_HARDVER    "00MX3161"
#endif

#ifdef EMW3162
#define WIFI_HARDVER    "00MX3162"
#endif

#ifdef GAGENT_V4PROTOCOL
#define WIFI_SOFTVAR    "04000005"  // Update from 04000004 at 02-09
#endif

#ifdef GAGENT_V3PROTOCOL
#define WIFI_SOFTVAR    "03000005"  // Update from 03000004 at 02-09
#endif
#define UART_NAME ""
#include <string.h>
#include <stdlib.h>
#include <string.h>
//#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#ifdef mxchipWNet_HA

//void hal_uart_init(void);
//void hal_uart_tx_tick(void);
// void reset_uart_init(void);
// void reset_uart_deinit(void);
// void uart_recv(void);
// int uart_rx_data_length(void);
// int uart_get_rx_buffer(u8* buf, u32 len);
// void uart_flush_rx_buffer(void);
// void uart_send_is_done(void);
//int hal_uart_send_data(u8 *buf, u32 len);
//int hal_uart_get_one_packet(u8* buf, int bufferMaxLen);

//void hal_dma_rx_half_handler(void);
//void hal_dma_rx_complete_handler(void);

int OpenAirkiss(int timeout);
int CloseAirkiss(void);
void easylink_user_data_result(int datalen, char*data);
void report_airkiss_value(void);

void restoreConfiguration(void);
#endif
//void UART_Init(void);
void Button_Init(void);
__weak void Button_irq_handler(void *arg);
int serial_write( int serial_fd,unsigned char *buf,int buflen );
#endif /* __PLATFORM_H */
