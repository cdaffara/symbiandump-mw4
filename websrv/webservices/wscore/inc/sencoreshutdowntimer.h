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
* Description: Header declaration
*
*/








#ifndef SEN_CORESHUTDOWNTIMER
#define SEN_CORESHUTDOWNTIMER

// INCLUDES
#include <e32base.h>
#include <flogger.h>

// CONSTANTS
const TInt KSenDefaultShutdownTime = 30;

// CLASS DECLARATION
class CSenCoreShutdownTimer : public CTimer
    {
    public:
        static CSenCoreShutdownTimer* NewL( TInt aShutdownTimeInSecs = KSenDefaultShutdownTime );
        static CSenCoreShutdownTimer* NewLC( TInt aShutdownTimeInSecs = KSenDefaultShutdownTime );
            
        ~CSenCoreShutdownTimer();

        /**
        * Calling this method will issue CActiveScheduler::Stop() [closing Ws-stack Core server thread],
        * unless a call to Cancel() was performed (by new CSenClientSession == SC, SM or HC type client)
        * Shuttime time is as defined in constuction of the timer
        */
        void ActivateShutdown();

        virtual void RunL();
        //virtual TInt RunError(TInt aError);
//        virtual void DoCancel();
    
    private:
        CSenCoreShutdownTimer( TInt aShutdownTimeInSecs );
//        void ConstructL();
        TInt iShutdownTimeInSecs;
//        TBool iCancelled;
    };

#endif // SEN_CORESHUTDOWNTIMER

// End of File
