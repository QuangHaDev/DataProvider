Component   : MsgObject
Version     : 1.6.7.45
=============
Description :
=============
1. Each time after add fid, user must call getMessage to get the latest message byte stream.
   This is only valid for the case of encoding a message object by an empty constructor.

2. User cannot add fid to the msg parser, but msg header information can be changed anytime.

3. For Datatype CharArray, the len field should be the numbers of characters inside the string. it
   doesn't include the "0"(terminator)

4. Please bring along the "testResult" and the "data" folder with the current working directory.
	
	"testResult" - only needed when user want to run the test program for the msgobject.
   	"data" - contains the deftable.txt file, it is used to create the fiddatabase object.

5. If user has their own test applications, please bring along with the "Data" directory to the
   current working directory or simply put it to the C:\NGN2\, otherwise, the test App will
   crash.

=============
Modification:
=============

Date            Version     Author      Description
07 Feb 2001     1.0.0.1     Kevin       Currency correction due to missing 8004 8008 ... 
16 Jan 2002     1.1.0.17    Calvin      Add data type QuoteList, MTCTicks, MTCV and YHeadline
18 Jan 2002     1.1.0.18    Calvin      Add FID_VOL_LAB for Real-time chart
30 May 2002     1.1.0.19    Raymond     Modify the ConstructMsgHeader() to solve the problem of calculating wrong message length
06 Mar 2002     1.1.0.20    Raymond     Add data type ObrkQue & For BBQ, Add fids - 
                                        FID_OSN, FID_OT_PRICE, FID_OUT_QTY, FID_FILLED_QTY, FID_TOTAL_QTY, FID_REJ_QTY,		
                                        FID_TRADE_QTY, FID_REJ_TIME, FID_OT_TIME, FID_OT_DATE, FID_REJ_DATE, FID_QL_01, 
                                        FID_QL_02, FID_QL_03, FID_QL_04, FID_QL_05, FID_QL_06, FID_QL_07, FID_QL_08,
                                        FID_QL_09, FID_QL_10, FID_QL_11, FID_QL_12, FID_QL_13, FID_QL_14, FID_QL_15,
                                        FID_OBBL, FID_OABL
07 May 2002     1.1.0.21    Raymond     Add data type KMultiple & 10KMultiple For PRS, Add fids - 
                                        FID_KTRADE FID_KPRICE FID_KSTRIKE FID_KCALLS_BID FID_KCALLS_BID_QTY FID_KCALLS_ASK
                                        FID_KCALLS_ASK_QTY FID_KCALLS_LAST FID_KCALLS_LAST_VOL FID_KCALLS_HIGH
                                        FID_KCALLS_LOW FID_KCALLS_CUMULATIVE_VOL FID_KPUTS_BID FID_KPUTS_BID_QTY
                                        FID_KPUTS_ASK FID_KPUTS_ASK_QTY FID_KPUTS_LAST FID_KPUTS_LAST_VOL FID_KPUTS_HIGH
                                        FID_KPUTS_LOW FID_KPUTS_CUMULATIVE_VOL FID_KOPEN_INTEREST FID_KHIGH
                                        FID_KLOW FID_KEAS FID_KLAST FID_KLAST_QTY FID_KCUMULATIVE_VOL FID_KPRVEOUS
                                        FID_KOPEN
26 Jul 2002     1.1.0.22    Kevin       Correct the rounding error share value in share order list
9 Sep 2002      1.1.0.23    Kevin       Correct the rounding error share value in share order list course by sprintf
13 Sep 2002     1.1.1.24    John        Add BQueVOSN data type.
                                        Add FID_VOBBL, FID_VOABL.
01 Jan 2003     1.1.2.25    Louisa      Add record type REC_FUTURELIST
19 Mar 2003     1.1.3.26    John        Add FID_SCH_NAME (423), FID_SCH_MSG (424)
27 Mar 2003     1.1.4.27    John        Add FID_XSCALE_LAB (425)
16 Mar 2003     1.1.5.28    John        - Change FID 201-208 from Long to ULong
                                        - Update FID 207-208, 361-368 FID Name for PRS Page
16 Mar 2003     1.1.6.29    John        - FID 201, 202 used already, Change the range to FID 202-210 to ULong.
22 Apr 2003     1.3.0.30    Raymond     Add MTCPTS (D47) data type
                                        (Caution: If the size for D47 is too large, say over 1K byte, 
                                         they may not be able to pass TX for the moment.)
22 Sep 2003     1.3.4.34    John        Add Link FIDs from 1810 to 1819.
02 Oct 2003     1.4.0.35    Raymond     Add MTCF (D48) data type
                                        (Caution: If the size for D48 is too large, say over 1K byte, 
                                         they may not be able to pass TX for the moment.)
4 Jun 2004      1.5.0.37    John        Add Record Type 13 - 18.
26 Jun 2004     1.6.0.38    Vincent     Add Record Type NGN_PRCVOLWSEQ and Fid 1830 to 1832
11 Nov 2004     1.3.1.25    Raymond     FitIterator support data type ProcessPrcVolwSeq , 
										Add fids - 
                                        FID_QL_16, FID_QL_17, FID_QL_18, FID_QL_19, FID_QL_20

23 Nov 2004     1.6.3.41    John        Change NGN_PRCVOLWSEQ from 49 to 50
                                        Reserved 49 for VLPNumber
7 Jan 2005      1.6.4.42    John        Add FID_CHT_DATA 1412 (MtcPoints)
                                        Add REC_WRTLINKDEF (41)
                                        Add REC_WRTEXOTICDEF (42)
15 Feb 2005     1.6.5.43    John        Change FID Name for FID_YL_OSMKTVOL 1307
                                        Add FID_XWRT_TYPE   43
                                        Add FID_SHROFR_PCT  289
                                        Add FID_OS_MKTVOL   380
                                        Add FID_WRT_ISR     426
                                        Add FID_LSTTRD_DATE 1118
11 May 2005     1.6.6.44    Raymond     Add FID_IDN_DELTA   1840
                                        Add fids: FID_REV_UFT11(1841) - FID_REV_UFT59(1889)
7  Jun 2005     1.6.7.45    Raymond     Add fids: FID_REV_PRCVOL1(1890) - FID_REV_DTCLB20(1929)
6  Aug 2005     1.6.8.46    Raymond     Add fids: FID_COUPON_TYPE(1930) - FID_REV_DB19(1959)
31 Oct 2005     1.6.9.47    Raymond     Add REC_STOCKSECTORDEF (44)
										Add REC_WRTSECTORDEF (45)
										Add REC_SHORTSELLDEF (46)
1  Mar 2006     1.6.10.48    Raymond    Add fids: FID_DEAL(1960),FID_DAYS_DELIVER(2000),FID_SETTLE_DATE(2050),FID_OI_DATE(2051)
                                        Add fids: FID_REV_UL21(1961) - FID_REV_UFT59(1999)
                                        Add fids: FID_REV_US11(2001) - FID_REV_US59(2049)
                                        Add fids: FID_REV_DATE5(2052) - FID_REV_DATE52(2099)
                                        Add fids: FID_TRAN_LOG1(2100) - FID_TRAN_LOG10(2109)
                                        Add fids: FID_REV_UCH21(2110) - FID_REV_UCH70(2159)
                                        Add fids: FID_YL_SPARE4(2160) - FID_YL_SPARE23(2179)
                                        Add fids: FID_CHT_SPARE1(2180) - FID_CHT_SPARE20(2199)	
                                        Add fids: FID_MTC_06(2200) - FID_MTC_25(2219)
2  Mar 2006 	1.6.11.49    Raymond    Add fids: FID_REV_MTCPTS1(2220) - FID_REV_MTCPTS20(2239)
28 Apr 2006 	1.6.12.50    Raymond    Add fids: FID_TATXT_PG(471) - FID_TATXT_CH_PG(472)
24 May 2006 	1.6.13.51    Raymond    Add fids: FID_REV_PG18(2240) - FID_REV_PG37(2259)
                                        Add fids: FID_REV_DB20(2260) - FID_REV_DB59(2299)

21 Nov 2006		1.6.14.52	 Lawrence	Purpose: Update for Alert Service Phase I
										Add fids: FID_CUR_WMA50(2300) - FID_REV_UFT135(2399)
										Change FID Name and definition in DefTable:
											1883 to FID_WMA10,
											1884 to FID_WMA50,
											1885 to FID_EMA10,
											1886 to FID_EMA50,
											1887 to FID_CUR_MA10,
											1888 to FID_CUR_MA50,
											1889 to FID_CUR_WMA10,
										Chage FID definition in DefTable for: 1951 - 1955
8 Apr 2008		1.7.0.53	John		Add FID_MTCV_06 (2400) - FID_MTCV_15 (2409)											
										(Sync deftable version with msgobj version) Prevous DefTable.txt version 2.2.12 

11 Nov 2008		1.7.0.54	Jessie		Add REC_TYPE 	REC_SHORTSELLLINK=47
									REC_INDSTRYSCTR	 =48
									REC_TOPRANKINGFF =49
									REC_TRSOURCEDEF	 =50

04 SEP 2009		2.0.0.56	Oliver		Add REC_TYPE	REC_OVERSEAINDEX =53
									REC_COMMODITY	 =54

15 APR 2010		2.0.0.57	Oliver		Update REC_TYPE	REC_SMP_AHPERFORM=53,
							Update REC_TYPE	REC_OVERSEAINDEX =54,
							Update REC_TYPE	REC_COMMODITY	 =55,
							Add REC_TYPE	REC_LPCONTRIBUTOR=56

25 Aug 2011		2.0.1.58	Add FID 2610 - 2619 (ShrOrdLst)
							Add FID 2620 - 2699 (Double)

05 Feb 2013		2.0.2.59	John		Add NGN_TRAN_D
										Add NGN_TXN_D
										Add FID 2700 - 2704 (TranD size 10)
										Add FID 2705 - 2709 (TranD size 12)
										Add FID 2710 - 2714 (TranD size 15)
										Add FID 2715 - 2719 (TranD size 20)
										Add FID 2720 (TxnD)