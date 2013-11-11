/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of Notifications receiving
*
*/



// INCLUDE FILES
#include "hssnotif.h"
#include "hssinterface.h"
#include "am_debug.h"
#include "hotspotsession.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HssNotifications::NewNetworksDetected
// New networks have been detected during scan.
// -----------------------------------------------------------------------------
void HssNotifications::NewNetworksDetected()
	{
    DEBUG( "HssNotifications::NewNetworksDetected()" );
    TBuf8<1> tmp;
    iSession.AddNotification( EHssNewNetworksDetected, tmp );
	}

// -----------------------------------------------------------------------------
// HssNotifications::OldNetworksLost
// One or more networks have been lost since the last scan.
// -----------------------------------------------------------------------------
void HssNotifications::OldNetworksLost()
	{
	DEBUG( "HssNotifications::OldNetworksLost()" );
	TBuf8<1> tmp;
	iSession.AddNotification( EHssOldNetworksLost, tmp ); 
	}

// -----------------------------------------------------------------------------
// CSessionNotification::CSessionNotification
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSessionNotification::CSessionNotification(
    CHotSpotSession& aSession ) :
    iSession( aSession )
    {
    DEBUG( "CSessionNotification::CSessionNotification()" );
    }

// -----------------------------------------------------------------------------
// CSessionNotification::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CSessionNotification* CSessionNotification::NewL(
    CHotSpotSession& aSession )
    {
    DEBUG( "CSessionNotification::NewL()" );
    CSessionNotification* self =
        new( ELeave ) CSessionNotification( aSession );
    return self;
    }

// Destructor
CSessionNotification::~CSessionNotification()
    {
    DEBUG( "CSessionNotification::~CSessionNotification()" );
    }

// -----------------------------------------------------------------------------
// CSessionNotification::AddNotification
// -----------------------------------------------------------------------------
void CSessionNotification::AddNotification(
    TUint aNotification,
    TDes8& aData )
    {
    DEBUG( "CSessionNotification::AddNotification()" );
    iSession.AddNotification( aNotification, aData );
    }

//  End of File  
