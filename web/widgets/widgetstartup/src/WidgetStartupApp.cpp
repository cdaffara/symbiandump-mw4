/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Source file for Application class
*
*/


// INCLUDE FILES

#include "WidgetStartupDocument.h"
#include "WidgetStartupApp.h"
#include <eikstart.h>


// LOCAL FUNCTION PROTOTYPES

// LOCAL CONSTANTS

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWidgetStartupApplication::CreateDocumentL
// Creates a CNTSubHandlerAppDocument object fully constructed and 
// returns a pointer to it.
// (other items were commented in a header).
// ---------------------------------------------------------
//

CApaDocument* CWidgetStartupApplication::CreateDocumentL()
    {
    return CWidgetStartupDocument::NewL(*this);
    }

// ---------------------------------------------------------
// CNTSubHandlerAppApplication::AppDllUid
// returns the UID of NTSubHandlerApp application
// (other items were commented in a header).
// ---------------------------------------------------------
//
TUid CWidgetStartupApplication::AppDllUid() const
    {
    return KUidWidgetStartupApp;
    }


// ---------------------------------------------------------
// NewApplication
// The entry point for the application code. It creates
// an instance of the CApaApplication derived
// class, CWidgetPreInstaller.
// Returns: pointer to CApaApplication
// ---------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CWidgetStartupApplication;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of File
