/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      Implementation of class TCodProgress.   
*      
*
*/


// INCLUDE FILES

#include "CodProgress.h"
#include "CodLoadObserver.h"
#include "CodPanic.h"
#include "CodLogger.h"
#include "FileExt.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TCodProgress::NotifyObserverL()
// ---------------------------------------------------------
//
void TCodProgress::NotifyObserverL()
    {
    CLOG(( ECodEng, 2, _L("TCodProgress::NotifyObserverL (0x%x) %d/%d"), \
        iObserver, CurrentValue(), FinalValue() ));
    if ( iObserver )
        {
        iObserver->ProgressL( FinalValue(), CurrentValue() );
        }
    }
// ---------------------------------------------------------
// TCodProgress::SetObserver()
// ---------------------------------------------------------
//
void TCodProgress::SetObserver( MCodLoadObserver* aObserver )
    {
    CLOG(( ECodEng, 2, _L("TCodProgress::SetObserver(0x%x)"), aObserver ));
    __ASSERT_ALWAYS( !(aObserver && iObserver), \
        CodPanic( ECodObserverAlreadySet ) );
    iObserver = aObserver;
    }

// ---------------------------------------------------------
// TCodProgress::Setup()
// ---------------------------------------------------------
//
void TCodProgress::Setup( TInt aLoad, TInt aNotify )
    {
    CLOG(( ECodEng, 2, \
        _L("TCodProgress::Setup load(%d) notify(%d)"), aLoad, aNotify ));
    iPhase = ELoad;
    iMax[ELoad] = aLoad;
    iMax[ENotify] = aNotify;
    iCur[ELoad] = 0;
    iCur[ENotify] = 0;
    }

// ---------------------------------------------------------
// TCodProgress::StartPhaseL()
// ---------------------------------------------------------
//
void TCodProgress::StartPhaseL( TPhase aPhase )
    {
    CLOG(( ECodEng, 2, _L("TCodProgress::StartPhaseL(%d)"), aPhase ));
    iPhase = aPhase;
    NotifyObserverL();
    }

// ---------------------------------------------------------
// TCodProgress::IncrementL()
// ---------------------------------------------------------
//
void TCodProgress::IncrementL( TInt aBytes )
    {
    CLOG(( ECodEng, 2, _L("TCodProgress::IncrementL (%d)"), aBytes ));
    if ( aBytes >= 0 )
        {
        iCur[iPhase] += aBytes;
        if ( iCur[iPhase] > iMax[iPhase] )
            {
            // Raise underestimated max. to match current.
            // Remaining unchanged, max. raised -> progress goes forward.
            iMax[iPhase] = iCur[iPhase];
            }
        }
        NotifyObserverL();
    }

// ---------------------------------------------------------
// TCodProgress::SetAmountL()
// ---------------------------------------------------------
//
void TCodProgress::SetAmountL( TInt aBytes )
    {
    CLOG(( ECodEng, 2, _L("TCodProgress::SetAmountL (%d)"), aBytes ));

    iCur[iPhase] = aBytes;
    if ( iCur[iPhase] > iMax[iPhase] )
        {
        // Raise underestimated max. to match current.
        // Remaining unchanged, max. raised -> progress goes forward.
        iMax[iPhase] = iCur[iPhase];
        }
    NotifyObserverL();
    }

// ---------------------------------------------------------
// TCodProgress::DoneL()
// ---------------------------------------------------------
//
void TCodProgress::DoneL()
    {
    CLOG(( ECodEng, 2, _L("TCodProgress::DoneL") ));
    NotifyObserverL();
    }
