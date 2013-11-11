/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of CHuiThemeManager class, the class
*                which is an interface to the ThemeManager component
*
*/


#include "uiacceltk/HuiThemeManager.h"

#include <utf.h>
#include "uiacceltk/HuiUtil.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CHuiThemeManager::CHuiThemeManager()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHuiThemeManager::~CHuiThemeManager()
    {

    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CHuiThemeManager* CHuiThemeManager::NewL()
    {
    CHuiThemeManager* self = CHuiThemeManager::NewLC();
    CleanupStack::Pop();
    return self;        
    }
    
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CHuiThemeManager* CHuiThemeManager::NewLC()
    {
    CHuiThemeManager* self = new (ELeave) CHuiThemeManager();
    self->ConstructL();
    CleanupStack::PushL(self);
    return self;        
    }

// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
//
void CHuiThemeManager::ConstructL()
	{
	iThemeManager = NULL;
	}

// ---------------------------------------------------------------------------
// Get Item ID from ThemeManager
// ---------------------------------------------------------------------------
//
EXPORT_C void CHuiThemeManager::GetItemID(const TDesC& aColorTable, TAknsItemID& aItemID)
    {
    HUI_DEBUG(_L("CHuiThemeManager::GetItemID DO NOT USE THIS FUNCTION"));
    TRAPD( err, GetItemIdL( aColorTable, aItemID ) );
    if ( err != KErrNone )
        {
        HUI_DEBUG1(_L("CHuiThemeManager::GetItemID error ignored %d" ), err);
        }
    }

// ---------------------------------------------------------------------------
// DEPRECATED
// ---------------------------------------------------------------------------
//
EXPORT_C void CHuiThemeManager::GetItemIdL(const TDesC& /*aColorTable*/, TAknsItemID& /*aItemID*/)
	{
	// Create ThemeManager instance
	HUI_DEBUG(_L("CHuiThemeManager::GetItemIDL FUNCTIONALITY REMOVED. REFER TO ANTRIKSH PROJECT"));
	}


// ---------------------------------------------------------------------------
// Get Skin Image parameters
// ---------------------------------------------------------------------------
//
EXPORT_C THuiThemeImageparams CHuiThemeManager::GetSkinImageParams(const TDesC& aSkinLogicalName)
    {
    THuiThemeImageparams result;
    
    HUI_DEBUG(_L("CHuiThemeManager::GetSkinImageParams DO NOT USE THIS FUNCTION"));
    TRAPD( err, result = GetSkinImageParamsL( aSkinLogicalName ) );
    if ( err != KErrNone )
        {
        HUI_DEBUG1(_L("CHuiThemeManager::GetSkinImageParams error ignored %d" ), err);
        }
    return result;
    }
    
    
    
// ---------------------------------------------------------------------------
// Get Skin Image parameters
// ---------------------------------------------------------------------------
//    
EXPORT_C THuiThemeImageparams CHuiThemeManager::GetSkinImageParamsL(const TDesC& aSkinLogicalName)
	{
	
	THuiThemeImageparams themeImageParams;
	themeImageParams.iSkinLogicalName= aSkinLogicalName;

	return themeImageParams;	
	}

