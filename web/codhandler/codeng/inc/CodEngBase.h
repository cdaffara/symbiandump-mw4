/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Declaration of class CCodEngBase.   
*      
*
*/


#ifndef COD_ENG_BASE_H
#define COD_ENG_BASE_H

// INCLUDES

#include <e32base.h>
#include <f32file.h>
#include <apmrec.h>
#include <apparc.h>
#include "CodSaver.h"
#include <CodDownload.h>

// FORWARD DECLARATION

class CCodData;
class CDocumentHandler;
class CEikProcess;
class MCodLoadObserver;
class CConnection;
class CHttpLoader;
class CRoapData;
class TCodProgress;
class CBodyPart;
class CAiwGenericParamList;

class CHeaderField;
class CDownloadDataClient;
class CMediaDataClient;

// CONSTANTS
#ifdef RD_MULTIPLE_DRIVE
const TInt KMaxDriveListStrLen = KMaxDrives << 1;
#endif

/// Estimated data transfer for notify. Needed for progress.
LOCAL_D const TInt KCodNotifyTraffic = 1024;

_LIT( KDownloadFolderFormat, "%S%x\\%S\\" ); // <DRIVE> <KCodDefaultTempDir> \\ <uid> \\ <KTempFilesCodDirName>

_LIT( KCodDefaultTempDir, "\\system\\dmgr\\" );
_LIT( KDmDefaultDir, "c:\\system\\dmgr\\" );
_LIT( KInfoFilesCodDirName, "codinfo" );
_LIT( KTempFilesCodDirName, "contents" );


// CLASS DECLARATION

/**
* COD Handler Engine Base (processing COD, fetching content, posting status).
*/
NONSHARABLE_CLASS( CCodEngBase ): public CActive, public MCodSaverFactory
    {

    public:     // Constructors and destructor

        /**
        * Destructor.
        */      
        IMPORT_C virtual ~CCodEngBase();


    public:     // new methods

        /**
        * Process non-Unicode buffer and set data. Character set conversion is
        * applied first. Character set is auto-detected if not supplied.
        * Do this first before attempting anything else.
        * @param aBuf Raw COD content buffer.
        * @param aStatus Completes when done.
        * @param aParams Parameters or NULL.
        * @param aParts If descriptor is part of a multipart/related, body
        * parts of the multipart/related content. NULL if not multipart.
        */
        IMPORT_C void SetL
            (
            const TDesC8& aBuf,
            TRequestStatus* aStatus,
            const CAiwGenericParamList* aParams,
            const RPointerArray<CBodyPart>* aParts
            );

        /**
        * Return current COD.
        * @return Current COD data.
        */
        IMPORT_C const CCodData& Data() const;

        /**
        * Set or unset observer (pass NULL to unset).
        * Panics if already set; unsetting more is allowed.
        * @param aObserver Observer to set or NULL to unset.
        */
        IMPORT_C void SetObserver( MCodLoadObserver* aObserver );
        
        /**
        * Copy data from CCodData to CDownloadDataClient.
        */ 
    	void ConvertCodDataToDownloadDataL( CDownloadDataClient*& aDlData );
    	
    	/**
        * Copy data from CCodData to CDownloadDataClient.
        */ 
    	void ConvertMOToMediaDataL( CMediaDataClient*& aMOData, TInt aMOIndex );
    	
    	/**
        * Inform DownloadMgrClntSrv about updated CodData.
        */ 
    	void UpdateMediaInfoL();
    	
    	/**
        * Inform DownloadMgrClntSrv about updated Media Object (Track).
        */ 
    	void UpdateTrackInfoL(); 
    	
    	/**
        * Inform DownloadMgrClntSrv about change in Type of a content. 
        */ 
    	void ContentTypeChanged();        
        
    public:     // command handling

        /**
        * Accept COD (download content, post status);
        * @param aStatus Completes when done.
        */
        IMPORT_C void Accept( TRequestStatus* aStatus );

        /**
        * Reject COD (don't download content, post status);
        * @param aStatus Completes when done.
        */
        IMPORT_C void Reject( TRequestStatus* aStatus );

        /**
        * Similar to Cancel, but further processing may be invoked (e.g.
        * install-notify), so the request may not complete immediately.
        * Please use this method (instead of Cancel) for user cancellation of
        * downloads. Keep Cancel for error situations (final cleanup etc.)
        */
        IMPORT_C void Stop();

    public:     // checking results

        /**
        * Check if downloading to removable media.
        * @return ETrue if downloading to removable media.
        */
        IMPORT_C TBool RemovableMedia() const;

        /**
        * Get path of saved content (or empty string).
        * @return Path of saved content. Fits to KMaxFileName.
        */
        IMPORT_C TPtrC GetPath() const;

        /**
        * Get type of saved content.
        * @return Type of saved content.
        */
        IMPORT_C const TDataType& GetType() const;

        /**
        * Get UID of handler for the saved content.
        * @return Handler UID.
        */
        IMPORT_C TUid GetHandler() const;

        /**
        * TODO can this be removed form API?
        * Get HTTP status code of last operation. Default value 900.
        * @return HTTP status code.
        */
        IMPORT_C TInt GetStatusCode() const;

        /**
        * Get ROAP data (or NULL if no ROAP was involved).
        * @return ROAP data. This is NULL if no ROAP was involved.
        */
        IMPORT_C const CRoapData* GetRoapData() const;

        /**
        * Set attach to download for the COD (set from the DlMgr client)
        * If aValue equals to ETrue it means that file is playing progressively
        */
        IMPORT_C void SetCodDlAttached(const TBool aValue);

        /**
        * for Paused Download the info files are read
        * Do this first before attempting anything else.
        * @param aDownloadId the unique download ID
        * @param aDlUid the application Uid
        * @param aParams Parameters or NULL.
        * @param aParts If descriptor is part of a multipart/related, body
        */
        IMPORT_C void SetL
            (
            const TInt aDownloadId,
            const TUid aDlUid,                  
            const RPointerArray<CBodyPart>* aParts
            );

        /**
        * Start the paused download
        * @param aParams, parameters from Download Manager server
        * @param aStatus , active object status
        */
        IMPORT_C void Start( const CAiwGenericParamList* aParams, TRequestStatus* aStatus );
        
        /**
        * Return current COD.
        * @return Current COD data.
        */
        IMPORT_C HBufC8* UpdatedDownloadDataL();
        
        /**
        * Return stream descriptor for single media object (track).
        * @return Current COD data.
        */
        IMPORT_C HBufC8* UpdatedTrackDataL(TInt& aValue);

        /**
        * Get Destination file name 
        * @param aMOIndex media object index between 1 to 'n'
        * @return NULL if index out of range
        */
        IMPORT_C TPtrC GetDestFilePath (TInt aMOIndex ) const;

        /**
        * Perform Pause for COD download.
        */      	
        IMPORT_C void Pause();
    	
        /**
        * Returns a flag saying whether or not download is pausable
        * @return void
        * @Added in OMA pause-resume feature
        */		
        IMPORT_C TBool Pausable();  
        
        /**
        *Returns the currently Active download
        * @return Index of the currently active download
        */	        
        IMPORT_C TInt ActiveDownload( );
        
        /**
        * TODO can this be removed form API?
        * Get HTTP status code of last operation. Default value EFailedPermanent.
        * @return HTTP status code.
        */        
        IMPORT_C TCodDownloadProgress::TState GetProductStatusCode() const;
        /**
        * Check if download has Preview-only rights
        * @return ETrue if download is Preview-only
        */
        TBool IsDrmPreviewOnly() { return iDrmPreviewOnly; }

        /**
        * Set Download descriptor for OMA 2
        */
        void SetDd2Type(TBool aIsDd2){iIsDd2 = aIsDd2;}
        
        /**
        * Creates download info folder
        * @return void
        * @Added in OMA pause-resume feature
        */
		void CODDownloadInfoFolder( TDes& aFolder ) const;
		
        /**
        * Returns the download ID
        * @return DownloadID
        * @Added in OMA pause-resume feature
        */
		TUint32 DownloadID();
		
        /**
        * Updates the download file path
        * @return void
        * @Added in OMA pause-resume feature
        */
        void StoreFilePathsL(TPtr8& aBuf);
		
        /**
        * Reads the download file path from info file
        * @return void
        * @Added in OMA pause-resume feature
        */		
        void LoadFilePathsL(RFile& aInFile);
		
		
        /**
        * Updates the pausability of the download
        * @return void
        * @Added in OMA pause-resume feature
        */		
        void SetPausable(TBool aPausable);

        /**
        *
        * To indicate the download is resumed and ready for PD
        * @return void
        * @Added in OMA pause-resume feature
        */
        void SetResumePDAvailable();
        
        /**
        * Returns download info file path
        * @return file path
        * @Added in OMA pause-resume feature
        */        
        TFileName& CodDlInfoFile() 
    	    {
    	    return   iCodDlInfoPath; 
    	    }
    	
    	/**
    	* Set iUrl in CCodData
    	*/
    	void SetUrlL(const TDesC& aUrl);
    	
        /**
        * Creates the info files when Cod Engine is started
        */
        void StoreInfoFilesL();
        
        /**
        * Creates info file for product (CCodData class)
        */
        void StoreMainInfoFileL();        
        
        /**
        * Creates info file for specified media object
        */
        void StoreSubInfoFileL( const CArrayPtrFlat<CHeaderField>* aHeaders, TInt aMediaIndex );

		/**
		* Loads the CodData and MediaObjectData from info files
		*/
        void LoadMainInfoFileL();	
        
		/**
		* Loads the CodData from info files
		*/
        void LoadInfoFilesL();
		
		/**
		* Updates the downloaded size in media object
		*/        
        void UpdateDownloadedSize( TInt aSize );
        /**
		* Loads the MediaObjectData from info files
		*/
        void LoadSubInfoFileL( TInt aMediaIndex, CArrayPtrFlat<CHeaderField>* aHeaders );

    protected:  // Constructors and destructor

        /**
        * Constructor.
        * @param aObserver Observer.
        */      
        CCodEngBase( MCodLoadObserver* aObserver );
        
        /**
        * Second phase constructor. Leaves on failure.
        * @param aProcess Host process.
        */      
        void ConstructL( CEikProcess* aProcess );

    protected:  // from CActive

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

    protected:  // NOT from MApaEmbeddedDocObserver

        /**
        * Embedded document viewing has ended.
        * @param aMode Exit mode (actually unused).
        */
        void NotifyExit( MApaEmbeddedDocObserver::TExitMode aMode );

    protected:    // types

        enum TState                 ///< State.
            {
            EInit,                  ///< Initial state (no valid data set).
            ESet,                   ///< Setting data is requested.
            EReady,                 ///< Initial state (valid data set).
            EStartInstallRoap,      ///< Installing Roap trigger from DD2
            EInstallRoap,           ///< RO recieved
            EStartConnect,          ///< Initiate connecting to network.
            EConnect,               ///< Connecting to network.
            EStartFetch,            ///< Fetching requested.
            EFetch,                 ///< Fetching content (GET).
            EStartInstall,          ///< Installing content requested.
            EInstall,               ///< Installing content.
            EStartNotify,           ///< Notify requested.
            ENotify,                 ///< Reporting status (POST).
            EPause,					///< Pause state.
            EMediaChange,           ///<Media object change (Track Change)     
            EStartProductNotify,    ///<Product Notify Requested
            EProductNotify          ///<Reporting Status (POST)
            };

        enum TNotificationStatus    ///< Notification status.
            {
            ENull,                  ///< No notification.
            ELoad,                  ///< Notified about loading.
            ECancel,                 ///< Notified about cancelling.
            EPaused                 ///< Notified about pause.
            };

    protected:    // state machine parts

        /**
        * Process buffer and set data.
        */
        void SetL();

        /**
        * Initiate Roap installation.
        */
        void StartInstallRoapL();

        /**
        * Roap installation ended.
        */
        void EndInstallRoapL();

        /**
        * Initiate connecting to network.
        */
        void StartConnectL();

        /**
        * Connecting to network ended.
        */
        void EndConnectL();

        /**
        * Initiate fetching content.
        */
        void StartFetchL();

        /**
        * Fetch ended.
        */
        void EndFetchL();

        /**
        * Initiate installing received content (if any).
        */
        void StartInstallL();

        /**
        * Installing ended.
        */
        void EndInstallL();

        /**
        * Initiate notification.
        */
        void StartNotifyL();

        /**
        * Notification ended.
        */
        void EndNotifyL();
		
		/**
		*Change Media Object
		*/
		void ChangeMediaObjectL();
        
        /**
        * Clean up and notify parent.
        */
        void Done();

        /**
        * Clean up and notify parent for case of dd2 with license tag.
        */
        void DoneDD2();

        /**
        * Invoke next state asynchronously (via RunL()).
        * @param aNextState Next state.
        */
        void Continue( TState aNextState );

        /**
        * Complete own request status (if not already completed).
        * @param aError Error code to complete with.
        */
        void SelfComplete( TInt aError );
        
        /**
        *Initiate Notification for Product
        */
        void StartProductNotifyL();
        
        /**
        *End Notification
        */
        void EndProductNotifyL();

    protected:  // validity checks

        /**
        * Check data (storage space, MIME type, mandatory attributes etc.)
        * Leave on any errors.
        */
        virtual void CheckDataL() = 0;

        /**
        * Check device capabilities (MIME types, disk space) before download.
        * Leaves with KMimeNotSupported, KErrCodInsufficientSpace
        * (or system error codes).
        */
        void CapabilityCheckL();

    protected:  // error code / status code conversion.

        /**
        * Parse data buffer.
        */
        virtual void ParseDataL() = 0;

        /**
        * Get http status code from error code and state. This method contains
        * the  semantic interpretation of errors; e.g. failure to notify due
        * to loss of service is regarded as success, etc.
        * @param aError Error code.
        * @param aState State.
        * @return HTTP status code (to post).
        */
        virtual TInt StatusCode( TInt aError, TState aState ) const = 0;

        /**
        * Get textual representation of COD notify HTTP code. This text is
        * sent back as notification.
        * @param aCode HTTP code, values should only be the ones returned by
        * StatusCode(). Other values panic.
        * @return Textual form of HTTP code.
        */
        virtual const TDesC8& StatusText( TInt aCode ) const = 0;

    protected:  // from MCodSaverFactory

        /**
        * Create saver object.
        * @param aType Data type.
        * @return Saver object for this data type. Ownership not passed.
        */
        CCodSaver* CreateSaverL( const TDesC8& aType );

    private:    // implementation
    
        /**
        * Set root and temp paths.
        */
        void SetPathsL();
        
        /**
        * Reset paths.
        */
        void ResetPaths();

        /**
        * Set file name based on the temp path file name from the url (for dd2) 
        */
        void SetFileNameL();
        
        /**
        * Append index to file name to avoid overwriting existing file
        */
        void SetUniqueFileNameL();

#ifdef RD_MULTIPLE_DRIVE

        /**
        * Query the dynamic drive list.
        */
        HBufC8* QueryDynDriveListLC();

        /**
        * Query the drive list from the CentRep.
        */
        void QueryDriveListL();
#endif
        
    protected:  // data

        /// COD Data. Owned.
        CCodData* iData;
        /// COD buffer. Owned.
        HBufC* iCodBuf;
        /// Connection. Owned.
        CConnection* iConn;
        /// Http loader. Owned.
        CHttpLoader* iLoader;
        /// Current state.
        TState iState;
        /// Document handler. Owned.
        CDocumentHandler* iDocHandler;
        /// Observer or NULL. Not owned.
        MCodLoadObserver* iObserver;
        /// Saver object. Owned.
        CCodSaver* iSaver;
        /// Parent status.
        TRequestStatus* iParentStatus;
        /// ETrue when Stop is in progress.
        TBool iStopInProgress;
        /// Preferred IAP.
        TUint32 iPreferredIap;
        /// File Server session.
        RFs iFs;
        /// Keeping track of observer notifications.
        TNotificationStatus iNotificationStatus;
        /// Data type of saved content.
        TDataType iType;
        /// Handler app UID.
        TUid iHandler;
        /// Saving to FS.
        TBool iFsUsed;
        /// Download id
		TUint32 iDownloadId;
        /// App UId
        TUid iAppUId;
        /// COD info file path
        TFileName iCodDlInfoPath;
        
#ifdef RD_MULTIPLE_DRIVE 
        /// Available memory has enough space to save the content
        TBool iSpaceAvailOk;
        /// Drive number that has enough space to save the content
        TInt iAvailDriveSpace;
        /// storage precedence from Cenrep
        TBuf<KMaxDriveListStrLen> iDriveLettersCenRep;
#else
        /// Saving to phone memory is OK.
        TBool iPhoneMemoryOk;
        /// Saving to MMC is OK.
        TBool iMmcOk;
#endif
        /// Paths on removable media?
        TBool iRemovableMedia;
        /// ROAP Data. Owned.
        CRoapData* iRoapData;
        /// Progress or NULL. Owned.
        TCodProgress* iProgress;
        /// Multipart body parts or NULL. Not owned.
        const RPointerArray<CBodyPart>* iParts;
        /// Parameters or NULL. Not owned.
        const CAiwGenericParamList* iParams;
        /// download rights are Preview-only
        TBool iDrmPreviewOnly;     
        /// ETrue if download descriptor is DD2 (OMA v2 download descriptor) 
        TBool iIsDd2;
        /// ETrue if DD2 has license element
        TBool iIsLicense;
        /// ETrue if download is attached (playing progressively)
        TBool iAttached;
        /// HTTP code sent in install-notify.
      	TInt iStatusCode;	        
      	/// Resulting error code (overall result).
        TInt iResult;
        /// Confirms whether content type check is done
        TBool iContentTypeCheck;
        //True if the download is resumed after pause
        TBool iResumedDownload;

    };

#endif /* def COD_ENG_BASE_H */
