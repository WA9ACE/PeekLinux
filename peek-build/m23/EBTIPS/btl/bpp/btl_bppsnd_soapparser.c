/***************************************************************************\
                                                                          
    File               		:     btl_bppsnd_soapParser.c

    Description                                                           
    -----------                                                           
    	Soap builder for Basic Printing Profile.
                                                                                                                                       
    Created by			:          Naveen Kumar K          
    Date created		:          1 June 2006
    Environment		:	    Embedded
    $Archive: /Agere/src/Lean/basecust/custt7/bt/btips/apps/Bpp/BppSoap.c $
    $Revision: 2 $                                      
    $Date: 10/12/06 4:18p $   

    iFoundry Systems Singapore  (c) 2002-2006
    
\***************************************************************************/
/*

REVISION LOG
------------

$Log: /Agere/src/Lean/basecust/custt7/bt/btips/apps/Bpp/BppSoap.c $
 
 2     10/12/06 4:18p Naveen Kumar
 Latest BPP with ESI fix patched, New application for Reference Object
 Channel, XHTML Builder, and Handsfree patch.
 
 1     9/28/06 11:35a Naveen Kumar
 
 1     8/30/06 11:15a Avinash Vemuluru
 
 1     8/01/06 5:37p Avinash Vemuluru
 
 8     6/26/06 10:49a Naveen Kumar
 
 7     6/22/06 5:07p Naveen Kumar
 
 6     6/22/06 2:14p Naveen Kumar
 
 5     6/15/06 4:06p Naveen Kumar
 
 4     6/14/06 10:32a Naveen Kumar
 
 3     6/06/06 10:26a Naveen Kumar
 
 2     6/01/06 4:09p Naveen Kumar
 For June 2nd Release !
 
 1     6/01/06 1:35p Naveen Kumar
 Initial Check in
 */
/***********************************************************************/

#include "btl_config.h"
#if BTL_CONFIG_BPPSND == BTL_CONFIG_ENABLED


/******* Include Files **************/

#include "oslib.h"
#include "osapi.h"
#include "btl_defs.h"
#include "Btl_bppsnd_soap.h"
#include "btl_unicode.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_BPPSND);

/******* Local #defines  ************/ 

#define INITIAL_SKIP_XML_HEADER_CNT			(4)
#define MAX_NUM_COPIES_SUPPORTED				(9)
#define MAX_ATTR_RESP_VALUE_SIZE				(50)
#define MAX_ATTR_NAME_SIZE					(40)

/***************************PRINTINGvsREPORT***********************************/

/******structures & unions**********/

enum
{
	ELEMENTS_NO_CHILD,
	ELEMENTS_HAVE_CHILD,
	ELEMENTS_NO_MATCH,
	ELEMENTS_NULL
};

typedef struct 
{
	BtlUtf8 attrRespName[MAX_ATTR_RESP_VALUE_SIZE];

}xmlPrintAttrResp;

typedef struct 
{
	BtlUtf8 attrName[MAX_ATTR_NAME_SIZE];

}xmlPrintAttrType;

/********************************************************/

/******* Global Variables ************/
xmlPrintAttrResp attrValue;
xmlPrintAttrType attrType;
U8 		*xhtmlPtr = NULL;


/******* Static  Variables **********/

static char parseStart = 0;

/******* Extern  Variables **********/


/******* Local Function  Prototypes ****/


/* -------------------------- Code -------------------------- */

 

/************************************************************************ BPP PARSER ***********************************************************************************************/

int parseBppXmlRspData(UIAppInstanceData * instanceData, U8 *xmlPtr, U32 totLength)
{
	char *parsePtr = NULL;
	char *eof = NULL;
	int len,count = 0;
	int	parseType = 0;

	parsePtr = (char*)xmlPtr;
	
	do
	{
		if(parseStart == 0)
		{
			len = totLength + 10;
			eof = parsePtr + len;

			/* Skip and move to the start of first element useful element we need to parse: <PrinterName>*/

			for(;count<INITIAL_SKIP_XML_HEADER_CNT;count++)
			{
				if(((parsePtr = StartOfElement(parsePtr, eof)) != NULL) && (*parsePtr) && (parsePtr + 1 < eof))
					parsePtr++;
				else
					return 0;
			}
			parseStart = 1;
		}

		/*Parsing complete*/
		if(*parsePtr == '/')
		{
			parsePtr = eof;
			break;
		}

		/* Skip any white spaces if any before we start to parse something */
		parsePtr = bpp_skipWhiteSpaces(parsePtr, eof);

		/* Next step, parse for the attribute type */
		parsePtr = parseAttrType(parsePtr,eof);

		/* Clear white space before you start to read the values */
		parsePtr = bpp_skipWhiteSpaces(parsePtr, eof);

		/* PRINTER NAME */
		if(OS_StriCmp((char*)attrType.attrName, MARGINS) == 0)
			parseType = ELEMENTS_NO_CHILD;

		/* IMAGE_FORMATS_SUPPORTED */
		else if(
		/*MEDIA_LOADED*/
		(OS_StriCmp((char*)attrType.attrName, MEDIA_LOADED) == 0))
			parseType = ELEMENTS_HAVE_CHILD;
		else
		{	
			parseType = ELEMENTS_NULL;
			if((OS_StriCmp((char*)attrType.attrName, BASIC_TEXT_PAGE_WIDTH) == 0) )
			{
				U32 textPageW=0;

				parsePtr = parseAttrValueNoChild(parsePtr,eof);

				textPageW = OS_AtoU32((char*)attrValue.attrRespName);

				instanceData->btAppBppInstanceData.basicTextPageWidth = textPageW;	
			}
			if((OS_StriCmp((char*)attrType.attrName, BASIC_TEXT_PAGE_HEIGHT) == 0) )
			{
				U32 textPageH=0;

				parsePtr = parseAttrValueNoChild(parsePtr,eof);

				textPageH = OS_AtoU32((char*)attrValue.attrRespName);

				instanceData->btAppBppInstanceData.basicTextPageHeight= textPageH;	
			}
			if((OS_StriCmp((char*)attrType.attrName, PRINTER_GENERAL_CURRENT_OPERATOR) == 0) )
			{
				parsePtr = parseAttrValueNoChild(parsePtr,eof);

				OS_StrnCpy((char*)instanceData->btAppBppInstanceData.printerGeneralCurrentOperator,
							(char*)attrValue.attrRespName,
							BTL_BPPSND_MAX_PRINTER_GENERAL_CURRENT_OPERATOR);
			}
			if((OS_StriCmp((char*)attrType.attrName, PRINTER_NAME) == 0) )
			{
				parsePtr = parseAttrValueNoChild(parsePtr,eof);

				OS_StrnCpy((char*)instanceData->btAppBppInstanceData.printerName,
							(char*)attrValue.attrRespName,
							BTL_BPPSND_MAX_PRINTER_NAME_LEN_COPY);
			}
			if((OS_StriCmp((char*)attrType.attrName, PRINTER_LOCATION) == 0) )
			{
				parsePtr = parseAttrValueNoChild(parsePtr,eof);
				
				OS_StrnCpy((char*)instanceData->btAppBppInstanceData.printerLocation,
							(char*)attrValue.attrRespName,
							BTL_BPPSND_MAX_PRINTER_LOCATION_LEN_COPY);
			}
			/*COLOR_SUPPORTED*/
			if((OS_StriCmp((char*)attrType.attrName, COLOR_SUPPORTED) == 0) )
			{
				parsePtr = parseAttrValueNoChild(parsePtr,eof);

				if((OS_StriCmp((char*)attrValue.attrRespName, "true") == 0) )
				{
					instanceData->btAppBppInstanceData.colorSupported = 1;
				}
				else if ((OS_StriCmp((char*)attrValue.attrRespName, "false") == 0))
				{
					instanceData->btAppBppInstanceData.colorSupported = 0;
				}
			}
			if((OS_StriCmp((char*)attrType.attrName, QUEUE_JOB_COUNT) == 0) )
			{
				parsePtr = parseAttrValueNoChild(parsePtr,eof);
				
				instanceData->btAppBppInstanceData.queuedJobCount = OS_AtoU32((char*)attrValue.attrRespName);
			}
			if((OS_StriCmp((char*)attrType.attrName, JOB_ID) == 0) )
			{
				U32 jobId=0;/* OS_AtoU32(const char * string) */

				parsePtr = parseAttrValueNoChild(parsePtr,eof);

				jobId = OS_AtoU32((char*)attrValue.attrRespName);

				instanceData->btAppBppInstanceData.jobId= jobId;	
			}
			if((OS_StriCmp((char*)attrType.attrName, JOB_STATE) == 0) )
			{
				parsePtr = parseAttrValueNoChild(parsePtr,eof);

				OS_StrnCpy((char*)instanceData->btAppBppInstanceData.jobState , 
							(char*)attrValue.attrRespName,
							BTL_BPPSND_MAX_JOB_STATE_LEN);
			}
			if((OS_StriCmp((char*)attrType.attrName, JOB_NAME) == 0) )
			{
				parsePtr = parseAttrValueNoChild(parsePtr,eof);
				
				OS_StrnCpy((char*)instanceData->btAppBppInstanceData.jobName,
							(char*)attrValue.attrRespName,
							BTL_BPPSND_MAX_JOB_NAME_LEN_COPY);
			}
			if((OS_StriCmp((char*)attrType.attrName, JOB_ORIGINATING_USER_NAME) == 0) )
			{
				parsePtr = parseAttrValueNoChild(parsePtr,eof);
				
				OS_StrnCpy((char*)instanceData->btAppBppInstanceData.jobOriginatingUserName,
							(char*)attrValue.attrRespName,
							BTL_BPPSND_MAX_JOB_ORIGINATING_USER_NAME_LEN_COPY);
			}
			if((OS_StriCmp((char*)attrType.attrName, JOB_MEDIA_SHEET_COMPLETED) == 0) )
			{
				U32 mediaComp = 0;
				
				parsePtr = parseAttrValueNoChild(parsePtr,eof);
				
				mediaComp = OS_AtoU32((char*)attrValue.attrRespName);

				instanceData->btAppBppInstanceData.jobMediaSheetsCompleted= mediaComp;	
			}
			if((OS_StriCmp((char*)attrType.attrName, NUM_INTERVENING_JOBS) == 0))
			{
				U32 interveningJob = 0;
				
				parsePtr = parseAttrValueNoChild(parsePtr,eof);
				
				interveningJob = OS_AtoU32((char*)attrValue.attrRespName);

				instanceData->btAppBppInstanceData.numOfInterveningJobs = interveningJob;	
			}
						
				/*OPERATION_STATUS*/
			if((OS_StriCmp((char*)attrType.attrName, OPERATION_STATUS) == 0))
			{
				U16 operStatus = 0;
				
				parsePtr = parseAttrValueNoChild(parsePtr,eof);
				sscanf((char*)attrValue.attrRespName,"%x",&operStatus);
				instanceData->btAppBppInstanceData.operationStatus = operStatus;	
			}
			/* PRINTER STATE */
			if((OS_StriCmp((char*)attrType.attrName, PRINTER_STATE) == 0))
			{
 				parsePtr = parseAttrValueNoChild(parsePtr,eof);
				bppSetPrinterState(instanceData, attrValue.attrRespName);
				
			}
			/* PRINTER STATE REASONS */
			else if((OS_StriCmp((char*)attrType.attrName, PRINTER_STATE_REASONS) == 0))
			{
				parsePtr = parseAttrValueNoChild(parsePtr,eof);
				bppSetPrinterStateReason(instanceData, attrValue.attrRespName);
			}
			/*NUMBER_UP_SUPPORTED*/
			else if((OS_StriCmp((char*)attrType.attrName, NUMBER_UP_SUPPORTED) == 0))
			{
				U16 numberUp = 0;
				
				parsePtr = parseAttrValueNoChild(parsePtr,eof);

				numberUp = (U16)OS_AtoU32((char*)attrValue.attrRespName);

				instanceData->btAppBppInstanceData.numberUp	= (U8)numberUp;
			}
			/*MAX_COPIES_SUPPORTED*/
			else if((OS_StriCmp((char*)attrType.attrName, MAX_COPIES_SUPPORTED) == 0))
			{
				U16 copies;

				parsePtr = parseAttrValueNoChild(parsePtr,eof);

				copies = (U16)OS_AtoU32((char*)attrValue.attrRespName);

				if(copies > MAX_NUM_COPIES_SUPPORTED)
					copies = MAX_NUM_COPIES_SUPPORTED;

			instanceData->btAppBppInstanceData.maxCopies = (U8)('0'+copies);
	
			}
			/*IMAGE_FORMATS_SUPPORTED*/
			else if((OS_StriCmp((char*)attrType.attrName, IMAGE_FORMATS_SUPPORTED) == 0) ||
				/*MEDIA_SIZES_SUPPORTED*/
				(OS_StriCmp((char*)attrType.attrName, MEDIA_SIZES_SUPPORTED) == 0) ||
				/*MEDIA_TYPES_SUPPORTED*/
				(OS_StriCmp((char*)attrType.attrName, MEDIA_TYPES_SUPPORTED) == 0) ||
				/* PRINTER_DOCUMENTFORMATS_SUPPORTED  */
				(OS_StriCmp((char*)attrType.attrName, PRINTER_DOCUMENTFORMATS_SUPPORTED) == 0) ||
				/* SIDES SUPPORTED */
				(OS_StriCmp((char*)attrType.attrName, SIDES_SUPPORTED) == 0) ||
				/*ORIENTATIONS_SUPPORTED*/
				(OS_StriCmp((char*)attrType.attrName, ORIENTATIONS_SUPPORTED) == 0) ||
				/*PRINT_QUALITY_SUPPORTED*/
				(OS_StriCmp((char*)attrType.attrName, PRINT_QUALITY_SUPPORTED) == 0))
			{
				parseType = ELEMENTS_NULL;
				parsePtr = parseNextractChildValues(instanceData,parsePtr,eof);
			}
			else
				parseType = ELEMENTS_NO_MATCH;
		}
		
		switch(parseType)
		{
			case ELEMENTS_NO_CHILD:
				parsePtr = parseAttrValueNoChild(parsePtr,eof);
				break;
				
			case ELEMENTS_HAVE_CHILD:
				parsePtr = parseAttrValueHaveChild(instanceData,parsePtr,eof);
				break;
				
			case ELEMENTS_NO_MATCH:
				break;

			case ELEMENTS_NULL:
			default:
				break;
		}
	}
	while(parsePtr<eof);
	parseStart = 0;
	return 1;
}

/********************************************************* BPP -XML PARSING FUNCTIONS ***************************************************/
char *parseAttrType(char *parsePtr, char *eof)
{
	int attrCount = 0;
	char *cpyPtr = NULL;
	/* BTL_LOG_DEBUG(("parseAttrType-The Attribute Name:Start %s",attrType.attrName)); */
	
	cpyPtr = parsePtr;
	OS_MemSet(attrType.attrName,0,sizeof(xmlPrintAttrType));

	attrCount = counttillEndOfElement(cpyPtr,eof);
	OS_StrnCpy((char*)attrType.attrName, parsePtr, attrCount);
	
	/* we have copied the Attribute type successfully,now move the pointer to read the value */
	parsePtr = parsePtr + attrCount + 1;
	
	return parsePtr;	
}

char *parseAttrValueNoChild(char *parsePtr, char *eof)
{
	int attrCount = 0;
	char *cpyPtr = NULL;
	
	cpyPtr = parsePtr;
	OS_MemSet(attrValue.attrRespName,0,sizeof(xmlPrintAttrResp));

	/* count till you get the start of the closing tag */
	attrCount = counttillStartOfElement(cpyPtr,eof);

	parsePtr = cpyPtr;
	if(attrCount >0)
		OS_StrnCpy((char*)attrValue.attrRespName,parsePtr,attrCount);
	else
		OS_StrCpy((char*)attrValue.attrRespName,"NO VALUE");


	/* move the pointer to the start of closing tag after copying */
	parsePtr = parsePtr + attrCount + 1;
	
	/* we have successfully parsed the "AttrValue", move to starting of the next "Element" */
	parsePtr = StartOfElement(parsePtr,eof);
	parsePtr++;
	
	return parsePtr;	
}

char *parseAttrValueHaveChild(UIAppInstanceData * instanceData,char *parsePtr, char *eof)
{
	int childTag = 0;
	U32 index=0;
	BOOL getLoadedMediaData = FALSE;
	
	/*We are starting a new parse - so set the pointer to start of the element*/
	parsePtr++;

	do
	{
		/* Skip any white spaces if any before we start to parse something */
		parsePtr = bpp_skipWhiteSpaces(parsePtr, eof);
		
		/* We have parsed the "Parent Tag", now we are at the start
	     	of the "First" Child Tag Parse the Type */
		if(!getLoadedMediaData&&(!OS_StriCmp((char*)attrType.attrName, LOADED_MEDIUM_DETAILS))&&
			instanceData->btAppBppInstanceData.numMediaLoaded<BTL_BPPSND_MAX_NUM_OF_MEDIA_LOADED)
		{
			instanceData->btAppBppInstanceData.numMediaLoaded++;
			getLoadedMediaData = TRUE;
		}
		parsePtr = parseAttrType(parsePtr,eof);
		
		/* Skip any white spaces if any before we start to parse something */
		parsePtr = bpp_skipWhiteSpaces(parsePtr, eof);

		if(*parsePtr != '<')
		{
			/* Now parse the attrValue of the "First Child" DEPTH = 1*/
			parsePtr = parseAttrValueNoChild(parsePtr,eof);
			if(getLoadedMediaData&&(!OS_StriCmp((char*)attrType.attrName, LOADED_MEDIUM_SIZE)))
			{
				index = instanceData->btAppBppInstanceData.numMediaLoaded -1;
				OS_StrnCpy((char*)instanceData->btAppBppInstanceData.mediaLoadedDetails[index].LoadedMediumSize,
							(char*)attrValue.attrRespName,
							BTL_BPPSND_MAX_MEDIA_SIZE_LEN);
			}
			else if(getLoadedMediaData&&(!OS_StriCmp((char*)attrType.attrName, LOADED_MEDIUM_TYPE)))
			{
				/* BTL_LOG_DEBUG(("  ")); */
				index = instanceData->btAppBppInstanceData.numMediaLoaded -1;
				OS_StrnCpy((char*)instanceData->btAppBppInstanceData.mediaLoadedDetails[index].LoadedMediumType,
							(char*)attrValue.attrRespName,
							BTL_BPPSND_MAX_MEDIA_TYPE_LEN);
			}
		}
		else
		{
			/*DEPTH = 2, so we need to parse more - do recursive*/
			parsePtr = parseAttrValueHaveChild(instanceData,parsePtr,eof);
		}

		if(*parsePtr != '/')
			childTag =1;	/* We have more than one child element */
		else
		{
			childTag = 0;
			parsePtr = StartOfElement(parsePtr, eof);
			parsePtr++;
		}
	}
	while(childTag);
	  
	return parsePtr;
}

char *parseNextractChildValues(UIAppInstanceData * instanceData, char *parsePtr, char *eof)
{
	int childTag = 0;
	
	/*We are starting a new parse - so set the pointer to start of the element*/
	parsePtr++;
	
	do
	{
		/* Skip any white spaces if any before we start to parse something */
		parsePtr = bpp_skipWhiteSpaces(parsePtr, eof);
		
		/* We have parsed the "Parent Tag", now we are at the start
	     	of the "First" Child Tag Parse the Type */
		parsePtr = parseAttrType(parsePtr,eof);

		/* Skip any white spaces if any before we start to parse something */
		parsePtr = bpp_skipWhiteSpaces(parsePtr, eof);

		if(*parsePtr != '<')
		{
			/* Now parse the attrValue of the "First Child" DEPTH = 1*/
			parsePtr = parseAttrValueNoChild(parsePtr,eof);

			/*Parsing done, now extract the values */
			extractAttrValues(instanceData,(char*)attrType.attrName, (char*)attrValue.attrRespName);
		}
		else
		{
			/*DEPTH = 2, so we need to parse more - do recursive*/
			parsePtr = parseAttrValueHaveChild(instanceData,parsePtr,eof);
		}

		if(*parsePtr != '/')
			childTag =1;	/* We have more than one child element */
		else
		{
			childTag = 0;
			parsePtr = StartOfElement(parsePtr, eof);
			parsePtr++;
		}
	}
	while(childTag);
	  
	return parsePtr;
}

void extractAttrValues(UIAppInstanceData * instanceData, char *attrType, char *attrValue)
{
	U32 index=0;

	if(OS_StriCmp(attrType, MEDIA_SIZE) == 0)
	{

		/*	while(index<BTL_BPPSND_NUM_OF_MEDIA_SIZES)
			{	

				
				if((!OS_StriCmp(attrValue, mediaSizeArr[index])))
				{	
					instanceData->btAppBppInstanceData.supportedMediaSizes += j; 
					instanceData->btAppBppInstanceData.numOfDiffMediaSizes++;
					break;
				}else 
				{
					j = j*2;
					index++;
				}
			}*/
				
		index = instanceData->btAppBppInstanceData.numMediaSizesSupported;
		if(index<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_MEDIA_SIZES)
		{
			OS_StrnCpy((char*)instanceData->btAppBppInstanceData.mediaSizesSupported[index],
						(char*)attrValue,
						BTL_BPPSND_MAX_MEDIA_SIZE_LEN);
			instanceData->btAppBppInstanceData.numMediaSizesSupported++;
		}		
	}
	else if(OS_StriCmp(attrType, IMAGE_FORMAT) == 0)
	{
		index = instanceData->btAppBppInstanceData.numImageFormatsSupported;
		if(index<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_IMAGE_FORMATS)
		{
			OS_StrnCpy((char*)instanceData->btAppBppInstanceData.imageFormatsSupported[index],
						(char*)attrValue,
						BTL_BPPSND_MAX_IMAGE_FORMAT_LEN);
			instanceData->btAppBppInstanceData.numImageFormatsSupported++;
		}
	}	
	else if(OS_StriCmp(attrType, BPP_MEDIA_TYPE) == 0)
	{
			/*while(index<BTL_BPPSND_NUM_OF_MEDIA_TYPES)
			{	

				
				if(!OS_StriCmp(attrValue, mediaTypeArr[index]))
				{	
					instanceData->btAppBppInstanceData.supportedMediaTypes += j; 
					instanceData->btAppBppInstanceData.numOfDiffMediaTypes++;
					break;
				}else 
				{
					j = j*2;
					index++;
				}
			}*/
		index = instanceData->btAppBppInstanceData.numMediaTypesSupported;
		if(index<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_MEDIA_TYPES)
		{
			OS_StrnCpy((char*)instanceData->btAppBppInstanceData.mediaTypesSupported[index],
						(char*)attrValue,
						BTL_BPPSND_MAX_MEDIA_TYPE_LEN);
			instanceData->btAppBppInstanceData.numMediaTypesSupported++;
		}		
	}
	else if(OS_StriCmp(attrType, DOCUMENT_FORMAT) == 0)
	{
			/*while(index<BTL_BPPSND_NUM_OF_DOCUMENT_FORMATS)
			{	

				
				if(!OS_StriCmp(attrValue, documentFormatArr[index]))
				{	
					instanceData->btAppBppInstanceData.documentFormat+= j; 
					instanceData->btAppBppInstanceData.numOfDiffDocumentFormats++;
					break;
				}else 
				{
					j = j*2;
					index++;
				}
			}
				
			j=1;
			index=0;*/
		index = instanceData->btAppBppInstanceData.numDocumentFormatsSupported;
		if(index<BTL_BPPSND_MAX_NUM_OF_SUPPORTED_DOCUMENT_FORMATS)
		{
			OS_StrnCpy((char*)instanceData->btAppBppInstanceData.documentFormatsSupported[index],
						(char*)attrValue,
						BTL_BPPSND_MAX_DOCUMENT_FORMAT_LEN);
			instanceData->btAppBppInstanceData.numDocumentFormatsSupported++;
		}	
			
	}
	else if(OS_StriCmp(attrType, SIDES) == 0)
	{
		if( !OS_StriCmp( ( char * )attrValue, oneSided ) )
			instanceData->btAppBppInstanceData.sides += ONE_SIDED_MASK;
		else if( !OS_StriCmp( ( char * )attrValue, twoSidedLong ) )
			instanceData->btAppBppInstanceData.sides += TWO_SIDED_LONG_MASK;
		else if( !OS_StriCmp( ( char * )attrValue, twoSidedShort ) )
			instanceData->btAppBppInstanceData.sides += TWO_SIDED_SHORT_MASK;
	}
	else if(OS_StriCmp(attrType, ORIENTATION) == 0)
	{
		if( !OS_StriCmp( ( char * )attrValue, portrait ) )
			instanceData->btAppBppInstanceData.orientation += PORTRAIT_MASK;
		else if( !OS_StriCmp( ( char * )attrValue, landscape ) )
			instanceData->btAppBppInstanceData.orientation += LANDSCAPE_MASK;
		else if( !OS_StriCmp( ( char * )attrValue, reverseLandScape ) )
			instanceData->btAppBppInstanceData.orientation += REVERSE_LANDSCAPE_MASK;
		else if( !OS_StriCmp( ( char * )attrValue, reversePortrait ) )
			instanceData->btAppBppInstanceData.orientation += REVERSE_PORTRAIT_MASK;
	}
	else if(OS_StriCmp(attrType, PRINT_QUALITY) == 0)
	{
		if( !OS_StriCmp( ( char * )attrValue, draft ) )
			instanceData->btAppBppInstanceData.printfQuality += DRAFT_MASK;
		else if( !OS_StriCmp( ( char * )attrValue, normal ) )
			instanceData->btAppBppInstanceData.printfQuality += NORMAL_MASK;
		else if( !OS_StriCmp( ( char * )attrValue, high ) )
			instanceData->btAppBppInstanceData.printfQuality += HIGH_MASK;
	}						
}

char *StartOfElement(char *dataPtr, char *eof)
{
	while ((*dataPtr) && (*dataPtr != '<'))
	{
		if (dataPtr + 1 < eof)
			dataPtr++;
		else
			return NULL;
	}
	return dataPtr;
}


int counttillStartOfElement(char * dataPtr, char * eof)
{
	int count = 0;
	while ((*dataPtr) && (*dataPtr != '<'))
	{
		if (dataPtr + 1 < eof)
		{
			dataPtr++;
		}
		else
			return 0;
		count++;
	}
	return count;
}

int counttillEndOfElement(char * dataPtr, char * eof)
{
	int count = 0;
	while ((*dataPtr) && (*dataPtr != '>'))
	{
		if (dataPtr + 1 < eof)
		{
			dataPtr++;
		}
		else
			return 0;
		count++;
	}
	return count;
}
/*********************************************************************************/
void bppSetPrinterState(UIAppInstanceData * instanceData, U8 * printerState)
{

   if(!OS_StriCmp((char *)printerState, idle_state))
   {
	
	instanceData->btAppBppInstanceData.printerState = BPP_PRINTER_STATE_IDLE;
	
   }
   else if(!OS_StriCmp((char *)printerState, processing_state))
   {
   	
      instanceData->btAppBppInstanceData.printerState = BPP_PRINTER_STATE_PROCESSING;
   }
   else if(!OS_StriCmp((char *)printerState, stopped_state))
   {
   	
      instanceData->btAppBppInstanceData.printerState = BPP_PRINTER_STATE_STOPPED;
   }
   else
   {
   	
   }
}

void bppSetPrinterStateReason(UIAppInstanceData * instanceData, U8 * printerStateReason)
{
   if(!OS_StriCmp((char *)printerStateReason, none))
   {
      instanceData->btAppBppInstanceData.printerStateReason = NONE;
   }
   else if(!OS_StriCmp((char *)printerStateReason, attention_required))
   {
      instanceData->btAppBppInstanceData.printerStateReason = ATTENTION_REQUIRED;
   }
   else if(!OS_StriCmp((char *)printerStateReason, media_jam))
   {
      instanceData->btAppBppInstanceData.printerStateReason = MEDIA_JAM;
   }
   else if(!OS_StriCmp((char *)printerStateReason, paused))
   {
      instanceData->btAppBppInstanceData.printerStateReason = PAUSED;
   }
   else if(!OS_StriCmp((char *)printerStateReason, door_open))
   {
      instanceData->btAppBppInstanceData.printerStateReason = DOOR_OPEN;
   }
   else if(!OS_StriCmp((char *)printerStateReason, media_low))
   {
      instanceData->btAppBppInstanceData.printerStateReason = MEDIA_LOW;
   }
   else if(!OS_StriCmp((char *)printerStateReason, media_empty))
   {
      instanceData->btAppBppInstanceData.printerStateReason = MEDIA_EMPTY;
   }
   else if(!OS_StriCmp((char *)printerStateReason, output_area_almost_full))
   {
      instanceData->btAppBppInstanceData.printerStateReason = OUTPUT_AREA_ALMOST_FULL;
   }
   else if(!OS_StriCmp((char *)printerStateReason, output_area_full))
   {
      instanceData->btAppBppInstanceData.printerStateReason = OUTPUT_AREA_FULL;
   }
   else if(!OS_StriCmp((char *)printerStateReason, maker_supply_low))
   {
      instanceData->btAppBppInstanceData.printerStateReason = MAKER_SUPPLY_LOW;
   }
   else if(!OS_StriCmp((char *)printerStateReason, maker_supply_empty))
   {
      instanceData->btAppBppInstanceData.printerStateReason = MAKER_SUPPLY_EMPTY;
   }
   else if(!OS_StriCmp((char *)printerStateReason, maker_failure))
   {
      instanceData->btAppBppInstanceData.printerStateReason = MAKER_FAILURE;
   }
   else
   {
   }
}


/********************************************************************************** UTILITY FUNCTIONS - START ********************************************************************************/
char * bpp_skipWhiteSpaces(char * dataPtr, char * stringMax)
{
	while (bpp_IsSpace(*dataPtr))	
	{
		if (dataPtr+1 < stringMax)
			dataPtr++;
		else
			return NULL;
	}
	return dataPtr;
}

int bpp_IsSpace(char c)
{
	switch(c)
	{
		case '\t':
		case '\n':
		case '\f':
		case '\r':
		case ' ' :
			return 1;
		default:
			return 0;
	}
}




#endif /*BTL_CONFIG_BPPSND = BTL_CONFIG_ENABLED*/

