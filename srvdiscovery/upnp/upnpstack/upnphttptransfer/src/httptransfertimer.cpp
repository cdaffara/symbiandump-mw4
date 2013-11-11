/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the class CHttpTransferTimer.
*
*/


// User include files
#include "httptransfertimer.h"
#include "httptransfertimerobserver.h"

// --------------------------------------------------------------------------
// CHttpTransferTimer::NewL
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpTransferTimer* CHttpTransferTimer::NewL( 
                                      TInt aInterval,
                                      MHttpTransferTimerObserver* aCallback )
    {
    CHttpTransferTimer* self = new (ELeave) 
                                CHttpTransferTimer( aInterval, aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CHttpTransferTimer::CHttpTransferTimer
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpTransferTimer::CHttpTransferTimer( 
                                       TInt aInterval,
                                       MHttpTransferTimerObserver* aCallback)
    {
    iInterval = aInterval;
    iCallback = aCallback;
    }

// --------------------------------------------------------------------------
// CHttpTransferTimer::ConstructL
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferTimer::ConstructL()
    {

    // Create CHeartbeat object
    iHeartbeat = CHeartbeat::NewL( EPriorityLow );

    iCounter = 0;

    // Start the heartbeat timer (beating on every second)
    iHeartbeat->Start( ETwelveOClock, this );
    }

// --------------------------------------------------------------------------
// CHttpTransferTimer::~CHttpTransferTimer()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CHttpTransferTimer::~CHttpTransferTimer()
    {
    // Cancel any outstanding request
    if( iHeartbeat )
        {
        iHeartbeat->Cancel();
        delete iHeartbeat;
        }
    }

// --------------------------------------------------------------------------
// CHttpTransferTimer::Beat()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CHttpTransferTimer::Beat()
    {
    // Increase heartbeat counter
    iCounter++;

    // If interval is reached, do the call back
    if( iCounter == iInterval )
        {
        if( iCallback )
            {
            iCallback->TimerCallback();
            }
        iCounter = 0;
        }
    }

//  End of File
