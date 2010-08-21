/*****************************************************************************/
/*  DEFS.C v2.54                                                             */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/*    This file defines some data structures declared in stdio.h.            */
/*****************************************************************************/
#include <stdio.h>

/*---------------------------------------------------------------------------*/
/* Define the streams for stdin, stdout, and stderr, and assign them as the  */
/* first three streams in _FTABLE upon its initialization.                   */
/*---------------------------------------------------------------------------*/
_DATA_ACCESS
FILE _ftable[_NFILE] = {{  0, NULL, NULL, NULL, NULL, (_MODER | _IOLBF), 0},
                        {  1, NULL, NULL, NULL, NULL, (_MODEW | _IOLBF), 1},
                        {  2, NULL, NULL, NULL, NULL, (_MODEW | _IONBF), 2},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           3},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           4},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           5},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           6},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           7},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           8},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           9},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           10},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           11},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           12},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           13},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           14},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           15},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           16},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           17},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           18},
			{ -1, NULL, NULL, NULL, NULL,  _IONBF,           19} 
			};

/*---------------------------------------------------------------------------*/
/* Allocate a table to store the filenames of temporary files, so they can   */
/* be deleted when they are closed.                                          */
/*---------------------------------------------------------------------------*/
_DATA_ACCESS char _tmpnams[_NFILE][L_tmpnam];

/*---------------------------------------------------------------------------*/
/* This global variable _FT_END is used to mark the end of used streams in   */
/* the file table.                                                           */
/*---------------------------------------------------------------------------*/
_DATA_ACCESS int _ft_end = 3; 
