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
* Description:  Manages widget windows, each window display one widget
*
*/


#include "WidgetUiWindowManager.h"
#include "WidgetUiWindowView.h"
#include "WidgetUiWindowContainer.h"
#include "WidgetUiWindow.h"
#include "WidgetUiAppUi.h"
#include "WidgetUiNetworkListener.h"
#include "WidgetInstallerInternalCRKeys.h"
#include "SWInstWidgetUid.h"
#include "widgetappdefs.rh"
#include "Browser_platform_variant.hrh"
#ifdef BRDO_WRT_HS_FF
#include "cpspublisher.h"
#endif

#include <WidgetUi.rsg>
#include <brctlinterface.h>
#include <WidgetRegistryConstants.h>
#include <centralrepository.h>
#include <StringLoader.h>
#include <aknnotedialog.h>
#include <BrowserDialogsProvider.h>
#include <s32file.h>
#include <aknnotewrappers.h>
#include "cpglobals.h" // CPS string definitions.

#include <InternetConnectionManager.h>
#include <ActiveApDb.h>
#include <oommonitorsession.h>
#include <AknGlobalNote.h>

// LOCAL FUNCTION PROTOTYPES
TInt doDestructOOMNotifyTimer( TAny* ptr );
TInt doNotifyHarvester( TAny* ptr );

// CONSTANTS
#define KUidWidgetOOMPlugin 0x10282855
const TUint32 KCRUidActiveIdleLV = 0x10275102;
const TUint32 KAIWebStatus = 0x0000300F;
const TInt KMemoryToCreateWidgetWindow = 10*1024*1024;
const TInt KOOMNotificationDialogIntervalTimeOut = 60000000;
const TInt KOOMNotificationDialogTimeOut = 2000000;
const TInt KOOMHarvesterNotificationTimeOut = 5000000;
const TInt KOOMWidgetCloseTimeOut = 15;//Do not close the widget that was started after OOM within 15 sec

class CRequestRAM : public CActive
    {
public: 

    static CRequestRAM* StartLD(CWidgetUiWindowManager* aWidgetUiWindowManager, const TUid& aUid, TUint32 aOperation);
protected: // From CActive
    /**
    * Execute asynchronous operation.
    */
    void RunL();
    
    /**
    * Provide cancellation methods.
    */
    void DoCancel();
    void ConstructL();
    
private:

    /**
    * Constructor.
    */
    CRequestRAM (CWidgetUiWindowManager* aWidgetUiWindowManager, const TUid& aUid, TUint32 aOperation);
        
    /**
    * Destructor. 
    *
    * Private on purpose.
    */
    ~CRequestRAM();
    
private:
    ROomMonitorSession iOomSession;
    CWidgetUiWindowManager* iWidgetUiWindowManager;
    TUid iUid;
    TUint32 iOperation;
    };

// ============================= LOCAL FUNCTIONS ================================

static void NotifyCommandHandled()
    {
    const TUid KMyPropertyCat = { 0x10282E5A };
    enum TMyPropertyKeys { EMyPropertyState = 109 };
    TInt state( 1 );
    RProperty::Set( KMyPropertyCat, EMyPropertyState , state );
    }

// =============================================================================

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::CWidgetUiWindowManager()
// Default constructor
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindowManager::CWidgetUiWindowManager(CWidgetUiAppUi& aAppUi):
    iAppUi(aAppUi),
    iStrictMode(ETrue),
    iNetworkMode(EUnknownMode),
    iNetworkConnected(EFalse),
    iWidgetCursorMode(TBrCtlDefs::EDefaultCursor)
    {
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::ConstructL()
// Symbian constructor
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::ConstructL()
    {
    CRepository* cenRep(NULL);
    
    CCoeEnv::Static()->FsSession().CreatePrivatePath( EDriveC );
    // Create the client-side session
    User::LeaveIfError( iClientSession.Connect() );
    iServerConnected = ETrue;


    TRAPD(error,cenRep = CRepository::NewL( TUid::Uid( KSWInstWidgetUIUid ) ));
    if (!error)
        {
        TInt strictMode;
        TInt cursorMode = -1;
        TInt enterKeyMode = -1;
        
        if (cenRep->Get( KWidgetInstallerStrictMode, strictMode ) == KErrNone)
            {
            iStrictMode = strictMode;
            }
        if (cenRep->Get( KWidgetCursorShowMode, cursorMode ) == KErrNone)
            {
            iWidgetCursorMode = (TBrCtlDefs::TCursorSettings) cursorMode;
            }
        
        if (cenRep->Get( KWidgetEnterKeyMode, enterKeyMode ) == KErrNone)
            {
            iWidgetEnterKeyMode = (TBrCtlDefs::TEnterKeySettings) enterKeyMode;
            }
        
        delete cenRep;
        }
        
    iDialogsProvider = CBrowserDialogsProvider::NewL( NULL );
    iHandler = CDocumentHandler::NewL(CEikonEnv::Static()->Process());

    iDb = CActiveApDb::NewL( EDatabaseTypeIAP );
    iConnection = CInternetConnectionManager::NewL( iDb->Database(), EFalse );
#ifdef BRDO_WRT_HS_FF    
    iCpsPublisher = CCpsPublisher::NewL();
#endif
    
    iNetworkListener = CWidgetUiNetworkListener::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::NewL
// Two-phased constructor
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindowManager* CWidgetUiWindowManager::NewL( CWidgetUiAppUi& aAppUi )
    {
    CWidgetUiWindowManager* self = new ( ELeave ) CWidgetUiWindowManager( aAppUi );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::~CWidgetUiWindowManager()
// Destructor
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindowManager::~CWidgetUiWindowManager()
    {
    iWindowList.ResetAndDestroy();
    
    delete iNetworkListener;

    // TODO Why there is a "Disconnect" method in the first place...
    // RHandleBase::Close() should be enough?
    if ( iServerConnected )
        {
        iClientSession.Disconnect();
        }
        
    iClientSession.Close();
    
    delete iHandler;
    delete iDialogsProvider;

    if ( iConnection )
        {
        TRAP_IGNORE( iConnection->StopConnectionL() );
        }

    delete iConnection;
#ifdef BRDO_WRT_HS_FF  
    delete iCpsPublisher;
#endif
    delete iDb;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::DeactivateMiniViewL()
// Stop MiniView. Stops publishing and exits widget
//
// -----------------------------------------------------------------------------
//
TBool CWidgetUiWindowManager::DeactivateMiniViewL( const TUid& aUid )
    {
    CWidgetUiWindow* wdgt_window = GetWindow(aUid);
    
    if(!wdgt_window)
        return EFalse;
    wdgt_window->SetWindowStateMiniViewL( EMiniViewEnabled );

    // TODO also other states are possible when we should react?

    // Removing . Miniview, shall remove full view as well. For blanket permissions
    // will be revoked for miniview

    iClientSession.SetBlanketPermissionL( aUid, EBlanketUnknown );
    iClientSession.SetMiniViewL( aUid, EFalse );
    return CloseWindow( wdgt_window );
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::ActivateMiniView()
// Start publishing offscreenbitmap  to HS.
//
// -----------------------------------------------------------------------------
//
TBool CWidgetUiWindowManager::ActivateMiniViewL( 
    const TUid& aUid, 
    const TRect& aRect )
    {
    TBool res( EFalse );
    CWidgetUiWindow* wdgt_window( GetWindow( aUid ) );
    
    __ASSERT_DEBUG( wdgt_window, User::Invariant() );
    
    if ( wdgt_window->WidgetMiniViewState() != EMiniViewNotEnabled )
        {
        // We could throw User::Invariant() if state == EPublishStart, but that would be nasty...
        if ( wdgt_window->WidgetMiniViewState() != EPublishStart )
            {            
            iClientSession.SetMiniViewL( aUid, ETrue );

            HideWindow( iActiveFsWindow );
            //This is done to prevent offscreen bit map overlap 
            //when widget selected from FSW
            wdgt_window->Engine()->MakeVisible( EFalse );                    
            }
        
        res = ETrue;
        }
        
    return res;
    }

// ------------------------------------------------------------------------
// CWidgetUiWindowManager::HandleWidgetCommandL()
// Open or close widget window
//
// ------------------------------------------------------------------------
//
void CWidgetUiWindowManager::HandleWidgetCommandL( 
    const TUid& aUid,
    TUint32 aOperation )
    {
    TBool exit( EFalse );
    TBool needToNotify (ETrue) ;
    switch ( aOperation )
        {
        case LaunchFullscreen:
        case LaunchMiniview:
            {
            OpenOrCreateWindowL( aUid, aOperation );
            }
            break;
        case Deactivate:
            {
            exit = DeactivateMiniViewL( aUid );
            }
            break;
        case WidgetResume:
            {
            needToNotify = GetWindow(aUid) ? ETrue: EFalse;
            ResumeWidgetL( aUid );
            }
            break;
        case WidgetSuspend:
            {
            SuspendWidget( aUid );
            }
            break;
        case WidgetSelect:
            {            	
            // If we don't have window we know that WidgetUI has died
            // We must enable miniview state
            if( !GetWindow(aUid))
                {        
                needToNotify = EFalse;
                CanWindowBeCreated( aUid, aOperation );
                break;
                }
            //WidgetLauncher modified to bring app to foreground
            GetWindow( aUid)->IncrementClickCount();
            OpenOrCreateWindowL( aUid, LaunchFullscreen );            
            }
            break;
        case WidgetOnline:
            {
            iNetworkMode = EOnlineMode;
            CWidgetUiWindow* wdgt_window( GetWindow( aUid ) );
#ifdef BRDO_WRT_HS_FF
            if ( wdgt_window->NetworkModeWait()->IsStarted() )
                {
                wdgt_window->NetworkModeWait()->AsyncStop();
                }
#endif
            wdgt_window->DetermineNetworkState();
            }
            break;
       case WidgetOffline:
            {
            iNetworkMode = EOfflineMode;
            CWidgetUiWindow* wdgt_window( GetWindow( aUid ) );
#ifdef BRDO_WRT_HS_FF
            if ( wdgt_window->NetworkModeWait()->IsStarted() )
                {
                wdgt_window->NetworkModeWait()->AsyncStop();
                }
#endif
            // if no full view widgets open, then close the network connection
            if ( ( !FullViewWidgetsOpen() ) && ( iConnection->Connected() ) )
                {
                wdgt_window->Engine()->HandleCommandL( 
                        (TInt)TBrCtlDefs::ECommandIdBase +
                        (TInt)TBrCtlDefs::ECommandDisconnect );
                iConnection->StopConnectionL();
                }
            wdgt_window->DetermineNetworkState();
            }
            break;
       case WidgetRestart:
           {
           OpenOrCreateWindowL( aUid, LaunchMiniview );
           ResumeWidgetL( aUid );
           }
           break;
        }
    if(needToNotify)
    // Widget is up and running, notify that next one can be launched    
    	NotifyCommandHandled();
    
    if( exit )
        {
        iAppUi.Exit();
        }
    }

// ------------------------------------------------------------------------
// CWidgetUiWindowManager::CanWindowBeCreated()
// Check for availaibilty for window creation
//
// ------------------------------------------------------------------------
//
void CWidgetUiWindowManager::CanWindowBeCreated(const TUid& aUid, TUint32 aOperation)
    {
    CRequestRAM* requestRam = CRequestRAM::StartLD(this, aUid, aOperation);
    }

// ------------------------------------------------------------------------
// CWidgetUiWindowManager::OpenOrCreateWindowL()
// Open or create widget window
//
// ------------------------------------------------------------------------
//
void CWidgetUiWindowManager::OpenOrCreateWindowL( 
    const TUid& aUid,
    TUint32 aOperation )
    {
    CWidgetUiWindow* wdgt_window( GetWindow( aUid ) );
    TBool setFullViewState(EFalse);

#ifdef BRDO_WRT_HS_FF
    TSize miniviewSize( iCpsPublisher->BitmapSize());
#else
    TSize miniviewSize( 0,0 );
#endif
         
    if( wdgt_window ) 
        {
        // This might come from app shell as well where appshell window needs to be resized
        // then active window should be set
        //  if active window is there hide it
        // set the new window as current and active. Chcek if window needs resizing and set the registry as fullView and active
        // can be checked , we can launch in full view if Rect size is same as view size


        if ( aOperation == LaunchMiniview )
            {
            if ( !ActivateMiniViewL( aUid, miniviewSize ) )
                {
                wdgt_window = NULL;
                }
            }

        // If the existing widget is in miniview state, we need to make it full-screen.
        // Sanity check that we don't have to do anything extra.
        else 
            {
#ifdef BRDO_WRT_HS_FF
            //Make sure that all the mini widgets are already suspended
            SuspendAllWidget();
#endif
            // TODO Combine these methods somewhere in WidgetUiWindow, so it knows
            // how to tamper its own internal state. For example MakeFullScreen().
           
            setFullViewState = ETrue;
            iClientSession.SetFullViewL( aUid, ETrue );
            }
        }
    else
        {
        // Create a new window.
        if ( !IsWidgetAllowedL( aUid ) )
            {
            ShowNoteDialogL( R_WIDGETUI_STRICT_MODE_ACCESS );
            
            iAppUi.SendAppToBackground();
            }
        else 
            {
            // Does the widget support miniviews, in case that is desired?
            if ( aOperation == LaunchFullscreen || DoesWidgetSupportMiniviewL( aUid ) )
                {
                if ( aOperation == LaunchFullscreen )
                    iClientSession.SetFullViewL( aUid, ETrue );
                // Create window.
                wdgt_window = CreateNewWidgetWindowL( aUid );
            
                if ( aOperation )
                    {
                    // Don't care if this returns, we know miniviews are supported.
                    TRAPD( err, ActivateMiniViewL( aUid, miniviewSize ) );
                    if ( err )
                        {
                        CloseWindow( wdgt_window );
                        wdgt_window = NULL;
                        }
                    }
                else
                    {
#ifdef BRDO_WRT_HS_FF
                    SuspendAllWidget();
#endif
                    setFullViewState = ETrue;
                   
                    }
                }
            }
        }
        
    // Windows are up & running. Hopefully.
    if ( wdgt_window  )
        {
        if ( iActiveFsWindow != wdgt_window )
            {
            HideWindow( iActiveFsWindow );
            }
        
        if( setFullViewState )
            {
            iActiveFsWindow = wdgt_window;
            iActiveFsWindow->SetWindowStateFullView(ETrue);
            
            if ( iActiveFsWindow->Engine()->Rect() != View()->ClientRect())
                {
                iActiveFsWindow->Engine()->SetRect( View()->ClientRect() );
                }
            iActiveFsWindow->SetCurrentWindow( ETrue ); 
            iActiveFsWindow->Engine()->SetFocus(ETrue);
            //iActiveFsWindow->Engine()->MakeVisible( ETrue );
            
            }
        }
    iClientSession.SetActive( aUid, ETrue );
    }

// =============================================================================
// CWidgetUiWindowManager::GetWindow()
// return the window of a widget with a particular url
//
// =============================================================================
//
CWidgetUiWindow* CWidgetUiWindowManager::GetWindow( const TDesC& aUrl )
    {
    for ( TInt i = 0; i < iWindowList.Count(); i++ )
        {
        CWidgetUiWindow* window = iWindowList[i];
        if ( window->Url()->Des() == aUrl )
            {
            return window;
            }
        }
    return NULL;
    }

// =============================================================================
// CWidgetUiWindowManager::GetWindow()
// return the window of a widget with a particular Uid
//
// =============================================================================
//
CWidgetUiWindow* CWidgetUiWindowManager::GetWindow( const TUid& aUid )
    {
    for ( TInt i = 0; i < iWindowList.Count(); i++ )
        {
        CWidgetUiWindow* window = iWindowList[i];
        if ( window->Uid() == aUid )
            {
            return window;
            }
        }
    return NULL;
    }

// =============================================================================
// CWidgetUiWindowManager::CloseWindow()
// close window of widget
//
// =============================================================================
//
TBool CWidgetUiWindowManager::CloseWindow( CWidgetUiWindow* aWidgetWindow )
    {

    TBool lastOne( iWindowList.Count() == 1 );
    TBool ret( EFalse );
    ret = RemoveFromWindowList( aWidgetWindow );
     
    if ( !lastOne )
        {
        // Starting JS timer, since we stopped it for deactivating miniview widget
        //If there is a widget on HS then the timer will not be started
        CWidgetUiWindow* window = iWindowList[iWindowList.Count() - 1];
        if ( window && AnyWidgetPublishing())
               TRAP_IGNORE ( window->Engine()->HandleCommandL( 
                              (TInt)TBrCtlDefs::ECommandAppForeground + 
                              (TInt)TBrCtlDefs::ECommandIdBase));

        for ( TInt i = 0; i < iWindowList.Count(); ++i )// Fix needed. Do we need onShow here.
            {
            CWidgetUiWindow* window( iWindowList[i] );
            if ( window->WidgetMiniViewState() == EPublishStart )
                {
                    TRAP_IGNORE (window->WidgetExtension()->HandleCommandL(
                                   (TInt)TBrCtlDefs::ECommandAppForeground + 
                                   (TInt)TBrCtlDefs::ECommandIdBase));

                    break;
                }
            }

        }
    return ret;
    }

// =============================================================================
// CWidgetUiWindowManager::RemoveFromWindowList()
// remove widget window from window list
//
// =============================================================================
//
TBool CWidgetUiWindowManager::RemoveFromWindowList( CWidgetUiWindow* aWidgetWindow )
    {
    __ASSERT_DEBUG( aWidgetWindow, User::Invariant() );
    if ( iDialogsProvider->IsDialogLaunched() )
        {
        return EFalse;
        }

    if ( iClientSession.IsWidgetInFullView ( aWidgetWindow->Uid()))
        {
        HideWindow( aWidgetWindow );
        if (aWidgetWindow == iActiveFsWindow)
            iActiveFsWindow = NULL;
        // make widgets act like separate applications by pushing to background
        // when widget is in fullview , this way user is sent back to app shell or idle 
        // to run another widget
        iAppUi.SendAppToBackground();
        }

    // update the status of the widget to not active.
    TRAP_IGNORE(iClientSession.SetFullViewL( aWidgetWindow->Uid(), EFalse ));
    iClientSession.SetActive( aWidgetWindow->Uid(), EFalse );

    iWindowList.Remove( iWindowList.Find( aWidgetWindow ) );
    TBool lastOne( iWindowList.Count() == 0 );
    TRAP_IGNORE( aWidgetWindow->Engine()->HandleCommandL( 
            (TInt)TBrCtlDefs::ECommandIdBase +
            (TInt)TBrCtlDefs::ECommandCancelFetch ) );   
    if ( lastOne )
        {
        TRAP_IGNORE( aWidgetWindow->Engine()->HandleCommandL( 
                (TInt)TBrCtlDefs::ECommandIdBase +
                (TInt)TBrCtlDefs::ECommandDisconnect ) );

        delete aWidgetWindow;
        return ETrue;
        }
    else
        {
        delete aWidgetWindow;
        }
    return EFalse;
    }

// =============================================================================
// CWidgetUiWindowManager::CloseWindowWithLeastClick()
// return true for the last widgets  to be closed
//
// =============================================================================
//
TBool CWidgetUiWindowManager::CloseWindowWithLeastClick()
    {
   
    TInt temp(iWindowList[0]->GetClickCount());
    TInt err(KErrNone);
    CWidgetUiWindow* windowToBeClosed(NULL);
    TTime currentTime;
    currentTime.HomeTime();    
    TTimeIntervalSeconds  seconds;
    for ( TInt i = 0; i < iWindowList.Count(); i++ )
        {
        CWidgetUiWindow* window = iWindowList[i];        
        err = currentTime.SecondsFrom(window->GetTime(),seconds);
        if ( window->GetClickCount() <= temp && window != iActiveFsWindow &&  
             (err == KErrOverflow || seconds.Int() > KOOMWidgetCloseTimeOut))
            {
            temp  = window->GetClickCount();
            windowToBeClosed = window;
            }
        else if( window == iActiveFsWindow )
            {
            if ( window->WidgetMiniViewState() == EPublishStart ||
                 window->WidgetMiniViewState() == EPublishSuspend )
                {
                // Incase when the widget is active and as in full as well as miniview. 
                // it will stop publishing
                TRAP_IGNORE(window->SetWindowStateMiniViewL( EMiniViewEnabled ));
                }
            }
        }
        if ( windowToBeClosed)
            {
            return RemoveFromWindowList(windowToBeClosed);
            }

    return EFalse;
    }
// =============================================================================
// CWidgetUiWindowManager::Exit()
// Exit from widget and close widget window
//
// =============================================================================
//
void CWidgetUiWindowManager::Exit( TInt aCommand, const TUid& aUid )
    {
    CWidgetUiWindow* window( GetWindow( aUid ) );
    if( !window )
        return;
        
    if ( window->WidgetExtension() )
        {
        if ( window->WidgetExtension()->HandleCommandL( aCommand ) )
            return;    
        }
    
    if( ( window->WidgetMiniViewState() == EMiniViewEnabled ) ||
        ( window->WidgetMiniViewState() == EMiniViewNotEnabled ) ) 
        {
        // The widget is not publishing.
        TBool isOkToExit = CloseWindow( window );
        if ( isOkToExit)
            iAppUi.Exit();
        }
    else
        {
        // Since widget is in miniview we just push the widget app to background and
        // set the window status
        SendWidgetToBackground( aUid );
        }
    }

// =============================================================================
// CWidgetUiWindowManager::SendWidgetToBackground()
// Push the widget to background and set the window status
//
// =============================================================================
//
void CWidgetUiWindowManager::SendWidgetToBackground( const TUid& aUid )
    {
    CWidgetUiWindow* window( GetWindow( aUid ) );
    if( !window )
        return;

    // make widgets act like separate applications by pushing to background
    // this way user is sent back to app shell or idle to run another widget
    iAppUi.SendAppToBackground();
    if ( window == iActiveFsWindow )
        {
        //make the active window NULL and also CurrentWindow as False
        iActiveFsWindow->SetIsCurrentWindow(EFalse);
        iActiveFsWindow = NULL;        
        }        

    window->Engine()->MakeVisible( EFalse );
    window->SetWindowStateFullView( EFalse );
    //  Add registry info
    TRAP_IGNORE(iClientSession.SetFullViewL( window->Uid(), EFalse ));
    }

// =============================================================================
// CWidgetUiWindowManager::SetActiveWidgetAtExit()
// Set active widget to widget in full view which is not added to homescreen 
// incase no such widget is there set it to null
//
// =============================================================================
//
void CWidgetUiWindowManager::SetActiveWidgetAtExit()
    {
    //Get widget in Fullview that is not publishing and not same as active window
    for ( TInt i = 0; i < iWindowList.Count(); ++i )
        {
        CWidgetUiWindow* window( iWindowList[i] );
        if ( ( window != iActiveFsWindow ) &&   
             ( window->WidgetFullViewState() ) )
            {
            iActiveFsWindow = window;
            //ShowWindow( window );
            return;
            }
        }
        
    iActiveFsWindow = NULL;
    }

// =============================================================================
// CWidgetUiWindowManager::HandleLSKcommandL()
// Execute LSK Handler if present
//
// =============================================================================
//
TBool CWidgetUiWindowManager::HandleLSKCommandL( TInt aCommand )
    {
    if ( iActiveFsWindow && iActiveFsWindow->WidgetExtension() )

        {
         return iActiveFsWindow->WidgetExtension()->HandleCommandL( aCommand );
       }
    return EFalse;

    }

// =============================================================================
// CWidgetUiWindowManager::GetWidgetEntryForUrl()
// return Uid of widget with a particular Url
//
// =============================================================================
//
TInt CWidgetUiWindowManager::GetWidgetEntryForUrl( const TDesC& aUrl ) //const
    {
    return iClientSession.GetWidgetUidForUrl( aUrl );
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::SetDisplayLandscape()
// switch the display orientation to landscape
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::SetDisplayLandscape( )
    {
    TRAP_IGNORE(iAppUi.SetDisplayLandscapeL());
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::SetDisplayPortrait()
// switch the display orientation to portrait
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::SetDisplayPortrait( )
    {
    TRAP_IGNORE(iAppUi.SetDisplayPortraitL());
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::SetDisplayAuto()
// switch the display orientation to device defined
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::SetDisplayAuto( )
    {
    iAppUi.SetDisplayAuto();
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::HandleForegroundEvent
// called when application goes to background or comes back to foreground
//
// This method is called only for "Go to background" events. "Come to foreground"
// is filtered away by CWidgetUiAppUi::HandleForegroundEventL .
// But we need to check the aForeground value anyway since flipping back and
// forth from powersave mode doesn't involve OpenOrCreateWindowL.
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::HandleForegroundEvent( TBool aForeground )
    {
    if ( aForeground )
        {
        ShowWindow( iActiveFsWindow );
        }
    else
        {
        HideWindow( iActiveFsWindow );
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::HandleOOMEventL
// called to display  notification for out of memory when message is received 
// by app ui
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::HandleOOMEventL( TBool /*aForeground*/ )
    {
    if ( iNotifyOOMFlagTimer)
        return;
    
    HBufC* message = StringLoader::LoadLC( R_WIDGETUI_OOM_EVENT );
    
    TInt NoteId (-1);
    CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
    NoteId = dialog->ShowNoteL( EAknGlobalInformationNote, *message);
    User::After(KOOMNotificationDialogTimeOut);
    dialog->CancelNoteL(NoteId);

    CleanupStack::PopAndDestroy(dialog);
    CleanupStack::PopAndDestroy( message );// message
    
    iNotifyOOMFlagTimer = CPeriodic::NewL(CActive::EPriorityLow);
    iNotifyOOMFlagTimer->Start(KOOMNotificationDialogIntervalTimeOut,0,TCallBack(&doDestructOOMNotifyTimer,this));
    }
 
TInt doDestructOOMNotifyTimer( TAny* ptr )
    {
    CWidgetUiWindowManager* temp = static_cast<CWidgetUiWindowManager*>(ptr);
    if(temp)
        {
        temp->DeleteOOMNotifyTimer();                         
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::CloseWindowsAsync
// close all windows or current window async
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::CloseWindowsAsync( TBool aAllWindows )
    {
    iAppUi.AsyncExit(aAllWindows);
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::SuspendWidget
// Suspend currently publishing widget (if so).
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::SuspendWidget( const TUid& aUid )
    {
    CWidgetUiWindow* wdgt_window = GetWindow(aUid);

    if(!wdgt_window)
        return;

    if( (wdgt_window->WidgetMiniViewState() == EPublishStart) &&
        (wdgt_window->WidgetMiniViewState() != EPublishSuspend))
        {
        TRAP_IGNORE(
        wdgt_window->SetWindowStateMiniViewL(EPublishSuspend);

        wdgt_window->WidgetExtension()->HandleCommandL( 
            (TInt)TBrCtlDefs::ECommandAppBackground + 
            (TInt)TBrCtlDefs::ECommandIdBase);

        wdgt_window->Engine()->HandleCommandL( 
            (TInt)TBrCtlDefs::ECommandAppBackground + 
            (TInt)TBrCtlDefs::ECommandIdBase);
            );

        }

    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::ResumeWidget
// Resume publishing stuff, in case it is valid for the widget.
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::ResumeWidgetL( const TUid& aUid )
    {
    CWidgetUiWindow* wdgt_window( GetWindow( aUid ) );
    
    if ( iNetworkMode == EUnknownMode )
        {
        TInt HSOnlineMode = GetCenrepHSModeL();
        if (HSOnlineMode)
            {
            iNetworkMode = EOnlineMode;
            }
        else
            {
            iNetworkMode = EOfflineMode;
            }
        }
    
    // Window can be null if WidgetUI has been killed due to low memory situation
    //__ASSERT_DEBUG( wdgt_window, User::Invariant() );
    if(!wdgt_window)
        {
        //LAUNCH WIDGET Window
        CanWindowBeCreated(aUid,WidgetResume);		
		return;
    	}
    
    // reload widget now moved to resume for miniview widgets, to be called 
    // after determining online/offline mode
    if ( !(wdgt_window->IsWidgetLoaded() || wdgt_window->IsWidgetLoadStarted()) )
        {
        wdgt_window->ReloadWidget();
        }
		
    if( (wdgt_window ->WidgetMiniViewState() == EMiniViewEnabled) ||
        (wdgt_window->WidgetMiniViewState() == EPublishSuspend) )
        {
        //Widgets on HS cannnot be active
        iActiveFsWindow = NULL;
        // Publish should start only after widget is resumed.
        wdgt_window->SetWindowStateMiniViewL(EPublishStart);

        wdgt_window->Engine()->HandleCommandL( 
            (TInt)TBrCtlDefs::ECommandAppForeground + 
            (TInt)TBrCtlDefs::ECommandIdBase);

        wdgt_window->WidgetExtension()->HandleCommandL(
            (TInt)TBrCtlDefs::ECommandAppForeground + 
            (TInt)TBrCtlDefs::ECommandIdBase);
#ifdef BRDO_WRT_HS_FF 
        wdgt_window->Engine()->MakeVisible( EFalse );
        wdgt_window->Engine()->SetRect( iCpsPublisher->BitmapSize());
#endif
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::SuspendAllWidget
// SUspend all publishing widgets, if not already done.
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::SuspendAllWidget()
    {
    for ( TInt i = 0; i < iWindowList.Count(); i++ )
        {
        CWidgetUiWindow* window = iWindowList[i];
        if ( window->WidgetMiniViewState() == EPublishStart)
            {
            SuspendWidget(window->Uid());
            }
        }

    }

// ------------------------------------------------------------------------
// CWidgetUiWindowManager::HideWindow
//
// Hide currently active window.
// ------------------------------------------------------------------------
void CWidgetUiWindowManager::HideWindow( CWidgetUiWindow* aWindow )
    {
    if ( aWindow )
        {
        // Hide the previously active widget.
        aWindow->SetCurrentWindow(EFalse);        
        }
    }
 
// ------------------------------------------------------------------------
// CWidgetUiWindowManager::IsWidgetAllowedL
// 
// Check widget's compatibility for running.
// ------------------------------------------------------------------------
TBool CWidgetUiWindowManager::IsWidgetAllowedL( 
    const TUid& aUid ) //const
    {
    __UHEAP_MARK;
    
    TBool res( EFalse );
    CWidgetPropertyValue* value( iClientSession.GetWidgetPropertyValueL(
        aUid, 
        ENokiaWidget ) );
    
    if ( iStrictMode )
        {
        if ( *value )
            {
            res = ETrue;
            }
        }
    else
        {
        res = *value;
        }
    
    delete value;
    
    __UHEAP_MARKEND;
    return res;
    }
 
// ------------------------------------------------------------------------
// CWidgetUiWindowManager::ShowNoteDialogL
// 
// Show a note to user.
// ------------------------------------------------------------------------
void CWidgetUiWindowManager::ShowNoteDialogL( TInt aResourceID ) const
    {
    CEikDialog* dialog( NULL );
    HBufC* message( StringLoader::LoadLC( aResourceID ) );
    
    dialog = new (ELeave) CAknNoteDialog( 
            &dialog,
            CAknNoteDialog::ENoTone, 
            CAknNoteDialog::ELongTimeout );
    
    CleanupStack::PushL( dialog );
    static_cast< CAknNoteDialog* >( dialog )->SetTextL( *message );
    CleanupStack::Pop( dialog );
    
    CleanupStack::PopAndDestroy( message );
    dialog->ExecuteLD(R_WIDGETUI_OK_NOTE);
    delete dialog;
    dialog = NULL;
    }

// ------------------------------------------------------------------------
// CWidgetUiWindowManager::CreateNewWidgetWindowL
//
// New widget, new window.
// ------------------------------------------------------------------------
CWidgetUiWindow* CWidgetUiWindowManager::CreateNewWidgetWindowL( 
    const TUid& aUid )
    {
#ifdef BRDO_WRT_HS_FF  
    CWidgetUiWindow* window(
        CWidgetUiWindow::OpenWindowL( *this, aUid, iCpsPublisher ) );
#else
    CWidgetUiWindow* window(
        CWidgetUiWindow::OpenWindowL( *this, aUid, NULL ) );
#endif
    
    CleanupStack::PushL( window );

    // Add it to the queue
    iWindowList.AppendL( window );
    
    CleanupStack::Pop( window );
    
    // reset the display orientation when the widget is launched
    iAppUi.SetDisplayAuto();

    // reload widget only for full view widgets
    if ( iClientSession.IsWidgetInFullView(aUid) )
        {
        window->ReloadWidget();
        }
    
    return window;
    }
    
// ------------------------------------------------------------------------
// CWidgetUiWindowManager::DoesWidgetSupportMiniviewL
//
// Register widget to CPS listener.
// ------------------------------------------------------------------------
TBool CWidgetUiWindowManager::DoesWidgetSupportMiniviewL( const TUid& aUid )
    {
    __UHEAP_MARK;
    CWidgetPropertyValue* val( iClientSession.GetWidgetPropertyValueL( aUid, EMiniViewEnable ) );
    TBool res( *val );
    
    delete val;
    
    __UHEAP_MARKEND;
    
    return res;
    }

void CWidgetUiWindowManager::ShowWindow( CWidgetUiWindow* aWindow )
    {
    if ( !aWindow )
        {
        iAppUi.SendAppToBackground();
        return;
        }
    if ( aWindow != iActiveFsWindow )
        {
        HideWindow( iActiveFsWindow );
        }
        
    iActiveFsWindow = aWindow;
    iActiveFsWindow->SetCurrentWindow( ETrue );
    iActiveFsWindow->Engine()->MakeVisible( ETrue );
    }
// ------------------------------------------------------------------------
// CWidgetUiWindowManager::DeleteOOMNotifyTimer
//
// ------------------------------------------------------------------------
    
void CWidgetUiWindowManager::DeleteOOMNotifyTimer()
    {
    iNotifyOOMFlagTimer->Cancel();
    delete iNotifyOOMFlagTimer;
    iNotifyOOMFlagTimer = NULL;
    }
    
void CWidgetUiWindowManager::StartHarvesterNotifyTimer()
{
    if(iNotifyHarvester)
        DeleteHarvesterNotifyTimer();
    iNotifyHarvester = CPeriodic::NewL(CActive::EPriorityLow);
    iNotifyHarvester->Start(KOOMHarvesterNotificationTimeOut,0,TCallBack(&doNotifyHarvester,this));
}

TInt doNotifyHarvester( TAny* ptr )
{
    NotifyCommandHandled();
    CWidgetUiWindowManager* temp = static_cast<CWidgetUiWindowManager*>(ptr);
    if(temp)
        temp->DeleteHarvesterNotifyTimer();
    return EFalse;    
}

void CWidgetUiWindowManager::DeleteHarvesterNotifyTimer()
{
    iNotifyHarvester->Cancel();
    delete iNotifyHarvester;
    iNotifyHarvester = NULL;    
}

// ------------------------------------------------------------------------
// CWidgetUiWindowManager::GetCenrepHSModeL
//
// Determine the homescreen network mode (online/offline) from the cenrep
// ------------------------------------------------------------------------
TInt CWidgetUiWindowManager::GetCenrepHSModeL()
    {
    TInt value( 0 );
    CRepository* rep( NULL );
    TRAPD( cenrepError, rep = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) ) );
    if ( KErrNone == cenrepError )
        {
        (void)rep->Get( KAIWebStatus, value );
        }
    delete rep;
    
    return value;
    }

// ------------------------------------------------------------------------
// CWidgetUiWindowManager::FullViewWidgetsOpen
//
// Checks if any full view widgets are open 
// ------------------------------------------------------------------------
TBool CWidgetUiWindowManager::FullViewWidgetsOpen()
    {
    for ( TInt i = 0; i < iWindowList.Count(); ++i )
        {
        CWidgetUiWindow* window( iWindowList[i] );
        if ( ( window->WidgetFullViewState() ) && ( !iClientSession.IsWidgetInMiniView( window->Uid() ) ) )
            {
            return ETrue;
            }
        }
    
    return EFalse;
    }

// ------------------------------------------------------------------------
// CWidgetUiWindowManager::NotifyConnecionChange
//
// Notify widgets of a network connection change 
// ------------------------------------------------------------------------
void CWidgetUiWindowManager::NotifyConnecionChange( TBool aConn )
    {
    iNetworkConnected = aConn;
    for ( TInt i = 0; i < iWindowList.Count(); ++i )
        {
        CWidgetUiWindow* window( iWindowList[i] );
        window->DetermineNetworkState();
        }
    }

#ifdef OOM_WIDGET_CLOSEALL
// ------------------------------------------------------------------------
// CWidgetUiWindowManager::CloseAllWidgetsUnderOOM
//
// In case of OOM closes all widgets. 
// ------------------------------------------------------------------------
TBool  CWidgetUiWindowManager::CloseAllWidgetsUnderOOM()
    {
    TInt temp(0);
    TInt err(KErrNone);
    CWidgetUiWindow* windowToBeClosed(NULL);
    TTime currentTime;
    currentTime.HomeTime();    
    TTimeIntervalSeconds  seconds;
    TInt nCountWidgetClosed = 0;
    TBool bAllWindowsClosed = ETrue;
      
    TInt nWidgetsCount = iWindowList.Count();
    for ( TInt i = (nWidgetsCount-1); i >= 0; i-- )
        {
        CWidgetUiWindow* window = iWindowList[i];        
        err = currentTime.SecondsFrom(iTimeLastWidgetOpen,seconds);
        if ( err == KErrOverflow || seconds.Int() > KOOMWidgetCloseTimeOut)
           {
           windowToBeClosed = window;
           if ( windowToBeClosed)
               {
               RemoveFromWindowList(windowToBeClosed);
               nCountWidgetClosed++;
               }
           }
        }
    if(nWidgetsCount == nCountWidgetClosed)
        {
        bAllWindowsClosed = ETrue;
        }
    else
        {
        bAllWindowsClosed = EFalse;
        }
    return bAllWindowsClosed;
    }
 
#endif  //OOM_WIDGET_CLOSEALL    
void CWidgetUiWindowManager::CloseAllWidgets()  
   {   
   TInt nWidgetsCount = iWindowList.Count();  
       for ( TInt i = (nWidgetsCount-1); i >= 0; i-- )  
           {  
           CWidgetUiWindow* window = iWindowList[i];    
           delete window;  
           }  
   iWindowList.Reset();
   } 

void CWidgetUiWindowManager::SendAppToBackground()
    {
    iAppUi.SendAppToBackground();    
    }

TBool CWidgetUiWindowManager::AnyWidgetOnHs()
    {
    for ( TInt i = 0; i < iWindowList.Count(); ++i )
        {
        CWidgetUiWindow* window( iWindowList[i] );
        if( window->WidgetMiniViewState() != EMiniViewEnabled && window->WidgetMiniViewState() != EMiniViewNotEnabled )
            return ETrue;
        }
    return EFalse;
    }

TBool CWidgetUiWindowManager::AnyWidgetPublishing()
    {
    for ( TInt i = 0; i < iWindowList.Count(); ++i )
        {
        CWidgetUiWindow* window( iWindowList[i] );
        if( window->WidgetMiniViewState() == EPublishStart )
            return ETrue;
        }
    return EFalse;
    }

CRequestRAM::CRequestRAM(CWidgetUiWindowManager* aWidgetUiWindowManager, const TUid& aUid, TUint32 aOperation):
    CActive( EPriorityStandard ),
    iOperation(aOperation),
    iUid(aUid),
    iWidgetUiWindowManager(aWidgetUiWindowManager)
    {
	}
	
CRequestRAM* CRequestRAM::StartLD(CWidgetUiWindowManager* aWidgetUiWindowManager, const TUid& aUid, TUint32 aOperation)
    {
     CRequestRAM* self = new ( ELeave ) CRequestRAM(aWidgetUiWindowManager, aUid, aOperation); 
     CleanupStack::PushL( self );
     self->ConstructL();
     CleanupStack::Pop();
     return self;
    }
    
void CRequestRAM::ConstructL()
    {
    User::LeaveIfError(iOomSession.Connect());
    CActiveScheduler::Add( this );
#ifdef FF_OOM_MONITOR2_COMPONENT
    iOomSession.RequestOptionalRam(KMemoryToCreateWidgetWindow, KMemoryToCreateWidgetWindow, KUidWidgetOOMPlugin, iStatus);
    SetActive();
#else
    TMemoryInfoV1Buf  info;
    UserHal::MemoryInfo (info);                
    TInt err = info().iFreeRamInBytes > KMemoryToCreateWidgetWindow ? KErrNone : KErrNoMemory;   
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, err); 
#endif
    }

void CRequestRAM::RunL()
    {    
    if(iStatus >= 0)
        {        
        iWidgetUiWindowManager->OpenOrCreateWindowL( iUid, LaunchMiniview );
        iWidgetUiWindowManager->ResumeWidgetL( iUid );
        iWidgetUiWindowManager->GetWindow(iUid)->SetTime();
#ifdef OOM_WIDGET_CLOSEALL        
        iWidgetUiWindowManager->SetLastWidgetRestartTime( iWidgetUiWindowManager->GetWindow(iUid)->GetTime());
#endif //OOM_WIDGET_CLOSEALL
        
        switch ( iOperation )
            {
            case WidgetSelect:
                {
                iWidgetUiWindowManager->GetWindow(iUid)->IncrementClickCount();                
                iWidgetUiWindowManager->OpenOrCreateWindowL( iUid, LaunchFullscreen );
                iWidgetUiWindowManager->GetWindow(iUid)->SetWindowStateMiniViewL(EPublishSuspend); 
                }
                break;
            case WidgetResume:
                {                

                }
                break;                
            } 
        iWidgetUiWindowManager->StartHarvesterNotifyTimer();            
        }
    else
        {
        NotifyCommandHandled();
        iWidgetUiWindowManager->SendAppToBackground();  
        iWidgetUiWindowManager->WidgetUIClientSession().SetActive( iUid, EFalse );
        }        
    delete this;    
    }


CRequestRAM::~CRequestRAM()
    {
    Cancel();
    iOomSession.Close();
    }
void CRequestRAM::DoCancel()
    {
    iOomSession.CancelRequestFreeMemory();
    }    

// End of file
