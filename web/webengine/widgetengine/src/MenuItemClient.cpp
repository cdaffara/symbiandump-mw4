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
* Description:  This class represents the Widget Extension object
*
*/

// INCLUDE FILES
#include "MenuItemClient.h"
#include "WidgetEventHandler.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ----------------------------------------------------------------------------
// CMenuItem::NewL
//
//
//
// ----------------------------------------------------------------------------
//
CMenuItem* CMenuItem::NewL(
    const TDesC16& aText, TInt aCmdId, TInt aInternalId, TBool aDimmed, 
    CMenuItem* aParent, void* aObj, TBool aShow )
{
    CMenuItem* self = new ( ELeave ) CMenuItem( 
        aText, aCmdId, aInternalId, aDimmed, aParent, aObj, aShow );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// ----------------------------------------------------------------------------
// CMenuItem::CMenuItem
//
//
//
// ----------------------------------------------------------------------------
//
CMenuItem::CMenuItem(
    const TDesC16& aText, TInt aCmdId, TInt aInternalId, TBool aDimmed, 
    CMenuItem* aParent, void* aObj, TBool aShow )
    :iCmdId( aCmdId ),
    iInternalId( aInternalId ),
    iParent( aParent ),
    iDimmed( aDimmed ),    
    iObj( aObj ),
    iShow( aShow )
{
    iText = aText.Alloc();
    iFirstChild = NULL;
    iNextSibling = NULL;
    iCallback = NULL;
}

// ----------------------------------------------------------------------------
// CMenuItem::ConstructL
//
//
//
// ----------------------------------------------------------------------------
//
void CMenuItem::ConstructL()
{
}

// ----------------------------------------------------------------------------
// CMenuItem::~CMenuItem
//
//
//
// ----------------------------------------------------------------------------
//
CMenuItem::~CMenuItem()
{
    delete iText;
}


//END OF FILE


