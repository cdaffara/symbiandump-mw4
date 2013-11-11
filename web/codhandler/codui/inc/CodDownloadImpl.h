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
* Description:  Declaration of class CCodDownloadImpl.   
*
*/


#ifndef COD_DOWNLOAD_IMPL_H
#define COD_DOWNLOAD_IMPL_H

// FORWARD DECLARATION

class CCodEngBase;
class CCodUiResource;
class CDocumentHandler;
class CErrorUI;
class CBodyPart;
class CAiwGenericParamList;

// INCLUDES

#include <e32std.h>
#include <f32file.h>
#include <CodDownload.h>
#include <CodLoadObserver.h>
#include <AknServerApp.h>

// CONSTANTS
const TUid KDRMHelperUid = {0x101F85C7};

/**
* Cod Download implementation (behind proxy CCodDownload).
*/
NONSHARABLE_CLASS( CCodDownloadImpl )
: public CActive,
  public MCodLoadObserver,
  public MAknServerAppExitObserver
    {
    public:     // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @param aProxy Proxy owning this object.
        * @param aBuf The descriptor.
        * @param aType Descriptor data type.
        * @param aObserver Observer.
        * @param aEikEnv Eikon environment or NULL. Can be NULL if download
        * does not require UI functionality.
        * @param aMultipartBoundary Multipart boundary or NULL.
        * Required for processing multipart.
        * @param aMultipartUrl Multipart URL or NULL.
        * Required for processing multipart.
        * @return The constructed object.
        */
        IMPORT_C static CCodDownloadImpl* NewL
            (
            CCodDownload& aProxy,
            const TDesC8& aBuf,
            const TDesC8& aType,
            MCodDownloadObserver& aObserver,
            CEikonEnv* aEikEnv,
            const TDesC8* aMultipartBoundary,
            const TDesC16* aMultipartUrl
            );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CCodDownloadImpl();

    public:     // New methods

        /**
        * Start download. Panics if already active.
        * Restart is OK for completed downloads.
        * @param aParams Parameters or NULL.
        */
        IMPORT_C void StartL( const CAiwGenericParamList* aParams );

        /**
        * Stop (cancel) download. Safe to call in any state.
        * Some processing still follows (e.g. install-notify).
        */
        IMPORT_C void Stop();

        /**
        * Get progress.
        * @return Progress.
        */
        IMPORT_C const TCodDownloadProgress& Progress() const;

        /**
        * Get name attribute. This may be empty string, if attribute is
        * empty or not available. Do not store pointer (data may change as
        * download proceeds).
        */
        IMPORT_C TPtrC Name();

        /**
        * Get data type of content (or empty string: type may not be
        * available).
        * @return Data type.
        */
        IMPORT_C TPtrC8 Type() const;

        /**
        * Get path of saved content (or empty string).
        * Available only after successful download.
        * @return Path of saved content. Fits to KMaxFileName.
        */
        IMPORT_C TPtrC GetPath() const;

        /**
        * Perform service flow after a successful or failed download.
        * This consists of zero, one or both of
        * - launching dwonloaded content
        * - activating Next-URL.
        * Load may also be retried.
        * Call this in response to EEndLoad event.
        */
        IMPORT_C void ServiceFlowL();

        /**
        * Get Next-URL.
        * @return Next-URL. Owner is the caller.
        */
        IMPORT_C HBufC8* NextUrlL() const;
        
        
        /**
        * Get PostResponse-URL.
        * @return PostResponse-URL. Owner is the caller.
        */
        IMPORT_C HBufC8* GetPostResponseUrlL() const;
        
        /**
        * Check if downloading to removable media.
        * Available after EAccept event.
        * @return ETrue if downloading to removable media.
        */
        IMPORT_C TBool RemovableMedia() const;

        /**
        * Set COD download attached
        * @param aValue If equals ETrue - download is attached
        */
        IMPORT_C void SetCodDlAttached(const TBool aValue);
        
        /**
        * Get updated DD URi
        * @return Updated DD-URL.
        */
        IMPORT_C HBufC8* UpdatedDDUriL() const;
        
        /**
        * Pause COD Download.
        */           
        IMPORT_C void Pause();

        /**
        * Two-phased constructor. Leaves on failure.
        * @param aProxy Proxy owning this object.
        * @param aDownloadId The download id.
        * @param aDlUid The app UID.
        * @param aType Descriptor data type.
        * @param aObserver Observer.
        * @param aEikEnv Eikon environment or NULL. Can be NULL if download
        * does not require UI functionality.
        * @param aMultipartBoundary Multipart boundary or NULL.
        * Required for processing multipart.
        * @param aMultipartUrl Multipart URL or NULL.
        * Required for processing multipart.
        * @return The constructed object.
        */            

        IMPORT_C static CCodDownloadImpl* NewL
            (
            CCodDownload& aProxy,
            const TInt aDownloadId,
            const TUid aDlUid,
            const TDesC8& aType,
            MCodDownloadObserver& aObserver,
            CEikonEnv* aEikEnv,
            const TDesC8* aMultipartBoundary,
            const TDesC16* aMultipartUrl            
            );

        /**
        * Get Destination file name 
        * @param aMOIndex media object index between 1 to 'n'
        * @return NULL if index out of range
        */
        IMPORT_C TPtrC GetDestFilePath (TInt aMOIndex ) const;

        /**
        * Set the CodData stream descriptor.
        * @param aDesStream.
        */
        IMPORT_C HBufC8* UpdatedDownloadDataL() const;
        
        /**
        * Set the stream descriptor for single mediaq object (track).
        * @param aDesStream.
        */
        IMPORT_C HBufC8* UpdatedTrackDataL(TInt& aValue) const;

        /**
        * Currently active COD download 
        */          
        IMPORT_C TInt ActiveDownload();

    public:     // Transparent user data

        /**
        * Get tansparent user data.
        * @return User data.
        */
        IMPORT_C TAny* UserData() const;

        /**
        * Set tansparent user data.
        * @param aUserData User data.
        */
        IMPORT_C void SetUserData( TAny* aUserData );

    private:    // Constructor

        /**
        * Constructor.
        * @param aProxy Proxy owning this object.
        * @param aObserver Observer.
        * @param aEikEnv Eikon environment or NULL. Can be NULL if download
        * does not require UI functionality.
        */
        inline CCodDownloadImpl
            (
            CCodDownload& aProxy,
            MCodDownloadObserver& aObserver,
            CEikonEnv* aEikEnv
            );

        /**
        * Second phase constructor. Leaves on failure.
        * @param aBuf The descriptor.
        * @param aType Data type.
        * @param aMultipartBoundary Multipart boundary or NULL.
        * Required for processing multipart.
        * @param aMultipartUrl Multipart URL or NULL.
        * Required for processing multipart.
        */
        void ConstructL
            (
            const TDesC8& aBuf,
            const TDesC8& aType,
            const TDesC8* aMultipartBoundary,
            const TDesC16* aMultipartUrl
            );

        /**
        * Second phase constructor. Leaves on failure.
        * @param aDownloadId The download id.        
        * @param aDlUid The app uid.
        * @param aType Data type.
        * @param aMultipartBoundary Multipart boundary or NULL.
        * Required for processing multipart.
        * @param aMultipartUrl Multipart URL or NULL.
        * Required for processing multipart.
        */            
        void ConstructL
            (
            const TInt aDownloadId,
            const TUid aDlUid,
            const TDesC8& aType,
            const TDesC8* aMultipartBoundary,
            const TDesC16* aMultipartUrl
            );
                        

    private:  // from CActive

        /**
        * Cancel protocol implementation.
        */
        virtual void DoCancel();

        /**
        * Outstanding request completed.
        */
        virtual void RunL();

        /**
        * Handle error.
        * @param aError Error code.
        * @return KErrNone.
        */
        virtual TInt RunError( TInt aError );

    private:    // processing completion

        /**
        * SetL succeeded.
        */
        void SetOkL();

        /**
        * Load done.
        */
        void LoadDoneL();

        /**
        * Perform service flow after a successful or failed download.
        * This consists of zero, one or both of
        * - launching dwonloaded content
        * - activating Next-URL.
        * Load may also be retried.
        * @return ETrue if application should exit.
        */
        TBool DoServiceFlowL();

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
        * Service flow: launch browser and (optionally) activate Next-URL,
        * if exists.
        */
        void ActivateNextUrlL();

        /**
        * Finished.
        */
        void Done();

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
        * Cancelling content load started. Not called if cancelling error
        * report.
        */
        virtual void StartCancelL();

        /**
        * Load progress update.
        * @param aFinalValue Final progress value.
        * @param aCurrentValue Current progress value.
        */
        virtual void ProgressL( TInt aFinalValue, TInt aCurrentValue );

        /**
        * Processing ended.
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
        * @param aPassword Return password here.
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
        * Inform DownloadMgrClntSrv about updated media info
        * return: Connection Error .
        */
		virtual void UpdateMediaInfoL();
        
        /**
        * SetActive Download
        */
        virtual void SetActiveDownload( );
		/**
        * Inform DownloadMgrClntSrv about updated track
        * return: Connection Error .
        */
		virtual void UpdateTrackInfoL();
        
        /**
        * Displays Info message "Waiting for license" 
        */
        virtual void WaitForLicenseL();
        
        /**
        * To indicate the download is paused
        */
        inline virtual void DownloadPaused() {iDownloadPaused = ETrue;}

        /**
        * To indicate the download is resumed and ready for PD
        */
        inline virtual void DownloadResumedPdAvailable()
            {
        	iObserver.CodEventL( iProxy, MCodDownloadObserver::EResumed );
            }

        /**
        * To update the server about whether or not download is pausable
        */
        virtual void UpdatePausable( TBool aPausable );
        
        
        /**
        * To update the server about next media object being downloaded.
        */        
        virtual void MediaObjectNameChanged();
        
        /**
        * To update the server about content type.
        */
        virtual void ContentTypeChanged();

        /**
        * 
        * To handle PostResponseUrlL as part of metering response
        * @param aPrUrl: The PostResponseURL
        * @return void
        *  
        */
        virtual void HandleRoapPostResponseUrlL( const TDesC8& aPrUrl );


    private:    // from MAknServerAppExitObserver

        /**
        * Embedded document viewing has ended.
        * @param aReason Exit reason (unused).
        */
        void HandleServerAppExit( TInt aReason );

    private:    // types
    
        enum TState ///< State.
            {
            EInit,          ///< Initial state.
            ESet,           ///< Set.
            ELoad,          ///< Loading.
            ELoadDone,      ///< Load done.
            EServiceFlow    ///< Service flow.
            };

    private:    // New methods

        /**
        * Reset progress.
        */
        inline void ResetProgress();

        /**
        * Get Next-URL (possibly relative) attribute.
        * @param aBack Etrue is returned here if Next-URL is 'back'.
        * @return Next-URL atttribute.
        */
        TPtrC8 NextUrlAttr( TBool& aBack ) const;

        /**
        * Call back function to launch the help 
        */ 
        static TInt ShowRoapCallback( TAny* aPtr ); 

       /**
        * Launch help dialog
        * @param TDesC aContext
        * @return void
        */
        void LaunchHelpL( const TDesC& aContext );

    private:    // Data 

        TState iState;                      ///< State.
        CCodDownload& iProxy;               ///< Proxy object. Not owned.
        TBool iCod;                         ///< COD or DD?
        TBool iRoap;                        ///< ROAP involved?
        HBufC8* iBuf;                       ///< Data buffer. Owned.
        TPtrC8 iDescriptor;                 ///< Descriptor.
        MCodDownloadObserver& iObserver;    ///< Observer. Not owned.
        CEikonEnv* iEikEnv;                 ///< Eikon env or NULL. Not own.
        CCodUiResource* iUiResource;        ///< UI Resource or NULL. Owned.
        CErrorUI* iErrorUi;                 ///< Error UI or NULL. Owned.
        CCodEngBase* iEng;                  ///< Engine. Owned.
        TCodDownloadProgress iProgress;     ///< Progress.
        TAny* iUserData;                    ///< User data. Not own.
        TBool iAutoAccept;                  ///< Suppress download conf.
        TBool iSilentMode;                  ///< Silent mode.
        TBool iSuppressNextUrl;             ///< Suppress Next-URL.
        TBool iSuppressLaunch;              ///< Suppress launch.
        TBool iFota;                        ///< FOTA download?
        CDocumentHandler* iDocHandler;      ///< DocumentHandler or NULL.
        CAiwGenericParamList* iDocParams;   ///< DocHandler params. Owned.
        RFs iFs;                            ///< File Server Session. Own.
        RPointerArray<CBodyPart> iParts;    ///< Multipart body parts. Owned.
        CAiwGenericParamList* iParams;      ///< Copy of received params. Own
        TInt iConnError;					///< Connection Error		.
        TBool iAttached;                    ///< Cod download attached (playing progressively)

        TUint32 iDownloadId;                ///< Download Id
        TUid iAppUId;                       ///< Client Application Uid 
        TBool iDownloadPaused;              ///< Download paused
        HBufC8* iPostResponseURL;			///< PostResponseURL from RoapSaver
    };

#include "CodDownloadImpl.inl"

#endif /* def COD_DOWNLOAD_IMPL_H */
