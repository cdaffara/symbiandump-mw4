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
* Description:   Server side screen buffer manager
*
*/



#ifndef C_ALFSRVSCREENBUFFERMANAGER_H
#define C_ALFSRVSCREENBUFFERMANAGER_H

#include <e32base.h>
#include <alf/alfconstants.h> 
#include <uiacceltk/HuiDisplay.h> 

class CAlfAppSrvSessionBase;
class CAlfAppServer;

/**
 *  Server side screen buffer manager
 *
 *  The class handles off-screen buffer events and observers.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CAlfSrvScreenBufferManager) : public CBase, public MHuiScreenBufferObserver
    {   
    
public:

    /**
     * Two-phased constructor.
     *
     * @param aAppServer Alf application server instance
     * @return new instance of CAlfSrvScreenBufferManager.
     */
    static CAlfSrvScreenBufferManager* NewL(CAlfAppServer& aAppServer);

    /**
     * Destructor.
     */
    virtual ~CAlfSrvScreenBufferManager();
    
    /**
     * Add a new observer.
     *
     * @param aSession Client session.
     * @param aMessage Client message.
     */
	void AddScreenBufferObserver(CAlfAppSrvSessionBase* aSession, const RMessage2& aMessage);
	
    /**
     * Remove an observer.
     *
     * @param aSession Client session.
     * @param aMessage Client message.
     */
	void RemoveScreenBufferObserver(CAlfAppSrvSessionBase* aSession, const RMessage2& aMessage);
	
    /**
     * Request a next frame and buffer from Hui display. Unlocks the display.
     *
     * @param aSession Client session.
     * @param aMessage Client message.
     */
	void RequestNextBuffer(CAlfAppSrvSessionBase* aSession, const RMessage2& aMessage);
	
    /**
     * Draw an off-screen buffer to the internal window.
     *
     * @param aSession Client session.
     * @param aMessage Client message.
     */
	void RequestBufferDraw(CAlfAppSrvSessionBase* aSession, const RMessage2& aMessage);
	
    /**
     * Request a general screen buffer event.
     *
     * @param aSession Client session.
     * @param aMessage Client message.
     */
	void RequestScreenBufferEvent(CAlfAppSrvSessionBase* aSession, const RMessage2& aMessage);
	
    /**
     * Remove all observer items related to this client session.
     *
     * @param aClientId Client session.
     */
	void RemoveScreenBufferObservers(CAlfAppSrvSessionBase* aClientId);
	
    /**
     * Trigger a screen buffer event.
     *
     * @param aEvent Event to be triggered.
     * @return Return true if there is a handler for the event.
     */
	TBool TriggerScreenBufferEvent(TAlfScreenBufferEvent& aEvent);
	
private: // internals

    CAlfSrvScreenBufferManager(CAlfAppServer& aAppServer);    

    void ConstructL();

	TInt InitScreenBufferDisplay(TAlfScreenBufferBitmaps& aEvent, CHuiDisplay*& aFoundDisplay);

	struct TScreenBufferObserverItem;	
	
	TBool TriggerScreenBufferCompleteEvent(TAlfScreenBufferEvent& aEvent);
		
	void FindScreenBufferDisplay(TUid aBufferUid, CHuiDisplay** aDisplay);

	TInt RemoveAndReturnScreenBufferObserver(TScreenBufferObserverItem& aItem);

	TBool ScreenBufferObserverExists(const TUid& aScreenBufferUid);
	
	TInt FindFromScreenBufferArray(
	        const CAlfSrvScreenBufferManager::TScreenBufferObserverItem& aItem, 
	        TInt& aIndex) const;

	static TInt InsertFunc(
	        const CAlfSrvScreenBufferManager::TScreenBufferObserverItem& aFirst, 
	        const CAlfSrvScreenBufferManager::TScreenBufferObserverItem& aSecond );

private: // From MHuiScreenBufferObserver
	
    TBool ScreenBufferComplete(TUid aUid, TRect& aDisplayRect, TRect& aDirtyRect);

    void HandleScreenBufferEvent(TUid aUid, TInt aEvent);

private:

	/** The array item struct for screen buffer observers */
   	struct TScreenBufferObserverItem
        {
        TScreenBufferObserverItem():
        	iBufferUid(KNullUid), 
        	iPriority(0), 
        	iFlags(0), 
        	iHuiDisplay(NULL), 
        	iSession(NULL),  
        	iNextEventId(0) {};
        	
        TScreenBufferObserverItem(TUid aBufferUid, TInt aPriority, TInt aFlags, CHuiDisplay* aHuiDisplay, CAlfAppSrvSessionBase* aSession, TInt aNextEventId):
        	iBufferUid(aBufferUid), 
        	iPriority(aPriority), 
        	iFlags(aFlags), 
        	iSession(aSession), 
        	iHuiDisplay(aHuiDisplay), 
        	iNextEventId(aNextEventId) {};
        
        /** Screen buffer uid */
        TUid iBufferUid; 
        /** Screen buffer priority */
        TInt iPriority; 
        /** Flags */
        TInt iFlags; 
        /** Pointer to the hui display */
        CHuiDisplay* iHuiDisplay; 
        /** Session */
        CAlfAppSrvSessionBase* iSession;
        /** Message */
        RMessagePtr2 iMessage;
        /** Id of the next event, if any */
        TInt iNextEventId;
       };
    
    /** Alf application server object */
    CAlfAppServer& iAlfAppServer;
    
	/** Array of screen buffer observers for all sessions and buffers */
	RArray<CAlfSrvScreenBufferManager::TScreenBufferObserverItem> iScreenBufferObservers;    
	
    };

#endif // C_ALFSRVSCREENBUFFERMANAGER_H
