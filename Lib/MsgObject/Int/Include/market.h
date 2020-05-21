#ifndef __MARKET_H__
#define __MARKET_H__

#define GET_MARKET(iNumber, pMarketCode)	\
{ \
	switch ((unsigned char) iNumber) \
	{ \
	case 1: \
		sprintf(pMarketCode, "%s", "ASE"); \
		break; \
	case 2: \
		sprintf(pMarketCode, "%s", "NYS"); \
		break; \
	case 3: \
		sprintf(pMarketCode, "%s", "BOS"); \
		break; \
	case 4: \
		sprintf(pMarketCode, "%s", "CIN"); \
		break; \
	case 5: \
		sprintf(pMarketCode, "%s", "PSE"); \
		break; \
	case 6: \
		sprintf(pMarketCode, "%s", "XPH"); \
		break; \
	case 7: \
		sprintf(pMarketCode, "%s", "THM"); \
		break; \
	case 8: \
		sprintf(pMarketCode, "%s", "MID"); \
		break; \
	case 9: \
		sprintf(pMarketCode, "%s", "NYQ"); \
		break; \
	case 10: \
		sprintf(pMarketCode, "%s", "TOR"); \
		break; \
	case 11: \
		sprintf(pMarketCode, "%s", "MON"); \
		break; \
	case 12: \
		sprintf(pMarketCode, "%s", "VAN"); \
		break; \
	case 13: \
		sprintf(pMarketCode, "%s", "ALB"); \
		break; \
	case 14: \
		sprintf(pMarketCode, "%s", "AOE"); \
		break; \
	case 15: \
		sprintf(pMarketCode, "%s", "NYO"); \
		break; \
	case 16: \
		sprintf(pMarketCode, "%s", "PHO"); \
		break; \
	case 17: \
		sprintf(pMarketCode, "%s", "PAO"); \
		break; \
	case 18: \
		sprintf(pMarketCode, "%s", "WCB"); \
		break; \
	case 19: \
		sprintf(pMarketCode, "%s", "ASQ"); \
		break; \
	case 20: \
		sprintf(pMarketCode, "%s", "CME"); \
		break; \
	case 21: \
		sprintf(pMarketCode, "%s", "IMM"); \
		break; \
	case 22: \
		sprintf(pMarketCode, "%s", "IOM"); \
		break; \
	case 23: \
		sprintf(pMarketCode, "%s", "CBT"); \
		break; \
	case 24: \
		sprintf(pMarketCode, "%s", "MAC"); \
		break; \
	case 25: \
		sprintf(pMarketCode, "%s", "WPG"); \
		break; \
	case 26: \
		sprintf(pMarketCode, "%s", "MGE"); \
		break; \
	case 27: \
		sprintf(pMarketCode, "%s", "PBT"); \
		break; \
	case 28: \
		sprintf(pMarketCode, "%s", "CMX"); \
		break; \
	case 29: \
		sprintf(pMarketCode, "%s", "CSC"); \
		break; \
	case 30: \
		sprintf(pMarketCode, "%s", "NYC"); \
		break; \
	case 31: \
		sprintf(pMarketCode, "%s", "NYM"); \
		break; \
	case 32: \
		sprintf(pMarketCode, "%s", "M32"); \
		break; \
	case 33: \
		sprintf(pMarketCode, "%s", "CSO"); \
		break; \
	case 34: \
		sprintf(pMarketCode, "%s", "CEO"); \
		break; \
	case 35: \
		sprintf(pMarketCode, "%s", "SHC"); \
		break; \
	case 36: \
		sprintf(pMarketCode, "%s", "NYF"); \
		break; \
	case 37: \
		sprintf(pMarketCode, "%s", "TFE"); \
		break; \
	case 38: \
		sprintf(pMarketCode, "%s", "MIO"); \
		break; \
	case 39: \
		sprintf(pMarketCode, "%s", "NMS"); \
		break; \
	case 40: \
		sprintf(pMarketCode, "%s", "TOE"); \
		break; \
	case 41: \
		sprintf(pMarketCode, "%s", "MOE"); \
		break; \
	case 42: \
		sprintf(pMarketCode, "%s", "VAO"); \
		break; \
	case 43: \
		sprintf(pMarketCode, "%s", "NAS"); \
		break; \
	case 44: \
		sprintf(pMarketCode, "%s", "MDM"); \
		break; \
	case 45: \
		sprintf(pMarketCode, "%s", "SCE"); \
		break; \
	case 46: \
		sprintf(pMarketCode, "%s", "BSQ"); \
		break; \
	case 47: \
		sprintf(pMarketCode, "%s", "M47"); \
		break; \
	case 48: \
		sprintf(pMarketCode, "%s", "WCQ"); \
		break; \
	case 49: \
		sprintf(pMarketCode, "%s", "CRC"); \
		break; \
	case 50: \
		sprintf(pMarketCode, "%s", "KBT"); \
		break; \
	case 51: \
		sprintf(pMarketCode, "%s", "SOM"); \
		break; \
	case 52: \
		sprintf(pMarketCode, "%s", "M52"); \
		break; \
	case 53: \
		sprintf(pMarketCode, "%s", "M53"); \
		break; \
	case 54: \
		sprintf(pMarketCode, "%s", "MIF"); \
		break; \
	case 55: \
		sprintf(pMarketCode, "%s", "JSF"); \
		break; \
	case 56: \
		sprintf(pMarketCode, "%s", "TCE"); \
		break; \
	case 57: \
		sprintf(pMarketCode, "%s", "TFF"); \
		break; \
	case 58: \
		sprintf(pMarketCode, "%s", "EME"); \
		break; \
	case 59: \
		sprintf(pMarketCode, "%s", "HOM"); \
		break; \
	case 60: \
		sprintf(pMarketCode, "%s", "FOM"); \
		break; \
	case 61: \
		sprintf(pMarketCode, "%s", "FIR"); \
		break; \
	case 62: \
		sprintf(pMarketCode, "%s", "ATH"); \
		break; \
	case 63: \
		sprintf(pMarketCode, "%s", "SFF"); \
		break; \
	case 64: \
		sprintf(pMarketCode, "%s", "LSE"); \
		break; \
	case 65: \
		sprintf(pMarketCode, "%s", "JNB"); \
		break; \
	case 66: \
		sprintf(pMarketCode, "%s", "LIF"); \
		break; \
	case 67: \
		sprintf(pMarketCode, "%s", "TLV"); \
		break; \
	case 68: \
		sprintf(pMarketCode, "%s", "CPH"); \
		break; \
	case 69: \
		sprintf(pMarketCode, "%s", "OSL"); \
		break; \
	case 70: \
		sprintf(pMarketCode, "%s", "STO"); \
		break; \
	case 71: \
		sprintf(pMarketCode, "%s", "M71"); \
		break; \
	case 72: \
		sprintf(pMarketCode, "%s", "LUX"); \
		break; \
	case 73: \
		sprintf(pMarketCode, "%s", "BRU"); \
		break; \
	case 74: \
		sprintf(pMarketCode, "%s", "PAR"); \
		break; \
	case 75: \
		sprintf(pMarketCode, "%s", "MAD"); \
		break; \
	case 76: \
		sprintf(pMarketCode, "%s", "BAR"); \
		break; \
	case 77: \
		sprintf(pMarketCode, "%s", "AMS"); \
		break; \
	case 78: \
		sprintf(pMarketCode, "%s", "EOE"); \
		break; \
	case 79: \
		sprintf(pMarketCode, "%s", "ATA"); \
		break; \
	case 80: \
		sprintf(pMarketCode, "%s", "VIE"); \
		break; \
	case 81: \
		sprintf(pMarketCode, "%s", "DUS"); \
		break; \
	case 82: \
		sprintf(pMarketCode, "%s", "FRA"); \
		break; \
	case 83: \
		sprintf(pMarketCode, "%s", "HAM"); \
		break; \
	case 84: \
		sprintf(pMarketCode, "%s", "MUN"); \
		break; \
	case 85: \
		sprintf(pMarketCode, "%s", "ZRH"); \
		break; \
	case 86: \
		sprintf(pMarketCode, "%s", "GVA"); \
		break; \
	case 87: \
		sprintf(pMarketCode, "%s", "BSL"); \
		break; \
	case 88: \
		sprintf(pMarketCode, "%s", "MIL"); \
		break; \
	case 89: \
		sprintf(pMarketCode, "%s", "M89"); \
		break; \
	case 90: \
		sprintf(pMarketCode, "%s", "M90"); \
		break; \
	case 91: \
		sprintf(pMarketCode, "%s", "NZE"); \
		break; \
	case 92: \
		sprintf(pMarketCode, "%s", "HKG"); \
		break; \
	case 93: \
		sprintf(pMarketCode, "%s", "SIM"); \
		break; \
	case 94: \
		sprintf(pMarketCode, "%s", "KLC"); \
		break; \
	case 95: \
		sprintf(pMarketCode, "%s", "M95"); \
		break; \
	case 96: \
		sprintf(pMarketCode, "%s", "M96"); \
		break; \
	case 97: \
		sprintf(pMarketCode, "%s", "M97"); \
		break; \
	case 98: \
		sprintf(pMarketCode, "%s", "M98"); \
		break; \
	case 99: \
		sprintf(pMarketCode, "%s", "LME"); \
		break; \
	case 100: \
		sprintf(pMarketCode, "%s", "M100"); \
		break; \
	case 101: \
		sprintf(pMarketCode, "%s", "LCE"); \
		break; \
	case 102: \
		sprintf(pMarketCode, "%s", "M102"); \
		break; \
	case 103: \
		sprintf(pMarketCode, "%s", "M103"); \
		break; \
	case 104: \
		sprintf(pMarketCode, "%s", "M104"); \
		break; \
	case 105: \
		sprintf(pMarketCode, "%s", "SFE"); \
		break; \
	case 106: \
		sprintf(pMarketCode, "%s", "TYO"); \
		break; \
	case 107: \
		sprintf(pMarketCode, "%s", "NGO"); \
		break; \
	case 108: \
		sprintf(pMarketCode, "%s", "SAP"); \
		break; \
	case 109: \
		sprintf(pMarketCode, "%s", "NII"); \
		break; \
	case 110: \
		sprintf(pMarketCode, "%s", "KYO"); \
		break; \
	case 111: \
		sprintf(pMarketCode, "%s", "HIR"); \
		break; \
	case 112: \
		sprintf(pMarketCode, "%s", "FKA"); \
		break; \
	case 113: \
		sprintf(pMarketCode, "%s", "OSA"); \
		break; \
	case 114: \
		sprintf(pMarketCode, "%s", "HFE"); \
		break; \
	case 115: \
		sprintf(pMarketCode, "%s", "BER"); \
		break; \
	case 116: \
		sprintf(pMarketCode, "%s", "HAN"); \
		break; \
	case 117: \
		sprintf(pMarketCode, "%s", "STU"); \
		break; \
	case 118: \
		sprintf(pMarketCode, "%s", "BRE"); \
		break; \
	case 119: \
		sprintf(pMarketCode, "%s", "M119"); \
		break; \
	case 120: \
		sprintf(pMarketCode, "%s", "BRN"); \
		break; \
	case 121: \
		sprintf(pMarketCode, "%s", "M121"); \
		break; \
	case 122: \
		sprintf(pMarketCode, "%s", "M122"); \
		break; \
	case 123: \
		sprintf(pMarketCode, "%s", "M123"); \
		break; \
	case 124: \
		sprintf(pMarketCode, "%s", "ROM"); \
		break; \
	case 125: \
		sprintf(pMarketCode, "%s", "TRN"); \
		break; \
	case 126: \
		sprintf(pMarketCode, "%s", "GOA"); \
		break; \
	case 127: \
		sprintf(pMarketCode, "%s", "NAP"); \
		break; \
	case 128: \
		sprintf(pMarketCode, "%s", "PAL"); \
		break; \
	case 129: \
		sprintf(pMarketCode, "%s", "BLO"); \
		break; \
	case 130: \
		sprintf(pMarketCode, "%s", "VCE"); \
		break; \
	case 131: \
		sprintf(pMarketCode, "%s", "TRI"); \
		break; \
	case 132: \
		sprintf(pMarketCode, "%s", "AQC"); \
		break; \
	case 133: \
		sprintf(pMarketCode, "%s", "HEL"); \
		break; \
	case 134: \
		sprintf(pMarketCode, "%s", "WSE"); \
		break; \
	case 135: \
		sprintf(pMarketCode, "%s", "BOR"); \
		break; \
	case 136: \
		sprintf(pMarketCode, "%s", "LIL"); \
		break; \
	case 137: \
		sprintf(pMarketCode, "%s", "LYO"); \
		break; \
	case 138: \
		sprintf(pMarketCode, "%s", "MRS"); \
		break; \
	case 139: \
		sprintf(pMarketCode, "%s", "NAY"); \
		break; \
	case 140: \
		sprintf(pMarketCode, "%s", "NAN"); \
		break; \
	case 141: \
		sprintf(pMarketCode, "%s", "M141"); \
		break; \
	case 142: \
		sprintf(pMarketCode, "%s", "LAG"); \
		break; \
	case 143: \
		sprintf(pMarketCode, "%s", "ZSE"); \
		break; \
	case 144: \
		sprintf(pMarketCode, "%s", "SHH"); \
		break; \
	case 145: \
		sprintf(pMarketCode, "%s", "BSE"); \
		break; \
	case 146: \
		sprintf(pMarketCode, "%s", "CAL"); \
		break; \
	case 147: \
		sprintf(pMarketCode, "%s", "DES"); \
		break; \
	case 148: \
		sprintf(pMarketCode, "%s", "MDS"); \
		break; \
	case 149: \
		sprintf(pMarketCode, "%s", "JKT"); \
		break; \
	case 150: \
		sprintf(pMarketCode, "%s", "KLS"); \
		break; \
	case 151: \
		sprintf(pMarketCode, "%s", "KAR"); \
		break; \
	case 152: \
		sprintf(pMarketCode, "%s", "MAK"); \
		break; \
	case 153: \
		sprintf(pMarketCode, "%s", "MNL"); \
		break; \
	case 154: \
		sprintf(pMarketCode, "%s", "FTM"); \
		break; \
	case 155: \
		sprintf(pMarketCode, "%s", "SES"); \
		break; \
	case 156: \
		sprintf(pMarketCode, "%s", "KSC"); \
		break; \
	case 157: \
		sprintf(pMarketCode, "%s", "MAU"); \
		break; \
	case 158: \
		sprintf(pMarketCode, "%s", "SET"); \
		break; \
	case 159: \
		sprintf(pMarketCode, "%s", "BAH"); \
		break; \
	case 160: \
		sprintf(pMarketCode, "%s", "CAI"); \
		break; \
	case 161: \
		sprintf(pMarketCode, "%s", "AMM"); \
		break; \
	case 162: \
		sprintf(pMarketCode, "%s", "KUW"); \
		break; \
	case 163: \
		sprintf(pMarketCode, "%s", "BUE"); \
		break; \
	case 164: \
		sprintf(pMarketCode, "%s", "RIO"); \
		break; \
	case 165: \
		sprintf(pMarketCode, "%s", "SAO"); \
		break; \
	case 166: \
		sprintf(pMarketCode, "%s", "SGO"); \
		break; \
	case 167: \
		sprintf(pMarketCode, "%s", "BOG"); \
		break; \
	case 168: \
		sprintf(pMarketCode, "%s", "MEX"); \
		break; \
	case 169: \
		sprintf(pMarketCode, "%s", "CCS"); \
		break; \
	case 170: \
		sprintf(pMarketCode, "%s", "NFE"); \
		break; \
	case 171: \
		sprintf(pMarketCode, "%s", "IND"); \
		break; \
	case 172: \
		sprintf(pMarketCode, "%s", "FIX"); \
		break; \
	case 173: \
		sprintf(pMarketCode, "%s", "LOT"); \
		break; \
	case 174: \
		sprintf(pMarketCode, "%s", "MAT"); \
		break; \
	case 175: \
		sprintf(pMarketCode, "%s", "TAI"); \
		break; \
	case 176: \
		sprintf(pMarketCode, "%s", "IPE"); \
		break; \
	case 177: \
		sprintf(pMarketCode, "%s", "M177"); \
		break; \
	case 178: \
		sprintf(pMarketCode, "%s", "M178"); \
		break; \
	case 179: \
		sprintf(pMarketCode, "%s", "M179"); \
		break; \
	case 180: \
		sprintf(pMarketCode, "%s", "SOP"); \
		break; \
	case 181: \
		sprintf(pMarketCode, "%s", "REU"); \
		break; \
	case 182: \
		sprintf(pMarketCode, "%s", "LIS"); \
		break; \
	case 183: \
		sprintf(pMarketCode, "%s", "OPO"); \
		break; \
	case 184: \
		sprintf(pMarketCode, "%s", "NOM"); \
		break; \
	case 185: \
		sprintf(pMarketCode, "%s", "M185"); \
		break; \
	case 186: \
		sprintf(pMarketCode, "%s", "RTS"); \
		break; \
	case 187: \
		sprintf(pMarketCode, "%s", "ASX"); \
		break; \
	case 188: \
		sprintf(pMarketCode, "%s", "IFX"); \
		break; \
	case 189: \
		sprintf(pMarketCode, "%s", "PmI"); \
		break; \
	case 190: \
		sprintf(pMarketCode, "%s", "MCE"); \
		break; \
	case 191: \
		sprintf(pMarketCode, "%s", "M191"); \
		break; \
	case 192: \
		sprintf(pMarketCode, "%s", "TGE"); \
		break; \
	case 193: \
		sprintf(pMarketCode, "%s", "TSU"); \
		break; \
	case 194: \
		sprintf(pMarketCode, "%s", "KRE"); \
		break; \
	case 195: \
		sprintf(pMarketCode, "%s", "MRV"); \
		break; \
	case 196: \
		sprintf(pMarketCode, "%s", "AOM"); \
		break; \
	case 197: \
		sprintf(pMarketCode, "%s", "BFX"); \
		break; \
	case 198: \
		sprintf(pMarketCode, "%s", "BRT"); \
		break; \
	case 199: \
		sprintf(pMarketCode, "%s", "DTB"); \
		break; \
	case 200: \
		sprintf(pMarketCode, "%s", "MRF"); \
		break; \
	case 201: \
		sprintf(pMarketCode, "%s", "M201"); \
		break; \
	case 202: \
		sprintf(pMarketCode, "%s", "GER"); \
		break; \
	case 203: \
		sprintf(pMarketCode, "%s", "MST"); \
		break; \
	case 204: \
		sprintf(pMarketCode, "%s", "IST"); \
		break; \
	case 205: \
		sprintf(pMarketCode, "%s", "MUS"); \
		break; \
	case 206: \
		sprintf(pMarketCode, "%s", "ABJ"); \
		break; \
	case 207: \
		sprintf(pMarketCode, "%s", "NAI"); \
		break; \
	case 208: \
		sprintf(pMarketCode, "%s", "TUN"); \
		break; \
	case 209: \
		sprintf(pMarketCode, "%s", "FSI"); \
		break; \
	case 210: \
		sprintf(pMarketCode, "%s", "NIN"); \
		break; \
	case 211: \
		sprintf(pMarketCode, "%s", "MCP"); \
		break; \
	case 212: \
		sprintf(pMarketCode, "%s", "OTB"); \
		break; \
	case 213: \
		sprintf(pMarketCode, "%s", "SIC"); \
		break; \
	case 214: \
		sprintf(pMarketCode, "%s", "RCT"); \
		break; \
	case 215: \
		sprintf(pMarketCode, "%s", "IGF"); \
		break; \
	case 216: \
		sprintf(pMarketCode, "%s", "MWQ"); \
		break; \
	case 217: \
		sprintf(pMarketCode, "%s", "PSQ"); \
		break; \
	case 218: \
		sprintf(pMarketCode, "%s", "PHQ"); \
		break; \
	case 219: \
		sprintf(pMarketCode, "%s", "JSD"); \
		break; \
	case 220: \
		sprintf(pMarketCode, "%s", "ECM"); \
		break; \
	case 221: \
		sprintf(pMarketCode, "%s", "OBB"); \
		break; \
	case 222: \
		sprintf(pMarketCode, "%s", "IFM"); \
		break; \
	case 223: \
		sprintf(pMarketCode, "%s", "MFI"); \
		break; \
	case 224: \
		sprintf(pMarketCode, "%s", "SME"); \
		break; \
	case 225: \
		sprintf(pMarketCode, "%s", "SHZ"); \
		break; \
	case 226: \
		sprintf(pMarketCode, "%s", "RSA"); \
		break; \
	case 227: \
		sprintf(pMarketCode, "%s", "BUD"); \
		break; \
	case 228: \
		sprintf(pMarketCode, "%s", "PHS"); \
		break; \
	case 229: \
		sprintf(pMarketCode, "%s", "MLT"); \
		break; \
	case 230: \
		sprintf(pMarketCode, "%s", "M230"); \
		break; \
	case 231: \
		sprintf(pMarketCode, "%s", "SZM"); \
		break; \
	case 232: \
		sprintf(pMarketCode, "%s", "SZF"); \
		break; \
	case 233: \
		sprintf(pMarketCode, "%s", "JBT"); \
		break; \
	case 234: \
		sprintf(pMarketCode, "%s", "MDC"); \
		break; \
	case 235: \
		sprintf(pMarketCode, "%s", "NTC"); \
		break; \
	case 236: \
		sprintf(pMarketCode, "%s", "YSE"); \
		break; \
	case 237: \
		sprintf(pMarketCode, "%s", "OTE"); \
		break; \
	case 238: \
		sprintf(pMarketCode, "%s", "NAK"); \
		break; \
	case 239: \
		sprintf(pMarketCode, "%s", "IOT"); \
		break; \
	case 240: \
		sprintf(pMarketCode, "%s", "TOT"); \
		break; \
	case 241: \
		sprintf(pMarketCode, "%s", "AHM"); \
		break; \
	case 242: \
		sprintf(pMarketCode, "%s", "KLF"); \
		break; \
	case 243: \
		sprintf(pMarketCode, "%s", "CSE"); \
		break; \
	case 244: \
		sprintf(pMarketCode, "%s", "BEC"); \
		break; \
	case 245: \
		sprintf(pMarketCode, "%s", "MXI"); \
		break; \
	case 246: \
		sprintf(pMarketCode, "%s", "ZHC"); \
		break; \
	case 247: \
		sprintf(pMarketCode, "%s", "BJC"); \
		break; \
	case 248: \
		sprintf(pMarketCode, "%s", "WHS"); \
		break; \
	case 249: \
		sprintf(pMarketCode, "%s", "GUF"); \
		break; \
	case 250: \
		sprintf(pMarketCode, "%s", "BMF"); \
		break; \
	case 251: \
		sprintf(pMarketCode, "%s", "VSE"); \
		break; \
	case 252: \
		sprintf(pMarketCode, "%s", "SFX"); \
		break; \
	case 253: \
		sprintf(pMarketCode, "%s", "M253"); \
		break; \
	case 254: \
		sprintf(pMarketCode, "%s", "DLC"); \
		break; \
	case 255: \
		sprintf(pMarketCode, "%s", "CSW"); \
		break; \
	default: \
		sprintf(pMarketCode, "%s", ""); \
		break; \
	} \
}\

#endif __MARKET_H__