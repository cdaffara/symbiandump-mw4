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
* Description:  Main transaction class for handling server client communication
 *
*/


#ifndef TRANSACTION_H
#define TRANSACTION_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES
#include <e32base.h>
#include <e32std.h>

#include <ApEngineconsts.h>
#include "FeedsServerMsg.h"

#include "FeedsEntity.h"
#include "FeedsInterface.h"

// CONSTANTS

// MACROS

// FUNCTION PROTOTYPES

// CLASS DECLARATION
class CIdle;
class CPackedFolder;
class CPackedFeed;
class CFeedsMap;
class CTransaction;
class RFeedsInterface;
class CFeedsEntity;
struct TFeedsServerSetting;

// DATA TYPES

// Feeds Events
enum TFeedsEvent
    {
    EFeedsRequestCompleted,
    EFeedsRequestFailed
    };

class CTransaction : public CActive
    {
public:
    // Data types 
    enum TTransactionType
        {
        ENone = 0,
        // folder related
        EFetchRootFolderItem,
        EWatchForChanges,
        EAddFolderItem,
        EDeleteFolderItem,
        EChangeFolderItem,
        EMoveFolderItem,
        EMoveFolderItemTo,
        EUpdateFolderItem,
        EImportOPML,
        EExportOPML,

        // feed related
        EFetchFeed,
        EUpdateItemStatus,

        // general client related
        EChangeSettings,
        EFetchSettings,
        EWatchForSettingChanges,
        ECancelAll,
        EDisconnectManualUpdateConnection,
        EPrintDBTables
        };

public:
    // Construct/Destructor
    /**
     * Two-phased constructor.
     */
    static CTransaction* NewL(MTransactionObserver& aObserver, TInt aUniqueId,
            TInt aFolderListId);

    static CTransaction* NewLC(MTransactionObserver& aObserver, TInt aUniqueId,
            TInt aFolderListId);

    /**
     * Destructor.
     */
    virtual ~CTransaction();

public:
    // From CActive
    /**
     * Implements cancellation of an outstanding request.
     *
     * @return void.
     */
    virtual void DoCancel();

    /**
     * Handles an active object's request completion event.
     *
     * @return void.
     */
    virtual void RunL();

    /**
     * Handles an active object's errors.
     *
     * @param aError The error.
     * @return A status code.
     */
    virtual TInt RunError(TInt aError);

public:
    // New methods
    /**
     * Starts an CIdle to delete the instance after the callstack has unrolled.
     *
     * @return Void
     */
     IMPORT_C void AutoDelete();

    /**
     * Returns the handler's type.
     *
     * @return Void
     */
    IMPORT_C TTransactionType Type();

private:
    // New Methods
    /**
     * C++ default constructor.
     */
    CTransaction(MTransactionObserver& aObserver, TInt aUniqueId,
            TInt aFolderListId);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Starts the handler
     *
     * @return void
     */
    void Start();

    /**
     * Called to handle the response.
     *
     * @return Void
     */
    void RequestCompletedL(TInt aStatusCode);

    /**
     * Request the FeedsServer to cancel all activities that can be cancelled.
     *
     * @return void
     */
    void CancelAllL();

private:
    /**
     * Deletes the instance after the callstack has unrolled..
     *
     * @return Void
     */
    static TInt DelayedDelete(TAny* aPtr);

public: // Function from client request handler
    /**
    * Print tables of database.
    *
    * @since 7.1
    * @return void
    */
    void DebugPrintTables();

    /**
     * Disconnect connection provided by client for manual update.
     *
     * @return void
     */
    void DisconnectManualUpdateConnection();

    /**
     * Sets up a notifier to execute when the settings of given folder list changes.
     *
     * @param aFolderListId The folder-list's id.
     * @return void
     */
    void WatchSettingsL();

    /**
     * Handles the upating of the FeedsServer settings.
     *
     * @return void
     */
    void ChangeServerSettingsL(const TFeedsServerSetting& aNewSetting);

    /**
     * Handles the fetching of the FeedsServer settings.
     *
     * @return void
     */
    void FetchServerSettingsL();

    /**
    * Handles the fetching of the feeds server settings synchronously.
    *
    * @since 7.1
    * @return void
    */
    void FetchServerSettingsSyncL();


     /**
     * Returns the FeedsServer's settings.
     *
     * @return The settings.
     */
    IMPORT_C TFeedsServerSetting Settings();

public:
    /**
     * Sets the feed interface object
     *
     * @param aFeedInterface
     * @return void
     */
    void SetFeedInterface(RFeedsInterface* aFeedInterface)
        {
        iFeedInterface = aFeedInterface;
        }

    /**
     * Returns transaction id
     *
     * @return Transaction id
     */
    IMPORT_C TInt GetId();

    /**
     * Returns Status code
     *
     * @return Status code
     */
    IMPORT_C TInt GetStatusCode();

    /**
     * Cancels the request
     * @return Status code
     */
    IMPORT_C void CancelRequest();

public:
    //Folder related functions
    /**
     * Fetch the root folder associated with the given folder list id.
     *
     * @param aFolderListId The folder-list's id.
     * @return void
     */
    void FetchFolderItemL(TBool aItemTitleNeed);

    /**
     * Add a new folder item.
     *
     * @param aTitle The title.
     * @param aUrl The url if this is not a folder.
     * @param aIsFolder Whether or not this is a folder.
     * @param aParent The parent folder-item.
     * @return void
     */
    void AddFolderItemL(const TDesC& aTitle, const TDesC& aUrl,
            TBool aIsFolder, const CFeedsEntity& aParent, TInt aFreq);

    /**
     * Change a folder item.
     *
     * @param aTitle The title.
     * @param aUrl The url if this is not a folder.
     * @return void
     */
    void ChangeFolderItemL(const CFeedsEntity& aFolderItem,
            const TDesC& aTitle, const TDesC& aUrl, TInt aFreq);

    /**
     * Delete one or more folder items.
     *
     * @param aFolderItems The list of folder items.
     * @return void
     */
    void DeleteFolderItemL(const RPointerArray<const CFeedsEntity>& aFolderItems);

    /**
     * Export one or more folder items.
     *
     * @param aFolderItems The list of folder items.
     * @return void
     */
    void ExportFolderItemL(const RPointerArray<const CFeedsEntity>& aFolderItems,
            const TDesC &aExportFileName);

    /**
     * Move one or more folder items to a different parent folder.
     *
     * @param aFolderItems The list of folder items.
     * @param aParent The parent.
     * @return void
     */
    void MoveFolderItemsToL(const RPointerArray<const CFeedsEntity>& aFolderItems,
            const CFeedsEntity& aParent);

    /**
     * Move one or more folder items to a new position.
     *
     * @param aFolderItems The list of folder items.
     * @param aIndex The target index.
     * @return void
     */
    void MoveFolderItemsL(const RPointerArray<const CFeedsEntity>& aFolderItems,
            TInt aIndex);

    /**
     * Update one or more feed.
     *
     * @param aFolderItems The list of folder items.
     * @return void
     */
    void UpdateFolderItemsL(
            const RPointerArray<const CFeedsEntity>& aFolderItems);

    /**
     * Update all of the feeds associated with the given folder list id.
     *
     * @param aFolderListId The folder-list's id.
     * @return void
     */
    void UpdateFolderItemsL();

    /**
     * Sets up a notifier to execute when the folder list changes.
     *
     * @param aFolderListId The folder-list's id.
     * @return void
     */
    void WatchFolderListL();

    /**
     * Returns the folder-item -- the caller takes ownership.
     * Only returns a valid folder item after a sucessful call to FetchFolderItemL.
     *
     * @return The folder item.
     */

    CFeedsEntity* OrphanFolderItem();

    /**
     * Import OPML file passed from the client. 
     *
     * @param aFolderListId The folder list id to which imported feeds belong.
     * @param aAutoUpdate The flag to or not to auto update the feeds with aFolderListId.
     * @param aPath The OPML file path.
     * @return void.
     */
    void ImportOPMLL(const TDesC& aPath, TBool aClearFolderList);
    
public:
    //Feed related functions
    /**
     * Fetch the given feed.
     *
     * @param aUrl The url of the feed to fetch (if length equals zero then the aFeedId is used instead).
     * @param aFeedId The id of the feed.
     * @param aForceUpdate If ETrue the feed is always updated before returning it.
     * @param aNoCache If ETrue the feed isn't placed in database after updating.
     * @return void
     */
    void FetchFeedHelperL(const TDesC& aUrl, TInt aFeedId, TBool aForceUpdate,
            TBool aNoCache);

    /**
     * Fetch the given feed.
     *
     * @param aUrl The url of the feed to fetch.
     * @param iFolderListId The folder list ID associate with the feed.
     * @param aForceUpdate If ETrue the feed is always updated before returning it.
     * @param aNoCache If ETrue the feed isn't placed in database after updating.
     * @return void
     */
    void FetchFeedL(const TDesC& aUrl, TBool aForceUpdate, TBool aNoCache);

    /**
     * Fetch the given feed.
     *
     * @param aFeedId The id of the feed.
     * @param aForceUpdate If ETrue the feed is always updated before returning it.
     * @param aNoCache If ETrue the feed isn't placed in database after updating.
     * @return void
     */
    void FetchFeedL(TInt aFeedId, TBool aForceUpdate = EFalse, TBool aNoCache = EFalse);

    /**
     * Handles the upating of a the feed's item-status.
     *
     * @param aFeedId The feed.
     * @param aItemIds The id's of the items to update.
     * @param aItemStatus The new status of the items.
     * @return void
     */
    void UpdateItemStatusL(TInt aFeedId, const RArray<TInt>& aItemIds,
            const RArray<TFeedItemStatus>& aItemStatus, TInt aUnreadCount);

    /**
     * Returns the feedentity -- the caller takes ownership.
     *
     * @return The FeedsEntity.
     */
    CFeedsEntity* OrphanFeedsEntity();

    /**
     * Returns Type of transaction being handled by this object
     *
     * @return TTransactionType.
     */
    IMPORT_C TTransactionType GetType();

    /**
     * Extract the current settings from the servers response.
     *
     * @since 7.1
     * @param aResponseBuffer The response buffer to extract the settings from.
     * @return The settings.
     */
    static TFeedsServerSetting SettingsFromResponseBuffPtrL(CBufFlat& aResponseBuffer);

    /**
     * returns the current settings.
     *
     * @since 7.1
     * @param .
     * @return The settings.
     */
    void GetSetting(TFeedsServerSetting&  aSetting);

    // friend class declaration.
    friend class RFeedsInterface;

private:
    /**
     * Initialises iSetting by reading response buffer from server.
     *
     * @return ETrue if waiter is completed.
     */
    void SettingsFromResponseBufferL();

private: // Data members.

    MTransactionObserver& iObserver;
    TTransactionType iType;
    CBufFlat* iRequestBuff;
    TPtrC8 iRequestBuffPtr;
    TInt iFolderListId; // Folderlist should be stored in transaction instead of FeedsEntity

    // Data
    CIdle* iAutoDelete;
    TBool iNotifyOnCancel;
    TInt iId;
    TInt iTranStatus;

    // Used to handle FetchFolderItemL/FetchFeed
    TFeedsServerResponseType iServerResponseType;
    TPckg<TFeedsServerResponseType> iServerResponseTypePkg;
    HBufC8* iResponseBuffer;
    CBufFlat* iSettingsResponseBuffer;
    TPtr8 iResponsePtr;
    CPacked* iPacked;
    CFeedsEntity* iFeedsEntity;
    TFeedsServerSetting iSetting;

    // Used to handle AddFolderItemL
    TInt iResponseEntryId;
    TPckg<TInt> iResponseEntryIdPkg;
    RFeedsInterface* iFeedInterface;

    };

#endif  // TRANSACTION_H

// End of File
