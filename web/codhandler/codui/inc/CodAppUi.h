/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of class CCodAppUi.
*
*/


#ifndef COD_APP_UI_H
#define COD_APP_UI_H

// INCLUDES

#include <e32base.h>
#include <aknappui.h>
#include <AknServerApp.h>
#include <CodLoadObserver.h>
#include "CodWaitDialog.h"

// FORWARD DECLARATIONS

class CCodView;
class CCodDocument;
class CCodEng;
class CDocumentHandler;
class CRequestCompleteCallback;
class CErrorUI;
class CCodUiResource;

// CLASS DECLARATION

/**
* COD Application UI.
*/
NONSHARABLE_CLASS( CCodAppUi )
: public CAknAppUi,
  public MCodLoadObserver,
  public MCodDialogCommandObserver,
  public MAknServerAppExitObserver
    {

    public:     // Constructors and destructor

        /**
        * @param aCod COD or DD?
        * Constructor.
        */      
        CCodAppUi( TBool aCod );
        
        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        virtual ~CCodAppUi();

    public:     // new methods

        /**
        * Open file done.
        * @param aError Error code.
        */
        void OpenFileDone( TInt aError );

        /**
        * Set service flow settings.
        * @param aSilentMode Silent Mode.
        * @param aSuppressNextUrl Suppress Next-URL.
        * @param aSuppressLaunch Suppress launch.
        */
        void SetServiceFlow
            (
            TBool aSilentMode,
            TBool aSuppressNextUrl,
            TBool aSuppressLaunch
            );

    public:     // from CAknAppUi

        /**
        * Handle model change.
        */
        virtual void HandleModelChangeL();

        /**
        * Open a file.
        * @param aFileName File name to open.
        */
        virtual void OpenFileL( const TDesC& aFileName );

        /**
        * Handle command.
        * @param aCommand Command id.
        */
        virtual void HandleCommandL( TInt aCommand );

    protected:  // from CAknAppUi

        /**
        * Process command line parameters.
        * @param aCommand Command.
        * @param aDocumentName Document name.
        * @param aTail Don't know what; ignored.
        * @return EFalse.
        */
        virtual TBool ProcessCommandParametersL
            (
            TApaCommand aCommand,
            TFileName& aDocumentName,
            const TDesC8& aTail
            );

        /**
        * Handle key event.
        * @param aKeyEvent The key event.
        * @param aType Key event type.
        * @return Response (was the key event consumed?).
        */
        virtual TKeyResponse HandleKeyEventL
            ( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Handle resource change.
        * @param aType Resource type.
        */
        virtual void HandleResourceChangeL( TInt aType );

    private:    // from MCodLoadObserver

        /**
        * Get download root path.
        * Leave with KErrCancel if query is cancelled.
        * @param aRootPath Root path returned here.
        */
        virtual void GetRootPathL( TDes& aRootPath );

        /**
        * Starting load (fetch content or error-notify).
        * @param aStatusCode HTTP status code.
        * - 900 (Success) indicates start of content load.
        * - Other values indicate start of error report.
        */
        virtual void StartLoadL( TInt aStatusCode );

        /**
        * Cancelling the fetch started. Start wait note.
        */
        virtual void StartCancelL();

        /**
        * Load progress update.
        * @param aFinalValue Final progress value.
        * @param aCurrentValue Current progress value.
        */
        virtual void ProgressL( TInt aFinalValue, TInt aCurrentValue );

        /**
        * Processing (content load or error report) ended.
        * Note that if processing does not include loading, this method can be
        * called without a preceding StartLoadL call.
        * @param aStatusCode HTTP status code (aError mapped to HTTP status
        * code).
        * @param aError Error code of result. (The Set(), Accept() or Reject()
        * request to CodEng will also complete with this value.)
        */
        virtual void Done( TInt aStatusCode, TInt aError );

        /**
        * Get confirmation for creating network connection.
        * @return ETrue if connection can be created.
        */
        virtual TBool ConfirmConnectL();
        /**
        * Opening a connection.
        * @param aIap AP to be used for creating the connection.
        */
        virtual void StartConnect( TUint32 aIap );

        /**
        * Connected.
        */
        virtual void EndConnect();

        /**
        * Authenticate user.
        * @param aHost Host.
        * @param aRealm Realm.
        * @param aProxyAuth ETrue if this is a proxy authentication.
        * @param aUsername Return username here.
        * @param aPasssword Return password here.
        * @return ETrue if auth is OK, EFalse if cancelled.
        */
        virtual TBool UserAuthL
            (
            const TDesC& aHost,
            const TDesC& aRealm,
            TBool aProxyAuth,
            TDes& aUsername,
            TDes& aPassword
            );

        /**
        * ROAP Trigger parsed (success or error).
        * @param aData ROAP Trigger data.
        */
        virtual void RoapTriggerParsedL( const CRoapData& aData );

        /**
        * Connection Error
        * return: Connection Error .
        */
		inline virtual void SetConnError( TInt aError) {iConnError = aError;}

        /**
        * Connection Error
        * return: Connection Error .
        */
		inline virtual TInt ConnError () {return iConnError;}

        /**
        * Progresive download "Play" is available
        * return: Connection Error .
        */
		virtual void PdPlayAvailable();
        
        /**
        * Set Active Download
        */
        inline virtual void SetActiveDownload( ){}
       
        /**
        * Displays Info message "Waiting for license" 
        */
		virtual void WaitForLicenseL();

        /**
        * To indicate the download is paused
        */
        inline virtual void DownloadPaused() {}

        /**
        * To indicate the download is resumed and ready for PD
        */
        inline virtual void DownloadResumedPdAvailable() {}
        
        /**
        * To update the server about whether or not download is pausable
        */
        inline virtual void UpdatePausable( TBool /*aPausable*/ ) {}
        
        /**
        * To update the server about next media object being downloaded.
        */        
        inline virtual void MediaObjectNameChanged() {}
        
        /**
        * To update the server about next media object being downloaded.
        */        
        inline virtual void ContentTypeChanged() {}
                        
        /**
        * To update the server about updated media info
        */
        inline virtual void UpdateMediaInfoL() {}
        
        /**
        * To update the server about updated media info
        */
        inline virtual void UpdateTrackInfoL() {}
        
        /**
        * To handle PostResponseUrlL as part of metering response
		*/	        
        inline virtual void HandleRoapPostResponseUrlL( const TDesC8& /*aPrUrl*/ ) {}
        
                        
    private:    // from MCodDialogCommandObserver

        /**
        * Handle dialog command (from wait note).
        * @param aCommand Dialog command id.
        */
        void HandleDialogCommandL( TInt aCommand );

    private:    // from MAknServerAppExitObserver

        /**
        * Embedded document viewing has ended.
        * @param aReason Exit reason (unused).
        */
        void HandleServerAppExit( TInt aReason );

    private:    // new methods

        /**
        * Open file done.
        */
        void OpenFileDoneL();

        /**
        * Static wrapper around LoadDone (for TCallBack use).
        * @param aPtr Pointer passed to the callback (this object as TAny*).
        * @return EFalse (as TInt).
        */
        static TInt StaticLoadDone( TAny* aPtr );

        /**
        * Load done, follow-up processing and exit.
        */
        void LoadDone();

        /**
        * Load done, follow-up processing and exit.
        */
        void LoadDoneL();

        /**
        * Handle successful load.
        * @return ETrue if application should exit.
        */
        TBool LoadOkL();

        /**
        * Handle failed load.
        * @return ETrue if application should exit.
        */
        TBool LoadFailedL();

        /**
        * Handle successful ROAP.
        * @return ETrue if application should exit.
        */
        TBool RoapOkL();

        /**
        * Handle successful RoAcq ROAP.
        * @return ETrue if application should exit.
        */
        TBool RoAcqOkL();

        /**
        * Handle failed ROAP.
        * @return ETrue if application should exit.
        */
        TBool RoapFailedL();

        /**
        * Launch content.
        * @param aFname File name.
        * @param aType MIME type.
        * @return ETrue if application should exit.
        */
        TBool LaunchContentL( const TDesC& aFname, const TDesC8& aType );
        
        /**
        * Service flow: launch browser and (optionally) activate Next-URL,
        * if exists.
        */
        void ActivateNextUrlL();

    private:    // new methods

        /**
        * Set CBA.
        * @param aCommandSetResourceId Command set resource id.
        */
        void SetCbaL( TInt aCommandSetResourceId );

        /**
        * Access the document.
        * @return The document.
        */
        inline CCodDocument* CodDocument();

    private:    // data

        
        TBool iCod;                         ///< COD or DD?
        CCodUiResource* iCodResource;       ///< CodUi resource. Owned.
        CCodView* iView;                    ///< App's view (control). Owned.
        CCodWaitDialog* iWaitNote;          ///< Wait note. Owned.
        CRequestCompleteCallback* iCallback;///< Callback object. Owned.
        TInt iCurrentCba;                   ///< Current CBA.
        CErrorUI* iErrorUi;                 ///< Error UI. Owned.
        CDocumentHandler* iDocHandler;      ///< DocHandler or NULL. Owned.
        CAiwGenericParamList* iDocParams;   ///< DocHandler params. Owned.
        RFs iFs;                            ///< File Server Session.
        TInt iError;                        ///< Error code.
        TBool iSilentMode;                  ///< Silent mode.
        TBool iSuppressNextUrl;             ///< Suppress Next-URL.
        TBool iSuppressLaunch;              ///< Suppress launch.
        TBool iRoap;                        ///< ROAP involved?
        TInt iConnError;					///< Connection Error
    };

#endif /* def COD_APP_UI_H */
