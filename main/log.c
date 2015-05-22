#include "platform.h"
#include "data_typedef.h"

void log( int8 level,uint8 *pData )
{
    switch( level )
    {
        case 1:
            break;
        case 2:
            break;
        default:break;
    }
    //printf( )
}
void dumplog( uint8 *pData,int32 len )
{
    int32 i=0;
    printf( "\n dump-------------------------\n");
    for( i=0;i<8;i++ )
    {
        printf( " %02x",pData[i]);
    }
    printf( "\n-------------------------\n");
}