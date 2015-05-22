#include "data_typedef.h"
#include "platform.h"
#include "net.h"
int32 GAgent_select(int32 nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds,int32 sec,int32 usec )
{
    struct timeval_t t;

    t.tv_sec = sec;// 秒
    t.tv_usec = usec;// 微秒
    return select( nfds,readfds,writefds,exceptfds,&t );   
}
int32 GAgent_CreateUDPBroadCastServer( uint16 udpbroadcast_port /*,  struct sockaddr_t *sockaddr*/ )
{
    int32 udpbufsize=2;
    int32 serversocketid=0;
    struct sockaddr_t addr;// ??UDP??
   
    serversocketid = socket(AF_INET, SOCK_DGRM, IPPROTO_UDP);
    if(serversocketid < 0)
    {
        //GAgent_Printf(GAGENT_ERROR, "UDP BC socket create error,errno:%d", errno);
        serversocketid = -1;
        return (-1);
    }

    addr.s_type = AF_INET;
    addr.s_port= udpbroadcast_port;
    addr.s_ip = htonl(INADDR_BROADCAST);
    if(setsockopt(serversocketid, SOL_SOCKET, SO_BROADCAST, &udpbufsize,4) != 0)
    {
        //GAgent_Printf(GAGENT_ERROR,"UDP BC Server setsockopt error,errno:%d", errno);
        //return RET_FAILED;
    }
    if(bind(serversocketid, &addr, sizeof(addr)) != 0)
    {
        //GAgent_Printf(GAGENT_ERROR, "UDP BC Server socket bind error,errno:%d", errno);
        close(serversocketid);
        serversocketid = -1;
        return (-1);
    }
    //GAgent_Printf(GAGENT_INFO,"UDP BC Server socketid:%d on port:%d", serversocketid, udpbroadcast_port );
    return serversocketid;
}
int32 socketSendBC( int32 socketid,int32 port,uint8 *pData,int32 len)
{ 
    struct sockaddr_t addr;
    addr.s_type = AF_INET;
    addr.s_port= port;
    addr.s_ip = htonl(INADDR_BROADCAST);
    return  sendto( socketid, pData, len,0,&addr, sizeof(struct sockaddr_t));
}
void DeviceDisCover( int32 socketid , int32 port )
{
    int8 i=0;
    
    uint8 buf[8]= {0};
    *(uint32 *)buf = htonl(0x00000003);
    buf[4] = 0x03;
    buf[5] = 0x00;
    *(uint16 *)(buf+6) = htons( 0x0003 );
    socketSendBC( socketid, port,buf,8 );
    dumplog( buf,8 );
}