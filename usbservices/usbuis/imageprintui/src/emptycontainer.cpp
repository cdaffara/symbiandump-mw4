/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Empty container for UI
*
*/



//INCLUDES
#include <AknsDrawUtils.h> 
#include <AknsBasicBackgroundControlContext.h>
#include <AknsConstants.h>
#include <AknUtils.h>
#include <eikenv.h>

#include "emptycontainer.h"
#include "imageprintuidebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CEmptyContainer::CEmptyContainer()
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEmptyContainer::ConstructL( const TRect& aRect  )
    {
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyContainer::ConstructL"));
    CreateWindowL();
	SetRect( aRect );
    
    // Temporary rect is passed. Correct rect is set in SizeChanged.
    iSkinContext = CAknsBasicBackgroundControlContext::NewL(
        KAknsIIDQsnBgAreaMain, Rect(), EFalse);

    ActivateL();

    FLOG(_L("[IMAGEPRINTUI]\t CEmptyContainer::ConstructL complete"));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CEmptyContainer* CEmptyContainer::NewL( 
    const TRect& aRect )
    {
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyContainer::NewL"));
    CEmptyContainer* self = CEmptyContainer::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CEmptyContainer* CEmptyContainer::NewLC( 
    const TRect& aRect )
    {
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyContainer::NewLC"));
    
    CEmptyContainer* self = new( ELeave ) CEmptyContainer;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CEmptyContainer::~CEmptyContainer()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyContainer::Destructor"));
    delete iSkinContext; 
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEmptyContainer::Draw( const TRect& aRect ) const
    {

    FLOG(_L("[IMAGEPRINTUI]\t CEmptyContainer::Draw"));
    CWindowGc& gc = SystemGc();

    // Drawing skin
    if (iSkinContext)
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        AknsDrawUtils::Background( skin, iSkinContext, this, gc, aRect );
        }
    else
        {//  clear the area
        gc.SetBrushColor( iEikonEnv->ControlColor( EColorWindowBackground, *this ) );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.Clear( aRect );
        }

    FLOG(_L("[IMAGEPRINTUI]\t CEmptyContainer::Draw complete"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEmptyContainer::SizeChanged()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyContainer::SizeChanged()"));

    TRect parentRect(Rect());
	if (iSkinContext)
        {
        iSkinContext->SetRect(parentRect);
        }
    FLOG(_L("[IMAGEPRINTUI]\t CEmptyContainer::SizeChanged() complete"));
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
 void CEmptyContainer::HandleResourceChange(TInt aType)
 {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {		
		TRect rect;
		// ask where container's rectangle should be
		// EMainPane equals to area returned by CEikAppUi::ClientRect()
		
 		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        SetRect(rect); 
		DrawNow();
        }
    else
        {
        CCoeControl::HandleResourceChange(aType);
        }  
 }

//End of File
