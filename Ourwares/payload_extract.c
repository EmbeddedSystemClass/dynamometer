/******************************************************************************
* File Name          : payload_extract.c
* Date First Issued  : 02/23/2019
* Description        : Extract payload from CAN msg
*******************************************************************************/

#include "MailboxTask.h"
#include "payload_extract.h"

/* NOTE:
If the CAN msg does not have a DLC big enough to accommodate the payload
format designated the mailbox pre8 and union are not updated, nor is the
update counter incremented.  The DTW in the CAN msg will have been updated
when the CAN msg was received.  

Therefore, checking for a stale CAN reading by checking only the DTW could 
result in stale readings not being detected if the DLC reading is wrong.  This
situation would be some sort of software error whereby "someone" is sends a
CAN msg with a bogus CAN id, or payload that is incorrect.
*/

/* ************************************************************************* 
 * static void payload_extract(struct MAILBOXCAN* pmbx, struct CANRCVBUFN* pncan);
 *	@brief	: Lookup CAN ID and load mailbox with extract payload reading(s)
 * @param	: pmbx  = pointer to mailbox
 * @param	: pncan = pointer to CAN msg in can_face.c circular buffer
 * *************************************************************************/
void payload_extract(struct MAILBOXCAN* pmbx, struct CANRCVBUFN* pncan)
{
	switch (pmbx->paytype)
	{
	case FF:
	case U32:
	case S32:
		if (pmbx->ncan.can.dlc >= 4)
		{ // Place 1st four bytes of payload in union
			pbmx->mbx.u.ui[0] = pmbx->ncan.can.cd.ui[0];
			pmbx->ctr +=1 ;
		}
		break;	
	case xFF:
	case xU32:
	case xS32:
		if (pmbx->ncan.can.dlc >= 5)
		{ // Place [1]-[4] of payload in union 
			pbmx->mbx.u.i8[0] = pmbx->ncan.can.cd.cd[1];
			pbmx->mbx.u.i8[1] = pmbx->ncan.can.cd.cd[2];
			pbmx->mbx.u.i8[2] = pmbx->ncan.can.cd.cd[3];
			pbmx->mbx.u.i8[3] = pmbx->ncan.can.cd.cd[4];
			pmbx->ctr +=1 ;
		}
		break;	
	case xxFF:
	case xxU32:
	case xxS32:
		if (pmbx->ncan.can.dlc >= 6)
		{ // Place [2]-[5] of payload in union 
			pbmx->mbx.u.i8[0] = pmbx->ncan.can.cd.cd[2];
			pbmx->mbx.u.i8[1] = pmbx->ncan.can.cd.cd[3];
			pbmx->mbx.u.i8[2] = pmbx->ncan.can.cd.cd[4];
			pbmx->mbx.u.i8[3] = pmbx->ncan.can.cd.cd[5];
			pmbx->ctr +=1 ;
		}
		break;
	case U8_FF:
	case U8_U32:
	case U8_S32:
	case UNIXTIME:
		if (pmbx->ncan.can.dlc >= 5)
		{ 
			pbmx->mbx.pre8[0] = mbx->ncan.can.cd.cd[0];
			// Place [2]-[5] of payload in union 
			pbmx->mbx.u.i8[0] = pmbx->ncan.can.cd.cd[1];
			pbmx->mbx.u.i8[1] = pmbx->ncan.can.cd.cd[2];
			pbmx->mbx.u.i8[2] = pmbx->ncan.can.cd.cd[3];
			pbmx->mbx.u.i8[3] = pmbx->ncan.can.cd.cd[4];
			pmbx->ctr +=1 ;		
		}
		break;	
	case U8_U8_FF:
	case U8_U8_U32:
	case U8_U8_S32:
		if (pmbx->ncan.can.dlc >= 6)
		{ 
			pbmx->mbx.pre8[0] = mbx->ncan.can.cd.cd[0];
			pbmx->mbx.pre8[1] = mbx->ncan.can.cd.cd[1];
			// Place [2]-[5] of payload in union 
			pbmx->mbx.u.i8[0] = pmbx->ncan.can.cd.cd[2];
			pbmx->mbx.u.i8[1] = pmbx->ncan.can.cd.cd[3];
			pbmx->mbx.u.i8[2] = pmbx->ncan.can.cd.cd[4];
			pbmx->mbx.u.i8[3] = pmbx->ncan.can.cd.cd[5];
			pmbx->ctr +=1 ;		
		}
		break;
	case U8_U8_U8_U32:
		if (pmbx->ncan.can.dlc >= 7)
		{ 
			pbmx->mbx.pre8[0] = mbx->ncan.can.cd.cd[0];
			pbmx->mbx.pre8[1] = mbx->ncan.can.cd.cd[1];
			pbmx->mbx.pre8[2] = mbx->ncan.can.cd.cd[2];
			// Place [2]-[5] of payload in union 
			pbmx->mbx.u.i8[0] = pmbx->ncan.can.cd.cd[3];
			pbmx->mbx.u.i8[1] = pmbx->ncan.can.cd.cd[4];
			pbmx->mbx.u.i8[2] = pmbx->ncan.can.cd.cd[5];
			pbmx->mbx.u.i8[3] = pmbx->ncan.can.cd.cd[6];
			pmbx->ctr +=1 ;		
		}
		break;
	case FF_FF:		// Two four byte readings
	case U32_U32:
	case S32_S32:
		if (pmbx->ncan.can.dlc >= 8)
		{ // Place [0]-[7] of payload in union 
			pbmx->mbx.u.i64 = pmbx->ncan.can.cd.ull;
			pmbx->ctr +=1 ;
		}
		break;	

	// Payload type not implemented
	case UNDEF:
	default: 
		{ // Place [0]-[7] of payload in union 
			pbmx->mbx.u.i64 = pmbx->ncan.can.cd.ull;
			pmbx->ctr +=1 ;
		}
		break;	
	}

	/*  Copy struct to update CAN msg */
	pbmx->ncan.can = *pcan; 

	return;
}
