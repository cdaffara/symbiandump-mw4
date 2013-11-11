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
* Description:        
*
*/








/**
* Note: please us ActivateShutdown() + Deque() -pair, instead of After() + Cancel() -pair
*/

// INCLUDES
#include "sencoreshutdowntimer.h"
#include <flogger.h>


// CLASS DECLARATION
CSenCoreShutdownTimer* CSenCoreShutdownTimer::NewL( TInt aShutdownTimeInSecs )
    {
    CSenCoreShutdownTimer* pNew = CSenCoreShutdownTimer::NewLC( aShutdownTimeInSecs );
    CleanupStack::Pop( pNew );
    return pNew;
    }

CSenCoreShutdownTimer* CSenCoreShutdownTimer::NewLC( TInt aShutdownTimeInSecs )
    {
    CSenCoreShutdownTimer* pNew = new (ELeave)CSenCoreShutdownTimer( aShutdownTimeInSecs );
    CleanupStack::PushL( pNew );
    pNew->ConstructL();
    return pNew;
    }
     
CSenCoreShutdownTimer::CSenCoreShutdownTimer( TInt aShutdownTimeInSecs )
:
CTimer( EPriorityNormal ),
iShutdownTimeInSecs( aShutdownTimeInSecs ) //, iCancelled( EFalse )
    {
    }     
            
CSenCoreShutdownTimer::~CSenCoreShutdownTimer()
    {
    }

void CSenCoreShutdownTimer::ActivateShutdown()
    {
    CActiveScheduler::Add(this);
    if( iShutdownTimeInSecs > 0 )
        {
        TTimeIntervalMicroSeconds32 interval = iShutdownTimeInSecs * 1000 * 1000;
        After( interval );
        }
    else // use 30 secs (default)
        {
        TTimeIntervalMicroSeconds32 interval = KSenDefaultShutdownTime * 1000 * 1000;
        After( interval );
        }
    }

void CSenCoreShutdownTimer::RunL()
    {
    CActiveScheduler::Stop(); // shuts WS-stack Core (server)
    }

//virtual TInt CSenCoreShutdownTimer::RunError(TInt aError) { ; }
//void CSenCoreShutdownTimer::DoCancel() 
//    {
//    iCancelled = ETrue;    
//    }
    
// End of File
