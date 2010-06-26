#ifndef ATB_COMMON_H
#define ATB_COMMON_H

/**********************************************
 * STANDARD DEFINITIONS
 *
 **********************************************/
#ifndef USHORT
#define EXTERN extern
#define GLOBAL
#define LOCAL static
#define ULONG unsigned long
#define LONG long
#define USHORT unsigned short
#define SHORT short
#define UBYTE unsigned char
#define BYTE unsigned char
#define BOOL unsigned char
#endif
#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

/* UNICODE CHARACTERS */

#define UNICODE_EOLN				0x0000
#define UNICODE_STARTHIGHLIGHT		0x0100 
#define UNICODE_ENDHIGHLIGHT		0x0200
#define UNICODE_LINEFEED			0x0A00
#define UNICODE_CR					0x0D00

#define UNICODE_SPACE				0x2000
#define UNICODE_HASH				0x2300
#define UNICODE_STAR				0x2A00
#define UNICODE_PLUS				0x2B00
#define UNICODE_FULLSTOP			0x2E00
#define UNICODE_EXCLAMATION			0x2100
#define UNICODE_QUESTION			0x3F00

#define UNICODE_ASCII				0xFF00
#define UNICODE_NONASCII			0x00FF
#define UNICODE_EXTENDEDASCII		0x8000

#define WHOLESTRING					0xFFFF	// parameter for length in uc... functions, copies until null is found
#define UNICODE_WIDEST_CHAR			0xFFFF	/* Used to return width of widest character in font */


/**********************************************
 * T_ATB_TEXT
 *
 * Standard text data structure
 *********************************************/

typedef struct
{
	UBYTE	dcs;
	USHORT	len;
	UBYTE	*data;
}
T_ATB_TEXT;


/**********************************************
 * T_ATB_DCS
 *
 * Data coding schemes for text
 *********************************************/

typedef enum
{
	ATB_DCS_ASCII,
	ATB_DCS_UNICODE
}
T_ATB_DCS;


/**********************************************
 * T_ATB_WIN_SIZE
 *
 * Size of editor window
 *********************************************/
 
typedef struct
{
	SHORT px;
	SHORT py;
	SHORT sx;
	SHORT sy;
}
T_ATB_WIN_SIZE;


/**********************************************
 * FUNCTION PROTOTYPES
 *
 *********************************************/

/* ATB memory allocation */

UBYTE* 	ATB_mem_Alloc(USHORT size);
void	ATB_mem_Free(UBYTE* address, USHORT size);

/* Character and string handling */

/*SPR2175, new function for conversion*/
int 	ATB_convert_String(			char * ipString,	UBYTE ipDataType,	int ipLength,
										char * opString, 	UBYTE opDataType,	int opLength, UBYTE addNull);
void convertUnpacked7To8 (char * ipString, int ipLength, 
						  char * opString, int opLength);

char 	ATB_char_GSM(char ascii_character);
char	ATB_char_Ascii(USHORT character);
USHORT	ATB_char_Unicode(char character);
BOOL	ATB_char_IsAscii(USHORT character);
BOOL	ATB_char_IsAlphabetic(USHORT character);
BOOL	ATB_char_IsNumeric(USHORT character);
BOOL	ATB_char_IsVisible(USHORT character);
UBYTE	ATB_string_Size(T_ATB_TEXT *text);
void	ATB_string_SetChar(T_ATB_TEXT *text, USHORT textIndex, USHORT character);
USHORT	ATB_string_GetChar(T_ATB_TEXT *text, USHORT textIndex);
BOOL	ATB_string_MoveLeft(T_ATB_TEXT *text, USHORT textIndex, USHORT offset);
BOOL	ATB_string_MoveRight(T_ATB_TEXT *text, USHORT textIndex, USHORT offset, USHORT size);
USHORT ATB_string_Length(T_ATB_TEXT* text);
USHORT	ATB_string_UCLength(USHORT* text);
void 	ATB_string_Copy(T_ATB_TEXT* dest, T_ATB_TEXT* src);
void	ATB_string_Concat(T_ATB_TEXT* dest, T_ATB_TEXT* srcen);
SHORT	ATB_string_FindChar(T_ATB_TEXT* text, USHORT character);
void	ATB_string_ToUnicode(T_ATB_TEXT *dest, T_ATB_TEXT *src);
void	ATB_string_ToAscii(T_ATB_TEXT *dest, T_ATB_TEXT *src);
USHORT	ATB_string_IndexMove(T_ATB_TEXT *text, USHORT textIndex, SHORT offset);

/* Added to remove warnings Aug -11 */
EXTERN UBYTE  utl_cvt8To7     ( const UBYTE* source,
                                UBYTE  source_len,
                                UBYTE* dest,
                                UBYTE  bit_offset);
/* End - remove warning Aug -11 */

#endif
