/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Class for Analog Dialer example application, which
*                implements long lasting tactile feed back event
*
*/



#include <touchfeedback.h>

#include "alfexanalogdialerfeedback.h"

// constants

// minimum values for clock in milliseconds
const TInt KFeedbackMinimumDuration = 100;
const TInt KFeedbackMinimumInterval = 100;


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CAlfExAnalogDialerFeedback* CAlfExAnalogDialerFeedback::NewL()
    {
    CAlfExAnalogDialerFeedback* self = new (ELeave) CAlfExAnalogDialerFeedback();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAlfExAnalogDialerFeedback::CAlfExAnalogDialerFeedback()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAlfExAnalogDialerFeedback::~CAlfExAnalogDialerFeedback()
    {
    if ( iClock )
        {
        iClock->Cancel();
        delete iClock;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerFeedback::ConstructL()
    {
    iFeedback = MTouchFeedback::Instance();
    // Remote control server command repeat timer.
    iClock = CPeriodic::NewL( EPriorityNormal );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerFeedback::Start( TInt    aDurationMilliSeconds,
                                        TInt    aIntervalMilliSeconds)
    {
    iDuration = KFeedbackDefault;
    iInterval = KFeedbackMinimumInterval;

    // 1000: convert millis to micros
    if (iFeedback)
        {
        iFeedback->InstantFeedback(ETouchFeedbackBasic);
        }
    if ( aIntervalMilliSeconds > KFeedbackMinimumInterval )
        {
        iInterval = 1000*aIntervalMilliSeconds;   
        }
    if (aDurationMilliSeconds >= KFeedbackMinimumDuration)
        {    
        iDuration = 1000*aDurationMilliSeconds;   
        iDurationStop.HomeTime();
        iDurationStop +=  TTimeIntervalMicroSeconds(TInt64(iDuration));
        iClock->Cancel();
        iClock->Start(  iInterval,
                        iInterval, 
                        TCallBack( HandleInterval, this ));
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerFeedback::Stop()
    {
    iClock->Cancel();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerFeedback::DoHandleInterval()
    {
    TTime timeNow;
    timeNow.HomeTime();
    if (timeNow > iDurationStop)
        {
        iClock->Cancel();
        }
    else
        {
        if (iFeedback)
            {
            iFeedback->InstantFeedback(ETouchFeedbackBasic);
            }
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CAlfExAnalogDialerFeedback::HandleInterval(TAny* aThis)
    {
    // cast, and call non-static function
    static_cast<CAlfExAnalogDialerFeedback*>(aThis)->DoHandleInterval();
    return KErrNone;
    }
