/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Implementation of class CSchemeApp.
*      
*
*/


// INCLUDE FILES
#include "SchemeApp.h"
#include "SchemeDocument.h"
#include "bldvariant.hrh"
#include <eikstart.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSchemeApp::CreateDocumentL()
// ---------------------------------------------------------
//
CApaDocument* CSchemeApp::CreateDocumentL()
    {
    return CSchemeDocument::NewL( *this );
    }

// ---------------------------------------------------------
// CSchemeApp::AppDllUid()
// ---------------------------------------------------------
//
TUid CSchemeApp::AppDllUid() const
    {
    return KSchemeApp;
    }

LOCAL_C CApaApplication* NewApplication()
	{
	return new CSchemeApp;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}

// End of File  

