/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares virtual MUpnpNotifyTimerObserver callback class
*                and the implemented active object class CUpnpNotifyTimer
*
*/


#ifndef C_CUPNPNOTIFYTIMER_H
#define C_CUPNPNOTIFYTIMER_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
static const TInt KSecond = 1000000;
// Max number of seconds we can pass to RTime::After
static const TInt KOneIterationSeconds = 1800;

// FORWARD DECLARATIONS
class CUpnpNotifyTimer;

// CLASS DECLARATION
/**
 * An interface from notifytimer to its customer (service)
 *
 *  @since Series60 2.0
 */
class MUpnpNotifyTimerObserver
    {
public:

    /**
     * TimerEventL
     * A callback function to time updates.
     * @since Series60 2.0
     * @param aTimer A pointer containing updated time.
     * @return None
     */
	virtual void TimerEventL( CUpnpNotifyTimer* aTimer ) = 0;

    /**
    * TimerEventError
    * Called when RunL leaves. By default no implementation needed.
    * @since Series60 3.2
    * @param aError Leave error
    * @return Error
    */
    virtual void TimerEventError( CUpnpNotifyTimer* /*aTimer*/, TInt /*aError*/ ) 
    { 
    }

    };
    
// CLASS DECLARATION
/**
* Wrapper for RTimer classs to notify observer about elaspsed time
*
*  @since Series60 2.0
*/
class CUpnpNotifyTimer : public CActive
    {
protected:

    /**
     * Constructor
     */
    IMPORT_C CUpnpNotifyTimer( MUpnpNotifyTimerObserver* aObserver );
    
public:
    IMPORT_C static CUpnpNotifyTimer* NewL(MUpnpNotifyTimerObserver* aObserver);

    void ConstructL();
    
    /**
     * Destructor.
     */
    ~CUpnpNotifyTimer();

    /**
     * At
     * @since Series60 2.0
     * @param aTime
     */
    IMPORT_C void AfterSeconds( TInt aIntervalInSeconds );

    /**
     * After
     * @since Series60 2.0
     * @param aInterval
     * @param aUnused this is unused now
     */
    IMPORT_C void After( TInt aInterval = 1, TBool aUnused = EFalse );

    /**
     * After
     * @since Series60 2.0
     * @param aInterval
     * @param aUnused this is unused now
     */
    IMPORT_C void After( TTimeIntervalMicroSeconds32 aInterval, TBool aUnused );
 
    /**
     * Start
     * Start the CUpnpNotifyTimer object.
     * @since Series60 2.0
     * @param aInterval
     * @param aUnused this is unused now 
     */
    IMPORT_C void Start( TTimeIntervalMicroSeconds32 aInterval,
        TBool aUnused = EFalse);

    /**
     * RunL
     * Active object state machine.
     * @since Series60 2.0
     */
    void RunL();

    /**
     * RunError
     * RunError in case RunL leaves.
     * @since Series60 2.0
     */
    TInt RunError( TInt aError );

    /**
     * DoCancel
     * Cancel outstanding request(s) and reset the iActive flag.
     * @since Series60 2.0
     */
    IMPORT_C void DoCancel();

    /**
     * CompleteOneselfImmediate
     * Immediate completes oneself by call of User::RequestComplete().
     * @since Series60 3.2
     */
    IMPORT_C void CompleteOneselfImmediate();

private: 

    /**
     * StartNextIteration
     * only for internal use, decrease repeat counter 
     * and waits for KOneIterationSeconds seconds it is required for long 
     * run timers because of RTimer::After 32 bit limitation - about 35 minutes      
     */
    void StartNextIteration();

    // Data

    // Interface observer object, owned
    MUpnpNotifyTimerObserver* iObserver;

    // Interval variable, owned
    TTimeIntervalMicroSeconds32 iInterval;

    // Timer, owned
    RTimer iTimer;
	
    // How many times timer need to be run for 30min 
    // after that timer will run for {(total time) mod (1800)} seconds 
    TInt iIterationsToRun;
    };


#endif // C_CUPNPNOTIFYTIMER_H

// End Of File