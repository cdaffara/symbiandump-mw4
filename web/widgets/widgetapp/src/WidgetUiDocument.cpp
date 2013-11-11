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


#include "WidgetUiDocument.h"
#include "WidgetUiAppUi.h"

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
// CWidgetUiDocument::CWidgetUiDocument
// Constructor
//
// ---------------------------------------------------------
//
CWidgetUiDocument::CWidgetUiDocument(
    CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    }

// ---------------------------------------------------------
// CWidgetUiDocument::~CWidgetUiDocument
// Destructor
//
// ---------------------------------------------------------
//
CWidgetUiDocument::~CWidgetUiDocument()
    {
    }

// ---------------------------------------------------------
// CWidgetUiDocument::NewL
// Two-phase constructor
//
// ---------------------------------------------------------
//
CWidgetUiDocument* CWidgetUiDocument::NewL(
    CEikApplication& aApp )
    {
    CWidgetUiDocument* self = new ( ELeave ) CWidgetUiDocument( aApp );
    return self;
    }

// ----------------------------------------------------
// CWidgetUiDocument::CreateAppUiL()
// constructs CBrCtlDemoAppUi
//
// ----------------------------------------------------
//
CEikAppUi* CWidgetUiDocument::CreateAppUiL()
    {
    return new ( ELeave ) CWidgetUiAppUi;
    }

// End of File
