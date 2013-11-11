/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include    "WidgetUiApp.h"
#include    "WidgetUiDocument.h"
#include    "MemoryManager.h"
#include    "widgetappdefs.rh"
#include    <eikstart.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWidgetUiApp::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CWidgetUiApp::AppDllUid() const
    {
    return KUidWidgetUi;
    }

// -----------------------------------------------------------------------------
// CWidgetUiApp::OpenIniFileLC()
// overrides CAknApplication::OpenIniFileLC to enable INI file support
// -----------------------------------------------------------------------------
//
CDictionaryStore* CWidgetUiApp::OpenIniFileLC(
    RFs& aFs ) const
    {
    return CEikApplication::OpenIniFileLC( aFs );
    }

// -----------------------------------------------------------------------------
// CWidgetUiApp::CreateDocumentL()
// Creates CWidgetUiDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CWidgetUiApp::CreateDocumentL()
    {
    return CWidgetUiDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

// -----------------------------------------------------------------------------
// NewApplication
//
//
// -----------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication( )
    {
    return new CWidgetUiApp;
    }

// -----------------------------------------------------------------------------
// E32Main
//
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    RAllocator* oldAllocator = MemoryManager::SwitchToFastAllocator();
    __UHEAP_MARK;
    TInt result = EikStart::RunApplication( NewApplication );
    __UHEAP_MARKEND;
    MemoryManager::CloseFastAllocator(oldAllocator);
    return result;
    }

// End of File  
