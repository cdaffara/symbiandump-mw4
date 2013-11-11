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
* Description:   
*
*/


#include "AlfExStickersAppUi.h"
#include "AlfExStickersDocument.h"

// Standard Symbian OS construction sequence
CAlfExStickersDocument* CAlfExStickersDocument::NewL(CEikApplication& aApp)
    {
    CAlfExStickersDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }


CAlfExStickersDocument* CAlfExStickersDocument::NewLC(CEikApplication& aApp)
    {
    CAlfExStickersDocument* self = new (ELeave) CAlfExStickersDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


CAlfExStickersDocument::CAlfExStickersDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
	// no implementation required
    }


void CAlfExStickersDocument::ConstructL()
    {
	// no implementation required
    }    


CAlfExStickersDocument::~CAlfExStickersDocument()
    {
	// no implementation required
    }


CEikAppUi* CAlfExStickersDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new (ELeave) CAlfExStickersAppUi;
    return appUi;
    }

