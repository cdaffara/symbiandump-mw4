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
* Description:   Timer class for  total polling time
*
*/



#ifndef ICTSPOLLINGTIMETIMER_H
#define ICTSPOLLINGTIMETIMER_H

#include <e32base.h>

class MIctsPollingTimeObserver;

/**
 *  Timer for total time used for polling 
 *
 *  @lib ictsclientinterface.lib
 */
class CIctsPollingTimeTimer : public CTimer
    {
    
    public:
        
        /**
        * Static constructor.
        */    
        static CIctsPollingTimeTimer* NewL( MIctsPollingTimeObserver& aObserver );
    
        /**
        * Destructor.
        */
        virtual ~CIctsPollingTimeTimer();
    
        // from CTimer

        /**
        * Timer expired
        */
        void RunL();
    
    private:
    
        /**
        * C++ default constructor.
        */
        CIctsPollingTimeTimer( MIctsPollingTimeObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
                 
    private: // data

        /**
        * Reference of client using timer 
        * Not own.  
        */
        MIctsPollingTimeObserver& iObserver;
    
    };

#endif // ICTSPOLLINGTIMETIMER_H