/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IKE send queue item
*
*/


// INCLUDE FILES
#include "ikesendqueueitem.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C TIkeSendQueueItem::TIkeSendQueueItem( HBufC8* aUdpData, 
                                               const TInetAddr& aDestAddr, 
                                               TInt aLocalPort,
                                               TUint8 aDscp )
 :  iUdpData( aUdpData ),
    iDestAddr( aDestAddr ),
    iLocalPort( aLocalPort ),
    iDscp( aDscp )
    {
    }

// ---------------------------------------------------------------------------
// Constructor with message identification parameter. Constructs object
// which should be used for matching with Match() method.
// ---------------------------------------------------------------------------
//
EXPORT_C TIkeSendQueueItem::TIkeSendQueueItem( HBufC8* aUdpData )
 :  iUdpData( aUdpData ),
    iDestAddr( TInetAddr() ),
    iLocalPort( 0 ),
    iDscp( 0 )
    {    
    }

// ---------------------------------------------------------------------------
// Determines whether two objects match.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TIkeSendQueueItem::Match( const TIkeSendQueueItem& aItemOne,
                                         const TIkeSendQueueItem& aItemTwo )
	{
	if ( aItemOne.iUdpData == aItemTwo.iUdpData )
		{
		return ETrue;
		}
	return EFalse;
	}

// ---------------------------------------------------------------------------
// Gets UDP data.
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* TIkeSendQueueItem::UdpData()
    {
    return iUdpData;
    }       

// ---------------------------------------------------------------------------
// Gets destination IP address.
// ---------------------------------------------------------------------------
//
EXPORT_C const TInetAddr& TIkeSendQueueItem::DestAddr()
    {
    return iDestAddr;
    }                                

// ---------------------------------------------------------------------------
// Gets local port.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TIkeSendQueueItem::LocalPort() const
    {
    return iLocalPort;
    }    

// ---------------------------------------------------------------------------
// Gets DSCP value.
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 TIkeSendQueueItem::Dscp() const
    {
    return iDscp;
    }    
