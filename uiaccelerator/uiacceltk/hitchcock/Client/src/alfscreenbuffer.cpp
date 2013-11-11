/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Screen buffer implementation
*
*/



#include <coemain.h>

#include "alflogger.h"
#include "alf/alfenv.h"
#include "alfclient.h"
#include "alf/alfstatic.h"
#include "alf/alfconstants.h"
#include "alfuids.h"
#include "alf/alfscreenbuffer.h"
#include "alf/alfsymbiansbdrawer.h"

#include <uiacceltk/HuiUtil.h>

// *** class CAlfScreenBufferEventFetcher

NONSHARABLE_CLASS(CAlfScreenBufferEventFetcher): public CActive
    {
public: // Struct
    
    struct TScreenBufferInfo
    	{
    	MAlfScreenBufferObserver* iObserver; // Observer for this screen buffer. Not owned
    	TUid iBufferUid; // Uid of the buffer to be observed.
    	TInt iPriority;  // Priority of the buffer. Not yet used.
    	TInt iBitmapHandle; // Handle to the shared CFbsBitmap that contains the buffer.
    	TInt iMaskHandle; // Mask handle
    	TRect iLastDisplayRect; // Display rect
    	TRect iLastDirtyRect; // Dirty rect  	
    	};

    
public: // New methods

    CAlfScreenBufferEventFetcher(RAlfClient& aClient, MAlfScreenBufferObserver& aScreenBufferObs)
        :CActive(CActive::EPriorityHigh), iClient(aClient), iEventAsDescriptor(iEvent)        
        {
        CAlfScreenBufferEventFetcher::TScreenBufferInfo info = {&aScreenBufferObs, KNullUid, 0, 0, 0, TRect(), TRect()};
        iBufferInfo = info;
        CActiveScheduler::Add(this);
        }

    ~CAlfScreenBufferEventFetcher()
        {
        Cancel();
        }

	// Request a new event
	void RequestBufferEvent()       
        {
        if(!IsActive())
            {
            SetActive();
        	iEvent.iBufferUid = iBufferInfo.iBufferUid;
        	iEvent.iEventId = 0;
        	iEvent.iDisplayRect = TRect();
        	iEvent.iDirtyRect = TRect();
    		iClient.SendAsyncCmd(EAlfSBufRequestEvent, iEventAsDescriptor, iStatus); 
            }
        }
        
    // Return buffer info    
    TScreenBufferInfo& ScreenBufferInfo()
    	{
    	return(iBufferInfo);
    	}

private: // From CActive
    
    void RunL()
        {
        if(iStatus.Int() != KErrNone)
           	{ 
           	USER_INVARIANT();            	
           	}
          	
        // Issue a new event request first             
        TAlfScreenBufferEvent event = iEvent;
		RequestBufferEvent();   
		
		// Handle event
		
		if (event.iEventId == MAlfScreenBufferObserver::ENone) // Buffer complete event    
			{
        	if (iBufferInfo.iObserver->BufferComplete(event.iBufferUid, event.iDisplayRect, event.iDirtyRect))
        		{
        		// Request next  buffer automatically
   			    TUid uidEvent = event.iBufferUid; 
    			TPckg<TUid> eventPckg(uidEvent);
    			iClient.SendSyncCmd(EAlfSBufRequestNextBuffer, eventPckg);
        		}
			}
		else     
			{
			if (event.iEventId == MAlfScreenBufferObserver::ECreated)
				{
		   		// Add this buffer observer again to the array in Alf Server to get valid bitmap handles in return. 
    			TAlfScreenBufferBitmaps addObserverEvent = {iBufferInfo.iBufferUid, iBufferInfo.iPriority, NULL, NULL}; 
    			TPckg<TAlfScreenBufferBitmaps> eventPckg(addObserverEvent);
    			iClient.SendSyncCmd(EAlfSBufAddObserver, eventPckg);
    			
    			// Update bitmap handles
    			iBufferInfo.iBitmapHandle = addObserverEvent.iBitmapHandle; 
    			iBufferInfo.iMaskHandle = addObserverEvent.iMaskHandle;			
				}
			else if (event.iEventId == MAlfScreenBufferObserver::EDeleted)
				{
    			// Invalidate bitmap handles
    			iBufferInfo.iBitmapHandle = NULL; 
    			iBufferInfo.iMaskHandle = NULL;							
				}
    			
        	iBufferInfo.iObserver->HandleScreenBufferEvent(event.iBufferUid, event.iEventId);			
			}
        }
        
    void DoCancel()
        {
    	// Remove this AO from the observer array in Alf Server
    	TAlfScreenBufferBitmaps event = {iBufferInfo.iBufferUid, 0, 0}; 
    	TPckg<TAlfScreenBufferBitmaps> eventPckg(event);
    	iClient.SendSyncCmd(EAlfSBufRemoveObserver, eventPckg);
        }

    TInt RunError(TInt /*aError*/)
        {
        // Ignore error
        return KErrNone;
        }

private: // Data

    RAlfClient& iClient;
    TAlfScreenBufferEvent iEvent; // Event data for pending buffer event. 
    TPckg<TAlfScreenBufferEvent> iEventAsDescriptor;
    
    CAlfScreenBufferEventFetcher::TScreenBufferInfo iBufferInfo; // Buffer information
    };



// *** Array item for CAlfScreenBuffer

struct TAlfScreenBufferHandlerItem
	{
	TAlfScreenBufferHandlerItem( TInt aId, CAlfScreenBufferEventFetcher* aAoPtr) 
		:iId( aId ), iAoPtr( aAoPtr )
		{};
	TInt iId; // Buffer uid
	CAlfScreenBufferEventFetcher* iAoPtr; // Pointer to the active object that handles buffer events. Owned.
	};
		

// *** Private data for CAlfScreenBuffer

NONSHARABLE_CLASS(CAlfScreenBuffer::CPrivateData):public CBase
    {
    public:
    ~CPrivateData()
        {
        iScreenBufferHandlers.Close();
        if (iIsAlfEnvOwned)
        	{
        	delete(iEnv);
        	}
        }
        
    CAlfEnv* iEnv;   
    TBool iIsAlfEnvOwned; 
    RArray<TAlfScreenBufferHandlerItem> iScreenBufferHandlers; // List of all buffer observer Active Objects
    };


// *** class CAlfScreenBuffer

EXPORT_C CAlfScreenBuffer* CAlfScreenBuffer::NewL()
	{
	// Create own aAlf env if it does not exist.
	TBool isAlfEnvOwned = EFalse; 
	CAlfEnv* env = CAlfEnv::Static();
	if (!env)
		{
		isAlfEnvOwned = ETrue;
		env = CAlfEnv::NewL();
		}
	
	// Create object	
    CAlfScreenBuffer* self = new( ELeave ) CAlfScreenBuffer();
    CleanupStack::PushL( self );        
    self->ConstructL(env, isAlfEnvOwned);
    CleanupStack::Pop( self );
    return self;	
	};

EXPORT_C CAlfScreenBuffer* CAlfScreenBuffer::NewL(CAlfEnv& aEnv)
	{
    CAlfScreenBuffer* self = new( ELeave ) CAlfScreenBuffer();
    CleanupStack::PushL( self );        
    self->ConstructL(&aEnv, EFalse);
    CleanupStack::Pop( self );
    return self;	
	};


CAlfScreenBuffer::CAlfScreenBuffer()
	{	
	}

// @deprecated
EXPORT_C CAlfScreenBuffer::CAlfScreenBuffer(CAlfEnv& /*aEnv*/) 
	{	
	}


EXPORT_C CAlfScreenBuffer::~CAlfScreenBuffer()
	{
    for (TInt i = iData->iScreenBufferHandlers.Count()-1; i >= 0 ; i--)
   		{
        RemoveObserver(iData->iScreenBufferHandlers[i].iAoPtr->ScreenBufferInfo().iBufferUid);
        }

    delete iData;
	}

// @deprecated
EXPORT_C void CAlfScreenBuffer::ConstructL()
    {
    // Not used, preserved for BC
    }
    
void CAlfScreenBuffer::ConstructL(CAlfEnv* aEnv, TBool aIsAlfEnvOwned)
    {
    //@todo : should I check that only one instance is created per alf client? 
    // Create private data
    iData = new (ELeave) CPrivateData; 
    iData->iEnv = aEnv;
    iData->iIsAlfEnvOwned = aIsAlfEnvOwned;
    }


EXPORT_C MAlfBufferDrawer* CAlfScreenBuffer::GetDrawingInterface(TUid aInterfaceUid, TUid aBufferUid)
	{
	// Symbian IF
	if (aInterfaceUid == KAlfSymbianBufferDrawerUid)
	    {
	    TAlfScreenBufferHandlerItem item( aBufferUid.iUid, NULL );
	    TInt index;
	    if ( iData->iScreenBufferHandlers.FindInSignedKeyOrder( item, index ) == KErrNone )
		    {
		    if (iData->iScreenBufferHandlers[index].iAoPtr->ScreenBufferInfo().iBitmapHandle)
		    	{
       	    	CAlfSymbianBufferDrawer* drawer = NULL;
       	    	TRAPD(err, drawer = CAlfSymbianBufferDrawer::NewL());
       	    	if (!err)
       	    		{
                	TRAP(err,drawer->SetBufferL(
                		iData->iScreenBufferHandlers[index].iAoPtr->ScreenBufferInfo().iBitmapHandle,
                		iData->iScreenBufferHandlers[index].iAoPtr->ScreenBufferInfo().iMaskHandle));
                	if (err)
                		{
                		delete drawer;
                		return NULL;
                		}
                	else
                		{
                		return drawer;
                		}	
            		}   
		    	}
	        }
	    }
	return NULL;	            
	}
	

EXPORT_C MAlfBufferGc* CAlfScreenBuffer::GetGraphicsContext(TUid /*aInterfaceUid*/, TUid /*aBufferUid*/)  
	{	
	return ((MAlfBufferGc*)NULL);
	}
      
        
EXPORT_C void CAlfScreenBuffer::AddObserverL(TUid aBufferId, MAlfScreenBufferObserver* aObserver, TInt aPriority) 
	{
	AddObserverL(aBufferId, CAlfScreenBuffer::EFlagNone, aObserver, aPriority); 
	}

	
EXPORT_C void CAlfScreenBuffer::AddObserverL(TUid aBufferId, TUint aFlags, MAlfScreenBufferObserver* aObserver, TInt aPriority) 
	{    
    // Add to the observer array in Alf Server, and get bitmap handles in return. 
    TAlfScreenBufferBitmaps event = {aBufferId, aPriority, aFlags, NULL, NULL}; 
    TPckg<TAlfScreenBufferBitmaps> eventPckg(event);
    User::LeaveIfError(iData->iEnv->Client().SendSyncCmd(EAlfSBufAddObserver, eventPckg)); 
    
  	// Append the handler to the client list
   	CAlfScreenBufferEventFetcher* eventFetcher =  new (ELeave) CAlfScreenBufferEventFetcher(iData->iEnv->Client(), *aObserver);
   	CAlfScreenBufferEventFetcher::TScreenBufferInfo info = {eventFetcher->ScreenBufferInfo().iObserver, event.iBufferUid, aPriority, event.iBitmapHandle, event.iMaskHandle, TRect(), TRect()};
   	eventFetcher->ScreenBufferInfo() = info;    
   	// Transfers ownership of eventFetcher
    TInt err = iData->iScreenBufferHandlers.InsertInUnsignedKeyOrder(TAlfScreenBufferHandlerItem(aBufferId.iUid, eventFetcher)); 

    if (err)
    	{
    	delete eventFetcher;
    	RemoveObserver(aBufferId); 
    	User::Leave(err);
    	}
    else
    	{
    	// Start observing any event 
    	eventFetcher->RequestBufferEvent(); 
    	}	
	}
	
	
// Note: This can be also called to already removed observers. In that case it has no effect. 
EXPORT_C void CAlfScreenBuffer::RemoveObserver(TUid aBufferId) 
	{	
    // Remove from the observer array in Alf Server
    TAlfScreenBufferBitmaps event = {aBufferId, 0, 0}; 
    TPckg<TAlfScreenBufferBitmaps> eventPckg(event);
    iData->iEnv->Client().SendSyncCmd(EAlfSBufRemoveObserver, eventPckg); 
     
  	// Remove the handler from the client list
   	TAlfScreenBufferHandlerItem item( aBufferId.iUid, NULL );
	TInt index;
	if ( iData->iScreenBufferHandlers.FindInSignedKeyOrder( item, index ) == KErrNone )
		{
		delete(iData->iScreenBufferHandlers[index].iAoPtr);
		iData->iScreenBufferHandlers.Remove(index);
		}
	}


EXPORT_C void CAlfScreenBuffer::RequestNextBuffer(TUid aBufferId)       
	{
   	TAlfScreenBufferHandlerItem item( aBufferId.iUid, NULL );
	TInt index;
	if ( iData->iScreenBufferHandlers.FindInSignedKeyOrder( item, index ) == KErrNone )
		{
	    TUid event = aBufferId; 
    	TPckg<TUid> eventPckg(event);
    	iData->iEnv->Client().SendSyncCmd(EAlfSBufRequestNextBuffer, eventPckg);
		}
	else
		{
		// I am not a listener for this buffer 
		__ALFLOGSTRING1( "CAlfScreenBuffer::RequestNextBuffer(). Observer not found for uid %x", aBufferId.iUid );
		}
	}
    
    
EXPORT_C void CAlfScreenBuffer::RequestBufferDraw(TUid aBufferId)       
	{
   	TAlfScreenBufferHandlerItem item( aBufferId.iUid, NULL );
	TInt index;
	if ( iData->iScreenBufferHandlers.FindInSignedKeyOrder( item, index ) == KErrNone )
		{
	    TUid event = aBufferId; 
    	TPckg<TUid> eventPckg(event);
    	iData->iEnv->Client().SendSyncCmd(EAlfSBufRequestBufferDraw, eventPckg);
		}
	else
		{
		// I am not a listener for this buffer 
		__ALFLOGSTRING1( "CAlfScreenBuffer::RequestNextBuffer(). Observer not found for uid %x", aBufferId.iUid );
		}
	}
    







