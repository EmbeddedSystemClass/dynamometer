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

/* Notification bit assignments for 'MailboxTask' */
#define MBXNOTEBITCAN1 (1 << 0)	// Notification bit for CAN1 msgs
#define MBXNOTEBITCAN2 (1 << 1)	// Notification bit for CAN2 msgs

/* Notification block for linked list */
struct CANNOTIFYLIST
{
	struct CANNOTIFYLIST* pnext; // Points to next; Last points to self
	osThreadId tskhandle;        // Task handle (usually 'MailboxTask')
	uint32_t notebit;            // Notification bit within task
};

struct MAILBOXREADINGS
{
	uint8_t pre8[4];
	union MAILBOXVALUES u;
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

/* CAN readings mailbox */
struct MAILBOXCAN
{
	struct CANRCVBUFN ncan;      // CAN msg plus DTW and CAN control block pointer (pctl)
	union  MAILBOXREADINGS mbx;  // Readings extracted from CAN msg
	struct CANNOTIFYLIST* pnote; // Pointer to notification block; NULL = none 
	uint32_t ctr;                // Update counter (increment each update)
	uint8_t paytype;             // Code for payload type
};

extern osThreadId MailboxTaskHandle;

#endif

