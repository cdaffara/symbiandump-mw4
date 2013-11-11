/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: VPN Connection Agent progresses.
*
*/



/**  
 @file
 @released
 @publishedPartner
 
 VPN Connection Agent progress parameter declaration.
 */

#if !defined __VPNCONNAGTPROG_H__
#define __VPNCONNAGTPROG_H__

// INCLUDES
#include <nifvar.h>
#include <comms-infras/cagentbase.h>

/**
 * Defines progresses sent to Nifman.
 */
enum TVPNConnAgtProgress
    {
    EVPNConnAgtIdle             = KMinAgtProgress,
    EVPNConnAgtConnecting,
    EVPNConnAgtConnected        = KConnectionOpen,
    EVPNConnAgtDisconnecting    = KConnectionStartingClose,
    EVPNConnAgtDisconnected     = KConnectionClosed
    };

#endif // VPNCONNAGTPROG_H
