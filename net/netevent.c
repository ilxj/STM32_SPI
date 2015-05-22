#include "netevent.h"
//#include "gagent.h"
/****************************************************************
Function    :   GAgent_DevCheckWifiStatus
Description :   check the wifi status and will set the wifi status
                and return it.
bit0        :   in soft ap mode     1 is on ,0 is off.
bit1        :   in station mode     1 is on ,0 is off.
bit2        :   in onboarding mode  1 is on ,0 is off.
...
bit4        :   if is connected to WiFi router,1 is on,0 is off.
bit6-bit8   :   only is meaningful if bit=1,this 3-bits integer 
                indicates the WiFi signal strength of the connected
                WiFi router. Range is from 0 to 7 ,0 is low and 
                7 is high.
wifistatus  :   wifistatus=0xFFFF: to get wifistatus from dev.
                user should use this value in GAgent_WiFiEventTick()
                function.
return      :   the new wifi status.
Add by Alex.lin     --2015-04-17.
****************************************************************/

uint16 GAgent_DevCheckWifiStatus( uint16 wifistatus )
{
    int8 flag=0;
    static uint16 halWiFiStatus=0;
    
    if( 0xFFFF!=wifistatus )
    {
        halWiFiStatus = wifistatus;
    }
    return halWiFiStatus;
   
   // return wifistatus;
}
/****************************************************************
Function    :   GAgent_CreateTcpServer
Description :   creat TCP server.
tcp_port    :   server port.
return      :   0> the socket id .
                other error.
Add by Alex.lin     --2015-04-24.
****************************************************************/
int32 GAgent_CreateTcpServer( uint16 tcp_port )
{
    int32 bufferSize;
    struct sockaddr_t addr;
    int32 serversocketid=0;
    
    serversocketid = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(serversocketid < 0)
    {
        ERRORCODE
        GAgent_Printf(GAGENT_ERROR, "Create TCPServer failed.errno:%d", errno);
        return RET_FAILED;
    }
    bufferSize = SOCKET_TCPSOCKET_BUFFERSIZE;

    if(setsockopt(serversocketid, SOL_SOCKET, SO_RDBUFLEN, &bufferSize, 4)<0)
    {
        ERRORCODE
        GAgent_Printf(GAGENT_ERROR,"TCP Server setsockopt fail.errno:%d", errno);
        return RET_FAILED;
    }
    else
    {
    }

    bufferSize = SOCKET_TCPSOCKET_BUFFERSIZE;

    if(setsockopt(serversocketid,SOL_SOCKET,SO_WRBUFLEN,&bufferSize,4) < 0)
    {
        ERRORCODE
        GAgent_Printf(GAGENT_ERROR,"TCP Server setsockopt fail.errno:%d", errno);
        return RET_FAILED;
    }
    memset(&addr, 0x0, sizeof(addr));
    addr.s_port = tcp_port;
    if(bind( serversocketid, (struct sockaddr_t *)&addr, sizeof(addr)) != 0 )
    {
        ERRORCODE
        GAgent_Printf(GAGENT_ERROR, "TCPSrever socket bind error.errno:%d", errno);
        close(serversocketid);
        return RET_FAILED;

    }
    if(listen(serversocketid, 0) != 0)
    {
        GAgent_Printf(GAGENT_ERROR, "TCPServer socket listen error.errno:%d", errno);
        close(serversocketid);
        return RET_FAILED;
    }

    GAgent_Printf(GAGENT_INFO,"TCP Server socketid:%d on port:%d", serversocketid, tcp_port);
    return serversocketid; 
}

int32 GAgent_CreateUDPServer( uint16 udp_port )
{
    struct sockaddr_t addr;
    int32 serversocketid=0;
    
    serversocketid = socket(AF_INET, SOCK_DGRM, IPPROTO_UDP);
    GAgent_Printf(GAGENT_INFO,"GOTO %s:%d", __FUNCTION__, __LINE__);
    if(serversocketid < 1)
    {
        ERRORCODE
        GAgent_Printf(GAGENT_ERROR, "UDPServer socket create error,errno:%d", errno);
        return RET_FAILED;
    }
    memset(&addr, 0x0, sizeof(addr));
    addr.s_port =(udp_port);

    if(bind(serversocketid, &addr, sizeof(addr)) != 0)
    {
        ERRORCODE
        GAgent_Printf(GAGENT_ERROR, "UDPServer socket bind error,errno:%d", errno);
        close(serversocketid);
        return RET_FAILED;
    }

    GAgent_Printf(GAGENT_INFO,"UDP Server socketid:%d on port:%d", serversocketid, udp_port);
    return serversocketid;
}
int32 GAgent_CreateUDPBroadCastServer( uint16 udpbroadcast_port, struct sockaddr_t *sockaddr)
{
    int32 udpbufsize=2;
    int32 serversocketid=0;
    struct sockaddr_t addr;// 用于UDP广播
   
    serversocketid = socket(AF_INET, SOCK_DGRM, IPPROTO_UDP);
    if(serversocketid < 0)
    {
        GAgent_Printf(GAGENT_ERROR, "UDP BC socket create error,errno:%d", errno);
        serversocketid = -1;
        return RET_FAILED;
    }

    addr.s_type = AF_INET;
    addr.s_port= udpbroadcast_port;
    addr.s_ip = htonl(INADDR_BROADCAST);
    if(setsockopt(serversocketid, SOL_SOCKET, SO_BROADCAST, &udpbufsize,4) != 0)
    {
        GAgent_Printf(GAGENT_ERROR,"UDP BC Server setsockopt error,errno:%d", errno);
        //return RET_FAILED;
    }
    if(bind(serversocketid, &addr, sizeof(addr)) != 0)
    {
        GAgent_Printf(GAGENT_ERROR, "UDP BC Server socket bind error,errno:%d", errno);
        close(serversocketid);
        serversocketid = -1;
        return RET_FAILED;
    }
    *sockaddr = addr;
    GAgent_Printf(GAGENT_INFO,"UDP BC Server socketid:%d on port:%d", serversocketid, udpbroadcast_port );
    return serversocketid;
}

