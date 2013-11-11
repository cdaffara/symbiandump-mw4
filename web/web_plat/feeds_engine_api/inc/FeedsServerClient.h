/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The client side connection to the FeedsSever.
*
*/


#ifndef FEEDS_SERVER_CLIENT_H
#define FEEDS_SERVER_CLIENT_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information


// INCLUDES
#include <e32std.h>

#include <FeedsServerFeed.h>
#include <FeedsServerFolderItem.h>
#include <FeedsServerRequestHandler.h>
#include <FeedsServerSettings.h>

//API changes
#include "FeedsInterface.h"
#include "FeedsMap.h"
#include "Transaction.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// CLASS DECLARATION
   class CClientRequestHandler;
/**
*  The RFeedsServer observer interface.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class MFeedsServerObserver
    {
    public:  // New Methods
        /**
        * Reports the progress of the UpdateAllFeedsL request.  
        *
        * @since 3.1
        * @param aTotalCount The total count of feeds to be updated.
        * @param aRemaining The current number of feeds remaining to be updated.
        * @return void.
        */
        // TODO: remove this.
        virtual void UpdateAllProgress(TInt aTotalCount, TInt aRemaining) = 0;

        /**
        * Notifies the observer that the feeds server settings have changed.  
        * ToDo: This is only called when the value was changed by a differnt client.
        * Call FetchSettingsL() to set up this notification
        *
        * @since 3.1
        * @param aStatus The result code of the request.  KErrNotFound is ok.
        * @return void.
        */
        virtual void SettingsChanged( TInt aStatus ) = 0;
    };


/**
*  The client side interface to the FeedsServer.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class RFeedsServer:  public RSessionBase, public MFeedsApiObserver
    {
    public:
        /**
        * C++ default constructor.
        */
        IMPORT_C RFeedsServer(MFeedsServerObserver& aObserver, TInt aFolderListId = 0);


    public: // From RSessionBase
        /**
        * Connects to the server
        *
        * @since 3.1
        * @return Error/success code
        */
        IMPORT_C TInt Connect();

        /**
        * Closes the connection to the server
        *
        * @since 3.1
        * @return Error/success code
        */
        IMPORT_C void Close();

        /**
        * Returns the implementation's version.
        *
        * @since 3.1
        * @return The version.
        */
        IMPORT_C TVersion Version() const;


    public: // From MFeedsApiObserver

        /**
        * Requests the observer to send a command to the server.
        *
        * @since 3.1
        * @param aFunction The function number identifying the request. 
        * @param aArgs A set of up to 4 arguments and their types to be passed to the server.
        * @param aStatus The request status object used to contain the completion status of the request.
        * @return void.
        */
    	virtual void SendAsyncCommand(TInt /*aFunction*/, const TIpcArgs& /*aArgs*/, 
               TRequestStatus& /*aStatus*/) const {} ; 

        /**
        * This function is called by Ctransaction class whenever
        * request is completed
        * 
        * @param aTrans Pointer to current transaction class
        * @param aEvent event related to this call.
        */
        virtual void RequestCompleted(CTransaction* aTrans, TInt aStatus);

       	/**
        * Request to create a network connection.
        *
        * @param aConnectionPtr A pointer to the new connection. If NULL, the 
        *  proxy filter will automatically create a network connection.
        * @param aSockSvrHandle A handle to the socket server.
        * @param aNewConn A flag if a new connection was created. If the 
        *                 connection is not new, proxy filter optimization will not 
        *                 read the proxy again from CommsBd.
        * @param aBearerType The bearer type of the new connection
        * @return void
        */
        virtual void NetworkConnectionNeededL(TInt* aConnectionPtr, TInt& aSockSvrHandle,
    	    TBool& aNewConn, TApBearerType& aBearerType);

        /**
        * Request the FeedsServer to cancel all activities that can be cancelled.
        */            
        virtual void CancelAllCommandsL() {} ;

    public:
        /**
        * Notifies the observer that the request is completed.
        *
        * @since 3.1
        * @param aRequestHandler The request handler.
        * @param aStatus The result code of the request.
        * @return void.
        */
        void RequestHandlerCompletedL(CTransaction& aTrans,
                TInt aStatus);

        void SetServerFeed(RFeed& aFeedsServerFeed);

        void SetServerFolderItem(RFolderItem& aFeedsServerFolderItem);

    public:  // New methods
        /**
        * Once fetched, it returns the settings.  This method panics the
        * client if it is called before the settings is available.
        *
        * @since 3.1
        * @return The settings.
        */
        IMPORT_C const TFeedsServerSettings Settings() const;

        /**
        * Requests the server to change its settings to match the provided values. 
        * Some care is needed by the client to prevent settings loss if SettingsChanged
        * was called after it called SettingsL.
        *
        * @since 3.1
        * @return void.
        */
        IMPORT_C void UpdateSettingsL( const TFeedsServerSettings& aNewSettings );

        /**
        * Fetch the feeds server settings synchronously.
        *
        * @since 3.1
        * @return The settings.
        */
        IMPORT_C const TFeedsServerSettings FetchServerSettingsSyncL() const;

        /**
        * Fetch the feeds server settings asynchronously.
        *
        * @since 3.1
        * @return void.
        */
        IMPORT_C void FetchServerSettingsL();

        /**
        * Sets up a notifier to execute when the settings of given folder list changes.
        *
        * @since 3.2
        * @return void
        */
        IMPORT_C void WatchSettingsL();

        /**
        * Request the FeedsServer to cancel all activities that can be cancelled.
        *
        * @since 3.1
        * @return void.
        */
        IMPORT_C void CancelAllL();

        /**
        * Disconnect connection provided by client for manual update. 
        *
        * @since 3.1
        * @return void.
        */
        IMPORT_C void DisconnectManualUpdateConnectionL();

        /**
        * Print tables of database.
        *
        * @since 3.2
        * @if _DEBUG is turned off, server do thign for this request 
        * @return void
        */
        IMPORT_C void DebugPrintTablesL();

    public:  // Friends
        friend class RFeed;
        friend class RFolderItem;


    private:  // Data
        MFeedsServerObserver&           iObserver;
        TFeedsServerSettings            iSettings;
        TFeedsServerSettings            iPendingSettings;
        TInt                            iFolderListId;
        RPointerArray<CClientRequestHandler>   iActiveRequests;
    };


#endif  // FEEDS_SERVER_CLIENT_H
// End of File
