/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



#ifndef CONNECTIONTIMER_H
#define CONNECTIONTIMER_H

//  INCLUDES

#include <e32std.h>
#include <e32base.h>

class CConnectionTimer : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CConnectionTimer* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CConnectionTimer();

    public: // New functions
        
        void SetConnectionTimer(TTimeIntervalMicroSeconds32 aTimeOut);
        
    private:

        /**
        * C++ default constructor.
        */
        CConnectionTimer( );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        //From Active
        void RunL();
		void DoCancel();
		private:    // Data
        //logging
        RTimer iTimer;
            };

#endif      // CONNECTIONTIMER_H
            
// End of File
