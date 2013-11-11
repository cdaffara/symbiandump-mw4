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
* Description:  Loads the plugin content
*
*/

#ifndef __PLUGINLOADER_H__
#define __PLUGINLOADER_H__

//  INCLUDES
#include <e32base.h>
#include "ContentListener.h"
#include "PluginWin.h"


// FORWARD DECLARATIONS
class CPluginLoadData;
class CUrlResponseInfo;
class CUrlResponseHeaderInfo;
class CPluginSkin;
class CWebKitFrame;
class MContentLoaderInterface;

/**
*  This is a plugin content loader class. It handles loading plugin content
*  @lib plugin.dll
*  @since 3.1
*/
class CPluginLoader: public CBase, 
                     public MContentListener                     
    {

    public:
        /**
        * NewL
        * Two phase constructor
        * @param aPluginSkin The plugin skin object that creates the loader
        * @param aWebKitBridge The webkitbridge
        * @param aBaseUrl base url from which the plugin is loaded
        * @return A newly created CPluginLoader object pointer
        *
        */
        static CPluginLoader* NewL( CPluginSkin& aPluginSkin,
                                    CWebKitFrame& aWebKitFrame,
                                    const TDesC8& aBaseUrl  );
        /**
        * ~CPluginLoader
        * Destructor
        *
        */
        virtual ~CPluginLoader();

        /**
        * LoadPluginContentL
        * Load the plugin content from the given URL
        * @since 3.1
        * @param aUrl url to load the plugin content from
        * @return void
        */
        void LoadPluginContentL( const TDesC8& aUrl, TPluginLoadMode aLoadMode );

        /**
        * LoadPluginContentL
        * Load the plugin content from the given URL
        * @since 3.1
        * @param aUrl url to load the plugin content from
        * @param aMethod url to load the plugin content from
        * @param aLoadMode url to load the plugin content from
        * @param aBody url to load the plugin content from
        * @param aTrId url to load the plugin content from
        * @return void
        */
        void LoadPluginContentL( const TDesC8& aUrl,
                                 TUrlLoadMethod aMethod,
                                 TPluginLoadMode aLoadMode,
                                 const TPtr8& aBody, TInt &aTrId);

        /**
        * CancelAllTransactions
        * Cancels all plugin transactions
        * @since 3.1
        * @return void
        */
        void CancelAllTransactions();

    public: // Methods From CUrlLoadListener

        /**
        * HeadersL
        * Invoked when headers arrive
        * @since 3.1
        * @param
        * @return KErrNotSupported if it is not a native browser content
        */
        TInt HeadersL( TInt aTransactionId, CUrlResponseInfo& aResponse );

        /**
        * ResponseL
        * Creates a PluginInst in response to first chunk and creates and writes to a stream
        * to the plugin. On subsequent chunks it just writes to the stream
        * @since 3.1
        * @param
        * @return void
        */
        void ResponseL( TInt aTransactionId, CUrlResponseInfo& aResponse );

        /**
        * Complete
        * Closes the stream to the plugin since response is complete
        * @since 3.1
        * @param aTransactionId Transaction ID for the load request
        * @param aError Error code for the transaction
        * @return void
        */
        void Complete( TInt aTransactionId, CUrlResponseInfo& aResponse, TInt aError );

        /**
        * HandleError
        * Handles any error
        * @since 3.1
        * @param aTransactionId Transaction ID for the load request
        * @param aError Error ID for the load request
        * @return void
        */
        void HandleError( TInt aTransactionId, TInt aError );

        /**
        * GetTransactionId
        * Return transaction Id, using the request url
        * @param aRequestUrl the request url
        * @return TInt the transaction Id
        */
        TInt GetTransactionId( const TDesC8& aRequestUrl );

        /**
        * GetCompleteError
        * Return the complete error using TrId as the index into the
        * PluginLoadDataArray
        * @param aTrId the transaction Id
        * @return TInt the completion error
        */
        TInt GetCompleteError( TInt aTrId );

    protected:

        /**
        * CPluginLoader
        * Constructor
        *
        */
        CPluginLoader( CPluginSkin& aPluginSkin, CWebKitFrame& aWebKitFrame );

        /**
        * ConstructL
        * Constructor
        *
        */
        void ConstructL(const TDesC8& aBaseUrl);

        /**
        * GetLoadData
        * Gets the load mode corresponding to load requests transId
        * @param aTrId the transaction Id
        * @return load data for a given transaction ID
        */
        CPluginLoadData* GetPluginLoadData(TInt aTrId);

        /**
        * GetLoadMode
        * Gets the load mode corresponding to load requests transId
        * @param aTrId the transaction Id
        * @return load mode for a given transaction ID
        */
        TPluginLoadMode CPluginLoader::GetLoadMode(TInt aTrId);

        /**
        * SaveCompleteError
        * Save the complete error using TrId as the index into the
        * PluginLoadDataArray
        * @param aTrId the transaction Id
        * @param aError the completion error
        * @return true if successfully saved
        */
        TBool SaveCompleteError( TInt aTrId, TInt aError );

        /**
        * SaveResponseHeader
        * Save the response header using TrId as the index into the
        * PluginLoadDataArray
        * @param aTrId the transaction Id
        * @param aResponseHeader the response header structure
        * @return true if successfully saved
        */
        TBool SaveResponseHeader( TInt aTrId, CUrlResponseHeaderInfo& aResponseHeader );

        /**
        * RemovePluginLoadData
        *
        * @return
        */
        void RemovePluginLoadData( TInt aTransactionId );

    private:

        //Pointer to the Webkitbridge; not owned
        CWebKitFrame*   iWebKitFrame;
        //Pointer to the plugin skin; not owned
        CPluginSkin*   iPluginSkin;
        //Base url of the plugin object; owned
        HBufC8*         iBaseUrl;
        //Array of Plugin load data
        CArrayFixFlat<CPluginLoadData>* iPluginLoadDataArray;
    };

/**
*  This is a plugin data holder class.
*  It maps the transaction to the load mode of the load request
*  An array of this datastructure is maintained by CPluginLoader
*  It is used for tracking load requests from plugin engines
*  @lib plugin.dll
*  @since 3.1
*/
class CPluginLoadData : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPluginLoadData* NewL(TInt aTrId);

        /**
        * Destructor.
        */
        ~CPluginLoadData();

    private:  // Constructors

        /**
        * C++ default constructor.
        */
        CPluginLoadData(TInt aTrId);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:  // New functions

        /**
        * TransactionId
        * Get Transaction Id
        * @since 3.1
        * @return TInt TransactionId
        */
        TInt TransactionId() { return iTransId; }

        /**
        * GetHttpStatus
        * Get HTTP status
        * @since 3.1
        * @return TInt iHttpStatus
        */
        TInt HttpStatus() { return iHttpStatus; }

        /**
        * SetHttpStatus
        * Set HTTP status
        * @since 3.1
        * @param aHttpStatus
        * @return void
        */
        void SetHttpStatus(TInt aHttpStatus);

        /**
        * SetLoadMode
        * Save the load mode.
        * @since 3.1
        * @param aLoadMode the enum TPluginLoadMode load mode
        * @return void
        */
        void SetLoadMode(TPluginLoadMode aLoadMode);

        /**
        * LoadMode
        * Return the load mode
        * @since 3.1
        * @param aTrId the transaction Id
        * @return TPluginLoadMode
        */
        TPluginLoadMode LoadMode() { return iLoadMode; }

        /**
        * SetCompleteError
        * Save completion "error/reason" code from response complete
        * @since 3.1
        * @param aCompleteError the completion error code
        * @return void
        */
        void SetCompleteError(TInt aCompleteError);

        /**
        * CompleteError
        * Get completion "error/reason" code from response complete
        * @since 3.1
        * @return TInt completion error code
        */
        TInt CompleteError() { return iCompleteError; }

        /**
        * SetRequestUrlL
        * Save request url. We take ownership of the url.
        * @since 3.1
        * @param aRequestUrl the request url
        * @return void
        */
        void SetRequestUrlL(const TDesC8& aRequestUrl);

        /**
        * RequestUrl
        * Return the request url
        * @since 3.1
        * @return void
        */
        HBufC8* RequestUrl() { return iRequestUrl; }

        /**
        * SetURLLoader
        * Save request url. We take ownership of the url.
        * @since 3.1
        * @param aRequestUrl the request url
        * @return void
        */
        void SetUrlLoader(MContentLoaderInterface* aUrlLoader);

        /**
        * UrlLoader
        * Return the request url
        * @since 3.1
        * @return void
        */
        MContentLoaderInterface* UrlLoader() { return iUrlLoader; }

  private: // members
        TInt iTransId;
        TPluginLoadMode iLoadMode;
        TInt iCompleteError;
        TInt iHttpStatus;
        HBufC8* iRequestUrl;
        MContentLoaderInterface* iUrlLoader;
    };

#endif