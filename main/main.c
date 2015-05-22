#include "stdio.h"
#include "platform.h"
#include "data_typedef.h"
#define UART_NAME ""


int fputc(int ch, FILE *f)
{
    UartDataRT(1, (char*)&ch, 1);
    return ch;
}
int32 GAgent_OpenUart( int32 BaudRate,int8 number,int8 parity,int8 stopBits,int8 flowControl )
{
    int32 uart_fd=0;
    uart_fd = serial_open( UART_NAME,BaudRate,number,'N',stopBits );
    if( uart_fd<=0 )
    while( uart_fd <=0 )
    {
        uart_fd = serial_open( UART_NAME,BaudRate,number,'N',stopBits );
        sleep(1);
    }

    return uart_fd;
}
void CoreInit(void)
{
    lib_config_t libConfig;
    int32 uart_fd=0;
    mxchipWNet_HA_init();
    libConfig.tcp_buf_dynamic = mxEnable;
    libConfig.tcp_max_connection_num = 12;
    libConfig.tcp_rx_size = 2048;
    libConfig.tcp_tx_size = 2048;
    libConfig.hw_watchdog = 0;
    libConfig.wifi_channel = WIFI_CHANNEL_1_13;
    lib_config(&libConfig);  

    set_tcp_keepalive(3,10);
    uart_fd = GAgent_OpenUart( 9600,8,0,0,0 );
    return;
}

fd_set readfd;
void main( void )
{
    uint8 ledflag=0;
    int32 udp_fd=0;
    int recLen=0;
    int8 rebuf[200]={0};
    CoreInit();
    udp_fd = GAgent_CreateUDPBroadCastServer( 12414 /*, struct sockaddr_t *sockaddr*/ );
    printf( "udp_fd =%d\n",udp_fd);
    if( udp_fd>0 )
    {
        FD_ZERO( &readfd );
        FD_SET( udp_fd,&readfd );
    }
    DeviceDisCover( udp_fd , 12414 );
    while(1)
    {
        mxchipTick();
        DeviceDisCover( udp_fd , 12414 );
        GAgent_select( udp_fd+1, &readfd, NULL,NULL,1,0 );
        if(FD_ISSET( udp_fd, &readfd ) )
        {
            struct sockaddr_t addr;
            int addrLen = sizeof(struct sockaddr_t);
            recLen = Socket_recvfrom( udp_fd, rebuf, 200,
                &addr, &addrLen);
            printf(" onDiscover!!!");
            dumplog( rebuf,recLen );
        }
        led( ledflag );  /*green led*/   
        ledflag = 1-ledflag;
        sleep(1);
        
    }
}