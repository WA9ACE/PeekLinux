#ifndef __BTC_ATA_EM_H_20080506__
#define __BTC_ATA_EM_H_20080506__

typedef enum ATA_EM_DataType
{
    EM_MOBILE_INFO                = 0x01,
    EM_SERVING_CELL_PARAMS        = 0x02,
    EM_NEIGHBOURING_CELL_PARAMS   = 0x04,
    EM_LOCATION_PARAMS            = 0x08, 
    EM_CIPH_HOP_DTX_PARAMS        = 0x10,
    EM_GPRS_PARAMS                = 0x20

} AtaEmDataType;

typedef struct 
{
    char                IMEI[16];
    char                IMSI[16];
    unsigned long       TMSI;

} ATA_EM_Mobile_Info;

typedef struct
{
    unsigned short      arfcn;
    unsigned char       rssi;         //Received field strength
    unsigned char       RXQ;          //received quality
    unsigned char       RLT;          //radio link timeout counter
    short               C1;           //C1 Path lost criterion
    short               C2;           //C2 Parameter - often the same as RSSI for SC
    unsigned char       BSIC;         //Base Station ID Code
    unsigned char       TAV;          //Timing Advanced
    unsigned short      LAC;          //Location Area Code
                              
} ATA_EM_Serving_Cell_Parameters;

typedef struct
{
    unsigned char       NUM;          //number of neighbouring cells
    unsigned short      arfcn[7];     //channel numbers
    unsigned char       rssi[7];      //received signal strength
    short               C2[7];        //received C2 Parameter
    unsigned short      LAC_NC[7];    //Location Area Code
    unsigned char       BSIC_NC[7];   //Base Station ID Code 
                                      
} ATA_EM_Neighbour_Cell_Parameters;

typedef struct
{
    unsigned char       LUP;          //Location update period
    unsigned char       MCC[4];       //mobile country code
    unsigned char       MNC[4];       //mobile network code
    unsigned short      LAC;          //location area code
    
} ATA_EM_Location_Parameters;

typedef struct
{
    unsigned char       ciph_status;  //ciphering status
    unsigned short      HSN;          //hopping sequence number
    unsigned char       DTX_status;

} ATA_EM_Ciph_hop_DTX_Parameters;

typedef struct
{  
    U8                        gmm_state;                /* codes the GPRS state                               */
    U8                        tn;                       /* timeslot number                                    */
    U8                        nmo;                      /* network mode of operation                          */
    T_net_ctrl                net_ctrl;                 /* Network control                                    */
    U8                        cba;                      /* cell bar access                                    */
    U8                        rac;                      /* routing area code                                  */
    U8                        tav;                      /* timing advance                                     */
    U8                        dsc;                      /* actual downlink signalling counter                 */
    S16                       c31;                      /* reselection criterion C31 (GPRS)                   */
    S16                       c32;                      /* reselection criterion C32 (GPRS)                   */
    U8                        nco;                      /* Network Controlled reselection Order               */
    U8                        bss_paging_coord;         /* BS paging coordination                             */
    U8                        band_ind;                 /* Band Indicator                                     */
                                      
} ATA_EM_GPRS_Parameter;

typedef struct
{
    ATA_EM_Mobile_Info                MobileInfo;
    ATA_EM_Serving_Cell_Parameters    SCInfo;
    ATA_EM_Neighbour_Cell_Parameters  NCInfo;
    ATA_EM_Location_Parameters        LocInfo;
    ATA_EM_Ciph_hop_DTX_Parameters    CHDTXInfo;
    ATA_EM_GPRS_Parameter             GPRSInfo;

} ATA_EM_Data;

int ata_em_init(void);
void ata_em_exit(void);

int ata_em_get_data(AtaEmDataType type);

#endif //__BTC_ATA_EM_H_20080506__

