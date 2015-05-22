#include "gagent.h"
#include "mxchipWNET.h"
#include "iof_arch.h"
#include "flash_if.h"

extern vu32 MS_TIMER;
static int32 socketid_callback=-1;
int8 connectflag=0;
static uint32 gCloudIp=0xFFFFFFFF;

/******mxchip callback function *****/
void socket_connected( int32 fd )
{
    socketid_callback = fd;
    GAgent_Printf(GAGENT_INFO,"socketid call back = %d",socketid_callback );
}
void socket_connectedflag( int8 flag )
{
    //static int8 connectflag=0;
    
}

void dns_ip_set(u8 *hostname, u32 ip)
{
        GAgent_Printf(GAGENT_INFO,"dns_ip_set %s:%04x\r\n",hostname, ip);
        gCloudIp=ip;
    return;
}
void RptConfigmodeRslt(network_InitTypeDef_st *nwkpara)
{
    CloseEasylink2();
    GAgent_Printf( GAGENT_INFO,"AirLink Callback...");

    memset( pgContextData->gc.wifi_ssid,0,SSID_LEN_MAX+1 );
    memset( pgContextData->gc.wifi_key,0,WIFIKEY_LEN_MAX+1 );
    strcpy( pgContextData->gc.wifi_ssid,nwkpara->wifi_ssid );
    strcpy( pgContextData->gc.wifi_key,nwkpara->wifi_key );
    GAgent_AirlinkResult( pgContextData );
}



void GAgent_DevLED_Red( uint8 onoff )
{
#ifdef EMW3162
    set_conncetion_status( onoff ); /*red led*/
#else
    led( onoff );  /*green led*/    
#endif
}
void GAgent_DevLED_Green( uint8 onoff )
{    
#ifdef EMW3162
    led( onoff );  /*green led*/  
#else
    set_conncetion_status( onoff ); /*green led*/
#endif
}

uint32 GAgent_GetDevTime_MS()
{
    return MS_TIMER;
} 
uint32 GAgent_GetDevTime_S()
{
    return (MS_TIMER/1000);
}
void GAgent_DevReset()
{
    GAgent_Printf(GAGENT_ERROR, "Call DRV_GAgent_Reset Now...");
    msleep(1000);
    NVIC_SystemReset();
}
void GAgent_DevInit( pgcontext pgc )
{
    CoreInit();
}
int8 GAgent_DevGetMacAddress( uint8* szmac )
{
  net_para_st stNetpara;
  getNetPara(&stNetpara, Soft_AP);
  strcpy( szmac,stNetpara.mac );
  return 0;
}

uint32 GAgent_DevGetConfigData( gconfig *pConfig )
{
    uint32 configInFlash = PARA_START_ADDRESS;
    memcpy(pConfig, (void *)configInFlash, sizeof(gconfig));
    return 0;
}
uint32 GAgent_DevSaveConfigData( gconfig *pConfig )
{
    uint32 paraStartAddress, paraEndAddress;
    
    paraStartAddress = PARA_START_ADDRESS;
    paraEndAddress = PARA_END_ADDRESS;
    
    FLASH_If_Init();
    FLASH_If_Erase(paraStartAddress, paraEndAddress);
    if(FLASH_If_Write(&paraStartAddress, (u32 *)pConfig, sizeof(gconfig))==0)
    {
        GAgent_Printf( GAGENT_INFO,"Flash write ok!");
    }
    FLASH_Lock();
    msleep(10);
    return 0; 
}

void WifiStatusHandler(int event)
{
    int16 wifiStatus=0;
    wifiStatus = pgContextData->rtinfo.GAgentStatus;
    GAgent_Printf(GAGENT_INFO, "WifiStatusHandler event:%d", event);
    switch (event) 
    {
        case MXCHIP_WIFI_UP:
        {
            GAgent_Printf(GAGENT_INFO,"WIFI UP! ");
            wifiStatus = wifiStatus|WIFI_STATION_CONNECTED;
            wifiStatus = wifiStatus|WIFI_MODE_STATION;
            //report_airkiss_value();
            
            uap_stop();
            break;
        }    
        case MXCHIP_WIFI_DOWN:
        {
            wifiStatus &=~ WIFI_STATION_CONNECTED;
            wifiStatus = wifiStatus|WIFI_MODE_STATION;
            break;
        }
        case MXCHIP_UAP_UP:
        {
            wifiStatus = wifiStatus|WIFI_MODE_AP;
            break;
        }
        case MXCHIP_UAP_DOWN:
        {
            wifiStatus &=~ WIFI_MODE_AP;
           
            break;
        }
        default:
          break;
    }
    wifiStatus = GAgent_DevCheckWifiStatus( wifiStatus );
    GAgent_Printf(GAGENT_INFO,"WIFI out--wifi %04X",wifiStatus );
    return;
}

/****************************************************************
*   function    :   socket connect 
*   flag        :   1 block 
                    0 no block 
    return      :   0> connect ok socketid 
                :   other fail.
    add by Alex.lin  --2015-02-02
****************************************************************/
int32 GAgent_connect( int32 iSocketId, uint16 port,
                        int8 *ServerIpAddr,int8 flag)
{
    int ret=0;
    int MTflag=0;
    int32 socket=-1;
    struct sockaddr_t Msocket_address;
    //MTflag = flag;
    MTflag = 1;
    if(setsockopt(iSocketId, 0, SO_BLOCKMODE, &MTflag,4)<0)
    {
        GAgent_Printf(GAGENT_WARNING," MQTT setsockopt fail");
        return -2;
    }

    // Create the stuff we need to connect
    Msocket_address.s_type = AF_INET;
    Msocket_address.s_port = port;
    Msocket_address.s_ip = inet_addr(ServerIpAddr);//IP
    ret = connect(iSocketId, &Msocket_address, sizeof(struct sockaddr_t));

    
    if (MTflag == 0)
    {
        char i=0;
        GAgent_Printf(GAGENT_INFO,"Socket connect in no block mode!");
        while( 1 )
        {
            mxchipTick();
            if( (socketid_callback>0) ||(i>10*5) )
            {
                break;
            }
            i++;
            msleep(100);
        }
        if( socketid_callback>0 )
        {
            socket = socketid_callback;
            socketid_callback=-1;
        }
        else
        {
            socketid_callback=-1;
            socket = -1;
        }
    }
    else
    {
        if(ret==0)
            socket=iSocketId;
        else 
            socket=-1;
        GAgent_Printf(GAGENT_INFO,"socket dev connect ret:[%d]", ret);
    }
    
    return socket;
 
}
/****************************************************************
FunctionName    :   GAgent_DRVGetWiFiMode
Description     :   Get the wifi running mode 
return          : 1-station mode
                  0-ap mode 
Add by Alex.lin     --2015-03-30
****************************************************************/
int8 GAgent_DRVGetWiFiMode( pgcontext pgc )
{
    int8 ret =0;
    int8 ssidlen=0,keylen=0;
    ssidlen = strlen( pgc->gc.wifi_ssid );
    keylen = strlen( pgc->gc.wifi_key );
    GAgent_Printf( GAGENT_INFO,"SSIDLEN=%d, keyLEN=%d ",ssidlen,keylen );
    if( (ssidlen>0 && ssidlen<=SSID_LEN_MAX) && keylen<=WIFIKEY_LEN_MAX )
    {
        pgc->gc.flag |= XPG_CFG_FLAG_CONNECTED;
        ret = 1;
    }
    else
    {
        memset( pgc->gc.wifi_ssid,0,SSID_LEN_MAX+1 );
        memset( pgc->gc.wifi_key,0,WIFIKEY_LEN_MAX+1 );
        pgc->gc.flag &=~ XPG_CFG_FLAG_CONNECTED;
        ret = 0;
    }
    GAgent_DevSaveConfigData( &(pgc->gc) );
    GAgent_DevGetConfigData( &(pgc->gc) );
    return ret;
}
//return the new wifimode 
int8 GAgent_DRVSetWiFiStartMode( pgcontext pgc,uint32 mode )
{
    return ( pgc->gc.flag +=mode );
}
int16 GAgent_DRVWiFi_StationCustomModeStart(int8 *StaSsid,int8 *StaPass,uint16 wifiStatus )
{
    network_InitTypeDef_st NetConfig;
    int ret;
    memset(&NetConfig, 0x0, sizeof(network_InitTypeDef_st));
    
    NetConfig.wifi_mode = Station;//
    strcpy((char*)NetConfig.wifi_ssid, StaSsid);
    strcpy((char*)NetConfig.wifi_key, StaPass);
    NetConfig.wifi_retry_interval = 1000*10;//1000ms = 1s .1*10=10s
    NetConfig.dhcpMode = DHCP_Client;
    ret = StartNetwork(&NetConfig);
    GAgent_Printf(GAGENT_DEBUG, "Start Connect to %s with %s[%d]", StaSsid, StaPass, ret);

    GAgent_DevLED_Red(0);
    GAgent_DevLED_Green(1);
    return 0;
//    GAgent_CreateTimer(GAGENT_TIMER_CLOCK, 1000*30, GAgent_WiFiStatusCheckTimer);
}
int16 GAgent_DRV_WiFi_SoftAPModeStart( const int8* ap_name,const int8 *ap_password,int16 wifiStatus )
{
    network_InitTypeDef_st NetConfig;
    int ret;
    memset(&NetConfig, 0x0, sizeof(network_InitTypeDef_st));

    NetConfig.wifi_mode = Soft_AP;
    strcpy((char*)NetConfig.wifi_ssid,ap_name);
    strcpy((char*)NetConfig.wifi_key, ap_password);
    strcpy((char*)NetConfig.local_ip_addr, AP_LOCAL_IP);
    strcpy((char*)NetConfig.net_mask, AP_NET_MASK);
    strcpy((char*)NetConfig.address_pool_start, ADDRESS_POOL_START);
    strcpy((char*)NetConfig.address_pool_end, ADDRESS_POOL_END);
    NetConfig.dhcpMode = DHCP_Server;
    ret = StartNetwork(&NetConfig);

    GAgent_DevLED_Red(1);
    GAgent_DevLED_Green(0);

    return 0;  
}
int16 GAgent_DRVWiFi_StationDisconnect()
{
    return 0;
}
/****************************************************************
FunctionName    :   GAgent_GetHostByName
Description     :   get host by name
return          :   0-ok
                    other -fail.
Add by Alex.lin     2015-03-30
****************************************************************/
uint32 GAgent_GetHostByName( int8 *domain, int8 *IPAddress)
{
    int ret=0;
    char i=0;
    char Temp_ipaddress[17]={0};
    GAgent_Printf(GAGENT_INFO,"get hostbyname:%s\r\n",domain);
    dns_request( domain );
    /* Will wait for 5s at most */
    while(1)
    {
        mxchipTick();
        if( ( 0xFFFFFFFF != gCloudIp ) || ( i>(5*10) ) )
        {
            break;
        }
        i++;
        msleep(100);
    }
    if( 0xFFFFFFFF != gCloudIp )
    {
        inet_ntoa(IPAddress,gCloudIp);
        gCloudIp=0xFFFFFFFF;
        GAgent_Printf( GAGENT_INFO,"IP:%s",IPAddress );
        return 0;
    }
    return 1;
}
void CoreInit(void)
{
    lib_config_t libConfig;

    mxchipWNet_HA_init();
    libConfig.tcp_buf_dynamic = mxEnable;
    libConfig.tcp_max_connection_num = 12;
    libConfig.tcp_rx_size = 2048;
    libConfig.tcp_tx_size = 2048;
    libConfig.hw_watchdog = 0;
    libConfig.wifi_channel = WIFI_CHANNEL_1_13;
    lib_config(&libConfig);  

    set_tcp_keepalive(3,10);
    
    return;
}
void DRV_ConAuxPrint( char *buffer, int len, int level )
{
//#if GAGENT_SOC==1
    int uart_fd=0;
    uart_fd = GetUartNum( UART_NAME );
    if( uart_fd<=0 )
        return ;

    write( uart_fd,buffer,len );
    return;
//#endif
}
int32 GAgent_OpenUart( int32 BaudRate,int8 number,int8 parity,int8 stopBits,int8 flowControl )
{
    int32 uart_fd=0;
    uart_fd = serial_open( UART_NAME,BaudRate,number,'N',stopBits );
    if( uart_fd<=0 )
        return (-1);
    return uart_fd;
}
void GAgent_LocalDataIOInit( pgcontext pgc )
{
    pgc->rtinfo.local.uart_fd = GAgent_OpenUart( 9600,8,0,0,0 );
    while( pgc->rtinfo.local.uart_fd <=0 )
    {
        pgc->rtinfo.local.uart_fd = GAgent_OpenUart( 9600,8,0,0,0 );
        sleep(1);
    }
    return ;
}

uint32 GAgent_LocalReceivePacketData( int8 *pData )
{
    return 0;
}
void GAgent_DevTick()
{
    mxchipTick();
}

int Socket_sendto(int sockfd, u8 *data, int len, void *addr, int addr_size)
{
    return sendto(sockfd, data, len, 0, (struct sockaddr_t*)addr, addr_size);
}
int Socket_accept(int sockfd, void *addr, int *addr_size)
{
    return accept(sockfd, (struct sockaddr_t*)addr, addr_size);
}
int Socket_recvfrom(int sockfd, u8 *buffer, int len, void *addr, int *addr_size)
{
    return recvfrom(sockfd, buffer, len, 0, (struct sockaddr_t *)addr, addr_size);
}
int connect_mqtt_socket(int iSocketId, struct sockaddr_t *Msocket_address, unsigned short port, char *MqttServerIpAddr)
{
    return 0;

}
int32 GAgent_select(int32 nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds,int32 sec,int32 usec )
{
    struct timeval_t t;

    t.tv_sec = sec;// 秒
    t.tv_usec = usec;// 微秒
    return select( nfds,readfds,writefds,exceptfds,&t );   
}
void GAgent_OpenAirlink( int32 timeout_s )
{
    OpenEasylink2( timeout_s );
    //TODO
    return ;
}
void GAgent_AirlinkResult( pgcontext pgc )
{
    pgc->gc.flag |=XPG_CFG_FLAG_CONFIG;
    return ;
}
void GAgent_DRVWiFiStartScan( )
{

}
void GAgent_DRVWiFiStopScan( )
{

}
NetHostList_str *GAgentDRVWiFiScanResult( NetHostList_str *aplist )
{
    //需要再平台相关的扫描结果调用该函数。
    //把平台相关扫描函数的结果拷贝到NetHostList_str这个结构体上。
    return  aplist;
}