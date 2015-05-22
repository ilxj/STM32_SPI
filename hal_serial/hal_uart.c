#include "hal_uart.h"

/****************************************************************
Function        :   open_serial
Description     :   open serial 
comport         :   serial comport number
bandrate        :   serial bandrate 
nBits           :   serial data Bit
return          :   >0 the serial fd 
                    other fail.
Add by Alex.lin     --2015-03-31
****************************************************************/
int serial_open(char *comport, int bandrate,int nBits,char nEvent,int nStop )
{
    uart_set_str uartpara;
    int uart_fd;
    
    uartpara.BaudRate = bandrate;
    uartpara.number = nBits;
    uartpara.parity = nEvent;
    uartpara.StopBits = nStop;
    uartpara.FlowControl = 0;
    
    SetUartPara (&uartpara);
    uart_fd = OpenUART( comport );
    
    return uart_fd;
}
  
/****************************************************************
Function        :   serial_read
Description     :   read data form serial fd 
buf             :   data form serial pointer.
buflen          :   data want to read.
return          :   >0 the realy data length form serial 
Add by Alex.lin     --2015-03-31
****************************************************************/
int serial_read( int serial_fd, unsigned char *buf,int buflen )
{
  int datalen=0;

    if( serial_fd<0 )
      return 0;
    datalen = read( serial_fd,buf,buflen );
  return datalen;   
}

/****************************************************************
Function        :   serial_write
Description     :   write data to serial fd 
buf             :   data need to serial pointer.
buflen          :   data want to write.
return          :   >0 the number of bytes written is returned
                    other error.
Add by Alex.lin     --2015-03-31
****************************************************************/
int serial_write( int serial_fd,unsigned char *buf,int buflen )
{
  int datalen=0;

    if( serial_fd<0 )
      return 0;
    datalen = write( serial_fd,buf,buflen);
  return datalen;
}
