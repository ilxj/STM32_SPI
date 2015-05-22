#include "gagent.h"
/****************************************************************
FunctionName  :     GAgent_LocalGetInfo
Description   :     get localinfo like pk.
return        :     return 
Add by Alex.lin         --2015-04-18
****************************************************************/
void GAgent_SOC_GetInfo( pgcontext pgc )
{
    int8 *mcu_protocol_ver="00000001";
    int8 *mcu_p0_ver="00000002";
    int8 *mcu_hard_ver="00000003";
    int8 *mcu_soft_ver="00000004";
    int8 *mcu_product_key="6f3074fe43894547a4f1314bd7e3ae0b";
    uint16 mcu_passcodeEnableTime=0;
    
    
    strcpy( pgc->mcu.hard_ver,mcu_hard_ver );
    strcpy( pgc->mcu.soft_ver,mcu_soft_ver );
    strcpy( pgc->mcu.p0_ver,mcu_p0_ver );
    strcpy( pgc->mcu.protocol_ver,mcu_p0_ver );
    strcpy( pgc->mcu.product_key,mcu_product_key );
    pgc->mcu.passcodeEnableTime = mcu_passcodeEnableTime;

    GAgent_Printf( GAGENT_INFO,"GAgent get local info ok.");
    GAgent_Printf( GAGENT_INFO,"MCU Protocol Vertion:%s.",pgc->mcu.protocol_ver);
    GAgent_Printf( GAGENT_INFO,"MCU P0 Vertion:%s.",pgc->mcu.p0_ver);
    GAgent_Printf( GAGENT_INFO,"MCU Hard Vertion:%s.",pgc->mcu.hard_ver);
    GAgent_Printf( GAGENT_INFO,"MCU Soft Vertion:%s.",pgc->mcu.soft_ver);
    GAgent_Printf( GAGENT_INFO,"MCU old product_key:%s.",pgc->gc.old_productkey);
    GAgent_Printf( GAGENT_INFO,"MCU product_key:%s.",pgc->mcu.product_key);
    GAgent_Printf( GAGENT_INFO,"MCU passcodeEnableTime:%d s.\r\n",pgc->mcu.passcodeEnableTime);
}