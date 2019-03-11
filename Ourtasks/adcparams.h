/******************************************************************************
* File Name          : adcparams.h
* Date First Issued  : 03/09/2019
* Board              : DiscoveryF4
* Description        : Parameters for ADC app configuration
*******************************************************************************/

#ifndef __ADCPARAMS
#define __ADCPARAMS

/* ADC reading sequence/array indices                         */
/* These indices MUST match the hardware ADC scan sequence    */
#define ADC1IDX_INTERNALTEMP  0   // Internal temperature sensor
#define ADC1IDX_INTERNALVREF  1   // Internal voltage reference
#define ADC1IDX_HALLLEVER     2 	// Torque control level
#define ADC1IDX_RESISRPOT     3 	// Speed control pot
#define ADC1IDX_CURRENTTOTAL  4   // Current sensor: total battery current
#define ADC1IDX_CURRENTMOTOR1 5   // Current sensor: motor #1
#define ADC1IDX_CURRENTMOTOR2 6   // Current sensor: motor #2

#define ADC1IDX_ADCSCANSIZE  7 // Number ADC channels read

/* Calibration option.                                    */
/* Calibration is applied after compensation adjustments. */
#define ADC1PARAM_CALIBTYPE_RAW   0    // No calibration applied
#define ADC1PARAM_CALIBTYPE_OFSC  1    // Offset & scale (poly ord 0 & 1)
#define ADC1PARAM_CALIBTYPE_POLY  2    // Polynomial 2nd ord
#define ADC1PARAM_CALIBTYPE_POLY  3    // Polynomial 3nd ord

/* Compensation type                                         */
/* Assumes 5v sensor supply is measured with an ADC channel. */
#define ADC1PARAM_COMPTYPE_NONE     0     // No supply or temp compensation applied
#define ADC1PARAM_COMPTYPE_VOLT3A   1     // 3.3v sensor; Vref (absolute) compensation
#define ADC1PARAM_COMPTYPE_VOLT5A   2     // 5v sensor; Vref w 5v supply reading compensation
#define ADC1PARAM_COMPTYPE_VOLT3AT  3     // "3A" above with temperature comp applied to Vref
#define ADC1PARAM_COMPTYPE_VOLT5AT  4     // "5A" above with temperature comp applied to Vref
#define ADC1PARAM_COMPTYPE_VOLT5R   5     // 5v sensor, ratiometric using 5v supply reading compensation 

/* Factory internal calibrations. */
const uint16_t pvref_cal = 


/* DMA summation accumulators. */
struct ADCSUMS
{
	struct ADCSUMSU sums;
	uint16_t maxct;
	uint16_t ct;
}

/* Summations might want to be more than 32b. */
union ADCSUMSU
{
	uint32_t ui[2];
	uint64_t ull;
};

/* This holds calibration values common to all ADC modules. */
struct ADCCALCOMMON
{
	struct FILTERCOMPLETE fcvdd; // Filter for Vdd
	// Calibration for external
	float sensor5vcal;   // The 5v->Vdd divider ratio (e.g. 0.54)
	// Calibration for internals
	float vref;          // Vref: 1.18 min, 1.21 typ, 1.24 max
	float tcoef;         // Vref: Temp coefficient (ppm/deg C: 30 typ; 50 max)
	float fvdd;          // Vdd: float (volts)
	float fvddfilt;      // Vdd: float (volts) filtered
	uint16_t ivdd;       // Vdd: fixed (mv)

	struct FILTERCOMPLETE fctemp; // Filter for Temperature
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

/* ADC parameters: initialized either from 'adcparamsinit.c' or high flash. */
struct ADCPARAM
{
	struct FILTERCOMPLETE fc; // Filter Complete
	uint8_t filttype;   // Type of result filtering
	uint8_t calibtype;  // Calibration type
	uint8_t comptype;   // Compensation type
};

union ADCPARAMWORK
{
	struct FILTERIIRF1 iir_f1;	// Filter block for iir_f1
	// TODO Other filter types to be added
};

struct ADCCHANNELSTUFF
{
	struct ADCPARAM xprms;	// ADC fixed parameters
	struct ADCSUMS adcsum;  // DMA buffering sums
	union ADCPARAMWORK fpw; // ADC filter params and variables
	uint8_t idx;            // ADC reading array index
};

extern struct ADCCHANNELSTUFF adcchannelstuff[ADC1IDX_ADCSCANSIZE];

#endif
