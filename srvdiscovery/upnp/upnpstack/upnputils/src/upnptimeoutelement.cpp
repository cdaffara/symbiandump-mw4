/** @file
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CUpnpTimeoutElement
*
*/



// INCLUDE FILES
#include <e32base.h>
#include "upnptimeoutelement.h"
#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpTimeoutElement::CUpnpTimeoutElement
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpTimeoutElement::CUpnpTimeoutElement(MUpnpTimeoutElementParent& aParent)
    : CActive(EPriorityNormal), iParent(aParent)
    {
    CActiveScheduler::Add( this );
    iRenew = EOnce;
    }

// ----------------------------------------------------------------------------
// CUpnpTimeoutElement::BaseConstructL
// Base class constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpTimeoutElement::BaseConstructL()
    {
    TInt err = iTimer.CreateLocal();
    if ( KErrNone != err )
        {
        LOGS1("CUpnpTimeoutElement::CUpnpTimeoutElement() CreateLocal FAILED: %d", err );
        User::Leave( err );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpTimeoutElement::~CUpnpTimeoutElement
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpTimeoutElement::~CUpnpTimeoutElement() 
    {
    Cancel();
    iTimer.Close();
    }
    
// -----------------------------------------------------------------------------
// CUpnpTimeoutElement::SetTimeout
// Set timeout and start timer
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpTimeoutElement::SetTimeout( TInt aSeconds )
    {
    iSeconds=aSeconds;
    if (iSeconds>KTime)
        {
        TUint32 microsec = KTime * KMicroInSec;
        if ( IsActive() )
           {
           Cancel();
           }
        iTimer.After(iStatus, TTimeIntervalMicroSeconds32(microsec));
    	SetActive();
        iState = ELongTime;       
        }    
    else
        {
        TUint32 microsec = aSeconds * KMicroInSec;
        if ( IsActive() )
            {
            Cancel();
            }
    	iTimer.After(iStatus, TTimeIntervalMicroSeconds32(microsec));
    	SetActive();
        iState = EAlive;    
        }    
    }

// -----------------------------------------------------------------------------
// CUpnpTimeoutElement::RunL
// When timer expires, RunL is called.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpTimeoutElement::RunL()
    {
    if (iStatus.Int() == KErrNone)
        {
        switch( iState )
            {
            case ELongTime:
                 if (iSeconds>KTime) iSeconds=iSeconds-KTime;                 
                 SetTimeout(iSeconds);
                 break;
            case EAlive:
                TRAP_IGNORE( iParent.TimeoutExpiredL( this ) );
                break;
            case EDead:
            default:
                
                break;
            }
        }
        else
        {
        LOGS1("CUpnpTimeoutElement::CUpnpTimeoutElement() RunL iStatus  %d", iStatus.Int() );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpTimeoutElement::DoCancel
// Cancel active request.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpTimeoutElement::DoCancel()
    {
    iTimer.Cancel();
    }

// -----------------------------------------------------------------------------
// CUpnpTimeoutElement::RunError
// Cancel active request.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpTimeoutElement::RunError( TInt aError )
    {
    LOGS1("CUpnpTimeoutElement::RunError, error %d",aError);
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CUpnpTimeoutElement::SetRenew
// Set renew.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpTimeoutElement::SetRenew( TRenew aRenew )
    {
    iRenew = aRenew;
    }

// -----------------------------------------------------------------------------
// CUpnpTimeoutElement::Renew
// Return renew.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpTimeoutElement::TRenew& CUpnpTimeoutElement::Renew()
    {
    return iRenew;
    }

//  End of File
