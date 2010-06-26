/*******************************************************************************\
*                                                                              *
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION           *
*                                                                              *
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE         *
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE      *
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO      *
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT       *
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL        *
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.         *
*                                                                              *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:   bthal_lineparser.c
*
*   DESCRIPTION: This file contains unicode specific routines.
*
*   AUTHOR:      Udi Ron
*
\*******************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lineparser.h"

#define LINE_PARSER_DELIMS	" \n\t"

typedef enum
{
	_LINE_PARSER_CHAR_TYPE_QUOTE,
	_LINE_PARSER_CHAR_TYPE_DELIM,
	_LINE_PARSER_CHAR_TYPE_NORMAL,
	_LINE_PARSER_CHAR_TYPE_END
} _lineParserCharType;

typedef enum
{
	_LINE_PARSER_STATE_IN_DELIMITER,
	_LINE_PARSER_STATE_IN_TOKEN,
	_LINE_PARSER_STATE_START_QUOTED_TOKEN,
	_LINE_PARSER_STATE_IN_QUOTED_TOKEN,
	_LINE_PARSER_STATE_END_QUOTED_TOKEN
} _lineParserParsingState;

static U32 	_lineParserNextArgIndex = 0;
static U32 	_lineParserNumOfArgs = 0;
char 	*_lineParserArgs[LINE_PARSER_MAX_NUM_OF_ARGUMENTS];
char	_lineParserLine[LINE_PARSER_MAX_LINE_LEN + 1];
static BOOL	parsingFailed = FALSE;

_lineParserCharType GetCharType(char c, const char* delimiters)
{
	if (c == (char)34)
	{
		return _LINE_PARSER_CHAR_TYPE_QUOTE;
	}
	else if (strchr(delimiters, c) != NULL)
	{
		return _LINE_PARSER_CHAR_TYPE_DELIM;
	}
	else if (c == '\0')
	{
		return _LINE_PARSER_CHAR_TYPE_END;
	}
	else
	{
		return _LINE_PARSER_CHAR_TYPE_NORMAL;
	}
}

LINE_PARSER_STATUS LINE_PARSER_ParseLine(U8 *line, const char* delimiters)
{
	char *pos = NULL;
	char *lineEnd;
	_lineParserParsingState state;
	_lineParserCharType	charType;

	_lineParserNextArgIndex = 0;
	_lineParserNumOfArgs = 0;
	parsingFailed = TRUE;

	OS_StrCpy(_lineParserLine, (char*)line);

	if (OS_StrLen(_lineParserLine)== 0)
	{
		parsingFailed = FALSE;
		return LINE_PARSER_STATUS_SUCCESS;
	}

	pos = _lineParserLine;
	lineEnd = _lineParserLine + OS_StrLen(_lineParserLine);

	state = _LINE_PARSER_STATE_IN_DELIMITER;

	while (pos <= lineEnd)
	{
		charType = GetCharType(*pos, delimiters);

		switch (state)
		{
		case _LINE_PARSER_STATE_IN_DELIMITER:

			switch (charType)
			{
			case _LINE_PARSER_CHAR_TYPE_QUOTE:

				state = _LINE_PARSER_STATE_START_QUOTED_TOKEN;

				break;

			case _LINE_PARSER_CHAR_TYPE_DELIM:

				break;

			case _LINE_PARSER_CHAR_TYPE_NORMAL:
				_lineParserArgs[_lineParserNumOfArgs] = pos;
				++_lineParserNumOfArgs;
				state = _LINE_PARSER_STATE_IN_TOKEN;
				break;

			case _LINE_PARSER_CHAR_TYPE_END:

				break;
			};
			
			break;

		case _LINE_PARSER_STATE_START_QUOTED_TOKEN:

			switch (charType)
			{
			case _LINE_PARSER_CHAR_TYPE_QUOTE:
				_lineParserArgs[_lineParserNumOfArgs] = pos;
				++_lineParserNumOfArgs;
				state = _LINE_PARSER_STATE_END_QUOTED_TOKEN;

				break;

			case _LINE_PARSER_CHAR_TYPE_DELIM:
			case _LINE_PARSER_CHAR_TYPE_NORMAL:
				_lineParserArgs[_lineParserNumOfArgs] = pos;
				++_lineParserNumOfArgs;
				state = _LINE_PARSER_STATE_IN_QUOTED_TOKEN;

				break;

			case _LINE_PARSER_CHAR_TYPE_END:
				return LINE_PARSER_STATUS_FAILED;

			};
			
			break;

		case _LINE_PARSER_STATE_IN_TOKEN:

			switch (charType)
			{
			case _LINE_PARSER_CHAR_TYPE_QUOTE:
				return LINE_PARSER_STATUS_FAILED;

			case _LINE_PARSER_CHAR_TYPE_DELIM:
				*pos = '\0';
				state = _LINE_PARSER_STATE_IN_DELIMITER;

				break;

			case _LINE_PARSER_CHAR_TYPE_NORMAL:

				break;

			case _LINE_PARSER_CHAR_TYPE_END:
				state = _LINE_PARSER_STATE_IN_DELIMITER;

				break;
			};
			
			break;

		case _LINE_PARSER_STATE_IN_QUOTED_TOKEN:

			switch (charType)
			{
			case _LINE_PARSER_CHAR_TYPE_QUOTE:
				state = _LINE_PARSER_STATE_END_QUOTED_TOKEN;

				break;

			case _LINE_PARSER_CHAR_TYPE_DELIM:

				break;

			case _LINE_PARSER_CHAR_TYPE_NORMAL:

				break;

			case _LINE_PARSER_CHAR_TYPE_END:

				return LINE_PARSER_STATUS_FAILED;

			};
			
			break;

		case _LINE_PARSER_STATE_END_QUOTED_TOKEN:

			switch (charType)
			{
			case _LINE_PARSER_CHAR_TYPE_QUOTE:
			case _LINE_PARSER_CHAR_TYPE_NORMAL:

				return LINE_PARSER_STATUS_FAILED;


			case _LINE_PARSER_CHAR_TYPE_DELIM:
			case _LINE_PARSER_CHAR_TYPE_END:

				*(pos-1) = '\0';
				state = _LINE_PARSER_STATE_IN_DELIMITER;

				break;
			};
			
			break;
		};

		++pos;
	}

	parsingFailed = FALSE;

	return LINE_PARSER_STATUS_SUCCESS;
}

U32 LINE_PARSER_GetNumOfArgs(void)
{
	return _lineParserNumOfArgs;
}

BOOL LINE_PARSER_AreThereMoreArgs(void)
{
	if ((parsingFailed == TRUE) || (_lineParserNextArgIndex >= _lineParserNumOfArgs))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void LINE_PARSER_ToLower(U8 *str)
{
	U32 i; 
	size_t len = OS_StrLen((char*)str);

	for (i = 0; i < len; ++i)
	{
		str[i] = (U8)tolower(str[i]);
	}
}

void LINE_PARSER_ToUpper(U8 *str)
{
	U32 i;
	size_t len = OS_StrLen((char*)str);

	for (i = 0; i < len; ++i)
	{
		str[i] = (U8)toupper(str[i]);
	}
}

typedef struct
{
	BtlModuleType		type;
	U8				name[LINE_PARSER_MAX_MODULE_NAME_LEN + 1];
} _lineParserMoudleData;

static const  _lineParserMoudleData _lineParserModuleDataArray[] =
{
	 {BTL_MODULE_TYPE_BMG, LINE_PARSER_MODULE_NAME_BMG},
	 {BTL_MODULE_TYPE_SPP, LINE_PARSER_MODULE_NAME_SPP},
	 {BTL_MODULE_TYPE_OPPC, LINE_PARSER_MODULE_NAME_OPPC},
	 {BTL_MODULE_TYPE_OPPS, LINE_PARSER_MODULE_NAME_OPPS},
	 {BTL_MODULE_TYPE_BPPSND, LINE_PARSER_MODULE_NAME_BPPSND},
	 {BTL_MODULE_TYPE_PBAPS, LINE_PARSER_MODULE_NAME_PBAPS},
	 {BTL_MODULE_TYPE_PAN, LINE_PARSER_MODULE_NAME_PAN},
	 {BTL_MODULE_TYPE_AVRCPTG, LINE_PARSER_MODULE_NAME_AVRCPTG},
	 {BTL_MODULE_TYPE_FTPS, LINE_PARSER_MODULE_NAME_FTPS},
	 {BTL_MODULE_TYPE_FTPC, LINE_PARSER_MODULE_NAME_FTPC},
	 {BTL_MODULE_TYPE_VG, LINE_PARSER_MODULE_NAME_VG},
	 {BTL_MODULE_TYPE_AG, LINE_PARSER_MODULE_NAME_AG},
	 {BTL_MODULE_TYPE_RFCOMM, LINE_PARSER_MODULE_NAME_RFCOMM},
	 {BTL_MODULE_TYPE_A2DP, LINE_PARSER_MODULE_NAME_A2DP},
	 {BTL_MODULE_TYPE_HID, LINE_PARSER_MODULE_NAME_HID},
	 {BTL_MODULE_TYPE_MDG, LINE_PARSER_MODULE_NAME_MDG},	 
	 {BTL_MODULE_TYPE_BIPINT, LINE_PARSER_MODULE_NAME_BIPINT},
	 {BTL_MODULE_TYPE_BIPRSP, LINE_PARSER_MODULE_NAME_BIPRSP},
	 {BTL_MODULE_TYPE_SAPS, LINE_PARSER_MODULE_NAME_SAPS},
	 {BTL_MODULE_TYPE_MAIN, LINE_PARSER_MODULE_NAME_MAIN},
	 {BTL_MODULE_TYPE_BSC, LINE_PARSER_MODULE_NAME_BSC},
	 {BTL_MODULE_TYPE_L2CAP, LINE_PARSER_MODULE_NAME_L2CAP},
	 {BTL_MODULE_TYPE_L2CAP_THROUGHPUT, LINE_PARSER_MODULE_NAME_L2CAP_THROUGHPUT},
	 {BTL_MODULE_TYPE_FMRADIO, LINE_PARSER_MODULE_NAME_FMRADIO}
	 
};

static const U32 lineParserNumOfModules = sizeof(_lineParserModuleDataArray) / sizeof(_lineParserMoudleData);

LINE_PARSER_STATUS LINE_PARSER_GetNextModuleType(BtlModuleType *moduleType)
{
	LINE_PARSER_STATUS status;
	U8					name[LINE_PARSER_MAX_MODULE_NAME_LEN + 1];
	U32					i;

	status = LINE_PARSER_GetNextStr(name, LINE_PARSER_MAX_MODULE_NAME_LEN);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		return status;
	}
	
	for (i = 0; i < lineParserNumOfModules; ++i)
	{
		LINE_PARSER_ToUpper(name);
		
		if (OS_StrCmp((char *)_lineParserModuleDataArray[i].name, (char*)name) == 0)
		{
			*moduleType = _lineParserModuleDataArray[i].type;
			return LINE_PARSER_STATUS_SUCCESS;
		}
	}
	
	return LINE_PARSER_STATUS_FAILED;
}

LINE_PARSER_STATUS LINE_PARSER_GetNextChar(U8 *c)
{
	char tempStr[200];
	LINE_PARSER_STATUS status = LINE_PARSER_GetNextStr((U8*)tempStr, 200);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		return status;
	}
	else
	{
		if (OS_StrLen(tempStr) > 1)
		{
			return LINE_PARSER_STATUS_ARGUMENT_TOO_LONG;
		}
		else
		{
			*c = tempStr[0];

			return LINE_PARSER_STATUS_SUCCESS;
		}
	}
}

LINE_PARSER_STATUS LINE_PARSER_GetNextStr(U8 *str, U8 len)
{
	if (parsingFailed == TRUE)
	{
		return LINE_PARSER_STATUS_FAILED;
	}

	if (LINE_PARSER_AreThereMoreArgs() == FALSE)
	{
		return LINE_PARSER_STATUS_NO_MORE_ARGUMENTS;
	}
			
	if (OS_StrLen(_lineParserArgs[_lineParserNextArgIndex]) > len)
	{
		parsingFailed = TRUE;
		return LINE_PARSER_STATUS_ARGUMENT_TOO_LONG;
	}

	OS_StrCpy((char*)str, _lineParserArgs[_lineParserNextArgIndex]);

	++_lineParserNextArgIndex;

	return LINE_PARSER_STATUS_SUCCESS;
}

LINE_PARSER_STATUS LINE_PARSER_GetNextU8(U8 *value, BOOL hex)
{
	U32	tempValue = 0;
	
	LINE_PARSER_STATUS status = LINE_PARSER_GetNextU32(&tempValue, hex);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		return status;
	}

	if (tempValue > 0xFF)
	{
		parsingFailed = TRUE;
		return LINE_PARSER_STATUS_FAILED;
	}

	*value = (U8)tempValue;
	
	return LINE_PARSER_STATUS_SUCCESS;
}

LINE_PARSER_STATUS LINE_PARSER_GetNextU16(U16 *value, BOOL hex)
{
	U32	tempValue = 0;
	
	LINE_PARSER_STATUS status = LINE_PARSER_GetNextU32(&tempValue, hex);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		return status;
	}

	if (tempValue > 0xFFFF)
	{
		parsingFailed = TRUE;
		return LINE_PARSER_STATUS_FAILED;
	}

	*value = (U16)tempValue;
	
	return LINE_PARSER_STATUS_SUCCESS;
}

LINE_PARSER_STATUS LINE_PARSER_GetNextU32(U32 *value, BOOL hex)
{
	int 		num;
	S32		signedValue = 0;

	if (parsingFailed ==TRUE)
	{
		return LINE_PARSER_STATUS_FAILED;
	}

	if (LINE_PARSER_AreThereMoreArgs() == FALSE)
	{
		return LINE_PARSER_STATUS_NO_MORE_ARGUMENTS;
	}

	if (hex == TRUE)
	{
		num = sscanf(_lineParserArgs[_lineParserNextArgIndex], "%x", &signedValue);
	}
	else
	{
		num = sscanf(_lineParserArgs[_lineParserNextArgIndex], "%d", &signedValue);
	}
	

	if ((num == 0) || (num == EOF))
	{	
		parsingFailed = TRUE;
		return LINE_PARSER_STATUS_FAILED;
	}
	else
	{
		if (signedValue < 0)
		{
			parsingFailed = TRUE;
			return LINE_PARSER_STATUS_FAILED;

		}

		*value = (U32)signedValue;
		++_lineParserNextArgIndex;
				
		return LINE_PARSER_STATUS_SUCCESS;
	}
}

LINE_PARSER_STATUS LINE_PARSER_GetNextS8(S8 *value)
{
	S32	tempValue = 0;
	
	LINE_PARSER_STATUS status = LINE_PARSER_GetNextS32(&tempValue);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		return status;
	}

	if ((U32)tempValue > 0xFF)
	{
		parsingFailed = TRUE;
		return LINE_PARSER_STATUS_FAILED;
	}

	*value = (S8)tempValue;
	
	return LINE_PARSER_STATUS_SUCCESS;
}

LINE_PARSER_STATUS LINE_PARSER_GetNextS16(S16 *value)
{
	S32	tempValue = 0;
	
	LINE_PARSER_STATUS status = LINE_PARSER_GetNextS32(&tempValue);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		return status;
	}

	if ((U32)tempValue > 0xFFFF)
	{
		parsingFailed = TRUE;
		return LINE_PARSER_STATUS_FAILED;
	}

	*value = (S16)tempValue;
	
	return LINE_PARSER_STATUS_SUCCESS;
}

LINE_PARSER_STATUS LINE_PARSER_GetNextS32(S32 *value)
{
	int 		num;

	if (parsingFailed ==TRUE)
	{
		return LINE_PARSER_STATUS_FAILED;
	}

	if (LINE_PARSER_AreThereMoreArgs() == FALSE)
	{
		return LINE_PARSER_STATUS_NO_MORE_ARGUMENTS;
	}
	
	if (_lineParserArgs[_lineParserNextArgIndex][0] != '-')
	{
		num = sscanf(_lineParserArgs[_lineParserNextArgIndex], "%d", value);
	}
	else
	{
		num = sscanf(_lineParserArgs[_lineParserNextArgIndex+1], "%d", value);
	}

	if ((num == 0) || (num == EOF))
	{	
		parsingFailed = TRUE;
		return LINE_PARSER_STATUS_FAILED;
	}
	else
	{
		++_lineParserNextArgIndex;
				
		return LINE_PARSER_STATUS_SUCCESS;
	}
}

LINE_PARSER_STATUS LINE_PARSER_GetNextBool(BOOL *value)
{
	char tempStr[200];
	LINE_PARSER_STATUS status = LINE_PARSER_GetNextStr((U8*)tempStr, 200);

	if (status != LINE_PARSER_STATUS_SUCCESS)
	{
		return status;
	}
	else
	{
		LINE_PARSER_ToLower((U8*)tempStr);

		if (OS_StrCmp(tempStr, "true") == 0)
		{
			*value = TRUE;
			return LINE_PARSER_STATUS_SUCCESS;
		}
		else if (OS_StrCmp(tempStr, "false") == 0)
		{
			*value = FALSE;
			return LINE_PARSER_STATUS_SUCCESS;
		}
		else
		{
			parsingFailed = TRUE;
			return LINE_PARSER_STATUS_FAILED;
		}
	}
}

