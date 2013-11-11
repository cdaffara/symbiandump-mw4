/*
* Copyright (c) 2006, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include "WidgetUiWindow.h"
#include "WidgetUiWindowView.h"
#include "WidgetUiWindowContainer.h"
#include "WidgetUiWindowManager.h"
#include "WidgetUiDialogsProviderProxy.h"
#include "WidgetRegistryConstants.h"
#include "Browser_platform_variant.hrh"
#ifdef BRDO_WRT_HS_FF
#include "cpspublisher.h"
#endif
#include <AknUtils.h>
#include <imageconversion.h>
#include <WidgetUi.rsg>

#include <SchemeHandler.h>
#include <InternetConnectionManager.h>
#include "WidgetInstallerInternalCRKeys.h"
#include <BrowserUiSDKCRKeys.h>
#include <StringLoader.h>
#include <centralrepository.h>
#include <ApUtils.h>
#include <AiwGenericParam.h>
#include <ActiveApDb.h>
#include <AknsDrawUtils.h>
#include "SWInstWidgetUid.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
static const TUint32 KDummyCommand = 0;

const TUint KWmlNoDefaultAccessPoint = KMaxTUint; // see cenrep setting default -1 as int, here as uint
const TUint KWmlNoDefaultSnapId = KMaxTUint; // see cenrep setting default -1 as int, here as uint

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES
static HBufC* GetUlrL(
    RWidgetRegistryClientSession& aSession,
    const TUid& aUid );

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

HBufC* GetUlrL(
    RWidgetRegistryClientSession& aSession,
    const TUid& aUid )
    {

    CWidgetPropertyValue* val( aSession.GetWidgetPropertyValueL( aUid, EMainHTML ) );
    CleanupStack::PushL( val );

    HBufC* res( static_cast< const TDesC& >( *val ).AllocL() );
    CleanupStack::PopAndDestroy( val );
    return res;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWidgetUiWindow::CWidgetUiWindow()
// C++ constructor
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindow::CWidgetUiWindow( CWidgetUiWindowManager& aWindowManager, CCpsPublisher* aCpsPublisher )
    : iWindowManager( aWindowManager ), iCpsPublisher( aCpsPublisher ), iNetworkAccessGrant(EInvalid),
      iPreferredOrientation(TBrCtlDefs::EOrientationUndefined),
      iIsCurrent(EFalse), iShowSoftkeys(EFalse), iWidgetLoaded(EFalse),
      iSchemeProcessing (EFalse),iClickCount(0), iWidgetLoadStarted(EFalse),
      iNetworkState(ENetworkNotAllowed), iUserPermission(ETrue)
    {
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::ConstructL()
// Symbian constructor
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::ConstructL( const TUid& aUid )
    {
    iUid = aUid;

    iPenEnabled = AknLayoutUtils::PenEnabled();
    iShowSoftkeys = iPenEnabled;

    CWidgetUiWindowView* view = iWindowManager.View();
    // view->ShowActivatedObject(EFalse);
    //TRect rect( view->Container()->Rect() );

    TRect rect;
    RWidgetRegistryClientSession clientSession = iWindowManager.WidgetUIClientSession();
    iUrl = GetUlrL( clientSession, aUid );
    iWidgetUiObserver = CWidgetUiObserver::NewL( *this );

    iWidgetUiDialogsProviderProxy = CWidgetUiDialogsProviderProxy::NewL(*(iWindowManager.DialogsProvider()), NULL, *this);

#ifdef BRDO_WRT_HS_FF
    iNetworkModeWait = new(ELeave) CActiveSchedulerWait();
#endif

    unsigned int capabilities;
    if (iWindowManager.CursorShowMode() == TBrCtlDefs::ENoCursor)
        {
        //no cursor
        capabilities = TBrCtlDefs::ECapabilityLoadHttpFw |
                         TBrCtlDefs::ECapabilityWebKitLite |
                         TBrCtlDefs::ECapabilityClientResolveEmbeddedURL;
        }
    else
        {
        //cursor is setting
        capabilities = TBrCtlDefs::ECapabilityLoadHttpFw |
                        TBrCtlDefs::ECapabilityCursorNavigation|
                        TBrCtlDefs::ECapabilityWebKitLite |
                        TBrCtlDefs::ECapabilityClientResolveEmbeddedURL;

        }
    iEngine = CreateBrowserControlL(
        view->Container(),
        rect,
            capabilities,
        TBrCtlDefs::ECommandIdBase,
        iWidgetUiObserver, /* softkeys */
        iWidgetUiObserver, /* link resolver */
        this, /* special load observer */
        iWidgetUiObserver, /* layout observer */
        iWidgetUiDialogsProviderProxy,
        iWidgetUiObserver, /* window observer */
        iWidgetUiObserver /* download observer */);
    //Set the cursor mode inside Widget
    iEngine->SetBrowserSettingL(TBrCtlDefs::ESettingsCursorShowMode, iWindowManager.CursorShowMode());
    iEngine->SetBrowserSettingL(TBrCtlDefs::ESettingsEnterKeyMode, iWindowManager.EnterKeyMode());

    iEngine->AddLoadEventObserverL( iWidgetUiObserver );
    iEngine->AddStateChangeObserverL( view );
    iEngine->SetComponentsToInheritVisibility();

    iWidgetExtension = iEngine->CreateWidgetExtensionL( *this );
    iWidgetExtension->SetParamL( TBrCtlDefs::EWidgetIdentifier, aUid.iUid );

    iWidgetBundleId = HBufC::NewL(KWidgetRegistryVal);
    TPtr widgetBundleId = iWidgetBundleId->Des();
    iWidgetPath = HBufC::NewL(KWidgetRegistryVal);
    TPtr widgetPath = iWidgetPath->Des();

    clientSession.GetWidgetBundleId( aUid, widgetBundleId );
    clientSession.GetWidgetPath( aUid, widgetPath );

    iWidgetExtension->SetParamL( TBrCtlDefs::EWidgetBasePath, widgetPath );
    iWidgetExtension->SetParamL( TBrCtlDefs::EWidgetBundleId, widgetBundleId );

    //    SetSoftkeysVisible(iPenEnabled);// default softkeys visible for touch and invisible for non-touch

    //iEngine->MakeVisible( EFalse );

    // Updating Widget UI window state

    if(HasMiniviewL())
        {
        SetWindowStateMiniViewL( EMiniViewEnabled );
        }
    else
        {
        SetWindowStateMiniViewL( EMiniViewNotEnabled );
        }

    iDlId = 0;

    // determine initial widget online/offline network state
    DetermineNetworkState();
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::~CWidgetUiWindow()
// Destructor
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindow::~CWidgetUiWindow()
    {
    if (iEngine && iWidgetUiObserver)
        {
        iEngine->RemoveLoadEventObserver( iWidgetUiObserver );
        iEngine->RemoveStateChangeObserver( iWindowManager.View() );
        }
    delete iEngine;
    delete iWidgetUiObserver;
    delete iUrl;
    delete iWidgetPath;
    delete iWidgetBundleId;
    delete iRightSoftKeyLabel;
    delete iLeftSoftKeyLabel;
    delete iWidgetUiDialogsProviderProxy;
    delete iSchemeHandler;
    delete iMiniviewBitmap;
#ifdef BRDO_WRT_HS_FF
    delete iNetworkModeWait;
#endif
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::CbaGroup()
// returns cba group
//
// -----------------------------------------------------------------------------
//
CEikButtonGroupContainer* CWidgetUiWindow::CbaGroup()
    {
    return iWindowManager.View()->CbaGroup();
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::OpenWindowL()
// Creates CWidgetUiWindow object
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindow* CWidgetUiWindow::OpenWindowL( CWidgetUiWindowManager& aWindowManager,
                                         const TUid& aUid, CCpsPublisher* aCpsPublisher )
    {
    CWidgetUiWindow* window = new ( ELeave ) CWidgetUiWindow( aWindowManager, aCpsPublisher );
    CleanupStack::PushL( window );
    window->ConstructL( aUid );
    CleanupStack::Pop( window );
    return window;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::setWindowStateFullView()
// Sets the widget state if widget is opened in full view
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::SetWindowStateFullView( TBool aWidgetFullViewState)
    {
    iWidgetWindowState.iFullViewState = aWidgetFullViewState;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::setWindowStateMiniView()
// Sets the MiniView widget state
//
// -----------------------------------------------------------------------------
//
/*void CWidgetUiWindow::SetWindowStateMiniView( TInt widgetMiniViewState )
    {

    switch(widgetMiniViewState)
        {
        case 0: iWidgetWindowState.miniViewState = EMiniViewNotEnabled;
                break;
        case 1: iWidgetWindowState.miniViewState = EMiniViewEnabled;
                break;
        case 2: iWidgetWindowState.miniViewState = EPublishStart;
                break;
        case 3: iWidgetWindowState.miniViewState = EPublishSuspend;
                break;
        }
    }
    */
// -----------------------------------------------------------------------------
// CWidgetUiWindow::setWindowStateMiniView()
// Sets the MiniView widget state
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::SetWindowStateMiniViewL( TMiniViewState widgetMiniViewState)
    {
    iWidgetWindowState.iMiniViewState = widgetMiniViewState;
    if (  iWidgetWindowState.iMiniViewState == EPublishStart )
        iWidgetExtension->SetParamL( TBrCtlDefs::EWidgetPublishState, ETrue);
    else
        iWidgetExtension->SetParamL( TBrCtlDefs::EWidgetPublishState, EFalse);
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::getWidgetMiniViewState()
// Get  the MiniView widget state
//
// -----------------------------------------------------------------------------
//
TMiniViewState CWidgetUiWindow::WidgetMiniViewState() const
    {
    return iWidgetWindowState.iMiniViewState;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::getWidgetFullViewState()
// Get  the FullView widget state
//
// -----------------------------------------------------------------------------
//

TBool CWidgetUiWindow::WidgetFullViewState() const
    {
    return iWidgetWindowState.iFullViewState;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::DialogMimeFileSelectLC()
// Navigates through your file system and selects a file of specific mime type
//
// -----------------------------------------------------------------------------
//
TBool CWidgetUiWindow::DialogMimeFileSelectLC(HBufC*& aSelectedFileName,
                                 const TDesC& aMimeType)
    {
    return iWindowManager.DialogsProvider()->DialogMimeFileSelectLC(aSelectedFileName, aMimeType);
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::SetSoftkeysVisible()
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::SetSoftkeysVisible(TBool aVisible)
    {
    iShowSoftkeys = (aVisible);

    MakeSoftkeysVisible(aVisible, EFalse);

    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::SetDisplayMode()
// switch the display orientation to landscape or portrait
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::SetDisplayMode( TBrCtlDefs::TBrCtlOrientation aOrientation )
    {
    if (aOrientation != TBrCtlDefs::EOrientationUndefined)
        {
        iPreferredOrientation = aOrientation;
        }
    switch (iPreferredOrientation)
        {
        case TBrCtlDefs::EOrientationLandscape:
            {
            iWindowManager.SetDisplayLandscape();
            break;
            }
        case TBrCtlDefs::EOrientationPortrait:
            {
            iWindowManager.SetDisplayPortrait();
            break;
            }
        default:
            {
            iWindowManager.SetDisplayAuto();
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::SetSoftKeyLabelL()
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::SetSoftKeyLabelL( TBrCtlKeySoftkey aKeySoftkey, const TDesC& aText)
    {
    TInt command;
    if (aKeySoftkey == EKeyLeft)
        {
        command = EAknSoftkeyOptions;
        // save the softkey label if it changes
        if (iLeftSoftKeyLabel && iLeftSoftKeyLabel->Compare(aText))
            {
            delete iLeftSoftKeyLabel;
            iLeftSoftKeyLabel = NULL;
            }
        if (!iLeftSoftKeyLabel)
            {
            iLeftSoftKeyLabel = aText.AllocL();
            }
        }
    else // EKeyRight
        {
        command = EAknSoftkeyExit;
        // save the softkey label if it changes
        if (iRightSoftKeyLabel && iRightSoftKeyLabel->Compare(aText))
            {
            delete iRightSoftKeyLabel;
            iRightSoftKeyLabel = NULL;
            }
        if (!iRightSoftKeyLabel)
            {
            iRightSoftKeyLabel = aText.AllocL();
            }
        }

    // make sure softkeys are visible before setting the text
    // CEikButtonGroupContainer::Current() returns NULL if softkeys are not visible
    SetSoftkeysVisible(iShowSoftkeys);

    TInt err(KErrNotFound);
    // check to see if aText is a filename
    if (aText.Find(_L(".")) != KErrNotFound)
        {
        TRAP(err,SetSoftKeyImageL( command, aText ));
        }

    if (CbaGroup())
        {
        if (err != KErrNone)
            {
            CbaGroup()->SetCommandL( command, aText );
            }
        CbaGroup()->DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::SetSoftKeyImageL()
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::SetSoftKeyImageL(const TInt aCommand, const TDesC& aFilename)
    {
    HBufC* filename = HBufC::NewLC( iUrl->Length() + aFilename.Length() );
    TPtr ptr(filename->Des());
    TInt last = iUrl->LocateReverse('\\');
    if (last > 0)
        {
        ptr.Append(iUrl->Left(++last));
        }
    ptr.Append(aFilename);
    ptr.ZeroTerminate();

    RFs& server( CCoeEnv::Static()->FsSession() );

    CImageDecoder *iImageDecoder = CImageDecoder::FileNewL(server,
               ptr,
               CImageDecoder::EOptionAlwaysThread);

    TFrameInfo info = iImageDecoder->FrameInfo();
    TRect      rect = info.iFrameCoordsInPixels;

    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    CFbsBitmap* mask = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(mask);

    bitmap->Create( rect.Size(), info.iFrameDisplayMode );
    mask->Create( rect.Size(), EGray256 );

    TRequestStatus Status;
    iImageDecoder->Convert( &Status, *bitmap, *mask );
    User::WaitForRequest( Status );
    delete iImageDecoder;
    iImageDecoder = NULL;
    //bitmap->Compress();

    if (CbaGroup())
        {
        CbaGroup()->SetCommandL( aCommand, *bitmap, *mask ); // FYI This hasn't been implemented
        }

    CleanupStack::PopAndDestroy(3);// filename,bitmap,mask
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::Relayout()
// switch the display orientation based on preferred setting
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::Relayout( )
    {
    if (iIsCurrent)
        {
        SetDisplayMode(iPreferredOrientation);
        SetSoftkeysVisible(iShowSoftkeys);
        //iWindowManager.View()->Container()->SizeChanged();
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::SetCurrentWindow()
// sets this window current or not and resets the previous state
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::SetCurrentWindow( TBool aCurrent )
    {
    if (iIsCurrent != aCurrent && iWindowManager.View() && Engine())
        {
        iIsCurrent = aCurrent;

        iWindowManager.View()->UpdateStatusPane(EFalse); // deactivates any open edit boxes

        if (aCurrent)
            {
            UpdateCba();
            Engine()->MakeVisible( iWidgetLoaded );
            // redraw incase the orientation changed while in the background
            Relayout();
            }
        else
            {
            // close the menu
            iWindowManager.View()->StopDisplayingMenuBar();
            //deactivate optionsmenu flag
            iWindowManager.View()->DeActivateOptionsMenu();
            Engine()->MakeVisible(EFalse);// hide the active widget
            }
        if ( !aCurrent &&  (EPublishStart != WidgetMiniViewState()) )
            {
            iWidgetExtension->HandleCommandL ( (TInt)TBrCtlDefs::ECommandAppBackground + (TInt)TBrCtlDefs::ECommandIdBase );
            if( iWindowManager.AnyWidgetOnHs() )
                {
                //If there is no widget on HS then we should not stop the JS timers
                Engine()->HandleCommandL( (TInt)TBrCtlDefs::ECommandAppBackground + (TInt)TBrCtlDefs::ECommandIdBase);
                }
            }
        if ( aCurrent )
            {
            Engine()->HandleCommandL( (TInt)TBrCtlDefs::ECommandAppForeground + (TInt)TBrCtlDefs::ECommandIdBase);
            iWidgetExtension->HandleCommandL( (TInt)TBrCtlDefs::ECommandAppForeground + (TInt)TBrCtlDefs::ECommandIdBase );
           }
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::UpdateCba()
// Restore the softkeys when the widget comes from the background
// Reset the softkeys when the widget goes in the background
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::UpdateCba( )
    {
    if (iIsCurrent)
        {
        TBool showSoftkeys( iShowSoftkeys );// save state
            //CbaGroup()->MakeVisible( ETrue );
        if (iRightSoftKeyLabel)
            {
            // restore the Right Softkey
            TRAP_IGNORE(iWidgetUiObserver->UpdateSoftkeyL(EKeyRight,*iRightSoftKeyLabel,KDummyCommand,EChangeReasonLoad));
            }
        else
            {
            // reset the Right Softkey
            TRAP_IGNORE(iWidgetUiObserver->UpdateSoftkeyL(EKeyRight,KNullDesC,KDummyCommand,EChangeReasonLoad));
            }

        // restore state
        if ( !showSoftkeys )
            {
            CbaGroup()->MakeVisible( EFalse );
            }
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::ReloadWidget()
// load the widget
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::ReloadWidget( )
    {
    SetWidgetLoaded(EFalse);
    iWidgetLoadStarted = ETrue;
    TRAPD(err,iEngine->LoadFileL( iUrl->Des() ));
    if (err != KErrNone)
        {
        SetWidgetLoaded(ETrue);
        iWidgetLoadStarted = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::SetWidgetLoaded()
// notification that the widget has finished loading
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::SetWidgetLoaded( TBool aWidgetLoaded )
    {
    iWidgetLoaded = aWidgetLoaded;
    RWidgetRegistryClientSession& widgetRegistry = iWindowManager.WidgetUIClientSession();
    TInt fullView = widgetRegistry.IsWidgetInFullView( Uid() );

    if (iWidgetLoaded && iIsCurrent && fullView )
        {
        Engine()->MakeVisible( ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::PublishSnapShot()
// pubish offscreen bitmap to CPSpublisger
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::PublishSnapShot()
    {
    if( iWidgetLoaded && (WidgetMiniViewState() == EPublishStart ) )
        {
#ifdef BRDO_WRT_HS_FF

        if ( !iMiniviewBitmap )
            {
            iMiniviewBitmap = new CFbsBitmap();
            }

        if ( iMiniviewBitmap && iCpsPublisher)
            {
            TRAP_IGNORE(
               (iEngine->TakeSnapshotL( *iMiniviewBitmap ));
                iCpsPublisher->PublishBitmapL( *iMiniviewBitmap, *iWidgetBundleId );
                );
            }
#endif
        }
    }



// -----------------------------------------------------------------------------
// CWidgetUiWindow::HasMiniviewL()
// Check whether widget has miniview or not
//
// -----------------------------------------------------------------------------
//
TBool CWidgetUiWindow::HasMiniviewL()
    {
    RWidgetRegistryClientSession clientSession = iWindowManager.WidgetUIClientSession();
    CWidgetPropertyValue* propValue = clientSession.GetWidgetPropertyValueL( iUid, EMiniViewEnable );
    TInt hasMiniview = *propValue;
    delete propValue;
    return hasMiniview;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::CheckNetworkAccessL()
// Check Network access is allowed
//
// -----------------------------------------------------------------------------
//
TBool CWidgetUiWindow::CheckNetworkAccessL()
    {
    // if widgets in offline mode, deny network access
    if (iWindowManager.GetNetworkMode() == EOfflineMode && !( EMiniViewEnabled == WidgetMiniViewState() ||
        EMiniViewNotEnabled == WidgetMiniViewState() ))
        {
        // if widget is in full view, offer user the option to go to online mode.The widget should be in HS else dont ask HS for prompt.
#ifdef BRDO_WRT_HS_FF
        if ( WidgetFullViewState() && WidgetMiniViewState() != EPublishStart )
            {
            iCpsPublisher->NetworkConnectionAllowedL();
            iNetworkModeWait->Start();
            }
#endif
        if (iWindowManager.GetNetworkMode() == EOfflineMode)
            {
            SetNetworkAccessGrant( EDeny );
            User::Leave( KErrAccessDenied );
            }
        }

    // begin info.plist (declare EAllowNetworkAccess or EAllowFullAccess ?)
    RWidgetRegistryClientSession& widgetRegistry
                = iWindowManager.WidgetUIClientSession();

    CWidgetPropertyValue* propValue = widgetRegistry.GetWidgetPropertyValueL(iUid, EAllowNetworkAccess );
    TInt networkAccess = *propValue;
    delete propValue;
    propValue = widgetRegistry.GetWidgetPropertyValueL(iUid, EAllowFullAccess );
    TInt fullAccess = *propValue;
    delete propValue;

    if ( !( networkAccess || fullAccess ) )
        {
        SetNetworkAccessGrant( EDeny );
        User::Leave( KErrAccessDenied );
        }
    SetNetworkAccessGrant( EAllow );
    // end info.plist check
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::NetworkSecurityCheckL()
// Prompt for netwrok security
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::NetworkSecurityCheckL()
    {
    if ( EInvalid == NetworkAccessGrant() && CheckNetworkAccessL()
        && EDeny != NetworkAccessGrant())
        {
        TInt prompt =  1 ; // default to prompt

        // CRepository KWidgetPomptForNetworkAccess: missing => prompt,
        // 0 => allow, 1 => prompt
        CRepository* rep( NULL );

        // WidgetApp sharing WidgetInstaller cenrep
        TRAPD( cenrepError, rep = CRepository::NewL( TUid::Uid( KSWInstWidgetUIUid ) ) );
        if ( !cenrepError )
            {
             if ( rep->Get( KWidgetPomptForNetworkAccess, prompt ) )
                {
                    prompt = 1; // got error, force prompt
                }
                delete rep;
            }

            if ( prompt )
                {
                CBrowserDialogsProvider* dialogProvider
                            = iWindowManager.DialogsProvider();
                TBool grant = EFalse;
                HBufC* message = StringLoader::LoadLC( R_WIDGETUI_NETWORK_ACCESS );
                HBufC* yes = StringLoader::LoadLC( R_WIDGETUI_SOFTKEY_YES );
                HBufC* no = StringLoader::LoadLC( R_WIDGETUI_SOFTKEY_NO );
                grant = dialogProvider->DialogConfirmL( _L(""), *message, *yes, *no );
                CleanupStack::PopAndDestroy( 3 );
                // save prompt result for session
                SetNetworkAccessGrant( grant? EAllow : EDeny );

                CheckUserPermissionChanged( grant );
                }
            else
                {
                // cenrep setting is no prompt for session
                SetNetworkAccessGrant( EAllow );
                }
            }

        // after code above, which may have prompted user, perform a final access check
        if ( EAllow != NetworkAccessGrant( ) )
        {
        User::Leave( KErrCancel );
        }
    }


// -----------------------------------------------------------------------------
// CWidgetUiWindow::ConnectionManagement()
// Set up the access point from browser setting
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::ConnectionManagement()
    {
    if ( !iWindowManager.GetConnection()->Connected() )
        {
        // setup a new connection depending on browser settings

        // 2.1. deal with access point settings
        TInt ask( 1 );
        TInt wmlId( KWmlNoDefaultAccessPoint );
        TInt snapId( KWmlNoDefaultSnapId );
        CRepository* rep( NULL );
        TRAPD( cenrepError, rep = CRepository::NewL( KCRUidBrowser ) );
        if ( KErrNone == cenrepError )
            {
            (void)rep->Get( KBrowserAccessPointSelectionMode, ask );
            (void)rep->Get( KBrowserDefaultAccessPoint, wmlId );
            (void)rep->Get( KBrowserNGDefaultSnapId, snapId );
            }
        delete rep;
        if ( ask == EBrowserCenRepApSelModeDestination &&
           ( snapId != KWmlNoDefaultSnapId) )
            {
            iWindowManager.GetConnection()->SetRequestedSnap( snapId );
            iWindowManager.GetConnection()->SetConnectionType( CMManager::EDestination );
            }

        if ( !ask && (KWmlNoDefaultAccessPoint != wmlId) )
            {
            // set the access point (ap)
            CApUtils* apUtils( NULL );
            TInt apUtilError( KErrNone );
            TRAP(   apUtilError,
                    apUtils = CApUtils::NewLC( *(iWindowManager.GetDb()->Database()) );
                    TUint32 iap = apUtils->IapIdFromWapIdL( wmlId );
                    CleanupStack::PopAndDestroy();
                    iWindowManager.GetConnection()->SetRequestedAP( iap );
                );
            }
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::StartNetworkConnection()
// Start network connection
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::StartNetworkConnectionL(TBool* aNewConn)
    {
    *aNewConn = EFalse;
    if ( !iWindowManager.GetConnection()->Connected() )
        {
        // 2.2. make a connection
        TInt connFailure = iWindowManager.GetConnection()->StartConnectionL( ETrue );
        if (KErrCancel == connFailure)
            {
#ifdef BRDO_WRT_HS_FF
            iCpsPublisher->NetworkConnectionCancelledL();
#endif
            User::Leave( connFailure );
            }
        else if ( KErrNone != connFailure )
            {
            // Note: In case of no network signal, user may be prompted
            // multiple times to pick a connection. But that is likely the
            // least evil option, because they can pick another connection,
            // or hit cancel altogether in which case the KErrCancel block above
            // kicks in and sets a EDeny.
            User::Leave( connFailure );
            }
        *aNewConn = ETrue;

        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::NetworkConnectionNeededL()
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::NetworkConnectionNeededL( TInt* aConnectionPtr,
                                      TInt* aSockSvrHandle,
                                      TBool* aNewConn,
                                      TApBearerType* aBearerType )

    {
    // default in case connection setup fails
    *aConnectionPtr = 0;

    if( EMiniViewEnabled == WidgetMiniViewState() ||
        EMiniViewNotEnabled == WidgetMiniViewState())
        {
        // step 1: session security check
        NetworkSecurityCheckL();
        }
    else
        {
        CheckNetworkAccessL();
        }
    if( EAllow == NetworkAccessGrant() )
        {
        // step 2: manage the connection
        ConnectionManagement();
        StartNetworkConnectionL(aNewConn);

        // retest connected status since above code may have started a connection
        if ( iWindowManager.GetConnection()->Connected() )
            {
            // make leaving call first so if leaves, then do cleanup
            TRAPD( bearError,
               *aBearerType = iWindowManager.GetConnection()->CurrentBearerTypeL(); );
            if ( bearError )
                {
                // on error, close connection
                TRAP_IGNORE( iWindowManager.GetConnection()->StopConnectionL(); );
                *aNewConn = EFalse; // force to false in case it was true
                }
            else
                {
                // non leaving code only
                *aConnectionPtr = reinterpret_cast<TInt>( &iWindowManager.GetConnection()->Connection() );
                *aSockSvrHandle = iWindowManager.GetConnection()->SocketServer().Handle();
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::HandleRequestL()
//
// -----------------------------------------------------------------------------
//
TBool CWidgetUiWindow::HandleRequestL( RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray )
    {
    TBool ret( ETrue );
    TRAPD( err, DoHandleRequestL( aTypeArray, aDesArray ) );
     if( KErrNone != err )
        {
        iSchemeProcessing = EFalse;
        User::Leave( err );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindow::HandleRequestL()
//
// -----------------------------------------------------------------------------
//

TBool CWidgetUiWindow::HandleDownloadL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray)
    {
    TBool paramFound( EFalse );
    TPtrC fileName = ExtractBrCtlParam(
                        EParamLocalFileName,
                        aTypeArray,
                        aDesArray,
                        paramFound );
    TPtrC contentType = ExtractBrCtlParam(
                            EParamReceivedContentType,
                            aTypeArray,
                            aDesArray,
                            paramFound );

    HBufC8* contentType8 = 0;
    if ( !paramFound )
        {
        contentType8 = KNullDesC8().AllocLC();
        }
    else
        {
        contentType8 = HBufC8::NewLC( contentType.Length() );
        // 16 bit buffer copied into 8 bit buffer.
        contentType8->Des().Copy( contentType );
        }

    //Get the download Id of the transaction.
    //Because, resource loader will not delete the download
    //So after handling the download, it will be cleared.
    TPtrC dlId = ExtractBrCtlParam(
                            EParamTransactionId,
                            aTypeArray,
                            aDesArray,
                            paramFound );

    TLex lex(dlId);
    User::LeaveIfError(lex.Val(iDlId));

    TDataType dataType( *contentType8 );
    CAiwGenericParamList* genericParamList =
        BrCtlParamList2GenericParamListL( aTypeArray, aDesArray );
    CleanupStack::PushL( genericParamList );
    RFile tempFile;

    iWindowManager.DocHandler().SetExitObserver(this);
    iWindowManager.DocHandler().OpenTempFileL( fileName, tempFile );
    CleanupClosePushL( tempFile );
    iWindowManager.DocHandler().OpenFileEmbeddedL( tempFile,
                                 dataType,
                                 *genericParamList );


    CleanupStack::PopAndDestroy( &tempFile ); // tempFile
    CleanupStack::PopAndDestroy( genericParamList ); // genericParamList
    CleanupStack::PopAndDestroy( contentType8 ); // contentType8
    return EFalse;
    }

// ---------------------------------------------------------
// CWidgetUiWindow::HandleServerAppExit()
//
// ---------------------------------------------------------
//
void CWidgetUiWindow::HandleServerAppExit( TInt /*aReason*/ )
    {
    if(!iSchemeProcessing)
        {
        //Clear the download.
        iEngine->HandleDownloadCommandL(iDlId, TBrCtlDefs::EDownloadCmdCancel);
        iDlId = 0;
        return;
        }
    iSchemeProcessing = EFalse;
    }


// -----------------------------------------------------------------------------
// CWidgetUiWindow::DoHandleRequestL
//
// -----------------------------------------------------------------------------
//
TBool CWidgetUiWindow::DoHandleRequestL(
    RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray )
    {
    if( !iSchemeProcessing )
        {
        iSchemeProcessing = ETrue;
        TBool paramFound( EFalse );
        TPtrC requestUrl = ExtractBrCtlParam(
                EParamRequestUrl,
                aTypeArray,
                aDesArray,
                paramFound );

        // pass to the CSchemaHandler
        CSchemeHandler* newSH = CSchemeHandler::NewL( requestUrl );
        delete iSchemeHandler;
        iSchemeHandler = newSH;

        // construct the current accesspoint
        // if it's connected, always add it
        if ( iWindowManager.GetConnection() && iWindowManager.GetConnection()->Connected() )
            {
            CAiwGenericParamList* paramsList = CAiwGenericParamList::NewL();
            CleanupStack::PushL(paramsList);
            TInt32 ap = iWindowManager.GetConnection()->CurrentAPId();

            TAiwVariant paramVariant (ap);
            TAiwGenericParam genericParam( EGenericParamAccessPoint, paramVariant );
            paramsList->AppendL (genericParam );

            // set the parameter
            iSchemeHandler->SetParameterList( paramsList );

            CleanupStack::Pop();
            }

        iSchemeHandler->Observer( this );
        iSchemeHandler->HandleUrlEmbeddedL();
        }

    return ETrue;
    }

// ---------------------------------------------------------
// CWidgetUiWindow::ExtractBrCtlParam()
// ---------------------------------------------------------
//
TPtrC CWidgetUiWindow::ExtractBrCtlParam
    ( TUint aParamTypeToFind,
      RArray<TUint>* aTypeArray,
      CDesCArrayFlat* aDesArray,
      TBool& aParamFound ) const
    {

    // initialize output parameter
    aParamFound = EFalse;
    TPtrC16 retParamValue;

    for( TInt j = 0; j < aTypeArray->Count(); j++ )
        {
        const TUint paramType = (*aTypeArray)[j];
        if ( aParamTypeToFind == paramType )
            {
            // That's we need
            retParamValue.Set( aDesArray->MdcaPoint(j) );
            aParamFound = ETrue; // Indicate it in the out param
            break; // break the loop - we found it
            }
        }

    return retParamValue;
    }

// ---------------------------------------------------------
// CWidgetUiWindow::BrCtlParamList2GenericParamListL()
// ---------------------------------------------------------
//
CAiwGenericParamList* CWidgetUiWindow::BrCtlParamList2GenericParamListL(
                           RArray<TUint>* /*aTypeArray*/,
                           CDesCArrayFlat* /*aDesArray*/ ) const
    {

     //Creating generic param list
    CAiwGenericParamList* genericParamList = CAiwGenericParamList::NewLC();

        // Always add the access point, if currently connected
    if( iWindowManager.GetConnection() && iWindowManager.GetConnection()->Connected() )
        {
        TInt32 ap = iWindowManager.GetConnection()->CurrentAPId();
        TAiwVariant paramVariant (ap);
        TAiwGenericParam genericParam( EGenericParamAccessPoint, paramVariant );
        genericParamList->AppendL (genericParam );
        }
    CleanupStack::Pop( genericParamList ); // genericParamList
    return genericParamList;
    }

// ---------------------------------------------------------
// CWidgetUiWindow::CheckUserPermissionChanged()
// ---------------------------------------------------------
//
void CWidgetUiWindow::CheckUserPermissionChanged(TBool iCurrUserPerm)
    {
    if ( iUserPermission != iCurrUserPerm )
        {
        iUserPermission = iCurrUserPerm;
        DetermineNetworkState();
        }
    }

// ---------------------------------------------------------
// CWidgetUiWindow::DetermineNetworkState()
// ---------------------------------------------------------
//
void CWidgetUiWindow::DetermineNetworkState()
    {
    TNetworkState currNetState;
    RWidgetRegistryClientSession& widgetRegistry = iWindowManager.WidgetUIClientSession();
    TInt inMiniView = widgetRegistry.IsWidgetInMiniView( iUid);
    CWidgetPropertyValue* propValue = widgetRegistry.GetWidgetPropertyValueL( iUid, EAllowNetworkAccess );
    TInt netAccessWdgtProp = *propValue;    // AllowNetworkAccess in the info.plist file

    if ( netAccessWdgtProp && ((inMiniView && (iWindowManager.GetNetworkMode() == (TInt)EOnlineMode))
                                    || (!inMiniView && iUserPermission)) )
        {
        if ( iWindowManager.GetNetworkConn() )
            {
            currNetState = ENetworkAccessible;
            }
        else
            {
            currNetState = ENetworkAccessAllowed;
            }
        }
    else
        {
        currNetState = ENetworkNotAllowed;
        }

    if ( iNetworkState != currNetState )
        {
        iNetworkState = currNetState;
        // send the new widget network state to widget engine
        iWidgetExtension->SetParamL( TBrCtlDefs::EWidgetNetworkState, (TInt)iNetworkState );
        }
    }


// -----------------------------------------------------------------------------
// CWidgetUiWindow::MakeSoftkeysVisible()
// -----------------------------------------------------------------------------
//
void CWidgetUiWindow::MakeSoftkeysVisible(TBool aVisible, TBool aTextBoxUpdate)
    {
    if (CbaGroup() && !(aTextBoxUpdate && iShowSoftkeys))
        {
        CbaGroup()->MakeVisible( aVisible );
        //in case CBA keys are disbaled by javascript ,SetToolbarVisibility EFalse,
        //Since it would take entire screen.Else SetToolbarVisibility to ETrue in Landscape mode
        aVisible?iWindowManager.View()->UpdateToolbar(ETrue):iWindowManager.View()->UpdateToolbar(EFalse);
        CbaGroup()->DrawNow();
        TRect clientRect = iWindowManager.View()->ClientRect();
        // resize the container to take into account the size of the softkey labels
        // reduction is only needed for portrait mode (softkey on bottom) since landscape
        // mode softkeys don't take up screen real estate
        if (AknLayoutUtils::CbaLocation() == AknLayoutUtils::EAknCbaLocationBottom)
            {
            CbaGroup()->ReduceRect(clientRect);
            }
        //Check if the container rect needs change and then call setRect
        if(clientRect != iWindowManager.View()->Container()->Rect())
           iWindowManager.View()->Container()->SetRect(clientRect);

        if (!aTextBoxUpdate)
            {
            // status pane always off unless in text box
            iWindowManager.View()->UpdateStatusPane(EFalse);
            }
        }
    }
// End of file
