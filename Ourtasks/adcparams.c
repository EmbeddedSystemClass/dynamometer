/******************************************************************************
* File Name          : adcparams.c
* Date First Issued  : 03/09/2019
* Board              : DiscoveryF4
* Description        : Parameters for ADC app configuration
*******************************************************************************/
/*
Not thread safe.
*/
#include "adcparams.h"
#include "adcparamsinit.h"


/*
AN3964
https://www.st.com/resource/en/application_note/dm00035957.pdf
V DDA = 3 × Val_V REFINT_CAL ⁄ Val_V REFINT

Temp = 80 ⁄ ( TS_CAL2 – TS_CAL1 ) × ( ValTS – TS_CAL1 ) + 30

The accurate embedded internal reference voltage (V REFINT ) is individually sampled by the
ADC, and the converted value for each device (Val_V REFINT_CAL ) is stored during the
manufacturing process in the protected memory area at address VREFINT_CAL specified
in the product datasheet. The internal reference voltage calibration data is a 12-bit unsigned
number (right-aligned bits, stored in 2 bytes) acquired by the STM32L1x ADC referenced to
V VREF_MEAS = V REF+ = 3V ± 0.01V
The total accuracy of the factory measured calibration data is then provided with an
accuracy of ± 5 mV (refer to the datasheet for more details).
We can determine the actual V DDA voltage by using the formula above as follows:
V DDA = 3 × Val_V REFINT_CAL ⁄ Val_V REFINT
The temperature sensor data, ValTS_bat, are sampled with the ADC scale referenced to the
actual V DDA value determined at the previous steps. Since the temperature sensor factory
calibration data are acquired with the ADC scale set to 3 V, we need to normalize ValTS_bat
to get the temperature sensor data (ValTS) as it would be acquired with ADC scale set to
3 V. ValTS_bat can be normalized by using the formula below:
ValTS = 3 × ValTS_bat ⁄ V DDA
If the ADC is referenced to the 3 V power supply (which is the case of the STM32L1
Discovery) such a normalization is not needed and the sampled temperature data can be
directly used to determine the temperature as described in Section 2.2.1: Temperature
sensor calibration.


Vdd = 3300*(*VREFINT_CAL_ADDR)/ADC_raw;

Temp(degree) = (V_sense - V_25)/Avg_slope + 25

*/

#define VREFINT_CAL_ADDR 

/* Calibration values common to all ADC modules. */
struct ADCCALCOMMON adcommon;

/* ADC1 parameters, calibrations, filtering, ... */
struct ADCCHANNELSTUFF adc1channelstuff[ADC1IDX_ADCSCANSIZE];

/* Raw and calibrated ADC1 readings. */
struct ADC1DATA adc1data;

/* *************************************************************************
 * void adcparams_init(void);
 *	@brief	: Copy parameters into structs
 * NOTE: => ASSUMES ADC1 ONLY <==
 * *************************************************************************/
void adcparams_init(void)
{
	/* Common to board */
	adcparamsinit_init_common(&adcommon);

	/* Load parameter values for ADC channels. */
	adcparamsinit_init(adc1channelstuff);
	return;
}

/* *************************************************************************
 * void adcparams_internal(struct ADCCALCOMMON* pacom, uint16_t* ptemp, uint316_t* pvref);
 *	@brief	: Update values used for compensation from Vref and Temperature
 * @param	: pacom = Pointer calibration parameters for Temperature and Vref
 * @param	: ptemp = Pointer to summed DMA reading
 * @param	: pvref = Pointer to summed Vref reading
 * *************************************************************************/
void adcparams_internal(struct ADCCALCOMMON* pacom, uint16_t* ptemp, uint16_t* pvref)
{
/*
#define PVREFINT_CAL ((uint16_t*)0x1FFF7A2A))  // Pointer to factory calibration: Vref
#define PTS_CAL1     ((uint16_t*)0x1FFF7A2C))  // Pointer to factory calibration: Vtemp
#define PTS_CAL2     ((uint16_t*)0x1FFF7A2E))  // Pointer to factory calibration: Vtemp
*/
	
	pacom->fvdd  = (3.300 * (float)ADC1DMANUMSEQ * (*PVREFINT_CAL)) /  (float)(*pvref);
	
// Temp = 80 ⁄ ( TS_CAL2 – TS_CAL1 ) × ( ValTS – TS_CAL1 ) + 30
	pacom->degC = pacom->ts_80caldiff * (float)(*ptemp) - pacom->ts_cal1 + 30;


	return;
}


