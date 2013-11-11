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

 
#include "FeedsInterface.h"
#include "FeedsMap.h"
#include "Transaction.h"
#include "FeedAttributes.h"
#include "FolderAttributes.h"
#include "Logger.h"
#include <e32math.h>

// -----------------------------------------------------------------------------
// RFeedsInterface::Constructor
// 
// Default constructor should never leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RFeedsInterface::RFeedsInterface(MFeedsApiObserver& aObserver,
        TInt aFolderListId) :
    iObserver(aObserver), iRootFolder(NULL), iCurrentFeed(NULL), iFolderListId(aFolderListId), iIsConnected(EFalse)
    {

    }

// -----------------------------------------------------------------------------
// RFeedsInterface::Connect
// 
// Connects to Feed server
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeedsInterface::Connect()
	{
    TInt retry = 2;

    TInt err;

    for (;;)
        {
        // Attempt to create a session with the server.
        err = CreateSession (KFeedsServerName, Version (), 25);

        // Get the settings.
        if ( err == KErrNone)
        	{
        	iIsConnected = ETrue;
        	return KErrNone;
        	}

        // Return general errors.
        if ( (err != KErrNotFound) && (err != KErrServerTerminated))
        	{
        	return err;
        	}

        // No more retries left.
        if ( --retry == 0)
        	{
        	return err;
        	}

        // If need be start the server.
        err = StartServer ();

        // Return if the server failed to start.
        if ( (err != KErrNone) && (err != KErrAlreadyExists))
        	{
        	return err;
        	}
        }
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::Close
// 
// Closes connection with server
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsInterface::Close()
    {
    SendReceive (EFeedsServerDisconnectManualUpdateConnection);
    SendReceive (EFeedsServerCloseFeedSession);
    SendReceive (EFeedsServerCompleteWatchSettings);
    SendReceive (EFeedsServerCompleteWatchFolderList);
    SendReceive (EFeedsServerCloseFolderSession);

    RSessionBase::Close ();

    delete iRootFolder;
    iRootFolder = NULL;
    delete iCurrentFeed;
    iCurrentFeed = NULL;
    iActiveTransaction.ResetAndDestroy ();
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::Version
// 
// Returns current version of API
// -----------------------------------------------------------------------------
//

EXPORT_C TVersion RFeedsInterface::Version() const
    {
    return (TVersion(KFeedsServerMajorVersionNumber, KFeedsServerMinorVersionNumber,
            KFeedsServerBuildVersionNumber));
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::SendAsyncCommand
// 
// calls SendReceive on current session
// -----------------------------------------------------------------------------
//

void RFeedsInterface::SendAsyncCommand(TInt aFunction, const TIpcArgs& aArgs,
        TRequestStatus& aStatus) const
        {
        SendReceive (aFunction, aArgs, aStatus);
        }

// -----------------------------------------------------------------------------
// RFeedsInterface::RequestHandlerCompleted
// 
// Called by transaction when request completes.
// -----------------------------------------------------------------------------
//

void RFeedsInterface::RequestHandlerCompleted(CTransaction& aTransaction,
        TInt aStatus)
    {
    TInt pos = KErrNotFound;
    
    for (int i = 0; i < iActiveTransaction.Count (); i++)
        {
        if ( aTransaction.GetId ()== iActiveTransaction[i]->GetId ())
            {
            pos = i;
            break;
            }
        }
        
    if ( aStatus != KErrNone && pos != KErrNotFound)
        {
        iObserver.RequestCompleted (iActiveTransaction[pos], aStatus == KErrNone ? EFeedsRequestCompleted
                                    : EFeedsRequestFailed);
        iActiveTransaction[pos]->AutoDelete();
        iActiveTransaction.Remove (pos);
        return;
        }
    
    if ( pos != KErrNotFound)
        {
        switch (iActiveTransaction[pos]->Type ())
        	{
            case CTransaction::EFetchRootFolderItem:
                if ( iActiveTransaction[pos]->iTranStatus == KErrNone)
                    {
                    if ( iRootFolder != NULL)
                        {
                        delete iRootFolder;
                        iRootFolder = NULL;
                        }
                    iRootFolder = iActiveTransaction[pos]->OrphanFeedsEntity ();
                    // Start watching for changes.
                    TRAP_IGNORE(WatchFolderListL ());
                    }
                break;
            case CTransaction::EFetchFeed:
                if ( iActiveTransaction[pos]->iTranStatus == KErrNone)
                    {
                    if ( iCurrentFeed != NULL)
                        {
                        delete iCurrentFeed;
                        }
                    iCurrentFeed = iActiveTransaction[pos]->OrphanFeedsEntity ();
                    UpdateFeed(iRootFolder, iCurrentFeed);
                    }
            	break;
            case CTransaction::EFetchSettings:
                   iActiveTransaction[pos]->GetSetting(iSetting);
                break;
            case CTransaction::EWatchForChanges:
                if ( iActiveTransaction[pos]->iTranStatus == KErrNone)
                    {
                    TRAP_IGNORE(GetRootFolderL());
                    iActiveTransaction[pos]->AutoDelete();
                    iActiveTransaction.Remove (pos);
                    return;
                    }
                break;
            // no need to notify observer now, server will notify us later
            // assuming FetchRootFolder or WatchFolderList has been called before
            case CTransaction::EAddFolderItem:
            case CTransaction::EDeleteFolderItem:
            case CTransaction::EChangeFolderItem:
            case CTransaction::EMoveFolderItem:
            case CTransaction::EMoveFolderItemTo:
            case CTransaction::EUpdateFolderItem:
                iActiveTransaction[pos]->AutoDelete();
                iActiveTransaction.Remove (pos);
                return;
            
            case CTransaction::EImportOPML:
            case CTransaction::EExportOPML:
                break;

            // feed related
            case CTransaction::EUpdateItemStatus:
                
            // general client related
            case CTransaction::EChangeSettings:
                break;
                
            case CTransaction::EWatchForSettingChanges:
                if ( iActiveTransaction[pos]->iTranStatus == KErrNone)
                    {
                    TRAP_IGNORE( 
                        {
                        CTransaction* trans = GetTransactionL();
                        CleanupStack::PushL(trans);
                        trans->FetchServerSettingsL();
                        iActiveTransaction.AppendL(trans);
                        CleanupStack::Pop(trans);
                        });
                    iActiveTransaction[pos]->AutoDelete();
                    iActiveTransaction.Remove (pos);
                    return;
                    }
                break;
                
            case CTransaction::ECancelAll:
            case CTransaction::EDisconnectManualUpdateConnection:
                return;

        	}
        iObserver.RequestCompleted (iActiveTransaction[pos],
                    aStatus == KErrNone ? EFeedsRequestCompleted
                                    : EFeedsRequestFailed);
        iActiveTransaction[pos]->AutoDelete();
        iActiveTransaction.Remove (pos);
        }
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::UpdateFeed
// 
// Updates timestamp/unreadcount in correponding on in rootfolder.
// -----------------------------------------------------------------------------
//

TBool RFeedsInterface::UpdateFeed(CFeedsEntity* aRootFolder, CFeedsEntity* aFeed)
	{
    if ( aRootFolder == NULL || aFeed == NULL)
         return EFalse;
    if ( aRootFolder->GetType ()== EFeed && aFeed->iFeedId == aRootFolder->iFeedId)
        {
        TTime time;
        time.HomeTime ();
        aFeed->GetTimeValue (EFeedAttributeTimestamp, time);
        aRootFolder->iAttrProp->SetTimeAttribute (EFeedAttributeTimestamp, time);
        TInt unreadCount = 0;
        TInt status;
        for (TInt i=0; i<aFeed->GetChildren().Count ();i++)
            {
            CFeedsEntity* item = aFeed->GetChildren()[i];
            item->GetIntegerValue (EItemAttributeStatus, status);
            if ( status != EItemStatusRead)
                {
                unreadCount++;
                }
            }
        aRootFolder->iAttrProp->SetIntegerAttribute (
            EFolderAttributeUnreadCount, unreadCount);
        return ETrue;
        }
    if ( aRootFolder->GetType ()== EFolder)
        {
        for (TInt i =0; i<aRootFolder->GetChildren().Count ();i++)
            {
            if ( UpdateFeed (aRootFolder->GetChildren()[i], aFeed))
                {
                return ETrue;
                }
            }
        }
    return EFalse;
	}

// -----------------------------------------------------------------------------
// RFeedsInterface::CancelAllCommandsL
// 
// Cancel commands
// -----------------------------------------------------------------------------
//
void RFeedsInterface::CancelAllCommandsL()
    {
    CTransaction* trans = GetTransactionL ();
    CleanupStack::PushL (trans);

    trans->CancelAllL();
    iActiveTransaction.AppendL (trans);
    CleanupStack::Pop (trans);
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::GetRootFolder
// 
// Fetches root folder
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeedsInterface::GetRootFolderL()
    {
    CTransaction* trans = GetTransactionL ();
    CleanupStack::PushL (trans);

    trans->FetchFolderItemL (EFalse);
    iActiveTransaction.AppendL (trans);
    CleanupStack::Pop (trans);
    return trans->GetId();
    }
// -----------------------------------------------------------------------------
// RFeedsInterface::Add
// 
// Adds new feed/folder into list
// -----------------------------------------------------------------------------
//

EXPORT_C void RFeedsInterface::AddL(TFeedsEntityType aType, const CFeedsMap& aArgs,
        const CFeedsEntity& aParent)
    {
    TPtrC title;
    TPtrC url;
    TInt freq;
    aArgs.GetStringAttribute(EFeedAttributeTitle,title);
    aArgs.GetStringAttribute(EFeedAttributeLink,url);
    if (aArgs.GetIntegerAttribute(EFeedAttributeAutoUpdateFreq,freq) == KErrNotFound)
        {
        freq = 0;
        }

    CTransaction* trans = GetTransactionL ();
    CleanupStack::PushL (trans);

    trans->AddFolderItemL (title, url, aType==EFolder, aParent, freq);
    iActiveTransaction.AppendL (trans);
    CleanupStack::Pop (trans);
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::SetAttributeL
// 
// Changes various feed/folder/item attribute on server side.
// -----------------------------------------------------------------------------
//

EXPORT_C TInt RFeedsInterface::SetAttributeL(const CFeedsEntity& aFeedEntity,
        const CFeedsMap& aNewValues)
    {
    TPtrC title;
    TPtrC url;
    TInt freq = 0;
    TFeedItemStatus readStatus;
    TInt unreadCount = 0;
    CFeedsEntity* parent = NULL;

    switch (aFeedEntity.GetType())
        {
        case EFeed:
            if( aNewValues.IsExist( EFeedAttributeTitle) )
                {
                aNewValues.GetStringAttribute ( EFeedAttributeTitle, title);                       
                }
                else
                {
                aFeedEntity.iAttrProp->GetStringAttribute( EFeedAttributeTitle, title);
                }
            if( aNewValues.IsExist ( EFeedAttributeLink) )     
                {
                aNewValues.GetStringAttribute ( EFeedAttributeLink, url);    
                }
            else
                {
                aFeedEntity.iAttrProp->GetStringAttribute( EFeedAttributeLink, url);    
                }    
            if( aNewValues.IsExist( EFeedAttributeAutoUpdateFreq))
                {
                aNewValues.GetIntegerAttribute( EFeedAttributeAutoUpdateFreq, freq);    
                }
            else
                {
                if ( KErrNotFound == aFeedEntity.iAttrProp->GetIntegerAttribute( EFeedAttributeAutoUpdateFreq, freq))
                    {
                    freq = 0;
                    }
                }
            break;	

        case EFolder:
            if ( aNewValues.IsExist (EFeedAttributeTitle))
                {
                aNewValues.GetStringAttribute (EFeedAttributeTitle, title);
                }
            else
                {
                return KErrArgument;
                }
            break;

        case EFeedsItem:

            if ( aNewValues.IsExist (EItemAttributeStatus))
                {
                TInt status;
                aNewValues.GetIntegerAttribute (EItemAttributeStatus, status);
                readStatus = (TFeedItemStatus)status;
                }
            else
                {
                return KErrArgument;
                }
            break;

        case EFeedsEnclosure:
            //return KErrArgument;
            break;
        }
    CTransaction* trans = GetTransactionL ();
    CleanupStack::PushL (trans);

    switch (aFeedEntity.GetType ())
        {
        case EFeed:
            trans->ChangeFolderItemL (aFeedEntity, title, url, freq);
            break;

        case EFolder:
            trans->ChangeFolderItemL (aFeedEntity, title, url, freq);
            break;

        case EFeedsItem:
            {
            parent = SearchFeed (aFeedEntity.GetParent()->iFeedId, iRootFolder);
            RArray<TInt> itemIds;
            RArray<TFeedItemStatus> status;
            TInt oldStatus;
            if ( aFeedEntity.iAttrProp->GetIntegerAttribute (
                    EItemAttributeStatus, oldStatus)== KErrNotFound)
                {
                oldStatus = (TInt)EItemStatusUndefined;
                }

            if ( parent != NULL && parent->iAttrProp->GetIntegerAttribute (
                    EFolderAttributeUnreadCount, unreadCount)!= KErrNotFound)
                {
                }

            if ( oldStatus != (TInt) readStatus)
                {
                if ( (oldStatus == EItemStatusNew || oldStatus == EItemStatusUnread) && readStatus == EItemStatusRead)
                    {
                    unreadCount--;
                    }
                else
                	if ( oldStatus == EItemStatusRead && (readStatus == EItemStatusNew || readStatus == EItemStatusUnread))
                        {
                        unreadCount++;
                        }
                }
            itemIds.Append (aFeedEntity.GetId ());
            status.Append (readStatus);

            trans->UpdateItemStatusL (aFeedEntity.GetParent()->iFeedId, itemIds, status, unreadCount);

            }
            break;

        case EFeedsEnclosure:
        	break;
        }
    iActiveTransaction.AppendL (trans);
    if ( aFeedEntity.GetType ()== EFeedsItem)
        {
        aFeedEntity.iAttrProp->SetIntegerAttribute (EItemAttributeStatus,
                readStatus);
        if ( parent != NULL)
            parent->iAttrProp->SetIntegerAttribute (
                EFolderAttributeUnreadCount, unreadCount);
        }

    CleanupStack::Pop (trans);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::SearchFeed
// 
// Searches feed with given id ,in list.
// -----------------------------------------------------------------------------
//
EXPORT_C CFeedsEntity* RFeedsInterface::SearchFeed(TInt aFeedId, CFeedsEntity* aSearch)
    {

    if ((aSearch->GetType ()== EFeed) && (aSearch->iFeedId == aFeedId))
        {
        return aSearch;
        }
    if ( aSearch->GetType ()== EFolder)
        {
        const RPointerArray<CFeedsEntity>& children = aSearch->GetChildren();
        TInt count = children.Count();
        CFeedsEntity* child = NULL;
        CFeedsEntity *foundFeed = NULL;
        for (TInt i=0; i<count;i++)
            {
            child = children[i];
            foundFeed = SearchFeed (aFeedId, child);
            if(foundFeed)
                {
                return foundFeed;
                }		
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::Delete
// 
// Delete feeds/folder
// -----------------------------------------------------------------------------
//

EXPORT_C void RFeedsInterface::DeleteL(const RPointerArray<const CFeedsEntity> aFeedsEntities)
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);

    trans->DeleteFolderItemL(aFeedsEntities);
    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::DebugPrintTablesL
//
// Print tables of database.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsInterface::DebugPrintTablesL()
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);

    trans->DebugPrintTables();
    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);
    }
	
// -----------------------------------------------------------------------------
// RFeedsInterface::DisconnectManualUpdateConnectionL
//
// Disconnect connection provided by client for manual update.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsInterface::DisconnectManualUpdateConnectionL()
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);

    trans->DisconnectManualUpdateConnection();
    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);
    }
	
	
// -----------------------------------------------------------------------------
// RFeedsInterface::Fetch
// 
// Fetch feed from server (May involve updation of feed)
// -----------------------------------------------------------------------------
//

EXPORT_C TInt RFeedsInterface::FetchL(const CFeedsEntity& aEntity, TBool aForceUpdate, TBool aNoCache)
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);

    trans->FetchFeedL(aEntity.iFeedId, aForceUpdate, aNoCache);
    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);
    return trans->GetId();
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::Fetch
// 
// Fetch feed from server (May involve updation of feed)
// -----------------------------------------------------------------------------
//

EXPORT_C TInt RFeedsInterface::FetchL(const TDesC& aUrl, TBool aForceUpdate, TBool aNoCache)
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);

    trans->FetchFeedL(aUrl, aForceUpdate, aNoCache);
    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);
    return trans->GetId();
    }
	
// -----------------------------------------------------------------------------
// RFeedsInterface::Update
// 
// Update feeds/Folder
// -----------------------------------------------------------------------------
//

EXPORT_C TInt RFeedsInterface::UpdateL(const RPointerArray<const CFeedsEntity>& aFeedsArray)
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);
    if(aFeedsArray.Count() == 1 && aFeedsArray[0]->GetId() == 0)
        {
        trans->UpdateFolderItemsL();
        }
    else
        {
        trans->UpdateFolderItemsL(aFeedsArray );
        }

    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);
    return trans->GetId();
    }


// -----------------------------------------------------------------------------
// RFeedsInterface::GetSettings
// 
// Fetches Settings from server
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeedsInterface::GetSettingsL(TFeedsServerSetting& aFeedsSetting, TBool aAsync/*=ETrue*/)
    {

    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);
    if(aAsync)
        {
        trans->FetchServerSettingsL();
        iActiveTransaction.AppendL(trans);
        }
    else
        {
        trans->FetchServerSettingsSyncL();
        iSetting = trans->Settings();
        trans->AutoDelete();
        aFeedsSetting = iSetting;
        }
    CleanupStack::Pop(trans);
    return KErrNone;
    }
	
// -----------------------------------------------------------------------------
// RFeedsInterface::SetSettings
// 
// Changes settings
// -----------------------------------------------------------------------------
//

EXPORT_C void RFeedsInterface::SetSettingsL(const TFeedsServerSetting& aFeedsSetting)
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);

    trans->ChangeServerSettingsL( aFeedsSetting );
    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::WatchSettings
// 
// Sets up a notifier to execute when the settings change.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsInterface::WatchSettingsL()
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);

    trans->WatchSettingsL();
    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::ExportFolders
// 
// Exports list of folders in form of OPML file
// -----------------------------------------------------------------------------
//

EXPORT_C void RFeedsInterface::ExportFoldersL(const RPointerArray<const CFeedsEntity>& aFeedsArray,const TDesC& aFileName)
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);

    trans->ExportFolderItemL( aFeedsArray,aFileName);
    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);
    }
    
// -----------------------------------------------------------------------------
// RFeedsInterface::ImportOPML
// 
// Imports OPML
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsInterface::ImportOPMLL(const TDesC& aFileName)
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);

    trans->ImportOPMLL( aFileName ,EFalse);
    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::CancelAll
// 
// Cancels all pending requests
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsInterface::CancelAllL()
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);

    trans->CancelAllL();
    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::Move
// 
// Move folder/feed to specific index.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsInterface::MoveL(const RPointerArray<const CFeedsEntity>& aFolderItems,TInt aIndex)
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);

    trans->MoveFolderItemsL( aFolderItems,aIndex);
    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::MoveToFolder
// 
// Move feeds/folders to diff folder.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsInterface::MoveToFolderL(const RPointerArray<const CFeedsEntity>& aSourceFolders,const CFeedsEntity& aDestinationFolder)
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);

    trans->MoveFolderItemsToL( aSourceFolders,aDestinationFolder);
    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::WatchFolderListL
// 
// Sets up notifier for changes happen on server side.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsInterface::WatchFolderListL()
    {
    for (int i=0; i<iActiveTransaction.Count ();i++)
        {
        if ( iActiveTransaction[i]->Type ()== CTransaction::EWatchForChanges)
            {
            return;
            }
        }

    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL (trans);

    trans->WatchFolderListL ();
    iActiveTransaction.AppendL (trans);
    CleanupStack::Pop (trans);
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::GetTransactionL
// 
// Factory function to return transaction with unique ID
// -----------------------------------------------------------------------------
//
CTransaction* RFeedsInterface::GetTransactionL()
    {
    TBool found = EFalse;
    TInt transId;
    do
        {
        found = EFalse;
        transId = Math::Random ();
        for (int i=0; i<iActiveTransaction.Count ();i++)
            {
            if ( iActiveTransaction[i]->GetId ()== transId)
                {
                found = ETrue;
                break;
                }
            }
        }
    while (found);
    CTransaction* trans = CTransaction::NewL (*this, transId, iFolderListId);
    trans->SetFeedInterface (this);
    return trans;
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::StartServer
// 
// Create the server thread/process
// -----------------------------------------------------------------------------
//
TInt RFeedsInterface::StartServer()
    {
    RProcess server;

    // EPOC is easy, just create a new server process. Simultaneous
    // launching of two such processes is detected when the second one
    // attempts to create the server object, failing with KErrAlreadyExists.
    TInt r = server.Create (KFeedsServerName, KNullDesC);
    if ( r != KErrNone)
        {
        return r;
        }

    TRequestStatus stat;
    server.Rendezvous (stat);

    // An unrecoverable error occurred.
    if ( stat != KRequestPending)
        {
        server.Kill (0);
        }

    // Logon OK, start the server.
    else
        {
        server.Resume ();
        }

    // Wait for start or death.
    User::WaitForRequest (stat);

    // The 'exit reason' can't be used if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    r = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int ();

    // Close the server.
    server.Close ();

    return r;
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::RootFolder
// 
// Returns RootFolder
// -----------------------------------------------------------------------------
//

EXPORT_C CFeedsEntity* RFeedsInterface::RootFolder()
    {
    return iRootFolder;
    }
// -----------------------------------------------------------------------------
// RFeedsInterface::Feed
// 
// Return currently fetched feed
// -----------------------------------------------------------------------------
//

EXPORT_C CFeedsEntity* RFeedsInterface::Feed()
    {
    return iCurrentFeed;
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::UpdateFeedItemStatusL
// Updates the feed item's status
// -----------------------------------------------------------------------------
//	
EXPORT_C void RFeedsInterface::UpdateFeedItemStatusL(const RArray<TInt>& aItemIds,
        const RArray<TFeedItemStatus>& aItemStatus, TInt aUnreadCount)
    {
    CTransaction* trans = GetTransactionL();
    CleanupStack::PushL(trans);

    trans->UpdateItemStatusL( iCurrentFeed->iFeedId, aItemIds, aItemStatus, aUnreadCount);
    iActiveTransaction.AppendL(trans);
    CleanupStack::Pop(trans);

    }


// -----------------------------------------------------------------------------
// RFeedsInterface::NetworkConnectionNeededL()
// -----------------------------------------------------------------------------
//
void RFeedsInterface::NetworkConnectionNeededL( TInt* aConnectionPtr,
        TInt& aSockSvrHandle, TBool& aNewConn, TApBearerType& aBearerType)
    {
    iObserver.NetworkConnectionNeededL ( aConnectionPtr, aSockSvrHandle,
            aNewConn, aBearerType);
    }
	

// -----------------------------------------------------------------------------
// RFeedsInterface::SendReceiveSync
// 
// calls synchronous SendReceive on current session
// -----------------------------------------------------------------------------
//

TInt RFeedsInterface::SendReceiveSync(TInt aFunction, const TIpcArgs& aArgs) const
    {
    return (SendReceive(aFunction, aArgs));
    }

// -----------------------------------------------------------------------------
// RFeedsInterface::Settings
// 
// return current settings.
// -----------------------------------------------------------------------------
//
EXPORT_C TFeedsServerSetting RFeedsInterface::Settings()
    {
    return iSetting;
    }
