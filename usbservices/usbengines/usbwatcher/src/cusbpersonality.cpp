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
* Description:  Base class for personalitites
*
*/


#include <usbman.h>
#include <usbuinotif.h>
#include <cusbpersonality.h>
#include <tusbpersonalityparams.h>
#include "debug.h"

// ---------------------------------------------------------------------------
// C++ constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
EXPORT_C CUsbPersonality::CUsbPersonality( TUsbPersonalityParams&
        aPersonalityParams )
    : CActive( EPriorityStandard )
    , iPersonalityParams( aPersonalityParams )
    {
    LOG_FUNC

    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CUsbPersonality::~CUsbPersonality()
    {
    LOG_FUNC

    }

// ---------------------------------------------------------------------------
// Called by CUsbActivePersonalityHandler.
// DEPRICATED
// ---------------------------------------------------------------------------
//
EXPORT_C void CUsbPersonality::ShowUsbConnectionNote()
    {
    LOG_FUNC

    }
    
// End of file

