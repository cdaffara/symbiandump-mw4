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
* Description:  The client side connection to a feed on the FeedsSever.
*
*/


#include "FeedsServerClient.h"
#include "FeedsServerFeed.h" 
#include "FeedRequestHandlers.h"
#include "ClientRequestHandlers.h"



#include "Logger.h"

// -----------------------------------------------------------------------------
// RFeed::RFeed
//
// C++ default constructor can NOT contain any code that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RFeed::RFeed(MFeedObserver& aObserver):
        iObserver(aObserver), iFeed(NULL), iActiveRequests(3), iPendingFetchFeed(EFalse),
        iFeedsServer( NULL )
    {
    CFeedRequestHandler *feedReqHandler = NULL;

    TRAP_IGNORE(
    {
    feedReqHandler =   CFeedRequestHandler::NewL();

    CleanupStack::PushL(feedReqHandler);
    iActiveRequests.AppendL(feedReqHandler);
    CleanupStack::Pop(feedReqHandler);
    });
    }


// -----------------------------------------------------------------------------
// RFeed::Open
//
// Opens a session to the given feed on the server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RFeed::Open(RFeedsServer &aFeedsServer)
    {
    // TODO: Create an empty feed which can be returned when something happens
    //         and a valid feed isn't available.

    iFeedsServer = &aFeedsServer;
    RFeedsInterface *feedsInterface = iFeedsServer->iActiveRequests[0]->GetFeedInterface();
    iActiveRequests[0]->SetFeedInterface(feedsInterface);
    iFeedsServer->SetServerFeed(*this);

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RFeed::Close
//
// Closes the connection to the server.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeed::Close()
	{

    iFeedsServer = NULL;    
    iActiveRequests.ResetAndDestroy();
    delete iFeed;
    iFeed = NULL;

    iPendingFetchFeed = EFalse;
    }


// -----------------------------------------------------------------------------
// RFolderItem::RequestHandlerCompletedL
//
// Notifies the observer that the request is completed.
// -----------------------------------------------------------------------------
//
void RFeed::RequestHandlerCompletedL(CTransaction& aTrans, TInt aStatus)
    {
    switch (aTrans.Type())
        {
        case CTransaction::EFetchFeed:
            iPendingFetchFeed = EFalse;
            if (aStatus == KErrNone)
                {
                delete iFeed;
                iFeed = NULL;
                iFeed = CFeed::NewL(iActiveRequests[0]->GetFeedInterface()->Feed());
                }
            // Notify the observer that the request has completed.
            iObserver.FeedRequestCompleted(aStatus);
            break;

        case CTransaction::EUpdateItemStatus:
            // TODO: Notify the observer.
            break;
        }
    }


// -----------------------------------------------------------------------------
// RFeed::FetchFeedL
//
// Fetches the given feed.  FeedRequestCompleted is called upon completion.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeed::FetchFeedL(const TDesC& aUrl, TInt /*aFolderListId*/, TBool aForceUpdate, TBool aNoCache)
    {
    iActiveRequests[0]->SetOperationRequested(ETrue);
    iActiveRequests[0]->GetFeedInterface()->FetchL(aUrl, aForceUpdate, aNoCache);
    }

// -----------------------------------------------------------------------------
// RFeed::FetchFeedL
//
// Fetches the given feed -- using the feed's id..  FeedRequestCompleted is 
// called upon completion.
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeed::FetchFeedL(TInt aFeedId, TBool aForceUpdate, TBool aNoCache)
    {
    RFeedsInterface *feedsInterface = iActiveRequests[0]->GetFeedInterface();
    
    CFeedsEntity* entity = feedsInterface->SearchFeed(aFeedId, feedsInterface->RootFolder());
    if (entity)
        {
        iActiveRequests[0]->SetOperationRequested(ETrue);
        feedsInterface->FetchL(*entity, aForceUpdate, aNoCache);
        }
    }

// -----------------------------------------------------------------------------
// RFeed::NetworkConnectionNeededL()
// -----------------------------------------------------------------------------
//
void RFeed::NetworkConnectionNeededL( TInt* aConnectionPtr, TInt& aSockSvrHandle,
                                      TBool& aNewConn, TApBearerType& aBearerType )
    {
    iActiveRequests[0]->SetOperationRequested(EFalse);
    iObserver.NetworkConnectionNeededL( aConnectionPtr, aSockSvrHandle, aNewConn, aBearerType );
    }

// -----------------------------------------------------------------------------
// RFeed::UpdateFeedItemStatusL
//
// Request the feeds server to update the feed's item status. 
// -----------------------------------------------------------------------------
//
EXPORT_C void RFeed::UpdateFeedItemStatusL(const RArray<TInt>& aItemIds,
        const RArray<TItemStatus>& aItemStatus, TInt aUnreadCount)
    {
    // Just return if there isn't an active feed.
    if (iFeed == NULL)
        {
        return;
        }
     
    iActiveRequests[0]->GetFeedInterface()->UpdateFeedItemStatusL(aItemIds, (const RArray<TFeedItemStatus> &)aItemStatus, aUnreadCount);
    }


// -----------------------------------------------------------------------------
// RFeed::HasCurrentFeed
//
// Returns ETrue if it has an available current feed.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RFeed::HasCurrentFeed() const
    {
    return (iActiveRequests[0]->GetFeedInterface()->Feed() != NULL);
    }


// -----------------------------------------------------------------------------
// RFeed::Feed
//
// Once fetched, it returns the CFeed.  This method panics the client if it is 
// called before the feed item is available.
// -----------------------------------------------------------------------------
//
EXPORT_C const CFeed& RFeed::Feed() const
    {
    return *iFeed;
    }
