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
#include "upnpgenamessage.h"

// -----------------------------------------------------------------------------
// CUpnpCpbInitialEventRetry::NewLC
// Ownership of aMessage is passed through 
// -----------------------------------------------------------------------------
//
CUpnpCpbInitialEventRetry* CUpnpCpbInitialEventRetry::NewLC( 
        CUpnpGenaMessage* aMessage, CUpnpCpbInitialEventRetryHandler& iOwner )
    {
    CleanupStack::PushL( aMessage );
    //ownership is passed through
    CUpnpCpbInitialEventRetry* self = new ( ELeave ) CUpnpCpbInitialEventRetry( aMessage, iOwner );
    CleanupStack::Pop( aMessage );
    CleanupStack::PushL( self ); 
    self->ConstructL();
    return self;
    }

        
// -----------------------------------------------------------------------------
// CUpnpCpbInitialEventRetry::CUpnpCpbInitialEventRetry
//
// -----------------------------------------------------------------------------
//
CUpnpCpbInitialEventRetry::CUpnpCpbInitialEventRetry( 
            CUpnpGenaMessage* aMessage, CUpnpCpbInitialEventRetryHandler& iOwner ): 
            CTimer(CActive::EPriorityStandard), iMessage( aMessage ), iOwner( iOwner )
    {   
    CActiveScheduler::Add( this );
    }        

// -----------------------------------------------------------------------------
// CUpnpCpbInitialEventRetry::ConstrucL
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbInitialEventRetry::ConstrucL()
    {
    CTimer::ConstructL();
    }

// -----------------------------------------------------------------------------
// CUpnpCpbInitialEventRetry::RunL
//
// -----------------------------------------------------------------------------
//
void CUpnpCpbInitialEventRetry::RunL()
   {           
   iOwner.TimerExpired( this );
   }

// -----------------------------------------------------------------------------
// CUpnpCpbInitialEventRetry::Message
//
// -----------------------------------------------------------------------------
//
CUpnpGenaMessage& CUpnpCpbInitialEventRetry::Message()
    {
    return *iMessage;
    }

// -----------------------------------------------------------------------------
// CUpnpCpbInitialEventRetry::~CUpnpCpbInitialEventRetry
//
// -----------------------------------------------------------------------------
//
CUpnpCpbInitialEventRetry::~CUpnpCpbInitialEventRetry()
    { 
    delete iMessage;
    }



