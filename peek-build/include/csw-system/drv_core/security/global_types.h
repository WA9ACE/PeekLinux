/******************************************************************************
 *                       WIRELESS TERMINAL BUSINESS UNIT
 *
 *            (C) 2001 TEXAS INSTRUMENTS FRANCE. All rights reserved
 ******************************************************************************
 *  Important Note
 *  --------------
 *
 *  This S/W is a preliminary version. It contains information on a product 
 *  under development and is issued for evaluation purposes only. Features 
 *  characteristics, data and other information are subject to change.
 *
 *  The S/W is furnished under Non Disclosure Agreement and may be used or
 *  copied only in accordance with the terms of the agreement. It is an offence
 *  to copy the software in any way except as specifically set out in the 
 *  agreement. No part of this document may be reproduced or transmitted in any
 *  form or by any means, electronic or mechanical, including photocopying and
 *  recording, for any purpose without the express written permission of Texas
 *  Instruments Inc.
 *
 *  FILE NAME: global_types.h
 *
 *
 *  PURPOSE:
 *
 *
 *
 *  FILE REFERENCES:
 *
 *  Name                  IO      Description
 *  -------------         --      ---------------------------------------------
 *
 *
 *
 *  EXTERNAL VARIABLES:
 *
 *  Source:
 *
 *  Name                  Type      IO   Description
 *  -------------         -------   --   --------------------------------------
 *
 *
 *
 *  EXTERNAL REFERENCES:
 *
 *  Name                Description
 *  ------------------  -------------------------------------------------------
 *
 *
 *
 *  ABNORMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES:
 *
 *
 *
 *  ASSUMPTION, CONSTRAINTS, RESTRICTIONS:
 *
 *
 *
 *  NOTES:
 *
 *
 *
 *  REQUIREMENTS/FUNCTIONAL SPECIFICATION REFERENCES:
 *
 *
 *
 *
 *  DEVELOPMENT HISTORY:
 *
 *  Date       Author(s)       Version  Description
 *  ---------  --------------  -------  ---------------------------------------
 *  01-Dec-01  F. AMAND        0.1      First implementation.
 *
 *
 *  ALGORITHM:
 *
 *
 *****************************************************************************/
 
#ifndef _USER_TYPES_
  #define _USER_TYPES_

  typedef unsigned char       UWORD8;
  typedef unsigned short int  UWORD16;
  typedef unsigned long int   UWORD32;

  typedef signed char         WORD8;
  typedef signed short int    WORD16;
  typedef signed long int     WORD32;
  #if (!defined(NUCLEUS) )
  typedef char                CHAR;
  #endif
  typedef double              DOUBLE;
  typedef unsigned char       C_BOOLEAN;
  #define C_FALSE 0
  #define C_TRUE 1

  #define C_CHIPSET_CALYPSOPLUS   11
  #define C_CHIPSET_LOCOSTO       15
  #define C_CHIPSET_FLAG          CHIPSET
#endif /* _USER_TYPES_ */
