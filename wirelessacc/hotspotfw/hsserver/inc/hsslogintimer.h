/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Timer class for login time
*
*/



#ifndef HSSLOGINTIMER_H
#define HSSLOGINTIMER_H

#include <e32base.h>

class MHssLoginTimerObserver;

/**
 *  Timer for total time used for polling 
 *
 *  @lib ictsclientinterface.lib
 *  @since S60 5.0
 */
class CHssLoginTimer : public CTimer
    {
    
    public:
        
        /**
        * Static constructor.
        */    
        static CHssLoginTimer* NewL( MHssLoginTimerObserver& aObserver );
    
        /**
        * Destructor.
        */
        virtual ~CHssLoginTimer();
    
        // from CTimer

        /**
        * Timer expired
        */
        void RunL();
    
    private:
    
        /**
        * C++ default constructor.
        */
        CHssLoginTimer( MHssLoginTimerObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
                 
    private: // data

        /**
        * Reference of client using timer 
        * Not own.  
        */
        MHssLoginTimerObserver& iObserver;
    
    };

#endif // HSSLOGINTIMER_H