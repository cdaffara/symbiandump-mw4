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
* Description:  The top-level handler of Feeds.  It takes a buffer and uses an
*                appropriate feeds parser to create a Feed instance.
*
*/


#include "CleanupLibXml2.h"
#include "FeedHandler.h"
#include "LeakTracker.h"
#include "RssFeedParser.h"
#include "AtomFeedParser.h"
#include "XmlUtils.h"


// -----------------------------------------------------------------------------
// CFeedHandler::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeedHandler* CFeedHandler::NewL(CXmlUtils& aXmlUtils)
    {
    CFeedHandler* self = new (ELeave) CFeedHandler(aXmlUtils);
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CFeedHandler::CFeedHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFeedHandler::CFeedHandler(CXmlUtils& aXmlUtils):
        iLeakTracker(CLeakTracker::EFeedHandler), iXmlUtils(aXmlUtils)
    {
    }
        

// -----------------------------------------------------------------------------
// CFeedHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFeedHandler::ConstructL()
    {
    }        


// -----------------------------------------------------------------------------
// CFeedHandler::~CFeedHandler
// Deconstructor.
// -----------------------------------------------------------------------------
//
CFeedHandler::~CFeedHandler()
    {
    }


// -----------------------------------------------------------------------------
// CFeedHandler::ParseFeedL
//
// Creates a Feed instance from the buffer.  This method may modify aBuffer.
// -----------------------------------------------------------------------------
//
void CFeedHandler::ParseL(TDesC8& aBuffer, const TDesC& aContentType, 
        const TDesC& aCharSet, MFeedParserObserver& aObserver)
    {
    CFeedParser*  parser = NULL;
    RXmlEngDocument     document;
    TBool         feedParsed = EFalse;
    
    // Parse the buffer.
    document = iXmlUtils.ParseBufferL(aBuffer, aCharSet);
    CleanupClosePushL(document);

    // If this is an RSS document then use the RSS parser to create a feed.
    if (CRssFeedParser::IsFeedSupported(iXmlUtils, document, aContentType))
        {
        parser = CRssFeedParser::NewL(iXmlUtils);        
        CleanupStack::PushL(parser);
        
        parser->ParseFeedL(document, aObserver);
        feedParsed = ETrue;
        
        CleanupStack::PopAndDestroy(parser);
        }
        
    // If this is an Atom document then use the Atom parser to create a feed.
    else if (CAtomFeedParser::IsFeedSupported(iXmlUtils, document, aContentType))
        {
        parser = CAtomFeedParser::NewL(iXmlUtils);        
        CleanupStack::PushL(parser);
        
        parser->ParseFeedL(document, aObserver);
        feedParsed = ETrue;
        
        CleanupStack::PopAndDestroy(parser);
        }

    if (!feedParsed)
        {
        User::Leave(KErrNotSupported);
        }

    // Clean up.
    CleanupStack::PopAndDestroy(/*document*/);
    }
