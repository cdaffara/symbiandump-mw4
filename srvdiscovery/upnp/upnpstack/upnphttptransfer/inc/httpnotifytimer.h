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
* Description:  Session timeout
*
*/


#ifndef C_HTTPNOTIFYTIMER_H
#define C_HTTPNOTIFYTIMER_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
static const TInt KSecond = 1000000;

// FORWARD DECLARATIONS
class CHttpNotifyTimer;

// CLASS DECLARATION
/**
* An interface from notifytimer to its customer (service)
*
*  @since Series60 5.0 HN
*/
class MHttpNotifyTimerObserver
    {
    public:

        /**
        * TimerEventL
        * A callback function to time updates.
        * @since Series60 5.0 HN
        * @param aTimer A pointer containing updated time.
        * @return None
        */
	virtual void TimerEventL( CHttpNotifyTimer* aTimer ) = 0;
    };
    
// CLASS DECLARATION
/**
* Wrapper for RTimer classs to notify observer about elaspsed time
*
*  @since Series60 5.0 HN
*/
class CHttpNotifyTimer : public CActive
    {
    protected:
        /**
        * Constructor
        */
        CHttpNotifyTimer( MHttpNotifyTimerObserver* aObserver );
    public:
        static CHttpNotifyTimer* NewL(MHttpNotifyTimerObserver* aObserver);
        void ConstructL();
        
        /**
        * Destructor.
        */
	    ~CHttpNotifyTimer();
        /**
        * At
        * @since Series60 5.0 HN
        * @param aTime
        */
        void AfterSeconds( TInt aIntervalInSeconds );
       
        /**
        * RunL
        * Active object state machine.
        * @since Series60 5.0 HN
        */
    	void RunL();

        /**
        * RunError
        * RunError in case RunL leaves.
        * @since Series60 5.0 HN
        */
        TInt RunError( TInt aError );

        /**
        * DoCancel
        * Cancel outstanding request(s) and reset the iActive flag.
        * @since Series60 5.0 HN
        */
    	void DoCancel();

    private: 

        // Interface observer object, owned
    	MHttpNotifyTimerObserver* iObserver;

        // Timer, owned
    	RTimer iTimer;
};


#endif // C_HTTPNOTIFYTIMER_H

// End Of File