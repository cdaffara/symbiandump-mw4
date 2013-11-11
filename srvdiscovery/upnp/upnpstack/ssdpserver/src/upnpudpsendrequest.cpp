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
* Description:  CUpnpUdpSendRequest
*
*/


// INCLUDE FILES
#include <in_sock.h>
#include "upnpudpsendrequest.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpUdpSendRequest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpUdpSendRequest* CUpnpUdpSendRequest::NewL( TDesC8& aBuffer,
    TInetAddr& anAddr )
    {
    CUpnpUdpSendRequest* self = new (ELeave) CUpnpUdpSendRequest( anAddr );
    CleanupStack::PushL( self );
    self->ConstructL( aBuffer );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpUdpSendRequest::CUpnpUdpSendRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpUdpSendRequest::CUpnpUdpSendRequest( const TInetAddr& anAddr )
    : iAddr( anAddr )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpUdpSendRequest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpUdpSendRequest::ConstructL( const TDesC8& aBuffer )
    {
    iBuffer = aBuffer.AllocL();
    }

// -----------------------------------------------------------------------------
// CUpnpUdpSendRequest::~CUpnpUdpSendRequest
// Destructor
// -----------------------------------------------------------------------------
// 
CUpnpUdpSendRequest::~CUpnpUdpSendRequest()
    {
    delete iBuffer;
    }

//  End of File
