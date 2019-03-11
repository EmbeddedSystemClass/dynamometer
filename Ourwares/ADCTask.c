/******************************************************************************
* File Name          : ADCTask.c
* Date First Issued  : 02/01/2019
* Description        : Processing ADC readings after ADC/DMA issues interrupt
*******************************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "malloc.h"

#include "ADCTask.h"

static int argument1;
void StartADCTask(void* argument);

static uint16_t* adcbuffptr;

osThreadId ADCTaskHandle;

/* *************************************************************************
 * void* xADCTask_init(void);
 * @brief	: Initialize handling of ADC readings
 * @param	: 
 * @return	:
 * *************************************************************************/
	#define TSK02BIT02	(1 << 0)  // Task notification bit for ADC dma 1st 1/2 (adctask.c)
	#define TSK02BIT03	(1 << 1)  // Task notification bit for ADC dma end (adctask.c)

void* xADCTask_init(void)
{


	return 1;
}

/* *************************************************************************
 * osThreadId xADCTaskCreate(uint32_t taskpriority);
 * @brief	: Create task; task handle created is global for all to enjoy!
 * @param	: taskpriority = Task priority (just as it says!)
 * @return	: ADCTaskHandle
 * *************************************************************************/
osThreadId xADCTaskCreate(uint32_t taskpriority)
{
 	osThreadDef(ADCTask, StartSADCTask, osPriorityNormal, 0, 256);
	ADCTaskHandle = osThreadCreate(osThread(MailboxTask), NULL);
	vTaskPrioritySet( ADCTaskHandle, taskpriority );
	return ADCTaskHandle;
}
/* *************************************************************************
 * void StartADCTask(void* argument);
 *	@brief	: Task startup
 * *************************************************************************/
void StartADCTask(void* argument)
{
	/* A notification copies the internal notification word to this. */
	uint32_t noteval = 0;    // Receives notification word upon an API notify

	/* notification bits processed after a 'Wait. */
	uint32_t noteused = 0;

	/* Get buffers, "our" control block, and start ADC/DMA running. */
	struct ADCDMATSKBLK* pblk = adctask_init(&hadc1,TSK02BIT02,TSK02BIT03,&noteval,ADCSEQNUM);
	if (pblk == NULL) {HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15,GPIO_PIN_SET); morse_trap(15);}

	uint64_t* psum;

  /* Infinite loop */
  for(;;)
  {
		/* Wait for DMA interrupt */
		xTaskNotifyWait(noteused, 0, &noteval, portMAX_DELAY);
		noteused = 0;	// Accumulate bits in 'noteval' processed.

		/* We handled one, or both, noteval bits */
		noteused |= (pblk->notebit1 | pblk->notebit2);

		/* Sum the readings 1/2 of DMA buffer to an array. */
		psum = adctask_sum(pblk);	// Sum 1/2 dma buffer 

		adcparams_internal((psum+ADC1IDX_INTERNALTEMP),(psum+ADC1IDX_INTERNALVREF));
		
  }
}



