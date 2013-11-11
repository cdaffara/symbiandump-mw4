/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CUpnpMSearchTimer
*
*/


// INCLUDE FILES

#include "upnpnotifytimer.h"
#include "upnpmsearchtimer.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpMSearchTimer::CUpnpMSearchTimer
// C++ default constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpMSearchTimer::CUpnpMSearchTimer(MUpnpNotifyTimerObserver* aObserver)
            : CUpnpNotifyTimer(aObserver)
	{
	}
// ----------------------------------------------------------------------------
// CUpnpMSearchTimer::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpMSearchTimer* CUpnpMSearchTimer::NewL(MUpnpNotifyTimerObserver* aObserver, 
                                const TDesC8& aTarget,
                                const TDesC8& aMX)
    {
    CUpnpMSearchTimer* self  = CUpnpMSearchTimer::NewLC(aObserver,aTarget,aMX);
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CUpnpMSearchTimer::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpMSearchTimer* CUpnpMSearchTimer::NewLC(MUpnpNotifyTimerObserver* aObserver, 
                                const TDesC8& aTarget,
                                const TDesC8& aMX)
    {
    CUpnpMSearchTimer* self  = new (ELeave) CUpnpMSearchTimer(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL(aTarget, aMX);
    return self;
    }

// ----------------------------------------------------------------------------
// CUpnpMSearchTimer::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CUpnpMSearchTimer::ConstructL(const TDesC8& aTarget,
                                const TDesC8& aMX)
    {
    CUpnpNotifyTimer::ConstructL();
    iTarget = aTarget.AllocL();
    iMXString = aMX.AllocL();
    TLex8 tmp(*iMXString);
    iMX = tmp;
    }
// -----------------------------------------------------------------------------
// CUpnpMSearchTimer::~CUpnpMSearchTimer
// C++ default destructor
// -----------------------------------------------------------------------------
//
CUpnpMSearchTimer::~CUpnpMSearchTimer()
    {
    delete iTarget;
    delete iMXString;
    }

EXPORT_C TInt CUpnpMSearchTimer::GetNextValue()
    {
    TInt i;
    TInt res = iMX.Val(i);
    if(res)
        {
        return res;	        
        }
    for(;;)
        {
        if (!iMX.Get().IsDigit() || iMX.Get() == ',')
            {
            break;
            }
        }
    return i;
    }

EXPORT_C TPtrC8 CUpnpMSearchTimer::GetTarget()
    {
    return *iTarget;
    }

// End Of File
