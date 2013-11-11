/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ?Description
*
*/



#include "alfexcalendarappui.h"
#include "alfexcalendardocument.h"

// Standard Symbian OS construction sequence
CAlfExCalendarDocument* CAlfExCalendarDocument::NewL(CEikApplication& aApp)
    {
    CAlfExCalendarDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

CAlfExCalendarDocument* CAlfExCalendarDocument::NewLC(CEikApplication& aApp)
    {
    CAlfExCalendarDocument* self = new (ELeave) CAlfExCalendarDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CAlfExCalendarDocument::ConstructL()
    {
	// no implementation required
    }    

CAlfExCalendarDocument::CAlfExCalendarDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
	// no implementation required
    }

CAlfExCalendarDocument::~CAlfExCalendarDocument()
    {
	// no implementation required
    }

CEikAppUi* CAlfExCalendarDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new (ELeave) CAlfExCalendarAppUi;
    return appUi;
    }

