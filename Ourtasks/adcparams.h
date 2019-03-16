/******************************************************************************
* File Name          : adcparams.h
* Date First Issued  : 03/14/2019
* Board              : DiscoveryF4
* Description        : Parameters for ADC app configuration
*******************************************************************************/

#ifndef __ADCPARAMS
#define __ADCPARAMS

#include "iir_f1.h"

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

#define ADC1IDX_ADCSCANSIZE  10 // Number ADC channels read

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

/* Factory internal calibrations. */
const uint16_t pvref_cal = 1.21;

/* Summations might want to be more than 32b. */
union ADCSUMSU
{
	uint32_t ui[2];
	uint64_t ull;
};

/* DMA summation accumulators. */
struct ADCSUMS
{
	union ADCSUMSU sums;
	uint16_t maxct;
	uint16_t ct;
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

	uint16_t ts_cal1;    // Vtemp: TS_CAL1 converted to float ( 30 deg C 3.3v)
	uint16_t ts_cal2;    // Vtemp: TS_CAL2 converted to float (110 deg C 3.3v)
	uint16_t ts_caldiff; // CAL2-CAL1
	float v25;           // Vtemp: 25 deg C (0.76v typ)
   float slope;         // Vtemp: mv/degC 
	float offset;        // Vtemp: offset
 	uint32_t sumct;      // Number of summations of ADC readings before floating
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
	struct ADCSUMS adcsum;   // DMA buffering sums
	union  ADCCALIB cal;     // ADC calibrations
	union  ADCPARAMWORK fpw; // ADC filter params and variables
	uint8_t idx;             // ADC reading array index
};

extern struct ADCCHANNELSTUFF adcchannelstuff[ADC1IDX_ADCSCANSIZE];

#endif
