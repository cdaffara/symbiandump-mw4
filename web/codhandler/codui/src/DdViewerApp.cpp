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
* Description:  Implementation of class CDdViewerApp.   
*
*/


// INCLUDE FILES

#include <CodDefs.h>
#include "DdViewerApp.h"
#include "CodDocument.h"

// CONSTANTS

/// UID of the application.
LOCAL_D const TUid KCodUidDdViewerApp = { DD_VIEWER_UID };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDdViewerApp::CDdViewerApp()
// ---------------------------------------------------------
//
EXPORT_C CDdViewerApp::CDdViewerApp()
    {
    }

// ---------------------------------------------------------
// CDdViewerApp::CreateDocumentL()
// ---------------------------------------------------------
//
CApaDocument* CDdViewerApp::CreateDocumentL()
    {
    return CCodDocument::NewL( *this, /*aCod=*/EFalse );
    }

// ---------------------------------------------------------
// CDdViewerApp::AppDllUid()
// ---------------------------------------------------------
//
TUid CDdViewerApp::AppDllUid() const
    {
    return KCodUidDdViewerApp;
    }
