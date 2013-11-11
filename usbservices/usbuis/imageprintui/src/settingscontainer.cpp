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
* Description:  Container class for UI
*
*/



//INCLUDES
#include <AknsDrawUtils.h> 
#include <AknsBasicBackgroundControlContext.h>
#include <AknsConstants.h>
#include <AknUtils.h>
#include <eikenv.h>
#include <eiklbo.h>


#include "imageprintuidocument.h"
#include "imageprintuiappui.h"
#include "imageprintuiapp.h"
#include "capabilitymanager.h"
#include "imageprintui.hrh"
#include "settingscontainer.h"
#include "imageprintuidebug.h"
#include "settingsitems.h"
#include "settingstables.h"
#ifdef __SERIES60_HELP
//#include <csxhelp/usb.hlp.hrh>   // Help id
#endif


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CSettingsContainer::CSettingsContainer()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CSettingsContainer::ConstructL( const TRect& aRect, CImagePrintUiAppUi* aAppUi)
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::ConstructL"));

    CreateWindowL();

	
    // Create main itemlist
    iSettingsMainListBox = new (ELeave) CAknSettingStyleListBox;
    iSettingsMainListBox->SetContainerWindowL( *this );
    iSettingsMainListBox->ConstructL( this, EAknListBoxMarkableList );
    iAppUi = aAppUi;  
 
    iSettingsItems = CSettingsItems::NewL( aAppUi->CapabilityManager(), aAppUi->Notes() );

    // Set up/down arrows at bottom of the screen (scrollbar)
    iSettingsMainListBox->CreateScrollBarFrameL( ETrue );
    iSettingsMainListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
        CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto );

    // Set up the observer (events listener)
    iSettingsMainListBox->SetListBoxObserver( iSettingsItems );
    // Set text labels for list items
    iSettingsMainListBox->Model()->SetItemTextArray( iSettingsItems );
    iSettingsMainListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );   // Does not delete items array

    SetRect(aRect);
    ActivateL();    

    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::ConstructL complete"));

    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CSettingsContainer* CSettingsContainer::NewL( 
    const TRect& aRect, CImagePrintUiAppUi* aAppUi )
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::NewL"));
    CSettingsContainer* self = CSettingsContainer::NewLC( aRect, aAppUi );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CSettingsContainer* CSettingsContainer::NewLC( 
    const TRect& aRect, CImagePrintUiAppUi* aAppUi)
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::NewLC"));
    
    CSettingsContainer* self = new( ELeave ) CSettingsContainer;
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aAppUi );
    return self;
    }



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CSettingsContainer::~CSettingsContainer()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::Destructor"));
    delete iSettingsMainListBox; 
    delete iSettingsItems;
    }


// ---------------------------------------------------------------------------
// From class CCoeControl
// ---------------------------------------------------------------------------
//
void CSettingsContainer::SizeChanged()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::SizeChanged()"));
    
    iSettingsMainListBox->SetRect( Rect() ); 
    iSettingsMainListBox->SetFocus( ETrue );
    
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::SizeChanged() complete"));
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
 void CSettingsContainer::HandleResourceChange(TInt aType)
 {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::HandleResourceChange"));
    CCoeControl::HandleResourceChange( aType ); 
    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
        SetRect(mainPaneRect);

        DrawNow(); 
        }
    else if ( aType == KAknsMessageSkinChange )
        {
        iSettingsMainListBox->HandleResourceChange(aType); 
        }
       
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::HandleResourceChange complete"));
}



// ---------------------------------------------------------------------------
// Returns the number of controls inside this container
// ----------------------------------------------------------------------------
//
TInt CSettingsContainer::CountComponentControls() const
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::CountComponentControls"));
    
    return KImagePrintComponentCount; 
    }


// ---------------------------------------------------------------------------
// Returns the control inside this container
// ----------------------------------------------------------------------------
//
CCoeControl* CSettingsContainer::ComponentControl( TInt aIndex ) const
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::ComponentControl"));
    
    switch ( aIndex )
        {
        case 0:
            {
            return iSettingsMainListBox;
            }
        default:
            {
            return NULL;
            }            
        }
    }



// ---------------------------------------------------------------------------
// Redirects keypresses to the listbox
// ----------------------------------------------------------------------------
//
TKeyResponse CSettingsContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType )
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::OfferKeyEventL()"));
    
    if( ( iSettingsMainListBox ) && ( aKeyEvent.iCode != EKeyLeftArrow )
         && ( aKeyEvent.iCode != EKeyRightArrow ) )
        {
        TKeyResponse res = iSettingsMainListBox->OfferKeyEventL( aKeyEvent, aType );
        // Store current item index for options list usage later
        return res;
        }
        
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::OfferKeyEventL complete"));
    
    return EKeyWasNotConsumed;
    
    }

// ---------------------------------------------------------------------------
// This is called by view class when user has selected Change from Options menu 
// ----------------------------------------------------------------------------
void CSettingsContainer::HandleChange()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::HandleChange"));
    TRAPD(err, iSettingsItems->HandleListBoxEventL( iSettingsMainListBox, 
        MEikListBoxObserver::EEventEnterKeyPressed ));
    if(err)
    	{
        FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::error at HandleChange")); 	
    	}
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::HandleChange complete"));
    }


#ifdef __SERIES60_HELP
// -----------------------------------------------------------------------------
// From class CCoeControl
// For getting help context
// -----------------------------------------------------------------------------
//
void CSettingsContainer::GetHelpContext( TCoeHelpContext& aContext ) const
	{
 /*
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::GetHelpContext"));
	aContext.iMajor = KUidUSB; 
	aContext.iContext = KUSB_HLP_PRINT_SETTINGS;
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsContainer::GetHelpContext complete"));
*/
	}


#endif //__SERIES60_HELP

//End of File

