/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1P_VER.H
 *
 *        Filename l1p_ver.h
 *  Copyright 2003 (C) Texas Instruments  
 *
 ************* Revision Controle System Header *************/

/*****************************************************/
 /*  Copyright 2003 (C) Texas Instruments  */
/* GPRS DEVELOPMENT                                  */
/*****************************************************/

//#define    DEVELPMTVERSION  0x0001L
//#define    DEVELPMTVERSION  0x0002L  // Corrected TI_4, TI_7, TI_12
//#define    DEVELPMTVERSION  0x0003L  // Starting GPRS transfer + PCCCH idle + packet access
//#define    DEVELPMTVERSION  0x0004L  // Implemented 1st draft of GPRS MCU-DSP interfaceStarting
                                       // GPRS transfer + PCCCH idle + packet access
//#define    DEVELPMTVERSION  0x0005L  // Included first MAC-S version
//#define    DEVELPMTVERSION  0x0006L  // Included MCU-DSP interface and control drivers
//#define    DEVELPMTVERSION  0x0007L  // Included MAC-S first validated version
                                       // for L1S - MACS interface with one Tx - one Rx
//#define    DEVELPMTVERSION  0x0008L  // MAC-S validated version with 1 Rx - 1 Tx -
                                       // no poll response
//#define    DEVELPMTVERSION  0x0009L  // Implemented FIXE transmit power.
                                       // Ported PRACH CTRL to GPRS dsp drivers.
                                       // Finalized GPRS API mapping.
//#define    GPRSVERSION  0x0010L      // Corrected PRACH implementation for packet access
                                       // Modified TPU driver interface
                                       // renamed DEVELPMTVERSION => GPRSVERSION
//#define    GPRSVERSION  0x0011L      // Include modifications related to Packet Idle:
                                       // PCCCH: Normal + Extented + Rorg
                                       // Implement Serving Cell PBCCH reading
//#define    GPRSVERSION  0x0012L      // Packet Idle Tested with GPRS MCU/DSP Interface.
                                       // L1S optimized for PCCCH scheduling
                                       // pccch structure created in l1pa_l1ps_com for
                                       // PCCCH parameters description
//#define    GPRSVERSION  0x0013L      // Reworked l1pdsp_idle_rx_nb() in order to be able
                                       // to read PBCCH blocks on TS != 0.
                                       // PBCCHS scheduling modified.
//#define    GPRSVERSION  0x0014L      // Packet Access (PRACH) corrected for USF method
//#define    GPRSVERSION  0x0015L      // Compilable/ Loadable version for EVA4
//#define    GPRSVERSION  0x0016L      // l1pddsp_idle_rx_nb() modified:
                                       // timeslot number used to set a_ctrl_ched_gprs.
//#define    GPRSVERSION  0x0017L      // PALLC and PBCCHS tasks optimized and reworked:
                                       // PBCCH blocks read in Paging Reorganization
                                       // for Serving Cell PBCCH: synchro performed during
                                       // first PBCCH ctrl frame.
//#define    GPRSVERSION  0x0018L      // Corrected TI_31, TI_35, TI_40, TI_41, TI_42.
//#define    GPRSVERSION  0x0019L      // Corrected TI_34, TI_36, TI_50.
//#define    GPRSVERSION  0x0020L      // Corrected TI_59, Full list implemented
//#define    GPRSVERSION  0x0021L      // PRACH USF implementation corrected,
                                       // Multi slot TPU drivers implemented
//#define    GPRSVERSION  0x0022L      // Corrected TI_71, TI_81, TI_82, TI_54, TI_55, TI_58, TI_60,
                                       // Single Block corrected, started PTCCH implementation,
                                       // compatibility with S921bis v0.7.
//#define    GPRSVERSION  0x0023L      // Corrected TI_66, TI_67, TI_72, TI_73, TI_74, TI_83
                                       // Implement Cell Reselection Power Measurement
                                       // (Measures performed on BA(GPRS),
                                       // Network Control and Extended frequency list)
//#define    GPRSVERSION  0x0024L      // Neighbor monitoring (FBNEW, SB2, SBCONF and BCCHN)
                                       // task support in Packet Idle mode
//#define    GPRSVERSION  0x0025L      // Corrected TI_76, TI_77, TI_90
//#define    GPRSVERSION  0x0026L      // Implement Neighbour Cell Measurement in Packet transfer mode
//#define    GPRSVERSION  0x0027L      // Correct TI_63, TI_128, TI_132, TI_135, TI_136
                                       // + modif due to DSP code file 0x3100
//#define    GPRSVERSION  0x0028L      // Implemented interference measurement processing
                                       // in packet idle and packet transfer mode
//#define    GPRSVERSION  0x0029L      // PTCCH implementation completed.
//#define    GPRSVERSION  0x0030L      // WIN-ID reworked.
//#define    GPRSVERSION  0x0031L      // Modification of Polling response implementation (Packet idle)
//#define    GPRSVERSION  0x0032L      // WIN-ID fine tuning. TI_172 corrected.
//#define    GPRSVERSION  0x0033L      // Implemented: TBF release, PDCH release and
                                       // assignment_id management (TI_146) - TI_61 corrected
//#define    GPRSVERSION  0x0034L      // MAC-S: Complete support of fixed allocation mode
//#define    GPRSVERSION  0x0035L      // Interference measurements rework -
                                       // new L1/L3 interface in packet transfer mode
//#define    GPRSVERSION  0x0036L      // Uplink Transfer Power Control implemented
                                       // + correct: TI_163, TI_164, TI_166, TI_167,
                                       // TI_168, TI_171, TI_173
//#define    GPRSVERSION  0x0037L      // Packet transfer mode: BCCHN + FB/SB, TI_175, TI_176,
                                       // TI_177 corrected
//#define    GPRSVERSION  0x0038L      // Included code related to INT_LABO environment
//#define    GPRSVERSION  0x0039L      // PTCCH rework for using C-W-R scheme - PTCCH
                                       // and ITMEAS traces (preliminary) - corrected TI_190 -
                                       // Preliminary correction for TI_213
//#define    GPRSVERSION  0x0040L      // API mapping change for a_ptcchu_gprs and a_dd_md_gprs
                                       // and a_ptcchu_gprs, RAMP management modification
                                       // Corrected some problems: VOICE_MEMO_TASK programming,
                                       // C_VBUR value in l1_rf2.h, compilation flag correction
                                       // for l1_voice.c (in p_tools)
//#define    GPRSVERSION  0x0041L      // Implementation of AFC and TOA algorithm feeding for GPRS,
                                       // added new L1 mode: PACKET_TRANSFER_MODE
//#define    GPRSVERSION  0x0042L      // Cell Reselection measurement in Packet Transfer re-worked.
                                       // TI_179 corrected. MACS modified due to the fact
                                       // that now it's not needed to performed a measurement
                                       // in between RX and TX burst (Cf. CR A155 of 05.02).
//#define    GPRSVERSION  0x0043L      // GPRS AGC algorithms implemented, TI_189 corrected,
                                       // TOA problem on gpr_fo23-24 and gpr_jl43 corrected
                                       // l1a_send_result modification and l1s_send_result creation
                                       // for sending MPHP_ASSIGNMENT_CON, MPHP_SINGLE_BLOCK_CON,
                                       // MPHP_TINT_MEAS_IND, MPHP_INT_MEAS_IND through these functions
//#define    GPRSVERSION  0x0044L      // increased memory space for simulation scenario
//#define    GPRSVERSION  0x0045L      // create PBCCHN task (Packet Transfert/Idle  & GSM Idle modes)
                                       // PBCCHS task is allowed in Packet Transfer mode
                                       // Corrected:TI_154, TI_155
//#define    GPRSVERSION  0x0046L      // L1S scheduler / Merge manager reworked:
                                       // abort management changed.
                                       // Corrected:TI_45, TI_67, TI_111, TI_180,
                                       // TI_223, TI_226, TI_237.
//#define    GPRSVERSION  0x0047L      // l1ps_read_l3frm function correction
//#define    GPRSVERSION  0x0048L      // added L3 handling of Starting time with Anite Tester
                                       // fixed side effect due to GPRSVERSION 0x0047 correction
//#define    GPRSVERSION  0x0049L      // NSYNC code reviewed: FBNEW/SB2/SBCONF scheduling modified.
                                       // PCCCH reading code reviewed: optimized L1A
                                       // state machine and L1S scheduling.
                                       // L1S scheduler reviewed: added check on INACTIVE
                                       // to reset task semaphore.
                                       // ITMEAS code reviewed: L1A state machine simplified
                                       // (both idle and transfer).
                                       // Corrected: TI_258, TI_260.
//#define    GPRSVERSION  0x0050L      // add: BCCHN multipriorities and BCCHS in packet transfer mode
                                       // Corrected TI_129
//#define    GPRSVERSION  0x0051L      // Problem Report correction: TI_170, TI_202, TI_245, TI_248
//#define    GPRSVERSION  0x0052L      // Fixed allocation mode features: Repeat allocation,
                                       // synchronization change when the allocation bitmap exhausts
                                       // - Corrected TI_200, TI_261
//#define    GPRSVERSION  0x0053L      // AGC: Added BTS_PWR_CTL_MODE B algorithm, no power
                                       // control mode algorithm modification, Packet PAGC
                                       // algorithm improvement (LNA state)  corrected TI_254 -
                                       // 255 - 233
//#define    GPRSVERSION  0x0054L      // corrected TI_268, TI_269, FB_26 problem corrected
//#define    GPRSVERSION  0x0055L      // corrected TI_234 (part 1 and 2). Added DSP 3300 for
                                       // Samson c files.
//#define    GPRSVERSION  0x0056L      // corrected TI_272. Modification in order to compile
                                       // without GPRS code.
//#define    GPRSVERSION  0x0057L      // corrected TI_235 TI_235, TI_257, TI_275 plus L3
                                       // and LLC scenario updates.
//#define    GPRSVERSION  0x0058L      // corrected TI_281 and change DSP speed from 65 MHz to 78 MHz
//#define    GPRSVERSION  0x0059L      // Dsp Code 3302 tested and integrated.
//#define    GPRSVERSION  0x0060L      // DSP Code 3303 integrated - New API mapping used
                                       // with DSP = 33 (only suitable on SAMSON)
//#define    GPRSVERSION  0x0061L      // leadboot on 4kbytes for compatibilty with Hercules SAMSON
                                       // and calypso to come. REQ0277 on teamtrack.
                                       // command files for the ARM emulator for SAMSON. armemusam now.
//#define    GPRSVERSION  0x0062L      // Corrected TI_276 and TI_280 (USF 4 blocks granularity)
//#define    GPRSVERSION  0x0063L      // Corrected TI_157, TI_286,TI_197,TI_152,TI_282,TI_144
//#define    GPRSVERSION  0x0064L      // DSP Code 3306 integrated - New API mapping
//#define    GPRSVERSION  0x0065L      // Corrected TI_240 - TI_277 - TI_259 - TI_267 - TI_287
                                       // - TI_271 - TI_284 - TI_299 - TI_300
//#define    GPRSVERSION  0x0066L      // Corrected REQ00570, REQ00682, BUG00565, BUG00588,
                                       // TI_298, REQ00575, BUG00687
//#define    GPRSVERSION  0x0067L      // Corrected TI_294,BUG702,BUG703,BUG652,BUG571
//#define    GPRSVERSION  0x0068L      // Corrected BUG684(TI_213), BUG578, compilation problem
                                       // due to spi.h
//#define    GPRSVERSION  0x0069L      // Corrected TI_206, BUG00649, BUG00548, BUG00559
//#define    GPRSVERSION  0x0070L      // Corrected BUG00814 - Added GPRS version in the new L1 trace
                                       // (merge with SOFTWAREVERSION 1337)
//#define    GPRSVERSION  0x0071L      // Corrected REQ_708, REQ_709, BUG_815
//#define    GPRSVERSION  0x0072L      // New GPRS trace
//#define    GPRSVERSION  0x0073L      // Integrate DSP Patch version 0x2010 for DSP code 0x3311.
                                       // Corrected BUG650, BUG772, BUG802.
//#define    GPRSVERSION  0x0074L      // Port MCU-L1 code to CHIPSET = 5 (G1/13Mhz)
                                       // and CHIPSET = 6 (G1/26Mhz) platform. Associated
                                       // PR corrected are: REQ665, REQ666, REQ667, BUG761,
                                       // BUG762, CHG825, REQ756.
//#define    GPRSVERSION  0x0075L      // Corrected :REQ_826
//#define    GPRSVERSION  0x0076L      // Update EMU_P WS with modification related to
                                       // B-SAMPLE/SAMSON porting.REQ924 & REQ925 corrected
//#define    GPRSVERSION  0x0077L      // Corrected BUG830, BUG1030, BUG1045
//#define    GPRSVERSION  0x0078L      // Corrected BUG979, BUG1062
//#define    GPRSVERSION  0x0079L      // Correctd REQ1046. Added tests run during G1 test campaign.
//#define    GPRSVERSION  0x0080L      // Correctd BUG_0567, BUG_0953, REQ_1024, REQ_1048, BUG_1081
//#define    GPRSVERSION  0x0081L      // Correctd TI_295, REQ00807, CHG01094
//#define    GPRSVERSION  0x0082L      // Corrected TI_141, TI_291, TI_299, CHG580, CHG581, BUG1064,
                                       // BUG1071, BUG1087, BUG1092
//#define    GPRSVERSION  0x0083L      // Corrected REQ1110: L1S CPU load optimization
//#define    GPRSVERSION  0x0084L      // Corrected: REQ526,BUG865, BUG1053, BUG1073,BUG1103
                                       // REQ573,REQ_1119,BUG1050,BUG1114
//#define    GPRSVERSION  0x0085L      // Corrected: REQ763, BUG833, BUG839, BUG855,
                                       // CHG958, BUG1002, BUG1003,
                                       // BUG1004, BUG1041, BUG1043, BUG1052, BUG1080, BUG1101,
                                       // BUG1106, BUG1043
                                       // BUG1143
//#define    GPRSVERSION  0x0086L      // BUG713, BUG853, BUG1052, BUG1088
//#define    GPRSVERSION  0x0087L      // Bug1025,Bug0987,Bug1153,Bug1154,TI_221,REQ1180
                                       // BUG1152,BUG1066,BUG1091,BUG1146
//#define    GPRSVERSION  0x0088L      // REQ1145,BUG1188
//#define    GPRSVERSION  0x0089L      // BUG1197,BUG 1198
//#define    GPRSVERSION  0x0090L      // BUG1200,BUG1201, BUG1202, BUG1203, REQ1204.
                                       // Patch updated: 17,32,33.
//#define    GPRSVERSION  0x0091L      // this release exists in order to avoid confusion between
                                       // 1352_90_404 which do not contain the GRPS 88,89
                                       // and  1352_91_404 which contains GPRS 88,89,90.
//#define    GPRSVERSION  0x0092L      // BUG1211, BUG1212, BUG1216, BUG1226, BUG1208, BUG1222
                                       // New CPU load trace (c.f. REQ1214) enabled
                                       // by the TRACE_TYPE==7.
//#define    GPRSVERSION  0x0093L      // BUG1233,BUG1206,BUG1224,BUG1239,REQ1271
                                       // BUG1240,BUG1256,BUG1254,BUG1257,BUG1262
                                       // BUG1263
//#define    GPRSVERSION  0x0094L      // BUG1302,BUG1275,BUG1280,BUG1274,BUG1273
                                       // BUG1278,BUG1302,BUG1370
//#define    GPRSVERSION  0x0095L      // BUG1279, BUG1281, BUG1317, BUG1331, REQ1374
//#define    GPRSVERSION  0x0096L      // REQ1385,REQ1326,REQ1386,BUG1387,REQ1390
//#define    GPRSVERSION  0x0097L      // Correction of BUG1227, BUG1237, BUG1276, BUG1285, BUG1303, REQ1306
//#define    GPRSVERSION  0x0098L      // Corrected BUG1086
//#define    GPRSVERSION  0x0099L      // Corrected BUG1389
//#define    GPRSVERSION  0x0100L      // Internal RAM using: added configuration LONG_JUMP = 3, modified configuration LONG_JUMP = 2
                                       // Corrected REQ1400
//#define    GPRSVERSION  0x0101L      // Corrected: BUG1395, BUG1396, BUG1397, BUG1404, BUG1406, BUG1408
//#define    GPRSVERSION  0x0102L      // Corrected: BUG1339, BUG1410, BUG1425
//#define    GPRSVERSION  0x0103L      // Corrected: BUG1413, BUG1416, BUG1418, BUG1419, BUG1434, BUG1506, BUG1513, BUG1514, BUG1516, BUG1519, BUG1523, REQ1556
//#define    GPRSVERSION  0x0104L      // Corrected: REQ1575,REQ1536,REQ576,REQ1624
//#define    GPRSVERSION  0x0105L      // Corrected: BUG1428, CHG1502, BUG1518, BUG1532, BUG1652, BUG1718, BUG1549
//#define    GPRSVERSION  0x0106L      // Corrected TI_288, BUG1107, CHG1144, BUG1542, BUG1576, BUG1582, BUG1597, BUG704, BUG1705, BUG1706
//#define    GPRSVERSION  0x0107L      // Corrected CHG1724
//#define    GPRSVERSION  0x0108L      // Corrected REQ1552
//#define    GPRSVERSION  0x0109L      // Corrected CHG1293, BUG1417, BUG1474, BUG1512, BUG1526, BUG1528, BUG1533, BUG1548, BUG1651, CHG1730
                                       // BUG1731, BUG1757, BUG1765, BUG1766, BUG1769, BUG1770
//#define    GPRSVERSION  0x0110L      // Corrected BUG01772
//#define    GPRSVERSION  0x0111L      // Corrected BUG01788, BUG01792
//#define    GPRSVERSION  0x0112L      // Corrected REQ1829 BUG1826
//#define    GPRSVERSION  0x0113L      // Corrected REQ1917, REQ1919
//#define    GPRSVERSION  0x0114L      // Corrected BUG1940,BUG1830,BUG1936,BUG1894,BUG1838,BUG1849
//#define    GPRSVERSION  0x0115L      // Corrected BUG1820, BUG1835, BUG1895, BUG1916, REQ1926, BUG1927, BUG1928, BUG1965
//#define    GPRSVERSION  0x0116L      // Corrected BUG2008, BUG2021, BUG2028, Updated Patch for DSP code 33 (v. 0x20B0)
//#define    GPRSVERSION  0x0117L      // Corrected REQ2051,CHG2033,BUG2022,REQ2061,REQ2062,BUG2068,BUG08051
//#define    GPRSVERSION  0x0118L      // Corrected REQ2123, BUG1707, TI_227
//#define    GPRSVERSION  0x0119L      // Corrected REQ0594
//#define    GPRSVERSION  0x0120L      // Corrected BUG2151,BUG2131,REQ2161
//#define    GPRSVERSION  0x0121L      //  BUG2175, BUG2178, BUG2188, BUG2171
//#define    GPRSVERSION  0x0122L      // Corrected BUG1933, CHG2115, BUG2220, BUG2283, BUG2297
//#define    GPRSVERSION  0x0123L      // REQ2367,BUG2350,BUG2343,BUG2266,BUG2364,BUG2293,BUG2259
                                       // BUG2277,BUG2368,BUG2348,BUG2245,REQ2373
//#define    GPRSVERSION  0x0124L      // Corrected REQ1561, BUG2176
//#define    GPRSVERSION  0x0125L      // Corrected BUG2385, BUG2426
//#define    GPRSVERSION  0x0126L      // Corrected BUG2410, REQ2376
//#define    GPRSVERSION  0x0127L      // Corrected BUG1709, BUG2298, BUG2416, BUG2529
//#define    GPRSVERSION  0x0128L      // Corrected BUG2395
//#define    GPRSVERSION  0x0129L      // Corrected BUG2509,REQ2617,BUG2567mBUG2527,BUG2237,BUG2474,BUG2405,BUG2413
//#define    GPRSVERSION  0x0130L      // Corrected BUG2695, BUG2724
//#define    GPRSVERSION  0x0131L      // Corrected BUG2751
//#define    GPRSVERSION  0x0132L      // Corrected REQ2860, REQ2859,BUG2842
//#define    GPRSVERSION  0x0133L      // Corrected BUG2914, REQ2986
//#define    GPRSVERSION  0x0134L      // Corrected BUG3182
//#define    GPRSVERSION  0x0135L      // Corrected REQ3247
//#define      GPRSVERSION  0x0136L      // REQ03410.
#define GPRSVERSION  0x0137L //CHG2438, BUG2783, BUG3142, BUG3351, BUG3358, BUG3370, BUG3377, BUG3378, BUG3407, BUG3424, CHG3456, BUG3457, CHG3460, BUG3461
