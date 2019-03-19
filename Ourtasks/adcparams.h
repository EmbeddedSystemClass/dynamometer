/******************************************************************************
* File Name          : adcparams.h
* Date First Issued  : 03/14/2019
* Board              : DiscoveryF4
* Description        : Parameters for ADC app configuration
*******************************************************************************/

#ifndef __ADCPARAMS
#define __ADCPARAMS

#include "iir_f1.h"

#define ADC1DMANUMSEQ        16 // Number of DMA scan sequences in 1/2 DMA buffer
#define ADC1IDX_ADCSCANSIZE  10 // Number ADC channels read

/* ADC reading sequence/array indices                         */
/* These indices -=>MUST<= match the hardware ADC scan sequence    */
#define ADC1IDX_SPARE         0   // PA0 IN0  - spare
#define ADC1IDX_CURRENTTOTAL  1   // PA5 IN5  - Current sensor: total battery current
#define ADC1IDX_CURRENTMOTOR1 2   // PA6 IN6  - Current sensor: motor #1
#define ADC1IDX_CURRENTMOTOR2 3   // PA7 IN7  - Current sensor: motor #2
#define ADC1IDX_RESISRPOT     4 	 // PB0 IN8  - Speed control pot
#define ADC1IDX_HALLLEVER     5 	 // PC1 IN11 - Torque control level
#define ADC1IDX_12VRAWSUPPLY  6   // PC2 IN12 - +12 Raw power to board
#define ADC1IDX_5VOLTSUPPLY   7   // PC5 IN15 - 5V sensor supply
#define ADC1IDX_INTERNALTEMP  8   //     IN17 - Internal temperature sensor
#define ADC1IDX_INTERNALVREF  9   //     IN18 - Internal voltage reference

/* Calibration option.                                    */
/* Calibration is applied after compensation adjustments. */
#define ADC1PARAM_CALIBTYPE_RAW    0    // No calibration applied
#define ADC1PARAM_CALIBTYPE_OFSC   1    // Offset & scale (poly ord 0 & 1)
#define ADC1PARAM_CALIBTYPE_POLY2  2    // Polynomial 2nd ord
#define ADC1PARAM_CALIBTYPE_POLY3  3    // Polynomial 3nd ord

/* Compensation type                                         */
/* Assumes 5v sensor supply is measured with an ADC channel. */
#define ADC1PARAM_COMPTYPE_NONE     0     // No supply or temp compensation applied
#define ADC1PARAM_COMPTYPE_VOLT3A   1     // 3.3v sensor; Vref (absolute) compensation
#define ADC1PARAM_COMPTYPE_VOLT5A   2     // 5v sensor; Vref w 5v supply reading compensation
#define ADC1PARAM_COMPTYPE_VOLT3AT  3     // "3A" above with temperature comp applied to Vref
#define ADC1PARAM_COMPTYPE_VOLT5AT  4     // "5A" above with temperature comp applied to Vref
#define ADC1PARAM_COMPTYPE_VOLT5R   5     // 5v sensor, ratiometric using 5v supply reading compensation 

/* Filter type codes */
#define ADCFILTERTYPE_NONE		0  // Skip filtering
#define ADCFILTERTYPE_IIR1		1  // IIR single pole
#define ADCFILTERTYPE_IIR2		2  // IIR second order

/* Calibrated ADC reading. */
union ADCCALREADING
{
	uint32_t ui;
	 int32_t  n;
	float     f;
};

/* This holds calibration values common to all ADC modules. */
struct ADCCALCOMMON
{
	// Calibration for external
	float sensor5vcal;   // The 5v->Vdd divider ratio (e.g. 0.54)
	// Calibration for internals
	float vref;          // Vref: 1.18 min, 1.21 typ, 1.24 max
	float tcoef;         // Vref: Temp coefficient (ppm/deg C: 30 typ; 50 max)
	float fvdd;          // Vdd: float (volts)
	float fvddfilt;      // Vdd: float (volts) filtered
	uint16_t ivdd;       // Vdd: fixed (mv)
	uint16_t ts_vref;

	float ts_cal1;    // Vtemp: TS_CAL1 converted to float ( 30 deg C 3.3v)
	float ts_cal2;    // Vtemp: TS_CAL2 converted to float (110 deg C 3.3v)
	float ts_caldiff; // CAL2-CAL1
	float ts_80caldiff;
	float v25;           // Vtemp: 25 deg C (0.76v typ)
   float slope;         // Vtemp: mv/degC 
	float offset;        // Vtemp: offset
	float degC;          // Temperature: degrees C
 	uint32_t dmact;      // DMA interrupt running counter
};

struct ADCVTEMPVREF
{
	struct ADCCALCOMMON acc;
	float	vref;         // Latest value of Vref
	float vtemp;        // Latest value of Vtemp
	float degC;         // Temp sensor converted to degC       
	uint64_t u64_vref;  // Summation accumulator: Vref
	uint64_t u64_vtemp; // Summation accumulator: Vtemp
	uint32_t ct;        // Summation counter

};

/* Calibration constants */
#define ADCCALIBSIZE 4 // Number of entries: none - 4th order polynomial
union ADCCALIB
{
	 float    f[ADCCALIBSIZE];
	uint32_t ui[ADCCALIBSIZE];
	 int32_t  n[ADCCALIBSIZE];
};

/* ADC parameters (for one channel): initialized either 
     from 'adcparamsinit.c' or high flash. */
struct ADCPARAM
{
	uint8_t filttype;   // Type of result filtering
	uint8_t calibtype;  // Calibration type
	uint8_t comptype;   // Compensation type
};

/* Intermediate working variables for various filter types. */
union ADCPARAMWORK
{
	struct FILTERIIRF1 iir_f1;	// Filter block for iir_f1
	// TODO Other filter types to be added
};

/* "Everthing" for one ADC channel. */
struct ADCCHANNELSTUFF
{
	struct ADCPARAM xprms;   // ADC fixed parameters
	union  ADCCALIB cal;     // ADC calibrations
	union  ADCPARAMWORK fpw; // ADC filter params and working variables
	uint32_t ctr;            // Update counter
};

/* struct allows pointer to access raw and calibrated ADC1 data. */
struct ADC1DATA
{
  union ADCCALREADING adc1calreading[ADC1IDX_ADCSCANSIZE]; // Calibrated readings
  uint32_t ctr; // Running count of updates.
  uint16_t adcs1sum[ADC1IDX_ADCSCANSIZE]; // Sum of 1/2 DMA buffer for each channel
};

/* *************************************************************************/
void adcparams_init(void);
/*	@brief	: Copy parameters into structs
 * NOTE: => ASSUMES ADC1 ONLY <==
 * *************************************************************************/
void adcparams_internal(struct ADCCALCOMMON* pacom, uint16_t* ptemp, uint16_t* pvref);
/*	@brief	: Update values used for compensation from Vref and Temperature
 * @param	: pacom = Pointer calibration parameters for Temperature and Vref
 * @param	: ptemp = Pointer to summed DMA reading
 * @param	: pvref = Pointer to summed Vref reading
 * *************************************************************************/

/* Raw and calibrated ADC1 readings. */
extern struct ADC1DATA adc1data;

/* Calibration values common to all ADC modules. */
extern struct ADCCALCOMMON adcommon;

#endif
