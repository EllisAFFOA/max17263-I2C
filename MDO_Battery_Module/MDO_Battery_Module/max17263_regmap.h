/*
 * max17263_regmap.c
 *
 * Created: 8/11/2023 3:29:43 PM
 *  Author: Ellis Hobby
 */ 


#ifndef MAX17263_REGMAP_H_
#define MAX17263_REGMAP_H_

/***********************************************************/
/***********************************************************
 *
 *
 *            DEVICE CONFIGURATION REGISTERS
 *
 *
 ***********************************************************/
/***********************************************************
 *
 * DesignCap sets the nominal capacity of cell being used
 *
 ***********************************************************/
#define DesignCap_REG_ADDR		0x18
#define DesignCap_DEFAULT		0x0BB8

typedef union {
	struct {
    unsigned cap: 16;
  }bit;
	uint16_t value;	
}__DesignCap_t;

extern volatile __DesignCap_t DesignCap;

/***********************************************************
 *
 * VEmpty sets the thresholds for empty detection
 *
 ***********************************************************/
#define VEmpty_REG_ADDR			0x3A
#define VEmpty_DEFAULT			0xA561

typedef union {
	struct {
    unsigned VR : 7; 
    unsigned VE : 9;
  }bit;
	uint16_t value;	
}__VEmpty_t;

extern volatile __VEmpty_t VEmpty;

/***********************************************************
 *
 * ModelCfg holds basic options for EZ algorithm
 *
 ***********************************************************/
#define ModelCfg_REG_ADDR	  	0xDB
#define ModelCfg_DEFAULT		  0x8400
#define ModelCfg_Refresh  (1 << 15)

typedef union {
	struct {
    unsigned __padding1 : 2; 
    unsigned CSEL       : 1;
    unsigned VSEL       : 1;
    unsigned ModelID    : 4;
    unsigned __padding2 : 2; 
    unsigned VCHG       : 1;
    unsigned __padding3 : 2; 
    unsigned R100       : 1;
    unsigned __padding4 : 1;
    unsigned Refresh    : 1; 
  }bit;
	uint16_t value;	
}__ModelCfg_t;

extern volatile __ModelCfg_t ModelCfg;

/***********************************************************
 *
 * IChgTerm detects when charge termination has occured
 * Program to charge current used in application
 *
 ***********************************************************/
#define IChgTerm_REG_ADDR		0x1E
#define IChgTerm_DEFAULT		0x0640

typedef union {
	struct {
    unsigned term: 16;
  }bit;
	uint16_t value;	
}__IChgTerm_t;

extern volatile __IChgTerm_t IChgTerm;

/***********************************************************
 *
 * LEDCfg1 configures the LED driver
 *
 ***********************************************************/
#define LEDCfg1_REG_ADDR		0x40
#define LEDCfg1_DEFAULT			0x6070

typedef union {
	struct {
    unsigned Nbars		: 4;
    unsigned GrEn		  : 1;
    unsigned LChg		  : 1;
    unsigned LEDMd		: 2;
    unsigned AniMd		: 2;
    unsigned AniStep	: 3;
    unsigned LEDTimer	: 3;		
  }bit;
	uint16_t value;	
}__LEDCfg1_t;

extern volatile __LEDCfg1_t LEDCfg1;

/***********************************************************
 *
 * LEDCfg2 configures the LED driver 
 *
 ***********************************************************/
#define LEDCfg2_REG_ADDR		0x4B
#define LEDCfg2_DEFAULT			0x011F

typedef union {
	struct {
    unsigned Brightness   : 5;
    unsigned FBlink		    : 1;
    unsigned EBlink		    : 1;
    unsigned GBlink		    : 1;
    unsigned EnAutoLEDCnt	: 2;
    unsigned VLED	        : 6;
    unsigned DLED	        : 1;		
  }bit;
	uint16_t value;	
}__LEDCfg2_t;

extern volatile __LEDCfg2_t LEDCfg2;

/***********************************************************
 *
 * LEDCfg3 offers addition LED settings (custom control)
 *
 ***********************************************************/
#define LEDCfg3_REG_ADDR		0x37
#define LEDCfg3_DEFAULT			0x8000

typedef union {
	struct {
    unsigned __padding    : 13; 
    unsigned CustLEDCtrl  : 1;
    unsigned DNC		      : 1;
    unsigned FullSpd		  : 1;	
  }bit;
	uint16_t value;	
}__LEDCfg3_t;

extern volatile __LEDCfg3_t LEDCfg3;
/***********************************************************
 *
 * Custom LED control, LEDs 0-11 = Bits 0-11 respectively
 *
 ***********************************************************/
#define CustLED_REG_ADDR		0x64
#define CustLED_DEFAULT			0x00

/***********************************************************
 *
 * Controls hibernate mode functionality
 *
 ***********************************************************/
#define HibCfg_REG_ADDR		0xBA
#define	ENHIB				      (1 << 15)
#define	HIBENTERTIME_2		(1 << 14)
#define	HIBENTERTIME_1		(1 << 13)
#define	HIBENTERTIME_0		(1 << 12)
#define	HIBTHRESHOLD_3		(1 << 11)
#define	HIBTHRESHOLD_2		(1 << 10)
#define	HIBTHRESHOLD_1		(1 << 9)
#define	HIBTHRESHOLD_0		(1 << 8)
#define	HIBEXITTIME_1		  (1 << 4)
#define	HIBEXITTIME_0		  (1 << 3)
#define	HIBSCALAR_2			  (1 << 2)
#define	HIBSCALAR_1			  (1 << 1)
#define	HIBSCALAR_0			  (1 << 0)

/***********************************************************
 *
 * Used to cear commands and wakeup from hibernate
 *
 ***********************************************************/
#define	SoftWakeup_REG_ADDR		0x60
#define SoftWakeup_CLEAR	    0x0000
#define SoftWakeup_SOFT		    0x0090




/***********************************************************/
/***********************************************************
 *
 *
 *            LEARNED PARAMETERS REGISTERS
 *
 *
 ***********************************************************/
/***********************************************************
 *
 * Holds info for computing open circuit voltage under load
 *
 ***********************************************************/
#define RCOMP0_REG_ADDR			0x38

/***********************************************************
 *
 * Holds temperature info for the RCOMP0 register
 *
 ***********************************************************/
#define TempCo_REG_ADDR			0x39

/***********************************************************
 *
 * The full discharge capacity, continuously calculated
 *
 ***********************************************************/
#define FullCapNom_REG_ADDR		0x23

/***********************************************************
 *
 * Reports full capacity, calculated at end of charge cycle
 *
 ***********************************************************/
#define FullCapRep_REG_ADDR		0x10

/***********************************************************
 *
 * Maintains total number of charge/discharge cycles of cell.
 * Learned parameters should be saved every time bit 6 of
 * Cycles register toggles. (per User Guide 6595)
 *
 ***********************************************************/
#define Cycles_REG_ADDR		0x17
#define Cycles_BIT6			  0x0040




/***********************************************************/
/***********************************************************
 *
 *
 *            FUEL-GAUGE RESULTS REGISTERS
 *
 *
 ***********************************************************/
/***********************************************************
 *
 * Reported remaining capacity register
 *
 ***********************************************************/
#define RepCap_REG_ADDR			0x05

/***********************************************************
 *
 * Reported state-of-charge percentage register
 *
 ***********************************************************/
#define RepSOC_REG_ADDR			0x10

/***********************************************************
 *
 * Estimated time to empty register
 *
 ***********************************************************/
#define TTE_REG_ADDR			0x11

/***********************************************************
 *
 * Estimated time to full register
 *
 ***********************************************************/
#define TTF_REG_ADDR			0x20




/***********************************************************/
/***********************************************************
 *
 *
 *            OPERATING STATUS REGISTERS
 *
 *
 ***********************************************************/
/***********************************************************
 *
 * Holds flags related to alerts such as battery removal
 *
 ***********************************************************/
#define Status_REG_ADDR			0x00
#define Status_DEFAULT			0x8082
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

/***********************************************************
 *
 * Monitors status of m5 algorithm
 *
 ***********************************************************/
#define FStat_REG_ADDR		  0x3D
#define	RELDT	(1 << 9)
#define	EDET	(1 << 8)
#define	FQ		(1 << 7)
#define	RELDT2	(1 << 6)
#define	DNR		(1 << 0)


#endif