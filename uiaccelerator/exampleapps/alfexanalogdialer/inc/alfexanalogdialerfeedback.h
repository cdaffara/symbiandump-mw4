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



#ifndef ANALOGDIALER_FEEDBACK_H
#define ANALOGDIALER_FEEDBACK_H

#include <e32std.h> 

// Forward reference
class MTouchFeedback;
class CPeriodic;

// default time values for playing tactile feedback
const TInt KFeedbackDefault = 0;

/**
 *  Class for giving physical feedback about events in the analog dialer
 *
 *  @since S60 5.0
 */
class CAlfExAnalogDialerFeedback : public CBase
    {
public:

    
    /* Constructors and destructor. */

    /**
     * First phase construction of CAlfExAnalogDialerFeedback.
     *
     * @since S60 5.0
     */
    static CAlfExAnalogDialerFeedback* NewL();

    /**
     * Destructor of CAlfExAnalogDialerFeedback
     * Destroy the object and release all memory objects
     *
     * @since S60 5.0
     */
    virtual ~CAlfExAnalogDialerFeedback();
    

    /* Methods. */

    /**
     * Starts tactile feedback which lasts for given time
     *
     * @param aDuration total duration of tactile feedback.
     *                  KFeedbackDefaultInterval: feedback is given once only
     * @param aInterval interval, when feed back is restarted
     *                  KFeedbackDefaultInterval: continuos feedback is given.
     *                  if aDuration == KFeedbackDefaultInterval, this parameter
     *                  is ignored.
     *        
     * @since S60 5.0
     */
    void Start( TInt    aDurationMilliSeconds = KFeedbackDefault,
                TInt    aIntervalMilliSeconds = KFeedbackDefault);
    
    /**
     * Stops tactile feedback immediately
     *
     * @since S60 5.0
     */
    void Stop();

private:


    /* Constructors */

    /**
     * Constructor of CAlfExAnalogDialerFeedback
     * @since S60 5.0
     */
    CAlfExAnalogDialerFeedback();
    
    /**
     * Second phase construction of CAlfExAnalogDialerFeedback.
     * Private as this object must be constructed via NewL() or NewLC()
     *
     * @since S60 5.0
     */
    void ConstructL();


    /* Methods. */

    /**
     * Handles elapsed interval of feedback clock
     *
     * @since S60 5.0
     */
    void DoHandleInterval();

    /**
     * TCallBack function. Handles elapsed interval of feedback clock. 
     * Required by CPeriodic.
     * 
     * @return Returns value forced by TCallBack
     * @since S60 5.0
     */
    static TInt HandleInterval(TAny* aThis);

private:

    /* Member data. */

    /**
     * Tactile feedback object
     * Not own.
     */
    MTouchFeedback*     iFeedback;
    
    /**
     * Class for implementing continuous feedback effect
     * Own.
     */
    CPeriodic*          iClock;
    
    /**
     * time for duration of current feedback
     */     
    TInt                iDuration;

    /**
     * interval of current feedback
     */
    TInt                iInterval;

    /**
     * Clock time when feedback stops
     */
    TTime               iDurationStop;
    
    };


#endif // ANALOGDIALER_FEEDBACK_H

