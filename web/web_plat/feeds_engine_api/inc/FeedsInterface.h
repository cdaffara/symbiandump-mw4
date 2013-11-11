/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef FEED_INTERFACE_H
#define FEED_INTERFACE_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <ApEngineconsts.h>
//#include "Transaction.h"
#include "FeedsEntity.h"
#include "FeedAttributes.h"
#include "FolderAttributes.h"
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CTransaction;

// CLASS DECLARATION

class MFeedsApiObserver
    {
public:
    /**
     * This function is called by Ctransaction class whenever
     * request is completed
     * 
     * @param aTrans Pointer to current transaction class
     * @param aEver event related to this call.
     */
     virtual void RequestCompleted(CTransaction* aTrans, TInt aEvent) = 0;

    /**
     * Request to create a network connection.
     *
     * @param aConnectionPtr A pointer to the new connection. If NULL, the 
     *                       proxy filter will automatically create a network connection.
     * @param aSockSvrHandle A handle to the socket server.
     * @param aNewConn A flag if a new connection was created. If the 
     *                 connection is not new, proxy filter optimization will not 
     *                 read the proxy again from CommsBd.
     * @param aBearerType The bearer type of the new connection
     * @return void
     */
     virtual void
    		NetworkConnectionNeededL(TInt* aConnectionPtr,
    				TInt& aSockSvrHandle, TBool& aNewConn,
    				TApBearerType& aBearerType) = 0 ;

    };

/**
 *  The CTransaction observer interface.
 *
 */
class MTransactionObserver
    {
public:
    // New Methods
    /**
     * Requests the observer to send a command to the server.
     *
     * @param aFunction The function number identifying the request. 
     * @param aArgs A set of up to 4 arguments and their types to be passed to the server.
     * @param aStatus The request status object used to contain the completion status of the request.
     * @return void.
     */
    virtual void SendAsyncCommand(TInt aFunction, const TIpcArgs& aArgs,
    		TRequestStatus& aStatus) const = 0;

    /**
     * Notifies the observer that the request is completed.
     *
     * @param aRequestHandler The request handler.
     * @param aStatus The result code of the request.
     * @return void.
     */
    virtual void RequestHandlerCompleted(CTransaction& aRequestHandler,
    		TInt aStatus) = 0;

    /**
     * Cancels all the commands
     */
    virtual void CancelAllCommandsL() = 0;

    /**
     * Request to create a network connection.
     *
     * @since 3.1
     * @param aConnectionPtr A pointer to the new connection. If NULL, the 
     *                       proxy filter will automatically create a network connection.
     * @param aSockSvrHandle A handle to the socket server.
     * @param aNewConn A flag if a new connection was created. If the 
     *                 connection is not new, proxy filter optimization will not 
     *                 read the proxy again from CommsBd.
     * @param aBearerType The bearer type of the new connection
     * @return void
     */
    virtual void
            NetworkConnectionNeededL(TInt* aConnectionPtr,
                TInt& aSockSvrHandle, TBool& aNewConn,
                TApBearerType& aBearerType) = 0;
    };

struct TFeedsServerSetting
    {
    TBool iAutoUpdate;
    TInt  iAutoUpdateFreq;
    TUint iAutoUpdateAP;
    TBool iAutoUpdateWhileRoaming;
    };
/**
 *  The client side interface to the FeedsServer.
 *
 *  \b Library: FeedsEngine.lib
 *
 */

class RFeedsInterface : public RSessionBase, public MTransactionObserver
    {
public:
    /**
     * C++ default constructor.
     */
    IMPORT_C RFeedsInterface(MFeedsApiObserver& aObserver,
            TInt aFolderListId = 0);

public:
    // From RSessionBase
    /**
     * Connects to the server
     *
     * @return Error/success code
     */
    IMPORT_C TInt Connect();

    /**
     * Closes the connection to the server
     *
     * @return Error/success code
     */
    IMPORT_C void Close();

    /**
     * Returns the implementation's version.
     *
     * @return The version.
     */
    IMPORT_C TVersion Version() const;

public: // From MRequestHandlerObserver
    /**
     * Requests the observer to send a command to the server.
     *
     * @param aFunction The function number identifying the request. 
     * @param aArgs A set of up to 4 arguments and their types to be passed to the server.
     * @param aStatus The request status object used to contain the completion status of the request.
     * @return void.
     */
    virtual void SendAsyncCommand(TInt aFunction, const TIpcArgs& aArgs,
    		TRequestStatus& aStatus) const;

    /**
     * Notifies the observer that the request is completed.
     *
     * @param aRequestHandler The request handler.
     * @param aStatus The result code of the request.
     * @return void.
     */
    virtual void RequestHandlerCompleted(CTransaction& aTransaction,
    		TInt aStatus);

    /**
     * Request the FeedsServer to cancel all activities that can be cancelled.
     */
    virtual void CancelAllCommandsL();

public:
    /**
     * Fetches Root folder (Asynchronous) Observer will be called when request completes
     *
     * @return TInt (KErrNone if successful).
     */

    IMPORT_C TInt GetRootFolderL();
    /**
     * Add new Feed/Folder into feedserver (Sync)
     *
     * @param aType (EFolder/EFeed)
     * @param aArgs Map cotaining various values
     * @param aParent Under which folder this object should be added
     * 
     * @return void.
     */

    IMPORT_C void AddL(TFeedsEntityType aType, const CFeedsMap& aArgs,const CFeedsEntity& aParent);
    /**
     * Deletes passed feeds/folders from list
     *
     * @param aFeedsEntities array of feedsentities to be deleted
     * @return void.
     */

    IMPORT_C void DeleteL(const RPointerArray<const CFeedsEntity> aFeedsEntities);

    /**
    * Print tables of database.
    *
    * @return void
    */
    IMPORT_C void RFeedsInterface::DebugPrintTablesL();

    /**
    * Disconnect connection provided by client for manual update.
    *
    * @return void
    */
    IMPORT_C void RFeedsInterface::DisconnectManualUpdateConnectionL();

     /**
     * Fetches feed (async)
     *
     * @param aEntity Feed to be fetched.
     * @param aForceUpdate If ETrue the feed is always updated before returning it.
     * @param aNoCache If ETrue the feed isn't placed in database after updating.
     * @return void.
     */

    IMPORT_C TInt FetchL(const CFeedsEntity& aEntity, TBool aForceUpdate, TBool aNoCache);
    /**
     * Fetches feed (async)
     *
     * @param aEntity Feed to be fetched.
     * @param aForceUpdate If ETrue the feed is always updated before returning it.
     * @param aNoCache If ETrue the feed isn't placed in database after updating.
     * @return TInt.
     */

    IMPORT_C TInt FetchL(const TDesC& aUrl, TBool aForceUpdate, TBool aNoCache);
     
    /**
     * Update Given list of folders(or All if only RootFolder is provided)
     *
     * @param aFeedsArray array of feeds to be updated
     * @return void
     */

    IMPORT_C TInt UpdateL(const RPointerArray<const CFeedsEntity>& aFeedsArray);

    //	TInt Find(TFeedsEntityType aType

    /**
     * Fetches settings from server.
     *
     * @param aFeedsSettings This Struct will contain settings if request is successful
     * @return void 
     */

    IMPORT_C TInt GetSettingsL(TFeedsServerSetting& aFeedsSetting, TBool aAsync = ETrue); 

     /**
     * Sets new setting.
     *
     * @param aFeedsSettings new settings.
     * @return void
     */

    IMPORT_C void SetSettingsL(const TFeedsServerSetting& aFeedsSetting);

     /**
     * Sets up a notifier to execute when the settings change.
     *
     * @return void.
     */

     IMPORT_C void RFeedsInterface::WatchSettingsL();

     /**
     * Export passed folder in OPML file
     *
     * @param aFeedsArray array of folder/feed to be exported
     * @param aFileName name of opml file.
     * @return void.
     */

    IMPORT_C void ExportFoldersL(const RPointerArray<const CFeedsEntity>& aFeedsArray,const TDesC& aFileName);
    /**
     * Imports OPML file
     *
     * @param aFileName name of OPML file to be imported
     * @return void.
     */

    IMPORT_C void ImportOPMLL(const TDesC& aFileName);
    /**
     * Cancels all pending requests
     *
     * @return void.
     */

    IMPORT_C void CancelAllL();
    /**
     * Moves list of folders/feeds to diff folder.
     *
     * @param aSourceFolders array of folder/feeds to be moved
     * @param aDestinationFolder 
     * @return void.
     */

    IMPORT_C void MoveToFolderL(const RPointerArray<const CFeedsEntity>& aSourceFolders,const CFeedsEntity& aDestinationFolder);
    /**
     * Changes index of folders.
     *
     * @param aFolderItems array of folderitem whose index are to be changed
     * @param aIndex new index
     * @return void
     */

    IMPORT_C void MoveL(const RPointerArray<const CFeedsEntity>& aFolderItems,TInt aIndex);

    /**
     * Sets attribute (Changes will be sent to feedserver)
     *
     * @param aFeedEntity Feed/Folder/Item whose attributes are to be changed
     * @param aNewValues map containing new attribute values.
     * @return void.
     */

    IMPORT_C TInt SetAttributeL(const CFeedsEntity& aFeedEntity, const CFeedsMap& aNewValues);
    /**
     * Returns Root folder
     *
     * @return pointer to CFeedsEntity (rootfolder)
     */

    IMPORT_C CFeedsEntity* RootFolder();
    /**
     * Fetches currently fetched feed
     *
     * @return pointer to CFeedsEntity (current feed)
     */

    IMPORT_C CFeedsEntity* Feed();
    /**
     * Sets notifier for folder list changes
     *
     * @return void
     */

    IMPORT_C void WatchFolderListL();
    /**
     * Uses aFeed attribute to change correponding feed attributes in aRootFolder tree
     *
     * @return TInt (KErrNone if successful).
     */
    TBool UpdateFeed(CFeedsEntity* aRootFolder,CFeedsEntity* aFeed);

    /**
     * Searches CFeedsEntity pertaining to aFeedId
     *
     * @param aFeedId (feed id to be searched)
     * @param aSearch folder/feed under which search will be performed.
     * @return TInt (KErrNone if successful).
     */

    IMPORT_C CFeedsEntity* SearchFeed(TInt aFeedId,CFeedsEntity* aSearch);

    /**
     * Request the feeds server to update the feed's item status. 
     *
     * @since 7.1
     * @param aItemIds The id's of the items to update.
     * @param aItemStatus The new item status.
     * @return void
     */
     IMPORT_C void UpdateFeedItemStatusL(const RArray<TInt>& aItemIds,
                const RArray<TFeedItemStatus>& aItemStatus, TInt aUnreadCount);

    /**
     * return currently fetched settings. 
     *
     * @since 7.1
     * @return TFeedsServerSetting.
     */   
     IMPORT_C TFeedsServerSetting Settings();

public:
    /**
     * Request to create a network connection.
     *
     * @param aConnectionPtr A pointer to the new connection. If NULL, the 
     *                       proxy filter will automatically create a network connection.
     * @param aSockSvrHandle A handle to the socket server.
     * @param aNewConn A flag if a new connection was created. If the 
     *                 connection is not new, proxy filter optimization will not 
     *                 read the proxy again from CommsBd.
     * @param aBearerType The bearer type of the new connection
     * @return void
     */
    void NetworkConnectionNeededL( TInt* aConnectionPtr, TInt& aSockSvrHandle,
    		TBool& aNewConn, TApBearerType& aBearerType );

    /**
     * Calls synchronous SendReceive on current session
     *
     * @param aFunction The function number identifying the request. 
     * @param aArgs A set of up to 4 arguments and their types to be passed to the server.
     * @return void.
     */
    TInt SendReceiveSync(TInt aFunction, const TIpcArgs& aArgs) const;

private:
    /**
     * Gets unique transaction (Factory function)
     *
     * @return new CTransaction
     */

    CTransaction* GetTransactionL();
    /**
     * Create the server thread/process
     *
     * @since 3.1
     * @return An error code.
     */
    TInt StartServer();

private:
    MFeedsApiObserver& iObserver;

    CFeedsEntity* iRootFolder;
    CFeedsEntity* iCurrentFeed;

    RPointerArray<CTransaction> iActiveTransaction;
    TInt iFolderListId;
    TInt iIsConnected;
    TFeedsServerSetting iSetting;

private:
    //Used for creating connection
    TBool iConnectionReady;
    TInt iConnectionPtr;
    TInt iSockSvrHandle;
    TBool iNewConn;
    TApBearerType iBearerType;

    };
    
#endif  // FEED_INTERFACE_H
// End of File
