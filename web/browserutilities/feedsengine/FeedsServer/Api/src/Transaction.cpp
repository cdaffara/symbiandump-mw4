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
* Description:  Main class for client side request handling.
 *
*/


#include "FeedsServerMsg.h"
#include "Transaction.h"
#include "FeedsEntity.h"
#include "FeedsInterface.h"
#include <s32mem.h>
#include <es_sock.h>
#include "FeedsMap.h"
#include "Packed.h"
#include "PackedFeed.h"
#include "PackedFolder.h"
#include "Logger.h"

const TInt KSettingsBufSize = 50;

// -----------------------------------------------------------------------------
// CFeedsMap::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTransaction* CTransaction::NewL(MTransactionObserver& aObserver, TInt aUniqueId,
        TInt aFolderListId)
    {
    CTransaction * self = NewLC(aObserver,aUniqueId,aFolderListId);
    CleanupStack::Pop(self);
    return self;
    }
    
// -----------------------------------------------------------------------------
// CTransaction::NewLC
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTransaction* CTransaction::NewLC(MTransactionObserver& aObserver, TInt aUniqueId,
        TInt aFolderListId)
    {
    CTransaction * self = new (ELeave) CTransaction(aObserver,aUniqueId,aFolderListId);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CTransaction::CTransaction
//
// C++ default constructor can NOT contain any code, that
// might leave.  
// -----------------------------------------------------------------------------
//
CTransaction::CTransaction(MTransactionObserver& aObserver,TInt aUniqueId,TInt aFolderListId):
CActive(EPriorityStandard), iObserver(aObserver), iRequestBuffPtr(NULL, 0),
iFolderListId(aFolderListId),iNotifyOnCancel(ETrue),iId(aUniqueId),iServerResponseTypePkg(iServerResponseType),
iResponsePtr(NULL, 0),iResponseEntryIdPkg(iResponseEntryId)
    {
    }

// -----------------------------------------------------------------------------
// CTransaction::ConstructL
//
// Symbian 2nd phase constructor can leave.  
// -----------------------------------------------------------------------------
//
void CTransaction::ConstructL()
    {
    CActiveScheduler::Add (this);

    iRequestBuff = CBufFlat::NewL (128);
    iSettingsResponseBuffer = CBufFlat::NewL (128);
    iAutoDelete = CIdle::NewL (CActive::EPriorityIdle);
    }

// -----------------------------------------------------------------------------
// CTransaction::~CTransaction
//
// Deconstructor.  
// -----------------------------------------------------------------------------
//
CTransaction::~CTransaction()
    {
    delete iFeedsEntity;
    delete iResponseBuffer;
    delete iPacked;

    iNotifyOnCancel = EFalse;
    Cancel ();

    delete iRequestBuff;
    delete iAutoDelete;
    delete iFeedsEntity;
    delete iSettingsResponseBuffer;
    }

// -----------------------------------------------------------------------------
// CTransaction::DoCancel
//
// Implements cancellation of an outstanding request.
// -----------------------------------------------------------------------------
//
void CTransaction::DoCancel()
    {
    // iNotifyOnCancel is set to false in the destructor.  
    // A virtual method, RequestCompletedL(),
    // that is implemented in a subclass can't be called through the destructor of the parent class.
    if ( iNotifyOnCancel)
        {
        TRAPD( err, RequestCompletedL(KErrCancel) )
        ;
        if ( err != KErrNone)
            {
            }
        }
    }

// -----------------------------------------------------------------------------
// CTransaction::RunL
//
// Handles an active object's request completion event.
// -----------------------------------------------------------------------------
//
void CTransaction::RunL()
    {
    RequestCompletedL (iStatus.Int ());
    }

// -----------------------------------------------------------------------------
// CTransaction::RunError
//
// Handles an active object's errors.
// -----------------------------------------------------------------------------
//
TInt CTransaction::RunError(TInt aError)
    {
    TRAPD( err, RequestCompletedL(aError) );
    return err;
    }

// -----------------------------------------------------------------------------
// CTransaction::Start
//
// Starts the handler.  
// -----------------------------------------------------------------------------
//
void CTransaction::Start()
    {
    SetActive ();
    }

// -----------------------------------------------------------------------------
// CTransaction::AutoDelete
//
// Starts an CIdle to delete the instance after the callstack has unrolled.
// -----------------------------------------------------------------------------
//    
EXPORT_C void CTransaction::AutoDelete()
    {
    iAutoDelete->Start (TCallBack (DelayedDelete, this));
    }

// -----------------------------------------------------------------------------
// CTransaction::DelayedDelete
// 
// Deletes the instance after the callstack has unrolled.
// -----------------------------------------------------------------------------
//
TInt CTransaction::DelayedDelete(TAny* aPtr)
    {
    CTransaction* self = static_cast<CTransaction*>(aPtr);

    delete self;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CTransaction::Type
// 
// Returns the handler's type.
// -----------------------------------------------------------------------------
//
EXPORT_C  CTransaction::TTransactionType CTransaction::Type()
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// CTransaction::DebugPrintTables
//
// Print tables of database.
// -----------------------------------------------------------------------------
//
void CTransaction::DebugPrintTables()
    {    
    // Set the request type.
    iType = EPrintDBTables;

    iObserver.SendAsyncCommand(EFeedsServerPrintDBTables, 
            TIpcArgs(), iStatus);

    Start();
    }

// -----------------------------------------------------------------------------
// CTransaction::DisconnectManualUpdateConnection
//
// Disconnect connection provided by client for manual update.
// -----------------------------------------------------------------------------
//
void CTransaction::DisconnectManualUpdateConnection()
    {
    // Set the request type.
    iType = EDisconnectManualUpdateConnection;

    iObserver.SendAsyncCommand (EFeedsServerDisconnectManualUpdateConnection,
            TIpcArgs (), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::ChangeServerSettingsL
//
// Handles the upating of a the feeds server settings.
// -----------------------------------------------------------------------------
//
void CTransaction::ChangeServerSettingsL(const TFeedsServerSetting& aNewSetting)
    {
    // Set the request type.
    iType = EChangeSettings;

    RBufWriteStream  stream;

    // Package the request.
    stream.Open(*iRequestBuff);
    CleanupClosePushL(stream);

    // Write out the settings.  
    // NOTE: Be sure to adjust KSettingsBufSize if more settings are added.
    // TODO: Also write out a version number -- do this with all R-Class messages.
    stream.WriteInt32L( iFolderListId );    
    stream.WriteUint8L(aNewSetting.iAutoUpdate);    
    stream.WriteUint16L(aNewSetting.iAutoUpdateFreq);    
    stream.WriteUint32L(aNewSetting.iAutoUpdateAP);
    stream.WriteUint8L(aNewSetting.iAutoUpdateWhileRoaming);

    iRequestBuff->Compress();
    CleanupStack::PopAndDestroy(/*stream*/);

    // Make the request.
    iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    iObserver.SendAsyncCommand (EFeedsServerChangeSettings,
        TIpcArgs (&iRequestBuffPtr), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::WatchSettingsL
//
// Sets up a notifier to execute when the settings change.
// -----------------------------------------------------------------------------
//
void CTransaction::WatchSettingsL()
	{
	RBufWriteStream stream;

	// Set the request type.
	iType = EWatchForSettingChanges;

	// Package the request.
	stream.Open (*iRequestBuff);
	CleanupClosePushL (stream);

	// Write out the folder list id.
	stream.WriteInt32L (iFolderListId);

	iRequestBuff->Compress ();
	CleanupStack::PopAndDestroy (/*stream*/);

	// Make the request.
	iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

	iObserver.SendAsyncCommand (EFeedsServerWatchSettings,
        TIpcArgs (&iRequestBuffPtr), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::FetchServerSettingsL
//
// Handles the fetching of the feeds server settings.
// -----------------------------------------------------------------------------
//
void CTransaction::FetchServerSettingsL()
    {
    RBufWriteStream stream;

    // Set the request type.
    iType = EFetchSettings;

    // Package the request.
    stream.Open (*iRequestBuff);
    CleanupClosePushL (stream);

    stream.WriteInt32L ( iFolderListId);

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy (/*stream*/);

    iRequestBuffPtr.Set ( iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    iSettingsResponseBuffer->ExpandL(0, KSettingsBufSize);
    iResponsePtr.Set((TUint8 *) iSettingsResponseBuffer->Ptr(0).Ptr(), KSettingsBufSize, 
            KSettingsBufSize);

    // Make the request.
    iObserver.SendAsyncCommand (EFeedsServerGetSettings, TIpcArgs (
            &iRequestBuffPtr, &iResponsePtr), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::FetchServerSettingsSyncL
//
// Handles the fetching of the feeds server settings synchronously.
// -----------------------------------------------------------------------------
//
void CTransaction::FetchServerSettingsSyncL()
    {
    TInt                  status(0);
    CBufFlat*             requestBuff = NULL;
    TPtr8                 requestBuffPtr(NULL, 0);
    CBufFlat*             responseBuff = NULL;
    TPtr8                 responsePtr(NULL, 0);
    RBufWriteStream       stream;

    requestBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(requestBuff);

    // Package the request.
    stream.Open(*requestBuff);
    CleanupClosePushL(stream);

    stream.WriteInt32L( iFolderListId );    

    requestBuff->Compress();
    requestBuffPtr.Set( requestBuff->Ptr(0) );
    CleanupStack::PopAndDestroy(/*stream*/);

    responseBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(responseBuff);

    // Make the request.
    responseBuff->ExpandL(0, KSettingsBufSize);
    responsePtr.Set((TUint8 *) responseBuff->Ptr(0).Ptr(), KSettingsBufSize, 
            KSettingsBufSize);

    status = iFeedInterface->SendReceiveSync(EFeedsServerGetSettings, TIpcArgs(&requestBuffPtr, &responsePtr));

    // Extract the settings if everything went ok.
    if (status == KErrNone)
        {
        iSetting = SettingsFromResponseBuffPtrL(*responseBuff);
        }
        
    CleanupStack::PopAndDestroy(responseBuff);
    CleanupStack::PopAndDestroy(requestBuff);
    }

// -----------------------------------------------------------------------------
// CTransaction::Settings
//
// return pointer to setting map,Caller takes ownership
// -----------------------------------------------------------------------------
//
EXPORT_C  TFeedsServerSetting CTransaction::Settings()
    {
    return iSetting; 
    }

// -----------------------------------------------------------------------------
// CTransaction::CancelAllL
//
// Request the FeedsServer to cancel all activities that can be cancelled.
// -----------------------------------------------------------------------------
//
void CTransaction::CancelAllL()
    {
    // Set the request type.
    iType = ECancelAll;

    // Make the request.
    iObserver.SendAsyncCommand (EFeedsServerCancelAll, TIpcArgs (), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::SettingsFromResponseBufferL
//
// Reads server response buffer and intialises iSetting;
// -----------------------------------------------------------------------------
//
void CTransaction::SettingsFromResponseBufferL()
    {
    RBufReadStream        stream;

    stream.Open(*iSettingsResponseBuffer, 0);
    CleanupClosePushL(stream);

    // Read the settings.  
    // NOTE: Be sure to adjust KSettingsBufSize if more settings are added.
    iSetting.iAutoUpdate = stream.ReadUint8L();
    iSetting.iAutoUpdateFreq = stream.ReadUint16L();
    iSetting.iAutoUpdateAP = stream.ReadUint32L();
    iSetting.iAutoUpdateWhileRoaming = stream.ReadUint8L();

    CleanupStack::PopAndDestroy(/*stream*/);
    }

// -----------------------------------------------------------------------------
// CTransaction::RequestCompletedL
//
// Called to handle the response.
// -----------------------------------------------------------------------------
//
void CTransaction::RequestCompletedL(TInt aStatusCode)
    {
    // Notify the observer if an error occurred.
    this->iTranStatus = aStatusCode;
    if ( aStatusCode != KErrNone)
        {
        iObserver.RequestHandlerCompleted (*this, aStatusCode);
        return;
        }

    switch (iType)
        {
        case EFetchSettings:
            {
            TRAPD(err,SettingsFromResponseBufferL());
            iObserver.RequestHandlerCompleted (*this, err);
            return;
            }
        case EFetchFeed:
            {
            if ( iServerResponseType != EFeedsServerConnectionNeeded &&iServerResponseType != EFeedsServerManualUpdateDone)
                {
                // Append the response chunk to the packed folder.
                if ( iServerResponseType == EFeedsServerTokensPayload)
                    {
                    iPacked->AppendTokenChunkL (iResponsePtr);
                    }
                else
                    {
                    iPacked->AppendStringTableChunkL (iResponsePtr);
                    }

                // If there is more data coming request the next chunk.
                if ( iServerResponseType != EFeedsServerPayloadDone)
                    {
                    // Reset the response.
                    iResponsePtr.Zero ();

                    // Clear the request buffer too.
                    iRequestBuffPtr.Set (NULL, 0);

                    // Get the next chunk asynchronously.
                    iObserver.SendAsyncCommand (EFeedsServerGetFeed, TIpcArgs (
                    		&iRequestBuffPtr, &iServerResponseTypePkg,
                    		&iResponsePtr), iStatus);

                    Start ();
                    }

                // Otherwise process the completed folder.
                else
                    {
                    // Create the new folder and notify the observer.
                    iFeedsEntity = CFeedsEntity::NewL ((CPackedFeed*)iPacked,iFeedInterface);
                    delete iPacked;
                    iPacked = NULL;
                    iObserver.RequestHandlerCompleted (*this, KErrNone);
                    }
                }
            else
                if ( iServerResponseType == EFeedsServerConnectionNeeded)
                    {
                    // Ask client for connection
                    TInt connectionPtr;
                    TInt sockSvrHandle;
                    TBool newConn;
                    TApBearerType bearerType;
                    TRAPD( err, iObserver.NetworkConnectionNeededL( &connectionPtr, sockSvrHandle, newConn, bearerType ) );

                    if ( err == KErrNone)
                        {
                        // Prepare RConnection name
                        RConnection* connPtr=  REINTERPRET_CAST( RConnection*, connectionPtr );
                        TName name;
                        connPtr->Name ( name);

                        // Package the request.
                        RBufWriteStream stream;
                        //
                        stream.Open (*iRequestBuff);
                        CleanupClosePushL (stream);
                        //
                        stream.WriteUint16L ( name.Length ());
                        stream.WriteL ( name);
                        //
                        stream.WriteUint16L ( bearerType);
                        //
                        iRequestBuff->Compress ();
                        CleanupStack::PopAndDestroy (/*stream*/);
                        // Make the request.
                        iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

                        // Reset the response.
                        iResponsePtr.Zero ();

                        // Send the request    
                        iObserver.SendAsyncCommand (
                        		EFeedsServerSetConnection,
                        		TIpcArgs (&iRequestBuffPtr,
                        				&iServerResponseTypePkg, &iResponsePtr),
                        		iStatus);
                        //
                        Start ();
                        }
                    else
                    	{
                    	iObserver.CancelAllCommandsL ();
                    	User::Leave ( err);
                    	}
                	}
                // EFeedsServerManualUpdateDone
                else
                    {
                    iObserver.RequestHandlerCompleted (*this, KErrNone);
                    }
            }
            break;
    	case EFetchRootFolderItem:
            {
            // Append the response chunk to the packed folder.
            if ( iServerResponseType == EFeedsServerTokensPayload)
                {
                iPacked->AppendTokenChunkL (iResponsePtr);
                }
            else
                {
                iPacked->AppendStringTableChunkL (iResponsePtr);
                }

            // If there is more data coming request the next chunk.
            if ( iServerResponseType != EFeedsServerPayloadDone)
                {
                // Reset the response.
                iResponsePtr.Zero ();

                // Clear the request buffer too.
                iRequestBuffPtr.Set (NULL, 0);

                // Get the next chunk asynchronously.
                iObserver.SendAsyncCommand (EFeedsServerGetRootFolder,
                		TIpcArgs (&iRequestBuffPtr, &iServerResponseTypePkg,
                				&iResponsePtr), iStatus);

                Start ();
                }

            // Otherwise process the completed folder.
            else
                {
                // Create the new folder and notify the observer.
                iFeedsEntity = CFeedsEntity::NewL ((CPackedFolder*)iPacked,iFeedInterface);
                delete iPacked;
                iPacked = NULL;

                iObserver.RequestHandlerCompleted (*this, KErrNone);
                }

            break;
            }

    	case EUpdateFolderItem:
            {
            if ( iServerResponseType == EFeedsServerConnectionNeeded)
                {
                // Ask client for connection
                TInt connectionPtr;
                TInt sockSvrHandle;
                TBool newConn;
                TApBearerType bearerType;
                TRAPD( err, iObserver.NetworkConnectionNeededL( &connectionPtr, sockSvrHandle, newConn, bearerType ) );

                if ( err == KErrNone)
                    {
                    // Prepare RConnection name
                    RConnection* connPtr=  REINTERPRET_CAST( RConnection*, connectionPtr );
                    TName name;
                    connPtr->Name ( name);

                    // Package the request.
                    RBufWriteStream stream;
                    //
                    stream.Open (*iRequestBuff);
                    CleanupClosePushL (stream);
                    //
                    stream.WriteUint16L ( name.Length ());
                    stream.WriteL ( name);
                    //
                    stream.WriteUint16L ( bearerType);
                    //
                    iRequestBuff->Compress ();
                    CleanupStack::PopAndDestroy (/*stream*/);
                    // Make the request.
                    iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

                    // Reset the response.
                    iResponsePtr.Zero ();

                    // Send the request    
                    iObserver.SendAsyncCommand (EFeedsServerSetConnection,
                    		TIpcArgs (&iRequestBuffPtr,
                    				&iServerResponseTypePkg, &iResponsePtr),
                    		iStatus);
                    //
                    Start ();
                    }
            	else
                    {
                    iObserver.CancelAllCommandsL ();
                    User::Leave ( err);
                    }
            	}
            // EFeedsServerManualUpdateDone
            else
                {
                iObserver.RequestHandlerCompleted (*this, KErrNone);
                }

            break;
            }

    	case EImportOPML:
            {
            iObserver.RequestHandlerCompleted (*this, KErrNone);
            break;
            }

    	case EExportOPML:
            {
            iObserver.RequestHandlerCompleted (*this, KErrNone);
            break;
            }

    	default:
            iObserver.RequestHandlerCompleted (*this, KErrNone);
            break;
        }

    }

// -----------------------------------------------------------------------------
// CTransaction::GetId
//
// Returns the id of the id of the transaction.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTransaction::GetId()
    {
    return iId;
    }

// -----------------------------------------------------------------------------
// CTransaction::GetStatusCode
//
// Returns the status code of the transaction.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTransaction::GetStatusCode()
    {
    return iTranStatus;
    }

// -----------------------------------------------------------------------------
// CTransaction::Cancel
//
// Cancels the transaction.
// -----------------------------------------------------------------------------
//
EXPORT_C void CTransaction::CancelRequest()
    {
    //TODO
    }

// -----------------------------------------------------------------------------
// CTransaction::FetchFeedL
//
// Fetch the given feed.
// -----------------------------------------------------------------------------
//
void CTransaction::FetchFeedL(const TDesC& aUrl, TBool aForceUpdate,
        TBool aNoCache)
    {
    FetchFeedHelperL (aUrl, 0, aForceUpdate, aNoCache);
    }

// -----------------------------------------------------------------------------
// CTransaction::FetchFeedL
//
// Fetch the given feed.
// -----------------------------------------------------------------------------
//
void CTransaction::FetchFeedL(TInt aFeedId, TBool aForceUpdate,
        TBool aNoCache)
    {
    FetchFeedHelperL (KNullDesC, aFeedId, aForceUpdate, aNoCache);
    }

// -----------------------------------------------------------------------------
// CTransaction::FetchFeedHelperL
//
// Fetch the given feed.
// -----------------------------------------------------------------------------
//
void CTransaction::FetchFeedHelperL(const TDesC& aUrl, TInt aFeedId,
        TBool aForceUpdate, TBool aNoCache)
    {
    RBufWriteStream stream;

    // Set the request type.
    iType = EFetchFeed;

    // Create the buffer to hold the response.
    iResponseBuffer = HBufC8::NewL (KMaxFeedsServerMessage);
    iResponsePtr.Set ((TUint8 *) iResponseBuffer->Ptr(), KMaxFeedsServerMessage, KMaxFeedsServerMessage);

    // Do some other init.
    iServerResponseType = EFeedsServerInitPayload;
    iPacked = CPackedFeed::NewL ();

    // Package the request.
    stream.Open (*iRequestBuff);
    CleanupClosePushL (stream);

    // If the url was provided then write it out.
    if ( aUrl.Length ()> 0)
        {
        // What follows is a url.
        stream.WriteUint8L (ETrue);

        stream.WriteUint16L (aUrl.Length ());
        stream.WriteL (aUrl);

        stream.WriteInt32L ( iFolderListId);
        }

    // Otherwise write out the feed's id.
    else
        {
        // What follows is a feed id.                
        stream.WriteUint8L (EFalse);

        stream.WriteUint16L (aFeedId);
        }

    // Write the load flags.
    stream.WriteUint8L (aForceUpdate);
    stream.WriteUint8L (aNoCache);

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy (/*stream*/);

    // Wrap the request buffer.
    iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    // Get the first chunk asynchronously.
    iObserver.SendAsyncCommand (EFeedsServerGetFeed, TIpcArgs (
            &iRequestBuffPtr, &iServerResponseTypePkg, &iResponsePtr), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::UpdateItemStatusL
//
// Handles the upating of a the feed's item-status.
// -----------------------------------------------------------------------------
//
void CTransaction::UpdateItemStatusL(TInt aFeedId,
        const RArray<TInt>& aItemIds, const RArray<TFeedItemStatus>& aItemStatus,
        TInt aUnreadCount)
    {
    RBufWriteStream stream;

    // Set the request type.
    iType = EUpdateItemStatus;

    // Package the request.
    stream.Open (*iRequestBuff);
    CleanupClosePushL (stream);

    stream.WriteUint16L (aFeedId);

    // Write the entries.
    stream.WriteUint16L (aItemStatus.Count ());
    for (TInt i = 0; i < aItemStatus.Count (); i++)
        {
        stream.WriteUint16L (aItemIds[i]);
        stream.WriteUint16L (aItemStatus[i]);
        }

    stream.WriteUint16L (aUnreadCount);

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy ();

    // Make the request.
    iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    iObserver.SendAsyncCommand (EFeedsServerChangeFeedItemStatus,
        TIpcArgs (&iRequestBuffPtr), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::FetchFolderItemL
//
// Fetch the root folder associated with the given folder list id.
// -----------------------------------------------------------------------------
//
void CTransaction::FetchFolderItemL(TBool aItemTitleNeed)
    {
    FEED_LOG(_L("Feeds"), _L("Feeds_Api.log"), 
        EFileLoggingModeAppend, _L("CTransaction::FetchFolderItemL"));

    RBufWriteStream stream;

    // Set the request type.
    iType = EFetchRootFolderItem;

    // Create the buffer to hold the response.
    iResponseBuffer = HBufC8::NewL (KMaxFeedsServerMessage);
    iResponsePtr.Set ((TUint8 *) iResponseBuffer->Ptr(), KMaxFeedsServerMessage, KMaxFeedsServerMessage);

    // Do some other init.
    iServerResponseType = EFeedsServerInitPayload;
    iPacked = CPackedFolder::NewL ();

    // Package the request.
    stream.Open (*iRequestBuff);
    CleanupClosePushL (stream);

    // Write the folder list id.
    stream.WriteInt32L (iFolderListId);
    stream.WriteUint8L ( aItemTitleNeed);

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy (/*stream*/);

    // Wrap the request buffer.
    iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    // Get the first chunk asynchronously.
    iObserver.SendAsyncCommand (EFeedsServerGetRootFolder, TIpcArgs (
            &iRequestBuffPtr, &iServerResponseTypePkg, &iResponsePtr), iStatus);

    Start ();
    FEED_LOG(_L("Feeds"), _L("Feeds_Api.log"), 
        EFileLoggingModeAppend, _L("//CTransaction::FetchFolderItemL"));

    }

// -----------------------------------------------------------------------------
// CTransaction::AddFolderItemL
//
// Add a new folder item.
// -----------------------------------------------------------------------------
//
void CTransaction::AddFolderItemL(const TDesC& aTitle,const TDesC& aUrl, TBool aIsFolder, const CFeedsEntity& aParent, TInt aFreq)
    {
    RBufWriteStream stream;

    // Set the request type.
    iType = EAddFolderItem;

    // Package the request.
    stream.Open (*iRequestBuff);
    CleanupClosePushL (stream);

    stream.WriteInt32L (iFolderListId);
    stream.WriteUint16L (aParent.GetId ());
    stream.WriteUint16L (aIsFolder);

    stream.WriteUint16L (aTitle.Length ());
    stream.WriteL (aTitle);

    stream.WriteUint16L (aUrl.Length ());
    stream.WriteL (aUrl);
    stream.WriteUint32L (aFreq);

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy (/*stream*/);

    // Make the request.
    iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    iObserver.SendAsyncCommand (EFeedsServerAddFolderItem, TIpcArgs (
        &iRequestBuffPtr, &iResponseEntryIdPkg), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::ChangeFolderItemL
//
// Change a new folder item.
// -----------------------------------------------------------------------------
//
void CTransaction::ChangeFolderItemL(const CFeedsEntity& aFeedsEntity,
        const TDesC& aTitle, const TDesC& aUrl, TInt aFreq)
    {
    RBufWriteStream stream;

    // Set the request type.
    iType = EChangeFolderItem;

    // Package the request.
    stream.Open (*iRequestBuff);
    CleanupClosePushL (stream);

    stream.WriteInt32L (iFolderListId);
    stream.WriteUint16L (aFeedsEntity.GetId ());

    stream.WriteUint16L (aTitle.Length ());
    stream.WriteL (aTitle);

    stream.WriteUint16L (aUrl.Length ());
    stream.WriteL (aUrl);
    stream.WriteUint32L (aFreq);

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy (/*stream*/);

    // Make the request.
    iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    iObserver.SendAsyncCommand (EFeedsServerChangeFolderItem,
            TIpcArgs (&iRequestBuffPtr), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::DeleteFolderItemL
//
// Delete one or more folder items.
// -----------------------------------------------------------------------------
//
void CTransaction::DeleteFolderItemL(
        const RPointerArray<const CFeedsEntity>& aFeedsEntities)
    {
    RBufWriteStream stream;

    // Set the request type.
    iType = EDeleteFolderItem;

    // Package the request.
    stream.Open (*iRequestBuff);
    CleanupClosePushL (stream);

    stream.WriteInt32L (iFolderListId);

    // Write the entries.
    stream.WriteUint16L (aFeedsEntities.Count ());
    for (TInt i = 0; i < aFeedsEntities.Count (); i++)
        {
        stream.WriteUint16L (aFeedsEntities[i]->GetId ());
        }

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy (/*stream*/);

    // Make the request.
    iRequestBuffPtr.Set(iRequestBuff->Ptr(0).Ptr(), iRequestBuff->Size());

    iObserver.SendAsyncCommand(EFeedsServerDeleteFolderItems,
            TIpcArgs(&iRequestBuffPtr), iStatus);

    Start();
    }

// -----------------------------------------------------------------------------
// CTransaction::ExportFolderItemL
//
// Export one or more folder items.
// -----------------------------------------------------------------------------
//
void CTransaction::ExportFolderItemL(
        const RPointerArray<const CFeedsEntity>& aFeedsEntities,
        const TDesC &aExportFileName)
    {

    if ( aExportFileName.Length ()<=0)
        {
        User::Leave (KErrArgument);
        }

    if ( aFeedsEntities.Count ()<=0)
        {
        User::Leave (KErrArgument);
        }

    // Set the request type.
    iType = EExportOPML;

    // Package the request.
    RBufWriteStream stream;
    stream.Open (*iRequestBuff);
    CleanupClosePushL (stream);

    stream.WriteUint16L (aExportFileName.Length ());
    stream.WriteL (aExportFileName);

    stream.WriteInt32L (iFolderListId);

    // Write the entries.
    stream.WriteUint16L (aFeedsEntities.Count ());
    for (TInt i = 0; i < aFeedsEntities.Count (); i++)
        {
        stream.WriteUint16L (aFeedsEntities[i]->GetId ());
        }

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy (/*stream*/);

    // Make the request.
    iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    iObserver.SendAsyncCommand (EFeedsServerExportOPML,
            TIpcArgs (&iRequestBuffPtr), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::MoveFolderItemsToL
//
// Move one or more folder items to a different parent folder.
// -----------------------------------------------------------------------------
//
void CTransaction::MoveFolderItemsToL(
        const RPointerArray<const CFeedsEntity>& aFeedsEntities,
        const CFeedsEntity& aParent)
    {
    RBufWriteStream stream;

    // Set the request type.
    iType = EMoveFolderItemTo;

    // Package the request.
    stream.Open (*iRequestBuff);
    CleanupClosePushL (stream);

    stream.WriteInt32L (iFolderListId);

    // Write the entries.
    stream.WriteUint16L (aParent.GetId ());

    stream.WriteUint16L (aFeedsEntities.Count ());
    for (TInt i = 0; i < aFeedsEntities.Count (); i++)
        {
        stream.WriteUint16L (aFeedsEntities[i]->GetId ());
        }

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy (/*stream*/);

    // Make the request.
    iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    iObserver.SendAsyncCommand (EFeedsServerMoveFolderItemsTo,
            TIpcArgs (&iRequestBuffPtr), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::MoveFolderItemsL
//
// Move one or more folder items to a new position.
// -----------------------------------------------------------------------------
//
void CTransaction::MoveFolderItemsL(
        const RPointerArray<const CFeedsEntity>& aFeedsEntities, TInt aIndex)
    {
    RBufWriteStream stream;

    // Set the request type.
    iType = EMoveFolderItem;

    // Package the request.
    stream.Open (*iRequestBuff);
    CleanupClosePushL (stream);

    stream.WriteInt32L ( iFolderListId );

    // Write the entries.
    stream.WriteUint16L (aIndex);

    stream.WriteUint16L (aFeedsEntities.Count ());
    for (TInt i = 0; i < aFeedsEntities.Count (); i++)
        {
        stream.WriteUint16L (aFeedsEntities[i]->GetId ());
        }

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy (/*stream*/);

    // Make the request.
    iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    iObserver.SendAsyncCommand (EFeedsServerMoveFolderItems,
            TIpcArgs (&iRequestBuffPtr), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::UpdateFolderItemsL
//
// Update one or more feed.
// -----------------------------------------------------------------------------
//
void CTransaction::UpdateFolderItemsL(
        const RPointerArray<const CFeedsEntity>& aFeedsEntities)
    {
    RBufWriteStream stream;

    // Set the request type.
    iType = EUpdateFolderItem;

    // Create the buffer to hold the response.
    iResponseBuffer = HBufC8::NewL (KMaxFeedsServerMessage);
    iResponsePtr.Set ((TUint8 *) iResponseBuffer->Ptr(), KMaxFeedsServerMessage, KMaxFeedsServerMessage);

    // Package the request.
    stream.Open (*iRequestBuff);
    CleanupClosePushL (stream);

    // Write out the folder list id.
    stream.WriteInt32L (iFolderListId);

    // Write out the update all flag.  
    stream.WriteUint8L (EFalse);

    // Write the entries.
    stream.WriteUint16L (aFeedsEntities.Count ());
    for (TInt i = 0; i < aFeedsEntities.Count (); i++)
        {
        stream.WriteUint16L (aFeedsEntities[i]->iFeedId);
        }

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy (/*stream*/);

    // Make the request.
    iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    iObserver.SendAsyncCommand (EFeedsServerUpdateFolderItems, TIpcArgs (
            &iRequestBuffPtr, &iServerResponseTypePkg, &iResponsePtr), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::UpdateFolderItemsL
//
// Update all of the feeds associated with the given folder list id.
// -----------------------------------------------------------------------------
//
void CTransaction::UpdateFolderItemsL()
    {
    RBufWriteStream stream;

    // Set the request type.
    iType = EUpdateFolderItem;

    // Create the buffer to hold the response.
    iResponseBuffer = HBufC8::NewL (KMaxFeedsServerMessage);
    iResponsePtr.Set ((TUint8 *) iResponseBuffer->Ptr(), KMaxFeedsServerMessage, KMaxFeedsServerMessage);

    // Package the request.
    stream.Open (*iRequestBuff);
    CleanupClosePushL (stream);

    // Write out the folder list id.
    stream.WriteInt32L (iFolderListId);

    // Write out the update all flag.  
    stream.WriteUint8L (ETrue);

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy (/*stream*/);

    // Make the request.
    iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    iObserver.SendAsyncCommand (EFeedsServerUpdateFolderItems, TIpcArgs (
            &iRequestBuffPtr, &iServerResponseTypePkg, &iResponsePtr), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::WatchFolderListL
//
// Sets up a notifier to execute when the folder list changes.
// -----------------------------------------------------------------------------
//
void CTransaction::WatchFolderListL()
    {
    RBufWriteStream stream;

    // Set the request type.
    iType = EWatchForChanges;

    // Package the request.
    stream.Open (*iRequestBuff);
    CleanupClosePushL (stream);

    // Write out the folder list id.
    stream.WriteInt32L (iFolderListId);

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy (/*stream*/);

    // Make the request.
    iRequestBuffPtr.Set (iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    iObserver.SendAsyncCommand (EFeedsServerWatchFolderList,
            TIpcArgs (&iRequestBuffPtr), iStatus);

    Start ();
    }

// -----------------------------------------------------------------------------
// CTransaction::ImportOPMLL
//
// Import OPML file passed from the client.
// -----------------------------------------------------------------------------
//
void CTransaction::ImportOPMLL( const TDesC& aPath,
        TBool aClearFolderList)
    {

    TIpcArgs ipcArgs;
    RBufWriteStream stream;

    if ( aPath.Length ()<= 0)
        User::Leave (KErrArgument);

    // Set the request type.
    iType = EImportOPML;

    // Create the buffer to hold the response.
    iResponseBuffer = HBufC8::NewL (KMaxFeedsServerMessage);
    iResponsePtr.Set ((TUint8 *) iResponseBuffer->Ptr(), KMaxFeedsServerMessage, KMaxFeedsServerMessage);

    // correct the path seperator
    TBuf<KMaxFileName> path( aPath);
    for (TInt i = 0; i < aPath.Length (); i++)
        {
        if ( path[i] == '/')
            {
            path[i] = '\\';
            }
        }
    // Package the request.

    stream.Open ( *iRequestBuff);
    CleanupClosePushL ( stream);

    stream.WriteInt32L ( iFolderListId);
    stream.WriteUint8L ( aClearFolderList);

    stream.WriteUint16L (aPath.Length ());
    stream.WriteL (path);

    iRequestBuff->Compress ();
    CleanupStack::PopAndDestroy (/*stream*/);

    // Make the request.
    iRequestBuffPtr.Set ( iRequestBuff->Ptr(0).Ptr (), iRequestBuff->Size ());

    ipcArgs.Set (KFeedsServerPackedRequestArg, &iRequestBuffPtr);

    iObserver.SendAsyncCommand ( EFeedsServerImportOPML, ipcArgs, iStatus);

    Start ();
    }


// -----------------------------------------------------------------------------
// CTransaction::OrphanFeedsEntity
//
// Returns the feedsEntity -- the caller takes ownership.  Only returns a valid 
// folder item after a sucessful call to FetchFeed/FeedFolderItemL
// -----------------------------------------------------------------------------
//
CFeedsEntity* CTransaction::OrphanFeedsEntity()
    {
    CFeedsEntity*  item = NULL;
    
    item = iFeedsEntity;
    iFeedsEntity = NULL;
    
    return item;
    }

// -----------------------------------------------------------------------------
// CTransaction::GetType
//
// Returns transaction type
// -----------------------------------------------------------------------------
//
EXPORT_C CTransaction::TTransactionType CTransaction::GetType() 
    { 
    return iType; 
    }

// -----------------------------------------------------------------------------
// CTransaction::SettingsFromResponseBuffPtrL
//
// Extract the current settings from the servers response.
// -----------------------------------------------------------------------------
//
TFeedsServerSetting CTransaction::SettingsFromResponseBuffPtrL(
        CBufFlat& aResponseBuffer)
    {
    RBufReadStream        stream;
    TFeedsServerSetting  setting;

    // Unpack the response.
    stream.Open(aResponseBuffer, 0);
    CleanupClosePushL(stream);

    // Read the settings.  
    // NOTE: Be sure to adjust KSettingsBufSize if more settings are added.
    setting.iAutoUpdate = stream.ReadUint8L();
    setting.iAutoUpdateFreq = stream.ReadUint16L();
    setting.iAutoUpdateAP = stream.ReadUint32L();
    setting.iAutoUpdateWhileRoaming = stream.ReadUint8L();

    CleanupStack::PopAndDestroy(/*stream*/);

    return setting;    
    }

// -----------------------------------------------------------------------------
// CTransaction::GetSetting
//
// Extract the current settings from the local variable.
// -----------------------------------------------------------------------------
//    
void CTransaction::GetSetting(TFeedsServerSetting&  aSetting)
    {
    aSetting = iSetting;
    }
