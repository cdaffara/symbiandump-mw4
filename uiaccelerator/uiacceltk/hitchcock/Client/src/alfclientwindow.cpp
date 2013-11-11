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
* Name        : alfclientwindow.cpp
* Part of     : Alfred UI Toolkit / alfclient.dll
* Description : AlfClientWindow class to post drawing commands to
*               serverside canvas
*
*/

#include <alf/alfenv.h>
#include <alf/alfclientwindow.h>
#include "alfclient.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//

EXPORT_C CAlfClientWindow* CAlfClientWindow::NewL( CAlfEnv& aEnv, RWindow& aWindow )
    {
    CAlfClientWindow* self = CAlfClientWindow::NewLC( aEnv, aWindow );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//

CAlfClientWindow* CAlfClientWindow::NewLC( CAlfEnv& aEnv, RWindow& aWindow )
    {
    CAlfClientWindow* self = new(ELeave)CAlfClientWindow;
    CleanupStack::PushL(self);
    self->ConstructL( aEnv, aWindow );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfClientWindow::CAlfClientWindow()
    {    
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//

void CAlfClientWindow::ConstructL( CAlfEnv& aEnv, RWindow& aWindow )
    {
    iEnv = &aEnv;
    iRWindow = &aWindow;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//

EXPORT_C CAlfClientWindow::~CAlfClientWindow()
    {    
    }

// ---------------------------------------------------------------------------
// PostCommandsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfClientWindow::PostCommandsL( TDes8& aCommandBuffer, TAlfCommandBufferStatus aStatus )
    {
    User::LeaveIfError(iEnv->Client().PostQtBuffer( iRWindow->WindowGroupId(),
            iRWindow->ClientHandle(), aCommandBuffer, aStatus ));
    }
    
