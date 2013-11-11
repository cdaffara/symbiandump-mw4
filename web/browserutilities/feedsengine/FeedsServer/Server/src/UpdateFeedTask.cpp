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
* Description:  A task to update a Feed.
*
*/


#include "FeedAttributes.h"
#include "FeedHandler.h"
#include "FeedsServer.h"
#include "LeakTracker.h"
#include "Logger.h"
#include "PackedFeed.h"
#include "UpdateFeedTask.h"
#include "UrlHandlerFactory.h"


// -----------------------------------------------------------------------------
// CUpdateFeedTask::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpdateFeedTask* CUpdateFeedTask::NewL(CFeedsServer& aFeedsServer, 
        const TDesC& aUrl,TInt aFeedId, MUpdateFeedTaskObserver& aObserver)
    {
    CUpdateFeedTask* self = new (ELeave) CUpdateFeedTask(aFeedsServer, aObserver);
    
    CleanupStack::PushL(self);
    self->ConstructL(aUrl,aFeedId);
    CleanupStack::Pop();

    return self;
    }
    
        
// -----------------------------------------------------------------------------
// CUpdateFeedTask::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpdateFeedTask* CUpdateFeedTask::NewL(CFeedsServer& aFeedsServer, const TDesC& aUrl,TInt aFeedId,
        TDesC8* aFeedBuffer, MUpdateFeedTaskObserver& aObserver)
    {
    CUpdateFeedTask* self = new (ELeave) CUpdateFeedTask(aFeedsServer, aObserver);
    
    CleanupStack::PushL(self);
    self->ConstructL(aUrl,aFeedId);
    CleanupStack::Pop();

    self->iFileBuffer = aFeedBuffer;
    return self;
    }

        
// -----------------------------------------------------------------------------
// CUpdateFeedTask::CUpdateFeedTask
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpdateFeedTask::CUpdateFeedTask(CFeedsServer& aFeedsServer, MUpdateFeedTaskObserver& aObserver):
        CTask(aFeedsServer), iLeakTracker(CLeakTracker::EUpdateFeedTask), iObserver(aObserver)
    {
    }
        

// -----------------------------------------------------------------------------
// CUpdateFeedTask::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::ConstructL(const TDesC& aUrl,TInt aFeedId)
    {
    BaseConstructL(ETrue);
    iFeedId = aFeedId;
    iUrl = aUrl.AllocL();        
    iPackedFeed = CPackedFeed::NewL();
    iIdle = CIdle::NewL(CActive::EPriorityIdle);
    }        


// -----------------------------------------------------------------------------
// CUpdateFeedTask::~CUpdateFeedTask
// Deconstructor.
// -----------------------------------------------------------------------------
//
CUpdateFeedTask::~CUpdateFeedTask()
    {
    delete iUrl;
    delete iIdle;
    delete iUrlHandler;

    delete iFileBuffer;
    delete iPackedFeed;
    }


// -----------------------------------------------------------------------------
// CUpdateFeedTask::StartTaskL.
//
// Starts the task.
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::StartTaskL()
    {
    // If the feed was already provided then get ready to parse the buffer.
    if (iFileBuffer != NULL)
        {
        FEED_LOG(_L("Feeds"), _L("Feeds.log"), 
            EFileLoggingModeAppend, _L("Processing Feed: External Buffer"));
            
        // Parse the buffer, but to be consistent with the asynchronous 
        // behavior of using an UrlHandlers the buffer isn't parsed until 
        // after the callstack unrolls.
        iIdle->Start(TCallBack(DelayedParse, this));
        }

    // If the feed is coming from a UrlHandler, then start the load.
    else if (iUrl != NULL)
        {
        FEED_LOG1(_L("Feeds"), _L("Feeds.log"), 
            EFileLoggingModeAppend, _L("Processing Feed: %S"), iUrl);

        iUrlHandler = UrlHandlerFactory::NewUrlHandlerL( iObserver.HttpConnection(), *iUrl );
        iUrlHandler->LoadUrlL(*iUrl, *this);
        }
                
    // Otherwise, panic
    else
        {
        // TODO:
        }
    }


// -----------------------------------------------------------------------------
// CUpdateFeedTask::StartWait
//
// Notifies the observer that the UrlHandler is about to start a lengthy operation.
// In response the observer could display somekind of status indicator.
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::StartWait()
    {
    iObserver.StartWait();
    }


// -----------------------------------------------------------------------------
// CUpdateFeedTask::LoadCompleted
//
// Passes the status code and responseBody to the observer.  The observer
// adopts aResponseBody.
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::LoadCompleted(TInt aStatusCode, TDesC8* aResponseBody,
        const TDesC& aContentType, const TDesC& aCharSet)
    {
    TInt err;
    
    // Parse the buffer
    if (aResponseBody != NULL)
        {
        // TODO: Just pass iPackedFeed instead and remove the
        //         whole observer interface altogether.
        TRAP(err, iFeedsServer.FeedHandler().ParseL(*aResponseBody, 
                aContentType, aCharSet, *this));
        if (err != KErrNone)
            {
            delete iPackedFeed;
            iPackedFeed = NULL;
            TRAP_IGNORE(iFeedsServer.ResetXmlUtilsL());
            }
        else
            {
            iPackedFeed->Trim();
            }

        delete aResponseBody;
        
        // Pass the feed to the observer.
        iObserver.Completed(iPackedFeed, *iUrl,iFeedId, err);
        iPackedFeed = NULL;
        }

    // The load failed, exit cleanly.
    else
        {
        // Pass the status to the observer.
        iObserver.Completed(NULL, *iUrl,iFeedId, ((aStatusCode == KErrNone)? KErrCorrupt:aStatusCode));
        }
    }


// -----------------------------------------------------------------------------
// CUpdateFeedTask::FeedBeginsL
//
// The beginning of a feed was found.
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::FeedBeginsL()
    {
    iPackedFeed->FeedBeginsL();
    
    // Add the oringal url too.  
    iPackedFeed->AddAttributeL(EFeedAttributeFeedUrl, *iUrl);
    }


// -----------------------------------------------------------------------------
// CUpdateFeedTask::FeedEndsL
//
// The end of a feed was found.
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::FeedEndsL()
    {
    iPackedFeed->FeedEndsL();
    }


// -----------------------------------------------------------------------------
// CUpdateFeedTask::ItemBeginsL
//
// The beginning of a item was found.
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::ItemBeginsL()
    {
    iPackedFeed->ItemBeginsL();
    }


// -----------------------------------------------------------------------------
// CUpdateFeedTask::ItemEndsL
//
// The end of a item was found.
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::ItemEndsL()
    {
    iPackedFeed->ItemEndsL();
    }


// -----------------------------------------------------------------------------
// CUpdateFeedTask::EnclosureBeginsL
//
// The beginning of a enclosure was found.
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::EnclosureBeginsL()
    {
    iPackedFeed->EnclosureBeginsL();
    }


// -----------------------------------------------------------------------------
// CUpdateFeedTask::EnclosureEndsL
//
// The end of a enclosure was found.
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::EnclosureEndsL()
    {
    iPackedFeed->EnclosureEndsL();
    }


// -----------------------------------------------------------------------------
// CUpdateFeedTask::AddAttributeL
//
// A attribute was found.  This attribute should be applied to
// the enclosing entity (Feed, Item, or Enclosure).
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::AddAttributeL(TInt aAttribute, const TDesC& aValue)
    {
    if (aValue.Length() > 0)
        {
        iPackedFeed->AddAttributeL(aAttribute, aValue);
        }
    }


// -----------------------------------------------------------------------------
// CUpdateFeedTask::OtherTitleL
//
// An unimportant element was found.  The client can use this information for 
// feed validation.
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::OtherTitleL()
    {
    iPackedFeed->OtherTitleL();
    }


// -----------------------------------------------------------------------------
// CUpdateFeedTask::OtherDescriptionL
//
// An unimportant element was found.  The client can use this information for 
// feed validation.
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::OtherDescriptionL()
    {
    iPackedFeed->OtherDescriptionL();
    }


// -----------------------------------------------------------------------------
// CUpdateFeedTask::OtherLinkL
//
// An unimportant element was found.  The client can use this information for 
// feed validation.
// -----------------------------------------------------------------------------
//
void CUpdateFeedTask::OtherLinkL()
    {
    iPackedFeed->OtherLinkL();
    }


// -----------------------------------------------------------------------------
// CFileHandler::DelayedParse
// 
// Parses the buffer to the observer now that the callstack has unrolled.
// -----------------------------------------------------------------------------
//
TInt CUpdateFeedTask::DelayedParse(TAny* aPtr)
    {
    CUpdateFeedTask*  self = static_cast<CUpdateFeedTask*>(aPtr);
    
    self->LoadCompleted(KErrNone, self->iFileBuffer, KNullDesC, KNullDesC);
    self->iFileBuffer = NULL;
    
    return EFalse;
    }
