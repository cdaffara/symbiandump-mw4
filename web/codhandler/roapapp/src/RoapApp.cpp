/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*      Implementation of class CRoapApp.   
*      
*
*/


// INCLUDE FILES

#include "RoapApp.h"
#include "RoapAppDocument.h"
#include <eikstart.h>

// CONSTANTS

/// UID of the application.
LOCAL_D const TUid KRoapAppUid = { 0x10008d64 };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRoapApp::CreateDocumentL()
// ---------------------------------------------------------
//
CApaDocument* CRoapApp::CreateDocumentL()
    {
    return CRoapAppDocument::NewL( *this );
    }


// ---------------------------------------------------------
// CRoapApp::AppDllUid()
// ---------------------------------------------------------
//
TUid CRoapApp::AppDllUid() const
    {
    return KRoapAppUid;
    }

// ================= OTHER FUNCTIONS ==============

/**
* Create application object.
* @return The created application object.
*/
LOCAL_C CApaApplication* NewApplication()
    {
    return new CRoapApp;
    }

/**
* Executable entry point.
* @return Error code.
*/
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }
