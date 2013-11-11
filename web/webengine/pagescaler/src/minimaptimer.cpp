/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



// INCLUDE FILES
#include "minimaptimer.h"

#include "minimap.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS


// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?classname::?classname
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMinimapTimer::CMinimapTimer(CMinimap& aMinimap, TMinimapCb aCallback, TInt aPriority)
    : CActive(aPriority),
      iMinimap(&aMinimap),
      iCb(aCallback)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CMinimap::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMinimapTimer::ConstructL()
    {
    User::LeaveIfError(iTimer.CreateLocal());
    }

// -----------------------------------------------------------------------------
// CMinimap::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMinimapTimer* CMinimapTimer::NewL(CMinimap& aMinimap, TMinimapCb aCallback, TInt aPriority)
    {
    CMinimapTimer* self = new( ELeave ) CMinimapTimer(aMinimap, aCallback, aPriority);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CMinimapTimer::~CMinimapTimer()
    {
    Cancel();
    iTimer.Close();
    }


// -----------------------------------------------------------------------------
// CMinimapTimer::Start
//
//
// -----------------------------------------------------------------------------
//
void CMinimapTimer::Start(TTimeIntervalMicroSeconds32 aTime)
    {
    Cancel();
    iTime = aTime;
    iTimer.After(iStatus,aTime);
    SetActive();
    }


// -----------------------------------------------------------------------------
// CMinimapTimer::StartOrExtend
//
//
// -----------------------------------------------------------------------------
//
void CMinimapTimer::StartOrContinue(TTimeIntervalMicroSeconds32 aTime)
    {
    if (!IsActive())
        {
        iTime = aTime;
        iTimer.After(iStatus,aTime);
        SetActive();
        }
    }


// -----------------------------------------------------------------------------
// CMinimapTimer::RunL
//
//
// -----------------------------------------------------------------------------
//
void CMinimapTimer::RunL()
    {
    if (iMinimap && iCb)
        {
        (iMinimap->*iCb)();
        }
    }

// -----------------------------------------------------------------------------
// CMinimapTimer::DoCancel
//
//
// -----------------------------------------------------------------------------
//
void CMinimapTimer::DoCancel()
    {
    iTimer.Cancel();
    }
