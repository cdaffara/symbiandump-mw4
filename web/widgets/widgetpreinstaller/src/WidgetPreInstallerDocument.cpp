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


#include "WidgetPreInstaller.h"
#include "WidgetPreInstallerDocument.h"

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

// ---------------------------------------------------------
// CWidgetPreInstallerDocument::CWidgetPreInstallerDocument
// Constructor
//
// ---------------------------------------------------------
//
CWidgetPreInstallerDocument::CWidgetPreInstallerDocument(
    CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    }

// ---------------------------------------------------------
// CWidgetPreInstallerDocument::~CWidgetPreInstallerDocument
// Destructor
//
// ---------------------------------------------------------
//
CWidgetPreInstallerDocument::~CWidgetPreInstallerDocument()
    {
    }

// ---------------------------------------------------------
// CWidgetPreInstallerDocument::ConstructL
// Symbian constructor
//
// ---------------------------------------------------------
//
void CWidgetPreInstallerDocument::ConstructL()
    {
    }

// ---------------------------------------------------------
// CWidgetPreInstallerDocument::NewL
// Two-phase constructor
//
// ---------------------------------------------------------
//
CWidgetPreInstallerDocument* CWidgetPreInstallerDocument::NewL(
    CEikApplication& aApp )
    {
    CWidgetPreInstallerDocument* self = new ( ELeave ) CWidgetPreInstallerDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CWidgetPreInstallerDocument::CreateAppUiL()
// constructs Application
//
// ----------------------------------------------------
//
CEikAppUi* CWidgetPreInstallerDocument::CreateAppUiL()
    {
    return new ( ELeave ) CWidgetPreInstaller;
    }

// End of File
