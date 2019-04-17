/******************************************************************************
* File Name          : iir_f2.h
* Date First Issued  : 04/16/2019
* Board              : DiscoveryF4
* Description        : IIR filter: second order, float
*******************************************************************************/

#ifndef __IIR_F2
#define __IIR_F2

#include <stdint.h>

/* With this struct one pointer will convey everything necessary. */
struct FILTERIIRF2
{
	/* coefficients numberator: {b0 b1 b2} = {1 2 1}; */
	float a1;      // coefficient
	float a2;      // coefficient
	float gain;    // gain scale factor
	float z1;      // Z^-1 (state1)
	float z2;      // Z^-1 (state2)
	uint16_t skipctr; // Number of initial readings to not filter
};

/* *************************************************************************/
float iir_f2_f(struct FILTERIIRF2* pfc, float flt);
/* @brief	: filter input value 
 * @param	: pfc = Pointer to struct holding fixed parameters and intermediate variables
 * @param	: flt = float new value input to filter
 * @param	: filter output, given new input
 * *************************************************************************/
float iir_f2_64b(struct FILTERIIRF2* pfc, uint64_t* pval);
/* @brief	: filter input value 
 * @param	: pfc = Pointer to struct holding fixed parameters and intermediate variables
 * @param	: pval = Pointer to 64b new value input to filter
 * @param	: filter output, given new input
 * *************************************************************************/
float iir_f2_32b(struct FILTERIIRF2* pfc, uint32_t val);
/* @brief	: filter input value 
 * @param	: pfc = Pointer to struct holding fixed parameters and intermediate variables
 * @param	: val = 32b new value input to filter
 * @param	: filter output, given new input
 * *************************************************************************/
#endif
