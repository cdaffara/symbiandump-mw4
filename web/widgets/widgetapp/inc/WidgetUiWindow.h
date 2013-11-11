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


#ifndef WIDGETUIWINDOW_H_
#define WIDGETUIWINDOW_H_

// INCLUDES FILES
#include <e32base.h>
#include <BrCtlDefs.h>
#include <brctlinterface.h> // for MWidgetCallback
#include <AknServerApp.h>
#include <coecntrl.h>
#include "WidgetUiObserver.h"

// CONSTANTS
enum TWidgetAccessGrant
    {
    EInvalid,
    EDeny,
    EAllow
    };
    
enum TMiniViewState
    {
    EMiniViewNotEnabled = 0,
    EMiniViewEnabled,
    EPublishStart,
    EPublishSuspend
    };

enum TNetworkState
    {
    ENetworkNotAllowed = 0,
    ENetworkAccessAllowed,
    ENetworkAccessible
    };

struct TWidgetState
    {
    TBool               iFullViewState; //  full view or not
    enum TMiniViewState iMiniViewState;
    };

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CWidgetUiWindowManager;
class CWidgetUiObserver;
class CWidgetUiDialogsProviderProxy;
class CPeriodic;
class CEikButtonGroupContainer;
class CCpsPublisher;
class CFbsBitmap;
class CSchemeHandler;
class CBrCtlInterface;
class CJpgSaver;
// CLASS DECLARATION

/**
*
*
*  @lib WidgetUi.app
*  @since Series 60 3.0
*/
class CWidgetUiWindow :  public CBase, 
                    public MWidgetCallback, 
                    public MBrCtlSpecialLoadObserver,
                    public MAknServerAppExitObserver
    {
    public:

        /**
        * OpenWindowL
        * @since 3.1
        * @param aWindowManager
        * @param aUid 
        * @param aCpsPublisher
        * @return CBrCtlInterface*
        */
        static CWidgetUiWindow* OpenWindowL( CWidgetUiWindowManager& aWindowManager,
                                           const TUid& aUid, CCpsPublisher* aCpsPublisher );

        /**
        * ~CWidgetUiWindow
        * Destructor
        * @since 3.1
        * @param none
        * @return none
        */
        virtual ~CWidgetUiWindow();

    public: // from MWidgetCallback

        /**
        * DialogMimeFileSelectLC
        * Navigates through your file system and selects a file of specific mime type;
        * analogous to the Browse command in Windows.
        * @since 3.1
        * @param aSelectedFileName The selected file name.
        * @param aMimeType The accepted mime type.
        * @return ETrue if the user selected a file
        * EFalse if the user cancelled the transaction and did not select a file.
        * @attention Returned on cleanup stack. Browser control will free the buffer.
        */
        TBool DialogMimeFileSelectLC(HBufC*& aSelectedFileName,
                                 const TDesC& aMimeType);

       /**
        * Called to show or hide softkeys from menu client callback
        *  -- sets a flag to reflect user preference 
        * @since 3.1
        * @param aVisible ETrue to show softkeys, EFalse when full screen is needed
        */
        void SetSoftkeysVisible(TBool aVisible);

        /**
        * Called to change the display orientation to landscape
        * @since 3.1
        */
        void SetDisplayMode(TBrCtlDefs::TBrCtlOrientation aOrientation);

        virtual TInt Reserved_1(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/) { return 0; }
        virtual TInt Reserved_2(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/) { return 0; }
        virtual TInt Reserved_3(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/) { return 0; }
        virtual TInt Reserved_4(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/) { return 0; }
        virtual TInt Reserved_5(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/) { return 0; }
        virtual TInt Reserved_6(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/) { return 0; }
        virtual TInt Reserved_7(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/) { return 0; }
        virtual TInt Reserved_8(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/) { return 0; }
        virtual TInt Reserved_9(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/) { return 0; }
        virtual TInt Reserved_10(TAny*& /*a0*/, TAny* /*a1*/, TAny* /*a2*/) { return 0; }
    public:  // MBrCtlSpecialLoadObserver
        /**
       * NetworkConnectionNeededL
       * Request to create a network connection.
       * @since 3.1
       * @param aConnectionPtr A pointer to the new connection. If NULL, the proxy filter will automatically create a network connection
       * @param aSockSvrHandle A handle to the socket server.
       * @param aNewConn A flag if a new connection was created. If the connection is not new, proxy filter optimization will not read the proxy again from CommsBd
       * @param aBearerType The bearer type of the new connection
       * @return void
       */
        void NetworkConnectionNeededL( TInt* aConnectionPtr,
                                           TInt* aSockSvrHandle,
                                           TBool* aNewConn,
                                           TApBearerType* aBearerType ) ;

        /**
       * ConnectionManagement
       * Setup connection based on browser settings
       * @since 
       * @return void
       */
        void ConnectionManagement();

        /**
       * StartNetworkConnectionL
       * Start new connection
       * @param aNewConn A flag if a new connection was created. If the connection is not new, proxy filter optimization will not read the proxy again from CommsBd
       * @since 
       * @return void
       */
        void StartNetworkConnectionL(TBool* aNewConn);

        /**
       * HandleRequestL
       * Request the host applicaion to handle non-http request.
       * @since 3.1
       * @param aTypeArray array of download parameter types
       * @param aDesArray array of values associated with the types in the type array
       * @return ETrue is handled by the host application. EFlase if not
       */
        TBool HandleRequestL( RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray );

        TBool HandleDownloadL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray);

        /**
       * DoHandleRequestL
       * Request the host applicaion to handle non-http request.
       * @since 3.1
       * @param aTypeArray array of download parameter types
       * @param aDesArray array of values associated with the types in the type array
       * @return ETrue is handled by the host application. EFlase if not
       */
        TBool DoHandleRequestL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray );

        /**
       * Extract the given BrCtl parameter from the list.
       * @param aParamTypeToFind Extract this parameter.
       * @param aTypeArray array of download parameter types.
       * @param aDesArray array of values associated with the types in the type array.
       * @param aParamFound Output: ETrue if the parameter was found.
       * @return A TPtrC pointer for the value.
       * @since 2.8
       */
        TPtrC ExtractBrCtlParam( TUint aParamTypeToFind,
                          RArray<TUint>* aTypeArray,
                          CDesCArrayFlat* aDesArray,
                          TBool& aParamFound ) const;

       /**
       * Make generic parameter list from BrCtl parameter list.
       * @param aTypeArray array of download parameter types
       * @param aDesArray array of values associated with the types in the type array
       * @return The dynamically allocated list.
       */
        CAiwGenericParamList* BrCtlParamList2GenericParamListL( 
                                             RArray<TUint>* aTypeArray, 
                                             CDesCArrayFlat* aDesArray ) const; 
		
    public: //MAknServerAppExitObserver

        void HandleServerAppExit( TInt aReason );


    public:  // new functions

        /**
        * Engine
        * @since 3.1
        * @param none
        * @return CBrCtlInterface*
        */
        void Done();
        CBrCtlInterface* Engine() const { return iEngine; }

        /**
        * CbaGroup
        * @since 3.1
        * @param none
        * @return CEikButtonGroupContainer*
        */
        CEikButtonGroupContainer* CbaGroup();


        /**
        * WidgetExtension
        * @since 3.1
        * @param none
        * @return MWidgetExtension*
        */
        MWidgetExtension* WidgetExtension() const { return iWidgetExtension; }

        /**
        * ReloadWidget
        * Reload the current widget main html page.
        * @3.1
        * @return void
        */
        void ReloadWidget();

        /**
        * Url
        * Return the  main html page url of current widget.
        * @3.1
        * @return pointer to Url but does not transfer ownership
        */
       HBufC* Url() { return iUrl; }

        /**
        * Path
        * Return the path of current widget.
        * @3.1
        * @return pointer to WidgetPath but does not transfer ownership
        */
       HBufC* WidgetPath() { return iWidgetPath; }

        /**
        * Path
        * Return the bundle id of current widget.
        * @3.1
        * @return pointer to WidgetByndleId but does not transfer ownership
        */
       HBufC* WidgetBundleId() { return iWidgetBundleId; }

       /**
        * UId
        * Return the UId of current widget.
        * @3.1
        * @return UId
        */
       TUid Uid() const { return iUid; }

       /**
        * NetworkAccessGrant
        * Return network access grant.
        * @since 3.1
        * @return network access grant
        */
       TWidgetAccessGrant NetworkAccessGrant() const { return iNetworkAccessGrant; }

       /**
        * SetNetworkAccessGrant
        * Set network access grant.
        * @since 3.1
        * @return void
        */
       void SetNetworkAccessGrant( TWidgetAccessGrant aGrant )
         { iNetworkAccessGrant = aGrant; }

        /**
        * Called to change the label of the right or left softkey
        * @since 3.1
        * @param aKeySoftkey softkey top change
        * @param aText label for right softkey
        */
        void SetSoftKeyLabelL( TBrCtlKeySoftkey aKeySoftkey, const TDesC& aText);

       /**
        * Relayout
        * relayout display to preferred orientation
        * @since 3.1
        * @return void
        */
       void Relayout( );

       /**
        * SetCurrentWindow
        * sets the current window active or non-current window inactive
        * @since 3.1
        * @return void
        */
       void SetCurrentWindow( TBool aCurrent );
       
       /**
        * WindowManager
        * returns pointer to window manager
        * @since 3.1
        * @return CWidgetUiWindowManager*
        */
       CWidgetUiWindowManager& WindowManager( ) { return iWindowManager; }

       /**
        * SetWidgetLoaded
        * notification that widget has finished loading
        * @since 5.0
        * @return none
        */
        void SetWidgetLoaded( TBool aWidgetLoaded );
        
        /**
        * IsWidgetLoaded
        * Return ETrue if widget has finished loading, else EFalse
        * @since 5.0
        * @return none
        */
        TBool IsWidgetLoaded( ) { return iWidgetLoaded; }
        
        /**
        * GetBundleName
        * Gets the Bundle name for Uid
        * @since 5.0
        * @return none
        */
        void GetBundleName(TPtr& aBundleName );

         /**
        * PublishSnapShot
        * Publishes offscreen bitmap to CPS
        * @since 5.0
        * @return none
        */
        void PublishSnapShot();

        /**
        * HasMiniviewL
        * Checks registry value whethre miniview is supporetd or not
        * @since 5.0
        * @return TBool
        */
        TBool HasMiniviewL();

        /**
        * WidgetMiniViewState
        * returns miniview state
        * @since 5.0
        * @return TMiniViewState
        */
        TMiniViewState WidgetMiniViewState() const;

        /**
        * SetWindowStateFullView
        * Sets	widget is started in full view or not
        * @since 5.0
        * @return none
        */
        void SetWindowStateFullView( TBool aWidgetFullViewState);
		
        /**
        * SetWindowStateMiniView
        * Sets	miniview state
        * @since 5.0
        * @return none
        */
        void SetWindowStateMiniViewL( TMiniViewState aWidgetMiniViewState);

        /**
        * WidgetFullViewState
        * Get whhether widget is started in fullView or not
        * @since 5.0
        * @return none
        */
        TBool WidgetFullViewState() const;

        /**
        * SetIsCurrentWindow
        * Sets IsCurrent window so that when window is launched next time
        * foreground background set is done properly
        * @since 5.0
        * @return none
        */
        void SetIsCurrentWindow(TBool aIsCurrent){ iIsCurrent = aIsCurrent;} 
        /**
        * GetClickCount
        * Get the count for widget being selected from homescreen
        * @since 5.0
        * @return TInt
        */              
        TInt GetClickCount(){ return iClickCount;}  
                
        /**
        * IncrementClickCount
        * Increase the click count for widgets being selected from homescreen
        * @since 5.0
        * @return none
        */              
        void IncrementClickCount(){ ++iClickCount;} 
        
        /**
        * SetTime
        * Set the time when widget is ReStarted after OOM
        * @since 5.0
        * @return none
        */
        void SetTime(){iOOMWidgetStartTime.HomeTime();}

        /**
        * GetTime when widget was started after OOM 
        * @since 5.0
        * @return TTime
        */        
        TTime GetTime(){return iOOMWidgetStartTime;}
        
       /**
        * IsWidgetLoadStarted 
        * @since 5.0
        * @return TBool
        */
        TBool IsWidgetLoadStarted(){return iWidgetLoadStarted;}
        
        /**
        * CheckUserPermissionChanged
        * Checks if the user permission to access network has changed
        * @since 5.0
        * @return none
        */              
        void CheckUserPermissionChanged(TBool iCurrUserPerm);
        
        /**
        * DetermineNetworkState
        * Determine the current network state
        * @since 5.0
        * @return none
        */              
        void DetermineNetworkState();
        
        
        /**
         * MakeSoftkeysVisible
         * Called to show or hide softkeys 
         * @since Browser 7.1
         * @param aVisible ETrue to show softkeys, EFalse when full screen is needed
         * @param aTextBoxUpdate ETrue when called due to textbox update (via updatestatuspane), else EFalse
         */
        void MakeSoftkeysVisible(TBool aVisible, TBool aTextBoxUpdate);
        
        /**
         * NetworkModeWait
         * returns pointer to NetworkMode wait object
         * @since 7.1
         * @return CActiveSchedulerWait*
         */
        CActiveSchedulerWait* NetworkModeWait() { return iNetworkModeWait; }

        
    protected:

        /**
        * CWidgetUiWindow
        * Construtor
        * @since 3.1
        * @param aWindowManger
        * @param aCpsPublisher
        * @return none
        */
        CWidgetUiWindow( CWidgetUiWindowManager& aWindowManager, CCpsPublisher* aCpsPublisher );

        /**
        * ConstructL
        * EPOC default constructor.
        * By default Symbian 2nd phase constructor is private.
        * @since 3.1
        * @param none
        * @return void
        */
        void ConstructL( const TUid& aUid );

    private:
        /**
        * UpdateCba
        * @since 3.1
        * @return none
        */
        void UpdateCba( );
        
        /**
        * SetSoftKeyImageL
        * @since 3.1
        * @param aCommand
        * @param aFilename
        * @return none
        */
        void SetSoftKeyImageL(const TInt aCommand, const TDesC& aFilename);
        
        /**
         * Constructs the publisher identifier
         * @since 5.1
         */
        HBufC* WidgetIdAndNameLC();
        
        /**
        * NetworkSecurityCheckL
        * Prompts for network access
        * @since 
        * @return void
        */
        void NetworkSecurityCheckL();

        /**
        * CheckNetworkAccessL
        * Checks widget registry values for whether widget is allowed network access
        * @since 
        * @return TBool
        */
        TBool CheckNetworkAccessL();
       
    private:
        CWidgetUiObserver*              iWidgetUiObserver;  // owned, responsible for deleting
        CWidgetUiWindowManager&         iWindowManager;
        
        CCpsPublisher*                  iCpsPublisher; // Interface to publish bitmap
        CWidgetUiDialogsProviderProxy*  iWidgetUiDialogsProviderProxy; // owned, responsible for deleting
        HBufC*                          iUrl;               // owned, responsible for deleting
        CBrCtlInterface*                iEngine;            // owned, responsible for deleting
        MWidgetExtension*               iWidgetExtension;
        CSchemeHandler*                 iSchemeHandler;     // Own. handles links to mailto etc
        TUid                            iUid;
        TWidgetAccessGrant              iNetworkAccessGrant;
        HBufC*                          iWidgetPath;        // owned, responsible for deleting
        HBufC*                          iWidgetBundleId;    // owned, responsible for deleting

        TBrCtlDefs::TBrCtlOrientation   iPreferredOrientation;
        TBool                           iIsCurrent;
        TBool                           iShowSoftkeys;
        HBufC*                          iRightSoftKeyLabel; // owned- copy of latest right softkey label
        HBufC*                          iLeftSoftKeyLabel; // owned- copy of latest left softkey label
        TBool                           iWidgetLoaded;
        TBool                           iPenEnabled; // For touch
        TInt                            iCount;
        TWidgetState                    iWidgetWindowState;
        TBool                           iSchemeProcessing;  
        TNetworkState                   iNetworkState;
        TBool                           iUserPermission;    // ETrue if user chooses to allow network access when prompted, else EFalse
        
        //Download transaction ID
        long                            iDlId;  
		TInt                            iClickCount;
        CFbsBitmap*                     iMiniviewBitmap ;
        TTime                           iOOMWidgetStartTime; 
        TBool                           iWidgetLoadStarted; // Set to true when widget load starts
        CJpgSaver*                       iJpgSaver; 
        CActiveSchedulerWait*           iNetworkModeWait;
                            
   };

#endif  //

// End of file
