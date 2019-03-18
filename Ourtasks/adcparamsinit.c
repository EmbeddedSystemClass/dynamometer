/******************************************************************************
* File Name          : adcparamsinit.c
* Date First Issued  : 03/09/2019
* Board              : DiscoveryF4
* Description        : Initialization of parameters for ADC app configuration
*******************************************************************************/

/* 
This is where hard-coded parameters for the ADC are entered.

Later, this may be replaced with a "copy" of the flat file in high flash, generated
by the java program from the sql database.
*/

/* *************************************************************************
 * void adcparamsinit_init_common(struct ADCCALCOMMON* padccommon);
 *	@brief	: Initialize struct with parameters common to all ADC for this =>board<=
 * @param	: padccommon = pointer to struct holding parameters
 * *************************************************************************/
void adcparamsinit_init_common(struct ADCCALCOMMON* padccommon)
{
	padccommon->sensor5vcal = 0.54;	// 5v->Vdd divide ratio

	padccommon->ts_vref = *(PVREFINT_CAL); // Factory calibration
	padccommon->tcoef   = 30; // 30 typ, 50 max, (ppm/deg C)

	padccommon->ts_cal1 = *(PTS_CAL1); // Factory calibration
	padccommon->ts_cal2 = *(PTS_CAL2); // Factory calibration
	padccommon->ts_caldiff = *(PTS_CAL2) - *(PTS_CAL1);
	padccommon->v25     = 0.76; // Voltage at 25 °C, typ
	padccommon->slope   = 2.0;  // Average slope (mv/deg C), typ

	return;
}

/* *************************************************************************
 * void adcparamsinit_init(struct ADCCHANNELSTUFF* pacs);
 *	@brief	: Load structs for compensation, calibration and filtering all ADC channels
 * @param	: pacs = Pointer to struct "everything" for this ADC module
 * *************************************************************************/
void adcparamsinit_init(struct ADCCHANNELSTUFF* pacs)
{
	struct ADCCHANNELSTUFF* pacs;

/* Hall effect lever.  5v supply. */
	pacs = &adcchannelstuff[ADC1IDX_HALLLEVER];

	pacs->adcsum.maxct = 0; // Skip additional summing
	pacs->xprms.filttype  = ADCFILTERTYPE_IIR1;        // Single pole IIR
	pacs->xprms.calibtype = ADC1PARAM_CALIBTYPE_OFSC;  // Offset & scale (poly ord 0 & 1)
	pacs->xprms.comptype  = ADC1PARAM_COMPTYPE_VOLT5A; // 5v sensor; Vref w 5v supply reading compensation

	pasc->xprms.cal.[0].f = 0.0     // Offset
	pasc->xprms.cal.[1].f = 0.5465; // Scale (volts) (3.9K - 4.7K)

	pasc->fpw.skipctr  = 4; 	 // Initial readings skip count
	pasc->fpw.coef     = 0.1;   // Filter coefficient (< 1.0)
	pasc->fpw.onemcoef = (1 - pasc->fpw.coef);

/* Resistor pot connected to 3.3v (Vdd) supply. */
	pacs = &adcchannelstuff[ADC1IDX_RESISRPOT];

	pacs->adcsum.maxct = 0; // Skip additional summing
	pacs->xprms.filttype  = ADCFILTERTYPE_IIR1;        // Single pole IIR
	pacs->xprms.calibtype = ADC1PARAM_CALIBTYPE_OFSC;  // Offset & scale (poly ord 0 & 1)
	pacs->xprms.comptype  = ADC1PARAM_COMPTYPE_VOLT3AT; // 5v sensor; Vref abs w temp
	pacs->xprms.coef[0].f = 510.0   //Offset
	pacs->xprms.coef[1].f = 0.123;  //Scale

	pasc->xprms.cal.[0].f = 0.0           // Offset
	pasc->xprms.cal.[1].f = (100.0/4095); // Scale

	pasc->fpw.skipctr  = 4; 	 // Initial readings skip count
	pasc->fpw.coef     = 0.1;   // Filter coefficient (< 1.0)
	pasc->fpw.onemcoef = (1 - pasc->fpw.coef);

/* Total battery current sensor. */
	pacs = &adcchannelstuff[ADC1IDX_CURRENTTOTAL];

	pacs->adcsum.maxct = 0; // Skip additional summing
	pacs->xprms.filttype  = ADCFILTERTYPE_IIR1;        // Single pole IIR
	pacs->xprms.calibtype = ADC1PARAM_CALIBTYPE_OFSC;  // Offset & scale (poly ord 0 & 1)
	pacs->xprms.comptype  = ADC1PARAM_COMPTYPE_VOLT5AT; // 5v w Vref abs w temp

	pasc->xprms.cal.[0].f = 2047.5; // Offset
	pasc->xprms.cal.[1].f = 0.1086; // Scale (200a @saturation)

	pasc->fpw.skipctr  = 4; 	 // Initial readings skip count
	pasc->fpw.coef     = 0.1;   // Filter coefficient (< 1.0)
	pasc->fpw.onemcoef = (1 - pasc->fpw.coef);

/* Current sensor: motor #1 */
	pacs = &adcchannelstuff[ADC1IDX_CURRENTMOTOR1];

	pacs->adcsum.maxct = 0; // Skip additional summing
	pacs->xprms.filttype  = ADCFILTERTYPE_IIR1;        // Single pole IIR
	pacs->xprms.calibtype = ADC1PARAM_CALIBTYPE_OFSC;  // Offset & scale (poly ord 0 & 1)
	pacs->xprms.comptype  = ADC1PARAM_COMPTYPE_VOLT5AT; // 5v w Vref abs w temp

	pasc->xprms.cal.[0].f = 2047.5;  // Offset
	pasc->xprms.cal.[1].f = 0.3257;  // Scale (600a @saturation)

	pasc->fpw.skipctr  = 4; 	 // Initial readings skip count
	pasc->fpw.coef     = 0.1;   // Filter coefficient (< 1.0)
	pasc->fpw.onemcoef = (1 - pasc->fpw.coef);

/* Current sensor: motor #2 */
	pacs = &adcchannelstuff[ADC1IDX_CURRENTMOTOR2];

	pacs->adcsum.maxct = 0; // Skip additional summing
	pacs->xprms.filttype  = ADCFILTERTYPE_IIR1;        // Single pole IIR
	pacs->xprms.calibtype = ADC1PARAM_CALIBTYPE_OFSC;  // Offset & scale (poly ord 0 & 1)
	pacs->xprms.comptype  = ADC1PARAM_COMPTYPE_VOLT5AT; // 5v w Vref abs w temp

	pasc->xprms.cal.[0].f = 2047.5;  // Offset
	pasc->xprms.cal.[1].f = 0.2172;  // Scale (400a @saturation)

	pasc->fpw.skipctr  = 4; 	 // Initial readings skip count
	pasc->fpw.coef     = 0.1;   // Filter coefficient (< 1.0)
	pasc->fpw.onemcoef = (1 - pasc->fpw.coef);

/* +12v supply voltage */
	pacs = &adcchannelstuff[ADC1IDX_12VRAWSUPPLY];

	pacs->adcsum.maxct = 0; // Skip additional summing
	pacs->xprms.filttype  = ADCFILTERTYPE_IIR1;        // Single pole IIR
	pacs->xprms.calibtype = ADC1PARAM_CALIBTYPE_OFSC;  // Offset & scale (poly ord 0 & 1)
	pacs->xprms.comptype  = ADC1PARAM_COMPTYPE_VOLT5AT; // 5v w Vref abs w temp

	pasc->xprms.cal.[0].f = 0.0     // Offset
	pasc->xprms.cal.[1].f = 0.1525; // Scale (volts) (1.8K-10K)

	pasc->fpw.skipctr  = 4; 	 // Initial readings skip count
	pasc->fpw.coef     = 0.1;   // Filter coefficient (< 1.0)
	pasc->fpw.onemcoef = (1 - pasc->fpw.coef);


/* 5v supply. */
	pacs = &adcchannelstuff[ADC1IDX_5VOLTSUPPLY];

	pacs->adcsum.maxct = 0; // Skip additional summing
	pacs->xprms.filttype  = ADCFILTERTYPE_IIR1;        // Single pole IIR
	pacs->xprms.calibtype = ADC1PARAM_CALIBTYPE_OFSC;  // Offset & scale (poly ord 0 & 1)
	pacs->xprms.comptype  = ADC1PARAM_COMPTYPE_VOLT5A; // 5v sensor; Vref w 5v supply reading compensation

	pasc->xprms.cal.[0].f = 0.0     // Offset
	pasc->xprms.cal.[1].f = 0.5465; // Scale (volts) (3.9K - 4.7K)

	pasc->fpw.skipctr  = 4; 	 // Initial readings skip count
	pasc->fpw.coef     = 0.1;   // Filter coefficient (< 1.0)
	pasc->fpw.onemcoef = (1 - pasc->fpw.coef);

	return;
};
