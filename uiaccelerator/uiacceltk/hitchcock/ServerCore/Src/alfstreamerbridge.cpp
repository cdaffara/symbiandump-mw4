/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Interthread synchronization object
*
*/



#include <e32debug.h>
#include <coemain.h>
#include "alfstreamerbridge.h"
#include "alfstreamerserver.h"
#include "alfbridge.h"
#include "alflogger.h"

#ifdef ALF_DEBUG_TRACK_DRAWING
#include "alfcommanddebug.h"
#endif


// From MMP macro nowadays
//#define ALF_DRAW_FRAME_BEFORE_END_CALLBACK    

// ---------------------------------------------------------------------------
// constructor
// 
// ---------------------------------------------------------------------------
//

// The priority is EActivePriorityWsEvents+1 instead of CActive::EPriorityHigh to prevent a deadlock
// when AlfredServerThread calling iWsSession.GetEvent(event); from inside CCoeEnv::RunL() while
// wserv was waiting inside CWindowServer::CDefaultAnimationScheduler::DoRedrawNow for Alf to
// complete the TRequestStatus passed to CAlfRenderStage::End.

CAlfStreamerBridge::CAlfStreamerBridge(MAlfStreamerListener* aObserver):CActive(EActivePriorityWsEvents+1),iObserver(aObserver), iBatchObserver(0), iThread(RThread().Id())
    {
    CActiveScheduler::Add(this);       
    iStatus = KRequestPending;
    SetActive();
    TInt err = iQueueSema.CreateLocal();
    
    if (err)
        {
        // Device startup ends here.
        __ALFLOGSTRING("Stop the music, we are running without synchronization");
        User::Panic(_L("UI Accelerator"), 1);
        }
    }

// ---------------------------------------------------------------------------
// NewL
// 
// ---------------------------------------------------------------------------
// 
CAlfStreamerBridge* CAlfStreamerBridge::NewL( MAlfStreamerListener* aObserver )
    {
    CAlfStreamerBridge* self = new(ELeave)CAlfStreamerBridge( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ConstructL
// 
// ---------------------------------------------------------------------------
// 
void CAlfStreamerBridge::ConstructL()
    {
    iDataBuf.Create( KInitialVariableBufferSize );
    iDataBuf.SetLength(0);
    iStreamerServer = NULL;
#ifdef ALF_DEBUG_TRACK_DRAWING
    iCommandDebugger = CAlfCommandDebug::NewL();
#endif
    }

// ---------------------------------------------------------------------------
// destructor
// 
// ---------------------------------------------------------------------------
// 
CAlfStreamerBridge::~CAlfStreamerBridge()
    {
    iDataBuf.Close();
    iQueueSema.Close();
#ifdef ALF_DEBUG_TRACK_DRAWING
    delete iCommandDebugger;
#endif
    }

// ---------------------------------------------------------------------------
// SetBatchObserver
// 
// ---------------------------------------------------------------------------
// 
void CAlfStreamerBridge::SetBatchObserver(MAlfBatchObs* aBatchObserver)
    {
    iBatchObserver = aBatchObserver;
    }

// ---------------------------------------------------------------------------
// Activate
// 
// ---------------------------------------------------------------------------
// 
void CAlfStreamerBridge::Activate()
    {
    iStatus = KRequestPending;    
    SetActive();
    }
        
// ---------------------------------------------------------------------------
// Trigger
// 
// ---------------------------------------------------------------------------
// 
TInt CAlfStreamerBridge::Trigger(TInt aStatus)
    {
    TInt err = KErrNone;
    if (!iVarDataAddedButNotPosted)
        {
        iQueueSema.Wait();
        }
    iVarDataAddedButNotPosted = EFalse; // after this point, data may just be lost if posting fails    
    __ALFLOGSTRING2("CAlfStreamerBridge::Trigger status %d, own request status %d", aStatus, iStatus.Int() );
    if (IsActive() && iStatus == KRequestPending)
        {
        RThread t;
        TInt err = t.Open(iThread);
        if (err)
            {
            __ALFLOGSTRING1( "CAlfStreamerBridge::Trigger() ignore RThread::Open() error: %d", err );
            }
        TRequestStatus* status = &iStatus;
        __ALFLOGSTRING("CAlfStreamerBridge::Trigger - completing  CAlfStreamerBridge");
        t.RequestComplete(status, aStatus);
        t.Close();
        }
    else
        {
#ifdef _DEBUG
        if ( IsActive() )
            {
            __ALFLOGSTRING("CAlfStreamerBridge::Trigger - already active");
            }
        __ALFLOGSTRING("CAlfStreamerBridge::Trigger - appending data");
#endif        
        err = iQueue.Append(aStatus);
        }

    iQueueSema.Signal();
    __ALFLOGSTRING1("CAlfStreamerBridge::Trigger request status %d", iStatus.Int() );
    return err;
    }
  
// ---------------------------------------------------------------------------
// AddData
// 
// ---------------------------------------------------------------------------
// 
TInt CAlfStreamerBridge::AddData( TAlfDecoderServerBindings aOp,TInt aI1,TInt aI2 ,TAny* aPtr )
    {
    TInt err = KErrNotFound;
        {    
#ifdef ALF_DEBUG_TRACK_DRAWING    
        iCommandDebugger->SetDescription( aOp, R_ALF_BRIDGE_COMMAND_DESCRIPTION_ARRAY );
        __ALFLOGSTRING2("CAlfStreamerBridge::AddData op %S, iQueue.Length = %d", &iCommandDebugger->Text(), iQueue.Count());
#else
        __ALFLOGSTRING2("CAlfStreamerBridge::AddData op %d, iQueue.Length = %d", aOp, iQueue.Count());    
#endif
        err = KErrNone;
        for(TInt i = 0; i < iMessages.Count(); i++ )
            {
            if (iMessages[i].IsEmpty())
                {
                iMessages[i].Set(aOp,aI1,aI2,aPtr);
                err = Trigger(i);
                return err;
                }
            }
    
        // No existing (empty) cell was available for recycling, add new cell to array
        TAlfBridgerData data;
        data.Set(aOp,aI1,aI2,aPtr);
        err = iMessages.Append(data);
        }
   if (!err) 
        {
        err = Trigger(iMessages.Count()-1);
        }
    else if (iVarDataAddedButNotPosted)
        {
	iVarDataAddedButNotPosted = EFalse; // data lost 
        iQueueSema.Signal();
        }
	    
    return err;
    }

// ---------------------------------------------------------------------------
// GetData
// 
// Note, that this method frees the message entry for reuse.
// ---------------------------------------------------------------------------
// 
TAlfBridgerData CAlfStreamerBridge::GetData(TInt aIndex)
    {
    TAlfBridgerData data;
    ASSERT(aIndex <= iMessages.Count()-1);
    data = iMessages[aIndex];
    return data;    
    }

EXPORT_C TUid CAlfStreamerBridge::FindAppUidForWgId(TInt /*aWgId*/)
    {
    __ASSERT_DEBUG(EFalse, User::Leave(KErrNotSupported));
    return KNullUid;
    }        

EXPORT_C TInt CAlfStreamerBridge::FindWgForAppUid(TUid /*aAppUid*/)
    {
    __ASSERT_DEBUG(EFalse, User::Leave(KErrNotSupported));
    return 0;
    }        
    
// ---------------------------------------------------------------------------
// RunL
// 
// ---------------------------------------------------------------------------
// 
void CAlfStreamerBridge::RunL()
    {
    TInt operation = iMessages[iStatus.Int()].iOp;
    
#ifdef ALF_DEBUG_TRACK_DRAWING 
    iCommandDebugger->SetDescription( operation, R_ALF_BRIDGE_COMMAND_DESCRIPTION_ARRAY );
    __ALFLOGSTRING1("CAlfStreamerBridge::RunL: First op %S", &iCommandDebugger->Text() );
#else
    __ALFLOGSTRING1("CAlfStreamerBridge::RunL: First op %d", operation);
#endif
    switch ( operation )
        {
        case EAlfRequestCommitBatch:
#ifdef ALF_DRAW_FRAME_BEFORE_END_CALLBACK    
            {
            CAlfBridge* bridge = dynamic_cast<CAlfBridge*>(iObserver);
            if (bridge)
                {
                bridge->RefreshNow(iMessages[iStatus.Int()].iInt2);
                }
            } // fall through
#endif
        case EAlfRequestCommandReadNotification:
        case EAlfReleaseTemporaryChunk:
            {
            if (iBatchObserver)
                {
                iBatchObserver->BridgerCallback( operation, iMessages[iStatus.Int()].iInt1 );
                }
            iMessages[iStatus.Int()].ResetEntry();
            break;
            }
        default:
            {
            // Handle command
            __ALFLOGSTRING("CAlfStreamerBridge:: calling observer callback");
            __ASSERT_ALWAYS(iObserver, User::Invariant());
            iObserver->HandleCallback(iStatus.Int());
            
            // For "getters" also call back to Alf decoder thread
            if ((operation > EAlfDSGetCommandsStart) && (operation < EAlfDSGetCommandsEnd))
            	{
            	if (iBatchObserver)
                	{
                	iBatchObserver->BridgerCallback( operation, iMessages[iStatus.Int()].iInt1 );
                	}
            	}
            	
    		iMessages[iStatus.Int()].ResetEntry(); // mark the cell free
            }
        }
    
    // Todo: is this kind of burst mode ok, it should be..
    iQueueSema.Wait();
    while (iQueue.Count())
        {
    	TInt operation2 = iMessages[iQueue[0]].iOp;
#ifdef ALF_DEBUG_TRACK_DRAWING 
    iCommandDebugger->SetDescription( operation2, R_ALF_BRIDGE_COMMAND_DESCRIPTION_ARRAY );
    __ALFLOGSTRING1("CAlfStreamerBridge:: Queue op %S", &iCommandDebugger->Text() );
#else
    __ALFLOGSTRING1("CAlfStreamerBridge:: Queue op %d", operation2 );
#endif
    switch ( operation2 )
            {
            case EAlfRequestCommitBatch:
#ifdef ALF_DRAW_FRAME_BEFORE_END_CALLBACK    
            {
            CAlfBridge* bridge = dynamic_cast<CAlfBridge*>(iObserver);
            if (bridge)
                {
                bridge->RefreshNow(iMessages[iQueue[0]].iInt2);
                }
            } // fall through
#endif
            case EAlfRequestCommandReadNotification:
            case EAlfReleaseTemporaryChunk:
                {
                if (iBatchObserver)
                    {
                    __ALFLOGSTRING("CAlfStreamerBridge:: calling BATCH observer callback");
                    iBatchObserver->BridgerCallback( operation2, iMessages[iQueue[0]].iInt1 );
                    }
                iMessages[iQueue[0]].ResetEntry();
                break;
                }
            default:
                {
	            // Handle command
                __ALFLOGSTRING("CAlfStreamerBridge:: calling observer callback");
                __ASSERT_ALWAYS(iObserver, User::Invariant());
	            iObserver->HandleCallback(iQueue[0]);
	            
	            // For "getters" also call back to Alf decoder thread
	            if ((operation2 > EAlfDSGetCommandsStart) && (operation2 < EAlfDSGetCommandsEnd))
	            	{
	            	if (iBatchObserver)
	                	{
	                	iBatchObserver->BridgerCallback( operation2, iMessages[iQueue[0]].iInt1 );
	                	}
	            	}
	    		iMessages[iQueue[0]].ResetEntry(); // mark the cell free
                }
            }
        iQueue.Remove(0);
        }
    if ( !iItemsInBuffer )
        {
        iDataBuf.SetLength(0);
        }
        
    __ALFLOGSTRING1("CAlfStreamerBridge::RunL - activating", iItemsInBuffer);
    Activate();
    iQueueSema.Signal();
    __ALFLOGSTRING1("CAlfStreamerBridge::RunL - really really ending", iItemsInBuffer);
    }

// ---------------------------------------------------------------------------
// DoCancel
// 
// ---------------------------------------------------------------------------
// 
void CAlfStreamerBridge::DoCancel()
    {
    if (iBatchObserver)
        {
        iBatchObserver->BridgerCanceled();
        }
    }

// ---------------------------------------------------------------------------
// StartNewBlock
// 
// ---------------------------------------------------------------------------
// 
void CAlfStreamerBridge::StartNewBlock(TBool aCompositionModified)
    {
    // Queue marker. Basically we could use one new member to assert that there can
    // be only one marker
    //__ALFLOGSTRING1("CAlfStreamerBridge:: Request command read notification, swap active: %d", iSwapActive );    
    AddData(EAlfRequestCommitBatch,0,aCompositionModified,0);
    }

// ---------------------------------------------------------------------------
// RequestCommandReadNotification
// 
// ---------------------------------------------------------------------------
// 
void CAlfStreamerBridge::RequestCommandReadNotification( TInt aLastReadOffset, TAlfDecoderServerBindings aCommand )
    {
    AddData( aCommand, aLastReadOffset, 0, 0);
    }

// ---------------------------------------------------------------------------
// AppendVarDataL
// 
// Note, that memory allocation is done only if current buffer is not large enough 
// to add an item.
// TODO: The buffers added are not the buffer in optimal way.
// ---------------------------------------------------------------------------
// 
EXPORT_C const TAny* CAlfStreamerBridge::AppendVarDataL( TInt aSize, TInt& aIndex )
    {
    iQueueSema.Wait();
    // Make sure the length of the allocated data is always divisible by 8
    if ( aSize % 8 != 0 )
        {
        __ALFLOGSTRING1("CAlfStreamerBridge::AppendVarDataL, size not multiple of 8: %d", aSize );  
        aSize = aSize + ( 8 - aSize % 8 );
        }
    if ( iDataBuf.Length() + aSize >= iDataBuf.MaxLength() )
        {
        TRAPD(err, iDataBuf.ReAllocL( iDataBuf.MaxLength() + KInitialVariableBufferSize ));
        if ( err )
            {
            __ALFLOGSTRING("CAlfStreamerBridge::AppendVarDataL, realloc failed");
		    iQueueSema.Signal();
            return NULL;
            }
        __ALFLOGSTRING1("CAlfStreamerBridge::AppendVarDataL, new max size: %d", iDataBuf.MaxLength() );  
        }
    aIndex = iDataBuf.Length();
    iDataBuf.SetLength( iDataBuf.Length() + aSize);
    iItemsInBuffer++;
    __ALFLOGSTRING1("CAlfStreamerBridge::AppendVarDataL: buflength %d", iDataBuf.Length());  
    iVarDataAddedButNotPosted = ETrue;    
    return iDataBuf.MidTPtr( aIndex ).Ptr();
    }

// ---------------------------------------------------------------------------
// GetVarDataL
// 
// Note, that data is not removed at any point. However it should not be accessed again 
// with this method, because that would confuse iItemsInBuffer counter.
// ---------------------------------------------------------------------------
// 
const TAny* CAlfStreamerBridge::GetVarDataL( TInt aIndex )
    {
    ASSERT( aIndex >= 0 && aIndex < iDataBuf.MaxLength() );
    ASSERT( iItemsInBuffer );
    iItemsInBuffer--;
    return iDataBuf.MidTPtr( aIndex ).Ptr();
    }

EXPORT_C const TAny* CAlfStreamerBridge::AppendEffectsDataL( TInt aSize, TInt& aIndex )
    {
	return AppendVarDataL( aSize, aIndex );
    }

// ---------------------------------------------------------------------------
// GetVarDataL
// 
// Note, that data is not removed at any point. However it should not be accessed again 
// with this method, because that would confuse iItemsInBuffer counter.
// ---------------------------------------------------------------------------
// 
const TAny* CAlfStreamerBridge::GetEffectsDataL( TInt aIndex )
    {
    return GetVarDataL(aIndex);
    }
