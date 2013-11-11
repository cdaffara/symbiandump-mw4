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


#include "WidgetUiAppUi.h"
#include "WidgetUiWindowView.h"
#include "WidgetUiWindowManager.h"
#include "WidgetUiWindow.h"
#include "WidgetUiAsyncExit.h"
#include "widgetappdefs.rh"
#include <WidgetUi.rsg>
#include "WidgetUi.hrh"
#include <s32mem.h>
#include <avkon.hrh>
#include <e32std.h>
#include <bautils.h>
#include <oommonitorplugin.h>
#include <BrowserDialogsProvider.h>
#include <e32property.h>

#ifdef BRDO_WRT_HS_FF
#include <akntoolbar.h>
#endif

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// const TUid KWidgetAppUid  = TUid::Uid( 0x10282822 );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

//LOCAL FUNCTION
void NotifyCommandHandled()
    {
    const TUid KMyPropertyCat = { 0x10282E5A };
    enum TMyPropertyKeys { EMyPropertyState = 109 };
    TInt state( 2 );
    RProperty::Set( KMyPropertyCat, EMyPropertyState , state );
    }

// ============================ MEMBER FUNCTIONS ================================
//
CWidgetUiAppUi::CWidgetUiAppUi()
    {
    //Nothing
    }

// ------------------------------------------------------------------------------
// CWidgetUiAppUi::ConstructL
// Symbian constructor
//
// ------------------------------------------------------------------------------
//
void CWidgetUiAppUi::ConstructL()
    {
    CWidgetUiWindowView* contentView( NULL );
    BaseConstructL( EAknEnableSkin | EAknEnableMSK );

    //
    iWindowManager = CWidgetUiWindowManager::NewL( *this );
    iWidgetUiAsyncExit = CWidgetUiAsyncExit::NewL( *this );
    iDelayedForegroundEvent = CIdle::NewL(CActive::EPriorityIdle);
    contentView = CWidgetUiWindowView::NewLC( *iWindowManager );

    iWindowManager->SetView( *contentView );
    // Transfer ownership to CAknViewAppUi
    AddViewL( contentView );
    CleanupStack::Pop( contentView );

    SetRotationSupport();
    LoadAdditionalResourcefile();
    }

// ------------------------------------------------------------------------------
// CWidgetUiAppUi::LoadAdditionalResourcefile
// Load resource file for creating dynamic options menu
//
// ------------------------------------------------------------------------------
//
void CWidgetUiAppUi::LoadAdditionalResourcefile()
    {
    // add widgetmenu resource file to the coe environment
    // load resource file
    TFileName resourceFileName;
    TParse parse;
    CEikAppUi* pAppUI = (CEikAppUi*)(CCoeEnv::Static()->AppUi());
    TFileName dllName = pAppUI->Application()->DllName();

    parse.Set( dllName, NULL, NULL );
    resourceFileName += parse.Drive();
     _LIT(WIDGET_RESOURCE_DIR,"\\resource\\");
    resourceFileName += WIDGET_RESOURCE_DIR;
    resourceFileName += _L("widgetmenu.rsc");

    CCoeEnv* coeEnv = CCoeEnv::Static();
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFileName  );
    iResourceFile = coeEnv->AddResourceFileL( resourceFileName );
    }

// ------------------------------------------------------------------------------
// CWidgetUiAppUi::~CWidgetUiAppUi
// Destructor
//
// ------------------------------------------------------------------------------
//
CWidgetUiAppUi::~CWidgetUiAppUi()
    {
    // remove resource file
    CCoeEnv::Static()->DeleteResourceFile( iResourceFile );
    delete iWindowManager;
    if (iDelayedForegroundEvent)
        {
        iDelayedForegroundEvent->Cancel(); 
        }
    delete iDelayedForegroundEvent;
    delete iWidgetUiAsyncExit;
    }

// ------------------------------------------------------------------------------
// CWidgetUiAppUi::::DynInitMenuPaneL
// This function is called by the EIKON framework just before it displays
// a menu pane. Its default implementation is empty, and by overriding it,
// the application can set the state of menu items dynamically according
// to the state of application data.
// ------------------------------------------------------------------------------
//
void CWidgetUiAppUi::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    ActiveView()->DynInitMenuPaneL( aResourceId, aMenuPane );
    }

// ------------------------------------------------------------------------------
// CWidgetUiAppUi::HandleCommandL
// Handles user commands
//
// ------------------------------------------------------------------------------
//
void CWidgetUiAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
            {
            iWindowManager->CloseAllWidgets();
            Exit();
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ------------------------------------------------------------------------------
// CWidgetUiAppUi::ActiveView
// returns the app UI's currently active view
//
// ------------------------------------------------------------------------------
//
CAknView* CWidgetUiAppUi::ActiveView()
    {
    TVwsViewId activeViewId;

    if ( GetActiveViewId( activeViewId ) == KErrNone )
        {
        return View( activeViewId.iViewUid );
        }
    return NULL;
    }


// ------------------------------------------------------------------------------
// CWidgetUiAppUi::ProcessCommandParametersL
// Process shell commands
//
// ------------------------------------------------------------------------------
//
TBool CWidgetUiAppUi::ProcessCommandParametersL(CApaCommandLine &aCommandLine)
    {   
    TBool res( ETrue );
    
    const TPtrC doc( aCommandLine.DocumentName() );
    TInt len( doc.Length() * 2 );
    
    if ( len )
        {
        HBufC8* params( HBufC8::NewMaxLC( len ) );
        
        Mem::Copy( 
            const_cast< TUint8* >( params->Ptr() ),
            reinterpret_cast< const TUint16* >( doc.Ptr() ),
            len );
        
        LaunchWindowL( *params );

        CleanupStack::PopAndDestroy( params );
        }
    else
        {
        res = CEikAppUi::ProcessCommandParametersL( aCommandLine );
        }
        
    return res;
}

// ------------------------------------------------------------------------------
// CWidgetUiAppUi::LaunchWindowL
// Process shell commands
//
// ------------------------------------------------------------------------------
//
void CWidgetUiAppUi::LaunchWindowL( const TDesC8& aMessage )
    {
    TUid uid( KNullUid );
    TUint32 operation( 0 );
    
    ProcessMessageArgumentsL( aMessage, uid, operation );
        
    iWindowManager->HandleWidgetCommandL( uid, operation );

    }

// ------------------------------------------------------------------------------
// CWidgetUiAppUi::HandleMessageL
// Called by WidgetLaucher
//
// ------------------------------------------------------------------------------
//
MCoeMessageObserver::TMessageResponse CWidgetUiAppUi::HandleMessageL(
                    TUint32 aClientHandleOfTargetWindowGroup,
                    TUid aMessageUid,
                    const TDesC8& aMessageParameters )
    {
    MCoeMessageObserver::TMessageResponse res( EMessageNotHandled );
    // TODO Magic number 1 comes from Widget Launcher.
    if ( ( aMessageUid == TUid::Uid( 1 ) ) )
        {
        // Hide red exclamation marks, as it is the habit of UI applications.
        LaunchWindowL( aMessageParameters );
        
        res = EMessageHandled;
        }
    else
        {
        res = CAknViewAppUi::HandleMessageL( 
            aClientHandleOfTargetWindowGroup,
            aMessageUid,
            aMessageParameters );
        }
    
    return res;
    }

// ------------------------------------------------------------------------------
// CWidgetUiAppUi::HandleResourceChangeL
// Handles change to the application's resources shared across the environment
//
// ------------------------------------------------------------------------------
//
void CWidgetUiAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknViewAppUi::HandleResourceChangeL( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch && iWindowManager && iWindowManager->ActiveWindow())
        {
        iWindowManager->ActiveWindow()->Relayout();
        }
    }

// ------------------------------------------------------------------------------
// CWidgetUiAppUi::HandleWsEventL
// Handles events sent to the application by the window server
//
// ------------------------------------------------------------------------------
//
void CWidgetUiAppUi::HandleWsEventL(
    const TWsEvent& aEvent, CCoeControl* aDestination )
    {

    if ( aEvent.Type() == KAknUidValueEndKeyCloseEvent )
        {
        // Close the current active widget
        iWindowManager->CloseWindowsAsync(EFalse);
        return;
        }

    if ( aEvent.Type() == EEventUser )
        {
        // Fetch the event data of the user event. Content of the event data:
        // bits 0-31: Uid of the WidgetAppUi to check if the event should be
        // handled here.
        // bits 32-63: Uid of the widget that should be closed.
        TUint8* eventData = aEvent.EventData();
        // Check if first 32 bits of event data contains uid of WidgetAppUi
        if ( ( TUint32& )*eventData == KWidgetAppUid )
            {
            eventData += sizeof( TUint32 );
            // Fetch uid of the widget that should be to be closed.
            TInt32 uidInteger = ( TInt32& )*eventData;
            TUid widgetUid = TUid::Uid( uidInteger );

            // Close the running widget with uid widgetUid
            //iWindowManager->CloseWindow( widgetUid );
            iWindowManager->Exit( EEikCmdExit,widgetUid );

            TWsEvent event;
            event.SetType(EEventWindowGroupListChanged);
            event.SetTimeNow();
            iEikonEnv->WsSession().SendEventToWindowGroup(
                iEikonEnv->WsSession().GetFocusWindowGroup(), event);

            return;
            }
        }

    CAknViewAppUi::HandleWsEventL( aEvent, aDestination );
    }
    
// -----------------------------------------------------------------------------
// CWidgetUiAppUi::HandleForegroundEventL
// called when application goes to background or comes back to foreground.
//
// Although WindowManager::OpenOrCreateWindowL does handle opening a new window,
// we have to worry about "blank screen" / "resume" in power save operations.
// It is true that there is now overhead with HandleForegroundEventL( ETrue )
// and OpenOrCreateWindowL. The latter might need some checking, or e.g.
// blinking might occur (flip the old widget visible, then open a new one).
// -----------------------------------------------------------------------------
//
void CWidgetUiAppUi::HandleForegroundEventL( TBool aForeground )
    {
    iIsForeground = aForeground;

    if ( !iDelayedForegroundEvent->IsActive() )
        {
        iDelayedForegroundEvent->Start(TCallBack(HandleDelayedForegroundEventCallback,this));
        }
    }
    
// ----------------------------------------------------
// CWidgetUiAppUi::HandleApplicationSpecificEventL
// called when application specific events like OOM from window server are triggered
//
// ----------------------------------------------------
//
void CWidgetUiAppUi::HandleApplicationSpecificEventL(TInt aEventType, const TWsEvent& aWsEvent)
    {
    CAknAppUi::HandleApplicationSpecificEventL(aEventType, aWsEvent);

    if ( iWindowManager )
        {
        if(aEventType == KAppOomMonitor_FreeRam )
            {            
#ifdef OOM_WIDGET_CLOSEALL            
            CloseAllWidgetsAndExit();
#else if    // OOM_WIDGET_CLOSEALL        
            iWindowManager->HandleOOMEventL(iIsForeground);
            CloseAndExitIfNoneLeft();
#endif            
            }
        }
    }


// -----------------------------------------------------------------------------
// CWidgetUiAppUi::SendAppToBackground
// force the application to the background
//
// -----------------------------------------------------------------------------
//
void CWidgetUiAppUi::SendAppToBackground()
    {
    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TUid wapUid( TUid::Uid( KWidgetAppUid ) );
    TApaTask task = taskList.FindApp( wapUid );
    task.SendToBackground();
    }
    
// -----------------------------------------------------------------------------
// CWidgetUiAppUi::SetRotationSupport()
// check to see if screen oriantation can be changed
// this code must match Widget::Widget in WidgetEngine
//
// -----------------------------------------------------------------------------
//
void CWidgetUiAppUi::SetRotationSupport()
    {
    iRotationSupported = iAvkonAppUi->OrientationCanBeChanged();
    }

// -----------------------------------------------------------------------------
// CWidgetUiAppUi::SetDisplayLandscapeL()
// switch the display orientation to landscape
//
// -----------------------------------------------------------------------------
//
void CWidgetUiAppUi::SetDisplayLandscapeL( )
    {
    if (!iRotationSupported) return;

    if ( Orientation() == EAppUiOrientationPortrait )
        {
        SetOrientationL(EAppUiOrientationLandscape);
        }
    else if (Orientation() == EAppUiOrientationUnspecified)
        {
        TRect rect = ApplicationRect();
        TInt width = rect.Width();
        TInt height = rect.Height();

        if (width < height)
            {
            SetOrientationL(EAppUiOrientationLandscape);
            }
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiAppUi::SetDisplayPortraitL()
// switch the display orientation to portrait
//
// -----------------------------------------------------------------------------
//
void CWidgetUiAppUi::SetDisplayPortraitL( )
    {
    if (!iRotationSupported) return;

    if ( Orientation() == EAppUiOrientationLandscape )
        {
        SetOrientationL(EAppUiOrientationPortrait);
        }
    else if (Orientation() == EAppUiOrientationUnspecified)
        {
        TRect rect = ApplicationRect();
        TInt width = rect.Width();
        TInt height = rect.Height();

        if (width > height)
            {
            SetOrientationL(EAppUiOrientationPortrait);
            }
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiAppUi::SetDisplayAuto()
// switch the display orientation to default device orientation
//
// -----------------------------------------------------------------------------
//
void CWidgetUiAppUi::SetDisplayAuto( )
    {
    TRAP_IGNORE(SetOrientationL(EAppUiOrientationAutomatic));
    }

// -----------------------------------------------------------------------------
// CWidgetUiAppUi::AsyncExit()
// start an async exit
//
// -----------------------------------------------------------------------------
//
void CWidgetUiAppUi::AsyncExit( TBool /*aAllWidgets*/ )
    {
    iWidgetUiAsyncExit->Start(); // close current and exits app if no widgets left running
    }

// -----------------------------------------------------------------------------
// CWidgetUiAppUi::CloseAndExitIfNoneLeft()
//
// -----------------------------------------------------------------------------
//
void CWidgetUiAppUi::CloseAndExitIfNoneLeft()
    {
    if(iWindowManager)
        {
        if(iWindowManager->CloseWindowWithLeastClick())  
            {
            //Exit Application
            NotifyCommandHandled();
            Exit();
            }            
        }
    }
// -----------------------------------------------------------------------------
// CWidgetUiAppUi::HandleDelayedForegroundEventCallback()
// CIdle callback function to handle delayed foreground events for WidgetUI
//
// -----------------------------------------------------------------------------
//
TInt CWidgetUiAppUi::HandleDelayedForegroundEventCallback( TAny* aAppUi )
    {
    return ((CWidgetUiAppUi*)aAppUi)->HandleDelayedForegroundEvent();
    }
    
// -----------------------------------------------------------------------------
// CWidgetUiAppUi::HandleDelayedForegroundEvent()
// function to handle delayed foreground events for WidgetUI
//
// -----------------------------------------------------------------------------
//
TInt CWidgetUiAppUi::HandleDelayedForegroundEvent( )
    {
    if (iWindowManager)
        {
        iWindowManager->HandleForegroundEvent(iIsForeground);
        }
    return (KErrNone);
    }

// ------------------------------------------------------------------------
// CWidgetUiWindowManager::RegisterWidgetL
//
// Register widget to CPS listener.
// ------------------------------------------------------------------------
void CWidgetUiAppUi::ProcessMessageArgumentsL( 
    const TDesC8& aLine,
    TUid& aUid,
    TUint32& aOperation )
    {
    __UHEAP_MARK;
    TUint32 version( -1 );
    TPtrC ptr( NULL, 0 );
    
    RDesReadStream stream( aLine );
    CleanupClosePushL( stream );
    
    version = stream.ReadUint32L();
    
    if ( version != 1 )
        {
        User::Leave( KErrNotSupported );
        }
    
    aUid.iUid = stream.ReadUint32L();
    aOperation = stream.ReadInt32L();
    
    CleanupStack::PopAndDestroy( &stream );
    __UHEAP_MARKEND;
    }
    
// -----------------------------------------------------------------------------
// CWidgetUiAppUi::ExitWidget()
// ApiProvider callback function to exit WidgetUI
//
// -----------------------------------------------------------------------------
//
void CWidgetUiAppUi::ExitWidget( )
    {
    if (iWindowManager)
        {
        if ( iWindowManager->ActiveWindow() )
            iWindowManager->Exit(EEikCmdExit, iWindowManager->ActiveWindow()->Uid());
        }
    }

void CWidgetUiAppUi::ProcessCommandL(TInt aCommand )
    {
        TBool doesLSKHandlerExist = EFalse;
    
        if ( aCommand == EAknSoftkeyOptions )
            {
                if ( iWindowManager)
                    {
                        doesLSKHandlerExist = iWindowManager->HandleLSKCommandL(aCommand);
                    }
            }
        
        if ( doesLSKHandlerExist == EFalse ) 
            {
                CAknViewAppUi::ProcessCommandL(aCommand);
            }
    }

#ifdef OOM_WIDGET_CLOSEALL
// -----------------------------------------------------------------------------
// CWidgetUiAppUi::CloseAllWidgetsAndExit()
// Close all widgets and exit in case of OOM
//
// -----------------------------------------------------------------------------
//
void CWidgetUiAppUi::CloseAllWidgetsAndExit()
{
    if(iWindowManager->CloseAllWidgetsUnderOOM())
        {
        //Exit Application
        NotifyCommandHandled();
        Exit();
        }
}

#endif

// End of File
