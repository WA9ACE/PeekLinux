/*
+--------------------------------------------------------------------+
| PROJECT: MMI-Framework (8417)         $Workfile:: mfw_ext.c       $|
| $Author:: Es  $ CONDAT GmbH           $Revision:: 2               $|
| CREATED: 29.01.99                     $Modtime:: 4.02.99 19:39    $|
| STATE  : code                                                      |
+--------------------------------------------------------------------+

   MODULE  : MFW_EXT

   PURPOSE : MFW external communication

   EXPORT  :

   TO DO   :

   $History:: mfw_ext.c                                             $
 * 
 * *****************  Version 2  *****************
 * User: Es           Date: 17.02.99   Time: 20:00
 * Updated in $/GSM/DEV/MS/SRC/MFW
 * 
 * *****************  Version 1  *****************
 * User: Es           Date: 9.02.99    Time: 14:53
 * Created in $/GSM/DEV/MS/SRC/MFW
 * TI display & keyboard interface for MFW
*/

#include <stdio.h>
#include <string.h>

#include "mfw_mmi.h"
#include "drv_key.h"


//extern void ext_syscmd (char *cmd, char *from, char *to);
extern void scrMobCommand (char *cmd);     /* ext_syscmd to driver     */


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_EXT            |
| STATE   : code                        ROUTINE : mfwExtIn           |
+--------------------------------------------------------------------+

  PURPOSE : execute incoming command

*/

int mfwExtIn (char *cmd)
{
    if (!strncmp("KBD ",cmd,4))
    {
        unsigned int upDown, key;
        sscanf(cmd+4,"%d,%d",&upDown,&key);
        drvKeyUpDown((char) upDown,(char) key);
    }
    else if (!strncmp("SCR ",cmd,4))
    {
        scrMobCommand(cmd+4);
    }
    else if (!strncmp("MFW init",cmd,8))
    {
        mmiInit();
    }
    else if (!strncmp("MFW exit",cmd,8))
    {
        mmiExit();
    }
    else
        return 0;

    return 1;
}


/*
+--------------------------------------------------------------------+
| PROJECT : MMI-Framework (8417)        MODULE  : MFW_EXT            |
| STATE   : code                        ROUTINE : mfwExtOut          |
+--------------------------------------------------------------------+

  PURPOSE : forward outgoing command

*/

int mfwExtOut (char *cmd)
{
//    ext_syscmd(cmd,"MMI","PCO");

    return 1;
}

