/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Timer implementation.
*/

#ifndef WSFKEEPALIVETIMER_H
#define WSFKEEPALIVETIMER_H

// System includes
 
#include <e32base.h>

// User includes

// Forward declarations
 
// External data types

// Global function prototypes
 
// Constants

// Class declaration

/**
 *  Timer interface class.
 *
 *  This is an interface class, which the Keepalive implementation needs
 *  to inherit in order to receive timer notifications.
 */
class MWsfKeepaliveTimerNotify
    {
public:
    
    /**
     * Pure virtual timer expiration function that needs to be
     * implemented in the Keepalive implementation class.
     * 
     * @param aError Status of the timer event
     */
    virtual void TimerExpired( TInt aError ) = 0;
    };
 
/**
 *  Timer implementation.
 *  
 *  This is the actual timer implementation class.
 */
class CWsfKeepaliveTimer: public CActive
{
    // Friend classes:

public:
    
    // Data types

    /** Constructors */
    
    /**
     * Two-phased constructor.
     * 
     * @param aNotify Timer expirations are notified to this class
     * @return New object allocated from heap
     */
    static CWsfKeepaliveTimer* NewL( MWsfKeepaliveTimerNotify& aNotify );
    
    /**> Destructor */
    virtual ~CWsfKeepaliveTimer();

public:

    /**
     * Sets the timer to expire after the given interval.
     * 
     * @param aInterval Interval in microseconds
     */
    void After( TTimeIntervalMicroSeconds32 aInterval );

    /**
     * Stops the timer.
     */
    void Stop();

protected:
    
private:
    
    CWsfKeepaliveTimer( MWsfKeepaliveTimerNotify& aNotify );

    void ConstructL();
    
    void RunL();
    
    void DoCancel();

private: // Data

    /**
     * Actual timer object
     */
    RTimer iTimer;

    /**
     * Timer expirations are notified to this class
     */
    MWsfKeepaliveTimerNotify& iNotify;
    };

#endif // WSFKEEPALIVETIMER_H
