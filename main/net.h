#ifndef _NET_H_
#define _NET_H_
int32 GAgent_select(int32 nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds,int32 sec,int32 usec );
int32 GAgent_CreateUDPBroadCastServer( uint16 udpbroadcast_port /*, struct sockaddr_t *sockaddr*/ );
void DeviceDisCover( int32 socketid , int32 port );
#endif