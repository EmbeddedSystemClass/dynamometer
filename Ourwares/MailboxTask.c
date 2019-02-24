/******************************************************************************
* File Name          : MailboxTask.c
* Date First Issued  : 02/20/2019
* Description        : Incoming CAN msgs to Mailbox
*******************************************************************************/

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include "CanTask.h"
#include "MailboxTask.h"
#include "morse.h"
#include "DTW_counter.h"
#include "payload_extract.h"


#define STM32MAXCANNUM 3	// So far STM32 only has 3 CAN modules
#define MBXARRAYSIZE	32	// Default array size of mailbox pointer array

struct MAILBOXCANNUM
{
	struct CAN_CTLBLOCK* pctl;     // CAN control block pointer associated with this mailbox list
	struct MAILBOXCAN** pmbxarray; // Point to sorted mailbox pointer array[0]
	struct CANTAKEPTR* ptake;      // "Take" pointer for can_iface circular buffer
	uint32_t notebit;              // Notification bit for this CAN module circular buffer
	uint16_t arraysizemax;         // Mailbox pointer array size that was calloc'd  
	uint16_t arraysizecur;         // Mailbox pointer array populated count
};

/* One struct for each CAN module, e.g. CAN 1, 2, 3, ... */
static struct MAILBOXCANNUM mbxcannum[STM32MAXCANNUM] = {0};

osThreadId MailboxTaskHandle; // This wonderful task handle

void StartMailboxTask(void const * argument);
static struct MAILBOXCAN* loadmbx(struct MAILBOXCANNUM* pmbxnum, struct CANRCVBUFN* pncan);

/* *************************************************************************
 * struct MAILBOXCANNUM* MailboxTask_add_CANlist(struct CAN_CTLBLOCK* pctl, uint16_t arraysize);
 *	@brief	: Add CAN module mailbox list
 * @param	: pctl = Pointer to CAN control block
 * @param	: arraysize = max number of mailboxes in sorted list
 * @return	: Pointer which probably will not be used; NULL = failed (more important)
 * NOTE: This is normally called in 'main' before the FreeRTOS scheduler starts.
 * *************************************************************************/
struct MAILBOXCANNUM* MailboxTask_add_CANlist(struct CAN_CTLBLOCK* pctl, uint16_t arraysize)
{
	struct MAILBOXCAN** ppmbxarray; // Pointer to array of pointers to mailboxes

	if (pctl == NULL) return NULL; // Oops

	if (arraysize == 0)
		arraysize = MBXARRAYSIZE;	// Use default size

	/* Max number of mailboxes for this CAN module */
	mbxcannum[pctl->canidx].arraysizemax = arraysize;

	/* This needed to find the CAN module in 'StartMailboxTask' */
	mbxcannum[pctl->canidx].pctl = pctl;

taskENTER_CRITICAL();

	/* Get memory for the array of mailbox pointers */
	ppmbxarray = (struct MAILBOXCAN**)calloc(arraysize, sizeof(struct MAILBOXCAN*));
	if (ppmbxarray == NULL){ taskEXIT_CRITICAL();return NULL;}

	/* Get a circular buffer 'take' pointer for this CAN module. */
	// The first three notification bits are reserved for CAN modules 
	mbxcannum[pctl->canidx].ptake = can_iface_mbx_init(pctl, NULL, (1 << pctl->canidx) );

taskEXIT_CRITICAL();

	/* Save pointer to array of pointers to mailboxes. */
	mbxcannum[pctl->canidx].pmbxarray = ppmbxarray;

	/* Save number of mailbox pointers */
	mbxcannum[pctl->canidx].arraysizemax = arraysize; // Max
	mbxcannum[pctl->canidx].arraysizecur = 0; // Number created

	/* Most important here, is to return a non-NULL pointer. */
	return &mbxcannum[pctl->canidx];
}

/* *************************************************************************
 * struct MAILBOXCAN* MailboxTask_add(struct CAN_CTLBLOCK* pctl, uint32_t canid, uint32_t notebit, uint8_t paytype);
 *	@brief	: Add a mailbox
 * @param	: pctl = Pointer to CAN control block, i.e. CAN module/CAN bus, for mailbox
 * @param	: canid = CAN ID
 * @param	: notebit = notification bit; NULL = no notification if tskhandle = NULL
 * @param	: paytype = payload type code (see 'PAYLOAD_TYPE_INSERT.sql' in 'GliderWinchCommons/embed/svn_common/db')
 * @return	: Pointer to mailbox; NULL = failed
 * NOTE: This is normally called from tasks that have started, but are not in the endless loop
 * *************************************************************************/
struct MAILBOXCAN* MailboxTask_add(struct CAN_CTLBLOCK* pctl, uint32_t canid, uint32_t notebit, uint8_t paytype)
{
	int j;
	struct MAILBOXCAN* pmbx;
	struct CANNOTIFYLIST* pnotex;
	struct CANNOTIFYLIST* pnotetmp;
	struct MAILBOXCAN** ppmbx;

	if (canid == 0)    return NULL;
	if (pctl  == NULL) return NULL;

	/* Pointer to beginning of array of mailbox pointers. */
	ppmbx = mbxcannum[pctl->canidx].pmbxarray;

taskENTER_CRITICAL();

	/* We are working with the array of pointers to mailboxes. */
	// Check if this 'canid' has a mailbox
	for (j = 0; j < mbxcannum[pctl->canidx].arraysizecur; j++)
	{
		pmbx = *(ppmbx+j);  // Get pointer to a mailbox from array of pointers
		if (pmbx == NULL) morse_trap(20); // jic|debug
		if (pmbx->ncan.can.id == canid)
		{ // Here, CAN id already has a mailbox, so a notification must be wanted by this task
			if (notebit != 0)
			{ // Here add a notification to the existing mailbox

				/* Get a notification block. */
				pnotex = (struct CANNOTIFYLIST*)calloc(1, sizeof(struct CANNOTIFYLIST));
				if (pnotex == NULL){ taskEXIT_CRITICAL();return NULL;}

				/* Check if this mailbox has any notifications */
				if (pmbx->pnote == NULL)
				{ // This is the first notification for this mailbox.
					pmbx->pnote = pnotex;   // Mailbox points to first notification
					pnotex->pnext = pnotex;	// Last on list points to self
					pnotex->tskhandle = xTaskGetCurrentTaskHandle();
					pnotex->notebit = notebit;
					taskEXIT_CRITICAL();
					return pmbx;
				}
				else
				{ // Here, one of more notifications.  Add to list.
					/* Seach end of list */
					pnotetmp = pmbx->pnote;
					while (pnotetmp != pnotetmp->pnext) pnotetmp = pnotetmp->pnext;

					/* Add to list and initialize. */
					pnotetmp->pnext = pnotex; // End block now points to new block
					pnotex->pnext   = pnotex; // New block points to self
					pnotex->tskhandle = xTaskGetCurrentTaskHandle();
					pnotex->notebit = notebit;
					taskEXIT_CRITICAL();
					return pmbx;
				}
			}
			/* Here, no notification bit, but CAN id already has a mailbox!
            Either the canid is wrong, or this call was not necessary. */
			taskEXIT_CRITICAL();
			return NULL;
		}
	}

	/* Here, a mailbox for 'canid' was not found in the list. */
	/* Create a mailbox for this canid                        */

	// Point to next available location in array of mailbox pointers. */
	ppmbx = mbxcannum[pctl->canidx].pmbxarray + mbxcannum[pctl->canidx].arraysizecur;

	/* Create one mailbox */
	pmbx = (struct MAILBOXCAN*)calloc(1, sizeof(struct MAILBOXCAN));
	if (pmbx == NULL){ taskEXIT_CRITICAL();return NULL;}

	pmbx->ctr   = 0;       // Redundant (calloc set it zero)
	pmbx->pnote = NULL;    // Redundant (calloc set it zero)
	pmbx->ncan. can.id = canid;   // Save CAN id
	pmbx->ncan.dtw     = DTWTIME; // Set current time count

	if (notebit != 0)
	{ // Here, a notification is requested.  Add first instance of notification  
		pnotex = (struct CANNOTIFYLIST*)calloc(1, sizeof(struct CANNOTIFYLIST));
		if (pnotex == NULL){ taskEXIT_CRITICAL();return NULL;}

		pmbx->pnote   = pnotex; // Mailbox points to first notification
		pnotex->pnext = pnotex;	// Last on list points to self
		pnotex->tskhandle = xTaskGetCurrentTaskHandle();
		pnotex->notebit = notebit;
	}
// TODO: New mailbox w CAN ID so sort pointer array by CAN id here.

taskEXIT_CRITICAL();
	return pmbx;
}

/* *************************************************************************
 * osThreadId xMailboxTaskCreate(uint32_t taskpriority);
 * @brief	: Create task; task handle created is global for all to enjoy!
 * @param	: taskpriority = Task priority (just as it says!)
 * @return	: QueueHandle_t = queue handle
 * *************************************************************************/
osThreadId xMailboxTaskCreate(uint32_t taskpriority)
{
 /* definition and creation of CanTask */
  osThreadDef(MailboxTask, StartMailboxTask, osPriorityNormal, 0, 128);
  MailboxTaskHandle = osThreadCreate(osThread(MailboxTask), NULL);
	vTaskPrioritySet( MailboxTaskHandle, taskpriority );
	return MailboxTaskHandle;
}
/* *************************************************************************
 * void StartMailboxTask(void const * argument);
 *	@brief	: Task startup
 * *************************************************************************/
void StartMailboxTask(void const * argument)
{
	int i;
	struct MAILBOXCANNUM* pmbxnum;
	struct CANRCVBUFN* pncan;
	struct CANTAKEPTR* ptake[STM32MAXCANNUM];

while(1==1) osDelay(10);

	/* Get circular buffer pointers for each CAN module in list. */	
	for (i = 0; i < STM32MAXCANNUM; i++)
	{
		if (mbxcannum[i].pmbxarray != NULL)
		{
					ptake[i] = can_iface_mbx_init(mbxcannum[i].pctl, NULL, (1 << i));
					if (ptake[i] == NULL) morse_trap(22);
		}
	}

	/* A notification copies the internal notification word to this. */
	uint32_t noteval = 0;    // Receives notification word upon an API notify

	/* notification bits processed after a 'Wait. */
	uint32_t noteused = 0;

  /* Infinite RTOS Task loop */
  for(;;)
  {
		/* Wait for a CAN module to load its circular buffer. */
		/* The notification bit identifies the CAN module. */
		xTaskNotifyWait(noteused, 0, &noteval, portMAX_DELAY);
		noteused = 0;	// Accumulate bits in 'noteval' processed.

		/* Step through possible notification bits */
		for (i = 0; i < STM32MAXCANNUM; i++)
		{
			if ((noteval & (1 << i)) != 0)
			{	
				noteused |= (1 << i);
				pmbxnum = &mbxcannum[i]; // Pt to CAN module mailbox control block
				do
				{
					/* Get a pointer to the circular buffer w CAN msgs. */
					pncan = can_iface_get_CANmsg(pmbxnum->ptake);

					if (pncan != NULL)
					{ // Load mailbox. of CANID is in list
						loadmbx(pmbxnum, pncan);
					}
				} while (pncan != NULL);
			}
		}
  }
}
/* *************************************************************************
 * static struct MAILBOXCAN* lookup(struct MAILBOXCANNUM* pmbxnum, struct CANRCVBUFN* pncan);
 *	@brief	: (Bonehead) Lookup CAN ID by a straight pass down the array of mailbox pointers
 * @param	: pmbxnum = pointer to mailbox control block
 * @param	: pncan = pointer to CAN msg in can_face.c circular buffer
 * *************************************************************************/
static struct MAILBOXCAN* lookup(struct MAILBOXCANNUM* pmbxnum, struct CANRCVBUFN* pncan)
{
	struct MAILBOXCAN** ppmbx;
	struct MAILBOXCAN*   pmbx;
	int i;

	ppmbx = pmbxnum->pmbxarray;
	for (i = 0; i < pmbxnum->arraysizecur; i++)
	{
		pmbx = *(ppmbx + i); // Point to mailbox[i]
		if (pmbx->ncan.can.id == pncan->can.id)
		{ // Here, found!
			return pmbx;
		}
	}
	return NULL;
}

/* ************************************************************************* 
 * static struct MAILBOXCAN loadmbx(struct MAILBOXCANNUM* pmbxnum, struct CANRCVBUFN* pncan);
 *	@brief	: Lookup CAN ID and load mailbox with extract payload reading(s)
 * @param	: pmbxnum = pointer to mailbox control block
 * @param	: pncan = pointer to CAN msg in can_face.c circular buffer
 * *************************************************************************/
static struct MAILBOXCAN* loadmbx(struct MAILBOXCANNUM* pmbxnum, struct CANRCVBUFN* pncan)
{
	struct CANNOTIFYLIST* pnotetmp;	
	struct CANNOTIFYLIST* pnotex;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	/* Check if received CAN id is in the mailbox CAN id list. */
	// 'lookup' is a straight loop; use 'lookupq' for binary search
	struct MAILBOXCAN* pmbx = lookup(pmbxnum, pncan);
	if (pmbx == NULL) return NULL; // Return: CAN id not in mailbox list

	/* Here, this CAN msg has a mailbox. */
	// Extract payload
	payload_extract(pmbx, pncan);

	/* Execute notifications */
	pnotex = pmbx->pnote;
	if (pnotex == NULL) return pmbx; // Return: no notifications
	
	// Traverse linked list making notifications
	pnotetmp = pnotex;	
	do 
	{
		xTaskNotifyFromISR(pnotetmp->tskhandle,\
		pnotetmp->notebit, eSetBits,\
		&xHigherPriorityTaskWoken );

		pnotetmp = pnotex->pnext;
		pnotex = pnotex->pnext;
	} while (pnotetmp != pnotetmp->pnext);

	return pmbx;
}



