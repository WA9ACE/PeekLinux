/**
 * @file    rnet_rt_ngos.c
 *
 * Riviera RNET - NexGenOS porting functions
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * $Id: rnet_rt_ngos.c,v 1.2 2002/10/30 15:30:32 rf Exp $
 * $Name: ti_20021030 $
 *
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  3/22/2002   Regis Feneon  Create
 *  28/10/2002   Regis Feneon  removed ngOSClockGetTime(),
 *                             now in rnet_rt_ng_timer.c
 *
 */

#include <ngos.h>

int ngOSIntrCtl( int ctl)
{
  ((void) ctl);
  return( 0);
}

void ngYield( void)
{
  return;
}

