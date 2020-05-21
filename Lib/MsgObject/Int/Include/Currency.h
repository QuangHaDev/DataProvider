#ifndef __CURRENCY_H__
#define __CURRENCY_H__

#define GET_CURRENCY(iNumber, pCurrencyCode)	\
{ \
	switch ((unsigned long) iNumber) \
	{ \
		 case 4 : \
				sprintf(pCurrencyCode, "%s", " AFA"); \
				break; \
		 case 8 : \
				sprintf(pCurrencyCode, "%s", " ALL"); \
				break; \
		 case 12 : \
				sprintf(pCurrencyCode, "%s", " DZD"); \
				break; \
		 case 20 : \
				sprintf(pCurrencyCode, "%s", " ADP"); \
				break; \
		 case 24 : \
				sprintf(pCurrencyCode, "%s", " AON"); \
				break; \
		 case 32 : \
				sprintf(pCurrencyCode, "%s", " ARS"); \
				break; \
		 case 36 : \
				sprintf(pCurrencyCode, "%s", " AUD"); \
				break; \
		 case 37 : \
				sprintf(pCurrencyCode, "%s", " AUc"); \
				break; \
		 case 40 : \
				sprintf(pCurrencyCode, "%s", " ATS"); \
				break; \
		 case 44 : \
				sprintf(pCurrencyCode, "%s", " BSD"); \
				break; \
		 case 48 : \
				sprintf(pCurrencyCode, "%s", " BHD"); \
				break; \
		 case 50 : \
				sprintf(pCurrencyCode, "%s", " BDT"); \
				break; \
		 case 52 : \
				sprintf(pCurrencyCode, "%s", " BBD"); \
				break; \
		 case 56 : \
				sprintf(pCurrencyCode, "%s", " BEF"); \
				break; \
		 case 60 : \
				sprintf(pCurrencyCode, "%s", " BMD"); \
				break; \
		 case 64 : \
				sprintf(pCurrencyCode, "%s", " BTN"); \
				break; \
		 case 68 : \
				sprintf(pCurrencyCode, "%s", " BOB"); \
				break; \
		 case 70 : \
				sprintf(pCurrencyCode, "%s", " BAD"); \
				break; \
		 case 72 : \
				sprintf(pCurrencyCode, "%s", " BWP"); \
				break; \
		 case 76 : \
				sprintf(pCurrencyCode, "%s", " BRL"); \
				break; \
		 case 84 : \
				sprintf(pCurrencyCode, "%s", " BZD"); \
				break; \
		 case 90 : \
				sprintf(pCurrencyCode, "%s", " SBD"); \
				break; \
		 case 96 : \
				sprintf(pCurrencyCode, "%s", " BND"); \
				break; \
		 case 100 : \
				sprintf(pCurrencyCode, "%s", " BGL"); \
				break; \
		 case 104 : \
				sprintf(pCurrencyCode, "%s", " MMK"); \
				break; \
		 case 108 : \
				sprintf(pCurrencyCode, "%s", " BIF"); \
				break; \
		 case 116 : \
				sprintf(pCurrencyCode, "%s", " KHR"); \
				break; \
		 case 124 : \
				sprintf(pCurrencyCode, "%s", " CAD"); \
				break; \
		 case 132 : \
				sprintf(pCurrencyCode, "%s", " CVE"); \
				break; \
		 case 136 : \
				sprintf(pCurrencyCode, "%s", " KYD"); \
				break; \
		 case 144 : \
				sprintf(pCurrencyCode, "%s", " LKR"); \
				break; \
		 case 152 : \
				sprintf(pCurrencyCode, "%s", " CLP"); \
				break; \
		 case 156 : \
				sprintf(pCurrencyCode, "%s", " CNY"); \
				break; \
		 case 170 : \
				sprintf(pCurrencyCode, "%s", " COP"); \
				break; \
		 case 174 : \
				sprintf(pCurrencyCode, "%s", " KMF"); \
				break; \
		 case 180 : \
				sprintf(pCurrencyCode, "%s", " ZRN"); \
				break; \
		 case 188 : \
				sprintf(pCurrencyCode, "%s", " CRC"); \
				break; \
		 case 191 : \
				sprintf(pCurrencyCode, "%s", " HRD"); \
				break; \
		 case 192 : \
				sprintf(pCurrencyCode, "%s", " CUP"); \
				break; \
		 case 196 : \
				sprintf(pCurrencyCode, "%s", " CYP"); \
				break; \
		 case 200 : \
				sprintf(pCurrencyCode, "%s", " 200"); \
				break; \
		 case 203 : \
				sprintf(pCurrencyCode, "%s", " CZK"); \
				break; \
		 case 208 : \
				sprintf(pCurrencyCode, "%s", " DKK"); \
				break; \
		 case 214 : \
				sprintf(pCurrencyCode, "%s", " DOP"); \
				break; \
		 case 218 : \
				sprintf(pCurrencyCode, "%s", " ECS"); \
				break; \
		 case 222 : \
				sprintf(pCurrencyCode, "%s", " SVC"); \
				break; \
		 case 226 : \
				sprintf(pCurrencyCode, "%s", " EEK"); \
				break; \
		 case 230 : \
				sprintf(pCurrencyCode, "%s", " ETB"); \
				break; \
		 case 238 : \
				sprintf(pCurrencyCode, "%s", " FKP"); \
				break; \
		 case 242 : \
				sprintf(pCurrencyCode, "%s", " FJD"); \
				break; \
		 case 246 : \
				sprintf(pCurrencyCode, "%s", " FIM"); \
				break; \
		 case 250 : \
				sprintf(pCurrencyCode, "%s", " FRF"); \
				break; \
		 case 262 : \
				sprintf(pCurrencyCode, "%s", " DJF"); \
				break; \
		 case 270 : \
				sprintf(pCurrencyCode, "%s", " GMD"); \
				break; \
		 case 280 : \
				sprintf(pCurrencyCode, "%s", " DEM"); \
				break; \
		 case 288 : \
				sprintf(pCurrencyCode, "%s", " GHC"); \
				break; \
		 case 292 : \
				sprintf(pCurrencyCode, "%s", " GIP"); \
				break; \
		 case 300 : \
				sprintf(pCurrencyCode, "%s", " GRD"); \
				break; \
		 case 320 : \
				sprintf(pCurrencyCode, "%s", " GTQ"); \
				break; \
		 case 324 : \
				sprintf(pCurrencyCode, "%s", " GNF"); \
				break; \
		 case 328 : \
				sprintf(pCurrencyCode, "%s", " GYD"); \
				break; \
		 case 332 : \
				sprintf(pCurrencyCode, "%s", " HTG"); \
				break; \
		 case 340 : \
				sprintf(pCurrencyCode, "%s", " HNL"); \
				break; \
		 case 344 : \
				sprintf(pCurrencyCode, "%s", " HKD"); \
				break; \
		 case 345 : \
				sprintf(pCurrencyCode, "%s", " HKc"); \
				break; \
		 case 348 : \
				sprintf(pCurrencyCode, "%s", " HUF"); \
				break; \
		 case 352 : \
				sprintf(pCurrencyCode, "%s", " ISK"); \
				break; \
		 case 356 : \
				sprintf(pCurrencyCode, "%s", " INR"); \
				break; \
		 case 360 : \
				sprintf(pCurrencyCode, "%s", " IDR"); \
				break; \
		 case 364 : \
				sprintf(pCurrencyCode, "%s", " IRR"); \
				break; \
		 case 368 : \
				sprintf(pCurrencyCode, "%s", " IQD"); \
				break; \
		 case 372 : \
				sprintf(pCurrencyCode, "%s", " IEP"); \
				break; \
		 case 373 : \
				sprintf(pCurrencyCode, "%s", " IEp"); \
				break; \
		 case 376 : \
				sprintf(pCurrencyCode, "%s", " ILS"); \
				break; \
		 case 380 : \
				sprintf(pCurrencyCode, "%s", " ITL"); \
				break; \
		 case 388 : \
				sprintf(pCurrencyCode, "%s", " JMD"); \
				break; \
		 case 392 : \
				sprintf(pCurrencyCode, "%s", " JPY"); \
				break; \
		 case 400 : \
				sprintf(pCurrencyCode, "%s", " JOD"); \
				break; \
		 case 404 : \
				sprintf(pCurrencyCode, "%s", " KES"); \
				break; \
		 case 408 : \
				sprintf(pCurrencyCode, "%s", " KPW"); \
				break; \
		 case 410 : \
				sprintf(pCurrencyCode, "%s", " KRW"); \
				break; \
		 case 414 : \
				sprintf(pCurrencyCode, "%s", " KWD"); \
				break; \
		 case 418 : \
				sprintf(pCurrencyCode, "%s", " LAK"); \
				break; \
		 case 422 : \
				sprintf(pCurrencyCode, "%s", " LBP"); \
				break; \
		 case 426 : \
				sprintf(pCurrencyCode, "%s", " LSL"); \
				break; \
		 case 428 : \
				sprintf(pCurrencyCode, "%s", " LVR"); \
				break; \
		 case 430 : \
				sprintf(pCurrencyCode, "%s", " LRD"); \
				break; \
		 case 434 : \
				sprintf(pCurrencyCode, "%s", " LYD"); \
				break; \
		 case 440 : \
				sprintf(pCurrencyCode, "%s", " LTL"); \
				break; \
		 case 442 : \
				sprintf(pCurrencyCode, "%s", " LUF"); \
				break; \
		 case 446 : \
				sprintf(pCurrencyCode, "%s", " MOP"); \
				break; \
		 case 450 : \
				sprintf(pCurrencyCode, "%s", " MGF"); \
				break; \
		 case 454 : \
				sprintf(pCurrencyCode, "%s", " MWK"); \
				break; \
		 case 458 : \
				sprintf(pCurrencyCode, "%s", " MYR"); \
				break; \
		 case 459 : \
				sprintf(pCurrencyCode, "%s", " MYc"); \
				break; \
		 case 462 : \
				sprintf(pCurrencyCode, "%s", " MVR"); \
				break; \
		 case 466 : \
				sprintf(pCurrencyCode, "%s", " MLF"); \
				break; \
		 case 470 : \
				sprintf(pCurrencyCode, "%s", " MTL"); \
				break; \
		 case 478 : \
				sprintf(pCurrencyCode, "%s", " MRO"); \
				break; \
		 case 480 : \
				sprintf(pCurrencyCode, "%s", " MUR"); \
				break; \
		 case 484 : \
				sprintf(pCurrencyCode, "%s", " MXN"); \
				break; \
		 case 496 : \
				sprintf(pCurrencyCode, "%s", " MNT"); \
				break; \
		 case 498 : \
				sprintf(pCurrencyCode, "%s", " MDL"); \
				break; \
		 case 504 : \
				sprintf(pCurrencyCode, "%s", " MAD"); \
				break; \
		 case 508 : \
				sprintf(pCurrencyCode, "%s", " MZM"); \
				break; \
		 case 512 : \
				sprintf(pCurrencyCode, "%s", " OMR"); \
				break; \
		 case 516 : \
				sprintf(pCurrencyCode, "%s", " NAD"); \
				break; \
		 case 524 : \
				sprintf(pCurrencyCode, "%s", " NPR"); \
				break; \
		 case 528 : \
				sprintf(pCurrencyCode, "%s", " NLG"); \
				break; \
		 case 532 : \
				sprintf(pCurrencyCode, "%s", " ANG"); \
				break; \
		 case 533 : \
				sprintf(pCurrencyCode, "%s", " AWG"); \
				break; \
		 case 548 : \
				sprintf(pCurrencyCode, "%s", " VUV"); \
				break; \
		 case 554 : \
				sprintf(pCurrencyCode, "%s", " NZD"); \
				break; \
		 case 558 : \
				sprintf(pCurrencyCode, "%s", " NIO"); \
				break; \
		 case 566 : \
				sprintf(pCurrencyCode, "%s", " NGN"); \
				break; \
		 case 578 : \
				sprintf(pCurrencyCode, "%s", " NOK"); \
				break; \
		 case 586 : \
				sprintf(pCurrencyCode, "%s", " PKR"); \
				break; \
		 case 590 : \
				sprintf(pCurrencyCode, "%s", " PAB"); \
				break; \
		 case 598 : \
				sprintf(pCurrencyCode, "%s", " PGK"); \
				break; \
		 case 600 : \
				sprintf(pCurrencyCode, "%s", " PYG"); \
				break; \
		 case 604 : \
				sprintf(pCurrencyCode, "%s", " PEN"); \
				break; \
		 case 608 : \
				sprintf(pCurrencyCode, "%s", " PHP"); \
				break; \
		 case 616 : \
				sprintf(pCurrencyCode, "%s", " PLZ"); \
				break; \
		 case 620 : \
				sprintf(pCurrencyCode, "%s", " PTE"); \
				break; \
		 case 624 : \
				sprintf(pCurrencyCode, "%s", " GWP"); \
				break; \
		 case 626 : \
				sprintf(pCurrencyCode, "%s", " TPE"); \
				break; \
		 case 634 : \
				sprintf(pCurrencyCode, "%s", " QAR"); \
				break; \
		 case 642 : \
				sprintf(pCurrencyCode, "%s", " ROL"); \
				break; \
		 case 643 : \
				sprintf(pCurrencyCode, "%s", " RUB"); \
				break; \
		 case 646 : \
				sprintf(pCurrencyCode, "%s", " RWF"); \
				break; \
		 case 654 : \
				sprintf(pCurrencyCode, "%s", " SHP"); \
				break; \
		 case 678 : \
				sprintf(pCurrencyCode, "%s", " STD"); \
				break; \
		 case 682 : \
				sprintf(pCurrencyCode, "%s", " SAR"); \
				break; \
		 case 690 : \
				sprintf(pCurrencyCode, "%s", " SCR"); \
				break; \
		 case 694 : \
				sprintf(pCurrencyCode, "%s", " SLL"); \
				break; \
		 case 702 : \
				sprintf(pCurrencyCode, "%s", " SGD"); \
				break; \
		 case 703 : \
				sprintf(pCurrencyCode, "%s", " SKK"); \
				break; \
		 case 704 : \
				sprintf(pCurrencyCode, "%s", " VND"); \
				break; \
		 case 705 : \
				sprintf(pCurrencyCode, "%s", " SIT"); \
				break; \
		 case 706 : \
				sprintf(pCurrencyCode, "%s", " SOS"); \
				break; \
		 case 710 : \
				sprintf(pCurrencyCode, "%s", " ZAR"); \
				break; \
		 case 716 : \
				sprintf(pCurrencyCode, "%s", " ZWD"); \
				break; \
		 case 720 : \
				sprintf(pCurrencyCode, "%s", " 720"); \
				break; \
		 case 724 : \
				sprintf(pCurrencyCode, "%s", " ESP"); \
				break; \
		 case 736 : \
				sprintf(pCurrencyCode, "%s", " SDD"); \
				break; \
		 case 740 : \
				sprintf(pCurrencyCode, "%s", " SRG"); \
				break; \
		 case 748 : \
				sprintf(pCurrencyCode, "%s", " SZL"); \
				break; \
		 case 752 : \
				sprintf(pCurrencyCode, "%s", " SEK"); \
				break; \
		 case 756 : \
				sprintf(pCurrencyCode, "%s", " CHF"); \
				break; \
		 case 760 : \
				sprintf(pCurrencyCode, "%s", " SYP"); \
				break; \
		 case 764 : \
				sprintf(pCurrencyCode, "%s", " THB"); \
				break; \
		 case 776 : \
				sprintf(pCurrencyCode, "%s", " TOP"); \
				break; \
		 case 780 : \
				sprintf(pCurrencyCode, "%s", " TTD"); \
				break; \
		 case 784 : \
				sprintf(pCurrencyCode, "%s", " AED"); \
				break; \
		 case 788 : \
				sprintf(pCurrencyCode, "%s", " TND"); \
				break; \
		 case 792 : \
				sprintf(pCurrencyCode, "%s", " TRL"); \
				break; \
		 case 795 : \
				sprintf(pCurrencyCode, "%s", " TMM"); \
				break; \
		 case 800 : \
				sprintf(pCurrencyCode, "%s", " UGX"); \
				break; \
		 case 804 : \
				sprintf(pCurrencyCode, "%s", " UAK"); \
				break; \
		 case 807 : \
				sprintf(pCurrencyCode, "%s", " MKD"); \
				break; \
		 case 810 : \
				sprintf(pCurrencyCode, "%s", " RUR"); \
				break; \
		 case 818 : \
				sprintf(pCurrencyCode, "%s", " EGP"); \
				break; \
		 case 826 : \
				sprintf(pCurrencyCode, "%s", " GBP"); \
				break; \
		 case 827 : \
				sprintf(pCurrencyCode, "%s", " GBp"); \
				break; \
		 case 834 : \
				sprintf(pCurrencyCode, "%s", " TZS"); \
				break; \
		 case 840 : \
				sprintf(pCurrencyCode, "%s", " USD"); \
				break; \
		 case 841 : \
				sprintf(pCurrencyCode, "%s", " USc"); \
				break; \
		 case 858 : \
				sprintf(pCurrencyCode, "%s", " UYU"); \
				break; \
		 case 862 : \
				sprintf(pCurrencyCode, "%s", " VEB"); \
				break; \
		 case 882 : \
				sprintf(pCurrencyCode, "%s", " WST"); \
				break; \
		 case 886 : \
				sprintf(pCurrencyCode, "%s", " YER"); \
				break; \
		 case 890 : \
				sprintf(pCurrencyCode, "%s", " 890"); \
				break; \
		 case 891 : \
				sprintf(pCurrencyCode, "%s", " YUM"); \
				break; \
		 case 894 : \
				sprintf(pCurrencyCode, "%s", " ZMK"); \
				break; \
		 case 901 : \
				sprintf(pCurrencyCode, "%s", " TWD"); \
				break; \
		 case 950 : \
				sprintf(pCurrencyCode, "%s", " XAF"); \
				break; \
		 case 951 : \
				sprintf(pCurrencyCode, "%s", " XCD"); \
				break; \
		 case 952 : \
				sprintf(pCurrencyCode, "%s", " XOF"); \
				break; \
		 case 953 : \
				sprintf(pCurrencyCode, "%s", " XPF"); \
				break; \
		 case 954 : \
				sprintf(pCurrencyCode, "%s", " XEU"); \
				break; \
		 case 955 : \
				sprintf(pCurrencyCode, "%s", " XBA"); \
				break; \
		 case 956 : \
				sprintf(pCurrencyCode, "%s", " XBB"); \
				break; \
		 case 957 : \
				sprintf(pCurrencyCode, "%s", " XBC"); \
				break; \
		 case 958 : \
				sprintf(pCurrencyCode, "%s", " XBD"); \
				break; \
		 case 959 : \
				sprintf(pCurrencyCode, "%s", " XAU"); \
				break; \
		 case 960 : \
				sprintf(pCurrencyCode, "%s", " XDR"); \
				break; \
		 case 961 : \
				sprintf(pCurrencyCode, "%s", " XAG"); \
				break; \
		 case 962 : \
				sprintf(pCurrencyCode, "%s", " XPT"); \
				break; \
		 case 963 : \
				sprintf(pCurrencyCode, "%s", " XTS"); \
				break; \
		 case 964 : \
				sprintf(pCurrencyCode, "%s", " XPD"); \
				break; \
		 case 978 : \
				sprintf(pCurrencyCode, "%s", " EUR"); \
				break; \
		 case 980 : \
				sprintf(pCurrencyCode, "%s", " UAH"); \
				break; \
		 case 985 : \
				sprintf(pCurrencyCode, "%s", " PLN"); \
				break; \
		 case 990 : \
				sprintf(pCurrencyCode, "%s", " CLF"); \
				break; \
		 case 991 : \
				sprintf(pCurrencyCode, "%s", " ZAL"); \
				break; \
		 case 995 : \
				sprintf(pCurrencyCode, "%s", " ESB"); \
				break; \
		 case 996 : \
				sprintf(pCurrencyCode, "%s", " ESA"); \
				break; \
		 case 997 : \
				sprintf(pCurrencyCode, "%s", " USN"); \
				break; \
		 case 998 : \
				sprintf(pCurrencyCode, "%s", " USS"); \
				break; \
		 case 999 : \
				sprintf(pCurrencyCode, "%s", " XXX"); \
				break; \
		 case 2000 : \
				sprintf(pCurrencyCode, "%s", " tst"); \
				break; \
		 case 2001 : \
				sprintf(pCurrencyCode, "%s", " AUc"); \
				break; \
		 case 2002 : \
				sprintf(pCurrencyCode, "%s", " CAc"); \
				break; \
		 case 2003 : \
				sprintf(pCurrencyCode, "%s", " HKc"); \
				break; \
		 case 2004 : \
				sprintf(pCurrencyCode, "%s", " IEp"); \
				break; \
		 case 2005 : \
				sprintf(pCurrencyCode, "%s", " MYc"); \
				break; \
		 case 2006 : \
				sprintf(pCurrencyCode, "%s", " NZc"); \
				break; \
		 case 2007 : \
				sprintf(pCurrencyCode, "%s", " SGc"); \
				break; \
		 case 2008 : \
				sprintf(pCurrencyCode, "%s", " GBp"); \
				break; \
		 case 2009 : \
				sprintf(pCurrencyCode, "%s", " USc"); \
				break; \
		 case 2010 : \
				sprintf(pCurrencyCode, "%s", " ZAc"); \
				break; \
		 case 2011 : \
				sprintf(pCurrencyCode, "%s", " ZWc"); \
				break; \
		 case 2012 : \
				sprintf(pCurrencyCode, "%s", " EUc"); \
				break; \
		 case 2013 : \
				sprintf(pCurrencyCode, "%s", " NAc"); \
			break; \
		default: \
			sprintf(pCurrencyCode, "%s", ""); \
	} \
}

#endif __CURRENCY_H__