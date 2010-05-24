

/* JPEG decoder module*/
#include "buijpegdec.h"

#ifndef BYD_USE_SIM
#include "exeapi.h"
#include "MonApi.h"
#endif

#include "balmiscapi.h"
#include "balfsiapi.h"

/* Used markers:*/
#define SOI 0xD8
#define EOI 0xD9
#define APP0 0xE0
#define SOF 0xC0
#define DQT 0xDB
#define DHT 0xC4
#define SOS 0xDA
#define DRI 0xDD
#define COM 0xFE

#ifndef BYD_USE_SIM
#define max(a,b) (a)>(b)?(a):(b)
#define min(a,b) (a)<(b)?(a):(b)
#endif


#define JpegLookKBits(k)  \
  ( (word)(wordval>>(16-(k))) )

#define JPEGWORDHILO(ret,byte_high,byte_low) \
{                     \
  (ret)=(byte_high);  \
  (ret) <<= 8;        \
  (ret) |= byte_low;  \
}


static byte *buf; /* the buffer we use for storing the entire JPG file*/

static byte bp; /*current byte*/
static word wp; /*current word*/

static dword byte_pos; /* current byte position*/
#define BYTE_p(i) bp=buf[(i)++]
#define WORD_p(i) wp=(((word)(buf[(i)]))<<8) + buf[(i)+1]; (i)+=2

const static byte zigzag[64]={ 
    0, 1, 5, 6,14,15,27,28,
    2, 4, 7,13,16,26,29,42,
    3, 8,12,17,25,30,41,43,
    9,11,18,24,31,40,44,53,
    10,19,23,32,39,45,52,54,
    20,22,33,38,46,51,55,60,
    21,34,37,47,50,56,59,61,
    35,36,48,49,57,58,62,63 };


typedef struct{
	byte Length[17];
	word MinorCode[17];
	word MajorCode[17];
	byte *ValuePtr[17];
}JpegHuffmanTableT;


static uint16 *QT[4]; /* quantization tables, no more than 4 quantization tables (QT[0..3])*/

static JpegHuffmanTableT HTDC[4]; /*DC huffman tables , no more than 4 (0..3)*/
static JpegHuffmanTableT HTAC[4]; /*AC huffman tables                  (0..3)*/

static byte HTAC_ValueIndex[4][256];
static byte HTAC_Index[4][256];

static byte YQ_nr,CbQ_nr,CrQ_nr; /* quantization table number for Y, Cb, Cr*/
static byte YDC_nr,CbDC_nr,CrDC_nr; /* DC Huffman table number for Y,Cb, Cr*/
static byte YAC_nr,CbAC_nr,CrAC_nr; /* AC Huffman table number for Y,Cb, Cr*/

static byte Restart_markers; /* if 1 => Restart markers on , 0 => no restart markers*/
static word McuRestart; /*Restart markers appears every McuRestart MCU blocks*/
typedef void (*decode_MCU_func)(dword);


static sword DCY, DCCb, DCCr; /* Coeficientii DC pentru Y,Cb,Cr*/
static sword DctCoeff[64]; /* Current DctCoefficients*/

#define HV_MAX 2
static byte YData[HV_MAX*HV_MAX][64];
static byte CbData[HV_MAX*HV_MAX][64];
static byte CrData[HV_MAX*HV_MAX][64];
 
static byte Table1[64],Table2[64],Table3[64],Table4[64]; 

static sword CrTab[256],CbTab[256]; /* Precalculated Cr, Cb tables*/
static sword CrGreenTab[256], CbGreenTab[256];

static byte d_k=0;  /* Bit displacement in memory, relative to the offset of w1 
                     it's always <16*/
static word w1,w2; /* w1 = First word in memory; w2 = Second word*/
static dword wordval ; /* the actual used value in Huffman decoding.*/
static dword mask[17];
const static sword NegPow2[17]={0,-1,-3,-7,-15,-31,-63,-127,-255,-511,-1023,-2047,-4095,-8191,-16383,-32767};
/*static dword start_neg_pow2=(dword)NegPow2;*/
const static word BitFlag[16] = {0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80,0x100,0x200,0x400,0x800,0x1000,
		0x2000,0x4000,0x8000};

static byte LimitTable[5*256L + 128];
static byte *RLimitTable;

#define BYTES_PER_8PIXELS 16

#define JPEG24BIT_TO_16BIT(r, g, b)      ( (((r>>3)&0x1f) << 11) + (((g>>2)&0x3f) << 5) + ((b>>3)&0x1f) )

static JpegInfoType JpegInfo;

JpegErrorType JpegReadFile(char *FileName);
void *JpegGetMemory(int32 nSize);
void JpegFreeMemory(void *pBuf);
void JpegFreeData(void);
void SetACIndex(int Acnr);
void JpegIDCT(sword* coef_block,byte *output_buf,byte qnr);

static void InitData()
{
  buf = NULL; /* the buffer we use for storing the entire JPG file*/
  bp = 0; /*current byte*/
  wp = 0; /*current word*/
  byte_pos = 0; /* current byte position*/
  memset(QT, 0, sizeof(QT));

  YQ_nr=CbQ_nr=CrQ_nr=0; /* quantization table number for Y, Cb, Cr*/
  YDC_nr=CbDC_nr=CrDC_nr=0; /* DC Huffman table number for Y,Cb, Cr*/
  YAC_nr=CbAC_nr=CrAC_nr=0; /* AC Huffman table number for Y,Cb, Cr*/

  Restart_markers=0; /* if 1 => Restart markers on , 0 => no restart markers*/
  McuRestart=0; /*Restart markers appears every McuRestart MCU blocks*/


  DCY=DCCb=DCCr=0; /* Coeficientii DC pentru Y,Cb,Cr*/
  d_k=0;  /* Bit displacement in memory, relative to the offset of w1 
                       it's always <16*/
  w1=w2=0; /* w1 = First word in memory; w2 = Second word*/
  wordval=0; /* the actual used value in Huffman decoding.*/

  memset(HTDC, 0, sizeof(HTDC)); /*DC huffman tables , no more than 4 (0..3)*/
  memset(HTAC, 0, sizeof(HTAC));
  memset(HTAC_ValueIndex, 0, sizeof(HTAC_ValueIndex));
  memset(HTAC_Index, 0, sizeof(HTAC_Index));
  memset(YData, 0, sizeof(YData));
  memset(CbData, 0, sizeof(CbData));
  memset(CrData, 0, sizeof(CrData));
  memset(DctCoeff, 0, sizeof(DctCoeff));
  memset(Table1, 0, sizeof(Table1));
  memset(Table2, 0, sizeof(Table2));
  memset(Table3, 0, sizeof(Table3));
  memset(Table4, 0, sizeof(Table4));
  memset(CrTab, 0, sizeof(CrTab));
  memset(CbTab, 0, sizeof(CbTab));
  memset(CrGreenTab, 0, sizeof(CrGreenTab));
  memset(CbGreenTab, 0, sizeof(CbGreenTab));
  memset(mask, 0, sizeof(mask));
  memset(LimitTable, 0, sizeof(LimitTable));
  RLimitTable = NULL;
  memset(&JpegInfo, 0, sizeof(JpegInfo));
  
}

void JpegPrepareRangeLimitTable()
/* Allocate and fill in the sample_range_limit table */
{
	int j;
/*	RLimitTable = (byte *)malloc(5 * 256L + 128) ;*/
	/* First segment of "simple" table: limit[x] = 0 for x < 0 */
	memset((void *)LimitTable,0,256);
	RLimitTable = LimitTable + 256;	/* allow negative subscripts of simple table */

	/* Main part of "simple" table: limit[x] = x */
	for (j = 0; j < 256; j++) 
    RLimitTable[j] = j;
	/* End of simple table, rest of first half of post-IDCT table */
	for (j = 256; j < 640; j++) 
    RLimitTable[j] = 255;
	/* Second half of post-IDCT table */
	memset((void *)(RLimitTable + 640),0,384);
	for (j = 0; j < 128 ; j++) 
    RLimitTable[j+1024] = j;
}



sword JpegGetSValue(byte k)
/* k>0 always*/
/* Takes k bits out of the BIT stream (wordval), and makes them a signed value*/
{
	dword tmp = wordval;
	tmp <<= k;
	tmp >>= 16;
	if( !(tmp & BitFlag[k-1]))
		tmp += (word)NegPow2[k];
	return (sword)(tmp&0xffff);
}

void JpegSkipKBits16(byte k)
{
  byte b_high,b_low;
  d_k-=16;
  w1=w2;
  /* Get the next word in w2*/
  BYTE_p(byte_pos);
  if (bp!=0xFF) 
    b_high=bp;
  else 
  {
    if (buf[byte_pos]==0) 
      ++byte_pos; /*skip 00*/
    else 
      --byte_pos; /* stop byte_pos pe restart marker*/
    b_high=0xFF;
  }
  BYTE_p(byte_pos);
  if (bp!=0xFF) 
    b_low=bp;
  else 
  {
    if (buf[byte_pos]==0) 
      ++byte_pos; /*skip 00*/
    else 
      --byte_pos; /* stop byte_pos pe restart marker*/
    b_low=0xFF;
  }
  JPEGWORDHILO(w2,b_high,b_low);/*w2=JpegWordHiLo(b_high,b_low);*/
}

#define JpegSkipKBits(k)  \
  {d_k+=(k); \
  if(d_k >=16) JpegSkipKBits16(k);  \
  wordval = ((dword)(w1)<<16) + w2; \
  wordval <<= d_k;      \
  wordval >>= 16;}

sword JpegGetKBits(byte k)
{
	sword signed_wordvalue;
	signed_wordvalue=JpegGetSValue(k);
	JpegSkipKBits(k);
	return signed_wordvalue;
}

void JpegCalculateMask()
{
	byte k;
	dword tmpdv;
	for (k=0;k<=16;k++) 
  { 
    tmpdv=0x10000;
    mask[k]=(tmpdv>>k)-1; /*precalculated bit mask*/
  }
}

void JpegInitQT()
{
	byte i;
	for (i=0;i<=3;i++)
	{
    QT[i]=(uint16 *)JpegGetMemory(sizeof(uint16)*64);
    memset(QT[i], 0, sizeof(uint16)*64);
  }
}

void JpegLoadQuantTable(uint16 *quant_table)
{
  byte j;
  /* Load quantization coefficients from JPG file, scale them for DCT and reorder*/
  /* from zig-zag order*/
  for (j=0;j<=63;j++) 
    quant_table[j]=buf[byte_pos+j];

	byte_pos+=64;
}

void JpegLoadHuffmanTable(JpegHuffmanTableT *HT)/*load_Huffman_table(Huffman_table *HT)*/
{
	byte k,j;
	dword code;
	
	/*read number of every length(1-16)*/
	for (j=1;j<=16;j++) 
  {
		BYTE_p(byte_pos);
		HT->Length[j]=bp;
	}
	
	/*init valuePtr*/
	for(j=1;j<=16;j++)
  {
		HT->ValuePtr[j] = NULL;
	}
	/*load value pointers*/
	for (k=1;k<=16;k++)
  {
		if(HT->Length[k]>0)
    {
			HT->ValuePtr[k] = buf + byte_pos;
			for (j=0;j<HT->Length[k];j++) 
      {
				BYTE_p(byte_pos);
			}
		}
	}
		
	/*calculate the minimum and maximum for every length*/
	code=0;
	for (k=1;k<=16;k++) 
  {
		HT->MinorCode[k] = (word)code;
		for (j=1;j<=HT->Length[k];j++) 
      code++;
		HT->MajorCode[k]=(word)(code-1);
		code*=2;
		if (HT->Length[k]==0) 
    {
			HT->MinorCode[k]=0xFFFF;
			HT->MajorCode[k]=0;
		}
	}
}

#define JpegGetHuffmanValue(HT,length,code) \
  ( *((HT)->ValuePtr[length] + ((code) - (HT)->MinorCode[length]) ) )

void JpegProcessHuffmanData(byte DC_nr, byte AC_nr,sword *previous_DC)
{
	/* Process one data unit. A data unit = 64 DCT coefficients*/
	/* Data is decompressed by Huffman decoding, then the array is dezigzag-ed*/
	/* The result is a 64 DCT coefficients array: DctCoeff*/
	byte nr,k,j,EOB_found;
	register word tmp_Hcode;
	byte size_val,count_0;
	word *min_code,*maj_code; 
	word *max_val, *min_val;
	sword DCT_tcoeff[64];
	byte byte_temp;
	
  byte *HuffIndexTable;
  byte *HuffKTable;
	/* Start Huffman decoding*/
	/* First the DC coefficient decoding*/
	min_code=HTDC[DC_nr].MinorCode;
	maj_code=HTDC[DC_nr].MajorCode;
/*	huff_values=HTDC[DC_nr].V;*/

  memset(DCT_tcoeff,0,sizeof(DCT_tcoeff));
	
	nr=0;/* DC coefficient*/

 	min_val = &min_code[1]; max_val = &maj_code[1];
  for (k=1;k<=16;k++) 
  {
    tmp_Hcode=JpegLookKBits(k);
    
    if ( (tmp_Hcode<=*max_val)&&(tmp_Hcode>=*min_val) ) 
    { /*Found a valid Huffman code */
      JpegSkipKBits(k);
      size_val = JpegGetHuffmanValue(&HTDC[DC_nr],k,tmp_Hcode);
      
      if (size_val==0) 
        DCT_tcoeff[0]=*previous_DC;
      else 
      {
        DCT_tcoeff[0]=*previous_DC+JpegGetKBits(size_val);
        *previous_DC=DCT_tcoeff[0];
      }
      break;
    }
    min_val++; max_val++;
  }

	/* Second, AC coefficient decoding*/
	min_code=HTAC[AC_nr].MinorCode;
	maj_code=HTAC[AC_nr].MajorCode;
	
  HuffIndexTable = HTAC_ValueIndex[AC_nr];
  HuffKTable = HTAC_Index[AC_nr];
	nr=1; /* AC coefficient*/
	EOB_found=0;
	while ( (nr<=63)&&(!EOB_found) )
	{
    tmp_Hcode = JpegLookKBits(8);
    if(HuffKTable[tmp_Hcode]) /*got one*/
    {
      k = HuffKTable[tmp_Hcode];
      JpegSkipKBits(k);
 
      byte_temp = HuffIndexTable[tmp_Hcode];
      
      size_val=byte_temp&0xF;
      count_0=byte_temp>>4;
      if (size_val==0) 
      {
        if (count_0==0) 
          EOB_found=1;
        else if (count_0==0xF) 
          nr+=16; /*kip 16 zeroes */
      }
      else
      {
        dword tmp = wordval;
        nr+=count_0; /*skip count_0 zeroes */
        tmp <<= size_val;
        tmp >>= 16;
        if( !(tmp & BitFlag[size_val-1]))
          tmp += (word)NegPow2[size_val];
        DCT_tcoeff[nr++]=(sword)(tmp&0xffff);
	      JpegSkipKBits(size_val);
      }
    }
    else
    {
      max_val = &maj_code[9]; min_val =&min_code[9];
      for (k=9;k<=16;++k)
      {
        tmp_Hcode=JpegLookKBits(k);
        
        if ( (tmp_Hcode<=*max_val)&&(tmp_Hcode>=*min_val) )
        {
          JpegSkipKBits(k);
          byte_temp = JpegGetHuffmanValue(&HTAC[DC_nr],k,tmp_Hcode);
          
          size_val=byte_temp&0xF;
          count_0=byte_temp>>4;
          if (size_val==0) 
          {
            if (count_0==0) 
              EOB_found=1;
            else if (count_0==0xF) 
              nr+=16; /*kip 16 zeroes */
          }
          else
          {
            nr+=count_0; /*skip count_0 zeroes */
            DCT_tcoeff[nr++]=JpegGetKBits(size_val);
          }
          break;
        }
        ++min_val; ++max_val;
       }
    }
	}
	for (j=0;j<=63;j++) 
    		DctCoeff[j]=DCT_tcoeff[zigzag[j]]; 
}


void JpegPrecalculateCrCbTables()
{
	word k;
	for(k=0;k<256;++k)
  {
		CrTab[k]=(sword)((k-128.0)*1.402);
		CbTab[k]=(sword)((k-128.0)*1.772);
		CbGreenTab[k]=(int)(-0.34414*(k-128.0));
		CrGreenTab[k]=(int)(-0.71414*(k-128.0));
	}
		
}

void JpegConvert8x8YCbCrToRGB(byte *Y, byte *Cb, byte *Cr, dword im_loc, dword X_image_bytes, byte *im_buffer,int nXUnit,int nYUnit)
{
  word x,y;
  int16 r,g,b;
  word nMaxX,nMaxY;
	byte im_nr;
	byte *Y_val = Y, *Cb_val = Cb, *Cr_val = Cr;
	byte *ibuffer = im_buffer + im_loc;
	if(nXUnit*8 >= JpegInfo.XImage || nYUnit*8 >= JpegInfo.YImage)
    return;
  nMaxX = min(8,JpegInfo.XImage-nXUnit*8);
  nMaxY = min(8,JpegInfo.YImage-nYUnit*8);

	for (y=0;y<nMaxY;y++)
	{
		im_nr=0;
		for (x=0;x<nMaxX;x++)
		{
			b = RLimitTable[*Y_val + CbTab[*Cb_val]];  
			g = RLimitTable[*Y_val + CrGreenTab[*Cr_val] + CbGreenTab[*Cb_val]]; 
			r = RLimitTable[*Y_val + CrTab[*Cr_val]];  
			((uint16*)ibuffer)[im_nr/2] = JPEG24BIT_TO_16BIT(r,g,b);
      im_nr+=2;
			Y_val++; Cb_val++; Cr_val++;  
  	}
    if(nMaxX!=8)
    {
      Y_val += (8-nMaxX);
      Cb_val += (8-nMaxX);
      Cr_val += (8-nMaxX);
    }
		ibuffer+=X_image_bytes;
	}
}

void JpegConvert8x8YToRGB(byte *Y, dword im_loc, dword X_image_bytes, byte *im_buffer,int nXUnit,int nYUnit)
{
  word x,y;
  word nMaxX,nMaxY;
	byte im_nr;
	byte *Y_val = Y;
	byte *ibuffer = im_buffer + im_loc;
  int16 r,g,b;

	if(nXUnit*8 >= JpegInfo.XImage || nYUnit*8 >= JpegInfo.YImage)
    return;
  nMaxX = min(8,JpegInfo.XImage-nXUnit*8);
  nMaxY = min(8,JpegInfo.YImage-nYUnit*8);

	for (y=0;y<nMaxY;y++)
	{
		im_nr=0;
		for (x=0;x<nMaxX;x++)
		{
      b = RLimitTable[*Y_val]; 
			g = RLimitTable[*Y_val];
			r = RLimitTable[*Y_val]; 
      ((uint16*)ibuffer)[im_nr/2] = JPEG24BIT_TO_16BIT(r,g,b);
      im_nr+=2;
			Y_val++; 
		}
    if(nMaxX!=8)
    {
      Y_val += (8-nMaxX);
    }
		ibuffer+=X_image_bytes;
	}
}

void JpegConvert8x8YCbCrTORGBTab(byte *Y, byte *Cb, byte *Cr, byte *tab, dword im_loc, dword X_image_bytes, byte *im_buffer,int nXUnit,int nYUnit)
{
	word x,y;
  word nMaxX,nMaxY;
	byte nr, im_nr;
	byte Y_val,Cb_val,Cr_val;
	byte *ibuffer = im_buffer + im_loc;
  int16 r,g,b;

  if(nXUnit*8 >= JpegInfo.XImage || nYUnit*8 >= JpegInfo.YImage)
    return;
  nMaxX = min(8,JpegInfo.XImage-nXUnit*8);
  nMaxY = min(8,JpegInfo.YImage-nYUnit*8);

	nr=0;
	for (y=0;y<nMaxY;y++)
	{
		im_nr=0;
		for (x=0;x<nMaxX;x++)
		{
			Y_val=Y[nr];
			Cb_val=Cb[tab[nr]]; Cr_val=Cr[tab[nr]]; 

      b = RLimitTable[Y_val + CbTab[Cb_val]]; 
			g = RLimitTable[Y_val + CrGreenTab[Cr_val] + CbGreenTab[Cb_val]];
			r = RLimitTable[Y_val + CrTab[Cr_val]]; 
      ((uint16*)ibuffer)[im_nr/2] = JPEG24BIT_TO_16BIT(r,g,b);
      im_nr+=2;
			nr++; 
		}
    if(nMaxX!=8)
    {
      nr += (8-nMaxX);
    }
		ibuffer+=X_image_bytes;
	}
}

void JpegConvertYCbCrToRGB( dword im_loc,int nUnitX,int nUnitY, byte *im_buffer)
{
  
	byte i,j;
  if(JpegInfo.YV == JpegInfo.CrV && JpegInfo.YH == JpegInfo.CrH)
  {
    for(i=0;i<JpegInfo.YV;++i)
    {
      for(j=0;j<JpegInfo.YH;++j)
      {
        	JpegConvert8x8YCbCrToRGB(YData[i*JpegInfo.YV+j],CbData[i*JpegInfo.YV+j],CrData[i*JpegInfo.YV+j],im_loc+BYTES_PER_8PIXELS*j+JpegInfo.YIncValue*i,JpegInfo.XImageBytes,im_buffer,nUnitX*JpegInfo.YH+j,nUnitY*JpegInfo.YV+i);
      }
    }
  }
  else if(JpegInfo.CrV==0 || JpegInfo.CrH==0)   /*gray bitmap*/
  {
    for(i=0;i<JpegInfo.YV;++i)
    {
      for(j=0;j<JpegInfo.YH;++j)
      {
        	JpegConvert8x8YToRGB(YData[i*JpegInfo.YV+j],im_loc+BYTES_PER_8PIXELS*j+JpegInfo.YIncValue*i,JpegInfo.XImageBytes,im_buffer,nUnitX*JpegInfo.YH+j,nUnitY*JpegInfo.YV+i);
      }
    }
  }
  else
  {
    if(JpegInfo.YH/JpegInfo.CrH == 1)
    {
      /*JpegInfo.YV/JpegInfo.CrV must == 2*/
      for(i=0;i<JpegInfo.YH;++i)
      {
        JpegConvert8x8YCbCrTORGBTab(YData[i],CbData[i],CrData[i],Table1,im_loc+i*BYTES_PER_8PIXELS,JpegInfo.XImageBytes,im_buffer,nUnitX*JpegInfo.YH+i,nUnitY*2);
      }
      for(i=0;i<JpegInfo.YH;++i)
      {
        JpegConvert8x8YCbCrTORGBTab(YData[i+JpegInfo.YH],CbData[i],CrData[i],Table3,im_loc+i*BYTES_PER_8PIXELS+JpegInfo.YIncValue,JpegInfo.XImageBytes,im_buffer,nUnitX*JpegInfo.YH+i,nUnitY*2+1);
      }

    }
    else    /*==2*/
    {
      if(JpegInfo.YV/JpegInfo.CrV == 1)
      {
        for(i=0;i<JpegInfo.YV;++i)
        {
          JpegConvert8x8YCbCrTORGBTab(YData[i*JpegInfo.YV],CbData[i],CrData[i],Table1,im_loc+JpegInfo.YIncValue*i,JpegInfo.XImageBytes,im_buffer,nUnitX*2,nUnitY*JpegInfo.YV);
          JpegConvert8x8YCbCrTORGBTab(YData[i*JpegInfo.YV+1],CbData[i],CrData[i],Table2,im_loc+BYTES_PER_8PIXELS+JpegInfo.YIncValue*i,JpegInfo.XImageBytes,im_buffer,nUnitX*2+1,nUnitY*JpegInfo.YV+1);
        }
      }
      else  /*JpegInfo.YV/JpegInfo.CrV==2*/
      {
        JpegConvert8x8YCbCrTORGBTab(YData[0],CbData[0],CrData[0],Table1,im_loc,JpegInfo.XImageBytes,im_buffer,nUnitX*2,nUnitY*2);
        JpegConvert8x8YCbCrTORGBTab(YData[1],CbData[0],CrData[0],Table2,im_loc+BYTES_PER_8PIXELS,JpegInfo.XImageBytes,im_buffer,nUnitX*2+1,nUnitY*2);
        JpegConvert8x8YCbCrTORGBTab(YData[2],CbData[0],CrData[0],Table3,im_loc+JpegInfo.YIncValue,JpegInfo.XImageBytes,im_buffer,nUnitX*2,nUnitY*2+1);
        JpegConvert8x8YCbCrTORGBTab(YData[3],CbData[0],CrData[0],Table4,im_loc+JpegInfo.YIncValue+BYTES_PER_8PIXELS,JpegInfo.XImageBytes,im_buffer,nUnitX*2+1,nUnitY*2+1);
      }
    }
  }
 
}


void JpegCalculateTabs()
{
	static byte tab_temp[256];
	int16 x,y;
        int16 tmpYVCrV,tmpYHCrH;

  if(JpegInfo.CrV == 0 || JpegInfo.CrH == 0)  /*needn't tab*/
    return;

  tmpYVCrV = JpegInfo.YV/JpegInfo.CrV;
  tmpYHCrH = JpegInfo.YH/JpegInfo.CrH;
  if(tmpYVCrV==0 || tmpYHCrH==0)
   {
    return;
   }

	/* calculate 16x16*/
  for (y=0;y<16;y++)
  {
    int16 tmp,tmpy;
    tmp = (y/tmpYVCrV) * 8;
    tmpy = y*16;
    for (x=0;x<16;x++)
      tab_temp[tmpy+x] = tmp + x/tmpYHCrH;
  }
		
	/* calculate 8x8 */
  for (y=0;y<8;y++)
  {
    int16 tmpy8,tmpy16;
    tmpy8 = y*8;
    tmpy16 = y*16;
    for (x=0;x<8;x++)
      Table1[tmpy8+x]=tab_temp[tmpy16+x];
    for (x=8;x<16;x++)
      Table2[tmpy8+(x-8)]=tab_temp[tmpy16+x];
  }
  for (y=8;y<16;y++)
  {
    int16 tmpy8,tmpy16;
    tmpy8 = (y-8)*8;
    tmpy16 = y*16;
    for (x=0;x<8;x++)
      Table3[tmpy8+x]=tab_temp[tmpy16+x];
    for (x=8;x<16;x++)
      Table4[tmpy8+(x-8)]=tab_temp[tmpy16+x];
  }
}

int JpegInitDecoding()
{
	byte_pos=0;
	JpegInitQT();
	JpegCalculateMask();
	JpegPrepareRangeLimitTable();
	JpegPrecalculateCrCbTables();
	return 1; /*for future error check*/
}


JpegErrorType JpegLoadHeader(JpegBitmapType  *BitmapPtr)
{
	word length;
	uint16 *qtable;
	dword old_byte_pos;

	JpegHuffmanTableT *htable;
	dword j;
	byte precision,comp_id,nr_components;
	byte QT_info,HT_info;
	byte SOS_found,SOF_found;
	
	if ((buf[0]!=0xFF)||(buf[1]!=SOI)) 
    return JPEG_ERROR_BAD_FILE;

	JpegInitDecoding();
	
	byte_pos =2 ;
	/* Start decoding process*/
	SOS_found=0; 
  SOF_found=0; 
  Restart_markers=0;
	while ((byte_pos<JpegInfo.Length)&&!SOS_found)
	{
		BYTE_p(byte_pos);
		if (bp!=0xFF) 
      continue;
		/* A marker was found*/
		BYTE_p(byte_pos);
		switch(bp)
		{
		case DQT: 
      WORD_p(byte_pos); 
      length=wp; /* length of the DQT marker*/
			for (j=0;j<wp-2;)
			{
				old_byte_pos=byte_pos;
				BYTE_p(byte_pos); 
        QT_info=bp;
				if ((QT_info>>4)!=0)
					return JPEG_ERROR_BAD_FILE;
				qtable=QT[QT_info&0xF];
				JpegLoadQuantTable(qtable);
				j+=byte_pos-old_byte_pos;
			}
			break;
		case DHT: 
      WORD_p(byte_pos); 
      length=wp;
			for (j=0;j<wp-2;)
			{
				old_byte_pos=byte_pos;
				BYTE_p(byte_pos); 
        HT_info=bp;
				if ((HT_info&0x10)!=0) 
          htable=&HTAC[HT_info&0xF];
				else 
          htable=&HTDC[HT_info&0xF];
				JpegLoadHuffmanTable(htable);
        if((HT_info&0x10)!=0) 
          SetACIndex(HT_info&0xF);
				j+=byte_pos-old_byte_pos;
			}
			break;
		case COM: 
      WORD_p(byte_pos); 
      length=wp;
			byte_pos+=wp-2;
			break;
		case DRI: 
      Restart_markers=1;
			WORD_p(byte_pos); 
      length=wp; /*should be = 4*/
			WORD_p(byte_pos);  
      McuRestart=wp;
			if (McuRestart==0) 
        Restart_markers=0;
			break;
		case SOF: 
      WORD_p(byte_pos); 
      length=wp; /*should be = 8+3*3=17*/
			BYTE_p(byte_pos); 
      precision=bp;
			if (precision!=8) 
        return JPEG_ERROR_BAD_FILE;
			WORD_p(byte_pos); 
      JpegInfo.YImage=wp; 
      WORD_p(byte_pos); 
      JpegInfo.XImage=wp;

			BYTE_p(byte_pos); 
      nr_components=bp;
      if(nr_components == 1)  /*gray bitmap*/
      {
        BYTE_p(byte_pos); comp_id=bp;
				if (comp_id!=1)    /*must be Y*/
          return JPEG_ERROR_BAD_FILE;
				BYTE_p(byte_pos); 
        JpegInfo.YH=bp>>4;
        JpegInfo.YV=bp&0xF;

				BYTE_p(byte_pos); 
        YQ_nr=bp;
      }
      else if(nr_components == 3)
      {
        for (j=1;j<=3;j++)
        {
          BYTE_p(byte_pos); 
          comp_id=bp;
          if ((comp_id==0)||(comp_id>3)) 
            return JPEG_ERROR_BAD_FILE;
          switch (comp_id)
          {
          case 1: /* Y*/
            BYTE_p(byte_pos); 
            JpegInfo.YH=bp>>4;
            JpegInfo.YV=bp&0xF;

            BYTE_p(byte_pos); 
            YQ_nr=bp;
            break;
          case 2: /* Cb*/
            BYTE_p(byte_pos); 
            JpegInfo.CbH=bp>>4;
            JpegInfo.CbV=bp&0xF;

            BYTE_p(byte_pos); 
            CbQ_nr=bp;
            break;
          case 3: /* Cr*/
            BYTE_p(byte_pos); 
            JpegInfo.CrH=bp>>4;
            JpegInfo.CrV=bp&0xF;

            BYTE_p(byte_pos); 
            CrQ_nr=bp;
            break;
          }
        }
      }
      else
         return JPEG_ERROR_BAD_FILE;
			SOF_found=1;
			break;
		case SOS: 
      WORD_p(byte_pos); 
      length=wp; /*should be = 6+3*2=12*/
			BYTE_p(byte_pos); 
      nr_components=bp;
			if (nr_components==1)   /*gray bitmap*/
      {
        BYTE_p(byte_pos); comp_id=bp;
        if (comp_id!=1)   /*must be Y*/
          return JPEG_ERROR_BAD_FILE;
        BYTE_p(byte_pos); 
        YDC_nr=bp>>4;
        YAC_nr=bp&0xF;
      }
      else if(nr_components==3)
      {
        for (j=1;j<=3;j++)
        {
          BYTE_p(byte_pos); 
          comp_id=bp;
          if ((comp_id==0)||(comp_id>3)) 
            return JPEG_ERROR_BAD_FILE;
          switch (comp_id)
          {
          case 1: /* Y*/
            BYTE_p(byte_pos); 
            YDC_nr=bp>>4;
            YAC_nr=bp&0xF;
            break;
          case 2: /* Cb*/
            BYTE_p(byte_pos); 
            CbDC_nr=bp>>4;
            CbAC_nr=bp&0xF;
            break;
          case 3: /* Cr*/
            BYTE_p(byte_pos); 
            CrDC_nr=bp>>4;
            CrAC_nr=bp&0xF;
            break;
          }
        }
      }
      else
        return JPEG_ERROR_BAD_FILE;
			BYTE_p(byte_pos); 
      BYTE_p(byte_pos); 
      BYTE_p(byte_pos); /* Skip 3 bytes*/
			SOS_found=1;
			break;
		case 0xFF:
			break; /* do nothing for 0xFFFF*/
			/* filling purposes and should be ignored*/
		default:  WORD_p(byte_pos); length=wp;
			byte_pos+=wp-2; /*skip unknown marker*/
			break;
		}
	}
	if (!SOS_found) 
    return JPEG_ERROR_BAD_FILE;
	if (!SOF_found) 
    return JPEG_ERROR_BAD_FILE;
	
	if ((JpegInfo.CbH>JpegInfo.YH)||(JpegInfo.CrH>JpegInfo.YH)) 
    return JPEG_ERROR_BAD_FILE;
	if ((JpegInfo.CbV>JpegInfo.YV)||(JpegInfo.CrV>JpegInfo.YV)) 
    return JPEG_ERROR_BAD_FILE;
  
  if(JpegInfo.YH>2 || JpegInfo.YV>2) /*support 1 and 2 only*/
    return JPEG_ERROR_BAD_FILE;  
  if(JpegInfo.CbH!=JpegInfo.CrH || JpegInfo.CbV!=JpegInfo.CrV)
    return JPEG_ERROR_BAD_FILE;
	
	JpegInfo.Hmax=JpegInfo.YH;
  JpegInfo.Vmax=JpegInfo.YV;
	if ( JpegInfo.XImage%(JpegInfo.Hmax*8)==0) 
    JpegInfo.XRound=JpegInfo.XImage; /* JpegInfo.XRound = Multiple of JpegInfo.Hmax*8*/
	else 
    JpegInfo.XRound=(JpegInfo.XImage/(JpegInfo.Hmax*8)+1)*(JpegInfo.Hmax*8);
	if ( JpegInfo.YImage%(JpegInfo.Vmax*8)==0) 
    JpegInfo.YRound=JpegInfo.YImage; /* JpegInfo.YRound = Multiple of JpegInfo.Vmax*8*/
	else 
    JpegInfo.YRound=(JpegInfo.YImage/(JpegInfo.Vmax*8)+1)*(JpegInfo.Vmax*8);
	
	BitmapPtr->Bmp=(byte *)JpegGetMemory(JpegInfo.XImage*JpegInfo.YImage*BYTES_PER_PIXEL);
	if (BitmapPtr->Bmp==NULL) 
    return JPEG_ERROR_NO_MEMORY;/*("No enough memory for storing the JPEG image");*/
  memset(BitmapPtr->Bmp, 0, JpegInfo.XImage*JpegInfo.YImage*BYTES_PER_PIXEL);
  BitmapPtr->BitCount = 16;
  BitmapPtr->h = JpegInfo.YImage;
  BitmapPtr->w = JpegInfo.XImage;
	
	return JPEG_ERROR_SUCCESS;
}

void JpegResync()
/* byte_pos  = pozitionat pe restart marker*/
{
	byte_pos+=2;
	BYTE_p(byte_pos);
	if (bp==0xFF) 
    byte_pos++; /* skip 00*/
	JPEGWORDHILO(w1,bp,0);
	BYTE_p(byte_pos);
	if (bp==0xFF) 
    byte_pos++; /* skip 00*/
	w1+=bp;
	BYTE_p(byte_pos);
	if (bp==0xFF) 
    byte_pos++; /* skip 00*/
	JPEGWORDHILO(w2,bp,0);
	BYTE_p(byte_pos);
	if (bp==0xFF) 
    byte_pos++; /* skip 00*/
	w2+=bp;
	wordval=w1; 
  d_k=0; /* Reinit bitstream decoding*/

	DCY=0; 
  DCCb=0; 
  DCCr=0; /* Init DC coefficients*/
}


void JpegDecodeMCU(dword im_loc,int nUnitX, int nUnitY,byte *bmp)
{
	byte i;
	/*Y*/
	for(i=0;i<JpegInfo.YH*JpegInfo.YV;++i)
  {
		JpegProcessHuffmanData(YDC_nr,YAC_nr,&DCY);
		JpegIDCT(DctCoeff,YData[i],YQ_nr);
	}
	/*Cb*/
	for(i=0;i<JpegInfo.CbH*JpegInfo.CbV;++i)
  {
		JpegProcessHuffmanData(CbDC_nr,CbAC_nr,&DCCb);
		JpegIDCT(DctCoeff,CbData[i],CbQ_nr);
	}
	/*Cr*/
	for(i=0;i<JpegInfo.CrH*JpegInfo.CrV;++i)
  {
		JpegProcessHuffmanData(CrDC_nr,CrAC_nr,&DCCr);
		JpegIDCT(DctCoeff,CrData[i],CrQ_nr);
	}

	JpegConvertYCbCrToRGB(im_loc,nUnitX,nUnitY,bmp);
}


JpegErrorType JpegDecodeImage(JpegBitmapType  *BitmapPtr)
{
	word x_mcu_cnt,y_mcu_cnt;
	dword nr_mcu;
	word X_MCU_nr,Y_MCU_nr; 
	dword MCU_dim_x; 
	dword im_loc_inc; 
	dword im_loc; 
	
	byte_pos-=2;
	JpegResync();
	
	JpegInfo.YIncValue = BYTES_PER_8PIXELS*JpegInfo.XImage;
	JpegCalculateTabs();
	
	MCU_dim_x=JpegInfo.Hmax*8*BYTES_PER_PIXEL;
	
	Y_MCU_nr=JpegInfo.YRound/(JpegInfo.Vmax*8); /* nr of MCUs on Y axis*/
	X_MCU_nr=JpegInfo.XRound/(JpegInfo.Hmax*8); /* nr of MCUs on X axis*/
	
	JpegInfo.XImageBytes=JpegInfo.XImage*BYTES_PER_PIXEL;
  im_loc_inc = (JpegInfo.Vmax*8-1) * JpegInfo.XImageBytes;
	nr_mcu=0; 
  im_loc=0; /* memory location of the current MCU*/

	for (y_mcu_cnt=0;y_mcu_cnt<Y_MCU_nr;y_mcu_cnt++)
	{
		for (x_mcu_cnt=0;x_mcu_cnt<X_MCU_nr;x_mcu_cnt++)
		{
			/*decode_MCU(im_loc);*/
      JpegDecodeMCU(im_loc,x_mcu_cnt,y_mcu_cnt,BitmapPtr->Bmp);
			if ((Restart_markers)&&((nr_mcu+1)%McuRestart==0)) 
        JpegResync();
			nr_mcu++;
      if(x_mcu_cnt<X_MCU_nr-1)
			  im_loc+=MCU_dim_x;
      else
      {
        word nXMax;
        nXMax = min(8*JpegInfo.YH,JpegInfo.XImage-x_mcu_cnt*8*JpegInfo.YH);
        im_loc += nXMax*BYTES_PER_PIXEL;
      }
		}
		im_loc+=im_loc_inc;
	}
  return JPEG_ERROR_SUCCESS;
}

JpegErrorType 	JpegDecode(	char *FileName,	
				JpegBitmapType  *BitmapPtr )
{
  JpegErrorType ret;

  InitData();


  
  ret = JpegReadFile(FileName);
  if(ret!=JPEG_ERROR_SUCCESS)
  {
    return ret;
  }
  ret = (JpegErrorType)JpegLoadHeader(BitmapPtr);
  if(ret!=JPEG_ERROR_SUCCESS)
  {
    JpegFreeMemory(buf);
    return ret;
  }
  ret = JpegDecodeImage(BitmapPtr);
  JpegFreeData();
  if(ret!=JPEG_ERROR_SUCCESS)
    return ret;
  return JPEG_ERROR_SUCCESS;
}

JpegErrorType JpegReadFile(char *FileName)
{
  BalFsiHandleT FileHandle;

  BalFsiFileAttribT FileInfo;
  uint32 nItems;
  BalFsiResultT ret;

 if( (ret = BalFsiFileOpen(&FileHandle, FileName, FSI_FILE_OPEN_READ_EXIST)) != FSI_SUCCESS)
 {
 	return JPEG_ERROR_NO_FILE;
 }
 
  BalFsiGetFileHandleAttrib(FileHandle,&FileInfo);
  JpegInfo.Length = FileInfo.Size;
  buf=(byte *)JpegGetMemory(JpegInfo.Length+BYTES_PER_PIXEL);
	if (buf==NULL) 
  {
    BalFsiFileClose(FileHandle);
    return JPEG_ERROR_NO_MEMORY;
  }
  memset(buf, 0, JpegInfo.Length+BYTES_PER_PIXEL);
  nItems = JpegInfo.Length;
  BalFsiFileRead(buf,1,&nItems,FileHandle);
  BalFsiFileClose(FileHandle);
  return JPEG_ERROR_SUCCESS;

}

void *JpegGetMemory(int32 nSize)
{
#ifdef BYD_USE_SIM
  return malloc(nSize);
#else
  return BalMalloc(nSize);
#endif
}

void JpegFreeMemory(void *pBuf)
{
 #ifdef BYD_USE_SIM
  free(pBuf);
#else
  BalFree(pBuf);
#endif
}

void JpegFreeData()
{
  int i;
  for(i=0;i<4;++i)
    JpegFreeMemory(QT[i]);

  if(buf != NULL)
    JpegFreeMemory(buf);

}


JpegErrorType JpegTest(byte *pData,JpegBitmapType *pBitmap)
{
  JpegErrorType ret;
  uint16 FileLen;

  buf=NULL;
  FileLen = *pData + ((*(pData+1))<<8);
  JpegInfo.Length = FileLen;
  /*read file*/
  buf = (byte*)JpegGetMemory(FileLen);
  if(buf==NULL)
  {
    return JPEG_ERROR_NO_MEMORY;
  }

  memcpy(buf,pData+2,FileLen);
  ret = JpegLoadHeader(pBitmap);
  if(ret!=JPEG_ERROR_SUCCESS)
  {
    return ret;
  }
  ret = JpegDecodeImage(pBitmap);
  if(ret!=JPEG_ERROR_SUCCESS)
   {
    return ret;
   }
  return JPEG_ERROR_SUCCESS;
}

JpegErrorType 	JpegDecodeBuffer(const uint8* BufferP, uint32 BuffLen,	JpegBitmapType  *BitmapPtr )
{
  JpegErrorType ret;

  if (BufferP == NULL || BuffLen == 0)
  {
    return JPEG_ERROR_NO_MEMORY;
  }

  InitData();  
  
  buf=(byte *)JpegGetMemory(BuffLen + BYTES_PER_PIXEL);
	if (buf==NULL) 
  {
    return JPEG_ERROR_NO_MEMORY;
  }
  memcpy(buf, BufferP, BuffLen);
  JpegInfo.Length = BuffLen;

  ret = (JpegErrorType)JpegLoadHeader(BitmapPtr);
  if(ret!=JPEG_ERROR_SUCCESS)
  {
    JpegFreeMemory(buf);
    return ret;
  }
  ret = JpegDecodeImage(BitmapPtr);
  JpegFreeData();
  if(ret!=JPEG_ERROR_SUCCESS)
    return ret;
  return JPEG_ERROR_SUCCESS;
}

#define CONST_BITS  13
#define PASS1_BITS  2

#define FIX_0_298631336  ((int32)  2446)	 
#define FIX_0_390180644  ((int32)  3196)	 
#define FIX_0_541196100  ((int32)  4433)	 
#define FIX_0_765366865  ((int32)  6270)	 
#define FIX_0_899976223  ((int32)  7373)	 
#define FIX_1_175875602  ((int32)  9633)	 
#define FIX_1_501321110  ((int32)  12299)	 
#define FIX_1_847759065  ((int32)  15137)	 
#define FIX_1_961570560  ((int32)  16069)	 
#define FIX_2_053119869  ((int32)  16819)	 
#define FIX_2_562915447  ((int32)  20995) 
#define FIX_3_072711026  ((int32)  25172)	 

static int ShiftTemp;
#define RIGHT_SHIFT(x,shft)  \
	((ShiftTemp = (x)) < 0 ? \
	(ShiftTemp >> (shft)) | ((~(0L)) << (32-(shft))) : \
  (ShiftTemp >> (shft)))
#define DESCALE(x,n)  RIGHT_SHIFT((x) + (1L << ((n)-1)), n)


void JpegIDCT(sword* coef_block,byte *output_buf,byte qnr)
{	int32 tmp0, tmp1, tmp2, tmp3;
	int32 tmp10, tmp11, tmp12, tmp13;
	int32 z1, z2, z3, z4, z5;
	int itp;
	int inptrary[64];
  int *inptr;
	int * wsptr;
	char *outptr;
	int ctr;
	int workspace[64];
  int j;

  inptr = inptrary;
  for(j=0;j<=63;j++)
  {
    inptr[j] = ((int32)coef_block[j]) * QT[qnr][zigzag[j]];
  }


	wsptr = workspace;
	for (ctr = 8; ctr > 0; ctr--)
	{	if ((inptr[8] | inptr[16] | inptr[24] | inptr[32] | inptr[40] | inptr[48] |
				inptr[56]) == 0) 
		{	int dcval = inptr[0] << PASS1_BITS;
		    wsptr[0] = dcval;
		    wsptr[8] = dcval;
		    wsptr[16] = dcval;
		    wsptr[24] = dcval;
		    wsptr[32] = dcval;
		    wsptr[40] = dcval;
		    wsptr[48] = dcval;
		    wsptr[56] = dcval;
		    inptr++;
		    wsptr++;
		    continue;
	    }
	    z2 = inptr[16];
	    z3 = inptr[48];
	    z1 = (z2 + z3)*FIX_0_541196100;
	    tmp2 = z1 + z3*(- FIX_1_847759065);
	    tmp3 = z1 + z2*FIX_0_765366865;
	    z2 = inptr[0];
	    z3 = inptr[32];
	    tmp0 = (z2 + z3) << CONST_BITS;
	    tmp1 = (z2 - z3) << CONST_BITS;
	    tmp10 = tmp0 + tmp3;
	    tmp13 = tmp0 - tmp3;
	    tmp11 = tmp1 + tmp2;
	    tmp12 = tmp1 - tmp2;
	    tmp0 = inptr[56];
	    tmp1 = inptr[40];
	    tmp2 = inptr[24];
	    tmp3 = inptr[8];
	    z1 = tmp0 + tmp3;
	    z2 = tmp1 + tmp2;
	    z3 = tmp0 + tmp2;
	    z4 = tmp1 + tmp3;
	    z5 = (z3 + z4) * FIX_1_175875602; 
	    tmp0 = tmp0 * FIX_0_298631336; 
	    tmp1 = tmp1 * FIX_2_053119869;
	    tmp2 = tmp2 * FIX_3_072711026;
	    tmp3 = tmp3 * FIX_1_501321110;
	    z1 = z1 * (- FIX_0_899976223);
	    z2 = z2 * (- FIX_2_562915447);
	    z3 = z3 * (- FIX_1_961570560);
	    z4 = z4 * (- FIX_0_390180644);
	    z3 += z5;
	    z4 += z5;
	    tmp0 += z1 + z3;
	    tmp1 += z2 + z4;
	    tmp2 += z2 + z3;
	    tmp3 += z1 + z4;
	    wsptr[0] = (int) DESCALE(tmp10 + tmp3, CONST_BITS-PASS1_BITS);
	    wsptr[56] = (int) DESCALE(tmp10 - tmp3, CONST_BITS-PASS1_BITS);
	    wsptr[8] = (int) DESCALE(tmp11 + tmp2, CONST_BITS-PASS1_BITS);
	    wsptr[48] = (int) DESCALE(tmp11 - tmp2, CONST_BITS-PASS1_BITS);
	    wsptr[16] = (int) DESCALE(tmp12 + tmp1, CONST_BITS-PASS1_BITS);
	    wsptr[40] = (int) DESCALE(tmp12 - tmp1, CONST_BITS-PASS1_BITS);
	    wsptr[24] = (int) DESCALE(tmp13 + tmp0, CONST_BITS-PASS1_BITS);
	    wsptr[32] = (int) DESCALE(tmp13 - tmp0, CONST_BITS-PASS1_BITS);
	    inptr++;
	    wsptr++;
	}
	wsptr = workspace;
	for (ctr = 0; ctr < 8; ctr++) {
    outptr = (char*)(output_buf + ctr*8);

    z2 = (int32) wsptr[2];
    z3 = (int32) wsptr[6];
    z1 = (z2 + z3) * FIX_0_541196100;
    tmp2 = z1 + z3 * (- FIX_1_847759065);
    tmp3 = z1 + z2 * FIX_0_765366865;
    tmp0 = ((int32) wsptr[0] + (int32) wsptr[4]) << CONST_BITS;
    tmp1 = ((int32) wsptr[0] - (int32) wsptr[4]) << CONST_BITS;
    tmp10 = tmp0 + tmp3;
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
    tmp0 = (int32) wsptr[7];
    tmp1 = (int32) wsptr[5];
    tmp2 = (int32) wsptr[3];
    tmp3 = (int32) wsptr[1];
    z1 = tmp0 + tmp3;
    z2 = tmp1 + tmp2;
    z3 = tmp0 + tmp2;
    z4 = tmp1 + tmp3;
    z5 = (z3 + z4) * FIX_1_175875602;
    tmp0 = tmp0 * FIX_0_298631336;
    tmp1 = tmp1 * FIX_2_053119869;
    tmp2 = tmp2 * FIX_3_072711026;
    tmp3 = tmp3 * FIX_1_501321110;
    z1 = z1 * (- FIX_0_899976223);
    z2 = z2 * (- FIX_2_562915447);
    z3 = z3 * (- FIX_1_961570560);
    z4 = z4 * (- FIX_0_390180644);
    z3 += z5;
    z4 += z5;
    tmp0 += z1 + z3;
    tmp1 += z2 + z4;
    tmp2 += z2 + z3;
    tmp3 += z1 + z4;
    itp = (int) DESCALE(tmp10 + tmp3,CONST_BITS+PASS1_BITS+3)+128;
    outptr[0] = RLimitTable[itp];
    itp = (int) DESCALE(tmp10 - tmp3,CONST_BITS+PASS1_BITS+3)+128;
    outptr[7] = RLimitTable[itp];
    itp = (int) DESCALE(tmp11 + tmp2,CONST_BITS+PASS1_BITS+3)+128;
    outptr[1] = RLimitTable[itp];
    itp = (int) DESCALE(tmp11 - tmp2,CONST_BITS+PASS1_BITS+3)+128;
    outptr[6] = RLimitTable[itp];
    itp = (int) DESCALE(tmp12 + tmp1,CONST_BITS+PASS1_BITS+3)+128;
    outptr[2] = RLimitTable[itp];
    itp = (int) DESCALE(tmp12 - tmp1,CONST_BITS+PASS1_BITS+3)+128;
    outptr[5] = RLimitTable[itp];
    itp = (int) DESCALE(tmp13 + tmp0,CONST_BITS+PASS1_BITS+3)+128;
    outptr[3] = RLimitTable[itp];
    itp = (int) DESCALE(tmp13 - tmp0,CONST_BITS+PASS1_BITS+3)+128;
    outptr[4] = RLimitTable[itp];
    wsptr += 8;
  }
}




void SetACIndex(int Acnr)
{
  
  byte *pValueIndex = HTAC_ValueIndex[Acnr];
  byte *pIndex = HTAC_Index[Acnr];
  JpegHuffmanTableT *pHuffTable = &HTAC[Acnr];
  int i,j,k,num;

  for(i=1;i<=8;++i)
  {
    if(pHuffTable->Length[i]>0)
    {
      for(j=0;j<pHuffTable->Length[i];++j)
      {
        byte first = (pHuffTable->MinorCode[i]+j)<<(8-i);
        num = 1<<(8-i);
        for(k=0;k<num;++k,++first)
        {
          pValueIndex[first] = JpegGetHuffmanValue(pHuffTable,i,pHuffTable->MinorCode[i]+j);/*pHuffTable->MinorCode[i] + j;*/
          pIndex[first] = i;
        }
      }
    }
  }
}

 /*****************************************************************************
 * $Log: uijpegdec.c $
 * Revision 1.5  2007/11/13 18:15:57  gzhu
 * fix a b/w jpg decode error
 * Revision 1.4  2007/11/12 17:34:20  ychen
 * fix coyote compile error
 * Revision 1.3  2007/11/12 15:37:59  gzhu
 * Fix display error bug
 * Revision 1.2  2007/11/09 17:39:08  gzhu
 * revise for 1bit, 8bit bmp display
 * Revision 1.1  2007/11/08 18:25:33  gzhu
 * Initial revision
 *****************************************************************************/
 


