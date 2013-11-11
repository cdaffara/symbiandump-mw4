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
#include "AtomFeedParser.h"
#include "XmlUtils.h"


// Element and attribute names used by this parser.
_LIT8(KHead, "feed");

_LIT8(KTagline, "tagline");
_LIT8(KSubtitle, "subtitle");
_LIT8(KTitle, "title");
_LIT8(KLink, "link");
_LIT8(KId, "id");
_LIT8(KSummary, "summary");
_LIT8(KContent, "content");
_LIT8(KEntry, "entry");
//_LIT8(KXmlns, "xmlns");
_LIT8(KRel, "rel");
_LIT8(KHref, "href");
_LIT8(KType, "type");
_LIT8(KLength, "length");
_LIT8(KPublished, "published");
_LIT8(KUpdated, "updated");
_LIT8(KModified, "modified");
_LIT8(KIssued, "issued");

_LIT(KAlternate, "alternate");
_LIT(KEnclosure, "enclosure");

//_LIT8(KAtom1_0Namespace, "http://www.w3.org/2005/Atom");

// -----------------------------------------------------------------------------
// AtomFeedParser::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAtomFeedParser* CAtomFeedParser::NewL(CXmlUtils& aXmlUtils)
    {
    CAtomFeedParser* self = new (ELeave) CAtomFeedParser(aXmlUtils);
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CAtomFeedParser::CAtomFeedParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAtomFeedParser::CAtomFeedParser(CXmlUtils& aXmlUtils):
        CFeedParser(aXmlUtils), iLeakTracker(CLeakTracker::EAtomFeedParser)
    {
    }
        

// -----------------------------------------------------------------------------
// CAtomFeedParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAtomFeedParser::ConstructL()
    {
    // The mappings are used to map elements to handler methods.  For example when
    // a <title> element is found its contents are extracted as CDATA and passed
    // on to the observer as a EFeedAttributeTitle.
    
    //
    // Add mappings to process the children of a channel.
    //
    AddFeedMappingL(KNullDesC8(), KEntry(), EFeedAttributeUnused, ElementHandlerItemL);

    AddFeedMappingL(KNullDesC8(), KTitle(), EFeedAttributeTitle, ElementHandlerTextL);
    AddFeedMappingL(KNullDesC8(), KLink(), EFeedAttributeLink, ElementHandlerLinkL);
    AddFeedMappingL(KNullDesC8(), KTagline(), EFeedAttributeDescription, ElementHandlerCDataL);
    AddFeedMappingL(KNullDesC8(), KSubtitle(), EFeedAttributeDescription, ElementHandlerCDataL);

    AddFeedMappingL(KNullDesC8(), KPublished(), EFeedAttributeTimestamp, ElementHandlerTimestampL); 
    AddFeedMappingL(KNullDesC8(), KUpdated(), EFeedAttributeTimestamp, ElementHandlerTimestampL); 
    AddFeedMappingL(KNullDesC8(), KModified(), EFeedAttributeTimestamp, ElementHandlerTimestampL); 
    AddFeedMappingL(KNullDesC8(), KIssued(), EFeedAttributeTimestamp, ElementHandlerTimestampL);        

    //
    // Add mappings to process the children of an item.
    //
    AddItemMappingL(KNullDesC8(), KTitle(), EItemAttributeTitle, ElementHandlerTextL);
    AddItemMappingL(KNullDesC8(), KLink(), EItemAttributeLink, ElementHandlerLinkL);
    AddItemMappingL(KNullDesC8(), KContent(), EItemAttributeDescription, ElementHandlerCDataL);    
    AddItemMappingL(KNullDesC8(), KSummary(), EItemAttributeDescription, ElementHandlerCDataL); 
    
    AddItemMappingL(KNullDesC8(), KPublished(), EItemAttributeTimestamp, ElementHandlerTimestampL); 
    AddItemMappingL(KNullDesC8(), KUpdated(), EItemAttributeTimestamp, ElementHandlerTimestampL); 
    AddItemMappingL(KNullDesC8(), KModified(), EItemAttributeTimestamp, ElementHandlerTimestampL); 
    AddItemMappingL(KNullDesC8(), KIssued(), EItemAttributeTimestamp, ElementHandlerTimestampL);        
    }


// -----------------------------------------------------------------------------
// CAtomFeedParser::~CAtomFeedParser
// Deconstructor.
// -----------------------------------------------------------------------------
//
CAtomFeedParser::~CAtomFeedParser()
    {
    }

        
// -----------------------------------------------------------------------------
// CAtomFeedParser::IsFeedSupported
//
// Returns true if this feed parser can process the given document. 
// -----------------------------------------------------------------------------
//
TBool CAtomFeedParser::IsFeedSupported(CXmlUtils& aXmlUtils, 
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

    // If the root node is not HEAD_STR the feed isn't supported.
    if (!aXmlUtils.IsNamed(node, KHead))
        {
        return EFalse;
        }

    return ETrue;
    }


// -----------------------------------------------------------------------------
// CAtomFeedParser::ParseFeedL
//
// Creates a Feed instance from the given document. 
// -----------------------------------------------------------------------------
//
void CAtomFeedParser::ParseFeedL(RXmlEngDocument aDocument, 
        MFeedParserObserver& aObserver)
    {
	TXmlEngElement             rootNode;
    RXmlEngNodeList<TXmlEngElement>  children;

    // Get the feed node.
    rootNode = iXmlUtils.GetDocumentFirstElement(aDocument);

    iFeedTimestampNode = NULL;
    
    // Process the channel's children.
    aObserver.FeedBeginsL();
    
    rootNode.GetChildElements(children);
    
    while (children.HasNext())
        {
        TXmlEngElement    node = children.Next();
        
        // Process the element.
        HandleFeedChildL(node, aObserver);
        }

    // Set the timestamp attribute.
    if (iFeedTimestampNode.NotNull())
        {
        ElementHandlerDateL(*this, iXmlUtils, iFeedTimestampNode, 
                EFeedAttributeTimestamp, aObserver);
        }
        
    aObserver.FeedEndsL();
    }


// -----------------------------------------------------------------------------
// CAtomFeedParser::ElementHandlerItemL
//
// A ElementHandler function that populates the Item instance with the 
// values from the given item node.
// -----------------------------------------------------------------------------
//
void CAtomFeedParser::ElementHandlerItemL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
        TXmlEngElement aNode, TInt /*aValueId*/, MFeedParserObserver& aObserver)
    {
    CAtomFeedParser&  parser(const_cast<CAtomFeedParser&>(static_cast<const CAtomFeedParser&>(aParser)));
    TXmlEngElement          node = NULL;
    
    // Create an empty Item.
    aObserver.ItemBeginsL();
    
    // Process the item's children.
    node = aXmlUtils.GetFirstElementChild(aNode);
    
    parser.iItemTimestampNode = NULL;
    
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
// CAtomFeedParser::ElementHandlerLinkL
//
// A ElementHandler function that handles the link element.
// -----------------------------------------------------------------------------
//
void CAtomFeedParser::ElementHandlerLinkL(const CFeedParser& /*aParser*/, CXmlUtils& aXmlUtils, 
        TXmlEngElement aNode, TInt aValueId, MFeedParserObserver& aObserver)
    {
    HBufC*  rel = NULL;
    HBufC*  href = NULL;
    
    // Get the link's relationship.
    rel = aXmlUtils.AttributeL(aNode, KRel);
    CleanupStack::PushL(rel);
        
    // Get the link's url.
    // TODO: Resolve the href using the base element or feed url.
    href = aXmlUtils.AttributeL(aNode, KHref);
    CleanupStack::PushL(href);

    if ((href == NULL) || (href->Length() == 0))
        {
        CleanupStack::PopAndDestroy(href);
        CleanupStack::PopAndDestroy(rel);
        
        return;
        }

    // The link points to an item's full story or feed's main web page.
    if ((rel == NULL) || (rel->CompareF(KAlternate) == 0))
        {
        aObserver.AddAttributeL(aValueId, *href);
        }
                
    // The link points to an enclsoure.
    else if (rel->CompareF(KEnclosure) == 0)
        {
        HBufC*  type = NULL;
        HBufC*  title = NULL;
        HBufC*  length = NULL;
            
        type = aXmlUtils.AttributeL(aNode, KType);
        CleanupStack::PushL(type);
        title = aXmlUtils.AttributeL(aNode, KTitle);
        CleanupStack::PushL(title);
        length = aXmlUtils.AttributeL(aNode, KLength);
        CleanupStack::PushL(length);

        aObserver.EnclosureBeginsL();

        if (type != NULL)
            {                    
            aObserver.AddAttributeL(EEnclosureAttributeContentType, *type);
            }
        if (length != NULL)
            {                    
            aObserver.AddAttributeL(EEnclosureAttributeSize, *length);
            }
        if (href != NULL)
            {                    
            aObserver.AddAttributeL(EEnclosureAttributeLink, *href);
            }
        if (title != NULL)
            {                    
            aObserver.AddAttributeL(EEnclosureAttributeTitle, *title);
            }

        aObserver.EnclosureEndsL();

        CleanupStack::PopAndDestroy(length);
        CleanupStack::PopAndDestroy(title);
        CleanupStack::PopAndDestroy(type);
        }
    
    CleanupStack::PopAndDestroy(href);
    CleanupStack::PopAndDestroy(rel);
    }


// -----------------------------------------------------------------------------
// CAtomFeedParser::ElementHandlerTimestampL
//
// A ElementHandler function that determines the timestamp to use.  The
// point is to track the most relevant timestamp (updated and modified take 
// precedence over published and issued).
// -----------------------------------------------------------------------------
//
void CAtomFeedParser::ElementHandlerTimestampL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
        TXmlEngElement aNode, TInt aValueId, MFeedParserObserver& /*aObserver*/) 
    {
    CAtomFeedParser&  parser(const_cast<CAtomFeedParser&>(static_cast<const CAtomFeedParser&>(aParser)));
    TXmlEngElement*         timestampNode;
    
    if (aValueId == EFeedAttributeTimestamp)
        {
        timestampNode = &parser.iFeedTimestampNode;
        }
    else
        {
        timestampNode = &parser.iItemTimestampNode;
        }
        
    // As updated and modified mean the same thing (Atom 1.0 vs 0.3) just blindly
    // update iTimestamp.
    if (aXmlUtils.IsNamed(aNode, KUpdated))
        {
        *timestampNode = aNode;
        }
    else if (aXmlUtils.IsNamed(aNode, KModified))
        {
        *timestampNode = aNode;
        }
        
        
    // Only update iTimestamp if it wasn't already set.
    else if (timestampNode->IsNull())
        {
        if (aXmlUtils.IsNamed(aNode, KPublished))
            {
            *timestampNode = aNode;
            }
        else if (aXmlUtils.IsNamed(aNode, KIssued))
            {
            *timestampNode = aNode;
            }
        }
    }


// -----------------------------------------------------------------------------
// CAtomFeedParser::SetItemIdStrAttributeL
//
// Determine and set the unique IdStr attribute (unique to the feed that is).
// -----------------------------------------------------------------------------
//
void CAtomFeedParser::SetItemIdStrAttributeL(TXmlEngElement aItemNode, 
        MFeedParserObserver& aObserver)
    {    
    const TInt  KStrChunk = 30;
    
    TDesC*    content = NULL;
    TDesC*    summary = NULL;
    TDesC*    description = NULL;
    TDesC*    title = NULL;
    HBufC*    url = NULL;
    HBufC*    idStr = NULL;
    TXmlEngElement  node;

    // If the id node is present use it.
    node = iXmlUtils.GetFirstNamedChild(aItemNode, KId);
    if (node.NotNull())
        {
        ElementHandlerCDataL(*this, iXmlUtils, node, EItemAttributeIdStr, aObserver);        
        return;
        }

    // Otherwise create a idStr from the first 30 chars of the description and title
    // and the last 30 chars of the url.  This doesn't guarantee a unique id, but 
    // it very likely.
    node = iXmlUtils.GetFirstNamedChild(aItemNode, KSummary);
    summary = iXmlUtils.ExtractSimpleTextL(node, KStrChunk);
    CleanupStack::PushL(summary);
        
    node = iXmlUtils.GetFirstNamedChild(aItemNode, KContent);
    content = iXmlUtils.ExtractSimpleTextL(node, KStrChunk);
    CleanupStack::PushL(content);

    node = iXmlUtils.GetFirstNamedChild(aItemNode, KTitle);
    title = iXmlUtils.ExtractSimpleTextL(node, KStrChunk);
    CleanupStack::PushL(title);
    
    node = iXmlUtils.GetFirstNamedChild(aItemNode, KLink);
    url = iXmlUtils.ExtractSimpleTextL(node, KStrChunk, ETrue);
    CleanupStack::PushL(url);

    // Determine whether content or summary will be used for the description.
    description = summary;
    
    if (content != NULL)
        {
        if (summary == NULL)
            {
            description = content;
            }
        else if (content->Length() > summary->Length())
            {
            description = content;
            }
        }

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
    CleanupStack::PopAndDestroy(content);    
    CleanupStack::PopAndDestroy(summary);    
    }
