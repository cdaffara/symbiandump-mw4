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


#include "WidgetStartup.h"
#include "WidgetStartupDocument.h"

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
// CWidgetStartupDocument::CWidgetStartupDocument
// Constructor
//
// ---------------------------------------------------------
//
CWidgetStartupDocument::CWidgetStartupDocument(
    CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    }

// ---------------------------------------------------------
// CWidgetStartupDocument::~CWidgetStartupDocument
// Destructor
//
// ---------------------------------------------------------
//
CWidgetStartupDocument::~CWidgetStartupDocument()
    {
    }

// ---------------------------------------------------------
// CWidgetStartupDocument::ConstructL
// Symbian constructor
//
// ---------------------------------------------------------
//
void CWidgetStartupDocument::ConstructL()
    {
    }

// ---------------------------------------------------------
// CWidgetStartupDocument::NewL
// Two-phase constructor
//
// ---------------------------------------------------------
//
CWidgetStartupDocument* CWidgetStartupDocument::NewL(
    CEikApplication& aApp )
    {
    CWidgetStartupDocument* self = new ( ELeave ) CWidgetStartupDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CWidgetStartupDocument::CreateAppUiL()
// constructs Application
//
// ----------------------------------------------------
//
CEikAppUi* CWidgetStartupDocument::CreateAppUiL()
    {
    return new ( ELeave ) CWidgetStartup;
    }

// End of File
