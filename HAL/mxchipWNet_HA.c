#include "stdio.h"
#include "ctype.h"
//#include "gagent.h"

#include "platform.h"
#include "mxchipWNet.h"
#include "mxchipWNet_HA.h"

#include "flash_if.h"

#define MAX_CLIENT 8
#define MAX_SITES 32

#define MAX_UPGRADE_TIME 5*ONE_MINUTE// upgrade time should less than 5 minutes.

#define UART_CMD_MAX_LEN 1400
#define UART_CMD_LEN 12
#define IP_CMD_MAX_LEN 1460

#define UART_FRAM_START 0xAA
#define UART_FRAM_END 0x55
#define CONTROL_FLAG 0xBB
#define FRAM_FLAG 0x00BB

#define UAP_START_TIME_SHORT 5*ONE_SECOND
#define STATION_RETRY_SHORT 20*ONE_SECOND
#define SEND_TIMEOUT 200



enum {
  CMD_OK = 0,
  CMD_FAIL,
};

enum {
  STA_CONNECT = 1<<0,
  //UAP_SERVER_START = 1<<1,
  UAP_START = 1<<2,
  REMOTE_CONNECT = 1<<3,
};

enum {
  INIT_STATE = 0, // first 20 seconds until wifi & tcp state changed
  DUAL_MODE_STATE, // After 20 seconds, wifi didn't connected
  NORMAL_WORK_STATE, // tcp connected.
  NORMAL_WORK_RETRY_STATE // TCP/STA disconnected, retry.
};

typedef  struct  _scan_tbl  
{  
    char ssid[32];  
    int rssi;  // sorted by rssi
}scan_tbl_t; 

#pragma pack(1)
typedef struct _upgrade_t {
  u8 md5[16];
  u32 len;
  u8 data[1];
}ota_upgrate_t;

#pragma pack(1)
typedef struct _udp_search {
  u8 ip[16];
  u8 mac[18];
  u8 ver[8]; 
}udp_search_st;


extern vu32 MS_TIMER;
extern vu32 SEC_TIMER;
extern vu32 TICK_TIMER;
/*******************************by alex********************/

extern int g_MqttCloudSocketID ;


/*******************************by alex********************/

static int wifi_disalbed = 0;
static int need_reload = 0;
static u32 network_state = 0;
static int cloud_ip_addr = 0;
static u32 dns_pending = 0;
static u32 cloud_enable = CLOUD_ENABLE;
static u32 uap_start_time = UAP_START_TIME;
//static u32 sta_retry_interval = STA_RETRY_SHORT;


static u8 hugebuf[1600]; // cmd, fwd data are saved in this buffer
mxchipWNet_HA_st  *device_info;


//static void tcp_client_tick(void);
void delay_reload(void);
static void report_state(void);
static void tcp_recv_tick(void);
static void connect_to_cloud(void);
static void close_cloud(int retry_interval);
static int ip_cmd_process(unsigned char *buf, int cmd_len, int socket_fd);
static int search_cmd_process(unsigned char *buf, int cmd_len, const struct sockaddr_t *dest_addr, socklen_t addrlen);
void fast_send(int sockfd, const void *buf, size_t len);

/*******add by Alex*********/




/***************************/

int is_wifi_disalbed(void)
{
  return wifi_disalbed;
}

int is_network_state(int state)
{
  if ((network_state & state) == 0)
    return 0;
  else
    return 1;
}

static void report_state()
{
//  mxchip_state_t *cmd = (mxchip_state_t*)hugebuf;
  //status_changed_cb((mxchip_state_t*)hugebuf);
}

void delay_reload()
{
  need_reload = 1;
}

void userWatchDog(void)
{
}

void NetCallback(net_para_st *pnet)
{
#if 0
    strcpy((char *)device_info->status.ip, pnet->ip);
    strcpy((char *)device_info->status.mask, pnet->mask);
    strcpy((char *)device_info->status.gw, pnet->gate);
    strcpy((char *)device_info->status.dns, pnet->dns);	
    strcpy((char *)device_info->status.mac, pnet->mac);	
#endif
    //GAgent_Printf(GAGENT_INFO,"MAC:%s",pnet->mac);
    //GAgent_Printf(GAGENT_INFO,"IP:%s",pnet->ip);
    //g_globalvar.status_ip_flag=1;
   //pgContextData->rtinfo.status_ip_flag=1;
}

network_InitTypeDef_st wNetConfig;


/* Sysytem initilize */
void mxchipWNet_HA_init(void) 
{
    net_para_st para;
    device_info = (mxchipWNet_HA_st *)malloc(sizeof(mxchipWNet_HA_st));
    memset(device_info, 0, sizeof(mxchipWNet_HA_st)); 
    SystemCoreClockUpdate();
    mxchipInit();
}

static void uap_start(void)
{
}

/* Back to default configuration and reload */
void system_default(void)
{
}
