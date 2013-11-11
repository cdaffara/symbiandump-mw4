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
* Description:   Class used to start and stop the TFX Server
*
*/


#ifndef C_TRANSITIONSERVERCONTROLLER_H
#define C_TRANSITIONSERVERCONTROLLER_H

#include <e32base.h>
#include <w32std.h>
#include <mtransitionserver.h>
#include <alfdecoderserverclient.h>

#include "alfpsobserver.h"

class RTfxServerInjector;
class CWsEventEater;
class CAlfPsObserver;
class CRepository;

/**
 * transition effect server controller
 * 
 * this class is used to load and start the TfxServer into the wserv process.
 * The code is loaded into the wserv through a CAnimDll wserv plugin.
 * because of this, when this class is destroyed, the TfxServer thread will be killed.
 */
 
// MPsObserver will probably not bee needed - to be removed later
NONSHARABLE_CLASS(CAlfTransitionServerController) : public CBase,
								   		 public MPsObserver
    {
public:
	friend class CAlfTransitionServerClient;

	/**
     * Public part of two phase construction.
     * @since S60 3.2
     *
     * @param Reference to a Window server session.
     * @return Pointer to the newly created CAlfTransitionServerController object.
    */  
    static CAlfTransitionServerController* NewL(RWsSession& /*aWsSession*/);

	/**
     * Destructor
     * @since S60 3.2
     *
    */  
    virtual ~CAlfTransitionServerController();

	/**
     * Start the transition server
     * @since S60 3.2
     *
     * @return KErrNone or any of the system error codes.
    */  
    TInt StartTransitionServer();

	/**
     * Gracefully shut down the transition server.
     * @since S60 3.2
     *
     * @return KErrNone or any of the system error codes.
    */  
    TInt StopTransitionServer();

	/**
     * Checks to see if server is running.
     * @since S60 3.2
     *
     * @return ETrue if server is running, otherwise EFalse.
    */  
    TBool ServerRunning();
    
   	/**
     * Start the transition server
     * @since S60 3.2
     * 
     * Starts the server with different values for MaxBytesOngoing and 
     * MaxBytesBetween than the default ones. 
     * MaxBytesOngoing is the maximum amount of bytes that may be cached while a transition
     * is ongoing. If this value is less than the total size of the 
	 * decoded images in the effect the images has to be decoded every 
	 * frame which will have a huge impact on performance. 
     *
     * MaxBytesBetween is the maximum amount of bytes that may be cached when a transition
	 * is not ongoing.
	 *
     * @param aMaxBytesOngoing maximum bytes in cache during a transition  	   
     * @param aMaxBytesBetween maximum bytes in cache between transitions
     * @return KErrNone or any of the system error codes.
    */  
    TInt StartTransitionServer(TInt32 aMaxBytesOngoing, TInt32 aMaxBytesBetween);
   
    static TInt ConnectionCallback( TAny* aParameter );
   
    TInt ConnectionAttempt();
    
  	void PsValueUpdated( const TUid aCategory, const TUint aKey, const TInt aVal );
  	
  	

private:

	/**
     * Private part of two phase construction.
     *
    */  
    void ConstructL();

	/**
     * Private constructor.
    */  
    CAlfTransitionServerController();
    

    
private: // data

    CAlfPsObserver* iPsObserver;
    
    // The public and subscribe value keeping track if server is running
    TInt iTransitionControlValue;
    
    // The value in central repository to keep track if effects are allowed
    TInt iEffectValue;
    
    CRepository* iRepository;
    
   	RAlfTfxClient iTfxServer;
   	TBool iIsConnected;
   	
   	CPeriodic* iTimer;
   	TBool iTimerRunning;

    };


#endif //C_TRANSITIONSERVERCONTROLLER_H