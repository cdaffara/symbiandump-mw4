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
* Description:  RSS parser
*
*/


#include "FeedAttributes.h"
#include "FeedParserObserver.h"
#include "LeakTracker.h"
#include "RssFeedParser.h"
#include "XmlUtils.h"


// Element and attribute names used by this parser.
_LIT8(KRssHead, "rss");
_LIT8(KRdfHead, "RDF");

_LIT8(KDc, "dc");
_LIT8(KEnc, "enc");

_LIT8(KChannel, "channel");
_LIT8(KTitle, "title");
_LIT8(KLink, "link");
_LIT8(KDescription, "description");
_LIT8(KItem, "item");
_LIT8(KAbout, "about");
_LIT8(KGuid, "guid");
_LIT8(KEnclosure, "enclosure");
_LIT8(KUrl, "url");
_LIT8(KResource, "resource");
_LIT8(KType, "type");
_LIT8(KLength, "length");
_LIT8(KDate, "date");
_LIT8(KPubDate, "pubdate");
_LIT8(KLastBuildDate, "lastbuilddate");

//#ifdef _DEBUG                    
_LIT8(KImage, "image");
_LIT8(KTextInput, "textinput");
//#endif

// -----------------------------------------------------------------------------
// RssFeedParser::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRssFeedParser* CRssFeedParser::NewL(CXmlUtils& aXmlUtils)
    {
    CRssFeedParser* self = new (ELeave) CRssFeedParser(aXmlUtils);
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CRssFeedParser::CRssFeedParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRssFeedParser::CRssFeedParser(CXmlUtils& aXmlUtils):
        CFeedParser(aXmlUtils), iLeakTracker(CLeakTracker::ERssFeedParser)
    {
    }
        

// -----------------------------------------------------------------------------
// CRssFeedParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRssFeedParser::ConstructL()
    {
    // The mappings are used to map elements to handler methods.  For example when
    // a <title> element is found its contents are extracted as CDATA and passed
    // on to the observer as a EFeedAttributeTitle.
    
    // Add mappings to process the children of a channel.
    AddFeedMappingL(KNullDesC8(), KItem(), EFeedAttributeUnused, ElementHandlerItemL);

    AddFeedMappingL(KNullDesC8(), KTitle(), EFeedAttributeTitle, ElementHandlerTextL);
    AddFeedMappingL(KNullDesC8(), KLink(), EFeedAttributeLink, ElementHandlerUrlL);
    AddFeedMappingL(KNullDesC8(), KDescription(), EFeedAttributeDescription, ElementHandlerCDataL);
    AddFeedMappingL(KNullDesC8(), KPubDate(), EFeedAttributeTimestamp, ElementHandlerTimestampL); 
    AddFeedMappingL(KNullDesC8(), KLastBuildDate(), EFeedAttributeTimestamp, ElementHandlerTimestampL); 

    AddFeedMappingL(KDc(), KTitle(), EFeedAttributeTitle, ElementHandlerTextL);
    AddFeedMappingL(KDc(), KDescription(), EFeedAttributeDescription, ElementHandlerCDataL);    
    AddFeedMappingL(KDc(), KDate(), EFeedAttributeTimestamp, ElementHandlerTimestampL); 

//#ifdef _DEBUG
    AddFeedMappingL(KNullDesC8(), KImage(), EFeedAttributeUnused, ElementHandlerOtherL);
    AddFeedMappingL(KNullDesC8(), KTextInput(), EFeedAttributeUnused, ElementHandlerOtherL);
//#endif

    // Add mappings to process the children of an item.
    AddItemMappingL(KNullDesC8(), KTitle(), EItemAttributeTitle, ElementHandlerTextL);
    AddItemMappingL(KNullDesC8(), KLink(), EItemAttributeLink, ElementHandlerUrlL);
    AddItemMappingL(KNullDesC8(), KDescription(), EItemAttributeDescription, ElementHandlerCDataL);    
    AddItemMappingL(KNullDesC8(), KEnclosure(), EItemAttributeEnclosure, ElementHandlerEnclosureL);    
    AddItemMappingL(KNullDesC8(), KPubDate(), EItemAttributeTimestamp, ElementHandlerTimestampL); 

    AddItemMappingL(KDc(), KTitle(), EItemAttributeTitle, ElementHandlerTextL);
    AddItemMappingL(KDc(), KDescription(), EItemAttributeDescription, ElementHandlerCDataL);
    AddItemMappingL(KDc(), KDate(), EItemAttributeTimestamp, ElementHandlerTimestampL); 
    AddItemMappingL(KEnc(), KEnclosure(), EItemAttributeEnclosure, ElementHandlerEnclosureL);    
    }


// -----------------------------------------------------------------------------
// CRssFeedParser::~CRssFeedParser
// Deconstructor.
// -----------------------------------------------------------------------------
//
CRssFeedParser::~CRssFeedParser()
    {
    }

        
// -----------------------------------------------------------------------------
// CRssFeedParser::ParseFeedL
//
// Creates a Feed instance from the given document. 
// -----------------------------------------------------------------------------
//
void CRssFeedParser::ParseFeedL(RXmlEngDocument aDocument, 
        MFeedParserObserver& aObserver)
    {
	TXmlEngElement  rootNode;
	TXmlEngElement  channelNode;

    // Get the root node.
    rootNode = iXmlUtils.GetDocumentFirstElement(aDocument);

    // Find the channel element.
    channelNode = iXmlUtils.GetFirstNamedChild(rootNode, KChannel());    
    if (channelNode.IsNull())
        {
        User::Leave(KErrCorrupt);
        }
        
    iFeedTimestampNode = NULL;
    
    // Process the channel's children.
    aObserver.FeedBeginsL();
    
    // Process the elements in the channel node.
    ProcessElementsL(channelNode, aObserver);

    // Process all of the other elements except the channel element as it is
    // processed above.  This second call is needed in order to support RSS 1.0
    // as well as malformed RSS 2.0.
    ProcessElementsL(rootNode, aObserver);
    
    // Set the timestamp attribute.
    if (iFeedTimestampNode.NotNull())
        {
        ElementHandlerDateL(*this, iXmlUtils, iFeedTimestampNode, 
                EFeedAttributeTimestamp, aObserver);
        }
        
    aObserver.FeedEndsL();
    }


// -----------------------------------------------------------------------------
// CRssFeedParser::IsFeedSupported
//
// Returns true if this feed parser can process the given document. 
// -----------------------------------------------------------------------------
//
TBool CRssFeedParser::IsFeedSupported(CXmlUtils& aXmlUtils, 
        RXmlEngDocument aDocument, const TDesC& /*aContentType*/)
    {
    TXmlEngElement  node;
    
    // Get the root element.
    node = aXmlUtils.GetDocumentFirstElement(aDocument);

    // If the root node is missing the feed isn't supported.    
    if (node.IsNull())
        {
        return EFalse;
        }

    // Check the content-type.
    // TODO:

    // If the root node is not HEAD_STR the feed isn't supported.
    if (!aXmlUtils.IsNamed(node, KRssHead) && !aXmlUtils.IsNamed(node, KRdfHead))
        {
        return EFalse;
        }

    return ETrue;
    }


// -----------------------------------------------------------------------------
// CRssFeedParser::ProcessElementsL
//
// Process all elements except the channel elements.
// -----------------------------------------------------------------------------
//
void CRssFeedParser::ProcessElementsL(TXmlEngElement aNode,
        MFeedParserObserver& aObserver) const
    {
    TXmlEngElement  node = NULL;
    
    node = iXmlUtils.GetFirstElementChild(aNode);
    
    while (node.NotNull())
        {
        // Process the element.
        HandleFeedChildL(node, aObserver);

        // Get the next element.
        node = iXmlUtils.GetNextSiblingElement(node);
        }
    }

        
// -----------------------------------------------------------------------------
// CRssFeedParser::ElementHandlerItemL
//
// A ElementHandler function that populates the Item instance with the 
// values from the given item node.
// -----------------------------------------------------------------------------
//
void CRssFeedParser::ElementHandlerItemL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
        TXmlEngElement aNode, TInt /*aValueId*/, MFeedParserObserver& aObserver)
    {
    CRssFeedParser&  parser(const_cast<CRssFeedParser&>(static_cast<const CRssFeedParser&>(aParser)));
    TXmlEngElement         node;
    
    // Create an empty Item.
    aObserver.ItemBeginsL();
    
    // Process the item's children.
    parser.iItemTimestampNode = NULL;    
    node = aXmlUtils.GetFirstElementChild(aNode);

    while (node.NotNull())
        {
        // Process the element.
        parser.HandleItemChildL(node, aObserver);

        // Get the next element.
        node = aXmlUtils.GetNextSiblingElement(node);
        }
            
    // Postprocess the EItemAttributeIdStr attribute.
    parser.SetItemIdStrAttributeL(aNode, aObserver);
        
    // Postprocess the EItemAttributeTimestamp attribute.
    if (parser.iItemTimestampNode.NotNull())
        {
        ElementHandlerDateL(aParser, aXmlUtils, parser.iItemTimestampNode, 
                EItemAttributeTimestamp, aObserver);
        }
        
    // Add the Item to the Feed.
    aObserver.ItemEndsL();    
    }


// -----------------------------------------------------------------------------
// CRssFeedParser::ElementHandlerEnclosureL
//
// An ElementHandler function that extracts the an enclosure.
// -----------------------------------------------------------------------------
//
void CRssFeedParser::ElementHandlerEnclosureL(const CFeedParser& /*aParser*/, CXmlUtils& aXmlUtils, 
        TXmlEngElement aNode, TInt /*aValueId*/, MFeedParserObserver& aObserver)
    {
    HBufC*  url = NULL;
    HBufC*  resource = NULL;
    HBufC*  type = NULL;
    HBufC*  length = NULL;
    
    // Create an empty enclosure.
    aObserver.EnclosureBeginsL();
    
    // Extract and the enclosure attributes
    url = aXmlUtils.AttributeL(aNode, KUrl);
    CleanupStack::PushL(url);    
        
    resource = aXmlUtils.AttributeL(aNode, KResource);
    CleanupStack::PushL(resource);    
        
    type = aXmlUtils.AttributeL(aNode, KType);
    CleanupStack::PushL(type);    
        
    length = aXmlUtils.AttributeL(aNode, KLength);
    CleanupStack::PushL(length);    
        
    // Add either the resource or url attribute.
    if (resource != NULL)
        {        
        aObserver.AddAttributeL(EEnclosureAttributeLink, *resource);
        }
    else if (url != NULL)
        {        
        aObserver.AddAttributeL(EEnclosureAttributeLink, *url);
        }

    // Add the other attributes.
    if (type != NULL)
        {        
        aObserver.AddAttributeL(EEnclosureAttributeContentType, *type);
        }

    if (length != NULL)
        {        
        aObserver.AddAttributeL(EEnclosureAttributeSize, *length);
        }
    
    // Signal the end of the enclosure.
    aObserver.EnclosureEndsL();
    
    CleanupStack::PopAndDestroy(length);
    CleanupStack::PopAndDestroy(type);
    CleanupStack::PopAndDestroy(resource);
    CleanupStack::PopAndDestroy(url);
    }


// -----------------------------------------------------------------------------
// CRssFeedParser::ElementHandlerTimestampL
//
// A ElementHandler function that determines the timestamp to use.  The
// point is to track the most relevant timestamp.
// -----------------------------------------------------------------------------
//
void CRssFeedParser::ElementHandlerTimestampL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
        TXmlEngElement aNode, TInt aValueId, MFeedParserObserver& /*aObserver*/)
    {
    CRssFeedParser&  parser(const_cast<CRssFeedParser&>(static_cast<const CRssFeedParser&>(aParser)));
    
    // Handle feed related timestamps
    if (aValueId == EFeedAttributeTimestamp)
        {
        if (aXmlUtils.IsNamed(aNode, KLastBuildDate))
            {
            parser.iFeedTimestampNode = aNode;
            }

        // As LastBuildDate is more relevant only update iTimestamp if it wasn't 
        // already set.  PubDate and dc:date mean the same thing, so once it's set, it's set.
        else if (parser.iFeedTimestampNode.IsNull())
            {            
            if (aXmlUtils.IsNamed(aNode, KPubDate) || aXmlUtils.IsNamed(aNode, KDc, KDate))
                {
                parser.iFeedTimestampNode = aNode;
                }
            }
        }
        
    // Otherwise handle item related timestamps
    else
        {
        // PubDate and dc:date mean the same thing, so once it's set, it's set.
        if (parser.iItemTimestampNode.IsNull())
            {
            if (aXmlUtils.IsNamed(aNode, KPubDate) || aXmlUtils.IsNamed(aNode, KDc, KDate))
                {
                parser.iItemTimestampNode = aNode;
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CRssFeedParser::ElementHandlerOtherL
//
// A ElementHandler function that used for feed validation.
// -----------------------------------------------------------------------------
//
void CRssFeedParser::ElementHandlerOtherL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
        TXmlEngElement aNode, TInt /*aValueId*/, MFeedParserObserver& aObserver)
    {
    const CRssFeedParser&  self = static_cast<const CRssFeedParser&>(aParser);
    TXmlEngElement               node = NULL;
    TDesC*                 text;
    
    if ((node = aXmlUtils.GetFirstNamedChild(aNode, KTitle)).NotNull())
        {
        text = self.iXmlUtils.ExtractTextL(node);
        CleanupStack::PushL(text);
        
        if ((text != NULL) && (text->Length() > 0))
            {
            aObserver.OtherTitleL();
            }
            
        CleanupStack::PopAndDestroy(text);
        }
    
    if ((node = aXmlUtils.GetFirstNamedChild(aNode, KDescription)).NotNull())
        {
        text = self.iXmlUtils.ExtractTextL(node);
        CleanupStack::PushL(text);
        
        if ((text != NULL) && (text->Length() > 0))
            {
            aObserver.OtherDescriptionL();
            }
            
        CleanupStack::PopAndDestroy(text);
        }
    
    if ((node = aXmlUtils.GetFirstNamedChild(aNode, KLink)).NotNull())
        {
        text = self.iXmlUtils.ExtractTextL(node);
        CleanupStack::PushL(text);
        
        if ((text != NULL) && (text->Length() > 0))
            {
            aObserver.OtherLinkL();
            }
            
        CleanupStack::PopAndDestroy(text);
        }
    }


// -----------------------------------------------------------------------------
// CRssFeedParser::SetItemIdStrAttributeL
//
// Determine and set the unique IdStr attribute (unique to the feed that is).
// -----------------------------------------------------------------------------
//
void CRssFeedParser::SetItemIdStrAttributeL(TXmlEngElement aItemNode, 
        MFeedParserObserver& aObserver)
    {    
    const TInt  KStrChunk = 30;
    
    TDesC*    aboutAttribute = NULL;
    TDesC*    description = NULL;
    TDesC*    title = NULL;
    HBufC*    idStr = NULL;
    HBufC*    url = NULL;
    TXmlEngElement  node;

    // If the about attribute is present then use it.
    aboutAttribute = iXmlUtils.AttributeL(aItemNode, KAbout);
    if (aboutAttribute != NULL)
        {        
        CleanupStack::PushL(aboutAttribute);
        aObserver.AddAttributeL(EItemAttributeIdStr, *aboutAttribute);
        CleanupStack::PopAndDestroy(aboutAttribute);
        
        return;
        }
        
    // Otherwise if the GUid node is present use it.
    node = iXmlUtils.GetFirstNamedChild(aItemNode, KGuid);
    if (node.NotNull())
        {
        ElementHandlerCDataL(*this, iXmlUtils, node, EItemAttributeIdStr, aObserver);        
        return;
        }

    // Otherwise create a idStr from the first 30 chars of the description and title
    // and the last 30 chars of the url.  This doesn't guarantee a unique id, but 
    // it very likely.
    node = iXmlUtils.GetFirstNamedChild(aItemNode, KDescription);
    description = iXmlUtils.ExtractSimpleTextL(node, KStrChunk);
    CleanupStack::PushL(description);
    
    node = iXmlUtils.GetFirstNamedChild(aItemNode, KTitle);
    title = iXmlUtils.ExtractSimpleTextL(node, KStrChunk);
    CleanupStack::PushL(title);
    
    node = iXmlUtils.GetFirstNamedChild(aItemNode, KLink);
    url = iXmlUtils.ExtractSimpleTextL(node, KStrChunk, ETrue);
    CleanupStack::PushL(url);
    
    // Construct the idStr from the parts.
    TInt  len = 0;
    
    if (description != NULL)
        {
        len += description->Length();
        }
    if (title != NULL)
        {
        len += title->Length();
        }
    if (url != NULL)
        {
        len += url->Length();
        }
    
    idStr = HBufC::NewL(len);
    CleanupStack::PushL(idStr);

    TPtr  ptr(idStr->Des());
    
    if (description != NULL)
        {
        ptr.Append(*description);
        }
    if (title != NULL)
        {
        ptr.Append(*title);
        }
    if (url != NULL)
        {
        ptr.Append(*url);
        }
        
    // Replace any chars that may interfere with the database.
    _LIT(KSpace, " ");
    
    for (TInt i = 0; i < ptr.Length(); i++)
        {
        if (ptr[i] == '\'')
            {
            ptr.Replace(i, 1, KSpace());
            }
        }
    
    // Set the idStr attribute.
    aObserver.AddAttributeL(EItemAttributeIdStr, *idStr);
    
    CleanupStack::PopAndDestroy(idStr);    
    CleanupStack::PopAndDestroy(url);    
    CleanupStack::PopAndDestroy(title);    
    CleanupStack::PopAndDestroy(description);    
    }
