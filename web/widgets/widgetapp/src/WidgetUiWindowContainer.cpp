/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include "WidgetUi.hrh"
#include "WidgetUiWindowContainer.h"
#include "WidgetUiWindowManager.h"
#include "WidgetUiWindowView.h"
#include "WidgetUiWindow.h"

#include <WidgetUi.rsg>

#include <WidgetRegistryConstants.h>

#include <brctlinterface.h>
#include <ApUtils.h>
#include <InternetConnectionManager.h>
#include <ActiveApDb.h>
#include <avkon.hrh>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>

 // CAknMessageQueryDialog
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <aknnotedialog.h>
#include <BrowserDialogsProvider.h>

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
// CWidgetUiWindowContainer::NewL
// two-phase constructor
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindowContainer*
CWidgetUiWindowContainer::NewL(
    const TRect& aRect, CWidgetUiWindowManager& aWindowManager )
    {
    CWidgetUiWindowContainer* container =
        new ( ELeave ) CWidgetUiWindowContainer( aWindowManager );
    CleanupStack::PushL( container );
    container->ConstructL( aRect );
    CleanupStack::Pop(); // container

    return container;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowContainer::ConstructL
// EPOC two phased constructor
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowContainer::ConstructL(
    const TRect& aRect)
    {
#ifdef _DEBUG
    _LIT(KDir, "WidgetUi");
    _LIT(KFile, "WidgetUi.log");
#endif

#ifdef _DEBUG
    TInt err = iFileLogger.Connect();
    if (err == KErrNone)
        {
        iFileLogger.CreateLog( KDir(), KFile(), EFileLoggingModeOverwrite );
        iCanLog = ETrue;
        }
#endif


    iCommandBase = TBrCtlDefs::ECommandIdBase;

    CreateWindowL();

    SetRect( aRect );
    iSkinContext = CAknsBasicBackgroundControlContext::NewL(
       KAknsIIDQsnBgScreen , aRect, EFalse);

    ActivateL();
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowContainer::CWidgetUiWindowContainer
// C++ default constructor
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindowContainer::CWidgetUiWindowContainer(CWidgetUiWindowManager& aWindowManager)
:iWindowManager( aWindowManager )
    {

    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowContainer::~CWidgetUiWindowContainer
// Destructor
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindowContainer::~CWidgetUiWindowContainer()
    {
    delete iSkinContext;

#ifdef _DEBUG
    if (iCanLog)
        {
        iFileLogger.CloseLog();
        iFileLogger.Close();
        }
#endif

    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowContainer::SizeChanged
// Called by framework when the view size is changed
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowContainer::SizeChanged()
    {
    TInt offset( 0 );

    if( Engine() && (Engine()->Rect() != Rect()) )
        {
        TRect rect( Rect() );
        //rect.iBr.iY-=offset;
        Engine()->SetRect( rect );
        }

    if(iSkinContext)
        iSkinContext->SetRect(Rect());

    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowContainer::CountComponentControls
// return the number of controls contained
//
// -----------------------------------------------------------------------------
//
TInt CWidgetUiWindowContainer::CountComponentControls() const
    {
    TInt count( 0 );
    if( Engine() )
        {
        count++;
        }
    return count;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowContainer::ComponentControl
// return the component control identified by the given index
//
// -----------------------------------------------------------------------------
//
CCoeControl* CWidgetUiWindowContainer::ComponentControl( TInt aIndex ) const
    {
    switch( aIndex )
        {
        case 0:
            {
            return Engine();
            }
        default:
            {
            return NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowContainer::Draw
// Draws the control
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if (iSkinContext && !AknsDrawUtils::Background(skin, iSkinContext, this, gc, aRect))
        {
        // default draw if no skin
        gc.SetPenStyle( CGraphicsContext::ENullPen );
        gc.SetBrushColor( KRgbWhite );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.DrawRect( aRect );
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowContainer::HandleKeyEventL
// Handles key events
//
// -----------------------------------------------------------------------------
//
TKeyResponse CWidgetUiWindowContainer::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,
    TEventCode /*aType*/ )
    {
    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowContainer::HandleCommandL
// Handles user commands
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowContainer::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case ECmdReload:
            break;
        // MSK pressed in content view so notify BrCtl to activate item under cursor
        case ECmdMsk:
            {
            Engine()->HandleCommandL(
                                    (TInt)TBrCtlDefs::ECommandOpen +
                                    (TInt)TBrCtlDefs::ECommandIdBase );
            break;
            }
        default:
            {
            Engine()->HandleCommandL( aCommand );
            DrawDeferred();// re-draw widget after option menu goes away
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowContainer::OfferKeyEventL
// Handles key events
//
// -----------------------------------------------------------------------------
//
TKeyResponse CWidgetUiWindowContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    // Handle RED End Key (EKeyNo)
    if (aKeyEvent.iCode == EKeyNo)
        {
        // Close the current active widget
        iWindowManager.CloseWindowsAsync(EFalse);
        return EKeyWasConsumed;
        }

    TKeyResponse resp(EKeyWasNotConsumed);
    TBool editing(EFalse);
	//Reset the iIsOptionsMenuActivated if enabled
	iWindowManager.View()->DeActivateOptionsMenu();

    if (Engine())
        {
        TBool showStausPane(EFalse);
        TBrCtlDefs::TBrCtlElementType eType = Engine()->FocusedElementType();
        switch(eType)
            {
            case TBrCtlDefs::EElementInputBox:
            case TBrCtlDefs::EElementActivatedInputBox:
            case TBrCtlDefs::EElementTextAreaBox:
                // status pane is needed for editing
                showStausPane = ETrue;
                // fall through
            case TBrCtlDefs::EElementObjectBox:
            case TBrCtlDefs::EElementActivatedObjectBox:
                {
                // Status pane must be shown before input is activated because resizing de-activates
                // the input box.
                // Rules for input activation:
                // Copied from WebKit BrowserView
                // CKeyEventHandler::HandleOfferKeyEventL  case EEventKeyUp
                // (aKeyEvent.iScanCode > EStdKeyScrollLock && aKeyEvent.iScanCode < EStdKeyF1)
                if (aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK ||
                   (aKeyEvent.iScanCode > EStdKeyScrollLock && aKeyEvent.iScanCode < EStdKeyF1))
                    {
                    if ( iWindowManager.ActiveWindow() &&
          			        iWindowManager.View()->CbaGroup()->IsVisible() )
                        {
                        iWindowManager.View()->UpdateStatusPane(ETrue);
                        editing = ETrue;
                        }
                    }
                break;
                }
            default:
                break;
            }

        resp = Engine()->OfferKeyEventL( aKeyEvent, aType );

        // special case for readonly input box - if not activated then remove status pane and cba
        if( editing && aType == EEventKeyUp &&
            (Engine()->FocusedElementType() != TBrCtlDefs::EElementActivatedInputBox  &&
             Engine()->FocusedElementType() != TBrCtlDefs::EElementActivatedObjectBox &&
             Engine()->FocusedElementType() != TBrCtlDefs::EElementTextAreaBox))
            {
            iWindowManager.View()->UpdateStatusPane(EFalse);
            }

        }

    return resp;
    }

void CWidgetUiWindowContainer::FocusChanged(TDrawNow aDrawNow)
    {

    if (Engine())
        Engine()->SetFocus(IsFocused());

    CCoeControl::FocusChanged( aDrawNow );
    }
// -----------------------------------------------------------------------------
// CWidgetUiWindowContainer::CancelLoad()
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowContainer::CancelLoad()
    {
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowContainer::Engine()
// return CBrCtlInterface*
//
// -----------------------------------------------------------------------------
//
CBrCtlInterface* CWidgetUiWindowContainer::Engine() const
    {
    CBrCtlInterface* engine = NULL;
    // Active window can be null at startup
    if( iWindowManager.ActiveWindow() )
        {
        engine = iWindowManager.ActiveWindow()->Engine();
        }

    return engine;
    }



// -----------------------------------------------------------------------------
// CWidgetUiWindowContainer::ErrorNote()
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowContainer::ErrorNoteL( const TDesC& aPrompt )
    {
    CAknErrorNote* note = new ( ELeave ) CAknErrorNote();
    note->ExecuteLD( aPrompt );
    }

// ----------------------------------------------------------------------------
// CWidgetUiWindowContainer::InfoNoteL
//
// ----------------------------------------------------------------------------
//
void CWidgetUiWindowContainer::InfoNoteL( const TDesC& aPrompt )
    {
    CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote( ETrue );
    note->ExecuteLD( aPrompt );
    }



// End of File
