/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Dialog containing the Downloads List
*
*/



// INCLUDE FILES
#include    "CDownloadsListDlg.h"
#include    "CDownloadUtils.h"
#include    "DownloadsListDlgObserver.h"
#include    "CDownloadsListArray.h"
#include    "DownloadMgrUiLib.hrh"
#include    "DMgrUiLibPanic.h"
#include    "UiLibLogger.h"
#include    <DownloadMgrUiLib.rsg>
#include    <e32std.h>
#include    <e32def.h>
#include    <eikmenub.h>
#include    <eikfrlb.h>
#include    <eikfrlbd.h>
#include    <aknlists.h>
#include    <StringLoader.h>
#include    <eikenv.h>
#include    <DocumentHandler.h>

#include "eikon.hrh"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDownloadsListDlg::CDownloadsListDlg
// -----------------------------------------------------------------------------
//
CDownloadsListDlg::CDownloadsListDlg( MDownloadsListDlgObserver& aDlgObserver )
:   iMyEikonEnv( *CEikonEnv::Static() ),
    iDlgObserver( aDlgObserver ),
    iProgressiveDownload( EFalse)
    {
    }


// -----------------------------------------------------------------------------
// CDownloadsListDlg::CDownloadsListDlg
// -----------------------------------------------------------------------------
//
CDownloadsListDlg::CDownloadsListDlg( MDownloadsListDlgObserver& aDlgObserver, TBool aProgressiveFlag )
:   iMyEikonEnv( *CEikonEnv::Static() ),
    iDlgObserver( aDlgObserver ),
    iProgressiveDownload(aProgressiveFlag)
    {
    }


// -----------------------------------------------------------------------------
// CDownloadsListDlg::ConstructL
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::ConstructL()
    {
    CLOG_ENTERFN("CDownloadsListDlg::ConstructL");

    CEikFormattedCellListBox* listBox = new (ELeave) CAknDoubleLargeGraphicPopupMenuStyleListBox;
    delete iListBox;
    iListBox = listBox;

    iIsMSKChangeHandled = EFalse;

    CAknPopupList::ConstructL( iListBox,
                               R_DMUL_DOWNLOADSLIST_SOFTKEYS_OPEN,
                               AknPopupLayouts::EMenuDoubleLargeGraphicWindow );
    iListBox->SetListBoxObserver(this);

    TInt flags = 0;
    listBox->ConstructL( this, flags );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                                        CEikScrollBarFrame::EAuto );

    HBufC* header = StringLoader::LoadLC( R_DMUL_DOWNLOADSLIST_HEADER );
    SetTitleL( *header );
    CleanupStack::PopAndDestroy( header );  // header
    header = NULL;

    // Create menu bar
    CEikMenuBar* newMenuBar = new (ELeave) CEikMenuBar();
    CleanupStack::PushL( newMenuBar );
    newMenuBar->ConstructL( this, NULL, R_DMUL_DOWNLOADSLIST_MENUBAR );
    iMyEikonEnv.EikAppUi()->AddToStackL( newMenuBar,
                                         ECoeStackPriorityMenu,
                                         ECoeStackFlagRefusesFocus );
    iMenuBar = newMenuBar;
    CleanupStack::Pop( newMenuBar ); // now owned by this.

    iRefreshTimer = CPeriodic::NewL( KRefreshTimerPriority );
    CLOG_WRITE(" iRefreshTimer created");

    iDownloadUtils = CDownloadUtils::NewL();

    CLOG_LEAVEFN("CDownloadsListDlg::ConstructL");
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::NewL
// -----------------------------------------------------------------------------
//
CDownloadsListDlg* CDownloadsListDlg::NewL( MDownloadsListDlgObserver& aDlgObserver )
    {
    CDownloadsListDlg* self = new ( ELeave ) CDownloadsListDlg( aDlgObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::NewL
// -----------------------------------------------------------------------------
//
CDownloadsListDlg* CDownloadsListDlg::NewL( MDownloadsListDlgObserver& aDlgObserver, TBool aProgressiveFlag )
    {
    CDownloadsListDlg* self = new ( ELeave ) CDownloadsListDlg( aDlgObserver, aProgressiveFlag );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CDownloadsListDlg::~CDownloadsListDlg()
    {
    CLOG_ENTERFN("CDownloadsListDlg::~CDownloadsListDlg");

    iDlgObserver.SetDownloadListHide( ETrue );
    if ( iMenuBar )
        {
        HideMenu();
        iMyEikonEnv.EikAppUi()->RemoveFromStack( iMenuBar );
        delete iMenuBar;
        iMenuBar = NULL;
        }
    delete iRefreshTimer;
    iRefreshTimer = 0;
    delete iListBox;
    iListBox = NULL;
    delete iDownloadUtils;
    iDownloadUtils = NULL;

    CLOG_LEAVEFN("CDownloadsListDlg::~CDownloadsListDlg");
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::SetModelL
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::SetModelL( CDownloadsListArray& aModel )
    {
    CLOG_ENTERFN("CDownloadsListDlg::SetModelL");

    __ASSERT_DEBUG( iListBox, Panic( EUiLibPanNullListBoxInSetModel ) );
    // Remember for the model:
    iDownloadsListArray = &aModel;
    // Set item text and icon array
    iListBox->Model()->SetItemTextArray( &aModel.ItemTextArray() );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    // iListBox will own the icon array - it is owned also by aModel, so...
    // Delete old icon array JIC?

    //delete (iListBox->ItemDrawer()->FormattedCellData()->IconArray());
    //Above line has been changed as below to avoid code scanner error

    CArrayPtr<CGulIcon>* iconarray = iListBox->ItemDrawer()->FormattedCellData()->IconArray();
    delete iconarray;
    iconarray = NULL;

    iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( &aModel.IconArray() );
    aModel.SetIconArrayOwnedExternally( ETrue );
    // Enable marquee.
    iListBox->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );

    if ( aModel.Count() == 0 )
        {
        // Disable Options softkey.
        ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeyOptions, EFalse );
        CLOG_WRITE(" aModel.Count() == 0");
        }

    CLOG_LEAVEFN("CDownloadsListDlg::SetModelL");
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::HandleModelChangeL
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::HandleModelChangeL( TDownloadsListDlgEvent aEvent, TInt aIndex )
    {
    CLOG_ENTERFN("CDownloadsListDlg::HandleModelChangeL");

    CLOG_WRITE_FORMAT(" aEvent: %d",aEvent);
    CLOG_WRITE_FORMAT(" aIndex: %d",aIndex);

    __ASSERT_DEBUG( iListBox, Panic( EUiLibPanNullListBoxInHandleModelChange ) );

    if ( aEvent == EDownloadAdded )
        {
        iListBox->HandleItemAdditionL();

        // Start refresh timer, if necessary
        if ( !iRefreshTimer->IsActive() && 0 < VisibleInProgressDownloads() )
            {
            iRefreshTimer->Start(
                KRefreshIntervalMicroSeconds,
                KRefreshIntervalMicroSeconds,
                TCallBack( RefreshTimerCallback, this ) );
            CLOG_WRITE(" iRefreshTimer strated");
            }
        }
    else if ( aEvent == EDownloadChanged )
        {
        // Redraw the item, but only if it is visible in order that
        // list box does not scroll to an invisible item
        // (as it does when calling DrawItem()).
        // Highlight does not change anyway.
        if ( iListBox->View()->ItemIsVisible( aIndex ) )
            {
            iListBox->DrawItem( aIndex );

            // Start refresh timer, if necessary
            if ( !iRefreshTimer->IsActive() && 0 < VisibleInProgressDownloads() )
                {
                iRefreshTimer->Start(
                    KRefreshIntervalMicroSeconds,
                    KRefreshIntervalMicroSeconds,
                    TCallBack( RefreshTimerCallback, this ) );
                CLOG_WRITE(" iRefreshTimer strated");
                }
            }

            if (!iIsMSKChangeHandled)
                {
                    HandleMiddleSoftKeyChangeL();
                }
        }
    else // ( aEvent == EDownloadRemoved )
        {
        CLOG_WRITE_FORMAT(" count: %d",iListBox->Model()->NumberOfItems());
        if ( aIndex == 0 )
            {
            // If there is at least one item still, then scroll to the first:
            if ( iListBox->Model()->NumberOfItems() > 0 )
                {
                iListBox->SetCurrentItemIndex( 0 ); // "0" = first
                }
            }
        else if ( aIndex > 0 )
            {
            iListBox->SetCurrentItemIndex( aIndex - 1 );
            }
        iListBox->HandleItemRemovalL();
        iListBox->DrawNow();

        // Stop refresh timer, if necessary
        if ( iRefreshTimer->IsActive() && VisibleInProgressDownloads() == 0 )
            {
            iRefreshTimer->Cancel();
            CLOG_WRITE(" iRefreshTimer cancelled");
            }
        }

    // Close the dialog, if necessary
    if ( iListBox->Model()->NumberOfItems() == 0 )
        {
        CancelPopup();
        }

    CLOG_LEAVEFN("CDownloadsListDlg::HandleModelChangeL");
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::CurrentItemIndex
// -----------------------------------------------------------------------------
//
TInt CDownloadsListDlg::CurrentItemIndex() const
    {
    return (iListBox==NULL?KErrNotFound:iListBox->CurrentItemIndex());
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::DisplayMenuL
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::DisplayMenuL()
    {
    __ASSERT_DEBUG( iMenuBar, Panic( EUiLibPanNullMenuBar ) );
    iMenuBar->TryDisplayMenuBarL();
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::HideMenu
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::HideMenu()
    {
    __ASSERT_DEBUG( iMenuBar, Panic( EUiLibPanNullMenuBar ) );
    iMenuBar->StopDisplayingMenuBar();
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::MenuShowing
// -----------------------------------------------------------------------------
//
TBool CDownloadsListDlg::MenuShowing() const
    {
    __ASSERT_DEBUG( iMenuBar, Panic( EUiLibPanNullMenuBar ) );
    return iMenuBar->IsDisplayed();
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::HighlightDownload
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::HighlightDownload( RHttpDownload& aHighlightDl )
    {
    iHighlightDownload = &aHighlightDl;
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::HandleMiddleSoftKeyChangeL
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::HandleMiddleSoftKeyChangeL()
    {

    TInt currentItemIndex = CurrentItemIndex();
    __ASSERT_DEBUG( 0 <= currentItemIndex, Panic( EUiLibPanDLNegItemIndex ) );
    RHttpDownload& currDownload = iDownloadsListArray->Download( currentItemIndex );
    CLOG_WRITE_FORMAT(" currDownload: 0x%x",&currDownload);

    // Get the UI data for the current download
    TDownloadUiData& dlData = iDownloadsListArray->DlUiData( CurrentItemIndex() );
    TInt32 state( dlData.iDownloadState );
    CLOG_WRITE_FORMAT(" state: %d",state);

    // The moved state is only a temporary state - the server
    // sets the download's original state back right after it,
    // thus we query the state again:
    if ( state == EHttpDlMoved )
        {
        User::LeaveIfError( currDownload.GetIntAttribute( EDlAttrState, state ) );
        
        //Changes for the bug JERI-7P8CF2
        //Changes made in the server side to fix the video center receiving unexpected events
        //Reassigning these events back to the changes done in server side
        if(state  == EHttpDlCompleted )
		    {
            state  = EHttpDlMultipleMOCompleted;
		    }
         else if(state  == EHttpDlFailed )
		    {
		     state  = EHttpDlMultipleMOFailed;
		    }
          
        
        CLOG_WRITE_FORMAT(" state re-query: %d",state);
        }
        __ASSERT_DEBUG( state != EHttpDlMoved, Panic( EUiLibPanStateIsStillMoved ) );

        TBool isCompleted   = state == EHttpDlMultipleMOCompleted;
        TBool isDrmDownload = EFalse;
        TBool isDrmRightsOnPhone = EFalse;
        TBool isPreviewRights = EFalse;
        // OMA DRM information is used only in case of completed downloads
        if ( isCompleted )
            {
            isDrmDownload = iDownloadUtils->DrmDownloadL( currDownload );
            if ( isDrmDownload )
                {
                isDrmRightsOnPhone = iDownloadUtils->DrmRightsOnThePhoneL
                                                    ( currDownload, isPreviewRights );
                }
            }
        TBool supported( EFalse );
        supported = iDownloadUtils->IsContentTypeSupportedL( dlData.iContentType );
        CLOG_WRITE_FORMAT(" supported: %d",supported);

        TBool isCodDownload( EFalse );
        TInt err = currDownload.GetBoolAttribute( EDlAttrCodDownload, isCodDownload );
        CLOG_WRITE_FORMAT(" EDlAttrCodDownload err: %d",err);
        TBool isCodPdDownload( EFalse );
        err = currDownload.GetBoolAttribute( EDlAttrCodPdAvailable, isCodPdDownload );
        CLOG_WRITE_FORMAT(" EDlAttrCodPdAvailable err: %d",err);

        // Get if it can be handled progressively
        TBool canProgHandled = EFalse;
        TUid pdPlayerUid = { 0 };
        HBufC8* contentType = iDownloadUtils->ContentTypeL( currDownload, ETrue );
        TDataType dataType( *contentType );
        delete contentType;
        contentType = NULL;
        CDocumentHandler* docHandler = CDocumentHandler::NewLC();
        canProgHandled = docHandler->CanHandleProgressivelyL( dataType, pdPlayerUid );
        CleanupStack::PopAndDestroy( docHandler ); // docHandler

        if ( isCompleted )
            {
            // If download is complete but file is not moved yet, do not show "Open"
            // menu item. Opening the file while it is being moved may result in
            // undefined behavior.
            TInt32 progressState = dlData.iProgressState;

            if ((progressState != EHttpProgContentFileMovedAndDestFNChanged) &&
                (progressState != EHttpProgContentFileMoved))
                {
                // Progressive downloads should open whenever user clicks on the
                // download. So return only if this download cannot be handled
                // progressively.
                if (!canProgHandled)
                    {
                    // Download State is complete but file is not moved yet.
                    // Hide the "Open" menu item and return.
                    ButtonGroupContainer()->SetCommandSetL( R_DMUL_DOWNLOADSLIST_SOFTKEYS_PLAY );
                    ButtonGroupContainer()->DrawNow();
                    ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeyOpen, EFalse );
                    iIsMSKChangeHandled = EFalse;
                    return;
                    }
                }
            }

        if ( iProgressiveDownload || isCompleted )
            {
            if ( canProgHandled && !(state == EHttpDlPaused || state == EHttpDlMultipleMOFailed) )  // it can be inprogress or completed
               {
               ButtonGroupContainer()->SetCommandSetL( R_DMUL_DOWNLOADSLIST_SOFTKEYS_PLAY );
               ButtonGroupContainer()->DrawNow();
               ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeyOpen, ETrue );
               iIsMSKChangeHandled = ETrue;
               if (isCodDownload && !isCompleted )
                  {
                  if ( !( supported && canProgHandled && isCodPdDownload &&
                     ( !isDrmDownload || ( isDrmDownload && isDrmRightsOnPhone ) ) ) )
                      {
                      ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeyOpen, EFalse );
                      iIsMSKChangeHandled = EFalse;
                      }
                  }
                  else
                  {
                  if ( !( supported && canProgHandled &&
                        ( !isDrmDownload || ( isDrmDownload && isDrmRightsOnPhone ) ) ) )
                      {
                      ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeyOpen, EFalse );
                      iIsMSKChangeHandled = EFalse;
                      }
                  }
               }
            else
               {
               ButtonGroupContainer()->SetCommandSetL( R_DMUL_DOWNLOADSLIST_SOFTKEYS_OPEN );
               ButtonGroupContainer()->DrawNow();
               ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeyOpen, ETrue );
               iIsMSKChangeHandled = ETrue;

               if ( !( isCompleted && supported &&
                  ( !isDrmDownload || ( isDrmDownload && isDrmRightsOnPhone ) ) ) )
                  {
                  ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeyOpen, EFalse );
                  iIsMSKChangeHandled = EFalse;
                  }
               }
            }
         else // if iProgressiveDownload == EFalse
            {
            ButtonGroupContainer()->SetCommandSetL( R_DMUL_DOWNLOADSLIST_SOFTKEYS_OPEN );
            ButtonGroupContainer()->DrawNow();
            ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeyOpen, ETrue );
            iIsMSKChangeHandled = ETrue;

            if ( !( isCompleted && supported &&
               ( !isDrmDownload || ( isDrmDownload && isDrmRightsOnPhone ) ) ) )
               {
               ButtonGroupContainer()->MakeCommandVisible( EAknSoftkeyOpen, EFalse );
               iIsMSKChangeHandled = EFalse;
               }
            }
        }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::ExecuteLD
// -----------------------------------------------------------------------------
//
TBool CDownloadsListDlg::ExecuteLD()
    {
    CLOG_ENTERFN("CDownloadsListDlg::ExecuteLD");

    iDlgObserver.PreLayoutDynInitL( *this );

    if ( iHighlightDownload )
        {
        // Find the index of this download
        TInt index( KErrNotFound );
        TInt err = iDownloadsListArray->Find( *iHighlightDownload, index );
        if ( !err )
            {
            iListBox->SetCurrentItemIndex( index );
            }
        }

    // Start refresh timer, if there is at least one visible download in
    // EHttpDlInprogress state.
    if ( !iRefreshTimer->IsActive() && 0 < VisibleInProgressDownloads() )
        {
        iRefreshTimer->Start(
            KRefreshIntervalMicroSeconds,
            KRefreshIntervalMicroSeconds,
            TCallBack( RefreshTimerCallback, this ) );
        CLOG_WRITE(" iRefreshTimer strated");
        }

    // Handle middle softkey
    HandleMiddleSoftKeyChangeL();

    TBool popupRet = CAknPopupList::ExecuteLD();

    CLOG_LEAVEFN("CDownloadsListDlg::ExecuteLD");
    return popupRet;
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::ProcessCommandL( TInt aCommandId )
    {
    CLOG_ENTERFN("CDownloadsListDlg::ProcessCommandL");

    HideMenu();

    if ( aCommandId == EAknSoftkeyOptions )
        {
        DisplayMenuL();
        }
    else if ( aCommandId == EAknSoftkeyCancel )
        {
        CancelPopup();
        }
    else
        {
        // Cancel this control not to dim the screen, and then process the command.
        //TODO CancelPopup();
        // Other, non-specific commands are processed by the observer.
        iDlgObserver.ProcessCommandL( *this, aCommandId );
        }

    CLOG_LEAVEFN("CDownloadsListDlg::ProcessCommandL");
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::SetEmphasis
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::SetEmphasis( CCoeControl* aMenuControl, TBool aEmphasis )
    {
    CLOG_ENTERFN("CDownloadsListDlg::SetEmphasis");

    // CAknDialog::SetEmphasis implements it like below.
    CEikAppUi* appUi = iMyEikonEnv.EikAppUi();
    appUi->RemoveFromStack( aMenuControl );
    TRAP_IGNORE( appUi->AddToStackL
        ( aMenuControl, aEmphasis ? ECoeStackPriorityDialog : ECoeStackPriorityMenu ) );
    //appUi->UpdateStackedControlFlags
    //    ( this, aEmphasis ? ECoeStackFlagRefusesFocus : 0, ECoeStackFlagRefusesFocus );
    appUi->UpdateStackedControlFlags
        ( aMenuControl, aEmphasis ? 0 : ECoeStackFlagRefusesFocus, ECoeStackFlagRefusesFocus );
    appUi->HandleStackChanged();

    CLOG_LEAVEFN("CDownloadsListDlg::SetEmphasis");
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    iDlgObserver.DynInitMenuPaneL( *this, aResourceId, aMenuPane );
    }


// -----------------------------------------------------------------------------
// CDownloadsListDlg::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
    {
    if (iListBox  && aListBox == iListBox)
        {
        switch (aEventType)
            {
            case MEikListBoxObserver::EEventItemClicked:
            case MEikListBoxObserver::EEventItemDraggingActioned:
                {
                HandleMiddleSoftKeyChangeL();
                break;
                }
            case MEikListBoxObserver::EEventItemDoubleClicked:
                {
                // If EAknSoftkeyOpen is visible, then we can activate the selected download
                if (ButtonGroupContainer()->IsCommandVisible(EAknSoftkeyOpen))
                    {
                    ProcessCommandL(EAknSoftkeyOpen);
                    }
                }
            default:
                {
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::FocusChanged
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::FocusChanged( TDrawNow aDrawNow )
    {
    // Focus gained or lost
    if ( IsFocused() )
        {
        // Focus gained. Start refresh timer if needed
        // Start refresh timer, if there is at least one visible download in
        // EHttpDlInprogress state.
        if ( !iRefreshTimer->IsActive() && 0 < VisibleInProgressDownloads() )
            {
            iRefreshTimer->Start(
                KRefreshIntervalMicroSeconds,
                KRefreshIntervalMicroSeconds,
                TCallBack( RefreshTimerCallback, this ) );
            CLOG_WRITE(" iRefreshTimer strated");
            }
        }
    else
        {
        // Focus lost. Cancel refresh timer
        iRefreshTimer->Cancel();
        CLOG_WRITE(" iRefreshTimer cancelled");
        }

    // Call base class' same functionality:
    // Important: CAknPopupList::FocusChanged() is private, so
    // the base class' very same method cannot be called from here.
    // It means that the functionality of that must be implemented here,
    // and meanwhile ask CAknPopupList to make it protected.
    // See SCB CR  GKOA-6KPC5L.
    if ( iListBox )
        {
          if( iListBox->CurrentItemIndex() >= 0 )
          {
           TRAP_IGNORE( HandleMiddleSoftKeyChangeL() );
          }
        iListBox->SetFocus(IsFocused(), aDrawNow);
        }
    // We have no find box, so this part can be omitted:
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    CAknPopupList::HandlePointerEventL(aPointerEvent);
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CDownloadsListDlg::OfferKeyEventL
    ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse resp = EKeyWasNotConsumed;
    resp = iDlgObserver.OfferKeyEventL( *this, aKeyEvent, aType );

    if ( resp == EKeyWasNotConsumed )
        {
        // Handle EKeyEscape specially - for it the
        // dialog has to be closed immediately.
        if ( aType == EEventKey && aKeyEvent.iCode == EKeyEscape )
            {
            CLOG_WRITE(" Offer: EKeyEscape - CancelPopup");
            CancelPopup();
            resp = EKeyWasConsumed;
            }
        }

    if ( 0 < KRefreshIntervalMicroSeconds )
        {
        // Scrolling effects refresh timer
        // If a download becomes visible due to scrolling, it is immediately updated.
        TBool refreshNeeded = EFalse;
        TInt refreshIndex = 0;
        if ( aType == EEventKey
             && (    aKeyEvent.iCode == EKeyLeftUpArrow          // Northwest
                  || aKeyEvent.iCode == EStdKeyDevice10          //   : Extra KeyEvent supports diagonal event simulator wedge
                  || aKeyEvent.iCode == EKeyUpArrow              // North
                  || aKeyEvent.iCode == EKeyRightUpArrow         // Northeast
                  || aKeyEvent.iCode == EStdKeyDevice11 ) )      //   : Extra KeyEvent supports diagonal event simulator wedge
            {
            if ( 0 < iListBox->TopItemIndex() )
                {
                refreshNeeded = ETrue;
                refreshIndex = iListBox->TopItemIndex() - 1;
                }
            }
        if ( aType == EEventKey
             && (    aKeyEvent.iCode == EKeyLeftDownArrow        // Southwest
                  || aKeyEvent.iCode == EStdKeyDevice13          //   : Extra KeyEvent supports diagonal event simulator wedge
                  || aKeyEvent.iCode == EKeyDownArrow            // South
                  || aKeyEvent.iCode == EKeyRightDownArrow       // Southeast
                  || aKeyEvent.iCode == EStdKeyDevice12 ) )      //   : Extra KeyEvent supports diagonal event simulator wedge
            {
            if ( iListBox->BottomItemIndex() < ( iDownloadsListArray->Count() - 1 ) )
                {
                refreshNeeded = ETrue;
                refreshIndex = iListBox->BottomItemIndex() + 1;
                }
            }
        if ( refreshNeeded )
            {
            // Update the invisible download if it is progressing -
            // when it becomes visible, it should show fresh information.
            TInt32 dlAttrState = 0;
            RHttpDownload& download =
                iDownloadsListArray->Download( refreshIndex );
            User::LeaveIfError( download.GetIntAttribute( EDlAttrState, dlAttrState ) );
            
           //Changes for the bug JERI-7P8CF2
           //Changes made in the server side to fix the video center receiving unexpected events
           //Reassigning these events back to the changes done in server side
           if(dlAttrState  == EHttpDlCompleted )
		       {
                dlAttrState  = EHttpDlMultipleMOCompleted;
		       }
           else if(dlAttrState  == EHttpDlFailed )
		       {
		       dlAttrState  = EHttpDlMultipleMOFailed;
		       }
            
            if ( dlAttrState == EHttpDlInprogress )
                {
                RefreshProgressL( refreshIndex );
                // Start refresh timer - there will be at least one visible
                // download in EHttpDlInprogress state.
                if ( !iRefreshTimer->IsActive() )
                    {
                    iRefreshTimer->Start(
                        KRefreshIntervalMicroSeconds,
                        KRefreshIntervalMicroSeconds,
                        TCallBack( RefreshTimerCallback, this ) );
                    CLOG_WRITE(" iRefreshTimer strated");
                    }
                }
            }
        }


    // Forward it to the list box:
    if ( resp == EKeyWasNotConsumed )
        {
        // Handle Middle Softkey. If it's EKeyEscape sent by Music Player,
        // It is considered as being consumed, and no longer needs to do MSK update
        HandleMiddleSoftKeyChangeL();
        resp = iListBox->OfferKeyEventL( aKeyEvent, aType );
        }

    return resp;
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::VisibleInProgressDownloads
// -----------------------------------------------------------------------------
//
TInt CDownloadsListDlg::VisibleInProgressDownloads() const
    {
    TInt visibleInProgressDownloads = 0;

    if ( KRefreshIntervalMicroSeconds <= 0 )
        {
        // Refresh timer solution is not used. To support this in
        // a centralized way, this function returns 0 in this case.
        }
    else
        {
        TInt topItemIndex = iListBox->TopItemIndex();
        TInt bottomItemIndex = iListBox->BottomItemIndex();
        TInt32 dlAttrState = 0;
        if (topItemIndex >0)
            {
            for ( TInt i = topItemIndex; i <= bottomItemIndex; ++i )
                {
                RHttpDownload& download = iDownloadsListArray->Download(i);
                if ( download.GetIntAttribute( EDlAttrState, dlAttrState ) == KErrNone )
                    {
                    if ( dlAttrState == EHttpDlInprogress )
                        {
                        ++visibleInProgressDownloads;
                        }
                    }
                }
            }
        }
    return visibleInProgressDownloads;
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::RefreshProgressL
// -----------------------------------------------------------------------------
//
void CDownloadsListDlg::RefreshProgressL( TInt aIndex )
    {
    TInt32 dlAttrState = EHttpDlInprogress;
    RHttpDownload& download = iDownloadsListArray->Download( aIndex );
    // Create a local copy
    TDownloadUiData dlData = iDownloadsListArray->DlUiData( aIndex );
    // Update download state - this must always be done!
    dlData.iDownloadState = dlAttrState;
    // Update the progress info.
    User::LeaveIfError( download.GetIntAttribute( EDlAttrDownloadedSize,
                                                   dlData.iDownloadedSize ) );
    User::LeaveIfError( download.GetIntAttribute( EDlAttrLength,
                                                       dlData.iFullSize ) );
    TBool updated = EFalse;
    iDownloadsListArray->UpdateIfNeededL( aIndex, dlData, updated );
    }

// -----------------------------------------------------------------------------
// CDownloadsListDlg::RefreshTimerCallback
// -----------------------------------------------------------------------------
//
TInt CDownloadsListDlg::RefreshTimerCallback( TAny* aPtr )
    {
    // Avoid leaving functions in this callback!

    TInt err(0);
    CDownloadsListDlg* thisDialog = (CDownloadsListDlg*)aPtr;

    // When timer completes, the visible list box items' format string is
    // regenerated (this is relatively fast), and if it differs from the
    // current string, the list box item is updated, and then the timer is
    // restarted. This applies only for inprogress downlods.
    TInt topItemIndex = thisDialog->iListBox->TopItemIndex();
    TInt bottomItemIndex = thisDialog->iListBox->BottomItemIndex();
    // Count visible inprogress downloads. If there is no such,
    // timer is stopped.
    TInt visibleInProgressDownloads = 0;

    for ( TInt i = topItemIndex; i <= bottomItemIndex; ++i )
        {
        TInt32 dlAttrState = 0;
        RHttpDownload& download = thisDialog->iDownloadsListArray->Download(i);
        if ( download.GetIntAttribute( EDlAttrState, dlAttrState ) == KErrNone )
            {
            if ( dlAttrState == EHttpDlInprogress )
                {
                ++visibleInProgressDownloads;
                TBool updated = EFalse;

                // Create a local copy
                TDownloadUiData dlData =
                    thisDialog->iDownloadsListArray->DlUiData(i);
                // Update download state - this must always be done!
                dlData.iDownloadState = dlAttrState;
                // Update the progress info.
                err = download.GetIntAttribute( EDlAttrDownloadedSize,
                                                               dlData.iDownloadedSize );
                if ( !err )
                    {
                    err = download.GetIntAttribute( EDlAttrLength,
                                                                   dlData.iFullSize );
                    if ( !err )
                        {
                        TRAP( err, thisDialog->iDownloadsListArray->UpdateIfNeededL
                                        ( i, dlData, updated ) );
                        // Refresh the view
                        if ( !err && updated )
                            {
                            thisDialog->HandleModelChangeL( EDownloadChanged, i );
                            // HandleModelChangeL with EDownloadChanged is not leaving
                            // so no nedd to trap it!
                            }
                        }
                    }
                }
            }
        }

    if ( !visibleInProgressDownloads )
        {
        thisDialog->iRefreshTimer->Cancel();
        CLOG_WRITE(" iRefreshTimer cancelled");
        }

    return KErrNone;
    }

/* End of file. */
