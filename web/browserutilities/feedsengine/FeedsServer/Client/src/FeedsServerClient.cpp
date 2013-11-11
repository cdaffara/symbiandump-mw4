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


#include "FeedsServerClient.h"
#include "FeedsServerMsg.h"
#include "FeedsServerSettings.h"
#include "Logger.h"
#include "ClientRequestHandlers.h"
#include "FeedRequestHandlers.h"
#include "FolderItemRequestHandlers.h"


// -----------------------------------------------------------------------------
// RFeedsServer::RFeedsServer
//
// C++ default constructor can NOT contain any code that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RFeedsServer::RFeedsServer( MFeedsServerObserver& aObserver, TInt aFolderListId ):
        iObserver(aObserver), iFolderListId(aFolderListId)
    {

    CClientRequestHandler *requestHandler = NULL;

    TRAP_IGNORE (
    {
    requestHandler =   CClientRequestHandler::NewL( this,  aFolderListId  );

    CleanupStack::PushL(requestHandler);
    iActiveRequests.AppendL(requestHandler);
    CleanupStack::Pop(requestHandler);
    });


#ifdef TRACK_LEAKS
    delete gLeakTracker;
    gLeakTracker = NULL;

    TRAP_IGNORE(gLeakTracker = CLeakTracker::NewL(_L("FeedsServerClient")));
#endif
    }


// -----------------------------------------------------------------------------
// RFeedsServer::Connect
//
// Connects to the server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeedsServer::Connect()
    {
    return iActiveRequests[0]->GetFeedInterface()->Connect();
    }


// -----------------------------------------------------------------------------
// RFeedsServer::Close
//
// Closes the connection to the server.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsServer::Close()
    {
    
    iActiveRequests[0]->GetFeedInterface()->Close();

    iActiveRequests.ResetAndDestroy();

#ifdef TRACK_LEAKS
    delete gLeakTracker;
    gLeakTracker = NULL;
#endif
    }


// -----------------------------------------------------------------------------
// RFeedsServer::Version
//
// Defines server version number
// -----------------------------------------------------------------------------
//
EXPORT_C TVersion RFeedsServer::Version() const
    {
    return(TVersion(KFeedsServerMajorVersionNumber, KFeedsServerMinorVersionNumber,
            KFeedsServerBuildVersionNumber));
    }

// -----------------------------------------------------------------------------
// RFeedsServer::RequestHandlerCompletedL
//
// Notifies the observer that the request is completed.
// -----------------------------------------------------------------------------
//
void RFeedsServer::RequestHandlerCompletedL(CTransaction& aTrans, TInt aStatus)
    {
    TFeedsServerSetting setting;

    // Notify the observer if an error occurred.
    switch (aTrans.Type())
        {
        case CTransaction::EFetchSettings:
            {
            // Notify the observer that the request has completed.
            if (aStatus == KErrNone)
                {
                setting = iActiveRequests[0]->GetFeedInterface()->Settings();
                iSettings.SetAutoUpdate(setting.iAutoUpdate);
                iSettings.SetAutoUpdateAP(setting.iAutoUpdateAP);
                iSettings.SetAutoUpdateFreq(setting.iAutoUpdateFreq);
                iSettings.SetAutoUpdateWhileRoaming(setting.iAutoUpdateWhileRoaming);
                }
            iObserver.SettingsChanged( aStatus );

            break;
            }

        case CTransaction::EWatchForSettingChanges:
            {
            // Only notify the user if an error occurred.
            if (aStatus != KErrNone)
                {
                iObserver.SettingsChanged(aStatus);
                }
            break;
            }

        case CTransaction::EChangeSettings:
            break;
        }
    }

// -----------------------------------------------------------------------------
// RFeedsServer::RequestCompletedL
//
// Called to handle the response.
// -----------------------------------------------------------------------------
void RFeedsServer::RequestCompleted(CTransaction* aTrans, TInt /*aStatus*/)
    {
    // Notify the observer if an error occurred.
    switch (aTrans->Type())
        {
        //Server related
        case CTransaction::EFetchSettings:
        case CTransaction::EWatchForSettingChanges:
        case CTransaction::EChangeSettings: 
            {
            TRAP_IGNORE(RequestHandlerCompletedL(*aTrans, aTrans->GetStatusCode()));
            break;
            }
        //Feed related
        case CTransaction::EFetchFeed:
        case CTransaction::EUpdateItemStatus:
            {
        	TRAP_IGNORE(iActiveRequests[0]->GetFeedsServerFeed()->RequestHandlerCompletedL(*aTrans, aTrans->GetStatusCode()));
        	break;
            }
        //Folder item related
        default:
            {
        	TRAP_IGNORE(iActiveRequests[0]->GetFeedsServerFolderItem()->RequestHandlerCompletedL(*aTrans, aTrans->GetStatusCode()));
        	break;	
            }
        }
    }
       
// -----------------------------------------------------------------------------
// RFeedsServer::NetworkConnectionNeededL
//
// Request to create a network connection.
// -----------------------------------------------------------------------------
 void RFeedsServer::NetworkConnectionNeededL(TInt* aConnectionPtr, TInt& aSockSvrHandle,
            TBool& aNewConn, TApBearerType& aBearerType)
   {
   TBool feedOperationRequested = EFalse;
   TBool FolderItemOperationRequested = EFalse;
   CClientRequestHandler *requestHandler = iActiveRequests[0];

   RFeed*	feedsServerFeed  = requestHandler->GetFeedsServerFeed(); 
   CFeedRequestHandler *feedReqHandler = feedsServerFeed->iActiveRequests[0];
   feedOperationRequested = feedReqHandler->GetOperationRequested();
   
   RFolderItem*  feedsServerFolderItem = requestHandler->GetFeedsServerFolderItem();
   CFolderItemRequestHandler *folderReqHandler = feedsServerFolderItem->iActiveRequests[0];   
   FolderItemOperationRequested = folderReqHandler->GetOperationRequested();
   

   if( feedOperationRequested )
      {
      feedsServerFeed->NetworkConnectionNeededL(aConnectionPtr, aSockSvrHandle, aNewConn, aBearerType);
      }
   else if ( FolderItemOperationRequested )
      {
      feedsServerFolderItem->NetworkConnectionNeededL(aConnectionPtr, aSockSvrHandle, aNewConn, aBearerType);
      }
   }


// -----------------------------------------------------------------------------
// RFeedsServer::DebugPrintTablesL
//
// Print tables of database.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsServer::DebugPrintTablesL()
    {
    iActiveRequests[0]->GetFeedInterface()->DebugPrintTablesL();
    }


// -----------------------------------------------------------------------------
// RFeedsServer::DisconnectManualUpdateConnectionL
//
// Disconnect connection provided by client for manual update.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsServer::DisconnectManualUpdateConnectionL()
    {
    iActiveRequests[0]->GetFeedInterface()->DisconnectManualUpdateConnectionL();
    }


// -----------------------------------------------------------------------------
// RFeedsServer::Settings
//
// Once fetched, it returns the Settings.  This method panics the client if it is 
// called before the Settings is available.
// -----------------------------------------------------------------------------
//
EXPORT_C const TFeedsServerSettings RFeedsServer::Settings() const
    {
    return iSettings;
    }


// -----------------------------------------------------------------------------
// RFeedsServer::SettingsL
//
// Returns feed related settings. 
// -----------------------------------------------------------------------------
//
EXPORT_C const TFeedsServerSettings RFeedsServer::FetchServerSettingsSyncL() const
    {
    TFeedsServerSetting setting;
	TFeedsServerSettings settings;
    CClientRequestHandler *requestHandler = iActiveRequests[0];

    RFeedsServer* self = const_cast<RFeedsServer*>(this);

    self->iActiveRequests[0]->GetFeedInterface()->GetSettingsL(setting, EFalse);
    
    settings.SetAutoUpdate(setting.iAutoUpdate);
    settings.SetAutoUpdateAP(setting.iAutoUpdateAP);
    settings.SetAutoUpdateFreq(setting.iAutoUpdateFreq);
    settings.SetAutoUpdateWhileRoaming(setting.iAutoUpdateWhileRoaming);

    return settings;
    }


// -----------------------------------------------------------------------------
// RFeedsServer::UpdateSettingsL
//
// Requests the server to change its settings to match the provided values. 
// Some care is needed by the client to prevent settings loss if SettingsChanged
// was called after it called SettingsL.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsServer::UpdateSettingsL( const TFeedsServerSettings& aNewSettings )
    {

    TFeedsServerSetting setting;

    setting.iAutoUpdate = aNewSettings.AutoUpdate();
    setting.iAutoUpdateAP = aNewSettings.AutoUpdateAP();
    setting.iAutoUpdateFreq = aNewSettings.AutoUpdateFreq();
    setting.iAutoUpdateWhileRoaming = aNewSettings.AutoUpdateWhileRoaming();

    iActiveRequests[0]->GetFeedInterface()->SetSettingsL(setting);

    iPendingSettings = aNewSettings;
    }

// -----------------------------------------------------------------------------
// RFeedsServer::WatchSettingsL
//
// Sets up a notifier to execute when the settings change.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsServer::WatchSettingsL()
    {
    iActiveRequests[0]->GetFeedInterface()->WatchSettingsL();
    }

// -----------------------------------------------------------------------------
// RFeedsServer::FetchServerSettingsL
//
// Fetch the feeds server settings.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsServer::FetchServerSettingsL()
    {
    TFeedsServerSetting setting;
    TFeedsServerSettings settings;
        
    iActiveRequests[0]->GetFeedInterface()->GetSettingsL(setting);	
    settings.SetAutoUpdate(setting.iAutoUpdate);
    settings.SetAutoUpdateAP(setting.iAutoUpdateAP);
    settings.SetAutoUpdateFreq(setting.iAutoUpdateFreq);
    settings.SetAutoUpdateWhileRoaming(setting.iAutoUpdateWhileRoaming);
    }


// -----------------------------------------------------------------------------
// RFeedsServer::CancelAllL
//
// Request the FeedsServer to cancel all activities that can be cancelled.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeedsServer::CancelAllL()
    {
    iActiveRequests[0]->GetFeedInterface()->CancelAllL();
    }

// -----------------------------------------------------------------------------
// RFeedsServer::SetServerFeed
// 
// Sets the address of RFeed instance which will be used for sending 
// RequestCompleted
// -----------------------------------------------------------------------------
//
void RFeedsServer::SetServerFeed(RFeed& aFeedsServerFeed)
    {
    iActiveRequests[0]->SetFeedsServerFeed(&aFeedsServerFeed);    
    }

// -----------------------------------------------------------------------------
// RFeedsServer::SetServerFolderItem
// 
// Sets the address of RFolderItem instance which will be used for sending 
// RequestCompleted
// -----------------------------------------------------------------------------
//
void RFeedsServer::SetServerFolderItem(RFolderItem& aFeedsServerFolderItem)
    {
    iActiveRequests[0]->SetFeedsServerFolderItem(&aFeedsServerFolderItem);        
    }

