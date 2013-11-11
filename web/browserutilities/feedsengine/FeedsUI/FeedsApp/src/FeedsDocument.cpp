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
* Description:  Feeds application's document instance.
*
*/


#include <AiwGenericParam.h>
#include <StringLoader.h>

#include <FeedsApp.rsg>

#include "FeedsAppUi.h"
#include "FeedsDocument.h"
#include "LeakTracker.h"
#include "Logger.h"


// -----------------------------------------------------------------------------
// CFeedsDocument::CFeedsDocument()
//
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CFeedsDocument::CFeedsDocument(CEikApplication& aApp):
        CAiwGenericParamConsumer(aApp)
    {
    LEAK_TRACKER_INCREMENT(CLeakTracker::EFeedsDocument);
    }


// -----------------------------------------------------------------------------
// CFeedsDocument::NewL()
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsDocument* CFeedsDocument::NewL(CEikApplication& aApp)
    {
    CFeedsDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);

    return self;
    }


// -----------------------------------------------------------------------------
// CFeedsDocument::NewLC()
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedsDocument* CFeedsDocument::NewLC(CEikApplication& aApp)
    {
    CFeedsDocument* self = new (ELeave) CFeedsDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }


// -----------------------------------------------------------------------------
// CFeedsDocument::ConstructL()
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFeedsDocument::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CFeedsDocument::~CFeedsDocument()
//
// Destructor.
// -----------------------------------------------------------------------------
//
CFeedsDocument::~CFeedsDocument()
    {
    LEAK_TRACKER_DECREMENT(CLeakTracker::EFeedsDocument);
    }


// -----------------------------------------------------------------------------
// CFeedsDocument::CreateAppUiL()
//
// Create the application user interface, and return a pointer to it
// -----------------------------------------------------------------------------
//
CEikAppUi* CFeedsDocument::CreateAppUiL()
    {
    return(static_cast<CEikAppUi*>(new (ELeave) CFeedsAppUi));
    }


// -----------------------------------------------------------------------------
// CFeedsDocument::OpenFileL()
//
// Opens a given file from the app-framework.
// -----------------------------------------------------------------------------
//
void CFeedsDocument::OpenFileL(CFileStore*& /*aFileStore*/, RFile& /*aFile*/)
    {
 #if 0       
    const CAiwGenericParamList* params = GetInputParameters();
    const TAiwGenericParam*     param;
    TPtrC                       url(KNullDesC);
    TPtrC                       contentType(KNullDesC);
    TPtrC                       charSet(KNullDesC);
    TBool                       urlSet = EFalse;
//    TInt                        size;
    HBufC8*                     buffer = NULL;    
    
    // Get the url from the parms.
    if (params)
        {
        TInt index;

        // Source URL.
        index = 0;
        param = params->FindFirst(index, EGenericParamURL, EVariantTypeDesC);
        if (param != NULL)
            {
            url.Set(param->Value().AsDes());
            urlSet = ETrue;
            }

        // Content-type.
        index = 0;
        param = params->FindFirst(index, EGenericParamMIMEType, EVariantTypeDesC);
        if (param != NULL)
            {
            contentType.Set(param->Value().AsDes());
            }

        // Charset.
        index = 0;
        param = params->FindFirst(index, EGenericParamCharSet, EVariantTypeDesC);
        if (param != NULL)
            {
            charSet.Set(param->Value().AsDes());
            }
        }

    // Read the buffer from the file.
    User::LeaveIfError(aFile.Size(size));
    buffer = HBufC8::NewLC(size);
    TPtr8 bufferPtr(buffer->Des());
    
    User::LeaveIfError(aFile.Read(bufferPtr, size));

    // If the url was set then subscribe to the Feed.
    if (urlSet)
        {
        SubscribeToL(url, contentType, charSet, *buffer);
        }
            
    // In any case, show the feed using the downloaded buffer.
    iUpdateTopicViewTask = CUpdateTopicViewTask::NewL(buffer, *this);
    iUpdateTopicViewTask->StartTaskL();
    iUpdateTopicViewTask->EnableAutoDeleteL();

    CleanupStack::Pop(buffer);
#endif
    }
    
#if 0    
// -----------------------------------------------------------------------------
// CFeedDocument::SubscribeToL
// 
// Subscribes to a feed, provided via OpenFileL (from an external app).
// -----------------------------------------------------------------------------
//
void CFeedsDocument::SubscribeToL(const TDesC& aUrl, const TDesC& aContentType, 
        const TDesC& aCharSet, HBufC8& aFeedBuffer)
    {   
    TPtr8          bufferPtr(aFeedBuffer.Des());
    CFeedHandler*  feedHandler = NULL;
    CFeed*         feed = NULL;
    const TDesC*   feedName = NULL;
    HBufC*         noName = NULL;

    // TODO: This may also be used with an empty aFile -> via an html link element.

    // Parse the buffer.
    if (bufferPtr.Length() > 0)
        {
        // TODO: could avoid this if we waited until the topic-view is updated.
        feedHandler = (CFeedHandler*) Singletons::Get(Singletons::EFeedHandler);
        
        TRAPD(err, feed = feedHandler->ParseL(&bufferPtr, aContentType, aCharSet));
        if (err == KErrNone)
            {
            CleanupStack::PushL(feed);
            }
        }
    
    // If possible use the feed's name.
    if (feed != NULL)
        {
        feedName = feed->Get(CFeed::ETitle);
        }
       
    // Otherwise use the default name.        
    if (feedName == NULL)
        {
        noName = StringLoader::LoadLC(R_FEEDS_NEW_FEED_ITEM);
        feedName = *noName;
        }
    
    // Subscribe to the feed. 
    SubscribeToL(*feedName, aUrl);
    
    // Cleanup
    if (noName != NULL)
        {
        CleanupStack::PopAndDestroy(noName);
        }
        
    if (feed != NULL)
        {
        CleanupStack::PopAndDestroy(feed);        
        }
    }


// -----------------------------------------------------------------------------
// CFeedDocument::SubscribeToL
// 
// Subscribes to a feed.
// -----------------------------------------------------------------------------
//
void CFeedsDocument::SubscribeToL(const TDesC& aName, const TDesC& aUrl)
    {   
    CFolderView*  folderView = NULL;
    
    // Subscribe to the feed. 
    folderView = (CFolderView*) Singletons::Get(Singletons::EFolderView);    
    folderView->ImportFeedL(aName, aUrl);
    }
#endif
