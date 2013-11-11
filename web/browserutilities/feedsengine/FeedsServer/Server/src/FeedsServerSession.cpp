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
* Description:  Represent a session on server side.
*
*/


#include <s32mem.h>
#include <uri16.h>

#include "FeedsDatabase.h"
#include "FeedsServer.h"
#include "FeedsServerMsg.h"
#include "FeedsServerSession.h"
#include "SessionHttpConnection.h"
#include "Logger.h"
#include "PackedFeed.h"
#include "PackedFolder.h"
#include <sysutil.h>

_LIT(KTempDirName, "c:\\system\\temp\\");

// -----------------------------------------------------------------------------
// CFeedsServerSession::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsServerSession* CFeedsServerSession::NewL(CFeedsServer& aFeedsServer)
    {
    CFeedsServerSession*  self;
    
    self = new (ELeave) CFeedsServerSession(aFeedsServer);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::NewL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::ConstructL()
    {
    // Create the shared http connection in this sesssion
    iHttpConnection = CSessionHttpConnection::NewL( *this );
    iLazyCaller = CIdle::NewL(CActive::EPriorityIdle);
    iOPMLImportTask = NULL;
    iExportOPMLFileName = NULL;
    iCurrOp = iPrevOp = 0;
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::CFeedsServerSession
//
// C++ default constructor can NOT contain any code that
// might leave.
// -----------------------------------------------------------------------------
//
CFeedsServerSession::CFeedsServerSession(CFeedsServer& aFeedsServer):
        iLeakTracker(CLeakTracker::EFeedsServerSession), iFeedsServer(aFeedsServer), 
        iStoreFeeds(ETrue), iWatchFolderListId(KNoFolderListId),iGetFeedCalled(EFalse)
    {
    iFeedsServer.SessionOpened();
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::~CFeedsServerSession
//
// Destructor.
// -----------------------------------------------------------------------------
//
CFeedsServerSession::~CFeedsServerSession()
    {
    delete iUpdateAllFeedsTask;
    delete iUpdateFeedTask;
    delete iPackedFeed;
    delete iPackedFolder;
    delete iOPMLImportTask;
    delete iExportOPMLFileName;
    delete iLazyCaller;
    if (iHttpConnection != NULL)
        {
        iHttpConnection->CancelAnyConnectionAttempt();
        iHttpConnection->AutoDelete();
        }
    
    iFeedsServer.SessionClosed();
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::StartWait
//
// Notifies the observer that the task is about to start a lengthy 
// operation.  In response the observer could display somekind of 
// status indicator.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::StartWait()
    {
    // Ignore.
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::SetConnection
//
// Set client's connection to use by the session http connection.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::SetConnectionL(const RMessage2& aMessage)
    {
    RBufReadStream  stream;
    CBufFlat*       requestBuff;    
    TInt            nameLength;
    HBufC*          nameBuf;
    TPtr            name( NULL, 0 );
    TInt            bearerType;
    TInt            setConn = EFalse;
    
    // Extract the request.
	requestBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(requestBuff);
	
	TInt len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
	
    aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
        
    // Unpack the request.
    stream.Open(*requestBuff, 0);
    CleanupClosePushL(stream);
    
    nameLength = stream.ReadUint16L();
    nameBuf = HBufC::NewLC( nameLength );
    name.Set( (TUint16*) nameBuf->Ptr(), nameLength, nameLength );
    stream.ReadL( name, nameLength );
    
    bearerType = stream.ReadUint16L();

    // Update the http connection.
    setConn = iHttpConnection->SetConnection( name, bearerType );

	CleanupStack::PopAndDestroy( nameBuf );
	CleanupStack::PopAndDestroy( /*stream*/ );
	CleanupStack::PopAndDestroy( requestBuff );

    if( setConn )
        {
        // make sure there's update task needing connection
        __ASSERT_DEBUG( (iUpdateAllFeedsTask != NULL) || (iUpdateFeedTask != NULL), User::Panic(_L("FeedsSvrSes SetConn"), KErrGeneral) );
        SetClientPendingMessage( aMessage );
        }
    else
        {
        // make sure no update task needing connection when CancelAll occurs bf SetConnection
        __ASSERT_DEBUG( (iUpdateAllFeedsTask == NULL) && (iUpdateFeedTask == NULL), User::Panic(_L("FeedsSvrSes SetConn"), KErrGeneral) );
        aMessage.Complete( KErrCancel );
        }
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::NetworkConnectionNeededL
//
// Called upon connection request.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::NetworkConnectionNeededL()
    {
    // Tell the client that the connection is needed from client app.
    TPckgBuf<TFeedsServerResponseType> responseType( EFeedsServerConnectionNeeded );
    
    User::LeaveIfError( iPendingMessage.Write(KFeedsServerPackedResponseTypeArg, 
            responseType, 0) );
    
    // Notify the client of result.
    CompleteClientPendingMessage();
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::Completed
//
// Called upon completion of the update feed task.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::Completed(CPackedFeed* aPackedFeed, const TDesC& aFeedUrl,TInt aFeedId,
        TInt aStatusCode)
    {

    // Do the real work...
    TRAPD(err, CompletedHelperL(aPackedFeed, aFeedUrl,aFeedId));
    if (err != KErrNone)
        {
        delete aPackedFeed;
        
        FEED_LOG1(_L("Feeds"), _L("Feeds_Errors.log"), 
                EFileLoggingModeAppend, _L("CFeedsServerSession::Completed: %d."), err);

        aStatusCode = err;
        }
    if(aStatusCode != KErrCancel && aStatusCode != KErrNoMemory)
    	{
        // Update the database.
        TRAP_IGNORE(iFeedsServer.Database().UpdateFeedStatusL(aFeedId,aStatusCode));
    	}

    // Solution of bug EMWG-7AC9HK,using CIdle class to send tocken instead sending it directly
    iLazyCaller->Start(TCallBack(LazyCallBack,this));
    // Compact the database.
    iFeedsServer.Database().Compact();
    iPendingStatus = aStatusCode;
    iPendingMessageHandle = iPendingMessage.Handle();
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::CompletedHelperL
//
// Called upon completion of the task -- helper method for Completed.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::CompletedHelperL(CPackedFeed*& aPackedFeed, 
        const TDesC& aFeedUrl,TInt aFeedId)
    {
    // A new packed feed is ready, send the first part to the client.
    if (aPackedFeed != NULL)
        {
        // If need be update the database.
        if (iStoreFeeds)
            {      
            //TInt  feedId;
            
            // Update the database to reflect the feed.
            iFeedsServer.iFeedsDatabase->UpdateFeedL(*aPackedFeed, aFeedUrl,aFeedId, iFolderListId);

            // Extract the feed from the database rather than passing along the 
            // oringal feed as the database contains other feed information.
            delete aPackedFeed;
            aPackedFeed = NULL;

            //(void) iFeedsServer.iFeedsDatabase->FeedIdFromUrlL(aFeedUrl, iFolderListId, feedId);
            
            // Extract the updated feed.
            iPackedFeed = CPackedFeed::NewL();
            iFeedsServer.iFeedsDatabase->ExtractFeedL(aFeedId, *iPackedFeed);
            }    
        
        // If the feed isn't being stored in the database then just use the feed.
        else
            {
            iPackedFeed = aPackedFeed;
            aPackedFeed = NULL;      
            }

        }

    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::UpdateAllFeedsProgress
//
// Called to notify the obsever how many feeds remain to be updated.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::UpdateAllFeedsProgress(TInt /*aMaxCount*/, TInt /*aRemaining*/)
    {
    // TODO:
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::UpdateAllFeedsCompleted
//
// Called upon completion of the task.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::UpdateAllFeedsCompleted(TInt aStatusCode)
    {
    TRAP_IGNORE(UpdateAllFeedsCompletedL(aStatusCode));
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::UpdateAllFeedsCompletedL
//
// Called upon completion of the task.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::UpdateAllFeedsCompletedL(TInt aStatusCode)
    {
    // Delete the task.
    iUpdateAllFeedsTask->AutoDelete();
    iUpdateAllFeedsTask = NULL;
   
    // Compact the database.
    iFeedsServer.Database().Compact();
    
    // Tell the client that the manual update is done.
    TPckgBuf<TFeedsServerResponseType> responseType( EFeedsServerManualUpdateDone );

    User::LeaveIfError( iPendingMessage.Write(KFeedsServerPackedResponseTypeArg, 
            responseType, 0) );

    // Notify the client.
    CompleteClientPendingMessage( aStatusCode );
    
    // Notify all open sessions that the folder list just changed -- the 
    // last-update timestamp.
    if (aStatusCode == KErrNone)
        {        
        iFeedsServer.NotifyFolderListChanged( iUpdateAllFolderListId );
        }
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::ServiceL
//
// Client-server framework calls this function to handle messages from client.  
// This calls DispatchMessageL under the trap harness in order to propagate 
// any errors back to the client (via RMessage2::Complete) rather than leaving.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::ServiceL(const RMessage2& aMessage)
    {
    // Ignore these messages...
    switch (aMessage.Function())
        {
        case EFeedsServerOpenFeedSession:
        case EFeedsServerOpenFolderSession:
            aMessage.Complete(KErrNone);
            return;

        case EFeedsServerCloseFeedSession:
            CancelFeedRequestsL();
            aMessage.Complete(KErrNone);
            return;
		
		case EFeedsServerCompleteWatchFolderList:
            if (iWatchFolderListId != KNoFolderListId)
                {                
                iWatchFolderListMessage.Complete(KErrCancel);
                }
			aMessage.Complete(KErrNone);
            return;

        case EFeedsServerCloseFolderSession:       
            CancelFolderRequestsL();
            aMessage.Complete(KErrNone);
            return;

        case EFeedsServerCompleteWatchSettings:
            if ( iWatchSettingsMessage.IsNull() == EFalse )
                {                
                iWatchSettingsMessage.Complete(KErrCancel);
                }

            aMessage.Complete(KErrNone);
            return;
        }

    // allow following functions while update or updateAll is occuring
    if ( (aMessage.Function() != EFeedsServerCancelAll) &&
         (aMessage.Function() != EFeedsServerSetConnection) &&
         ( (iUpdateAllFeedsTask != NULL) || (iUpdateFeedTask != NULL) ) )
        {
        User::Leave(KErrServerBusy);
        }
        
	        
    // Handle the message.
    // TODO: Does this need to be TRAPed?  I think it "Complete" the message 
    //       with the leave err if it does.  If so the trap can be removed.
    TRAPD(err, DispatchMessageL(aMessage));

    // Propagate any errors back to the client.
    if (err != KErrNone)
        {
        aMessage.Complete(err);
        }
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::DispatchMessageL
//
// Handle messages from the client.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::DispatchMessageL(const RMessage2 &aMessage)
    {
    // Ensure the server is ready.
    iFeedsServer.WakeupServerL();
    iCurrOp = aMessage.Function();

    if ( (!IsSpaceAvailableL()) && ((iCurrOp != EFeedsServerGetRootFolder) ||
                                (iCurrOp != EFeedsServerWatchFolderList) || (iCurrOp != EFeedsServerGetSettings) ||
                                (iCurrOp != EFeedsServerWatchSettings) || (iCurrOp != EFeedsServerSetConnection) ||
                                (iCurrOp != EFeedsServerDisconnectManualUpdateConnection) ||
                                (iCurrOp != EFeedsServerCancelAll) || (iCurrOp != EFeedsServerGetFeed) ||
                                (iCurrOp != EFeedsServerPrintDBTables)))
        {
        User::Leave(KErrNoMemory);
        }

    switch (iCurrOp)
        {
	    case EFeedsServerGetRootFolder:
	        GetRootFolderL(aMessage);
            break;
            
	    case EFeedsServerAddFolderItem:
	        AddFolderItemL(aMessage);
            break;
            
	    case EFeedsServerChangeFolderItem:
	        ChangeFolderItemL(aMessage);
            break;
            
	    case EFeedsServerDeleteFolderItems:
	        DeleteFolderItemsL(aMessage);
            break;
            
	    case EFeedsServerMoveFolderItems:
	        MoveFolderItemsL(aMessage);
            break;
            
	    case EFeedsServerMoveFolderItemsTo:
	        MoveFolderItemsToL(aMessage);
            break;
            
	    case EFeedsServerUpdateFolderItems:
	        UpdateFolderItemsL(aMessage);
            break;
            
        case EFeedsServerWatchFolderList:
            WatchFolderListL(aMessage);
            break;
            
	    case EFeedsServerImportOPML:
	        ImportOPMLAsyncL( aMessage );
            break;

	    case EFeedsServerExportOPML:
	        ExportOPMLL( aMessage );
            break;
            
	    case EFeedsServerGetSettings:
	        GetSettingsL(aMessage);
            break;
            
	    case EFeedsServerChangeSettings:
	        ChangeSettingsL(aMessage);
            break;
            
        case EFeedsServerWatchSettings:
            WatchSettingsL(aMessage);
            break;

	    case EFeedsServerSetConnection:
	        SetConnectionL(aMessage);
            break;

	    case EFeedsServerDisconnectManualUpdateConnection:
	        DisconnectManualUpdateConnection(aMessage);
            break;

	    case EFeedsServerCancelAll:
	        CancelAllRequestsL(aMessage);
            break;
            
	    case EFeedsServerGetFeed:
	        GetFeedL(aMessage);
            break;
            
	    case EFeedsServerChangeFeedItemStatus:
	        ChangeFeedItemStatusL(aMessage);
            break;
            
	    case EFeedsServerPrintDBTables:
            #if defined(_DEBUG)
             //   iFeedsServer.iFeedsDatabase->DebugPrintTablesL();
            #endif

            aMessage.Complete(KErrNone);
            break;
            
        default:
            {
            // TODO:CR RMessage.Panic...
            iFeedsServer.PanicClient(aMessage, EFeedsServerBadRequest);
            break;
            }
        }
        iPrevOp = iCurrOp;
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::GetRootFolderL
//
// Returns the root folder item -- fetching it if need be.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::GetRootFolderL(const RMessage2& aMessage)
    {
    TFeedsServerResponseType         type = EFeedsServerPayloadDone;
    TPckg<TFeedsServerResponseType>  typePkg(type);
	iGetFeedCalled = EFalse;
    iCurrentRequest = 0; 
    // Note: The a packed folder is made up of two buffers, a buffer of tokens
    //       and a string table buffer.  The tokens define the structure of the 
    //       folder-list where the string table contains the folder's data. This
    //       method sends the two buffers serially.  To fetch the entire packed
    //       folder the client makes multiple requests.
    
    // Get the response type
    User::LeaveIfError(aMessage.Read(KFeedsServerPackedResponseTypeArg, typePkg, 0));
    
    // When the response type equals init-payload that indicates that the folder 
    // buffer needs to be fetched and serialized.
    if (type == EFeedsServerInitPayload)
        {        
        RBufReadStream  stream;
        CBufFlat*       requestBuff;
        TInt            folderListId;
        TInt            len;
        TBool           itemTitleNeed;
        
        // Reset the GetRootFolder state.
        delete iPackedFolder;
        iPackedFolder = NULL;
        
        iResponseOffset[iCurrentRequest] = 0;
        iResponseTokensSent[iCurrentRequest] = EFalse;
        
        // Extract the request.
    	requestBuff = CBufFlat::NewL(128);
        CleanupStack::PushL(requestBuff);
    	
    	len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
    	requestBuff->ExpandL(0, len);
    	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
    	
        aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
            
        // Unpack the request.
        stream.Open(*requestBuff, 0);
        CleanupClosePushL(stream);
        
        // Read the feed-url flag
        folderListId = stream.ReadInt32L();
        itemTitleNeed = stream.ReadUint8L();

    	CleanupStack::PopAndDestroy(/*stream*/);
    	CleanupStack::PopAndDestroy(requestBuff);

        // Extract the root folder.
        iPackedFolder = CPackedFolder::NewL();
        iFeedsServer.iFeedsDatabase->ExtractRootFolderL( folderListId, *iPackedFolder, itemTitleNeed );
        
        // Send the first chunk.
        SendTokenChunkL(*iPackedFolder, aMessage);
        aMessage.Complete(KErrNone);
        }
        
    // Otherwise just return the next chunk.
    else
        {
        // Fixed for Bug id - JJUN-78VES7 (FeedsServer crashes under IPC attack)
        if (iPackedFolder)
            {
            // Continue sending the token array.
            if (!iResponseTokensSent[iCurrentRequest])
                {
                SendTokenChunkL(*iPackedFolder, aMessage);
                }
                
            // Otherwise write the next string table chunk.
            else 
                {
                SendStringTableChunkL(*iPackedFolder, aMessage);
                }         
            }
            else
                {
                User::Leave(KErrGeneral);
                }
       
            
        aMessage.Complete(KErrNone);
        }
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::GetFeedL
//
// Returns the requested feed -- fetching it if need be.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::GetFeedL(const RMessage2& aMessage)
    {
    TFeedsServerResponseType         type = EFeedsServerPayloadDone;
    TPckg<TFeedsServerResponseType>  typePkg(type);
    TInt    folderListId = KNoFolderListId;
    const TInt KAutoUpdatingOff = 0;
    const TInt KRootFolderId = 0;

    iCurrentRequest = 1;
    // Note: The a packed feed is made up of two buffers, a buffer of tokens
    //       and a string table buffer.  The tokens define the structure of the 
    //       feed where the string table contains the feed's data. This
    //       method sends the two buffers serially.  To fetch the entire packed
    //       feed the client makes multiple requests.
    
    // Get the response type
    User::LeaveIfError(aMessage.Read(KFeedsServerPackedResponseTypeArg, typePkg, 0));
    
    // When the response type equals init-payload that indicates that the feed 
    // buffer needs to be fetched and serialized.
    if (type == EFeedsServerInitPayload)
        {        
        RBufReadStream  stream;
        CBufFlat*       requestBuff;
  	    HBufC*          feedUrlBuf = NULL;
        TPtr            feedUrl(NULL, 0);
        TInt            len;
        TInt            feedId;        
        TBool           updateNeeded = EFalse;
        TBool           hasFeedUrl;

        // Reset the GetFeed state.
        delete iPackedFeed;
        iPackedFeed = NULL;        
        iResponseOffset[iCurrentRequest] = 0;
        iResponseTokensSent[iCurrentRequest] = EFalse;
        
        // Extract the request.
    	requestBuff = CBufFlat::NewL(128);
        CleanupStack::PushL(requestBuff);
    	
    	len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
    	requestBuff->ExpandL(0, len);
    	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
    	
        aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
            
        // Unpack the request.
        stream.Open(*requestBuff, 0);
        CleanupClosePushL(stream);
        
        // Read the feed-url flag
        hasFeedUrl = stream.ReadUint8L();
        
        // Read the feed's url or id.
        if (hasFeedUrl)
            {
            len = stream.ReadUint16L();
            feedUrlBuf = HBufC::NewLC(len);
            feedUrl.Set((TUint16*) feedUrlBuf->Ptr(), len, len);
            stream.ReadL(feedUrl, len);
            folderListId = stream.ReadInt32L();
            
            // Get the corresponding feed id. If it isn't found then force
            // an update.
            //Gyanendra TODO // should create entry into database
            if (!iFeedsServer.iFeedsDatabase->FeedIdFromUrlL(feedUrl, folderListId, feedId))
                {

                if (!IsSpaceAvailableL())
                    {
                    aMessage.Complete(KErrNoMemory);
                    }

                //Find feed id from folder id
                iFeedsServer.iFeedsDatabase->SetIsFolderTableUpdateNeeded(ETrue);
                TInt entryId = iFeedsServer.iFeedsDatabase->FolderItemAddL(folderListId, feedUrl, feedUrl, EFalse, KRootFolderId, KAutoUpdatingOff);
                iFeedsServer.iFeedsDatabase->FeedIdFromEntryIdL(entryId, folderListId, feedId);
                updateNeeded = ETrue;
                }
            }
        else
            {
            feedId = stream.ReadUint16L();
            
            // Get the corresponding url.
            if (!iFeedsServer.iFeedsDatabase->UrlFromFeedIdL(feedId, feedUrlBuf))
                {
                User::Leave(KErrCorrupt);
                }
                
            feedUrl.Set((TUint16*) feedUrlBuf->Ptr(), feedUrlBuf->Length(), 
                    feedUrlBuf->Length());
            CleanupStack::PushL(feedUrlBuf);
            }
        
        // Read the load flags.
        updateNeeded |= stream.ReadUint8L();
        iStoreFeeds = !(stream.ReadUint8L());
        
        // If the feed is newly created, then set updateNeeded.
        if (!updateNeeded && iFeedsServer.iFeedsDatabase->IsNewlyCreatedL(feedId))
            {
            updateNeeded = ETrue;
            }
        
        // If the feed is in database and its fresh extract it and send it.
        if (!updateNeeded)
            {
            // Get the feed.
            iPackedFeed = CPackedFeed::NewL();
            iFeedsServer.iFeedsDatabase->ExtractFeedL(feedId, *iPackedFeed);
            
            // Send the first chunk.
            SendTokenChunkL(*iPackedFeed, aMessage);
            aMessage.Complete(KErrNone);
            }

        // Otherwise create a task to update the feed.
        else
            {
            // Abort the updation of feeds under low memory.
            if ( IsSpaceAvailableL() )
                {

                // If need be clean up the previous task.
                delete iUpdateFeedTask;
                iUpdateFeedTask = NULL;

                // Start a new task to update the feed.
                iUpdateFeedTask = CUpdateFeedTask::NewL(iFeedsServer, feedUrl,feedId, *this);    
                SetClientPendingMessage( aMessage );
                // When FetchFeedL called with FeedId, we don't know its folder list Id
                if( folderListId == KNoFolderListId )
                    {
                    iFeedsServer.iFeedsDatabase->FolderListIdFromFeedIdL( feedId, folderListId );
                    }
                iFolderListId = folderListId;

                TRAPD(err, iUpdateFeedTask->StartTaskL());
                if (err != KErrNone)
                    {
                    delete iUpdateFeedTask;
                    iUpdateFeedTask = NULL;
                    if(err != KErrCancel && err != KErrNoMemory)
            	        {
    	        	    iFeedsServer.Database().UpdateFeedStatusL(feedId,err);
        	        	NotifyFolderListChanged(folderListId);
                    	}
                    CompleteClientPendingMessage(err);
                    }
                }
            else
                {
                aMessage.Complete(KErrNoMemory);
                }
            }
            
        CleanupStack::PopAndDestroy(feedUrlBuf);
    	CleanupStack::PopAndDestroy(/*stream*/);
    	CleanupStack::PopAndDestroy(requestBuff);
        }
        
    // Otherwise just return the next chunk.
    else
        {
	        if(iPackedFeed)
	        	{
	        	// Continue sending the token array.
		        if (!iResponseTokensSent[iCurrentRequest])
		            {
		            SendTokenChunkL(*iPackedFeed, aMessage);
		            }
		            
		        // Otherwise write the next string table chunk.
		        else 
		            {
		            SendStringTableChunkL(*iPackedFeed, aMessage);
		            }
	            
	        	aMessage.Complete(KErrNone);	
	        	}
	        else
	        	{
	        	aMessage.Complete(KErrArgument);
	        	}
        
        }
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::CancelAllRequestsL
//
// Cancel all activities that can be cancelled.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::CancelAllRequestsL(const RMessage2& aMessage)
    {
    CancelFeedRequestsL();
    CancelFolderRequestsL();

    iHttpConnection->CancelAnyConnectionAttempt();

    aMessage.Complete(KErrNone);
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::CancelFeedRequestsL
//
// Cancel all feed related activities that can be cancelled.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::CancelFeedRequestsL()
    {
    // Cancel the update task.
    if (iUpdateFeedTask != NULL)
        {
        delete iUpdateFeedTask;
        iUpdateFeedTask = NULL;

        CompleteClientPendingMessage( KErrCancel );
        }
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::CancelFolderRequestsL
//
// Cancel all folder related activities that can be cancelled.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::CancelFolderRequestsL()
    {
    // Cancel the update all task.
    if (iUpdateAllFeedsTask != NULL)
        {
        delete iUpdateAllFeedsTask;
        iUpdateAllFeedsTask = NULL;
        
        CompleteClientPendingMessage( KErrCancel );
        }
    else if(iOPMLImportTask != NULL)
	    {
        delete iOPMLImportTask;
        iOPMLImportTask = NULL;
    	
    	CompleteClientPendingMessage( KErrCancel );
    	}
    	
    if(iPrevOp == EFeedsServerExportOPML)
    	{
   		// Delete the exported file
		TBuf<KMaxFileName>  path(KTempDirName);
		path.Append(iExportOPMLFileName->Des());
		RFs rfs;

		User::LeaveIfError(rfs.Connect());
		CleanupClosePushL(rfs);

		rfs.Delete(path);
		
		CleanupStack::PopAndDestroy(/*rfs*/);
    	}
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::ChangeFeedItemStatusL
//
// Updates the given feed's item status.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::ChangeFeedItemStatusL(const RMessage2& aMessage)
    {
    RBufReadStream  stream;
    CBufFlat*       requestBuff;
    TInt            feedId;
    TInt            itemCount;
    RArray<TInt>    itemStatus;
    RArray<TInt>    itemIds;
    TInt            unreadCount;
    
    // Extract the request.
	requestBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(requestBuff);
	
	TInt len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
	
    aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
        
    // Unpack the request.
    stream.Open(*requestBuff, 0);
    CleanupClosePushL(stream);
    
    feedId = stream.ReadUint16L();
    itemCount = stream.ReadUint16L();

    // Extract the updated item status.
    CleanupClosePushL(itemIds);
    CleanupClosePushL(itemStatus);
    for (TInt i = 0; i < itemCount; i++)
        {
        User::LeaveIfError(itemIds.Append(stream.ReadUint16L()));
        User::LeaveIfError(itemStatus.Append(stream.ReadUint16L()));
        }

    unreadCount = stream.ReadUint16L();

    // Update the items status.
    iFeedsServer.Database().FeedUpdateItemStatusL(feedId, itemIds, itemStatus, unreadCount);

    // Clean up.
	CleanupStack::PopAndDestroy(/*itemStatus*/);
	CleanupStack::PopAndDestroy(/*itemIds*/);
	CleanupStack::PopAndDestroy(/*stream*/);
	CleanupStack::PopAndDestroy(requestBuff);

    // Send the response.
    aMessage.Complete(KErrNone);
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::AddFolderItemL
//
// Updates the database to reflect the addition of the folder item.
// A update root folder item is sent out to all active
// folder item sessions.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::AddFolderItemL(const RMessage2& aMessage)
    {
    RBufReadStream  stream;
    CBufFlat*       requestBuff;
    TInt            titleLength;
    HBufC*          titleBuf;
    TInt            urlLength;
    HBufC*          urlBuf;
    TBool           isFolder;
    TInt            parentEntryId;
    TInt            folderListId;
    TInt            entryId = 0;
    TPtr            title(NULL, 0);
    TPtr            url(NULL, 0);
	TInt            freq;
    
    // Extract the request.
	requestBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(requestBuff);
	
	TInt len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
	
    aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
        
    // Unpack the request.
    stream.Open(*requestBuff, 0);
    CleanupClosePushL(stream);
    
    folderListId = stream.ReadInt32L();
    parentEntryId = stream.ReadUint16L();
    isFolder = stream.ReadUint16L();

    titleLength = stream.ReadUint16L();
    titleBuf = HBufC::NewLC(titleLength);
    title.Set((TUint16*) titleBuf->Ptr(), titleLength, titleLength);
    stream.ReadL(title, titleLength);
    
    // Fixed for Bug id - JJUN-78VES7 (FeedsServer crashes under IPC attack)
    if ( titleLength > KMaxTitleLength )
        {
            User::Leave(KErrOverflow);
        }
    urlLength = stream.ReadUint16L();
    urlBuf = HBufC::NewLC(urlLength);
    url.Set((TUint16*) urlBuf->Ptr(), urlLength, urlLength);
    stream.ReadL(url, urlLength);
    
    // If the title isn't set then use the url's host name.
    if (title.Length() == 0)
        {
        TUriParser  uriParser;
        TInt        err;
    
        // Parse the url.  If the url isn't valid then just leave the title blank.
        err = uriParser.Parse(url);
        
        if (err == KErrNone)
            {            
            // Get the host name
            const TDesC& host = uriParser.Extract(EUriHost);        	
            const TDesC& path = uriParser.Extract(EUriPath); 

            if (host.Length() > 0)
                {
                TInt len = host.Length();
                if (path.Length() > 0)
                    {
                    len += path.Length();
                    }

                // Free the old title.
            	CleanupStack::Pop(urlBuf);
            	CleanupStack::PopAndDestroy(titleBuf);
            	CleanupStack::PushL(urlBuf);
            	    
            	titleBuf = HBufC::NewL(len);
            	titleBuf->Des().Append( host );
                if (path.Length() > 0)
                    {
                	titleBuf->Des().Append( path );
                    }
          	    title.Set((TUint16*) titleBuf->Ptr(), len, len);

            	CleanupStack::Pop(urlBuf);
            	CleanupStack::PushL(titleBuf);
            	CleanupStack::PushL(urlBuf);    	
                }
            }
        }
	  freq = stream.ReadUint32L();
    // Insert the new entry.
    entryId = iFeedsServer.Database().FolderItemAddL(folderListId, title, url, 
            isFolder, parentEntryId, freq);

    // Write the entry's id.  This returns the new entry's id to the client.
    TPckgBuf<TInt> entryIdPckg(entryId);

    User::LeaveIfError(aMessage.Write(1, entryIdPckg, 0));
    
    // Clean up.
	CleanupStack::PopAndDestroy(urlBuf);
	CleanupStack::PopAndDestroy(titleBuf);
	CleanupStack::PopAndDestroy(/*stream*/);
	CleanupStack::PopAndDestroy(requestBuff);
    
    // Send the response.
    aMessage.Complete(KErrNone);

    // Notify all open sessions that the folder list just changed.
    iFeedsServer.NotifyFolderListChanged(folderListId);
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::ChangeFolderItemL
//
// Updates the database to reflect the edited folder item.
// A update root folder item is sent out to all active
// folder item sessions.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::ChangeFolderItemL(const RMessage2& aMessage)
    {
    RBufReadStream  stream;
    CBufFlat*       requestBuff;
    TInt            folderListId;    
    TInt            entryId;
    TInt            titleLength;
    HBufC*          titleBuf;
    TInt            urlLength;
    HBufC*          urlBuf;
    TPtr            title(NULL, 0);
    TPtr            url(NULL, 0);
	TInt            freq;
    
    // Extract the request.
	requestBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(requestBuff);
	
	TInt len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
	
    aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
        
    // Unpack the request.
    stream.Open(*requestBuff, 0);
    CleanupClosePushL(stream);
    
    folderListId = stream.ReadInt32L();
    entryId = stream.ReadUint16L();

    titleLength = stream.ReadUint16L();
    titleBuf = HBufC::NewLC(titleLength);
    title.Set((TUint16*) titleBuf->Ptr(), titleLength, titleLength);
    stream.ReadL(title, titleLength);
    
    urlLength = stream.ReadUint16L();
    urlBuf = HBufC::NewLC(urlLength);
    url.Set((TUint16*) urlBuf->Ptr(), urlLength, urlLength);
    stream.ReadL(url, urlLength);    

	freq = stream.ReadUint32L();
    // Insert the new entry.
    iFeedsServer.Database().FolderItemUpdateL(entryId, title, url, freq);

    // Clean up.
	CleanupStack::PopAndDestroy(urlBuf);
	CleanupStack::PopAndDestroy(titleBuf);
	CleanupStack::PopAndDestroy(/*stream*/);
	CleanupStack::PopAndDestroy(requestBuff);

    // Send the response.
    aMessage.Complete(KErrNone);

    // Notify all open sessions that the folder list just changed.
    iFeedsServer.NotifyFolderListChanged(folderListId);
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::DeleteFolderItemsL
//
// Updates the database to reflect the deletion of the folder item.
// A update root folder item is sent out to all active
// folder item sessions.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::DeleteFolderItemsL(const RMessage2& aMessage)
    {
    RBufReadStream  stream;
    TInt            folderListId;    
    TInt            entryCount;
    CBufFlat*       requestBuff;
    RArray<TInt>    entries;    
        
    // Extract the request.
	requestBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(requestBuff);
	
	TInt len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
	
    aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
        
    // Unpack the request.
    stream.Open(*requestBuff, 0);
    CleanupClosePushL(stream);
    
    folderListId = stream.ReadInt32L();
    entryCount = stream.ReadUint16L();

    // Extract the ids of the entries to delete.
    CleanupClosePushL(entries);
    for (TInt i = 0; i < entryCount; i++)
        {
        User::LeaveIfError(entries.Append(stream.ReadUint16L()));
        }

    // Delete the items.
    iFeedsServer.Database().FolderItemDeleteL(entries,ETrue);
    iFeedsServer.Database().Compact();

    // Clean up.
	CleanupStack::PopAndDestroy(/*entries*/);
	CleanupStack::PopAndDestroy(/*stream*/);
	CleanupStack::PopAndDestroy(requestBuff);

    // Send the response.
    aMessage.Complete(KErrNone);

    // Notify all open sessions that the folder list just changed.
    iFeedsServer.NotifyFolderListChanged(folderListId);
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::MoveFolderItemsL
//
// Updates the database to reflect the moved folder items.
// A update root folder item is sent out to all active
// folder item sessions.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::MoveFolderItemsL(const RMessage2& aMessage)
    {
    RBufReadStream  stream;
    CBufFlat*       requestBuff;
    TInt            folderListId;
    TInt            targetIndex;
    TInt            entryCount;
    RArray<TInt>    entries;
    
    // Extract the request.
	requestBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(requestBuff);
	
	TInt len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
	
    aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
        
    // Unpack the request.
    stream.Open(*requestBuff, 0);
    CleanupClosePushL(stream);
    
    folderListId = stream.ReadInt32L();
    targetIndex = stream.ReadUint16L();
    entryCount = stream.ReadUint16L();

    // Extract the ids of the entries to move.
    CleanupClosePushL(entries);
    for (TInt i = 0; i < entryCount; i++)
        {
        User::LeaveIfError(entries.Append(stream.ReadUint16L()));
        }

    // Move the entries.
    iFeedsServer.Database().FolderItemMoveL(entries, targetIndex);

    // Clean up.
	CleanupStack::PopAndDestroy(/*entries*/);
	CleanupStack::PopAndDestroy(/*stream*/);
	CleanupStack::PopAndDestroy(requestBuff);

    // Send the response.
    aMessage.Complete(KErrNone);

    // Notify all open sessions that the folder list just changed.
    iFeedsServer.NotifyFolderListChanged(folderListId);
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::MoveFolderItemsToL
//
// Updates the database to reflect the new parentage of the 
// folder items.  A update root folder item is sent out to 
// all active folder item sessions.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::MoveFolderItemsToL(const RMessage2& aMessage)
    {
    RBufReadStream  stream;
    CBufFlat*       requestBuff;
    TInt            folderListId;
    TInt            parentId;
    TInt            entryCount;
    RArray<TInt>    entries;
    
    // Extract the request.
	requestBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(requestBuff);
	
	TInt len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
	
    aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
        
    // Unpack the request.
    stream.Open(*requestBuff, 0);
    CleanupClosePushL(stream);
    
    folderListId = stream.ReadInt32L();
    parentId = stream.ReadUint16L();
    entryCount = stream.ReadUint16L();

    // Extract the ids of the entries to move.
    CleanupClosePushL(entries);
    for (TInt i = 0; i < entryCount; i++)
        {
        User::LeaveIfError(entries.Append(stream.ReadUint16L()));
        }

    // Move each of the entries to the new parent.
    iFeedsServer.Database().FolderItemMoveToL(entries, parentId);

    // Clean up.
	CleanupStack::PopAndDestroy(/*entries*/);
	CleanupStack::PopAndDestroy(/*stream*/);
	CleanupStack::PopAndDestroy(requestBuff);

    // Send the response.
    aMessage.Complete(KErrNone);

    // Notify all open sessions that the folder list just changed.
    iFeedsServer.NotifyFolderListChanged(folderListId);
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::UpdateFolderItemsL
//
// Update the given folder items or all feeds associated with the folder list id.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::UpdateFolderItemsL(const RMessage2& aMessage)
    {
    RBufReadStream  stream;
    CBufFlat*       requestBuff;
    TInt            folderListId;
    TBool           isFolderListId;
    TInt            err;
    
    // Extract the request.
	requestBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(requestBuff);
	
	TInt len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
	
    aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
        
    // Unpack the request.
    stream.Open(*requestBuff, 0);
    CleanupClosePushL(stream);
    
    folderListId = stream.ReadInt32L();

        // If need be clean up the previous task.
        delete iUpdateAllFeedsTask;
        iUpdateAllFeedsTask = NULL;
    
        // The update request is associated with a folder-list.
        isFolderListId = stream.ReadUint8L();
        if (isFolderListId)
            {        
            // Create a task to update all of the feeds.
            iUpdateAllFeedsTask = CUpdateAllFeedsTask::NewL(iFeedsServer, folderListId, *this);    
            iUpdateAllFolderListId = folderListId;
            SetClientPendingMessage( aMessage );

            TRAP(err, iUpdateAllFeedsTask->StartTaskL());
            if (err != KErrNone)
                {
                delete iUpdateAllFeedsTask;
                iUpdateAllFeedsTask = NULL;
            
                aMessage.Complete(err);
                }
            }
        
        // Otherwise update the given folder items.
        else
            {        
            TInt          itemCount;
            RArray<TInt>  entries;

            CleanupClosePushL(entries);

            // Extract the updated item status.
            itemCount = stream.ReadUint16L();

            for (TInt i = 0; i < itemCount; i++)
                {
                User::LeaveIfError(entries.Append(stream.ReadUint16L()));
                }

            // Create a task to update the feeds.
            iUpdateAllFeedsTask = CUpdateAllFeedsTask::NewL(iFeedsServer, folderListId, entries, *this);    
            iUpdateAllFolderListId = folderListId;
            SetClientPendingMessage( aMessage );

            TRAP(err, iUpdateAllFeedsTask->StartTaskL());
            if (err != KErrNone)
                {
                delete iUpdateAllFeedsTask;
                iUpdateAllFeedsTask = NULL;
            
                aMessage.Complete(err);
                }

    	    CleanupStack::PopAndDestroy(/*entries*/);    	    	
            }

    
    // Clean up.
	CleanupStack::PopAndDestroy(/*stream*/);
	CleanupStack::PopAndDestroy(requestBuff);
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::GetSettingsL
//
// Returns the settings.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::GetSettingsL(const RMessage2& aMessage)
    {
    CBufFlat*        buff;
    RBufWriteStream  stream;
    TBool            autoUpdate;
    TBool            autoUpdateWhileRoaming;
    TInt             autoUpdateFreq;
    TUint32          autoUpdateAP;
    CBufFlat*        requestBuff;    
    RBufReadStream   readStream;
    TInt             folderListId;
    
    // Extract the request.
	requestBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(requestBuff);
	
	TInt len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
	
    aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
        
    // Unpack the request.
    readStream.Open(*requestBuff, 0);
    CleanupClosePushL(readStream);
    
    folderListId = readStream.ReadInt32L();    
    iFolderListId = folderListId;

	CleanupStack::PopAndDestroy(/*stream*/);
	CleanupStack::PopAndDestroy(requestBuff);

    // Extract the settings.
    iFeedsServer.Database().ExtractAutoUpdateSettingsL( folderListId, autoUpdate, autoUpdateFreq, 
            autoUpdateAP, autoUpdateWhileRoaming );

    // Package the settings.
    buff = CBufFlat::NewL(128);
    CleanupStack::PushL(buff);

    stream.Open(*buff);
    CleanupClosePushL(stream);

    // Write out the settings.
    stream.WriteUint8L(autoUpdate);
    stream.WriteUint16L(autoUpdateFreq);    
    stream.WriteUint32L(autoUpdateAP);
    stream.WriteUint8L(autoUpdateWhileRoaming);

    buff->Compress();
	CleanupStack::PopAndDestroy(/*stream*/);

    // Send the response.
    TPtrC8 ptr(buff->Ptr(0).Ptr(), buff->Size());

    User::LeaveIfError(aMessage.Write(1, ptr, 0));
    aMessage.Complete(KErrNone);

	CleanupStack::PopAndDestroy(buff);
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::ChangeSettingsL
//
// Updates the database to reflect the new settings.
// A update settings is sent out to all active
// server sessions.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::ChangeSettingsL(const RMessage2& aMessage)
    {
    RBufReadStream  stream;
    CBufFlat*       requestBuff;    
    TBool           autoUpdate;
    TBool           autoUpdateWhileRoaming;
    TInt            autoUpdateFreq;
    TUint32         autoUpdateAP;
    TInt            folderListId;
    
    // Extract the request.
	requestBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(requestBuff);
	
	TInt len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
	
    aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
        
    // Unpack the request.
    stream.Open(*requestBuff, 0);
    CleanupClosePushL(stream);
    
    folderListId = stream.ReadInt32L();    
    autoUpdate = stream.ReadUint8L();
    autoUpdateFreq = stream.ReadUint16L();
    autoUpdateAP = stream.ReadUint32L();    
    autoUpdateWhileRoaming = stream.ReadUint8L();    

    // Update the server.
    iFeedsServer.SetAutoUpdateSettingsL( folderListId, autoUpdate, autoUpdateFreq, autoUpdateAP, autoUpdateWhileRoaming );

	CleanupStack::PopAndDestroy(/*stream*/);
	CleanupStack::PopAndDestroy(requestBuff);

    // Send the response.
    aMessage.Complete(KErrNone);
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::WatchSettingsL
//
// Sets up a notifier to execute when settings changes.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::WatchSettingsL(const RMessage2& aMessage)
    {
    RBufReadStream  stream;
    CBufFlat*       requestBuff;   

    // Extract the request.
	requestBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(requestBuff);
	
	TInt len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
	
    aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
        
    // Unpack the request.
    stream.Open(*requestBuff, 0);
    CleanupClosePushL(stream);
    
    TInt folderListId = stream.ReadInt32L();
    
	CleanupStack::PopAndDestroy(/*stream*/);
	CleanupStack::PopAndDestroy(requestBuff);

    // We should never have more than a single outstanding asynchronous request
    __ASSERT_DEBUG( iWatchSettingsMessage.IsNull(), User::Panic(_L("Feeds Server Session"), KErrGeneral) );
    // Store the watch info.
    iWatchSettingsMessage = aMessage;
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsServerSession::NotifySettingsChanged
//
// Notify the session that the settings associated with the folder list changed.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::NotifySettingsChanged(TInt aFolderListId)
    {
    if ((aFolderListId == iFolderListId) || (aFolderListId == KAllFolderListId))
        {
        if ( iWatchSettingsMessage.IsNull() == EFalse )
            {           
            iWatchSettingsMessage.Complete(KErrNone);
            }
        }
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::DisconnectManualUpdateConnection
//
// Disconnect the session http connection provided by client app.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::DisconnectManualUpdateConnection(const RMessage2& aMessage)
    {
    iHttpConnection->Disconnect();

    // Send the response.
    aMessage.Complete(KErrNone);
    }


// -----------------------------------------------------------------------------
// CFeedsServerSession::WatchFolderListL
//
// Sets up a notifier to execute when the folder list changes.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::WatchFolderListL(const RMessage2& aMessage)
    {
    RBufReadStream  stream;
    CBufFlat*       requestBuff;   
    TInt            folderListId; 

    // Extract the request.
	requestBuff = CBufFlat::NewL(128);
    CleanupStack::PushL(requestBuff);
	
	TInt len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);
	
    aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);
        
    // Unpack the request.
    stream.Open(*requestBuff, 0);
    CleanupClosePushL(stream);
    
    folderListId = stream.ReadInt32L();
    
	CleanupStack::PopAndDestroy(/*stream*/);
	CleanupStack::PopAndDestroy(requestBuff);

    // Store the watch info.
    iWatchFolderListId = folderListId;
    iWatchFolderListMessage = aMessage;
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsServerSession::NotifyFolderListChanged
//
// Notify the session that the given folder-list changed.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::NotifyFolderListChanged(TInt aFolderListId)
    {
    // TODO: Improve this so multiple folder-lists can be watched from
    //       a single session i.e. store this info in an array.  The client
    //       is allowed to open multiple RFolderItems on the same RFeedsServer
    //       instance.

    if ((aFolderListId == iWatchFolderListId) || (aFolderListId == KAllFolderListId))
        {
        if (iWatchFolderListId != KNoFolderListId)
            {           
            iWatchFolderListId = KNoFolderListId;
            iWatchFolderListMessage.Complete(KErrNone);
            }
        }
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsServerSession::SendTokenChunkL
//
// Send the next chunk of the token-array to the client.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::SendTokenChunkL(CPacked& aPacked, const RMessagePtr2& aMessage)
    {
    // Determine how much to write.  This is done by comparing the size of the client's
    // receptor buffer to the amount left to write.
    TInt  size = KMaxFeedsServerMessage;
    TInt  clientSize;
    
    clientSize = aMessage.GetDesMaxLengthL(KFeedsServerPackedResponseArg);
    if (clientSize < KMaxFeedsServerMessage)
        {
        size = clientSize;
        }
        
    if ((aPacked.iTokenArrayAsDes.Length() - iResponseOffset[iCurrentRequest]) < size)
        {
        size = aPacked.iTokenArrayAsDes.Length() - iResponseOffset[iCurrentRequest];
        }

    // Tell the client that the payload contains token info.
    TPckgBuf<TFeedsServerResponseType> responseType(EFeedsServerTokensPayload);
    
    User::LeaveIfError(aMessage.Write(KFeedsServerPackedResponseTypeArg, 
            responseType, 0));

    // TODO: Panic the client if the buffer isn't an increment of sizeof(TUint).
    
    // Write the next chunk of token info.
    TPtrC8  buff(aPacked.iTokenArrayAsDes.Ptr() + iResponseOffset[iCurrentRequest], size);
    
    User::LeaveIfError(aMessage.Write(KFeedsServerPackedResponseArg, buff, 0));
    iResponseOffset[iCurrentRequest] += size;

    // If the entire token buffer is written then get ready to write the string table.
    if (iResponseOffset[iCurrentRequest] >= aPacked.iTokenArrayAsDes.Length())
        {
        iResponseTokensSent[iCurrentRequest] = ETrue;
        iResponseOffset[iCurrentRequest] = 0;
        }
    }
    
            
// -----------------------------------------------------------------------------
// CFeedsServerSession::SendStringTableChunkL
//
// Send the next chunk of the string-table to the client.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::SendStringTableChunkL(CPacked& aPacked, const RMessage2& aMessage)
    {
    TPtrC8  stringTable8Bit((const TUint8*) aPacked.iStringTable->Ptr(), 
            aPacked.iStringTable->Size());
            
    // Determine how much to write.  This is done by comparing the size of the client's
    // receptor buffer to the amount left to write.
    TInt  size = KMaxFeedsServerMessage;
    TInt  clientSize;
    
    clientSize = aMessage.GetDesMaxLengthL(KFeedsServerPackedResponseArg);
    if (clientSize < KMaxFeedsServerMessage)
        {
        size = clientSize;
        }
        
    if ((stringTable8Bit.Length() - iResponseOffset[iCurrentRequest]) < size)
        {
        size = stringTable8Bit.Length() - iResponseOffset[iCurrentRequest];
        }

    // TODO: Panic the client if the buffer isn't an increment of sizeof(TUint16).
    
    // Write the next chunk of the string table.
    TPtrC8  buff(stringTable8Bit.Ptr() + iResponseOffset[iCurrentRequest], size);
    
    User::LeaveIfError(aMessage.Write(KFeedsServerPackedResponseArg, buff, 0));
    iResponseOffset[iCurrentRequest] += size;

    // Tell the client that the payload contains a chunk of the string table.
    if (iResponseOffset[iCurrentRequest] < stringTable8Bit.Length())
        {
        TPckgBuf<TFeedsServerResponseType> responseType(EFeedsServerStringTablePayload);
        
        User::LeaveIfError(aMessage.Write(KFeedsServerPackedResponseTypeArg, 
                responseType, 0));
        }
        
    // Otherwise tell the client that the payload contains a chunk of the 
    // string table and this is the last payload.
    else
        {
        TPckgBuf<TFeedsServerResponseType> responseType(EFeedsServerPayloadDone);
        
        User::LeaveIfError(aMessage.Write(KFeedsServerPackedResponseTypeArg, 
                responseType, 0));
                
		if(iGetFeedCalled)
			{
    	    // Notify all open sessions that the folder list just changed.
    		iFeedsServer.NotifyFolderListChanged(iWatchFolderListId);
    		iGetFeedCalled = EFalse;
			}
        }
   }

// -----------------------------------------------------------------------------
// CFeedsServerSession::ImportOPMLL
//
// Import OPML file passed from the client asyncronously.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::ImportOPMLAsyncL( const RMessage2& aMessage )
	{
		SetClientPendingMessage( aMessage );
		iOPMLImportTask = CImportFeedsTask::NewL(iFeedsServer, *this, aMessage);
		ExtractClientImportMessageL();
		iOPMLImportTask->StartTaskL();
	}


// -----------------------------------------------------------------------------
// CImportFeedsTask::ExtractClientImportMessageL.
//
// Extract the client passes message
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::ExtractClientImportMessageL()
	{
	RBufReadStream	stream;
	TInt			messageLen(0);

    if( iPendingMessage.IsNull())
	    {
    	User::Leave(KErrArgument);
	    }

	CBufFlat *requestBuff = CBufFlat::NewL(512);
	CleanupStack::PushL(requestBuff);

	messageLen = iPendingMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	
	requestBuff->ExpandL(0, messageLen);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), messageLen);

	iPendingMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);

	// Unpack the request.
	stream.Open(*requestBuff, 0);
	CleanupClosePushL(stream);

	iOPMLImportTask->iFolderListId = stream.ReadInt32L();
	iOPMLImportTask->iClearFolderList = stream.ReadUint8L();

	TInt pathLen = stream.ReadUint16L();
	
	iOPMLImportTask->iFullImportFileName = HBufC::NewL(pathLen);
	TPtr str = iOPMLImportTask->iFullImportFileName->Des();
	stream.ReadL(str, pathLen);

    //Chop-off the path, to get the display name
    TInt Pos = str.LocateReverse('\\');
	if(Pos != KErrNotFound)
    	{    	
    	iOPMLImportTask->iDispImportFileName = str.Mid(Pos+1, Min(KMaxFileName, (str.Length() - (Pos + 1))));
    	}
    	else
    	{
    	iOPMLImportTask->iDispImportFileName = str.Mid(0, Min(KMaxFileName, str.Length()));	
    	}

    CleanupStack::PopAndDestroy(/*stream*/);
    CleanupStack::PopAndDestroy(/*requestBuff*/);
	}


// -----------------------------------------------------------------------------
// CFeedsServerSession::ImportOPMLL
//
// Import OPML file passed from the client.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::ImportOPMLL( const RMessage2& aMessage )
    {
    RBufReadStream      stream;
    TInt                len;
    TInt                folderListId;
    TBool               clearFolderList = EFalse;
    TInt                err = KErrNone;
    TPtr           		fileName(NULL, 0);
    
    // Extract the request.
    CBufFlat *requestBuff = CBufFlat::NewL(512);
    CleanupStack::PushL(requestBuff);

	len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8  reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);

	aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);

	// Unpack the request.
	stream.Open(*requestBuff, 0);
	CleanupClosePushL(stream);

	folderListId = stream.ReadInt32L();
	clearFolderList = stream.ReadUint8L();

	TInt pathLen = stream.ReadUint16L();
	HBufC *fileNameBuf = HBufC::NewLC(pathLen);
	fileName.Set((TUint16*) fileNameBuf->Ptr(), pathLen, pathLen);
	stream.ReadL(fileName, pathLen);

	RFs rfs;
	RFile file;

    // Get the path to the default folder.
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);

	err = file.Open(rfs, fileName, EFileRead | EFileShareReadersOnly);
	User::LeaveIfError(err);
	
	CleanupClosePushL(file);

    // Import the default folder file synchronously.
    CPackedFolder *packedFolder = NULL;
    
    //Chop-off the path
    TInt Pos = fileName.LocateReverse('\\');
    if(Pos)
    {
    	fileName = fileName.Mid(Pos+1);
    }
    
    TRAP(err, packedFolder = iFeedsServer.ImportFolderL(file, fileName));
    
    CleanupStack::PopAndDestroy(/*file*/);
    CleanupStack::PopAndDestroy(/*rfs*/);
	CleanupStack::PopAndDestroy(/*fileNameBuf*/);
	CleanupStack::PopAndDestroy(/*stream*/);
	CleanupStack::PopAndDestroy(/*requestBuff*/);    

    if(err != KErrNone)
		{
		delete packedFolder;
		aMessage.Complete(err);
		return;
		}

	CleanupStack::PushL(packedFolder);

	// For news ticker, this is true
	if(clearFolderList)
		{
		TRAP(err, iFeedsServer.Database().FolderListDeleteL(folderListId));
		}
		
    if(err != KErrNone)
		{
		CleanupStack::PopAndDestroy(/*packedFolder*/);

		aMessage.Complete(err);
		return;
		}

    // Load it into the database.
    
	TRAP(err, iFeedsServer.Database().ImportFolderL(folderListId, *packedFolder));
    if(err != KErrNone)
		{
		CleanupStack::PopAndDestroy(/*packedFolder*/);

		aMessage.Complete(err);
		return;
		}

	CleanupStack::PopAndDestroy(/*packedFolder*/);

	// Compact the database.
	iFeedsServer.Database().Compact();	

	// Send the response.
	aMessage.Complete(KErrNone);
	// Notify new folder, feeds added
	iFeedsServer.NotifyFolderListChanged( folderListId );    			
	}


// -----------------------------------------------------------------------------
// CFeedsServerSession::ExportOPMLL
//
// Export feeds as an OPML file passed from the client.
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::ExportOPMLL( const RMessage2& aMessage )
    {
    // Extract the request.
	CBufFlat* requestBuff = CBufFlat::NewL(128);
	CleanupStack::PushL(requestBuff);

	TInt len = aMessage.GetDesLengthL(KFeedsServerPackedRequestArg);
	requestBuff->ExpandL(0, len);
	TPtr8 reqPtr((TUint8*) requestBuff->Ptr(0).Ptr(), len);

	aMessage.ReadL(KFeedsServerPackedRequestArg, reqPtr, 0);

    // Unpack the request.
	RBufReadStream  stream;
	stream.Open(*requestBuff, 0);

	CleanupClosePushL(stream);

	TInt fileNameLength = stream.ReadUint16L();
	if(iExportOPMLFileName)
		{
		delete iExportOPMLFileName;
		iExportOPMLFileName = NULL;
		}
		
	iExportOPMLFileName = HBufC::NewL(fileNameLength);
	TPtr fileName = iExportOPMLFileName->Des();
	stream.ReadL(fileName, fileNameLength);

	TInt folderListId = stream.ReadInt32L();
	TInt entryCount = stream.ReadUint16L();

    // Extract the ids of the entries to delete.
	RArray<TInt> entries;
	CleanupClosePushL(entries);
	for (TInt i = 0; i < entryCount; i++)
		{
		User::LeaveIfError(entries.Append(stream.ReadUint16L()));
		}

    // Export the entries.
	iFeedsServer.ExportFolderL(entries, fileName);

    // Clean up.
	CleanupStack::PopAndDestroy(/*entries*/);
	CleanupStack::PopAndDestroy(/*stream*/);
	CleanupStack::PopAndDestroy(/*requestBuff*/);

    // Send the response.
	aMessage.Complete(KErrNone);
   }

// -----------------------------------------------------------------------------
// CFeedsServerSession::CompleteClientPendingMessage
//
// 
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::CompleteClientPendingMessage( TInt aCompletionCode )
    {
    if  ( iPendingMessage.IsNull() == EFalse )
        {
        iPendingMessage.Complete( aCompletionCode );
        }
	else
		{
		FEED_LOG(_L("Feeds"), _L("Feeds_Errors.log"), 
				EFileLoggingModeAppend, _L("CFeedsServerSession::CompleteClientPendingMessage - pending msg: Null."));
		}
	}

// -----------------------------------------------------------------------------
// CFeedsServerSession::SetClientPendingMessage
//
// 
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::SetClientPendingMessage( RMessagePtr2 aMessage )
    {
    // We should never have more than a single outstanding asynchronous request
    __ASSERT_DEBUG( iPendingMessage.IsNull(), User::Panic(_L("Feeds Server Session"), KErrGeneral) );
    iPendingMessage = aMessage;
    }
    
    
// -----------------------------------------------------------------------------
// CFeedsServerSession::StartImportWait
//
// Called to notify the obsever to indicate the start of OPML import
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::StartImportWait()
	{
	// Ignore.
	}

// -----------------------------------------------------------------------------
// CFeedsServerSession::ImportCompleted
//
// Called to notify the obsever to indicate the completion OPML import
// -----------------------------------------------------------------------------
//
void CFeedsServerSession::ImportCompleted(TInt aStatusCode)
{
    // Delete the import task.
    delete iOPMLImportTask;
    iOPMLImportTask = NULL;
   
    // Compact the database.
	iFeedsServer.Database().Compact();

	CompleteClientPendingMessage( aStatusCode );
}
    
// -----------------------------------------------------------------------------
// CFeedsServerSession::LazyCallBack
//
// This function calls SendTokenChunkL instead of calling it from CompletedHelperL function
// To ensure it doesn't called if user has cancelled the request
// -----------------------------------------------------------------------------
//
TInt CFeedsServerSession::LazyCallBack(TAny* aPtr)
{
	CFeedsServerSession* sessionPtr = (CFeedsServerSession*)aPtr;
    sessionPtr->iCurrentRequest = 1; //This is GetFeed Call

	if(sessionPtr->iPendingMessage.IsNull() == EFalse && sessionPtr->iPendingMessageHandle == sessionPtr->iPendingMessage.Handle())
	{
		if(sessionPtr->iPendingStatus == KErrNone)
		{
			sessionPtr->iGetFeedCalled = ETrue;
    		TRAP_IGNORE(sessionPtr->SendTokenChunkL(*(sessionPtr->iPackedFeed), sessionPtr->iPendingMessage));
		}
		else
		{
    	    // Notify all open sessions that the folder list just changed.
    		sessionPtr->iFeedsServer.NotifyFolderListChanged(sessionPtr->iWatchFolderListId);
    		sessionPtr->iGetFeedCalled = EFalse;
		}
	    // Delete the task.
	    if(sessionPtr->iUpdateFeedTask != NULL)
	    {
	    	sessionPtr->iUpdateFeedTask->AutoDelete();
	    	sessionPtr->iUpdateFeedTask = NULL;
	    }
    	// Notify the client of result.
	    sessionPtr->CompleteClientPendingMessage( sessionPtr->iPendingStatus );
	}
	else if(sessionPtr->iPackedFeed != NULL)
	{
		delete sessionPtr->iPackedFeed;
		sessionPtr->iPackedFeed = NULL;
	}
	return EFalse;
}

// -----------------------------------------------------------------------------
// CFeedsServerSession::IsSpaceAvailableL
//
// This function checks whether disk space has not gone below critical level
// -----------------------------------------------------------------------------
//
TBool CFeedsServerSession::IsSpaceAvailableL()
{
    TInt drive( EDriveC );
    RFs  rfs;
    TBool isSpaceAvailable(EFalse);

    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);

    TRAP_IGNORE( isSpaceAvailable = !SysUtil::DiskSpaceBelowCriticalLevelL(
                                                        &rfs,
                                                        KMinFreebytes,
                                                        drive ));
    CleanupStack::PopAndDestroy();  //rfs

    return isSpaceAvailable;
}
