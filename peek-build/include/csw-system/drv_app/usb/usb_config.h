/**
 * @file	usb_config.h
 *
 * 
 * @author	Anton van Breemen
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	26/09/2003	Anton van Breemen (ICT)		
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */



#define USB_TEST_MODE

#if (USB_FF_DYNAMIC == 1)
#define USB_DYNAMIC_CONFIG_SUPPORT
#endif

#ifndef USB_TEST_MODE
  #include "usb/usb_cfg.h"
#else
  #include "usb/usb_test_cfg.h"
#endif
