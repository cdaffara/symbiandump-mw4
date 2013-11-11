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
* Description:  Implementation of class CCodViewerApp.   
*
*/


// INCLUDE FILES

#include <CodDefs.h>
#include "CodViewerApp.h"
#include "CodDocument.h"

// CONSTANTS

/// UID of the application.
LOCAL_D const TUid KCodUidCodViewerApp = { COD_VIEWER_UID };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodViewerApp::CCodViewerApp()
// ---------------------------------------------------------
//
EXPORT_C CCodViewerApp::CCodViewerApp()
    {
    }

// ---------------------------------------------------------
// CCodViewerApp::CreateDocumentL()
// ---------------------------------------------------------
//
CApaDocument* CCodViewerApp::CreateDocumentL()
    {
    return CCodDocument::NewL( *this, /*aCod=*/ETrue );
    }

// ---------------------------------------------------------
// CCodViewerApp::AppDllUid()
// ---------------------------------------------------------
//
TUid CCodViewerApp::AppDllUid() const
    {
    return KCodUidCodViewerApp;
    }
