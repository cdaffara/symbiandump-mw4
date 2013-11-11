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
* Description:   Server side screen buffer handling
*
*/



#include "alfsrvscreenbuffermanager.h"
#include "alflogger.h"
#include <alf/alfappserver.h> 
#include <uiacceltk/HuiEnv.h> 

// ----------------------------

// Uid for requesting Hui Screen Buffer GC
const TUid KHuiInternalFbsBitmapBufferGcUid = {0x2000e5a3}; 

// Screen buffer flags
enum TAlfServerScreenBufferFlags 
	{
	EAlfScreenBufferFlagNone = 0,
	EAlfScreenBufferFlagDisableDrawToWindow = 1
	};


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfSrvScreenBufferManager* CAlfSrvScreenBufferManager::NewL(CAlfAppServer& aAppServer)
    {
    CAlfSrvScreenBufferManager* self = new( ELeave ) CAlfSrvScreenBufferManager(aAppServer);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfSrvScreenBufferManager::CAlfSrvScreenBufferManager(CAlfAppServer& aAppServer)
	: iAlfAppServer(aAppServer)
    {
    }


// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CAlfSrvScreenBufferManager::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfSrvScreenBufferManager::~CAlfSrvScreenBufferManager()
    {
	iScreenBufferObservers.Close();    
    }


// ---------------------------------------------------------------------------
// Delivers screen buffer complete event.
// ---------------------------------------------------------------------------
// 
TBool CAlfSrvScreenBufferManager::ScreenBufferComplete(TUid aUid, TRect& aDisplayRect, TRect& aDirtyRect)
	{
    TAlfScreenBufferEvent event = {aUid, 0, aDisplayRect, aDirtyRect};
    TriggerScreenBufferCompleteEvent(event);
    return EFalse;
	}


// ---------------------------------------------------------------------------
// Delivers screen buffer event.
// ---------------------------------------------------------------------------
// 
void CAlfSrvScreenBufferManager::HandleScreenBufferEvent(TUid aUid, TInt aEventId)
	{
    TAlfScreenBufferEvent event = {aUid, aEventId, TRect(), TRect()};
    TriggerScreenBufferEvent(event);
	}


// ---------------------------------------------------------------------------
// Complete pending client requests for given screen buffer.
// ---------------------------------------------------------------------------
// 
TBool CAlfSrvScreenBufferManager::TriggerScreenBufferEvent(TAlfScreenBufferEvent& aEvent)
    {
	CAlfSrvScreenBufferManager::TScreenBufferObserverItem item(aEvent.iBufferUid, 0, 0, NULL, NULL, 0);
	TInt index;
	TBool observerClientFound = EFalse;
	
	if (FindFromScreenBufferArray(item, index) != KErrNone)
		{
		// Error or no observers. Remove the observer from Hui as we cannot process it.
       	__ALFLOGSTRING1( "CAlfSrvScreenBufferManager::TriggerScreenBufferEvent. Observer not found for buffer %x ", aEvent.iBufferUid.iUid )
		CHuiDisplay* display = NULL;
		FindScreenBufferDisplay(aEvent.iBufferUid, &display);
		if (display)
			{
			display->RemoveScreenBufferObserver();
			}
		}
	else
		{
		observerClientFound = ETrue;
				
		// Complete pending request for all observers
		TInt count = iScreenBufferObservers.Count();
		
		// Buffer draw complete event is given only the first observer i.e. which has the highest priority
		if (aEvent.iEventId == MHuiScreenBufferObserver::EBufferDrawComplete)
			{
			count = index + 1; // do loop only once
			}
		
		for(; (index < count) && (iScreenBufferObservers[index].iBufferUid == aEvent.iBufferUid); index++)
			{		
		    if (aEvent.iEventId == MHuiScreenBufferObserver::ECreated)
    			{
    			// Update Hui display pointer in the observer list
				FindScreenBufferDisplay(iScreenBufferObservers[index].iBufferUid, &iScreenBufferObservers[index].iHuiDisplay);
    			}
		    else if (aEvent.iEventId == MHuiScreenBufferObserver::EDeleted)
    			{
    			// Remove Hui display pointer from the observer list
    			iScreenBufferObservers[index].iHuiDisplay = NULL;
    			}
    	    else
    	        {
    	        // For PC-lint
    	        }
			
			RMessagePtr2* message = &iScreenBufferObservers[index].iMessage;
	    	if (!message->IsNull())
	    		{
    			TPckg<TAlfScreenBufferEvent> eventPckg(aEvent);   	
        		TInt error = message->Write(0, eventPckg);
        		if ( error != KErrNone )
        			{
            		__ALFLOGSTRING1( "CAlfSrvScreenBufferManager::TriggerScreenBufferEvent: write error: %d", error )
        			}
        		message->Complete(error);        	    	
	    		}
	    	else
	    		{
	    		// Cannot sent a message to the client	    		
	    		// Store the event, if that cannot yet be sent 
	    		__ALFLOGSTRING2( "CAlfSrvScreenBufferManager::TriggerScreenBufferEvent: client not ready for buffer %x, pri %d",iScreenBufferObservers[index].iBufferUid.iUid, iScreenBufferObservers[index].iPriority )        		    		
	    		iScreenBufferObservers[index].iNextEventId = aEvent.iEventId; 
	    		}
			}
        }
    return observerClientFound;       
    }


// ---------------------------------------------------------------------------
// Complete pending client requests for given screen buffer
// ---------------------------------------------------------------------------
// 
TBool CAlfSrvScreenBufferManager::TriggerScreenBufferCompleteEvent(TAlfScreenBufferEvent& aEvent)
    {
	CAlfSrvScreenBufferManager::TScreenBufferObserverItem item(aEvent.iBufferUid, 0, 0, NULL, NULL, 0);
	TInt index;
	TBool observerClientFound = EFalse;
	CHuiDisplay* display = NULL;
	
	if (FindFromScreenBufferArray(item, index) != KErrNone)
		{
		// Error or no observers. Remove the observer from Hui as we cannot process it.
       	__ALFLOGSTRING1( "CAlfSrvScreenBufferManager::TriggerScreenBufferEvent. Observer not found for buffer %x ",
       		aEvent.iBufferUid.iUid )
		FindScreenBufferDisplay(aEvent.iBufferUid, &display);
		if (display)
			{
			display->RemoveScreenBufferObserver();
			}
		}
	else
		{
		observerClientFound = ETrue;
		display = iScreenBufferObservers[index].iHuiDisplay;
		
		// If needed, draw Screen Buffer automatically to the window when the buffer is complete 
		if (display && 
		    !(iScreenBufferObservers[index].iFlags & EAlfScreenBufferFlagDisableDrawToWindow))
			{
			display->DrawScreenBuffer();
 			} 			
		
		// Complete pending request for the first observer only i.e. which has the highest priority
		RMessagePtr2* message = &iScreenBufferObservers[index].iMessage;
	    if (!message->IsNull())
	    	{
    		TPckg<TAlfScreenBufferEvent> eventPckg(aEvent);   	
        	TInt error = message->Write(0, eventPckg);
        	if ( error != KErrNone )
        		{
           		__ALFLOGSTRING1( "CAlfSrvScreenBufferManager::TriggerScreenBufferEvent: write error: %d", error )
        		}
        	message->Complete(error);        	    	
	    	}
	    else
	    	{
	    	// Cannot sent a message to the client	    		
	    	__ALFLOGSTRING2( "CAlfSrvScreenBufferManager::TriggerScreenBufferEvent: client not ready for buffer %x, pri %d",
	    		iScreenBufferObservers[index].iBufferUid.iUid, iScreenBufferObservers[index].iPriority )        		    		
	    	if (iScreenBufferObservers[index].iHuiDisplay)
	    		{
	    		// Unlock the buffer to get the next callback
	    		// todo: should we invalidate the whole hui display now as we can loose dirty rects here?
				iScreenBufferObservers[index].iHuiDisplay->SetScreenBufferLock(EFalse);
	    		}
	    	}
        }
    return observerClientFound;       
    }


// ---------------------------------------------------------------------------
// Add the item to the observer array.
// ---------------------------------------------------------------------------
// 
void CAlfSrvScreenBufferManager::AddScreenBufferObserver(CAlfAppSrvSessionBase* aSession, const RMessage2& aMessage)
    {
    TAlfScreenBufferBitmaps event = {KNullUid, 0, 0, 0, 0}; 
    TPckg<TAlfScreenBufferBitmaps> eventPckg(event);
    aMessage.Read(0, eventPckg);
    
    // Check capabilities in main screen buffer case
    if (event.iBufferUid == KHuiUidBackBufferScreen0)
    	{
   		if(!aMessage.HasCapability(ECapabilityWriteDeviceData) && !aMessage.HasCapability(ECapabilitySwEvent))
   			{
			aMessage.Complete(KErrAccessDenied); 
			return;
   			}
    	}
    
     // Find the related display, if exists
 	CHuiDisplay* foundDisplay = NULL;
	TInt err = InitScreenBufferDisplay(event, foundDisplay);
    
    if (!err)
    	{
    	// Insert to the observer array
    	// Can have several observer sessions for one buffer, but do not allow duplicate (buffer,session) items.
    	// Order by (buffer, priority)
    	
    	CAlfSrvScreenBufferManager::TScreenBufferObserverItem removedItem(event.iBufferUid, 0, 0, NULL, aSession, 0);   
    	TInt count = RemoveAndReturnScreenBufferObserver(removedItem); // remove matching (iBufferUid,aSession) item
    	// Reinitialize in case RemoveAndGetObserver() has modified item
    	CAlfSrvScreenBufferManager::TScreenBufferObserverItem item(event.iBufferUid, event.iPriority, event.iFlags, foundDisplay, aSession, 0);  
    	item.iMessage = removedItem.iMessage; // Preserve message

// NOTE: Commented out because no okDisplay exists => Compilation error
//		__ALFLOGSTRING3( "CAlfSrvScreenBufferManager::AddScreenBufferObserver: Observer item in server list: uid=%x, pri=%d, disp=%d", event.iBufferUid, event.iPriority, okDisplay)
//		__ALFLOGSTRING1( "                                                                      session=%d", aSession )

    	err = iScreenBufferObservers.InsertInOrderAllowRepeats(item, CAlfSrvScreenBufferManager::InsertFunc);    	
    	}
    
    TInt msgErr = KErrNone;    

	// Get the index of the first, i.e. highest prioroty, observer
	CAlfSrvScreenBufferManager::TScreenBufferObserverItem item(event.iBufferUid, 0, 0, NULL, NULL, 0);
	TInt highestPriorityIndex = KErrNotFound;
	FindFromScreenBufferArray(item, highestPriorityIndex);
	
	if ( err )
		{
		// Error recovery. Remove the observer from Hui if there are no observers.		
   		__ALFLOGSTRING1( "CAlfSrvScreenBufferManager::AddScreenBufferObserver: Not Added, error=%d", err)
		if ((highestPriorityIndex < KErrNone) && foundDisplay)
			{
			foundDisplay->RemoveScreenBufferObserver(); 
			}
		msgErr = KErrCancel; // cancel
		}
	else	
		{
		if ( highestPriorityIndex >= 0 && (iScreenBufferObservers[highestPriorityIndex].iSession == aSession) &&
			foundDisplay)
			{
			// Clear the Hui display lock as the newly added observer is of highest priority
			foundDisplay->SetScreenBufferLock(EFalse);
			}
		
   		__ALFLOGSTRING( "CAlfSrvScreenBufferManager::AddScreenBufferObserver: Added ok.")
	    aMessage.Write(0, eventPckg); // update message           	
		msgErr = KErrNone; // ok!
		}
	aMessage.Complete(msgErr); 	
	}


// ---------------------------------------------------------------------------
// Initialize the aEvent struct with Hui display information 
// ---------------------------------------------------------------------------
// 
TInt CAlfSrvScreenBufferManager::InitScreenBufferDisplay(TAlfScreenBufferBitmaps& aEvent, CHuiDisplay*& aFoundDisplay)
    {
	TInt err = KErrNone;
 
	FindScreenBufferDisplay(aEvent.iBufferUid, &aFoundDisplay);	
		
	if (aFoundDisplay)
		{
		// Display exists already
		
		TBool isFirstObserver = !ScreenBufferObserverExists(aEvent.iBufferUid);
		if (isFirstObserver)
			{
			// Add server as an observer to Hui display instance
			TRAP(err, aFoundDisplay->AddScreenBufferObserverL(this)); 		
			}
		
		if (!err)
			{
    		// Get a handle to the buffer of the display
			MHuiFbsBitmapBufferGc* gc = (MHuiFbsBitmapBufferGc*)aFoundDisplay->GetGraphicsContext(KHuiInternalFbsBitmapBufferGcUid);
			if (gc)
				{
				CFbsBitmap* bitmap = gc->Bitmap();
				if (bitmap)
					{
					aEvent.iBitmapHandle = bitmap->Handle();
			    	// event.iMaskHandle is not yet implemented	
					}
				// Note: Gc can be released for KHuiInternalFbsBitmapBufferGcUid interface *only* as we 
				// know that it does not own the bitmap. So the bitmap is still valid.
				gc->ReleaseGc(); 	
				}
			}
		}	
	return err;	
    }

// ---------------------------------------------------------------------------
// remove the item from the observer array.
// ---------------------------------------------------------------------------
// 
void CAlfSrvScreenBufferManager::RemoveScreenBufferObserver(CAlfAppSrvSessionBase* aSession, const RMessage2& aMessage)
    {
    TAlfScreenBufferEvent event;
    TPckg<TAlfScreenBufferEvent> eventPckg(event);
    aMessage.Read(0, eventPckg);
    
	TScreenBufferObserverItem item(event.iBufferUid, 0, 0, NULL, aSession, 0);
    TInt count = RemoveAndReturnScreenBufferObserver(item); // Remove matching (iBufferUid,aSession) items
    
    if (count)
    	{
   		__ALFLOGSTRING( "CAlfSrvScreenBufferManager::RemoveScreenBufferObserver: observer removed.")

		// Complete pending message
		RMessagePtr2* message = &item.iMessage;
    	if (!message->IsNull())
        	{
        	message->Complete(KErrCancel); 
        	}
        		
        // Remove observer from Coretoolkit
        if (item.iHuiDisplay && !ScreenBufferObserverExists(event.iBufferUid))
			{
        	item.iHuiDisplay->RemoveScreenBufferObserver();
        	}
        }  
	}


// ---------------------------------------------------------------------------
// Unlock the buffer in Hui display
// ---------------------------------------------------------------------------
// 
void CAlfSrvScreenBufferManager::RequestNextBuffer(CAlfAppSrvSessionBase* aSession, const RMessage2& aMessage)
    {
    TInt err = KErrCancel;
    
	TUid uid; 
    TPckg<TUid> eventPckg(uid);
    aMessage.Read(0, eventPckg);
    
    TInt index;   
   	TScreenBufferObserverItem item(uid, 0, 0, NULL, NULL, 0);
   	
   	// Check if there is an observer for this buffer
	if (FindFromScreenBufferArray(item, index) != KErrNone)
		{
		// Observer not found. Cancel this request.
   		__ALFLOGSTRING( "CAlfSrvScreenBufferManager::RequestNextScreenBuffer: Observer not found. Cancel this request.")
		}
	// Check if the client is the higest priority observer?	
	else if (iScreenBufferObservers[index].iSession == aSession) 
		{
		CAlfSrvScreenBufferManager::TScreenBufferObserverItem* foundItem = &iScreenBufferObservers[index];
			
		// Release buffer lock to signal HUI to process the next frame
		if (foundItem->iHuiDisplay) 
			{
			foundItem->iHuiDisplay->SetScreenBufferLock(EFalse);
       		err = KErrNone;
 			}
       	}
	else 
		{
		// Not the highest priority observer. Just ignore the request.
		err = KErrNone; 
		}
		
    aMessage.Complete(err);   	            
	}


// ---------------------------------------------------------------------------
// Draw the screen buffer to the internal window
// ---------------------------------------------------------------------------
// 
void CAlfSrvScreenBufferManager::RequestBufferDraw(CAlfAppSrvSessionBase* aSession, const RMessage2& aMessage)
    {
    TInt err = KErrCancel;
    
	TUid uid; 
    TPckg<TUid> eventPckg(uid);
    aMessage.Read(0, eventPckg);
    
    TInt index;   
   	TScreenBufferObserverItem item(uid, 0, 0, NULL, NULL, 0);
	if ( FindFromScreenBufferArray(item, index) != KErrNone)
		{
		// Observer not found. Cancel this request.
   		__ALFLOGSTRING( "CAlfSrvScreenBufferManager::RequestNextScreenBuffer: Observer not found. Cancel this request.")
		}
	// Is the client the the higest priority observer?	
	else if (iScreenBufferObservers[index].iSession == aSession) 
		{
		CAlfSrvScreenBufferManager::TScreenBufferObserverItem* foundItem = &iScreenBufferObservers[index];
			
		// Release buffer lock to signal HUI to process the next frame
		if (foundItem->iHuiDisplay) 
			{
			foundItem->iHuiDisplay->DrawScreenBuffer();
       		err = KErrNone;
 			}
       	}
	else 
		{
		// Not the highest priority observer. Just ignore the request.
		err = KErrNone; 
		}
		     
    // Complete this message first 
    aMessage.Complete(err); 
      
    // Send a draw complete event if this is a highes priority observer  	            
    if (!err && (iScreenBufferObservers[index].iSession == aSession))
    	{
    	HandleScreenBufferEvent(uid, MHuiScreenBufferObserver::EBufferDrawComplete);
    	}   	
	}


// ---------------------------------------------------------------------------
// Store request message to the array item.
// ---------------------------------------------------------------------------
// 
void CAlfSrvScreenBufferManager::RequestScreenBufferEvent(CAlfAppSrvSessionBase* aSession, const RMessage2& aMessage)
    {
    TAlfScreenBufferEvent event;
    TPckg<TAlfScreenBufferEvent> eventPckg(event);
    aMessage.Read(0, eventPckg);
    TInt index;   
   	TScreenBufferObserverItem item(event.iBufferUid, 0, 0, NULL, aSession, 0);
	if ( FindFromScreenBufferArray(item, index) != KErrNone)
		{
		// Observer not found. Cancel this request.
   		__ALFLOGSTRING( "CAlfSrvScreenBufferManager::RequestNextScreenBuffer: Observer not found. Cancel this request.")
       	aMessage.Complete(KErrCancel);
		}
	else
		{
		CAlfSrvScreenBufferManager::TScreenBufferObserverItem* foundItem = &iScreenBufferObservers[index];
		if (foundItem->iMessage.IsNull())
			{
			foundItem->iMessage = aMessage; // Copy message to the list item
			}			
		else
			{
			aMessage.Complete(KErrAlreadyExists); 
			}
			
		// Check for pending next event	
		if (foundItem->iNextEventId != MHuiScreenBufferObserver::ENone)
			{
			TInt eventId = foundItem->iNextEventId;
			foundItem->iNextEventId = MHuiScreenBufferObserver::ENone;
			HandleScreenBufferEvent(foundItem->iBufferUid, eventId);
			}
       	}            
	}


// ---------------------------------------------------------------------------
// Find Hui display for the buffer.
// ---------------------------------------------------------------------------
// 
void CAlfSrvScreenBufferManager::FindScreenBufferDisplay(TUid aBufferUid, CHuiDisplay** aDisplay)
	{
	*aDisplay = NULL;
	CHuiDisplay* display = NULL;
	CAlfAppUi* alfAppUi = iAlfAppServer.AppUi();
	if (alfAppUi)
		{		
		TInt count = alfAppUi->HuiEnv().DisplayCount();
		for(TInt i = 0; i < count; i++)
			{
			display = &(alfAppUi->HuiEnv().Display(i));
			if (display->ScreenBufferUid() == aBufferUid)
				{
				*aDisplay = display;
				break;
				}
			}
		}
	}
	
	
// ---------------------------------------------------------------------------
// Remove item from the array. Return a copy of removed item.
// ---------------------------------------------------------------------------
// 
TInt CAlfSrvScreenBufferManager::RemoveAndReturnScreenBufferObserver(TScreenBufferObserverItem& aItem)
	{
	TInt removedObserverCount = 0; 
	TInt index = 0;
	TInt err = KErrNone;
	// There should not be more than one item but issue a while loop just in case...
	while (err == KErrNone)
		{
		err = FindFromScreenBufferArray(aItem, index);
		if (err == KErrNone)
			{
			aItem = iScreenBufferObservers[index];
			iScreenBufferObservers.Remove(index);
			removedObserverCount++;
			}	
		}
	return removedObserverCount;		
	}
	

// ---------------------------------------------------------------------------
// Remove all remaining observers from the array in case of client panic etc. 
// ---------------------------------------------------------------------------
// 
void CAlfSrvScreenBufferManager::RemoveScreenBufferObservers(CAlfAppSrvSessionBase* aClientId)
	{
	// Sequential search is slow but normally this is called on client exit only
	TInt count = iScreenBufferObservers.Count();
	for(TInt i = count - 1; i >= 0; i--)
		{
		if (iScreenBufferObservers[i].iSession == aClientId)
			{
			TUid bufferUid = iScreenBufferObservers[i].iBufferUid;
			CHuiDisplay* display = iScreenBufferObservers[i].iHuiDisplay;
			iScreenBufferObservers.Remove(i);
			if (display && !ScreenBufferObserverExists(bufferUid))
				{
        		display->RemoveScreenBufferObserver();
				}
			}
		}
	}

 	
// ---------------------------------------------------------------------------
// Find screen buffer from the array.
// ---------------------------------------------------------------------------
// 
TBool CAlfSrvScreenBufferManager::ScreenBufferObserverExists(const TUid& aScreenBufferUid)	
	{
   	TScreenBufferObserverItem item(aScreenBufferUid, 0, 0, NULL, NULL, 0);
   	TInt index = 0;
   	// Finds if there is any item with the specified uid, not necessarily the first such item.
	return (iScreenBufferObservers.FindInSignedKeyOrder(item, index) == KErrNone);
	}

	
// -----------------------------------------------------------------------------------
// Finds the first screen buffer item from the array based on buffer id and session id
// -----------------------------------------------------------------------------------
// 

TInt CAlfSrvScreenBufferManager::FindFromScreenBufferArray(
	const CAlfSrvScreenBufferManager::TScreenBufferObserverItem& aItem, TInt& aIndex) const
	{
	TInt err = KErrNotFound;
	TInt count = iScreenBufferObservers.Count(); 
	aIndex = 0;
	for(; (aIndex < count) && (iScreenBufferObservers[aIndex].iBufferUid.iUid <= aItem.iBufferUid.iUid); 
		aIndex++)
		{	
		if (iScreenBufferObservers[aIndex].iBufferUid == aItem.iBufferUid)
			{
			if ((aItem.iSession == NULL) 
			    || (iScreenBufferObservers[aIndex].iSession == aItem.iSession))
				{
				err = KErrNone;
				break;	
				}
			}
		}
	return err;
	}
	
	
// ---------------------------------------------------------------------------
// Insert function for iScreenBufferObservers array.
// ---------------------------------------------------------------------------
// 
TInt CAlfSrvScreenBufferManager::InsertFunc(const TScreenBufferObserverItem& aFirst, const TScreenBufferObserverItem& aSecond)
    {
    // Compare iBufferUid first
    if (aFirst.iBufferUid.iUid < aSecond.iBufferUid.iUid)
        {
        return -1; // less than
        }
    else if (aFirst.iBufferUid.iUid > aSecond.iBufferUid.iUid)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    // Because iBufferUid is equal, compare iPriority next.
    // Note that bigger priority should be first in the array!
    if (aFirst.iPriority > aSecond.iPriority)
        {
        return -1; // less than
        }
    else if (aFirst.iPriority < aSecond.iPriority)
        {
        return 1; // more than	
        }
    else
        {
        // For PC-lint
        }

    // Both
    return 0; // equal
    }
 
 
