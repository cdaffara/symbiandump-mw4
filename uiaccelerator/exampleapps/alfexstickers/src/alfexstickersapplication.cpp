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


#include "AlfExStickersDocument.h"
#include "AlfExStickersApplication.h"

// UID for the application, this should correspond to the uid defined in the mmp file
static const TUid KUidAlfExStickersApp = {0x20012480};


CApaDocument* CAlfExStickersApplication::CreateDocumentL()
    {  
    // Create an AlfExStickers document, and return a pointer to it
    CApaDocument* document = CAlfExStickersDocument::NewL(*this);
    return document;
    }


TUid CAlfExStickersApplication::AppDllUid() const
    {
    // Return the UID for the AlfExStickers application
    return KUidAlfExStickersApp;
    }

