/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container class for personalities
*
*/


#include <usbman.h>
#include <cusbpersonalitynotifier.h>
#include <tusbpersonalityparams.h>
#include "debug.h"

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
EXPORT_C TUsbPersonalityParams::TUsbPersonalityParams( RUsb& aUsbMan,
    CUsbPersonalityNotifier& aPersonalityNotifier )
    : iUsbMan( aUsbMan ),
      iPersonalityNotifier( aPersonalityNotifier )
    {
    LOG_FUNC
	
    }
    
// ---------------------------------------------------------------------------
// C++ default destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C TUsbPersonalityParams::~TUsbPersonalityParams()
    {
    LOG_FUNC
	
    }
    
// ---------------------------------------------------------------------------
// Handle to USB Manager.
// ---------------------------------------------------------------------------
//
EXPORT_C RUsb& TUsbPersonalityParams::UsbMan() const
    {
    LOG_FUNC
	
    return iUsbMan;
    }
    
// ---------------------------------------------------------------------------
// Handle to personality notifier.
// ---------------------------------------------------------------------------
//
EXPORT_C CUsbPersonalityNotifier& TUsbPersonalityParams::PersonalityNotifier() 
	const
    {
    LOG_FUNC
	
    return iPersonalityNotifier;    
    }

// ---------------------------------------------------------------------------
// Set personality id. Set is done by CUsbActivePersonalityHandler.
// ---------------------------------------------------------------------------
//
EXPORT_C void TUsbPersonalityParams::SetPersonalityId( TInt aPersonalityId )
    {
    LOG_FUNC
	
    iPersonalityId = aPersonalityId;
    }

// ---------------------------------------------------------------------------
// Currently loaded personality's id.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TUsbPersonalityParams::PersonalityId() const
    {
    LOG_FUNC
	
    return iPersonalityId;
    }

// End of file
