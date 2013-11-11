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
* Description: 
*      Declaration of class CRoapAppUi.   
*      
*
*/


#ifndef ROAP_APP_UI_H
#define ROAP_APP_UI_H

// INCLUDES

#include <e32base.h>
#include <f32file.h>
#include <aknappui.h>
#include <AknServerApp.h>
#include <RoapObserver.h>
#include <DownloadMgrClient.h>
#include "RoapAppWaitDialog.h"
#include <centralrepository.h>

// FORWARD DECLARATIONS

class CRoapAppView;
class CRoapAppDocument;
class CRequestCompleteCallback;
class CDocumentHandler;

// CONSTANTS
const TUid KDRMHelperUid = {0x101F85C7}; // KBrowserHelpUid = {0x10008D39} 

/**
* ROAP Application UI.
*/
class CRoapAppUi
: public CAknAppUi,
  public MRoapDialogCommandObserver,
  public Roap::MRoapObserver,
  public MAknServerAppExitObserver,
  public MHttpDownloadMgrObserver
    {

    public:     // Constructors and destructor

        /**
        * Constructor.
        */      
        CRoapAppUi();
        
        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        virtual ~CRoapAppUi();

    public:     // from CAknAppUi

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
        * Handle resource change.
        * @param aType Resource type.
        */
        virtual void HandleResourceChangeL( TInt aType );

    private:    // from Roap::MRoapObserver
    
        /**
        * Confirm creating network connection.
        * @return ETrue if network connection confirmed.
        */
        virtual TBool ConnectionConfL();

        /**
        * Confirm contacting Rights Issuer.
        * @return ETrue if confirmed.
        */
        virtual TBool ContactRiConfL();
		
        /**
        * Confirm transaction ID tracking.
        * @return ETrue if transaction ID tracking confirmed.
        */
        virtual TBool TransIdConfL();

       /**
        * Handle right objects after a successful RoAcq.
        * @param aRightsList Received right objects.
        */
		virtual void RightsObjectDetailsL
	        ( const RPointerArray<CDRMRights>& aRightsList );
		
        /**
        * ROAP response returned multipart response, return parameters
        * needed saving.
        * @param aTempFolder Folder for temp file returned here.
        * @param aContentName Name for content file returned here.
        * @param aMaxSize Safety size limit returned here.
        */
        virtual void ContentDownloadInfoL
            ( TPath& aTempFolder, TFileName& aContentName, TInt& aMaxSize );

        /**
        * Details of DCF file received in ROAP (saved by ROAP Engine).
        * @param aPath: The path and the filename of the saved DCF file.
        * @param aType: The plain MIME type of the saved DCF file.
        * @param aAppUid: The handler app UID of the saved DCF file.
        */
		virtual void ContentDetailsL
		    ( const TDesC& aPath, const TDesC8& aType, const TUid& aAppUid );
                                      
        /**
        * Handle DCF file (received in multipart RO response).
        * @aFilename File name.
        */
        virtual void HandleDCFPartL( const TDesC8& aFilename );

        /**
        * Progress callback.
        * @param aProgressInfo Percentage completed.
        */
		virtual void RoapProgressInfoL( const TInt aProgressInfo );
		
       /**
        * ErrorUrl
        * @param aErrorUrl: The error URL
        * @return void
        */
        virtual void ErrorUrlL( const TDesC8& aErrorUrl );

        /**
        * PostResponseUrlL
        * @param aPrUrl: The PostResponseURL
        * @return void
        */
        virtual void PostResponseUrlL( const TDesC8& aPrUrl );
        
	private:	//from MHttpDownloadMgrObserver        
        /**
        * Indicates an event.
        * @param aDownload The download with which the event occured.
        * @param aEvent Event type.
        * @return None.
        */
        virtual void HandleDMgrEventL( RHttpDownload& aDownload, THttpDownloadEvent aEvent );

        		
    private:    // from MAknServerAppExitObserver
    
        /**
        * Handle exmbedded application exit.
        * @param aReason Exit reason.
        */
        virtual void HandleServerAppExit( TInt aReason );
    
    public:     // new methods

        /**
        * Trigger parsed.
        */
        void HandleTriggerParsedL();
        
        /**
        * Call back function to launch the help 
        */ 
        static TInt ShowRoapCallback( TAny* aPtr ); 

        /**
        * Handle One Pass RO
        */
        void HandleOnePassRoL( TInt aError);

    private:    // new methods
            
        /**
        * Start ROAP.
        */
        void StartRoapL();
    
        /**
        * Cancel ROAP.
        */
        void CancelRoap();
    
        /**
        * Handle ROAP completion.
        */
        void RoapDone();

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
        * @param aError ROAP error.
        * @return ETrue if application should exit.
        */
        TBool RoapFailedL( TInt aError );

        /**
        * Handle ROAP completion (static wrapper for TCallBack use).
        * @param aPtr Pointer passed to the callback (this object as TAny*).
        * @return EFalse (as TInt).
        */
        static TInt StaticRoapDone( TAny* aPtr );

    private:    // new methods

        /**
        * Access the document (casted to its real type).
        * @return The document (casted to its real type).
        */
        CRoapAppDocument* RoapDocument();

    private:    // from MRoapDialogCommandObserver

        /**
        * Handle dialog command (from wait note).
        * @param aCommand Dialog command id.
        */
        void HandleDialogCommandL( TInt aCommand );

    private:    // new methods

        /**
        * Show an information note.
        * @param aPromptResourceId Resource id of prompt text (RESOURCE TBUF).
        */
        void InfoNoteL( TInt aPromptResourceId );

        /**
        * Show an information note.
        * @param aPrompt Prompt text.
        */
        void InfoNoteL( const TDesC& aPrompt );

        /**
        * Show an confirmation note.
        * @param aPromptResourceId Resource id of prompt text (RESOURCE TBUF).
        */
        void ConfNoteL( TInt aPromptResourceId );

        /**
        * Show an confirmation note.
        * @param aPrompt Prompt text.
        */
        void ConfNoteL( const TDesC& aPrompt );

    private:    // new methods

        /**
        * Check if file can be opened (unprotected, or rights present).
        * @param aFile File.
        * @return ETrue if file can be opened.
        */
        TBool CanOpenL( RFile& aFile ) const;
        
       /**
        * Launch help dialog
        * @param TDesC aContext
        * @return void
        */
        void LaunchHelpL( const TDesC& aContext );
		
    private:    // data

        /// App's view (control). Owned.
        CRoapAppView* iView;
        /// Wait note. Owned.
        CRoapAppWaitDialog* iWaitNote;
        /// Callback object. Owned.
        CRequestCompleteCallback* iCallback;
        /// Document handler or NULL. Owned.
        CDocumentHandler* iDocHandler;
        /// File Server Session.
        RFs iFs;
        /// MO associated with the received RO.
        RFile iFile;
        /// Download manager
        RHttpDownloadMgr   iDownloadMgr;
        /// Download status of PostResponse URL 
        TBool iPostResponseURLDownLoad;
    };

#endif /* def ROAP_APP_UI_H */
