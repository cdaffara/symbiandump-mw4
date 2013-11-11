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
*      Declaration of class CHttpLoader.   
*      
*
*/


#ifndef HTTP_LOADER_H
#define HTTP_LOADER_H

// INCLUDES

#include <e32base.h>
#include <http.h>
#include <stringpool.h>
#include <http/mhttpauthenticationcallback.h>
#include "CodLoadObserver.h"
#include "HttpSessionBase.h"
#include "RFeatMgr.h"

// FORWARD DECLARATION

class MCodSaverFactory;
class CCodSaver;
class CConnection;
class CTimeout;
class TCodProgress;
class CCodEngBase;
class CHeaderField;

// CLASS DECLARATION

/**
* HTTP Loader.
*/
NONSHARABLE_CLASS( CHttpLoader )
: public CActive,
  public MHTTPTransactionCallback,
  public MHTTPDataSupplier,
  public MHTTPAuthenticationCallback
    {
    public:     // Constructors and destructor

        /**
        * Two phased constructor. Leaves on failure.
        * @param aConnection Connection.
        * @param aCodLoadObserver Observer or NULL.
        * @param aProgress Progress or NULL.
        * @return The created loader.
        */      
        static CHttpLoader* NewL
            (
            CConnection& aConnection,
            MCodLoadObserver* aCodLoadObserver,
            TCodProgress* aProgress,
            CCodEngBase* aCodEngBase
            );

        /**
        * Destructor.
        */
        virtual ~CHttpLoader();

    public:     // new methods

        /**
        * Issue load.
        * @param aUrl Request URL.
        * @param aSaverFactory Saver factory. Ownership not taken.
        * @param aStatus Completes when done.
        */
        void LoadL
            (
            const TDesC8& aUrl,
            MCodSaverFactory& aSaverFactory,
            TRequestStatus* aStatus
            );

        /**
        * Send status notification.
        * @param aUrl Request URL.
        * @param aNotifyBody Notify body to be sent.
        * @param aStatus Completes when done.
        */
        void NotifyL
            (
            const TDesC8& aUrl,
            const TDesC8& aNotifyBody,
            TRequestStatus* aStatus
            );

        /**
        * Set or unset observer.
        * @param aCodLoadObserver Observer to set (pass NULL to unset).
        */
        inline void SetObserver( MCodLoadObserver* aCodLoadObserver );
        
        /**
        * Load headers from info file
        * @param info file handle
        * @Added in OMA pause-resume feature.
        */
        void LoadHeadersL( RFile& aFile );
		
		/**
		* Pause the HTTP transaction
		*/        
        void Pause();
        
        /**
		* Get Response Headers
		*/
        CArrayPtrFlat<CHeaderField>* ResponseHeaders();

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

    protected:  // from MHTTPTransactionCallback

        /**
        * Handle transaction event.
        * @param aTransaction The transaction that the event has occurred on.
        * @param aEvent The event that has occurred.
        */
        virtual void MHFRunL
            ( RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

        /**
        * Handle errors occured in MHFRunL().
        * @param aError The leave code that MHFRunL left with.
        * @param aTransaction The transaction that was being processed.
        * @param aEvent The Event that was being processed.
        * @return KErrNone.
        */
        virtual TInt MHFRunError
            (
            TInt aError,
            RHTTPTransaction aTransaction,
            const THTTPEvent& aEvent
            );

    protected:  // from MHTTPDataSupplier

        /**
        * Obtain a data part. The data is guaranteed
        * to survive until a call is made to ReleaseData().
        * @param aDataPart The data part.
        * @return ETrue if this is the last part. EFalse otherwise.
        */
        virtual TBool GetNextDataPart( TPtrC8& aDataPart );

        /**
        * Release the current data part being held.
        */
        virtual void ReleaseData();

        /**
        * Obtain the overall size of the data being supplied.
        * @return Data size in bytes.
        */
        virtual TInt OverallDataSize();

        /**
        * Reset the data supplier.
        * @return KErrNone.
        */
        virtual TInt Reset();

    protected:  // from MHTTPAuthenticationCallback

        /** Get credentials.
        * @param aURI The URI being requested.
        * @param aRealm The realm being requested.
        * @param aAuthenticationType The type of authentication (basic or digest).
        * @param aUsername The username is returned here. The caller will close
        * the returned string.
        * @param aPassword The password is returned here. The caller will close
        * the returned string.
        * @return True if credentials have been obtained.
        */
	    virtual TBool GetCredentialsL
            (
            const TUriC8& aURI,
            RString aRealm, 
            RStringF aAuthenticationType,
            RString& aUsername, 
            RString& aPassword
            );

    private:    // types

        enum TState         ///< State.
            {
            EInit,          ///< Initial state.
            EStart,         ///< Starting operation.
            EOpen,          ///< Opening session.
            ERequest        ///< HTTP request.
            };

    private:    // Constructors and destructor

        /**
        * Constructor.
        * @param aConnection Connection.
        * @param aCodLoadObserver Observer or NULL.
        * @param aProgress Progress or NULL.
        */      
        CHttpLoader
            (
            CConnection& aConnection,
            MCodLoadObserver* aCodLoadObserver,
            TCodProgress* aProgress ,
            CCodEngBase* aCodEng 
            );

        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();

    protected:    // state machine parts

        /**
        * Open session.
        */
        void OpenSessionL();

        /**
        * Make the HTTP request.
        */
        void RequestL();

        /**
        * Clean up and notify parent.
        */
        void Done();

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
        * Create transaction, set request headers (and body for POST request).
        */
        void CreateTransactionL();

        /**
        * Submit the transaction (make the HTTP request).
        */
        void SubmitTransactionL();

        /**
        * Close the transaction and complete with aError.
        */
        void CompleteTransaction( TInt aError );
        

        /**
        * Close the transaction and complete with aError.
        */
        void RestartTransaction();
    
        /**
        * Handle response from server.
        * @param aResponse Response from server.
        */
        void HandleResponseHeadersL( RHTTPResponse aResponse );

        /**
        * HTTP Version Not Supported. Resubmit transaction using HTTP/1.0.
        * @return ETrue if transaction was resubmitted, EFalse if not (i.e
        * we already use HTTP/1.0).
        */
        TBool VersionRetryL();

        /**
        * 305 Use Proxy. Set proxy address and resubmit transaction - this is
        * a redirect case which the redirect filter cannot do for us.
        * Proxy is set only if currently no proxy is used.
        * @return ETrue if transaction was resubmitted, EFalse if not.
        */
        TBool UseProxyL();

        /**
        * Setup the session (session properties, filters).
        */
        void SetupSessionL();

        /**
        * Start/restart timeout timer.
        */
        void StartTimeout();

        /**
        * Handle timeout (fail or retry request).
        */
        void Timeout();

        /**
        * Handle install-notify "timeout": checks if network is still available.
        * if still available, assume that install-notify POST was submitted by
        * network layer and make downloaded content available to user.
        */
        void INTimeout();

        /**
        * Increment progress.
        * @param aBytes Bytes received.
        */
        void IncProgressL( TInt aBytes );
        
       /**
        * Checks if accept range is supported.
        * @Added in OMA pause-resume feature
        */        
        TBool AcceptRangesSupported();

    private:    // helpers

        /**
        * Helper function to set a header.
        * @param aHeaders Set the header to this header set.
        * @param aHdrField Header field name.
        * @param aHdrValue Header value.
        */
        void SetHeaderL
            (
            RHTTPHeaders aHeaders,
            HTTP::TStrings aHdrField,
            const TDesC8& aHdrValue
            );

        /**
        * Helper function to set a header.
        * @param aHeaders Set the header to this header set.
        * @param aHdrField Header field name.
        * @param aHdrValue Header value.
        */
        void SetHeaderL
            (
            RHTTPHeaders aHeaders,
            HTTP::TStrings aHdrField,
            HTTP::TStrings aHdrValue
            );

        /**
        * Helper function to set a header.
        * @param aHeaders Set the header to this header set.
        * @param aHdrField Header field name.
        * @param aHdrValue Header value.
        */
        void SetHeaderL
            (
            RHTTPHeaders aHeaders,
            HTTP::TStrings aHdrField,
            const RStringF aHdrValue
            );

        /**
        * Helper function to get the Content-Type header value.
        * @param aHeaders HTTP header set.
        * @return Content-Type header value.
        */
        const TDesC8& GetContentTypeL( RHTTPHeaders aHeaders );

        /**
        * Returns content type list.
        * @Added in OMA pause-resume feature
        */           
        const TDesC8& GetContentTypeL();

        /**
        * Helper function to get the source URI of a transaction.
        * @param aTransaction Transaction.
        * @return Source URI.
        */
        const TDesC8& GetSourceUriL( RHTTPTransaction aTransaction );

        /**
        * Get a shared string from the session stringpool.
        * @param aId Id of string.
        * @return Shared string from HTTP string table. No need to close.
        */
        inline RStringF StringF( HTTP::TStrings aId );

        /**
        * Static wrapper around the timeout function.
        * @param aPtr This object as TAny*
        * @return EFalse.
        */
        static TInt StaticTimeout( TAny* aPtr );

        /**
        * Static wrapper around the install-notify timeout function.
        * @param aPtr This object as TAny*
        * @return EFalse.
        */
        static TInt INStaticTimeout( TAny* aPtr );

        /**
        * Check status of HTTP network session and Access Point
        * @return ETrue if network available, or EFalse if not.
        */
        TBool IsConnectionActive();

		/**
        * Store entire response header with raw data in iHeaders array
        * @since Series 60 v2.8
        * @return none. Leaves of error
        */
        void StoreResponseHeaderL();

		/**
        * Appends the header finr to a buffer
        * @return none. Leaves of error
        * @Added in OMA pause-resume feature
        */        
		void AppendHeadersL( TPtr8& aBuf,
         		             CArrayPtrFlat<CHeaderField>* aHeaders );
        /**
        * Sets the range field in request headers
        * @return none. Leaves of error
        * @Added in OMA pause-resume feature
        */                 		             
		void SetRangeFieldL( RStringPool& aStringPool,
                                    RHTTPHeaders& aHeaders );	

        /**
        * Searches for a given header in header list
        * @return KErrNotFound if not found, header index otherwise
        * @Added in OMA pause-resume feature
        */        
        TInt FindHeaderField( CArrayPtrFlat<CHeaderField>* aHeaders,
                                     const TDesC8& aFieldName ) const;


        /**
        * Checks if the content is DRM content
        * @return none
        * @Added in OMA pause-resume feature
        */        
        void CheckRealDRMContentTypeL();

        /**
        * Update the pausability of download
        * @return none
        * @Added in OMA pause-resume feature
        */        
        void UpdatePausableL();
		
		/**
        * Sets the specified request header
        * @return none
        * @Added in OMA pause-resume feature
        */        
        void SetRequestHeaderL( RStringPool& aStringPool, RHTTPHeaders& aHeaders);
        
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

    private:    // debug utilities

        /**
        * In debug build, log headers. In release build, do nothing.
        * @param aHeaders Headers to log.
        */
        void LogHeaders( RHTTPHeaders aHeaders );

    private:    // data

        HBufC8* iUri;                       ///< Request URI or NULL. Owned.
        HTTP::TStrings iMethod;             ///< Request method.
        HTTP::TStrings iHttpVersion;        ///< HTTP version.
        HBufC8* iNotifyBody;                ///< Notify body or NULL. Owned.
        CHttpSessionBase* iSess;            ///< HTTP session. Owned.
        RHTTPTransaction iTrans;            ///< Transaction. Owned.
        CConnection& iConn;                 ///< Connection. Not owned.

        MCodSaverFactory* iSaverFactory;    ///< Saver factory. Not owned.
        CCodSaver* iSaver;                  ///< Saver. Not owned.
        CTimeout* iTimeout;                 ///< Timeout timer. Owned.
        MCodLoadObserver* iCodLoadObserver; ///< Observer or NULL.
        TRequestStatus* iParentStatus;      ///< Parent status.
        TState iState;                      ///< Current state.
        TInt iResult;                       ///< Resulting error code.
        TBool iSuppressErrors;              ///< Suppress errors.
        TBool iProxySet;                    ///< ETrue if we have set a proxy.
        RFeatMgr iFeatMgr;                  ///< Feature Manager.
        TCodProgress* iProgress;            ///< Progress or NULL.
        TInt iRetry;                        ///< Retry count (incl. first).
        CTimeout* iINTimeout;               ///< Timeout timer for sending install-notify. Owned.
        CCodEngBase* iCodEng;
        HBufC8* iDownloadInfo;          				// contains download info
        TBuf<KMaxPath>   iFolderBuf;					//Folder for cod download
        CArrayPtrFlat<CHeaderField>* iResponseHeaders;  // Response header
        TDateTime   iDate;
        TDateTime   iExpires;
        TTimeIntervalSeconds    iMaxAge;

        // Set to ETrue if KDRMOldContentType is found in response header.
        TBool iPausableDRM;
        TBool   iDrmContentLengthValid;        
        
        // Redirection
        TBool iRedirect;
    };

#include "HttpLoader.inl"

#endif /*def HTTP_LOADER_H */
