/******************************************************************************
* File Name          : MailboxTask.h
* Date First Issued  : 02/20/2019
* Description        : Incoming CAN msgs to Mailbox
*******************************************************************************/

#ifndef __MAILBOXTASK
#define __MAILBOXTASK

#include "stm32f4xx_hal_def.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "malloc.h"
#include "common_can.h"
#include "can_iface.h"

#define STM32MAXCANNUM 2	// So far just two.
#define MBXARRAYSIZE	32	// Default array size of mailbox pointer array

/* Notification bit assignments for 'MailboxTask' */
// The first three notification bits are reserved for CAN modules 
#define MBXNOTEBITCAN1 (1 << 0)	// Notification bit for CAN1 msgs
#define MBXNOTEBITCAN2 (1 << 1)	// Notification bit for CAN2 msgs
#define MBXNOTEBITCAN3 (1 << 2)	// Notification bit for CAN3 msgs

/* Notification block for linked list */
struct CANNOTIFYLIST
{
	struct CANNOTIFYLIST* pnext; // Points to next; Last points to self
	osThreadId tskhandle;        // Task handle (usually 'MailboxTask')
	uint32_t   notebit;          // Notification bit within task
};

/* Combine variable types for payload readings */
union MAILBOXVALUES
{
	   float   f[2];
	uint32_t i32[2];
	 int32_t s32[2];
   uint16_t i16[4];
	 int16_t s16[4];
    uint8_t  i8[8];
     int8_t  s8[8];
	uint64_t  i64;
};

struct MAILBOXREADINGS
{
	uint8_t pre8[4];
	union MAILBOXVALUES u;
};

/* CAN readings mailbox */
struct MAILBOXCAN
{
	struct CANRCVBUFN ncan;      // CAN msg plus DTW and CAN control block pointer (pctl)
	struct MAILBOXREADINGS mbx;  // Readings extracted from CAN msg
	struct CANNOTIFYLIST* pnote; // Pointer to notification block; NULL = none 
	uint32_t ctr;                // Update counter (increment each update)
	uint8_t paytype;             // Code for payload type
};

struct MAILBOXCANNUM
{
	struct CAN_CTLBLOCK* pctl;     // CAN control block pointer associated with this mailbox list
	struct MAILBOXCAN** pmbxarray; // Point to sorted mailbox pointer array[0]
	struct CANTAKEPTR* ptake;      // "Take" pointer for can_iface circular buffer
	uint32_t notebit;              // Notification bit for this CAN module circular buffer
	uint16_t arraysizemax;         // Mailbox pointer array size that was calloc'd  
	uint16_t arraysizecur;         // Mailbox pointer array populated count
};

/* *************************************************************************/
struct MAILBOXCANNUM* MailboxTask_add_CANlist(struct CAN_CTLBLOCK* pctl, uint16_t arraysize);
/*	@brief	: Add CAN module mailbox list
 * @param	: pctl = Pointer to CAN control block
 * @param	: arraysize = max number of mailboxes in sorted list
 * @return	: Pointer which probably will not be used; NULL = failed (more important)
 * NOTE: This is normally called in 'main' before the FreeRTOS scheduler starts.
 * *************************************************************************/
struct MAILBOXCAN* MailboxTask_add(struct CAN_CTLBLOCK* pctl, uint32_t canid, uint32_t notebit, uint8_t paytype);
/*	@brief	: Add a mailbox
 * @param	: pctl = Pointer to CAN control block, i.e. CAN module/CAN bus, for mailbox
 * @param	: canid = CAN ID
 * @param	: notebit = notification bit; NULL = no notification if tskhandle = NULL
 * @param	: paytype = payload type code (see 'PAYLOAD_TYPE_INSERT.sql' in 'GliderWinchCommons/embed/svn_common/db')
 * @return	: Pointer to mailbox; NULL = failed
 * NOTE: This is normally called from tasks that have started, but are not in the endless loop
 * *************************************************************************/
osThreadId xMailboxTaskCreate(uint32_t taskpriority);
/* @brief	: Create task; task handle created is global for all to enjoy!
 * @param	: taskpriority = Task priority (just as it says!)
 * @return	: QueueHandle_t = queue handle
 * *************************************************************************/
void StartMailboxTask(void const * argument);
/*	@brief	: Task startup
 * *************************************************************************/

extern osThreadId MailboxTaskHandle;
extern struct MAILBOXCANNUM mbxcannum[STM32MAXCANNUM];

#endif

