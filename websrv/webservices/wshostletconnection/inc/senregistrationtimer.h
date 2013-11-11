/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    not a public class       
*
*/




#ifndef SEN_REGISTRATION_TIMER
#define SEN_REGISTRATION_TIMER

// INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KSenDefaultRegistrationTimeout = 1; // 1 second

// CLASS DECLARATION

#ifdef EKA2
NONSHARABLE_CLASS( MSenRegistrationActor )
#else
class MSenRegistrationActor
#endif
    {
    public:
        // Registers the files observer (connection agent) and/or RProperty subscription for (file) progress (BLOB) info
        virtual TInt RegisterAndSubscribeFileProgressObserversL() = 0;
    };


#ifdef EKA2
//NONSHARABLE_CLASS(CSenRegistrationTimer) : public CTimer
NONSHARABLE_CLASS( CSenRegistrationTimer ) : public CActive
#else
//class CSenRegistrationTimer : public CTimer
class CSenRegistrationTimer : public CActive
#endif
    {
    public:
        static CSenRegistrationTimer* NewL( MSenRegistrationActor& aActor, TInt aRegistrationTimeoutInSecs = KSenDefaultRegistrationTimeout );
        static CSenRegistrationTimer* NewLC( MSenRegistrationActor& aActor, TInt aRegistrationTimeoutInSecs = KSenDefaultRegistrationTimeout );
            
        ~CSenRegistrationTimer();

        /**
        * Calling this method will activate the registration, that will happen N seconds.
        */
        void IssueRegistrations( TBool aRegisterFilesObserver = ETrue );

        virtual void RunL();
        void DoCancel();

        //virtual TInt RunError(TInt aError);
    
    private:
        CSenRegistrationTimer( MSenRegistrationActor& aActor, TInt aRegistrationTimeoutInSecs );
        void ConstructL();
        
    private: // Data:
		MSenRegistrationActor& iActor;
        TInt iRegistrationTimeoutInSecs;		
		RTimer iTimer;
		// Registration flags
		TBool iRegisterFilesObserver;
    };

#endif // SEN_REGISTRATION_TIMER

// End of File
