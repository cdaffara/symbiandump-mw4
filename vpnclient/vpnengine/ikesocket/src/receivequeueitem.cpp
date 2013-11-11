/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Receive queue item
*
*/


// INCLUDE FILES
#include "receivequeueitem.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
TReceiveQueueItem::TReceiveQueueItem( HBufC8* aUdpData, 
                                      const TInetAddr& aSrcAddr,
                                      const TInt aLocalPort ) 
 :  iUdpData( aUdpData ),
    iSrcAddr( aSrcAddr ),
    iLocalPort( aLocalPort ) 
    {
    }

// ---------------------------------------------------------------------------
// Returns UDP data.
// ---------------------------------------------------------------------------
//
HBufC8* TReceiveQueueItem::UdpData() const
    {
    return iUdpData;
    }

// ---------------------------------------------------------------------------
// Returns source address.
// ---------------------------------------------------------------------------
//
const TInetAddr& TReceiveQueueItem::SrcAddr() const
    {
    return iSrcAddr;
    }

// ---------------------------------------------------------------------------
// Returns local port.
// ---------------------------------------------------------------------------
//
TInt TReceiveQueueItem::LocalPort() const
    {
    return iLocalPort;
    }


