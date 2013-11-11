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
* Description:   Timer class for polling interval
*
*/



#ifndef ICTSPOLLINGINTERVALTIMER_H
#define ICTSPOLLINGINTERVALTIMER_H

#include <e32base.h>

class MIctsPollingIntervalObserver;

/**
 *  Timer for polling interval
 *
 *  @lib ictsclientinterface.lib
 */
class CIctsPollingIntervalTimer : public CTimer
    {
    
    public:
        
        /**
        * Static constructor.
        */    
        static CIctsPollingIntervalTimer* NewL( MIctsPollingIntervalObserver& aObserver );
    
        /**
        * Destructor.
        */
        virtual ~CIctsPollingIntervalTimer();
    
        // from CTimer
    
        /**
        * Timer expired
        */
        void RunL();
    
    private:
    
        /**
        * C++ default constructor.
        */
        CIctsPollingIntervalTimer( MIctsPollingIntervalObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
                     
    private: // data

        /**
        * Reference of client using timer 
        * Not own.  
        */
        MIctsPollingIntervalObserver& iObserver;
    
    };

#endif // ICTSPOLLINGINTERVALTIMER_H