/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of member functions of 
*                CAlfExAnalogDialerDocument.
*
*/



#include "alfexanalogdialerdocument.h"
#include "alfexanalogdialerappui.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfExAnalogDialerDocument* CAlfExAnalogDialerDocument::NewL(CEikApplication& aApp)
    {
    CAlfExAnalogDialerDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CAlfExAnalogDialerDocument* CAlfExAnalogDialerDocument::NewLC(CEikApplication& aApp)
    {
    CAlfExAnalogDialerDocument* self = new (ELeave) CAlfExAnalogDialerDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// ~CAlfExAnalogDialerDocument
// ---------------------------------------------------------------------------
//
CAlfExAnalogDialerDocument::~CAlfExAnalogDialerDocument()
    {
	// no implementation required
    }
    
// ---------------------------------------------------------------------------
// CreateAppUiL
//     // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object

// ---------------------------------------------------------------------------
//
CEikAppUi* CAlfExAnalogDialerDocument::CreateAppUiL()
    {
    CEikAppUi* appUi = new (ELeave) CAlfExAnalogDialerAppUi;
    return appUi;
    }

// ---------------------------------------------------------------------------
// CAlfExAnalogDialerDocument
// ---------------------------------------------------------------------------
//
CAlfExAnalogDialerDocument::CAlfExAnalogDialerDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
	// no implementation required
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfExAnalogDialerDocument::ConstructL()
    {
	// no implementation required
    }    


