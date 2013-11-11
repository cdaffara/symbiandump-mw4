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
* Description:  Manages widget windows, each window display one widget
*
*/


#ifndef WIDGETUIWINDOWMANAGER_H_
#define WIDGETUIWINDOWMANAGER_H_

// INCLUDES FILES

#include <WidgetRegistryClient.h>
#include <AknGlobalConfirmationQuery.h>
#include <coecntrl.h>
#include "WidgetUiObserver.h"
#include "Browser_platform_variant.hrh"
// CONSTANTS

enum TNetworkMode
    {
    EUnknownMode = 0,
    EOnlineMode,
    EOfflineMode
    };

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDocumentHandler;
class CWidgetUiWindowView;
class CWidgetUiWindow;
class CWidgetUiAppUi;
class CWidgetUiCpsObserver;
class CAknNoteDialog;
class CBrowserDialogsProvider;
class CBrCtlInterface;
class CLiwServiceHandler;
class MLiwInterface;
class CInternetConnectionManager;
class CSchemeHandler;
class CWidgetUiNetworkListener;

#ifdef BRDO_WRT_HS_FF
class CCpsPublisher;
#endif
// CLASS DECLARATION

/**
*  CWidgetUiWindowManager
*  @lib WidgetUi.app
*  @since 3.1
*/
class CWidgetUiWindowManager : public CBase
    {
    public:  // constructors / destructor

        /**
        * NewL
        * Two-phased constructor.
        * @since 3.1
        * @param none
        * @return CWidgetUiWindowManager*
        */
        static CWidgetUiWindowManager* NewL( CWidgetUiAppUi& aAppUi );

        /**
        * ~CWidgetUiWindowManger
        * Destructor
        * @since 3.1
        * @param none
        * @return none
        */
        virtual ~CWidgetUiWindowManager();

    public:  

// Create window
        
        /**
        * HandleWidgetCommandL
        * @since 5.0
        * @param aUid UID of widget.
        * @param aOperation Whether to open or close the widget, and how.
        */
        void HandleWidgetCommandL( 
            const TUid& aUid,
            TUint32 aOperation );
        
        /**
        * Resume a suspended widget.
        * @since 5.0
        * @param aUid UID of widget.
        */
        void ResumeWidgetL( const TUid& aUid );

        /**
        * CloseWindowsAsync
        * Close window async
        * @param aAllWindows ETrue closes all windows; EFalse closes active window
        * @since 3.1
        */
        void CloseWindowsAsync( TBool aAllWindows );
        
        /**
        * CloseAllWindowsExceptCurrent
        * Closes all of the window except the current widget
        * @since 3.1
        * @return none
        */            
        void CloseAllWindowsExceptCurrent();

        /**
        * CheckIfWindowNeedsToBeClosed
        * Returns false only for current window
        * @since 5.0
        * @return true if widget needs to be closed
        */ 
        TBool CheckIfWindowNeedsToBeClosed(CWidgetUiWindow* aWindow) const;
        
        /**
        * Exit the widget, if the widget is the only widget running, exit the application
        * otherwise, close the widget window and activate the next widget window who is running
        * @since 3.1
        */
        void Exit( TInt aCommand , const TUid& aUid);

        /**
        * SetActiveWidgetAtExit
        * Sets the FullView window which is not publishing to active
        * @since 5.0
        */ 
        void SetActiveWidgetAtExit();

// Query/set states.

        /**
        * SetDisplayLandscape
        * Called to change the display orientation to landscape
        * @since 3.1
        */
        void SetDisplayLandscape( );
        
        /**
        * GetWidgetEntryForUrl
        * Returns the UId of widget entry with a particular Url
        * @since 3.1
        * @param none
        * @return UId
        */
        TInt GetWidgetEntryForUrl( const TDesC& aUrl ); //const;
        
        /**
        * ActiveWindow
        * @since 3.1
        * @param none
        * @return CWidgetUiWindow*
        */
        inline CWidgetUiWindow* ActiveWindow() const { return iActiveFsWindow; }

        /**
        * GetWindow
        * Gets the window for certain widget identified by a URL
        * @since 3.1
        * @param aUrl - a URL to identify the widget
        * @return the widget running, null if not found
        */
        CWidgetUiWindow* GetWindow( const TDesC& aUrl );

        /**
        * GetWindow
        * Gets the window for certain widget identified by a Uid
        * @since 3.1
        * @param aUid - Uid to identify the widget
        * @return the widget running, null if not found
        */
        CWidgetUiWindow* GetWindow( const TUid& aUid );

        /**
        * SetView
        * @since 3.1
        * @param aContentView
        * @return void
        */
        void SetView( CWidgetUiWindowView& aContentView ) { iContentView = &aContentView; }

        /**
        * View
        * @since 3.1
        * @param none
        * @return CWidgetUiWindowView*
        */
        CWidgetUiWindowView* View() const { return iContentView; }
     
        /**
        * SetDisplayPortrait
        * Called to change the display orientation to portrait
        * @since 3.1
        */
        void SetDisplayPortrait( );
        
        /**
        * SetDisplayAuto
        * Called to change the display orientation to device defined
        * @since 3.1
        */
        void SetDisplayAuto( );

        /**
        * HandleForegroundEvent
        * Called when application goes to background or comes back to foreground
        * @since 3.1
        */
        void HandleForegroundEvent( TBool aForeground );

        /**
        * HandleOOMEventL
        * Called when out of memory message is received by app ui
        * @since 3.1
        */
        void HandleOOMEventL( TBool aForeground );

// Utility stuff.

        /**
        * WindowList
        * Gets the list of running windows
        * @since 3.1
        * @param none
        * @return CArrayPtrFlat<CWidgetUiWindow>*
        */
        void WindowList( RPointerArray<CWidgetUiWindow>& )  { /*return iWindowList; */ User::Invariant(); }

        /**
        * WidgetUIClientSession
        * @since 3.1
        * @param none
        * @return RWidgetUIRegistryClientSession*
        */
        RWidgetRegistryClientSession& WidgetUIClientSession() { return iClientSession; }

        /**
        * DialogsProvider
        * @since 3.1
        * @param none
        * @return CBrowserDialogsProvider
        */
        CBrowserDialogsProvider* DialogsProvider() { return iDialogsProvider; }
        
        /**
         * HandleLSKCommandL
         * @since 5.0
         * @param TInt - command to execute
         * @return TBool - True if LSK handler is executed
         */
        TBool HandleLSKCommandL( TInt aCommand );
        
        /**
        * DocHandler
        * @since 5.0
        * @return Document Handler.
        */
        inline CDocumentHandler& DocHandler() { return *iHandler; }

        /**
        * GetConnection
        * @since 5.0
        * @return CInternetConnectionManager
        */
        CInternetConnectionManager* GetConnection() {return iConnection;}

        /**
        * GetDb
        * @since 5.0
        * @return CActiveApDb
        */
        CActiveApDb*  GetDb() {return iDb;}
        
        /**
         * GetNetworkMode
         * @since 5.0
         * @returns online/offline mode
         */
        TNetworkMode GetNetworkMode() {return iNetworkMode;}
        
        /**
         * GetNetworkConn
         * @since 5.0
         * @returns ETrue if there is an active network connection
         */
        TBool GetNetworkConn() {return iNetworkConnected;}
        
        /**
         * GetCenrepHSModeL
         * @since 5.0
         * @returns online/offline mode read from the cenrep
         */
        TInt GetCenrepHSModeL();
        
        /**
         * FullViewWidgetsOpen
         * @since 5.0
         * @returns ETrue if atleast one full view widget is open
         */
        TBool FullViewWidgetsOpen();
		
        /**
        * CloseWindowWithLeastClick
        * Removes widget window with least click count
        * @since 5.0
        * @return TBool
        */
        TBool CloseWindowWithLeastClick();  
        
        /**
        * DeleteOOMNotifyTimer
        * @since 5.0
        * @return void
        */
        void  DeleteOOMNotifyTimer();

	    /**
        * OpenOrCreateWindowL
        * @since 5.0
        * @param aUid UID of widget.
        * @param aOperation Whether to open widget to miniview or full screen.
        */
        void OpenOrCreateWindowL( 
            const TUid& aUid,
            TUint32 aOperation );     
        
        /**
        * CanWindowBeCreated
        * Checks for available RAM before creating window
        * @since 5.0
        * @return none
        */
        void CanWindowBeCreated(const TUid& aUid, TUint32 aOperation);  	
        
        /**
        * StartHarvesterNotifyTimer
        * @since 5.0
        * @return void
        */        
        void StartHarvesterNotifyTimer();	
        
        /**
        * DeleteHarvesterNotifyTimer
        * @since 5.0
        * @return void
        */        
        void DeleteHarvesterNotifyTimer();
        
        /**
        * SendAppToBackground
        * Send widget ui to background
        * @since 5.0
        * @return void
        */        
        void SendAppToBackground();
        
        /**
        * NotifyConnecionChange
        * Notify widgets of a network connection change
        * @since 5.0
        * @return void
        */        
        void NotifyConnecionChange(TBool aConn);
        TBrCtlDefs::TCursorSettings CursorShowMode() {return iWidgetCursorMode;}
        
        TBrCtlDefs::TEnterKeySettings  EnterKeyMode() {return iWidgetEnterKeyMode;}

        /**
        * AnyWidgetOnHs
        * Checks to see if any widget is on HS
        * @since 5.0
        * @return TBool
        */		
		TBool AnyWidgetOnHs();
		
        /**
        * AnyWidgetOnHs
        * Checks to see if any widget is Publishing on HS
        * @since 5.0
        * @return TBool
        */  		
		TBool AnyWidgetPublishing();
        
        
#ifdef  OOM_WIDGET_CLOSEALL
        /**
        * CloseAllWidgetsUnderOOM
        * @param none
        * @return ETrue if all widgets were closed else EFalse
        */        
        TBool CloseAllWidgetsUnderOOM();
        
        /**
        * GetLastWidgetRestartTime
        * @param none
        * @return Time when last widget was created after OOM
        */        
        TTime GetLastWidgetRestartTime(){return iTimeLastWidgetOpen; }

        /**
        * SetLastWidgetRestartTime
        * The time when last widget was restarted in case of OOM
        * @param aStartTime 
        * @return none
        */        
        void SetLastWidgetRestartTime(TTime aStartTime){iTimeLastWidgetOpen = aStartTime;}
#endif  // OOM_WIDGET_CLOSEALL
           /**  
           * CloseAllWidgets  
           * Basic pre-exit routine to make sure all widgets are closed out  
           * @param none  
           * @return none  
           */          
           void CloseAllWidgets(); 
    protected:

        /**
        * CWidgetUiWindowManager
        * Constructor
        * @since 3.1
        * @param none
        * @return none
        */
        CWidgetUiWindowManager( CWidgetUiAppUi& aAppUi );

        /**
        * ConstructL
        * EPOC default constructor
        * @since 3.1
        * @param none
        * @return void
        */
        void ConstructL();

    private:
        
        /**
        * Suspend a widget.
        * @since 5.0
        * @param aUid UID of widget.
        */
        void SuspendWidget( const TUid& aUid );
        
        /**
        * Suspend all the publishing widget.
        * @since 5.0
        * @return none.
        */
        void SuspendAllWidget();

        /**
        * Stop mini view and exit
        * @since 5.0
        * @param aUrl
        * @return ETrue if last window was closed
        */
        TBool DeactivateMiniViewL( const TUid& aUid );
 
        /**
        * Suspend publishing on home screen
        * @since 5.0
        * @param aUid UID of widget.
        * @param aSize Size of miniview.
        * @return Successful or not.
        */
        TBool ActivateMiniViewL( const TUid& aUid, const TRect& aRect );

// Close / terminate widgets.

        /**
        * CloseWindow
        * Closes the window of the widget
        * @since 3.1
        * @param aWidgetWindow - window of the widget
        * @return ETrue if last window was closed
        */
        TBool CloseWindow( CWidgetUiWindow* aWidgetWindow );
        
        /**
        * RemoveFromWindowList
        * Remove  the widget window from window list
        * @since 5.0
        * @param aWidgetWindow - window of the widget
        * @return ETrue if last window was closed
        */
        TBool RemoveFromWindowList( CWidgetUiWindow* aWidgetWindow );
        
        /**
        * Hide the window.
        * @param aWindow Window. Ownership is retained.
        */
        void HideWindow( CWidgetUiWindow* aWindow );
        
        /** 
        * Is the widget allowed to run or not
        *
        * @param aUid out-param, contains the UID of widget aon return.
        */
        TBool IsWidgetAllowedL( const TUid& aUid );// const;
            
        /**
        * Notify user if widget is not allowed to run.
        *
        * @param aResourceID String identifier.
        */        
        void ShowNoteDialogL( TInt aResourceID ) const;
            
        /**
        * Create new window for a widget and add it to window list.
        *
        * @param aUid UID
        * @return Newly created window.
        */
        CWidgetUiWindow* CreateNewWidgetWindowL( const TUid& aUid );
        
        /**
        * Check if widget is supposed to support miniviews.
        *
        * @param aUid UID.
        * @return Whether widgets are supported or not.
        */
        TBool DoesWidgetSupportMiniviewL( const TUid& aUid );

            
        /**
        * Set the given window as "active full-screen window".
        * The method handles swapping of active window.
        * @since 5.0
        * @param aWindow Window. Onwership is retained.
        */
        void ShowWindow( CWidgetUiWindow* aWindow );
            
        /**
        * SendWidgetToBackground
        * @since 5.0
        * @param aUid
        * @return none
        */
        void SendWidgetToBackground( const TUid& aUid );

    private:

        CWidgetUiWindow*                    iActiveFsWindow;    // reference.
        CWidgetUiWindowView*                iContentView;   // not owned, not responsible for deleting
        RPointerArray<CWidgetUiWindow>      iWindowList;    // owned, responsible for deleting
        CWidgetUiAppUi&                     iAppUi;          // not owned, not responsible for deleting.
        CWidgetUiNetworkListener*           iNetworkListener;   // owned, responsible for deleting
        CDocumentHandler*                   iHandler;           // own        
        RWidgetRegistryClientSession        iClientSession;     // owned
        TBool                               iServerConnected;   // connected to Widget Registry server ?
        TBool                               iStrictMode;
        CBrowserDialogsProvider*            iDialogsProvider;// owned, responsible for deleting
        CInternetConnectionManager*         iConnection;        // owned, responsible for deleting
        TNetworkMode                        iNetworkMode;       // unknown mode =  0, online mode = 1, offline mode = 2
        TBool                               iNetworkConnected;  // ETrue if there is an active network connection, else EFalse
        TBrCtlDefs::TCursorSettings                     iWidgetCursorMode;
        TBrCtlDefs::TEnterKeySettings       iWidgetEnterKeyMode;
        
#ifdef BRDO_WRT_HS_FF       
        CCpsPublisher*                      iCpsPublisher;      // Owned, interface to publish bitmap to CPS
#endif
        // TODO should this be created only when needed?
        CActiveApDb*                        iDb;                // owned, responsible for deleting        
        CPeriodic*                          iNotifyOOMFlagTimer;//Timer for one min timeout before next oom note is shown
        CPeriodic*                          iNotifyHarvester;//Notify harvester to send next event
#ifdef  OOM_WIDGET_CLOSEALL
        TTime                               iTimeLastWidgetOpen;
#endif         
    };

#endif  // WIDGETUIWINDOWMANAGER_H_

// End of File
