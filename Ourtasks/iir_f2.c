/******************************************************************************
* File Name          : iir_f2.c
* Date First Issued  : 04/16/2019
* Board              : DiscoveryF4
* Description        : IIR filter: second order, float
*******************************************************************************/

#include "iir_f2.h"

/* *************************************************************************
 * float iir_f2_f(struct FILTERIIRF2* pfc, float flt);
 * @brief	: filter input value 
 * @param	: pfc = Pointer to struct holding fixed parameters and intermediate variables
 * @param	: flt = float new value input to filter
 * @param	: filter output, given new input
 * *************************************************************************/
/*
temp = f_in - a1*state1 - a2*state2
f_out = b0*temp + b1*state1 + b2*state2
state2 = state1
state1 = temp
out = f_out * 0.1
*/
float iir_f2_f(struct FILTERIIRF2* pfc, float flt)
{
	float ftmp;

	if (pfc->skipctr > 0)
	{ // Here, skip starting filter until a few readings
		pfc->skipctr -= 1;
		ftmp = 0;
	}
	else
	{
		ftmp = flt - (pfc->a1 * pfc->z1) - (pfc->a2 * pfc->z2);
		pfc->z2 = pfc->z1;
		pfc->z1 = ftmp;
	}
	return (ftmp + (2 * pfc->z1) + pfc->z2) * pfc->gain;
}
/* *************************************************************************
 * float iir_f2_64b(struct FILTERIIRF2* pfc, uint64_t* pval);
 * @brief	: filter input value 
 * @param	: pfc = Pointer to struct holding fixed parameters and intermediate variables
 * @param	: pval = Pointer to 64b new value input to filter
 * @param	: filter output, given new input
 * *************************************************************************/
float iir_f2_64b(struct FILTERIIRF2* pfc, uint64_t* pval)	
{
	return iir_f2_f( pfc, (float)(*pval) );
}
/* *************************************************************************
 * float iir_f2_32b(struct FILTERIIRF2* pfc, uint32_t val);
 * @brief	: filter input value 
 * @param	: pfc = Pointer to struct holding fixed parameters and intermediate variables
 * @param	: pval = Pointer to 32b new value input to filter
 * @param	: filter output, given new input
 * *************************************************************************/
float iir_f2_32b(struct FILTERIIRF2* pfc, uint32_t val)
{
	return iir_f2_f( pfc, (float)(val) );
}

