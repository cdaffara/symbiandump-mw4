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
* Description:  Monitoring of IP Event Notification from ipeventnotifier module
*
*/


#include "upnpipeventnotifier.h"
#include <networking/ipeventtypesids.h>

#ifdef _DEBUG
#define KLogFile _L("UPnPStack.txt")
#endif
#include "upnpcustomlog.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CUpnpIPEventNotifier::NewL
// ---------------------------------------------------------------------------
//
CUpnpIPEventNotifier* CUpnpIPEventNotifier::NewL( TInt aInterfaceIndex )
    {
    CUpnpIPEventNotifier* self = new (ELeave) CUpnpIPEventNotifier;
    CleanupStack::PushL( self );
    self->ConstructL( aInterfaceIndex );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpnpIPEventNotifier::CUpnpIPEventNotifier
// ---------------------------------------------------------------------------
//
CUpnpIPEventNotifier::CUpnpIPEventNotifier() : CActive( EPriorityStandard )
    {
    }

// ---------------------------------------------------------------------------
// CUpnpIPEventNotifier::ConstructL
// ---------------------------------------------------------------------------
//    
void CUpnpIPEventNotifier::ConstructL( TInt aInterfaceIndex )
    {
    User::LeaveIfError( iProperty.Attach( TUid::Uid( IPEvent::EIPReady ), 
                                          aInterfaceIndex ) );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CUpnpIPEventNotifier::~CUpnpIPEventNotifier
// ---------------------------------------------------------------------------
//    
CUpnpIPEventNotifier::~CUpnpIPEventNotifier()
    {
    Cancel();
    iProperty.Close();
    }

// ---------------------------------------------------------------------------
// CUpnpIPEventNotifier::DoCancel
// ---------------------------------------------------------------------------
//    
void CUpnpIPEventNotifier::DoCancel()
    {
    iProperty.Cancel();
    }
    
// ---------------------------------------------------------------------------
// CUpnpIPEventNotifier::Subscribe
// ---------------------------------------------------------------------------
//    
void CUpnpIPEventNotifier::Subscribe( MUpnpIPEventObserver* aObserver )
    {
    iObserver = aObserver; 
	if(!IsActive())
		{
		iProperty.Subscribe( iStatus );
	    SetActive();        
		}
    }
    
// ---------------------------------------------------------------------------
// CUpnpIPEventNotifier::RunL
// ---------------------------------------------------------------------------
//    
void CUpnpIPEventNotifier::RunL()
    {
    if ( iStatus.Int() == KErrNone ) 
        {
        iProperty.Subscribe( iStatus );
        SetActive();        
        iObserver->IPEventOccursL();        
        }    
    }

// ---------------------------------------------------------------------------
// CUpnpIPEventNotifier::RunError
// ---------------------------------------------------------------------------
//  
TInt CUpnpIPEventNotifier::RunError( TInt /*aErr*/ )
    {
    LOG_FUNC_NAME;
    return KErrNone;
    }

