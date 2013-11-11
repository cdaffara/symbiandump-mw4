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
* Description:  CAlfPerfAppApplication implementation.
*
*/


// INCLUDE FILES
#include <eikstart.h>

#include "alfperfappapplication.h"
#include "alfperfappdocument.h"

// ============================ MEMBER FUNCTIONS ===============================

CApaDocument* CAlfPerfAppApplication::CreateDocumentL()
    {
    CAlfPerfAppDocument* document = new (ELeave) CAlfPerfAppDocument( *this );
    CleanupStack::PushL( document );
    document->ConstructL();
    CleanupStack::Pop( document );
    return document;
    }

TUid CAlfPerfAppApplication::AppDllUid() const
    {
    return KAlfPerfAppUid;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

LOCAL_C CApaApplication* NewApplication()
    {
    return new CAlfPerfAppApplication;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

