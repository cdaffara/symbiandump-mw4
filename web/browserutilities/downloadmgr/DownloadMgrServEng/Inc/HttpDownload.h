/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements one download in DownloadManager
*
*/



#ifndef HTTPDOWNLOAD_H
#define HTTPDOWNLOAD_H

//  INCLUDES
#include "HttpClientAppInstance.h"
#include "HttpConnHandler.h"
#include "DownloadDataServ.h"

#include <HttpDownloadMgrCommon.h>
#include <e32base.h>
#include <es_sock.h>
#include <http.h>

// CONSTANTS
const TInt KDownloadNameMaxSize = 256;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS

// CLASS DECLARATION
class RFs;
class RFile;
class CFileMan;
class CHeaderField;
class CHttpStorage;


/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series 60 v2.8
*/
NONSHARABLE_CLASS( CHttpDownload ): public CActive
                                  , public MConnectionCallback
                                  , public MHTTPTransactionCallback
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CHttpDownload* NewL( const TDesC8& aUrl,
                                    CHttpClientApp *aClientApp,
                                    TInt32 aId,
                                    CHttpClientAppInstance* aClientAppInstance = NULL );
        
        /**
        * Destructor.
        */
        virtual ~CHttpDownload();

    public: // New functions

        /**
        * Starts download process.
        * @since Series 60 v3.2
        * @return none.
        */
        IMPORT_C void Attach( CHttpClientAppInstance* aClientAppInstance );
      
        /**
        * Starts download process.
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        IMPORT_C void StartL();

        /**
        * Pauses the download process.
        * @since Series 60 v2.8
        * @param bStore if ETrue download state is stored into file
        * @return none. Leaves on error.
        */
        IMPORT_C void PauseL( TBool bStore = EFalse );

        /**
        * Resets to the download.
        * @since Series 60 v2.8
        * @return none.
        */
        IMPORT_C void Reset();

        /**
        * Deletes the download. Every reference to this object 
        * becomes invalid after this function returns.
        * @since Series 60 v2.8
        * @return none.
        */
        IMPORT_C void Delete( CHttpClientAppInstance* aClAppInstance );

        /**
        * Deletes the info file associated with the download.
        * This can be called only after the download is completed, 
        *  since deletion of the file is illegal when the download is not yet complete.
        * @since Series 60 v3.2
        * @return none.
        */
		IMPORT_C void DeleteInfoFile( CHttpClientAppInstance* aClAppInstance );

        /**
        * Moves/renames download content file. Destination can be
        * specified with EDlAttrDestFilename
        * @since Series 60 v2.8
        * @param none
        * @return none. Leaves on error:
        * KErrNotReady - if download is not completed yet
        * KErrInUse - if move is in progress
        */
        IMPORT_C void MoveL();

        /**
		* Gets the value for a TInt attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aValue On completion, contains the requested TInt attribute.
        * @return void. Leaves on error:
        * - KErrArgument if requested attribute is unknown or not integer
		*/
		IMPORT_C void GetIntAttributeL( THttpDownloadAttrib aAttribute, 
									    TInt32& aValue );

        /**
		* Gets the value for a TBool attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aValue On completion, contains the requested TBool attribute.
        * @return void. Leaves on error:
        * - KErrArgument if requested attribute is unknown or not boolean
		*/
		IMPORT_C void GetBoolAttributeL( THttpDownloadAttrib aAttribute, 
                                         TBool& aValue );

		/**
		* Gets the value for a string attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aValue On completion, contains the requested string. Use a 16-bit descriptor!
        * @return void. Leaves on error:
        * - KErrArgument if requested attribute is unknown or not string
        * - KErrNotFound if requested attribute is not initialized. This can make
        * easier to differentiate between that string is zero length or not allocated yet.
		*/
		IMPORT_C HBufC* GetStringAttributeL( THttpDownloadAttrib aAttribute, 
                                             TBool& aDelete );

		/**
		* Gets the value for a string attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aValue On completion, contains the requested string. Use a 16-bit descriptor!
        * @return void. Leaves on error:
        * - KErrArgument if requested attribute is unknown or not string
        * - KErrNotFound if requested attribute is not initialized. This can make it
        * easier to differentiate between that string is zero length or not allocated yet.
		*/
		IMPORT_C HBufC8* GetString8AttributeL( THttpDownloadAttrib aAttribute, 
                                               TBool& aDelete );

        /**
        * 
        */
        IMPORT_C RFile* GetFileHandleAttributeL();

        /**
		* Sets the value for a TInt attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set.
        * @return void. Leaves on error:
        * - KErrArgument if requested attribute is unknown or not integer
		*/
		IMPORT_C void SetIntAttributeL( THttpDownloadAttrib aAttribute, 
                                        const TInt32 aValue );
		
		/**
		* Sets the value for a TBool attribute.
		* This method can be used to stop a connection (KConnectionStop).
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The value to be set.
        * @return void. Leaves on error:
        * - KErrArgument if requested attribute is unknown or not boolean
		*/
		IMPORT_C void SetBoolAttributeL( THttpDownloadAttrib aAttribute, 
                                         const TBool aValue );
		
		/**
		* Sets the value for a string attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The string value to be set. Must be a 16-bit descriptor!.
        * @return void. Leaves on error:
        * - KErrArgument if requested attribute is unknown or not string
		*/
		IMPORT_C void SetStringAttributeL( THttpDownloadAttrib aAttribute, 
                                           const TDesC16& aValue );

		/**
		* Sets the value for a string attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aValue The string value to be set. Must be a 16-bit descriptor!.
        * @return void. Leaves on error:
        * - KErrArgument if requested attribute is unknown or not string
		*/
		IMPORT_C void SetStringAttributeL( THttpDownloadAttrib aAttribute, 
                                           const TDesC8& aValue );

        /**
        * 
        */
        IMPORT_C void SetFileHandleAttributeL( RFile* aFile );
        
        /**
        * Returns the associated client application instance.
        * @since Series 60 v2.8
        * @return associated client application instance
        */
        IMPORT_C CHttpClientAppInstance* ClientAppInstance() const;

        /**
        * Called only from the server when a leave trapped.
        * This is the place where client app can be informed
        * about this error if not done before.
        * @since Series 60 v2.8
        * @param aError Error code from a previous leave
        * @return none
        */
        IMPORT_C void OnError( TInt aError,
                               THttpDownloadMgrError aDlError = EGeneral );

        /**
		* Gets the value for a TInt attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aMoIndex Identifies the media object index.
		* @param aValue On completion, contains the requested TInt attribute.
        * @return void. Leaves on error:
        * - KErrArgument if requested attribute is unknown or not integer
		*/
		IMPORT_C void GetIntAttributeL( THttpDownloadAttrib aAttribute, 
									    TInt32& aMoIndex,
									    TInt32& aValue );

        /**
		* Gets the value for a TBool attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aMoIndex Identifies the media object index.
		* @param aValue On completion, contains the requested TBool attribute.
        * @return void. Leaves on error:
        * - KErrArgument if requested attribute is unknown or not boolean
		*/
		IMPORT_C void GetBoolAttributeL( THttpDownloadAttrib aAttribute, 
		                                 TInt32& aMoIndex,
                                         TBool& aValue );
        
        /**
		* Gets the value for a string attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aMoIndex Identifies the media object index.
		* @param aValue On completion, contains the requested string. Use a 16-bit descriptor!
        * @return void. Leaves on error:
        * - KErrArgument if requested attribute is unknown or not string
        * - KErrNotFound if requested attribute is not initialized. This can make
        * easier to differentiate between that string is zero length or not allocated yet.
		*/
		IMPORT_C HBufC* GetStringAttributeL( THttpDownloadAttrib aAttribute, 
		                                     TInt32& aMoIndex,
                                             TBool& aDelete );
        
        /**
		* Gets the value for a string attribute.
		* @param aAttribute Identifies the attribute to be retrived.
		* @param aMoIndex Identifies the media object index.
		* @param aValue On completion, contains the requested string. Use a 16-bit descriptor!
        * @return void. Leaves on error:
        * - KErrArgument if requested attribute is unknown or not string
        * - KErrNotFound if requested attribute is not initialized. This can make it
        * easier to differentiate between that string is zero length or not allocated yet.
		*/
		IMPORT_C HBufC8* GetString8AttributeL( THttpDownloadAttrib aAttribute, 
		                                       TInt32& aMoIndex,
                                               TBool& aDelete );

		/**
		* Sets the value for a string attribute.
		* @param aAttribute Identifies the attribute to be set.
		* @param aMOIndex - media object index.
		* @param aValue The string value to be set. Must be a 16-bit descriptor!.
        * @return void. Leaves on error:
        * - KErrArgument if requested attribute is unknown or not string
		*/
        IMPORT_C void SetStringAttributeL( THttpDownloadAttrib aAttribute, const TInt32 aMOIndex,
								                  const TDesC16& aValue );

        /**
		* Sets the Download data (media info).
		*/
        IMPORT_C void SetDownloadDataAttributeL(HBufC8* dlData);
        
        /**
        * Sets the media object data (track info).
        */
        IMPORT_C void SetTrackDataAttributeL(TInt aIndex, HBufC8* dlData);

        /**
        * Detach the given client instance from the download.
        * @param aInstance client app instance to be detached
        * @return ETrue if download is still attached to another instance
        */                                
        IMPORT_C TBool DetachClientInstance( CHttpClientAppInstance* aInstance );

        /**
        * Return the client application where this download is created.
        * @since Series 60 v2.8
        * @return client application where this download is created
        */
        CHttpClientApp* ClientApp() const;

        /**
        * Returns the associated PD client application instance.
        * @since Series 60 v3.2
        * @return associated PD client application instance
        */
        CHttpClientAppInstance* PDClientAppInstance() const;

        /**
        * Returns the unique download id.
        * @since Series 60 v2.8
        * @return unique id of this download
        */
        TInt Id() const;

        /**
        * Returns the state of this download. 
        * @since Series 60 v2.8
        * @return state of this download
        */
        THttpDownloadState State() const;

        /**
        * Returns the progress state of the download.
        * @since Series 60 v2.8
        * @return progress state of this download
        */
        THttpProgressState ProgState() const;

        /**
        * Returns associated connection handler.
        * @since Series 60 v2.8
        * @return associated connection handler
        */
        CHttpConnHandler* ConnHandler() const;

        /**
        * Return EDlAttrNoMedia
        * @since Series 60 v2.8
        * @return ETrue if media is removed
        */
        TBool NoMedia() const;

        /**
        * Called when media card removed.
        * @since Series 60 v2.8
        * @return none.
        */
        void MediaRemoved( TUint aUid, TBool aDontCheckMediaUid = EFalse );

        /**
        * Called when media card inserted.
        * @since Series 60 v2.8
        * @return none.
        */
        void MediaInserted( TUint aUid );

        /**
        * Returns the current download destination drive id.
        * @since Series 60 v2.8
        * @return none.
        */
        TInt GetDestinationDriveID() const;
        
        /**
        * Associates download with the given client instance.
        * @since Series 60 v2.8
        * @param aInstance client instance this download to be associated with
        * @param aNoMoveEvent if ETrue move event disabled
        * @return none.
        */
        void SetClientInstance( CHttpClientAppInstance* aInstance,
                                TBool aNoMoveEvent = EFalse );

        
        /**
        * Associates download with the given connection handler
        * @since Series 60 v2.8
        * @param aConnHandler connection handler this download to be associated with
        * @return ?description
        */
        void SetConnHandler( CHttpConnHandler* aConnHandler );

        /**
        * Returns pausable state of the download
        */
        TBool Pausable() const;
        
        /**
        * Sets a new download name. Triggers EHttpProgDlNameChanged event.
        * @param aNewName new download name
        * @return none. Leaves on error.
        */
        void SetDownloadNameL( const TDesC& aNewName );

        void Ref() { ++iRefCount; }

        void Deref() { if( --iRefCount == 0 ) delete this; }
        
    public: // Functions from base classes

        /**
        * methods from MHTTPTransactionCallback
        */
        virtual void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);
        virtual TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction, const THTTPEvent& aEvent);

        /**
        * methods from CActive
        */
	    virtual void DoCancel();
	    virtual void RunL();
        virtual TInt RunError(TInt aError);
    
        /**
        * methods from MConnectionCallback
        */
        void Connected();
        void Suspended();
        void Disconnected();
        void ConnectionFailed( TInt aError );
        
    protected:  // New functions

        /**
        * Does real reset on download
        * @since Series 60 v2.8
        * @param aOnDelete if ETrue function called on delete
        * @return none.
        */
        void DoReset( TBool aOnDelete = EFalse );

        /**
        * Stores download info into file.
        * @since Series 60 v2.8
        * @return none. Leave on error.
        */
        void StoreDownloadInfoL();

        /**
        * Loads download info from file.
        * @since Series 60 v2.8
        * @return none. Leaves on failure.
        */
        void LoadDownloadInfoL();

        /**
        * Checks that (if known) that content
        * is still valid or expired.
        * @since Series Series60
        * @return ETrue if content is expired
        */
        TBool IsExpired();

        /**
        * Called if download completed.
        * @since Series 60 v2.8
        * @param ?arg1 ?description
        * @return ?description
        */
        void OnCompletedL();
        
       
       /**
        * Checks that if content type is of file storage type 
        * @return ETrue if content is of file storage type
        */
        TBool IsContentFileStorageType();
        
        /**
        * Reinitialize download. 
        * @since Series 60 v2.8
        */
        void ReInitializeDownload();

        /**
        * Cancels transaction.
        * @since Series 60 v2.8
        * @return none.
        */
        void CancelTransaction();

        /**
        * Pauses the download process.
        * @since Series 60 v2.8
        * @param aDisconnect if ETrue download disconnects
        * @return none. Leaves on error.
        */
        void InternalPauseL( TBool aDisconnect = EFalse );

        /**
        * Set download status and calls event callback function.
        * @since Series 60 v2.8
        * @param aProgState see THttpProgressState
        * @param aStatus see THttpDownloadState
        * @param aErrorId see THttpDownloadMgrError
        * @param aGlobalErrorId global error id
        * @return none.
        */
        void SetDownloadStatus( THttpProgressState aProgState,
                                THttpDownloadState aStatus = EHttpDlInprogress,
                                THttpDownloadMgrError aErrorId = ENoError,
                                TInt aGlobalErrorId = KErrUnknown );

        /**
        * Called when response header received. 
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        void ResponseHeaderReceivedL();

        /**
        * Called when a new body chunk received.
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        void ResponseBodyReceivedL( const TDesC8& aBuf );

        /**
        * Start connection creation process.
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        void Connect();

        /**
        * Disconnects download from the network.
        * @since Series 60 v2.8
        * @return none.
        */
        void Disconnect();

        /**
        * Requesting header.
        * @param aStringPool stringpool of HTTP session
        * @param aHeaders request header of the transaction
        * @param ETrue if HEAD method used.
        * @return none. Leaves on error.
        */
        void SetRequestHeaderL( RStringPool& aStringPool,
                                RHTTPHeaders& aHeaders,
                                TBool aHeadMethod );

        /**
        * Disables pipelining of the transaction.
        * @since Series 60 v2.8
        * @param aStringPool stringpool of HTTP session
        * @return none. Leaves on error.
        */
        void DisablePipeliningL( RStringPool& aStringPool );

        /**
        * Sets transaction property.
        * @param aStringPool stringpool of HTTP session
        * @param aPropertyName name of the property
        * @param aToken property value
        * @return none. Leaves on error.
        */
        void SetPropertyL( RStringPool& aStringPool,
                           RStringF aPropertyName, 
                           const TDesC8& aToken );

        /**
        * Sets transaction property.
        * @param aPropertyName name of the property
        * @param aValue property value
        * @return none. Leaves on error.
        */
        void SetPropertyL( RStringF aPropertyName, 
                           const TInt aValue );

        /**
        * Add client app's own fields to request header
        * @since Series 60 v2.8
        * @param aStringPool stringpool of HTTP session
        * @param aHeaders request header of the transaction
        * @return none. Leaves of error
        */
        void SetRequestHeaderAddOnL( RStringPool& aStringPool,
                                     RHTTPHeaders& aHeaders );
        /**
        * Set credential properties of transaction
        * @since Series 60 v2.8
        * @param aStringPool stringpool of HTTP session
        * @return none. Leaves of error
        */
        void SetCredentialsInfoL( RStringPool& aStringPool );

        /**
        * Set 'Range' field in request header.
        * @since Series 60 v2.8
        * @param aStringPool stringpool of HTTP session
        * @param aHeaders request header of the transaction
        * @return none. Leaves of error
        */
        void SetRangeFieldL( RStringPool& aStringPool,
                             RHTTPHeaders& aHeaders );

        /**
        * Sets the given header field with the expire value
        * @since Series 60 v2.8
        * @param aField header field to be set
        * @param aHeaders request header of the transaction
        * @return ?description
        */
        void SetExpireToFieldL( RStringF& aField, 
                                RStringPool& aStringPool,
                                RHTTPHeaders& aHeaders );

        /**
        * Rebuilds the url with scheme and port
        * @since Series 60 v2.8
        * @return none. Leaves of error
        */
        void ParseRequestedUrlL();

        /**
        * Creates the download name from the url
        * @since Series 60 v2.8
        * @return none. Leaves of error
        */
        void ParseDownloadNameL();

        /**
        * Store entire response header with raw data in iHeaders array
        * @since Series 60 v2.8
        * @return none. Leaves of error
        */
        void StoreResponseHeaderL();

        /**
        * Saves credentials info to memory for next submit.
        * Credential info are deleted in destructor and never
        * persisted.
        * @since Series 60 v2.8
        * @param aStringPool stringpool of HTTP session
        * @return none. Leaves of error
        */
        void SaveCredentialsL( RStringPool aStringPool );

        /**
        * Builds up the request and submits the transaction.
        * @since Series 60 v2.8
        * @return none. Leaves of error
        */
        void RequestContentL();

    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CHttpDownload( CHttpClientApp *aClientApp,
                       TInt32 aId,
                       CHttpClientAppInstance* aClientAppInstance );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8& aUrl );

        // Prohibit copy constructor if not deriving from CBase.
        // CHttpDownload( const CHttpDownload& );
        // Prohibit assigment operator if not deriving from CBase.
        // CHttpDownload& operator=( const CHttpDownload& );

        /**
        * Called when content type received in response header.
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        void ContentTypeReceivedL();

        /**
        * Handles HTTP error 401
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        void HttpResponse401L();

        /**
        * Handles HTTP error 407
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        void HttpResponse407L();

        /**
        * Search for and return a field in response header
        * @since Series 60 v2.8
        * @param aFieldName name of the field
        * @return index to the field in iHeaders or KErrNotFound
        */
        TInt FindHeaderField( CArrayPtrFlat<CHeaderField>* aHeaders,
                              const TDesC8& aFieldName ) const;

        /**
        * Converts download name to unique one.
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        void ConvertDownloadNameUniqueL();

        /**
        * Creates a unique, indexed name
        * @since Series 60 v2.8
        * @return none. Leaves on error.
        */
        void CreateIndexedNameL( HBufC* &aUniqueName, 
                                 TDesC& aOrgName, 
                                 TInt& aIndex );

        /**
        * Stores response header received by client application.
        * It is used only in case of continue download.
        * @param aResponseHeader see EDlAttrResponseHeader
        * @return none. Leaves on error.
        */
        void ContinueDownloadStoreResponseHeaderL( 
                                                const TDesC8& aResponseHeader );

        /**
        * Parses content type and media type from the response header.
        * @return none. Leaves on error.
        */
        void ParseContentTypeL( RStringPool& aStrPool );

        /**
        * Parses content disposition from the response header.
        * @return none. Leaves on error.
        */
        void ParseContentDispositionL( RStringPool& aStrPool );

        /**
        * Called mainly from SetDownloadStatus. This function passes
        * the download statuses to the event handler
        * @since Series 60 v2.8
        * @return none.
        */
        void TriggerEvent( THttpDownloadState aDlState,
                           THttpProgressState aProgState = EHttpProgNone );

        /**
        * Checks that in current situation the download is pausable, or not.
        * Triggers event to client if pausable attribute changes.
        * @since Series 60 v2.8
        * @return none.
        */
        void UpdatePausable();

        /**
        * Do some (re)initialization before a forced restart.
        * @since Series 60 v2.8
        * @return none.
        */
        void ForcedRestartL();

        /**
        * Returns the raw data of the requested header field.
        * @since Series 60 v2.8
        * @return HBufC8* buffer with raw data of header field.
        */
        HBufC8* HeaderFieldL( CArrayPtrFlat<CHeaderField>* aHeaders, 
                              TInt aFieldIndex );

#ifdef DEAD_CODE
        /**
        * Checks the response header on 206 response, if the
        * content is really what we requested and hasn't changed
        * since last request.
        * Checked field: date
        * @return ETrue if content is probably not modified
        */
        TBool CheckIfContentUnModified();
#endif

        /**
        * Checks the max length of the string attribute
        * since last request.
        * Checked field: date
        * @return none.
        */
        void CheckAttribMaxLengthL( THttpDownloadAttrib aAttribute, 
	                                const TDesC16& aValue );

        /**
        * Checks the max length of the string attribute
        * since last request.
        * Checked field: date
        * @return none.
        */
        void CheckAttribMaxLengthL( THttpDownloadAttrib aAttribute, 
	                                const TDesC8& aValue );

        /**
        * Set cookie transaction property
        * @param aStringPool stringpool of HTTP session
        * @return none. Leaves of error.
        */
        void SetCookiesL( RStringPool& aStringPool );

        /**
        * Client side transaction completed.
        */
        void DownloadSucceededL();

        /**
        * Parses input request header add-on and stores it in request headers array.
        * @param aValue Request header add-on from client application
        * @return None. Leaves on error.
        */
        void ParseRequestHeaderAddOnL( const TDesC8& aValue );

        /**
        * Load headers from file
        */
        void LoadHeadersL( RFile& aFile, CArrayPtrFlat<CHeaderField>* aHeaders );

        /**
        * Append headers to buffer
        */
        void AppendHeadersL( TPtr8& aBuf, CArrayPtrFlat<CHeaderField>* aHeaders );

        /**
        * Get the size of Http headers
        */
        TInt GetHttpHeadersSize(CArrayPtrFlat<CHeaderField>* aHeaders );
        
        /**
        * Add the given header in the header array
        * @return None. Leaves on error.
        */
        void AddHeaderL( THttpDownloadAttrib aAttribute, 
			             const TDesC8& aValue,
                         const TInt aConversionTable[][2],
                         CArrayPtrFlat<CHeaderField>* aHeaders );

        /**
        * Handles permanent redirection event
        * @param aNewUrl new URL after redirection
        * @return None. Leaves on error.
        */
        void RedirectedPermanentlyL( const TDesC8& aNewUrl );
        
        /**
        * Handles temporary redirection event
        * @param aNewUrl new URL after redirection
        * @return None. Leaves on error.
        */
        void RedirectedTemporaryL( const TDesC8& aNewUrl );
        
        /**
        * Check that the content is supported multipart
        * @param aBuf new chunk of message body
        * @return TBool. Leaves on error.
        */        
        TBool IsMultipartSupportedL( const TDesC8& aBuf );
        
        /**
        * Gets the specified attribute from media type
        * @param aAttribute Required attribute
        * @return TPtrC8. Leaves on error - KErrNotFound.
        */                
        TPtrC8 GetParamFromMediaTypeL( const TDesC8& aAttribute );
        
        /**
        * Escape decoded URL may contain characters that cannot
        * be used to create filename. These characters are replaced
        * with KUrlFixChar
        * @return none. Leaves on error.
        */
        void FixDownloadNameL();
        
        /**
        * Set iCodDownload = aValue
        */       
        void SetCodFlag( TBool aValue );
        
        
        /**
        * Check the contenttype and returns TRUE if drm
        */         
        void CheckRealDRMContentType();
        
        /**
        * Self completes the active object
        */
        void SelfComplete( TInt aReason = KErrNone );
        
        /**
        *check for correct drive to move from cenrep
        */
        void UpdateDestFileNameL();

        /**
        * notify DcfRepository for new/deleted files
        */
        void UpdateDCFRepositoryL( const TDesC& aFileName );
        
        /**
        * Move completed media object files to gallery when delete is called
        * @param aMediaObjectIndex media object index to be moved
        * @return error code
        */
        TInt MoveInDelete(TInt aMediaObjectIndex);

        /**
        * Initiates a move for downloaded media object files to gallery/download folder
        * @param aMediaObjectIndex media object index to be moved
        */
        void MoveDownloadedMediaObjectL(TInt aMediaObjectIndex);
        
        /**
        * Moves downloaded media object file to gallery/download folder
        * @param aMediaObjectIndex media object index to be moved
        */
        void MoveDownloadedMediaObjectSyncL(TInt aMediaObjectIndex);
        
        /**
        * Calls ConvertDownloadNameUniqueL to create unique name based on source file name
        * and destination path. 
        */
        void FindUniqueDestinationFileNameL( TDesC& srcFile, HBufC*& destPath );

        /**
        * Notifies gallery about new file
        * @param aFileName file name
        */        
        void CHttpDownload::NotifyMediaGalleryL( const TDesC& aFileName );
        
        /**
        * ConvertDownloadNameUniqueL
        */        
        void CHttpDownload::ConvertDownloadNameUniqueL( HBufC*& filePath,
                                            			HBufC*& fileName,
                                            			HBufC*& fileExtn);
        
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data

        TInt32  iId;    // unique id of the download
        CHttpClientApp* iClientApp;
        CHttpClientAppInstance* iClAppInstance;    // pointer to the client app's 
                                            // preferencies class
		CHttpClientAppInstance* iPDClAppInstance;
        TInt32   iUserData;     // EDlAttrUserData

        HBufC8* iUrl;           // EDlAttrReqUrl
        HBufC8* iRedirUrl;      // EDlAttrRedirUlr
        HBufC8* iCurrentUrl;    // EDlAttrCurrentUrl
        HBufC*  iDlName;        // EDlAttrName
        TInt32  iPort;          // EDlAttrPort
        TInt    iTargetApp;     // EDlAttrTargetApp
        THttpMethod iMethod;    // EDlAttrMethod

        TInt    iAuthScheme;    // EDlAttrAuthScheme
        HBufC8* iHttpRealm;     // EDlAttrRealm
        HBufC8* iHttpUsername;  // EDlAttrUsername
        HBufC8* iHttpPassword;  // EDlAttrPassword
        HBufC8* iHttpProxyRealm;// EDlAttrProxyRealm
        HBufC8* iHttpProxyUsername;  // EDlAttrProxyUsername
        HBufC8* iHttpProxyPassword;  // EDlAttrProxyPassword
        HBufC8* iHttpNonce;     // Nonce-value used in digest authentication
                                // Received from HTTP server, non-accessible from
                                // outside

        TInt32  iPreferencies;

        TBool   iDisconnectOnPause;     // EDlAttrDisconnectOnPause
        TBool   iDisconnectOnReset;     // EDlAttrDisconnectOnReset
        TInt    iCodDownload;           // See EDlCodDownload this is a bool attrib but needs to increment!
        TInt32  iFotaPckgId;            // EDlAttrFotaPckgId

        CArrayPtrFlat<CHeaderField>* iResponseHeaders;  // Response header
        CArrayPtrFlat<CHeaderField>* iRequestHeaders;   // Additional request header
                                                        // See ERequestHeaderAddon
        CArrayPtrFlat<CHeaderField>* iEntityHeaders;
        CArrayPtrFlat<CHeaderField>* iGeneralHeaders;

        // the following members are initialized from
        // HTTP response header.
        TInt32      iStatusCode;        // EDlAttrStatusCode
        HBufC8*     iContentType;       // EDlAttrContentType
        HBufC8*     iDispositionType;         // EDlAttrDispositionType (e.g.:"inline", "attachment")
        HBufC*		iAttachmentFileName;      // EDlAttrFileNameParm
        HBufC8*     iMediaType;         // EDlAttrMediaType
        TDateTime   iDate;
        TDateTime   iExpires;
        TTimeIntervalSeconds    iMaxAge;

        CHttpConnHandler* iConnHandler; // pointer to the CHttpConnHandler.
                                        // Not owned.
        RHTTPTransaction    iTrans;
        TBool               iTransValid;// iTrans is a valid transaction

        THttpDownloadState  iDlState;       // EDlAttrState
        THttpProgressState  iProgState;     // EDlAttrProgressState

        THttpDownloadMgrAction  iAction;    // EDlAttrAction
        THttpRestartActions iRestartAction; // EDlAttrRestartAction
        
        TInt    iLastError;             // EDlAttrErrorId
        TInt    iGlobalErrorId;         // EDlAttrGlobalErrorId

        TBool   iNoContentTypeCheck;    // EDlAttrNoContentTypeCheck

        TInt    iDontFireEvent;

        CFileMan*   iFileMan;           // used in MoveL

        TBool   iNoMedia;               // See EDlAttrNoMedia
        TBool   iContinueDownload;      // See EDlContinue

        CHttpStorage*   iStorage;       // Stores received body data
        
        HBufC8*  iHashedMsgBody;        // EDlAttrHashedMsgBody

		TBool	iUseAttachmentFileName;	// Content disposition type = "attachment"
		TBool	iUseInlineFileName;		// Content disposition type = "inline"

		TBool   iCodPdAvailable;		// COD Progerssive download is available

        TBool   iUpdatedDDUriSet;       //True if updated DD URI is set from COD Eng
        HBufC8* iDDType;                // EDlAttrDDType
        
        CDownloadDataServ* iCodDlData;// COD download info
        
        TFileName iFname;               ///< File name.

    private:    // Data

        TBool   iNoRealError;           // It's set in Get/SetAttrib functions. Indicates
                                        // that if a leave occures it doesn't have any
                                        // real effect on download process and nothing
                                        // to do in the OnError()
        TBool   iRedirect;              // Transaction redirected.
                                        // Restore iCurrentUrl to iUrl after transaction abnormally
                                        // completed (paused, failed)
        TBool   iPausable;              // EDlAttrPausable 

        TBool   iDlStartedByClient;     // Download started by client app via ::StartL.
                                        // In some cases the download is restarted by the engine.
        TBool   iSilentMode;            // EDlAttrSilent
        TBool   iHidden;                // EDlAttrHidden

        TBool   iUseCookies;
        
        TBool   iPausableDRM;           // Set to ETrue if KDRMOldContentType is found in response header.
        TBool   iDrmContentLengthValid; //
      
       
        HBufC8* iHeaderOfMultipart;     // Header of multipart
        TBool   iMultiPart;

        TBool   iDlNameChanged;         //Set if download name has changed        
        TBool   iContTypeRecognitionAvailSent;

        HBufC8* iDownloadInfo;          // contains download info

        TInt    iRefCount;
        
        TBool    iMoveInProgress;
        
        TInt    iMoLength;              //Multiple Media Object Length
        TInt    iActiveDownload;        // Currenlty active Cod Download
        TInt    iActivePlayedDownload;  // Currenlty actively played Cod Download
        TInt	iMOMoved;				// index for bulk move
        TBool   iMoDownloadCompleted;   //MO download Completed.But Move will be issued by PD Client
        
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes

        friend class CHttpStorage;

    private:    // Friend classes
        //?friend_class_declaration;
    };

#endif      // HTTPDOWNLOAD_H   
            
// End of File
