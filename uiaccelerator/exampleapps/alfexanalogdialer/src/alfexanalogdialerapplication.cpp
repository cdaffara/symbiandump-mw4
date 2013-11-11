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
* Description:   Implementation of member functions of CAlfExAnalogDialerApplication.
*
*/



#include "alfexanalogdialerapplication.h"
#include "alfexanalogdialerdocument.h"

// UID of the application. It must be same as in MMP file 
static const TUid KUidAlfExAnalogDialerApp = {0x2001247F};

// ---------------------------------------------------------------------------
// AppDllUid
// Return the UID of the Analog Dialer application
// ---------------------------------------------------------------------------
//
TUid CAlfExAnalogDialerApplication::AppDllUid() const
    {
    return KUidAlfExAnalogDialerApp;
    }

// ---------------------------------------------------------------------------
// CreateDocumentL
// Creates document class of Analog Dialer and returns pointer to it. 
// ---------------------------------------------------------------------------
//
CApaDocument* CAlfExAnalogDialerApplication::CreateDocumentL()
    {  
    CApaDocument* document = CAlfExAnalogDialerDocument::NewL(*this);
    return document;
    }


