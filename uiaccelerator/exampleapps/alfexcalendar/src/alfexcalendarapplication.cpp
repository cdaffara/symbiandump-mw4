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



#include "alfexcalendardocument.h"
#include "alfexcalendarapplication.h"

// UID for the application, this should correspond to the uid defined in the mmp file
static const TUid KUidAlfExCalendarApp = {0x0FCD4AC7};

CApaDocument* CAlfExCalendarApplication::CreateDocumentL()
    {  
    // Create an AlfExCalendar document, and return a pointer to it
    CApaDocument* document = CAlfExCalendarDocument::NewL(*this);
    return document;
    }

TUid CAlfExCalendarApplication::AppDllUid() const
    {
    // Return the UID for the AlfExCalendar application
    return KUidAlfExCalendarApp;
    }

