#ifndef __BTL_BPPSND_SOAP_H
#define __BTL_BPPSND_SOAP_H
/***************************************************************************\
                                                                          
    File               		:     btl_bppsnd_soap.h

    Description                                                           
    -----------                                                           
    	Soap builder for Basic Printing Profile.
                                                                                                                                       
    Created by			:          Naveen Kumar K          
    Date created		:          1 June 2006
    Environment		:	    Embedded
    $Revision: 2 $                                      

USE_ARRYFOR_STRINGS    
\***************************************************************************/
/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
 #include "btl_bppsnd.h"
 

 /********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/
/******* Defines ************/
#define	BTL_BPPSND_MAX_PRINTER_NAME_LEN_COPY						20
#define	BTL_BPPSND_MAX_PRINTER_LOCATION_LEN_COPY					20
#define   BTL_BPPSND_MAX_JOB_NAME_LEN_COPY  							20
#define	BTL_BPPSND_MAX_JOB_ORIGINATING_USER_NAME_LEN_COPY		20
#define   BTL_BPPSND_MAX_JOB_STATE_LEN  								20


#define   BTL_BPPSND_NUM_OF_MEDIA_TYPES								20
#define   BTL_BPPSND_NUM_OF_MEDIA_SIZES								27
#define   BTL_BPPSND_NUM_OF_DOCUMENT_FORMATS						17


/* Defines used both for GetPrinterAttributes request and CreateJob request	*/
#define CONTENT_LENGTH										"CONTENT-LENGTH: "
#define CONTENT_TYPE										"CONTENT-TYPE: text/xml; charset=\"utf-8\"\r\n"
#define HTTP_NEWLINE										"\r\n"
#define ENVELOPE_START										"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">\n"

/* Defines used for GetPrinterAttributes request							*/
#define SOAP_ACTION_GET_PRINTER_ATTR						"SOAPACTION:\"urn:schemas-bluetooth-org:service:Printer:1#GetPrinterAttributes\"\r\n\r\n"
#define BODY_START_GET_PRINTER_ATTR						"<s:Body>\n<u:GetPrinterAttributes xmlns:u=\"urn:schemas-bluetooth-org:service:Printer:1\">\n"
#define ENVELOPE_BODY_END_GET_PRINTER_ATTR				"</u:GetPrinterAttributes>\n</s:Body>\n</s:Envelope>"
#define REQ_PRINTER_ATTRIBUTES_START						"<RequestedPrinterAttributes>\n"
#define REQ_PRINTER_ATTRIBUTES_END						"</RequestedPrinterAttributes>\n"
#define PRINTER_ATTRIBUTE_START							"<PrinterAttribute>"
#define PRINTER_ATTRIBUTE_END								"</PrinterAttribute>\n"
#define PRINTER_NAME										"PrinterName"
#define PRINTER_LOCATION									"PrinterLocation"
#define PRINTER_STATE										"PrinterState"
#define PRINTER_STATE_REASONS								"PrinterStateReasons"
#define PRINTER_DOCUMENTFORMATS_SUPPORTED				"DocumentFormatsSupported"
#define COLOR_SUPPORTED									"ColorSupported"
#define MAX_COPIES_SUPPORTED								"MaxCopiesSupported"	
#define SIDES_SUPPORTED									"SidesSupported"
#define NUMBER_UP_SUPPORTED								"NumberUpSupported"
#define ORIENTATIONS_SUPPORTED							"OrientationsSupported"
#define MEDIA_SIZES_SUPPORTED								"MediaSizesSupported"
#define MEDIA_TYPES_SUPPORTED								"MediaTypesSupported"
#define MEDIA_LOADED										"MediaLoaded"
#define PRINT_QUALITY_SUPPORTED							"PrintQualitySupported"
#define QUEUE_JOB_COUNT									"QueuedJobCount"
#define IMAGE_FORMATS_SUPPORTED							"ImageFormatsSupported"
#define BASIC_TEXT_PAGE_WIDTH								"BasicTextPageWidth"
#define BASIC_TEXT_PAGE_HEIGHT							"BasicTextPageHeight"
#define PRINTER_GENERAL_CURRENT_OPERATOR				"PrinterGeneralCurrentOperator"
#define OPERATION_STATUS									"OperationStatus"
#define MEDIA_SIZE											"MediaSize"
#define BPP_MEDIA_TYPE										"MediaType"
#define DOCUMENT_FORMAT									"DocumentFormat"
#define SIDES												"Sides"
#define ORIENTATION											"Orientation"
#define PRINT_QUALITY										"PrintQuality"

/*#define MEDIA_LOADED										"MediaLoaded"*/
#define LOADED_MEDIUM_DETAILS								"LoadedMediumDetails"
#define LOADED_MEDIUM_SIZE								"LoadedMediumSize"
#define LOADED_MEDIUM_TYPE								"LoadedMediumType"

#define IMAGE_FORMAT										"ImageFormat"


/* Defines used for GetPrinterAttributes request							*/
#define SOAP_ACTION_GET_JOB_ATTR						"SOAPACTION:\"urn:schemas-bluetooth-org:service:Printer:1#GetJobAttributes\"\r\n\r\n"
#define BODY_START_GET_JOB_ATTR						"<s:Body>\n<u:GetJobAttributes xmlns:u=\"urn:schemas-bluetooth-org:service:Printer:1\">\n"
#define ENVELOPE_BODY_END_GET_JOB_ATTR				"</u:GetJobAttributes>\n</s:Body>\n</s:Envelope>"
#define REQ_JOB_ATTRIBUTES_START						"<RequestedJobAttributes>\n"
#define REQ_JOB_ATTRIBUTES_END						"</RequestedJobAttributes>\n"
#define JOB_ATTRIBUTE_START							"<JobAttribute>"
#define JOB_ATTRIBUTE_END								"</JobAttribute>\n"
#define JOB_ID_START									"<JobId>"
#define JOB_ID_END										"</JobId>\n"



/* Defines used for CreateJob request							*/
#define SOAP_ACTION_CREATE_JOB							"SOAPACTION:\"urn:schemas-bluetooth-org:service:Printer:1#CreateJob\"\r\n\r\n"
#define BODY_START_CREATE_JOB								"<s:Body>\n<u:CreateJob xmlns:u=\"urn:schemas-bluetooth-org:service:Printer:1\">\n"
#define ENVELOPE_BODY_END_CREATE_JOB						"</u:CreateJob>\n</s:Body>\n</s:Envelope>"	
#define JOB_NAME_START										"<JobName>"
#define JOB_NAME_END										"</JobName>\n"
#define JOB_ORIGINATING_USER_NAME_START					"<JobOriginatingUserName>"
#define JOB_ORIGINATING_USER_NAME_END					"</JobOriginatingUserName>\n"
#define DOCUMENT_FORMAT_START							"<DocumentFormat>"
#define DOCUMENT_FORMAT_END								"</DocumentFormat>\n"
#define COPIES_START										"<Copies>"
#define COPIES_END											"</Copies>\n"
#define SIDES_START											"<Sides>"
#define SIDES_END											"</Sides>\n"
#define NUMBER_UP_START									"<NumberUp>" 	
#define NUMBER_UP_END										"</NumberUp>\n"
#define ORIENTATION_REQUESTED_START						"<OrientationRequested>"
#define ORIENTATION_REQUESTED_END						"</OrientationRequested>\n"
#define MEDIA_SIZE_START									"<MediaSize>"	
#define MEDIA_SIZE_END										"</MediaSize>\n"
#define MEDIA_TYPE_START									"<MediaType>"
#define MEDIA_TYPE_END										"</MediaType>\n"	
#define PRINT_QUALITY_START								"<PrintQuality>"
#define PRINT_QUALITY_END									"</PrintQuality>\n"
#define CANCEL_ON_LOST_LINK_START							"<CancelOnLostLink>"
#define CANCEL_ON_LOST_LINK_END							"</CancelOnLostLink>\n"
#define CANCEL_ON_LOST_LINK_TRUE							"true"
#define CANCEL_ON_LOST_LINK_FALSE							"false"
#define SIDES_ONE_SIDED									"one-sided"
#define SIDES_TWO_SIDED_LONG_EDGE						"two-sided-long-edge"	
#define SIDES_TWO_SIDED_SHORT_EDGE						"two-sided-short-edge"
#define ORIENTATION_REQUESTED_PORTRAIT					"portrait"
#define ORIENTATION_REQUESTED_LANDSCAPE					"landscape"
#define ORIENTATION_REQUESTED_REVERSE_LANDSCAPE		"reverse-landscape"
#define ORIENTATION_REQUESTED_REVERSE_PORTRAIT			"reverse-portrait"
#define PRINT_QUALITY_DRAFT								"draft"	
#define PRINT_QUALITY_NORMAL								"normal"	
#define PRINT_QUALITY_HIGH									"high"

/* Cancle Job request*/
#define SOAP_ACTION_CANCEL_JOB							"SOAPACTION:\"urn:schemas-bluetooth-org:service:Printer:1#CancelJob\"\r\n\r\n"
#define BODY_START_CANCEL_JOB								"<s:Body>\n<u:CancelJob xmlns:u=\"urn:schemas-bluetooth-org:service:Printer:1\">\n"
#define ENVELOPE_BODY_END_CANCEL_JOB						"</u:CancelJob>\n</s:Body>\n</s:Envelope>"

/* Cancle Job request*/
#define SOAP_ACTION_GET_EVENT								"SOAPACTION:\"urn:schemas-bluetooth-org:service:Printer:1#GetEvent\"\r\n\r\n"
#define BODY_START_GET_EVENT								"<s:Body>\n<u:GetEvent xmlns:u=\"urn:schemas-bluetooth-org:service:Printer:1\">\n"
#define ENVELOPE_BODY_END_GET_EVENT						"</u:GetEvent>\n</s:Body>\n</s:Envelope>"

/*Get Job Attributes Response*/
#define JOB_ID												"JobId"
#define JOB_STATE											"JobState"
#define JOB_NAME 											"JobName"
#define JOB_ORIGINATING_USER_NAME 						"JobOriginatingUserName"
#define JOB_MEDIA_SHEET_COMPLETED						"JobMediaSheetsCompleted"
#define NUM_INTERVENING_JOBS								"NumberOfInterveningJobs"

/* Get Margins Response */
#define MARGINS 												"Margins"

/* Paper size */
#define PAPER_SIZE_4X6_WIDTH                 						"101.6mm"
#define PAPER_SIZE_4X6_HEIGHT                					"152.4mm"
#define PAPER_SIZE_8X10_WIDTH                					"203.2mm"
#define PAPER_SIZE_8X10_HEIGHT               					"254.0mm"
#define PAPER_SIZE_LETTER_WIDTH              					"216.0mm"
#define PAPER_SIZE_LETTER_HEIGHT             					"279.4mm"
#define PAPER_SIZE_11X17_WIDTH               					"279.4mm"
#define PAPER_SIZE_11X17_HEIGHT              					"431.8mm"
#define PAPER_SIZE_A4_WIDTH                  						"210.0mm"
#define PAPER_SIZE_A4_HEIGHT                	 					"297.0mm"
#define PAPER_SIZE_L_WIDTH                   						"89.0mm"
#define PAPER_SIZE_L_HEIGHT                  						"127.0mm"
#define PAPER_SIZE_2L_WIDTH                  						"127.0mm"
#define PAPER_SIZE_2L_HEIGHT                 						"178.0mm"
#define PAPER_SIZE_POSTCARD_WIDTH            					"100.0mm"
#define PAPER_SIZE_POSTCARD_HEIGHT           					"148.0mm"
#define PAPER_SIZE_CARD_WIDTH                					"54.0mm"
#define PAPER_SIZE_CARD_HEIGHT               					"85.6mm"

#define SIZE_4X6_WIDTH_EXCEPT_MARGIN         				"91.6mm"
#define SIZE_4X6_HEIGHT_EXCEPT_MARGIN        				"142.4mm"
#define SIZE_8X10_WIDTH_EXCEPT_MARGIN        				"193.2mm"
#define SIZE_8X10_HEIGHT_EXCEPT_MARGIN       				"244.0mm"
#define SIZE_LETTER_WIDTH_EXCEPT_MARGIN      				"206.0mm"
#define SIZE_LETTER_HEIGHT_EXCEPT_MARGIN     				"269.4mm"
#define SIZE_11X17_WIDTH_EXCEPT_MARGIN       				"269.4mm"
#define SIZE_11X17_HEIGHT_EXCEPT_MARGIN      				"421.8mm"
#define SIZE_A4_WIDTH_EXCEPT_MARGIN          				"200.0mm"
#define SIZE_A4_HEIGHT_EXCEPT_MARGIN         				"287.0mm"
#define SIZE_L_WIDTH_EXCEPT_MARGIN           					"79.0mm"
#define SIZE_L_HEIGHT_EXCEPT_MARGIN          					"117.0mm"
#define SIZE_2L_WIDTH_EXCEPT_MARGIN          				"117.0mm"
#define SIZE_2L_HEIGHT_EXCEPT_MARGIN         				"168.0mm"
#define SIZE_POSTCARD_WIDTH_EXCEPT_MARGIN    			"90.0mm"
#define SIZE_POSTCARD_HEIGHT_EXCEPT_MARGIN   			"138.0mm"
#define SIZE_CARD_WIDTH_EXCEPT_MARGIN        				"44.0mm"
#define SIZE_CARD_HEIGHT_EXCEPT_MARGIN       				"75.6mm"


/* Image size */
#define IMAGE_SIZE_LARGE                     						"99%"
#define IMAGE_SIZE_MEDIUM                    						"90%"
#define IMAGE_SIZE_SMALL                     						"80%"
/*
		<DocumentFormat>application/vnd.hp-pcl:3c</DocumentFormat>
		static char vMessage[]											= "text/x-vmessage:1.1";//new added
*/
/* Document format */
#define XHTML95_MASK						   	   			(1) 
#define TEXT_MASK								      			(2)
#define VCARD_MASK								   			(4)
#define VCAL_MASK								      			(8)
#define ICAL_MASK								      			(16)
#define POSTS2_MASK										(32)
#define POSTS3_MASK							    		  	(64)
#define PCL5E_MASK							      				(128)
#define PCL6_MASK							         			(256)
#define PDF_MASK							        		 	(512)
#define JPEG_MASK							        		 	(1024)
#define GIF_MASK							      			   	(2048)	
#define XHTML10_MASK						     			    	(4096) 
#define XHTML_PRINT_INLINE_DATA_MASK						(8192)
#define PCLC3_MASK											(16384)
#define VMESSAGE_MASK										(32768)
/* Paper size mask - Media Sizes Supported*/
#define PAPER_SIZE_4X6_MASK                  						(1)   /* 101.6 mm x 152.4 mm */
#define PAPER_SIZE_8X10_MASK                						(2)   /* 203.2 mm x 254.0 mm */
#define PAPER_SIZE_LETTER_MASK               					(4)   /* 216.0 mm x 279.4 mm */
#define PAPER_SIZE_11X17_MASK                					(8)   /* 279.4 mm x 431.8 mm */
#define PAPER_SIZE_A4_MASK                   						(16)  /* 210.0 mm x 297.0 mm */

#define PAPER_SIZE_L_MASK                    						(32)  /*  89.0 mm x 127.0 mm */
#define PAPER_SIZE_2L_MASK                   						(64)  /* 127.0 mm x 178.0 mm */
#define PAPER_SIZE_POSTCARD_MASK             					(128) /* 100.0 mm x 148.0 mm */
#define PAPER_SIZE_CARD_MASK                 					(256) /*  54.0 mm x  85.6 mm */ 
#define PAPER_SIZE_A3_MASK                   						(512)  /* 297 mm x 420 mm */

#define PAPER_SIZE_EXECUTIVE_MASK       						(1024)  
#define PAPER_SIZE_LEGAL_MASK               						(2048)  
#define PAPER_SIZE_A5_MASK                   						(4096)  
#define PAPER_SIZE_A6_MASK                   						(8192)  
#define PAPER_SIZE_B5_MASK                   						(16384)  
#define PAPER_SIZE_182X257_MASK           						(32768)  
#define PAPER_SIZE_CHOU3_MASK                   					(65536)  
#define PAPER_SIZE_CHOU4_MASK                   					(131072)  
#define PAPER_SIZE_3X5_MASK                   						(262144)  
#define PAPER_SIZE_5X7_MASK                   						(524288)  
#define PAPER_SIZE_5X8_MASK                   						(1048576) 
#define PAPER_SIZE_NUMBER10_MASK                   				(2097152)  
#define PAPER_SIZE_DL_MASK                   						(4194304)  
#define PAPER_SIZE_C6_MASK                   						(8388608) 
#define PAPER_SIZE_A2_MASK                   						(16777216)  


/**************************************************************************************/

/* Paper type mask - Media Types Supported*/
#define PAPER_TYPE_STATIONERY_MASK                  				(1)   
#define PAPER_TYPE_PHOTOGRAPHIC_MASK                			(2)   
#define PAPER_TYPE_CARDSTOCK_MASK               				(4)   
#define PAPER_TYPE_STATIONERYCOATED_MASK 	      			(8)
#define PAPER_TYPE_STATIONERYINKJET_MASK 	      			(16)
#define PAPER_TYPE_TRANSPARENCY_MASK 					(32)
#define PAPER_TYPE_ENVELOPE_MASK 				    		  	(64)
#define PAPER_TYPE_ENVELOPEPLAIN_MASK 	      				(128)
#define PAPER_TYPE_ENVELOPEWINDOW_MASK 	         			(256)
#define PAPER_TYPE_TABSTOCK_MASK 			        		 	(512)
#define PAPER_TYPE_PRECUTTABS_MASK 		        		 	(1024)
#define PAPER_TYPE_FULLCUTTABS_MASK 		      			   	(2048)	
#define PAPER_TYPE_LABELS_MASK 			     			    	(4096) 


/**************************************************************************************/
/* Sides */
#define ONE_SIDED_MASK							   			(1)
#define TWO_SIDED_LONG_MASK								(2)
#define TWO_SIDED_SHORT_MASK					   			(4)

/* Print quality */
#define DRAFT_MASK								   			(1)
#define NORMAL_MASK										(2)
#define HIGH_MASK								      			(4)

/* Orientation */
#define PORTRAIT_MASK							   			(1)
#define LANDSCAPE_MASK							   			(2)
#define REVERSE_LANDSCAPE_MASK							(4)
#define REVERSE_PORTRAIT_MASK					   			(8)

#define MAX_NUM_OF_DOC_FORMATS							(5) 

/* NumberUp */
#define PRINT_LAYOUT_1UP_MASK                					(1)
#define PRINT_LAYOUT_2UP_MASK                					(2)
#define PRINT_LAYOUT_3UP_MASK                					(4)
#define PRINT_LAYOUT_4UP_MASK                					(8)
#define PRINT_LAYOUT_6UP_MASK                					(16) 
#define PRINT_LAYOUT_9UP_MASK                					(32)
#define PRINT_LAYOUT_12UP_MASK               					(64)
#define PRINT_LAYOUT_15UP_MASK               					(128)
#define PRINT_LAYOUT_16UP_MASK               					(256)

#define CAPITAL(x)											((x >= 'a') && (x <= 'z') ? ((x) & 0xDF) : (x))


static char printerName[]                    							= "PrinterName";
static char printerLocation[]                							= "PrinterLocation";
static char printerState[]                   							= "PrinterState";
static char printerStateReasons[]            						= "PrinterStateReasons";
static char printerGeneralCurrentOperator[]						= "PrinterGeneralCurrentOperator";
static char colorSupported[]                 							= "ColorSupported";
static char maxCopiesSupported[]             						= "MaxCopiesSupported";
static char numberUpSupported[]              						= "NumberUpSupported";
static char queuedJobCount[]                 							= "QueuedJobCount";
static char operationStatus[]                							= "OperationStatus";
static char basicTextPageWidth[]             						= "BasicTextPageWidth";
static char basicTextPageHeight[]            						= "BasicTextPageHeight";
static char documentFormatsSupported[]						= "DocumentFormatsSupported";
static char imageFormatsSupported[]          						= "ImageFormatsSupported";
static char sidesSupported[]                 							= "SidesSupported";
static char orientationsSupported[]          						= "OrientationsSupported";
static char mediaSizesSupported[]            						= "MediaSizesSupported";
static char mediaTypesSupported[]            						= "MediaTypesSupported";
static char mediaLoaded[]                    							= "MediaLoaded";
static char loadedMediumDetails[]            						= "LoadedMediumDetails";
static char loadedMediumSize[]               						= "LoadedMediumSize";
static char loadedMediumType[]               						= "LoadedMediumType";
static char printQualitySupported[]          						= "PrintQualitySupported";
static char jobId[]                          								= "JobId";
static char oneSided[]                       							= "one-sided";
static char twoSidedLong[]                   							= "two-sided-long-edge";
static char twoSidedShort[]                  							= "two-sided-short-edge";
static char draft[]                          								= "draft";
static char normal[]                         								= "normal";
static char high[]                           								= "high";
static char portrait[]                       								= "portrait";
static char landscape[]                      							= "landscape";
static char reverseLandScape[]               						= "reverse-landScape";
static char reversePortrait[]                							= "reverse-portrait";

/* Document format */
/*
static char Xhtml95[]                        								= "application/vnd.pwg-xhtml-print+xml:0.95";
static char Xhtml10[]                        								= "application/vnd.pwg-xhtml-print+xml:1.0";
static char xhtmlPrintInlineData[]         					  		= "application/vnd.pwg-multiplexed";
static char text[]                           								= "text/plain";
static char vCard[]                          								= "text/x-vcard:2.1";
static char vCalendar[]                      							= "text/x-vcalendar:1.0";
static char iCalendar[]                      								= "text/calendar:2.0";
static char postS2[]                         								= "application/PostScript:2";
static char postS3[]                         								= "application/PostScript:3";
static char pcl5e[]                          								= "application/vnd.hp-PCL:5E";
static char pcl6[]                           								= "application/vnd.hp-PCL:6";
static char pdf[]                            								= "application/PDF";
static char jpeg[]                           								= "image/jpeg";
static char gif89a[]                         								= "image/gif:89A";
static char pcl3c[]                         								= "application/vnd.hp-PCL:3c";// new added
static char vMessage[]											= "text/x-vmessage:1.1";//new added
*/

static char *documentFormatArr[] = {"application/vnd.pwg-xhtml-print+xml:0.95","application/vnd.pwg-xhtml-print+xml:1.0","application/vnd.pwg-multiplexed",
								"text/plain","text/x-vcard:2.1","text/x-vcalendar:1.0","text/calendar:2.0","application/PostScript:2"
								,"application/PostScript:3","application/vnd.hp-PCL:5E","application/vnd.hp-PCL:6","application/PDF"
								,"image/jpeg","image/gif:89A","application/vnd.hp-PCL:3c","text/x-vmessage:1.1","multipart/related"};	/*17*/

/*
static char jpegFileName[]                   							= "jpeg_file.jpg";
static char vCardFileName[]                  							= "v_card_file.vcf";
static char pdfFileName[]                    							= "pdf_file.pdf";
static char textFileName[]                   							= "text_file.txt";
static char gifFileName[]                    							= "gif_file.gif";
static char vCalFileName[]                   							= "v_cal_file.vcs";
static char iCalFileName[]                   							= "i_cal_file.vcs";  
static char pcl5eFileName[]                  							= "pcl5eFile.pcl";
static char pcl6FileName[]                   							= "pcl6File.pcl";
static char ps2FileName[]                    							= "post_script2file.ps";
static char ps3FileName[]                    							= "post_script3file.ps";
static char xhtmlVer0_95FileName[]          			 			= "xhtml_file_ver0_95.htm";
static char xhtmlVer1_0_FileName[]          			 			= "xhtml_file_ver1_0.htm";
*/


/* Printer state */
static char idle_state[]                     								= "idle";
static char processing_state[]               							= "processing";
static char stopped_state[]                  							= "stopped";

/* Printer state reason */
static char none[]                           								= "none";
static char attention_required[]             							= "attention-required";
static char media_jam[]                      							= "media-jam";
static char paused[]                         								= "paused";
static char door_open[]                      							= "door-open";
static char media_low[]                      							= "media-low";
static char media_empty[]                    							= "media-empty";
static char output_area_almost_full[]        						= "output-area-almost-full";
static char output_area_full[]               							= "output-area-full";
static char maker_supply_low[]               						= "maker-supply-low";
static char maker_supply_empty[]             						= "maker-supply-empty";
static char maker_failure[]                  							= "maker-failure";

/*
static char ps4x6[]                          								= "na_index-4x6_4x6in";        	
static char ps4x6_1[]                        							= "na_index_4x6in";            		
static char ps8x10[]                         								= "na_govt-letter_8x10in";
static char psLetter[]                       								= "na_letter_8.5x11in";
static char ps11x17[]                        							= "na_ledger_11x17in";
static char psA4[]                           								= "iso_a4_210x297mm";
static char psA3[]                           								= "iso_a3_297x420mm";
static char psL[]                            								= "custom_l-size_89x127mm";
static char ps2L[]                           								= "custom_2l-size_127x178mm";
static char psHagakiPostcard[]               							= "jpn_hagaki_100x148mm";
static char psCardSize[]                     							= "custom_card-size_54x85.6mm";

static char psExecutive[]                     							= "na_executive_7.25x10.5in";//new added
static char psA5[] 			                    							= "iso_a5_148x210mm";//new added
static char psLegal[]           		          							= "na_legal_8.5x14in";//new added
static char psA6[]       		              							= "iso_a6_105x148mm";//new added
static char ps182x257[]                     							= "jis_b5_182x257mm";//new added
static char psOufuku[]                	     							= "jpn_oufuku_148x200mm";//new added
static char psChou3[]          	           							= "jpn_chou3_120x235mm";//new added
static char psChou4[]   	                  							= "jpn_chou4_90x205mm";//new added
static char ps3x5[]	                     							= "na_index-3x5_3x5in";//new added
static char ps5x7[]	                     							= "na_5x7_5x7in";//new added
static char ps5x8[]	                     							= "na_index-5x8_5x8in";//new added
static char psNaNumber[]                     							= "na_number-10_4.125x9.5in";//new added
static char psDl110x220[]                     							= "iso_dl_110x220mm";//new added
static char psC6114x162[]                     							= "iso_c6_114x162mm";//new added
static char psA2[]        			             							= "na_a2_4.375x5.75in";//new added
*/

static char *mediaSizeArr[] = {"na_index-4x6_4x6in", "na_index_4x6in","na_govt-letter_8x10in","na_letter_8.5x11in","na_ledger_11x17in"
							,"iso_a4_210x297mm","iso_a3_297x420mm","custom_l-size_89x127mm","custom_2l-size_127x178mm"
							,"jpn_hagaki_100x148mm","custom_card-size_54x85.6mm","na_executive_7.25x10.5in","iso_a5_148x210mm"
							,"na_legal_8.5x14in","iso_a6_105x148mm","jis_b5_182x257mm","jpn_oufuku_148x200mm","jpn_chou3_120x235mm"
							,"jpn_chou4_90x205mm","na_index-3x5_3x5in","na_5x7_5x7in","na_index-5x8_5x8in","na_number-10_4.125x9.5in"
							,"iso_dl_110x220mm","iso_c6_114x162mm","na_a2_4.375x5.75in","custom_epson-photo-2l_5x7in"};	/*27*/






static char *mediaTypeArr[] = {"stationery", "photographic","cardstock","stationery-coated","stationery-inkjet",
							"transparency","envelope","envelope-plain","envelope-window","tab-stock",
							"pre-cut-tabs","full-cut-tabs","labels","photographic-glossy","photographic-semi-gloss",
							"photographic-matte","custom_epson-photo-2l_5x7in","custom-media-type-epson-iron-on-transfer",
							"custom-media-type-epson-ultra-glossy-photo-paper","photographic-high-gloss"};	/* 20*/


 /********************************************************************************
 *
 * Types
 *
 *******************************************************************************/
/*-----------------------------------FROM Temprefdefs.h-----------------------------------------------*/

typedef unsigned  char    bool_t;

typedef U32       classOfDevice_t; 
typedef char      deviceName_t[32 + 1];      /* name of the peer device. add 1 to ensure that we can add a zero char*/ 


typedef BD_ADDR      deviceAddr_t;                       /*arry of U8 length=BD_ADDR_SIZE*/
typedef BD_ADDR      BD_ADDR_T;


/*------------------------------------------FROM UIFBTBPP.H---------------------------------------*/
enum
{
   BPP_PRINTER_STATE_IDLE,          /* new jobs can start processing without waiting */
   BPP_PRINTER_STATE_PROCESSING,    /* jobs are processing; new jobs may wait before processing */
   BPP_PRINTER_STATE_STOPPED       /* no jobs can be processed and intervention is required */
};

enum
{
   NONE,                            /* Indicates that there are no current state reasons */
   ATTENTION_REQUIRED,              /* Indicates reason other that those listed */
   MEDIA_JAM,                       /* The device has a media jam */
   PAUSED,                          /* Someone has paused the printer and the PrinterState is "stopped". In this state, a Printer may not produce printed output */
   DOOR_OPEN,                       /* One or more covers on the device are open */
   MEDIA_LOW,                       /* At least one input tray is low on media */
   MEDIA_EMPTY,                     /* At least one input tray is empty */
   OUTPUT_AREA_ALMOST_FULL,         /* One or more output area is almost full:e.g., tray, stacker, collator */
   OUTPUT_AREA_FULL,                /* One or more output area is full:e.g., tray, stacker, collator */
   MAKER_SUPPLY_LOW,                /* The device is low on at least one maker supply:e.g., toner, ink, ribbon */
   MAKER_SUPPLY_EMPTY,              /* The device is out of at least one maker supply:e.g., toner, ink, ribbon */
   MAKER_FAILURE,                   /* The device has at least one marking device witch has failed and requires service or replacement:e.g., pen */
   HANDSET_SIDE_ERR                 /* Handset-side error */
};

enum
{
   ACT_NONE,
   CONNECTING,
   CONNECTED,
   PRINTING_MULTI_OBJ,
   REQUEST_PRINTING,
   NUM_OF_COPIES_PRINTING
};
/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/
typedef struct
{
   U8           paperSize;
   U8           numOfCopies;
   U8           pagesPerSheet;
   U8           printQuility;
   U8           imageSize;
   U8           sides;
   U8           orientation;
   bool_t            withBorder;
} PrinterSettings;

typedef struct
{
	U8                               printerName[BTL_BPPSND_MAX_PRINTER_NAME_LEN_COPY+1];
	U8                               printerLocation[BTL_BPPSND_MAX_PRINTER_LOCATION_LEN_COPY+1];
	U8					jobName[BTL_BPPSND_MAX_JOB_NAME_LEN_COPY +1];
	U8 					jobOriginatingUserName[BTL_BPPSND_MAX_JOB_ORIGINATING_USER_NAME_LEN_COPY + 1];
	U32					jobMediaSheetsCompleted;
	U32					numOfInterveningJobs;
	U8					jobState[BTL_BPPSND_MAX_JOB_STATE_LEN+1];
	U32					queuedJobCount;
	/*U32   				supportedMediaSizes; */
	/*U32   				supportedMediaTypes;*/
	bool_t				colorSupported;
	BtlMediaLoadedDetails			mediaLoadedDetails[BTL_BPPSND_MAX_NUM_OF_MEDIA_LOADED];
	U32							numMediaLoaded;
	BtlBppsndImageFormat			imageFormatsSupported[BTL_BPPSND_MAX_NUM_OF_SUPPORTED_IMAGE_FORMATS];
	U32							numImageFormatsSupported;
	U32							basicTextPageWidth;
	U32							basicTextPageHeight;
	U8							printerGeneralCurrentOperator[
									BTL_BPPSND_MAX_PRINTER_GENERAL_CURRENT_OPERATOR+1];
	BtlMediaSize					mediaSizesSupported[BTL_BPPSND_MAX_NUM_OF_SUPPORTED_MEDIA_SIZES];
	U32							numMediaSizesSupported;
	BtlMediaType					mediaTypesSupported[BTL_BPPSND_MAX_NUM_OF_SUPPORTED_MEDIA_TYPES];
	U32							numMediaTypesSupported;
	BtlBppsndDocumentFormat		documentFormatsSupported[
									BTL_BPPSND_MAX_NUM_OF_SUPPORTED_DOCUMENT_FORMATS];
	U32							numDocumentFormatsSupported;
	
   	U8           			bppState;
	U8				* recvXmlPtr;	
	U8				* xhtmlPrintImageFormats;
	/*U8				* documentFormatsSupported;*/
	/*U8				* mediaTypesSupported;	*/
	void				   	* bppFileHandle;
	bool_t				cancelConnect;	                    /*unsigned char*/
	bool_t				transferModel;
	PrinterSettings   		settings;
	U32			   	jobId;
	U8				printerState;                             /*unsigned char*/
	U8           			printerStateReason;
	U8				sides;
	U8				printfQuality;
	U8				orientation;
	U8				numberUp;
	U32				operationStatus;/* was changed from U8 to U32 because the value range bigger the 255*/
	U8				maxCopies;
	/*U8				numOfDiffMediaTypes;*/
	/*U8				numOfDiffMediaSizes;*/
	/*U16			   	numOfDiffDocumentFormats; //unsigned short int*/
	/*U32			   	documentFormat;*/
	U32			   	recvXmlSize;                          /*unsigned long int*/
	U32			   	currentFilePosition;
	U32			   	currentFileSize;
	deviceAddr_t			deviceAddr;                    /*arry of U8 length=6*/
	deviceName_t			mimeMediaType;           /*arry of char length=32+1*/
	deviceName_t			documentName;
	U8           			bppreadDir;
   	U8           			transferPacketNum;
  	U8				addressLength;
	U8				authenticateLength;
	U8				miscLength;
	
} BTAppBppInstanceData_t;   



typedef struct
{
   
   BTAppBppInstanceData_t  btAppBppInstanceData;

}UIAppInstanceData;




/*****************************************************************************************************************
 *
 *  									FUNCTION PROTOTYPES
 *
 ******************************************************************************************************************/
/*****************************************************************************************************************
 *
 *  									Building Functions
 *
 ******************************************************************************************************************/
void 		buildsoapPrinterAttrRequest(BtlBppsndPrinterAttributesMask	attributesMask, U8 *soapPtr, U16 *length);
void 		buildsoapCreateJobRequest(const BtlBppsndCreateJobAttributesMask attributesMask ,
									U8 *soapPtr ,const BtlBppsndCreateJobRequestData	*attributesData ,
									U16 *length);
void buildsoapJobAttrRequest(BtlBppsndGetJobAttributesMask	attributesMask, U8 *soapPtr , U16 *length, BtlBppsndJobId	 jobId);
void buildsoapCancelJobRequest(U8 *soapPtr , BtlBppsndJobId	 jobId, U16 *totalLength);
void buildsoapGetEventRequest(U8 * soapPtr, BtlBppsndJobId jobId, U16 * totalLength);
/*****************************************************************************************************************
 *
 *  									Parsing Functions
 *
 ******************************************************************************************************************/	

int 			parseBppXmlRspData(UIAppInstanceData * instanceData, U8 *xmlPtr, U32 totLength);
static char *	parseAttrType(char *parsePtr, char *eof);
static char *	parseAttrValueNoChild(char *parsePtr, char *eof);
static void 	extractAttrValues(UIAppInstanceData * instanceData,char *attrType, char *attrValue);
static char *	parseAttrValueHaveChild(UIAppInstanceData * instanceData,char *parsePtr, char *eof);
static char *	parseNextractChildValues(UIAppInstanceData * instanceData,char *parsePtr, char *eof);
static char *	StartOfElement(char *data, char *eof);
static int 		counttillEndOfElement(char * dataPtr, char * eof);
static int 		counttillStartOfElement(char * dataPtr, char * eof);
void			 bppSetPrinterState(UIAppInstanceData * instanceData, U8 * printerState);
void 		bppSetPrinterStateReason(UIAppInstanceData * instanceData, U8 * printerStateReason);

/*****************************************************************************************************************
 *
 *  									Utility Functions - actually created in Btl_bppsnd_soapParser.c
 *
 ******************************************************************************************************************/	
int			 bpp_IsSpace(char c);
char * 		bpp_skipWhiteSpaces(char * dataPtr, char * stringMax);

#endif  /* __BTL_BPPSND_H*/

