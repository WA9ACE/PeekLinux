/**
 * @file	usb_config.h
 *
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


#include "usb/usb_config.h"

/* the testdefine is set in usb_config.h! */
#ifndef USB_TEST_MODE
  #include "usb/usb_interface_cfg.h"
#else
  #include "usb/usb_test_interface_cfg.h"
#endif
