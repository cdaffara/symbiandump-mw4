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
* Description: 
*
*/




#ifndef __ALFEXCALENDARENGINEEVENTTIMER_H__
#define __ALFEXCALENDARENGINEEVENTTIMER_H__

#include <e32base.h>
#include <e32std.h>



/**
* MAlfExCalendarEngineEventTimer is a pure virtual class that 
* defines callback function which is called when timer event
* occurs.
*/
class MAlfExCalendarEngineEventTimerObserver
    {
public:
    /**
     * Called when timer event triggered in 
     * CAlfExCalendarEngineEventTimer
     */	
    virtual void HandleTimerEventL() = 0;
    };




/**
* CAlfExCalendarEngine is a class that gives data to be displayed in 
* alfexcalendar example application.
*/
class CAlfExCalendarEngineEventTimer : public CActive
	{
public:
    /* Constructors and destructor. */

    /**
     * Constructor
     */	
    static CAlfExCalendarEngineEventTimer* NewL(MAlfExCalendarEngineEventTimerObserver& aObserver);
    
    /**
     * Constructor
     */
    static CAlfExCalendarEngineEventTimer* NewLC(MAlfExCalendarEngineEventTimerObserver& aObserver);
    
    /**
     * Destructor.
     */
    virtual ~CAlfExCalendarEngineEventTimer();

public:
	
	
    /* Methods. */

    /** @beginAPI */
    /**
     * Starts timer. 
     * Cancels previous timer if still active. 
     *
     * @param aDelay: delay.
     */
    void StartTimer( TTimeIntervalMicroSeconds32 aDelay);

    /** @endAPI */

private:
    /* Constructors. */

    /**
     * C++ constructor
     */
    CAlfExCalendarEngineEventTimer(MAlfExCalendarEngineEventTimerObserver& aObserver);
	
    /**
     * Second-phase constructor. 
     */
    void ConstructL();


protected:
    /**
     * From CActive
     */
    virtual void DoCancel();

    /**
     * From CActive
     */
    virtual void RunL();
	
    /**
     * From CActive.
     * This is called if RunL function leaves. 
     * Here we can just ignore all the errors and return KErrNone
     */
    virtual TInt RunError(TInt aError);
private:
    /** 
     * timer object.
     */
    RTimer		iTimer;
    
    /**
     * Observer object
     */
    MAlfExCalendarEngineEventTimerObserver& iObserver;
	};

#endif // __ALFEXCALENDARENGINEEVENTTIMER_H__
