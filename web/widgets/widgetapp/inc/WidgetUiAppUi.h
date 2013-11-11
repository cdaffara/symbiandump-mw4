/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef WIDGETUIAPPUI_H_
#define WIDGETUIAPPUI_H_

//  INCLUDES

#include <eikapp.h>
#include <eikdoc.h>
#include <coeccntx.h>
#include <aknViewAppUi.h>
#include "ApiProvider.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CAknView;
class CWidgetUiWindowManager;
class CWidgetUiAsyncExit;
class CIdle;

// CLASS DECLARATION

/**
*  CWidgetUiAppUi
*  @lib
*  @since 3.1
*/
class CWidgetUiAppUi : public CAknViewAppUi, MApiProvider
    {
    public: // // Constructors and destructor

        /**
        * Default constructor.
        * @since 3.1
        */
        CWidgetUiAppUi();
  

        /**
        * ~CWidgetUiAppUi
        * Destructor
        * @since 3.1
        * @param none
        * @return none
        */
        virtual ~CWidgetUiAppUi();

        /**
        * ProcessCommandParametersL
        * @since 3.1
        * @param CApaCommandLine
        * @return none
        */
        TBool ProcessCommandParametersL(CApaCommandLine &aCommandLine);

        /**
        * HandleCommandL
        * @since 3.1
        * @param aCommand id
        * @return void
        */
        void HandleCommandL( TInt aCommand );

        /**
        * HandleResourceChangeL
        * Handles screen resolution changes
        * @since 3.1
        * @param aType
        * @return void
        */
        void HandleResourceChangeL( TInt aType );
        
        /**
        * SendAppToBackground
        * Send widget ui to background
        * @since 3.1
        * @return void
        */
        void SendAppToBackground();
        
        /**
        * SetDisplayLandscapeL
        * Called to change the display orientation to landscape
        * @since 3.1
        */
        void SetDisplayLandscapeL( );

        /**
        * SetDisplayPortraitL
        * Called to change the display orientation to portrait
        * @since 3.1
        */
        void SetDisplayPortraitL( );
        
        /**
        * SetDisplayAuto
        * Called to set the display orientation is automatic
        * @since 3.1
        */
        void SetDisplayAuto( );
        
        /**
        * AsyncExit
        * Start an Async exit
        * @param aAllWidgets - ETrue to exit all widgets; EFalse to exit active widget
        * @since 3.1
        */
        void AsyncExit( TBool aAllWidgets );
        
    public: // from CEikAppUi 
          
        /**
        * ConstructL
        * EPOC default constructor.
        * @since 3.1
        * @param none
        * @return void
        */
        void ConstructL();
        
    public: // From MApiProvider
    
        /**
        * Exit the widget, if the widget is the only widget running, exit the application
        * @since 3.1
        */
        void ExitWidget();
    
    public:
        /**
         * Callback to delay foreground event after new widget starts
         * @param TAny* - pointer to app ui
         * @return TInt - return error code
         * @since 3.1
         */
        static TInt HandleDelayedForegroundEventCallback( TAny* aAppUi );
        
        /**
         * Delays foreground event after new widget starts
         * @return TInt - return error code
         * @since 3.1
         */
        TInt HandleDelayedForegroundEvent();
        
        /**
         * Override ProcessCommandL() to handle LSK handler
         * @param TInt - user command
         * @since 5.0
         */
        void ProcessCommandL(TInt aCommand);
        
        /**
         * Closes widget Window and exits WidgetUI if no windowleft
         * @return void
         * @since 5.0
         */        
        
        void CloseAndExitIfNoneLeft();

    private: // From CEikAppUi

        /**
        * DynInitMenuPaneL
        * @since 3.1
        * @param aResourceId
        * @param aMenuPane
        * @return void
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * HandleMessageL
        * @since 3.1
        * @param aClientHandleOfTargetWindowGroup
        * @param aMessageUid
        * @param aMessageParameters
        * @return TMessageResponse
        */
       MCoeMessageObserver::TMessageResponse HandleMessageL( 
                        TUint32 aClientHandleOfTargetWindowGroup,
                        TUid aMessageUid, 
                        const TDesC8& aMessageParameters );

        /**
        * HandleWsEventL
        * @since 3.1
        * @param aEvent
        * @param aDestination
        * @return none
        */               
       void HandleWsEventL( const TWsEvent&, CCoeControl* );
       
        /**
        * From CAknViewAppUi, CEikAppUi
        * Handles changes in keyboard focus when an 
        * application switches to foreground
        * @param aForeground Is foreground event?
        */
        void HandleForegroundEventL( TBool aForeground );

		/**
        * Handles application specific events like OOM from window server.
        * @param aEventType The type of event.
        * @param aWsEvent window server event.
        * @return Key response.
        */
		void HandleApplicationSpecificEventL(TInt aEventType, const TWsEvent& aWsEvent);

    private: // new function

        /**
        * ActiveView
        * @since 3.1
        * @param none
        * @return CAknView*
        */
        CAknView* ActiveView();

        /**
        * LoadAdditionalResourcefile
        * @since 3.1
        * @param none
        * @return none
        */
        void LoadAdditionalResourcefile();
        
        /**
        * SetRotationSupport
        * @since S60 v5.0
        * @param none
        * @return none
        */
        void SetRotationSupport();
        
        /**
        * Process command line argument and fetch related information.
        * 
        * The method parses the given command line argument, and 
        * fetches the UID and miniview size. Values are stored in
        * out-parameters.
        * Miniview is a boolean value. 0 == normal launch, !0 == miniview.
        *
        * @param aLine Command line
        * @param aUid Application uid
        * @param aOperation What to do: launch or deactivate.
        */
        void ProcessMessageArgumentsL( 
            const TDesC8& aLine,
            TUid& aUid,
            TUint32& aOperation );

        /**
        * Launch window.
        * 
        * Launches full window or miniview window, as suggested in parameters.
        * @param aMessage Message to process.
        */
        void LaunchWindowL( const TDesC8& aParams );
            
#ifdef OOM_WIDGET_CLOSEALL
        /**
        * CloseAllWidgetsAndExit
        * @param none
        */        
        void CloseAllWidgetsAndExit();
#endif        
    private:
        // WindowManager to manage the list of running widget windows
        CWidgetUiWindowManager*       iWindowManager;
        // WidgetUiAsyncExit to handle async exiting of widgets
        CWidgetUiAsyncExit*           iWidgetUiAsyncExit;
        // delays foreground events till after new widget starts
        CIdle* iDelayedForegroundEvent;
        // Resource File
        TInt iResourceFile;
        
        TBool iIsForeground;
        TBool iRotationSupported;
    };

#endif // WIDGEUIAPPUI_H

// End of File
