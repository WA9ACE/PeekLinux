/*---------------------------------------------------------------------------
               ----> DO NOT REMOVE THE FOLLOWING NOTICE <----

      Copyright (c) 2003 - 2005 Datalight, Inc.  All Rights Reserved.

  Datalight, Incorporated is a Washington State corporation located at:

        21520 30th Dr SE, Suite 110,      Tel:  425-951-8086
        Bothell, WA  98021                Fax:  425-951-8094
        USA                               Http://www.datalight.com

  This software, including without limitation, all source code and documen-
  tation, is the confidential, trade secret property of Datalight, Inc., and
  is protected under the copyright laws of the United States and other juris-
  dictions.

  ---> Portions of the software are patent pending <---

  In addition to civil penalties for infringement of copyright under applic-
  able U.S. law, 17 U.S.C. 1204 provides criminal penalties for violation of
  (a) the restrictions on circumvention of copyright protection systems found
  in 17 U.S.C. 1201 and (b) the protections for the integrity of copyright
  management information found in 17 U.S.C. 1202.

  U.S. Government Restricted Rights:  Use, duplication, reproduction, or
  transfer of this commercial product and accompanying documentation is
  restricted in accordance with FAR 12.212 and DFARS 227.7202 and by a
  License Agreement.

  IN ADDITION TO COPYRIGHT AND PATENT LAW, THIS SOFTWARE IS PROTECTED UNDER A
  SOURCE CODE AGREEMENT, NON-DISCLOSURE AGREEMENT (NDA), OR SIMILAR BINDING
  CONTRACT BETWEEN DATALIGHT, INC. AND THE LICENSEE ("BINDING AGREEMENT").
  YOUR RIGHT, IF ANY, TO COPY, PUBLISH, MODIFY OR OTHERWISE USE THE SOFTWARE,
  IS SUBJECT TO THE TERMS AND CONDITIONS OF THE BINDING AGREEMENT.  BY USING
  THE SOFTWARE IN ANY MANNER, IN WHOLE OR IN PART, YOU AGREE TO BE BOUND BY
  THE TERMS OF THE BINDING AGREEMENT.  CONTACT DATALIGHT, INC. AT THE ADDRESS
  SET FORTH ABOVE IF YOU OBTAINED THIS SOFTWARE IN ERROR.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                              Description

    This header file contains stuff that is shared between multiple OIL
    implementations.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                              Revision History
    $Log: rloilcmn.h $
    Revision 1.1  2005/05/24 16:26:06  Pauli
    Initial revision
    Revision 1.1  2005/05/24 17:26:06Z  Garyp
    Initial revision
    Revision 1.1  2005/05/24 17:26:06Z  pauli
    Initial revision
    Revision 1.2  2005/05/24 17:26:05Z  garyp
    Fixed REL_TRANSACT_RESERVED_MASK to be defined correctly -- fortunately
    it was not being used yet anywhere.
    Revision 1.1  2005/03/23 23:12:26Z  GaryP
    Initial revision
---------------------------------------------------------------------------*/

#ifndef RLOILCMN_H_INCLUDED
#define RLOILCMN_H_INCLUDED


/*-------------------------------------------------------------------
    These are the transaction point modes.  The various options may
    be OR'd together to create the desired automatic transaction
    configuration.  Automatic transactions can be disabled by using
    the REL_TRANSACT_MANUAL option.  While in manual transaction
    mode, the IOCTL transaction call must be made periodically
    to commit (transact) changes.
-------------------------------------------------------------------*/
#define REL_TRANSACT_MANUAL            0x00000000
#define REL_TRANSACT_DIR_CREATE        0x00000001
#define REL_TRANSACT_DIR_DELETE        0x00000002
#define REL_TRANSACT_DIR_RENAME        0x00000004
#define REL_TRANSACT_FILE_CREATE       0x00000008
#define REL_TRANSACT_FILE_WRITE        0x00000010
#define REL_TRANSACT_FILE_FLUSH        0x00000020
#define REL_TRANSACT_FILE_TRUNCATE     0x00000040
#define REL_TRANSACT_FILE_CLOSE        0x00000080
#define REL_TRANSACT_FILE_RENAME       0x00000100
#define REL_TRANSACT_FILE_DELETE       0x00000200
#define REL_TRANSACT_ATTRIBUTES        0x00000400
#define REL_TRANSACT_VOL_FULL          0x00000800
#define REL_TRANSACT_VOL_DISMOUNT      0x00001000
#define REL_TRANSACT_POWERDOWN         0x00002000
#define REL_TRANSACT_RESERVED_MASK     0xFFFFC000


#endif  /* #ifndef RLOILCMN_H_INCLUDED */
