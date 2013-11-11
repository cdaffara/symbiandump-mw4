/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Document definition
*
*/



#include <apgwgnam.h>
#include "alfdocument.h"
#include "alfapplication.h"
#include "alf/alfappui.h"


_LIT(KSERVERNAME, "ALF");
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfDocument::CAlfDocument( CAlfApplication& aApplication )
: CAknDocument( aApplication )
    {
    }

// ---------------------------------------------------------------------------
// From class CAknDocument.
// Allocates the appui instance.
// ---------------------------------------------------------------------------
//
CEikAppUi* CAlfDocument::CreateAppUiL()
    {
    User::Leave(KErrNotSupported);    
    return 0;
    }


// This is currently only place where we can modify CEikonEnv's instance of wgname
// that would override our custom settings in AppReady
void CAlfDocument::UpdateTaskNameL(CApaWindowGroupName* aWgName)
    {
    aWgName->SetHidden(ETrue); // hides us from FSW and protects us from OOM FW etc.
    aWgName->SetSystem(ETrue); // Allow only application with PowerManagement cap to shut us down    
    aWgName->SetCaptionL(KSERVERNAME); // user friendly name of server
    }
