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
* Description:   Screen buffer class
*
*/



#ifndef C_ALFSCREENBUFFER_H
#define C_ALFSCREENBUFFER_H

#include <e32base.h>

class CAlfEnv;

/**
 * @deprecated 
 *  Observer class for screen buffer.
 */
class MAlfScreenBufferObserver
    {
    /**
     * Screen buffer Events.
     */
public:     
        enum TAlfScreenBufferEvent
            {
            ENone,
            ECreated,  			// Buffer created 
            EDeleted, 			// Buffer deleted
            ERestored, 			// Not yet implemented
            EReleased, 			// Not yet implemented
            EBufferDrawComplete // Buffer has been drawn to window
            };
    
    /**
     * Alf calls this when all the content have been draw to the back buffer.
     * If EFalse is returned, the buffer is locked until the observer calls RequestNextBuffer().
     * If ETrue is returned, the RequestNextBuffer() is called automatically, immediately after this method returns.
     *
     * @param aBufferUid Unique id of the buffer.
     * @param aDisplayRect Screen buffer rect.
     * @param aDirtyRect Dirty rect for the buffer.
     * @return Return True, if buffer can be freed automatically after this call.
     */
    virtual TBool BufferComplete(TUid aBufferUid, TRect& aDisplayRect, TRect& aDirtyRect) = 0;
    
    /**
     * Alf calls this when e.g. the back buffer is created or deleted.
     *
     * @param aId The unique id of the buffer.
     * @param aEvent @see MAlfScreenBufferObserver::TAlfScreenBufferEvent
     */
    virtual void HandleScreenBufferEvent(TUid aBufferUid, TInt aEvent) = 0;
    };


/**
 * This interface is to be used when drawing the buffer to the window etc.      
 */
class MAlfBufferDrawer
    {
public:
    /**
     * Clean all resources (usually delete this).
     */
    virtual void ReleaseDrawer() = 0;    
    };

/**
 * @deprecated 
 * This interface is to be used when modifying the buffer (sent back to server).      
 * Could be attached to special content visual.
 */
class MAlfBufferGc
    {
    public:
    /**
     * Clean all resources (usually delete this).
     */
    virtual void ReleaseGc() = 0;
    };


/**
 * @deprecated 
 * The class for managing screen buffers.
 *
 * Usage:
 * @code
 * 
 * //Create screen buffer and observer
 * CAlfScreenBuffer* screenBuffer = CAlfScreenBuffer::NewL( *iEnv );
 * CallbackObserver* observer = new (ELeave) CallbackObserver();
 * 
 * // Add observer
 * screenBuffer->AddObserverL( testScreenBufferUid, observer );
 * MAlfBufferDrawer* drawer = screenBuffer->GetDrawingInterface( SYMBIAN_BUFFER_DRAWER_UID, 
 * 																	testScreenBufferUid );
 * 
 * //Request for next sceenbuffer
 * screenBuffer->RequestNextBuffer( testScreenBufferUid );
 * 
 * drawer->ReleaseDrawer();
 * drawer = NULL; 
 * 
 * //Remove observer
 * 	screenBuffer->RemoveObserver( testScreenBufferUid );
 * 
 * //Clean up
 *  delete screenBuffer;
 *  delete observer;
 * 
 * @endcode
 */
NONSHARABLE_CLASS(CAlfScreenBuffer): public CBase
    {
public:
	enum TAlfScreenBufferFlags 
		{
		EFlagNone = 0,
		EFlagDisableDrawToWindow = 1 // Do not automatically draw buffer content to the window
		};

    /**
     * Constructor.
     *
     * @param aEnv Alf environment.
     */
    IMPORT_C static CAlfScreenBuffer* NewL(CAlfEnv& aEnv);
    
    /**
     * Constructor.
     * 
     * Creates own Alf environment
     */
    IMPORT_C static CAlfScreenBuffer* NewL();
    
    /**
     * Destructor.
     *
     * If Alf environment is owned, it is deleted.
     */
    IMPORT_C virtual ~CAlfScreenBuffer();
    
    /**
     * Returns an instance of an interface for drawing the buffer e.g. to a window. 
     *
     * @param aInterfaceUid Unique id of the interface.
     * @param aBufferUid Unique id of the buffer.
     */
    IMPORT_C MAlfBufferDrawer* GetDrawingInterface(TUid aInterfaceUid, TUid aBufferUid);
    
    /**
     * Returns an instance of an interface for drawing to the buffer. 
     *
     * @param aInterfaceUid Unique id of the interface.
     * @param aBufferUid Unique id of the buffer.
     */
    IMPORT_C MAlfBufferGc* GetGraphicsContext(TUid aInterfaceUid, TUid aBufferUid);  
    
    /**
     * Request next buffer from the server. Unlocks buffer for drawing.
     *
     * @param aBufferUid Unique id of the buffer.
     */
    IMPORT_C void RequestNextBuffer(TUid aBufferUid);  
    
    /**
     * Request the server to draw the screen buffer to the window.
     *
     * @param aBufferUid Unique id of the buffer.
     */
    IMPORT_C void RequestBufferDraw(TUid aBufferUid);  
    
    /**
     * Add an observer for a buffer.
     *
     * @param aBufferUid Unique id of the buffer.
     * @param aObserver Pointer to the observer.
     * @param aPriority Not yet implemented.
     */
    IMPORT_C void AddObserverL(TUid aBufferUid, MAlfScreenBufferObserver* aObserver, TInt aPriority = 0  );  
    
    /**
     * Add an observer for a buffer.
     *
     * @param aBufferUid Unique id of the buffer.
     * @param aFlags Screen buffer flags.
     * @param aObserver Pointer to the observer.
     * @param aPriority Not yet implemented.
     */
    IMPORT_C void AddObserverL(TUid aBufferUid, TUint aFlags, MAlfScreenBufferObserver* aObserver, TInt aPriority = 0  );  
    
    /**
     * Remove observer from the buffer.
     *
     * @param aBufferUid Unique id of the buffer.
     */
    IMPORT_C void RemoveObserver(TUid aBufferId);  

private: 
    	CAlfScreenBuffer();
    	CAlfScreenBuffer(CAlfEnv& aEnv);
		void ConstructL(CAlfEnv* aEnv, TBool aIsAlfEnvOwned);
		void ConstructL();
		
private:    
        class CPrivateData;
        CPrivateData* iData; 
    };

#endif // C_ALFSCREENBUFFER_H