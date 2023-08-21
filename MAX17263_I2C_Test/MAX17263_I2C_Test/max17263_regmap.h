/*
 * max17263_regmap.c
 *
 * Created: 8/11/2023 3:29:43 PM
 *  Author: Ellis Hobby
 */ 


/*
*/
#define DesignCap_ADDR		0x18
#define DesignCap_DEF		0x0BB8

/*
*/
#define Vempty_ADDR			0x3A
#define Vempty_DEF			0xA561

/*
*/
#define ModelCFG_ADDR		0xDB
#define ModelCFG_DEF		0x8400
#define REFRESH				(1 << 15)
#define R100				(1 << 13)
#define VCHG				(1 << 10)
#define ModelID_3			(1 << 7)
#define ModelID_2			(1 << 6)
#define ModelID_1			(1 << 5)
#define ModelID_0			(1 << 4)

/*
*/
#define RCOMP0_ADDR			0x38

/*
*/
#define TempCo_ADDR			0x39

/*
*/
#define FullCapNom_ADDR		0x23


/*
*/
#define IchgTerm_ADDR		0x1E
#define IchgTerm_DEF		0x0640

/*
*/
#define Config_Addr			0x1D
#define Config_DEF			0x2210

/*
*/
#define Config2_ADDR		0xBB
#define Config2_DEF			0x3658

/*
*/
#define RepCap_ADDR			0x05

/*
*/
#define RepSOC_ADDR			0x10

/*
*/
#define FullCapRep_ADDR		0x10

/*
*/
#define TTE_ADDR			0x11

/*
*/
#define TTF_ADDR			0x20

/*
*/
#define Cycles_ADDR			0x17
#define Cycles_B6			0x0040


/*
*/
#define Status_ADDR			0x00
#define Status_DEF			0x8082
#define Br		(1 << 15)
#define Smx		(1 << 14)
#define Tmx		(1 << 13)
#define	Vmx		(1 << 12)
#define	Bi		(1 << 11)
#define	Smn		(1 << 10)
#define	Tmn		(1 << 9)
#define	Vmn		(1 << 8)
#define	dSOCi	(1 << 7)
#define	Imx		(1 << 6)
#define	Bst		(1 << 3)
#define Imn		(1 << 2)
#define	POR		(1 << 1)

/*
*/
#define FStat_ADDR			0x3D
#define	RELDT	(1 << 9)
#define	EDET	(1 << 8)
#define	FQ		(1 << 7)
#define	RELDT2	(1 << 6)
#define	DNR		(1 << 0)

/*
*/
#define HibCFG_ADDR			0xBA
#define	ENHIB				(1 << 15)
#define	HIBENTERTIME_2		(1 << 14)
#define	HIBENTERTIME_1		(1 << 13)
#define	HIBENTERTIME_0		(1 << 12)
#define	HIBTHRESHOLD_3		(1 << 11)
#define	HIBTHRESHOLD_2		(1 << 10)
#define	HIBTHRESHOLD_1		(1 << 9)
#define	HIBTHRESHOLD_0		(1 << 8)
#define	HIBEXITTIME_1		(1 << 4)
#define	HIBEXITTIME_0		(1 << 3)
#define	HIBSCALAR_2			(1 << 2)
#define	HIBSCALAR_1			(1 << 1)
#define	HIBSCALAR_0			(1 << 0)

#define	SoftWakeup_ADDR		0x60
#define SoftWakeup_CLEAR	0x0000
#define SoftWakeup_SOFT		0x0090
		
/*
*/
#define MaxMinVolt_ADDR		0x1B
#define MaxMinVolt_DEF		0x00FF

/*
*/
#define Current_ADDR		0x0A

/*
*/
#define AvgCurrent_ADDR		0x0B

/*
*/
#define MaxMinCurr_ADDR		0x1C
#define MaxMinCurr_DEF		0x807F

/*
*/
#define Temp_ADDR			0x08

/*
*/
#define AvgTA_ADDR			0x16

/*
*/
#define MaxMinTemp_ADDR		0x1A
#define MaxMinTemp_DEF		0x807F

/*
*/
#define DieTemp_ADDR		0x34

/*
*/
#define Power_ADDR			0xB1

/*
*/
#define AvgPower_ADDR		0xB3

/*
*/
#define VAlrtTh_ADDR		0x01
#define VAlrtTh_DEF			0xFF00

/*
*/
#define TAlrtTh_ADDR		0x02
#define TAlrtTh_DEF			0x7F80

/*
*/
#define SAlrtTh_ADDR		0x03
#define SAlrtTh_DEF			0xFF00

/*
*/
#define IAlrtTh_ADDR		0xB4
#define IAlrtTh_DEF			0x7F80

/*
*/
#define MaxPeakPower_ADDR	0xD4

/*
*/
#define SusPeakPower_ADDR	0xD5

/*
*/
#define MPPCurrent_ADDR		0xD9

/*
*/
#define SPPCurrent_ADDR		0xDA

/*
*/
#define AtQResidual_ADDR	0xDC

/*
*/
#define AtTTE_ADDR			0xDD

/*
*/
#define AtAvSOC_ADDR		0xDE

/*
*/
#define AtAvCap_ADDR		0xDF


/*
*/
#define LEDCfg1_ADDR		0x40
#define LEDCfg1_DEF			0x6070
#define LEDTIMER_2	(1 << 15)
#define LEDTIMER_1	(1 << 14)
#define LEDTIMER_0	(1 << 13)
#define ANISTEP_2	(1 << 12)
#define ANISTEP_1	(1 << 11)
#define ANISTEP_0	(1 << 10)
#define ANIMD_1		(1 << 9)
#define ANIMD_0		(1 << 8)
#define LEDMD_1		(1 << 7)
#define LEDMD_0		(1 << 6)
#define LCHG		(1 << 5)
#define GREN		(1 << 4)
#define NBARS_3		(1 << 3)
#define NBARS_2		(1 << 2)
#define NBARS_1		(1 << 1)
#define NBARS_0		(1 << 0)

/*
*/
#define LEDCfg2_ADDR		0x4B
#define LEDCfg2_DEF			0x011F
#define DLED			(1 << 15)
#define VLED_5			(1 << 14)
#define VLED_4			(1 << 13)
#define VLED_3			(1 << 12)
#define VLED_2			(1 << 11)
#define VLED_1			(1 << 10)
#define VLED_0			(1 << 9)
#define ENAUTOLEDCNT	(1 << 8)
#define GBLINK			(1 << 7)
#define EBLINK			(1 << 6)
#define FBLINK			(1 << 5)
#define BRIGHT_4		(1 << 4)
#define BRIGHT_3		(1 << 3)
#define BRIGHT_2		(1 << 2)
#define BRIGHT_1		(1 << 1)
#define BRIGHT_0		(1 << 0)

/*
*/
#define LEDCfg3_ADDR		0x37
#define LEDCfg3_DEF			0x8000
#define FULLSPD		(1 << 15)
#define DNC			(1 << 14)
#define CUSTLEDCTRL	(1 << 13)

/*
*/
#define CustLED_ADDR		0x64
#define CustLED_DEF			0x00

/*
*/
#define MiscCfg_ADDR		0x2B
#define MiscCfg_DEF			0x3870