/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of HuiTextStyleManager. Handles the management
*                of Hitchcock UI Toolkit specific font styles. The 
*                implementation consists only of Symbian / S60 specific
*                implementation but the interface is designed so that 
*                alternative platforms can be utilized.
*
*/



#include <avkon.hrh> // @todo: include proper header file
#include <AknUtils.h>
#include <aknappui.h>
#include <gdi.h>
#ifndef PUBLIC_SDK
#include <aknlayoutscalable_avkon.cdl.h>
#endif

#include "uiacceltk/huitextstylemanager.h"
#include "HuiS60TextStyle.h"
#include "uiacceltk/HuiUtil.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Creates the CHuiTextStyleManager. Sets parameters to default values.
// ---------------------------------------------------------------------------
//
CHuiTextStyleManager::CHuiTextStyleManager() 
	{

	}

// ---------------------------------------------------------------------------
// Destroys the CHuiTextStyleManager. Makes sure all allocated resources are 
// freed.
// ---------------------------------------------------------------------------
//
CHuiTextStyleManager::~CHuiTextStyleManager()
    {
    // delete the text style in the array
    for(TInt i = 0; i < iTextStyles.Count(); i++)
        {
        // need to down cast, because base class does not have
        // virtual destructor.
        THuiS60TextStyle* deletedStyle = static_cast<THuiS60TextStyle*>(iTextStyles[i]);
        delete deletedStyle;
        }
    
    // Close the array
    iTextStyles.Close();
    }


// -----------------------------------------------------------------------------
// CHuiTextStyleManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHuiTextStyleManager* CHuiTextStyleManager::NewL()
    {
    CHuiTextStyleManager* self = NewLC();
    CleanupStack::Pop();
    return self;
    }


// -----------------------------------------------------------------------------
// CHuiTextStyleManager::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHuiTextStyleManager* CHuiTextStyleManager::NewLC()
    {
    CHuiTextStyleManager* self = new (ELeave) CHuiTextStyleManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CHuiTextStyleManager::ConstructL
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CHuiTextStyleManager::ConstructL()
    {
    // Construct the default text style first. 
	// It is the same as ENormalFont / EHuiTextStyleNormal, EHuiTextStyleMenuItem
    // Parent id -1 means no parent
    CreatePlatformTextStyleL( EAknLogicalFontPrimaryFont, -1 );
    }

// ---------------------------------------------------------------------------
// Creates a new text style instance.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHuiTextStyleManager::CreatePlatformTextStyleL(TInt aFontStyleId, TInt aParentId)
	{
	THuiS60TextStyle* style = new (ELeave) THuiS60TextStyle(aFontStyleId, aParentId);
	
    // Append the specified text style into the array of styles.
    CleanupStack::PushL( style );
    InsertIntoArrayAndSetIdL(*style); 
    CleanupStack::Pop( style );
    
    // Return the unique ID of the text style.
    return style->Id();
	}

// ---------------------------------------------------------------------------
// Retuns A pointer to the requested text style.
// ---------------------------------------------------------------------------
//
EXPORT_C THuiTextStyle* CHuiTextStyleManager::TextStyle(TInt aId)
	{
    if ( aId >= 0 && aId < iTextStyles.Count() )
        {
        __ASSERT_DEBUG( iTextStyles[aId] , USER_INVARIANT() );
        return iTextStyles[aId];
        }

	return iTextStyles[EHuiTextStyleNormal];
	}

// ---------------------------------------------------------------------------
// Creates a new text style object that is identical with the given source 
// text style.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHuiTextStyleManager::CopyTextStyleL(TInt aSourceId)
	{
	// get the source text style by the id
	const THuiTextStyle sourceStyle = *TextStyle(aSourceId);
	
	// make a copy of the source text style
	THuiTextStyle* style = new (ELeave) THuiTextStyle(sourceStyle);
	
    // Append the specified text style into the array of styles.
    CleanupStack::PushL( style );
    InsertIntoArrayAndSetIdL(*style); 
    CleanupStack::Pop( style );
    
    // Return the unique ID of the text style.
    return style->Id();
	}

// ---------------------------------------------------------------------------
// Deletes text style with given ID.
// ---------------------------------------------------------------------------
//	
EXPORT_C TInt CHuiTextStyleManager::DeleteTextStyle(TInt aId)
    {
    // Cannot delete platform styles from an application.
    if ( aId >= 0 && aId <= iLastBuiltInStyleId )
        {
        return KErrAccessDenied;
        }
        
    if ( aId >= iTextStyles.Count() )
        {
        return KErrArgument;
        }
    // need to down cast, because base class does not have
    // virtual destructor.
    THuiS60TextStyle* deletedStyle = static_cast<THuiS60TextStyle*>(iTextStyles[aId]);
    
    if ( deletedStyle )
        {
        delete deletedStyle;
        iTextStyles[aId] = NULL; // NULL the slot for later usage.
        }
    else
        {
        return KErrArgument;
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Construct built-in styles.
// these ate the preconfigured text styles enumerated in HuiSkin.h
// ---------------------------------------------------------------------------
//
void CHuiTextStyleManager::ConstructBuiltInStylesL()
    {
    THuiTextStyle* style;

    // the order here must be the same as in HuiSkin.h THuiPreconfiguredTextStyle
    
 	// ENormalItalicFont / no EHuiTextStyleXxx def
    TInt normalItalicStyleId = CreatePlatformTextStyleL( EAknLogicalFontPrimaryFont, EHuiTextStyleNormal );
    style = TextStyle(normalItalicStyleId);
	style->SetPosture(ETrue);
	
 	// EHuiTextStyleMenuItem
    CreatePlatformTextStyleL( EAknLogicalFontPrimaryFont, EHuiTextStyleNormal );
	
 	// ESmallFont / EHuiTextStyleSmall
    TInt smallStyleId = CreatePlatformTextStyleL( EAknLogicalFontSecondaryFont, EHuiTextStyleNormal );
	
 	// ESmallFont EHuiTextStyleMenuSmall
    CreatePlatformTextStyleL( EAknLogicalFontSecondaryFont, smallStyleId );
	
 	// ENormalBoldFont / EHuiTextStyleMenuTitle 
    TInt normalBoldStyleId = CreatePlatformTextStyleL( EAknLogicalFontPrimaryFont, EHuiTextStyleNormal );
    style = TextStyle(normalBoldStyleId);
	style->SetStrokeWeight(ETrue);
	
 	// ELargeFont / EHuiTextStyleLarge
    CreatePlatformTextStyleL( EAknLogicalFontTitleFont, EHuiTextStyleNormal );
	
    // ESoftkeyFont / EHuiTextStyleSoftkey
    TInt softkeyStyleId = CreatePlatformTextStyleL( EAknLogicalFontTitleFont, EHuiTextStyleNormal );
    SetSoftkeyTextStyle(softkeyStyleId);
	
    // EHuiTextStyleSupplement
    CreatePlatformTextStyleL( EAknLogicalFontPrimaryFont, EHuiTextStyleNormal );
	
    // EHuiTextStyleTitle
    iLastBuiltInStyleId = CreatePlatformTextStyleL( EAknLogicalFontTitleFont, EHuiTextStyleNormal );

    NotifyDisplaySizeChangedL(); // @todo: Why this is needed to make EHuiTextStyleSmall to look "small" instead of "normal" ???
    }

void CHuiTextStyleManager::SetSoftkeyTextStyle(TInt aTextStyleId)
	{
    // Deprecated, really
	if (!CCoeEnv::Static())
	    {
	    return;
	    }
		
	THuiTextStyle* style = TextStyle(aTextStyleId);
	TFontSpec spec;
	
	/// Approximate the S60 softkey font parameters    
    const CFont* font = AknLayoutUtils::FontFromId(EAknLogicalFontTitleFont, NULL);
    spec = font->FontSpecInTwips();
	spec.iHeight = 80 * spec.iHeight / 100;
	THuiFont huiFont(0, spec);
	style->SetFont(huiFont);
	}

void CHuiTextStyleManager::NotifyDisplaySizeChangedL()
    {
    for(TInt i = 0; i < iLastBuiltInStyleId; i++)
        {
        THuiS60TextStyle* platformStyle = dynamic_cast<THuiS60TextStyle*>(iTextStyles[i]);
        // Assume that dynmic cast returns null if iTextStyles[i] is not THuiS60TextStyle
        if (platformStyle)
            {
            platformStyle->Init();
            }                
        }        
    }
    
void CHuiTextStyleManager::InsertIntoArrayAndSetIdL(THuiTextStyle& style)
    {
    TInt id = KErrNotFound;
    
    // try to find an empty slot
    for ( TInt i = iTextStyles.Count() - 1 ; i >= 0 && id == KErrNotFound; i-- )
        {
        if ( !iTextStyles[i] )
            {
            // empty found
            id = i;
            }
        }
    
    if ( id != KErrNotFound )
        {
        // empty slot found -> utilize that
        iTextStyles[id] = &style;
        }
    else
        {
        // append
        id = iTextStyles.Count();
        iTextStyles.AppendL(&style);
        }
        
    style.SetId( id );
    }
