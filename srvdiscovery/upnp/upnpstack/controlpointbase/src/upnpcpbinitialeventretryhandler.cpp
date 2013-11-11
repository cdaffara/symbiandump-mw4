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
* Description:  
*
*/

#include "upnpcpbinitialeventretry.h"
#include "upnpcpbinitialeventretryhandler.h"
#include "upnpcontrolpoint.h"

static const TInt KCpInitialEventTimeout = 1000000; 

// -----------------------------------------------------------------------------
// CUpnpCpbInitialEventRetryHandler::CUpnpCpbInitialEventRetryHandler
//
// -----------------------------------------------------------------------------
//
CUpnpCpbInitialEventRetryHandler::CUpnpCpbInitialEventRetryHandler( CUpnpControlPoint& aCp ):
iControlPoint( aCp )    
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CUpnpCpbInitialEventRetryHandler::CUpnpCpbInitialEventRetryHandler
//
// -----------------------------------------------------------------------------
//
CUpnpCpbInitialEventRetryHandler::~CUpnpCpbInitialEventRetryHandler()
    {
    iList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CUpnpCpbInitialEventRetryHandler::AddL
// Ownership of the argument is passed through
// -----------------------------------------------------------------------------
//
void CUpnpCpbInitialEventRetryHandler::AddL( CUpnpGenaMessage* aMsg )
    {
    //ownership is passed through
    CUpnpCpbInitialEventRetry* timer = CUpnpCpbInitialEventRetry::NewLC( aMsg, *this );    
    iList.AppendL( timer ); 
    CleanupStack::Pop( timer );
    timer->After( KCpInitialEventTimeout );
    }

// -----------------------------------------------------------------------------
// CUpnpCpbInitialEventRetryHandler::TimerExpired
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbInitialEventRetryHandler::TimerExpired( CUpnpCpbInitialEventRetry* aTimer )
    {
    TInt index = iList.Find( aTimer );
    ASSERT( index != KErrNotFound );
    iList.Remove( index );          
    TRAP_IGNORE( iControlPoint.HandlePostponedInitialEventL( aTimer->Message() ) ) ;
    delete aTimer;        
    }




