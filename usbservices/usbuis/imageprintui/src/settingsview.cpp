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
* Description:  Settings view for UI
*
*/


#include <aknViewAppUi.h>
#include <imageprintui.rsg>
#include <coecntrl.h>

#include "imageprintui.hrh"
#include "settingsview.h"
#include "settingscontainer.h"
#include "imageprintuidebug.h"
#include "imageprintuiappui.h"

#ifdef __SERIES60_HELP
#include <hlplch.h>         // Help launcher
#endif



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CSettingsView* CSettingsView::NewL()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::NewL()"));
    CSettingsView* self = CSettingsView::NewLC();
    CleanupStack::Pop( self );
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::NewL() complete"));
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CSettingsView* CSettingsView::NewLC()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::NewLC()"));
    CSettingsView* self = new( ELeave ) CSettingsView;
    CleanupStack::PushL( self );
    self->ConstructL();
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::NewLC() complete"));
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CSettingsView::~CSettingsView()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::Destructor"));
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::Destructor complete"));

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CSettingsView::CSettingsView()
    {
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CSettingsView::ConstructL()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::ConstructL()"));
    BaseConstructL( R_SETTINGS_VIEW );
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::ConstructL() complete"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUid CSettingsView::Id() const
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::Id"));
    return KImagePrintSettingsViewId;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CSettingsView::HandleCommandL(
    TInt aCommand )
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::HandleCommandL"));

    switch ( aCommand )
        {
        case EAknSoftkeyExit:
             {
             FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::Softkey exit"));
             AppUi()->Exit();
             break;
             }
                
        case EEikCmdExit:
            {
            FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::cmd exit"));
            AppUi()->Exit();
            break;
            }
        
        #ifdef __SERIES60_HELP
        case ECmdSettingsHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( 
                iEikonEnv->WsSession(), AppUi()->AppHelpContextL() );
            break;
            }
        #endif //__SERIES60_HELP    
        
        case ECmdSettingsPrint:
            {
            static_cast<CImagePrintUiAppUi*>(AppUi() )->StartPrinting();
            break;  
            }
             
        case ECmdSettingsChange:
            {
            iContainer->HandleChange(); 
            break; 
            }
         default:
            {
            FLOG( _L("[IMAGEPRINTUI]\t CSettingsView::HandleCommandL default switch"));    
            break;
            }
            
        }
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::HandleCommandL complete"));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CSettingsView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,
   TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {       
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::DoActivateL"));

	iContainer = CSettingsContainer::NewL( ClientRect(), static_cast<CImagePrintUiAppUi*>(AppUi() ));
	iContainer->SetMopParent( this );
	AppUi()->AddToStackL( *this, iContainer );
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::DoActivateL complete"));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CSettingsView::DoDeactivate()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::DoDeactivate()"));
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
	    iContainer = NULL;
        }
    FLOG(_L("[IMAGEPRINTUI]\t CSettingsView::DoDeactivate() complete"));

    }

//End of File

