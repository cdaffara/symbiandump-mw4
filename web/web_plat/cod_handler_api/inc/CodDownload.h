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
* Description:  Declaration of class CCodDownload.   
*
*/


#ifndef COD_DOWNLOAD_H
#define COD_DOWNLOAD_H

#warning The COD Handler API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES

#include <e32base.h>

// FORWARD DECLARATION

class CCodDownload;
class CCodDownloadImpl;
class CAiwGenericParamList;
class CEikonEnv;

// CLASS DECLARATION

/**
* Cod Download observer.
*/
NONSHARABLE_CLASS( MCodDownloadObserver )
    {
    public:     // types

        enum TEvent              ///< Event.
            {
            EStart,              ///< Load initiated.
            ENameChanged,        ///< Name changed.
            ETypeChanged,        ///< Type changed.
            EProgressUpdate,     ///< Progress update.
            EDone,               ///< Finished (success or failure).
            EAccept,             ///< Load accepted by user (start of load).
            EEndLoad,            ///< Waiting for service flow (end of load).
            EPdPlayAvailable,	 ///< in case of PD allow user to choose 'play'
            EUpdatedDDURI,       ///< Updated DD URI notification to the DM Server
            EResumed,			 ///< Resume successful and ready for PD
            EDownloadPausable,   ///< Informs the Server that download is PAUSABLE
            EDownloadNonPausable,///< Informs the Server that download is NOT PAUSABLE
            EHandlePostResponse, ///<  Intiates fresh download as part handling PostResponseURL of metering response 
            EUpdatedMediaInfo,   ///< Updated Media Info to DM Server
            ESetActiveDownload,  ///< Inform the server about currently active download
            EUpdatedTrackInfo    ///< Updated Media Info to DM Server
            };

    public:     // New methods.

        /**
        * Cod Download event.
        * @param aDownload Download.
        * @param aEvent Event.
        */
        virtual void CodEventL( CCodDownload& aDownload, TEvent aEvent ) = 0;

    };

/**
* Cod Download progress tracking.
*/
NONSHARABLE_STRUCT( TCodDownloadProgress )
    {
    public:     // types

        enum TState             ///< State.
            {
            EInit,              ///< Initial state.
            EInProgress,        ///< In progress.
            ESucceeded,         ///< Succeeded.
            EFailedTemporary,   ///< Failed (temporary error).
            EFailedPermanent,    ///< Failed (permanent error).
            EPaused  			///< Paused
            };

    public:     // data

        TState iState;  ///< Download state.
        TInt iError;    ///< Error code.
        TInt iBytesCur; ///< Bytes downloaded so far (-1 if unknown).
        TInt iBytesMax; ///< Total download size (estimated; -1 if unknown).
    };

/**
* Descriptor-based download.
* Supported descriptor types:
* - text/x-co-desc (Nokia COD)
* - application/vnd.oma.dd+xml (OMA DD)
* - multipart/related, where the first part is one of the two above.
*/
NONSHARABLE_CLASS( CCodDownload ): public CBase
    {
    public:     // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @param aBuf The descriptor.
        * @param aType Descriptor data type.
        * @param aObserver Observer.
        * @param aEikEnv Eikon environment or NULL. Can be NULL if download
        * does not require UI functionality. If download needs UI but NULL is
        * passed here, the download fails with EFailedPermanent KErrCancel.
        * @return The constructed object.
        */
        IMPORT_C static CCodDownload* NewL
            (
            const TDesC8& aBuf,
            const TDesC8& aType,
            MCodDownloadObserver& aObserver,
            CEikonEnv* aEikEnv
            );

        /**
        * Two-phased constructor. Leaves on failure.
        * This one supports multipart/related where the first part of the
        * multipart is the descriptor.
        * @param aBuf Data buffer (descriptor or multipart).
        * @param aType Data type.
        * @param aObserver Observer.
        * @param aEikEnv Eikon environment or NULL. Can be NULL if download
        * does not require UI functionality. If download needs UI but NULL is
        * passed here, the download fails with EFailedPermanent KErrCancel.
        * @param aMultipartBoundary Multipart boundary or NULL.
        * Required for processing multipart.
        * @param aMultipartUrl Multipart URL or NULL.
        * Required for processing multipart.
        * @return The constructed object.
        */
        IMPORT_C static CCodDownload* NewL
            (
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
        IMPORT_C virtual ~CCodDownload();

    public:     // New methods

        /**
        * Start download. Panics if already active.
        * Restart is OK for completed downloads.
        * @param aParams Parameters or NULL.
        */
        IMPORT_C void StartL( const CAiwGenericParamList* aParams );

        /**
        * Stop (cancel) download. Safe to call in any state.
        * Note: processing continues until MCodDownloadObserver::EDone event.
        */
        IMPORT_C void Stop();

        /**
        * Pauses the COD download
        */  
        IMPORT_C void Pause();
        
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
        * Available in ESucceeded state only.
        * @return Path of saved content. Fits to KMaxFileName.
        */
        IMPORT_C TPtrC GetPath() const;

        /**
        * Perform service flow after a succesful or failed download.
        * This consists of zero, one or both of
        * - launching downloaded content
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
        * Get the updated DD URL.
        * @return Updated DD URL.
        */
        IMPORT_C HBufC8* UpdatedDDUriL() const;
        
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

    public:     //Added as part of multiple download

        /**
        * Two-phased constructor. Leaves on failure.
        * @param aDownloadId Download ID.
        * @param aDlUid App UID.        
        * @param aType Descriptor data type.
        * @param aObserver Observer.
        * @param aEikEnv Eikon environment or NULL. Can be NULL if download
        * does not require UI functionality. If download needs UI but NULL is
        * passed here, the download fails with EFailedPermanent KErrCancel.
        * @return The constructed object.
        */            
        IMPORT_C static CCodDownload* NewL
	        (
	        const TInt aDownloadId,
	        const TUid aDlUid,
	        const TDesC8& aType,
	        MCodDownloadObserver& aObserver,
	        CEikonEnv* aEikEnv 
	        );

        /**
        * Set the CodData stream descriptor.
        * @param aDesStream.
        */
        IMPORT_C HBufC8* UpdatedDownloadDataL() const;
        
        /**
        * Set the stream descriptor for single media object (track).
        * @param aDesStream.
        */
        IMPORT_C HBufC8* UpdatedTrackDataL(TInt& aValue) const;

        /**
        * Get COD Active download 
        * @return Count of currently active download
        */
        IMPORT_C TInt ActiveDownload ( );
        /**
        * Get Destination file name 
        * @param aMOIndex media object index between 1 to 'n'
        * @return NULL if index out of range
        */
        IMPORT_C TPtrC GetDestFilePath (TInt aMOIndex ) const;
            
    private:    // Constructor

        /**
        * Constructor.
        */
        CCodDownload();

    private:    // Data 

        CCodDownloadImpl* iImpl;    ///< Implementation. Owned.

    };

#endif /* def COD_DOWNLOAD_H */
