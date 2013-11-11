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
* Description:  Misc. libxml2 related utilities.
*
*/



#include <charconv.h>
#include <escapeutils.h>
#include <utf.h>

#include "CleanupLibXml2.h"  
#include "LeakTracker.h"
#include "Logger.h"
#include "XmlEncoding.h"
#include "XmlEntity.h"
#include "XmlUtils.h"

// Local data-types
struct ErrorContext
    {
    TInt   error;
    TBool  retry;
    };

// Static variables
static CXmlEntity*  sXmlEntity = NULL;


// -----------------------------------------------------------------------------
// EntityResolverL
//
// A local function to resolve entities found in the buffer being parsed. 
// -----------------------------------------------------------------------------
//
static xmlEntityPtr EntityResolverL(void* /*aContext*/, const xmlChar *aName) 
    {
    xmlEntityPtr  entity = NULL;
	
    // Use the standard resolver.
    entity = xmlGetPredefinedEntity(aName);
    
    // Resolve other entities too.
    if ((sXmlEntity != NULL) && (entity == NULL))
        {
        entity = const_cast<xmlEntityPtr>(sXmlEntity->ResolveL(aName));
        }

    return entity;
    }


// -----------------------------------------------------------------------------
// StructuredErrorReporter
//
// A local function to collect errors found while parsing the buffer.
// See http://www.xmlsoft.org/html/libxml-xmlerror.html.
// -----------------------------------------------------------------------------
//
static void StructuredErrorReporter(void* aContext, xmlErrorPtr error)
    {
    _LIT(KErr, "error");
    
    TBool          ignore = EFalse;
    TPtrC          type(KErr);
    ErrorContext*  context = static_cast<ErrorContext*>(aContext);

    if (context != NULL)
        {        
        context->error = error->code; 

        switch (error->code)
            {
            case XML_ERR_UNSUPPORTED_ENCODING:          // 32
                // Try to reparse the buffer with another char-encoding.
                context->retry = ETrue;
                ignore = ETrue;
                break;

            case XML_ERR_UNDECLARED_ENTITY:             // 26
            case XML_WAR_UNDECLARED_ENTITY:             // 27
                type.Set(_L("warning"));
                break;

            case XML_ERR_INTERNAL_ERROR:                // 1
            case XML_ERR_DOCUMENT_EMPTY:                // 4
            case XML_ERR_DOCUMENT_END:                  // 5
            case XML_ERR_INVALID_CHAR:                  // 9
            case XML_ERR_INVALID_DEC_CHARREF:           // 7
            case XML_ERR_ENTITYREF_SEMICOL_MISSING:     // 23
            case XML_NS_ERR_QNAME:                      // 38
            case XML_ERR_LT_IN_ATTRIBUTE:               // 39
            case XML_ERR_ATTRIBUTE_NOT_STARTED:         // 41
            case XML_ERR_ATTRIBUTE_WITHOUT_VALUE:       // 42
            case XML_ERR_ATTRIBUTE_REDEFINED:           // 43
            case XML_ERR_LITERAL_NOT_STARTED:           // 46
            case XML_ERR_PI_NOT_STARTED:                // 47
            case XML_ERR_PI_NOT_FINISHED:               // 61
            case XML_ERR_DOCTYPE_NOT_FINISHED:          // 62
            case XML_ERR_MISPLACED_CDATA_END:           // 63
            case XML_ERR_RESERVED_XML_NAME:             // 64
            case XML_ERR_SPACE_REQUIRED:                // 65
            case XML_ERR_NAME_REQUIRED:                 // 68
            case XML_ERR_URI_REQUIRED:                  // 70
            case XML_ERR_GT_REQUIRED:                   // 73
            case XML_ERR_LTSLASH_REQUIRED:              // 74
            case XML_ERR_HYPHEN_IN_COMMENT:             // 80
            case XML_ERR_TAG_NAME_MISMATCH:             // 76
            case XML_ERR_TAG_NOT_FINISHED:              // 77
            case XML_NS_ERR_UNDEFINED_NAMESPACE:        // 201
                // Just log these as errors and continue if possible.
                break;

            default:
                break;
            }
        }
        
    if (ignore == EFalse)
        {
        FEED_LOG2(_L("Feeds"), _L("Feeds.log"), 
                EFileLoggingModeAppend, _L("FEEDS: parser %S!!! Code:%d"), &type, error->code);
        }
    }


// -----------------------------------------------------------------------------
// CXmlUtils::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXmlUtils* CXmlUtils::NewL()
    {
    CXmlUtils* self = new (ELeave) CXmlUtils();
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CXmlUtils::CXmlUtils
//
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CXmlUtils::CXmlUtils():
        iLeakTracker(CLeakTracker::ELibXml2)
    {
    }
        

// -----------------------------------------------------------------------------
// CXmlEncoding::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXmlUtils::ConstructL()
    {
    // will call XmlEngine::XmlEngineAttachL()
    iImpl.OpenL();

    // Create the XmlEncoding singleton.
    iXmlEncoding = CXmlEncoding::NewL();
    
    // Create the XmlEntity singleton.
    iXmlEntity = CXmlEntity::NewL();
    }
        

// -----------------------------------------------------------------------------
// CXmlUtils::~CXmlUtils
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CXmlUtils::~CXmlUtils()
    {
    delete iXmlEncoding;
    delete iXmlEntity;

    // will call XmlEngine::XmlEngineCleanup()
    iImpl.Close();
    }


// -----------------------------------------------------------------------------
// CXmlUtils::ParseBufferL
//
// Returns a libxml2 document from the provided buffer.
// -----------------------------------------------------------------------------
//
RXmlEngDocument CXmlUtils::ParseBufferL(const TDesC8& aBuffer, const TDesC& aCharSet) const
    {
    xmlDocPtr         document = NULL;
    xmlParserCtxtPtr  contxt = NULL;
    ErrorContext      errorContext;
    HBufC8*           utf8Buffer = NULL;
    const xmlChar*    parseBuffer = NULL;
    TInt              parseLength = 0;
    TUint             encoding;
    RXmlEngDocument   doc;
    
    if (aBuffer.Length() == 0)
        {
        User::Leave(KErrArgument);
        }
    
    // Prepare the parser's context.
  	User::LeaveIfNull(contxt = xmlNewParserCtxt());
    
    sXmlEntity = iXmlEntity;
    contxt->sax->getEntity = EntityResolverL;
    CleanupStack::PushL(TCleanupItem(&CleanupParseBuffer, contxt));
    
    // Set the error callback
    xmlSetStructuredErrorFunc(&errorContext, StructuredErrorReporter);

    // If need be convert the buffer to utf8.
    if (iXmlEncoding->DetermineCharEncodingL(aBuffer, aCharSet, encoding))
        {
        if (encoding != KCharacterSetIdentifierUtf8)
            {
            utf8Buffer = iXmlEncoding->ConvertToUtf8L(encoding, aBuffer);
            CleanupStack::PushL(utf8Buffer);

            // Skip anything before the xml prolog.
            parseBuffer = utf8Buffer->Ptr();
            parseLength = utf8Buffer->Length();

            SkipCharsBeforeXmlProlog(&parseBuffer, parseLength);
            }
        }

    // Otherwise its already utf8 so use the provided buffer.
    if (utf8Buffer == NULL)
        {
        // Skip anything before the xml prolog.
        parseBuffer = aBuffer.Ptr();
        parseLength = aBuffer.Length();

        SkipCharsBeforeXmlProlog(&parseBuffer, parseLength);
        }

    // Parse the document.
    errorContext.error = XML_ERR_OK;
    errorContext.retry = EFalse;        

    document = xmlSAXParseMemory(contxt->sax, (char*) parseBuffer, parseLength, FALSE);
    
    if (document == NULL)
        {
        User::Leave(KErrCorrupt);
        }

    if (utf8Buffer != NULL)
        {
        CleanupStack::PopAndDestroy(utf8Buffer);
        }
    
    CleanupStack::PopAndDestroy(/*contxt*/);    

    // Wrap and return the document.
    CleanupStack::PushL(document);
    doc.OpenL( (RXmlEngDOMImplementation&)iImpl, document );
    CleanupStack::Pop(/*document*/);
    
    return doc;
    }


// -----------------------------------------------------------------------------
// CXmlUtils::CleanupParseBuffer
// 
// The cleanup method for ParseBufferL
// -----------------------------------------------------------------------------
//
void CXmlUtils::CleanupParseBuffer(TAny *aPtr)
    {
    xmlResetLastError();
    xmlSetGenericErrorFunc(NULL, NULL);
    xmlFreeParserCtxt(static_cast<xmlParserCtxtPtr>(aPtr));
    }


// -----------------------------------------------------------------------------
// CXmlUtils::SkipCharsBeforeXmlProlog
// 
// Skip any chars before the XML-prolog.
// -----------------------------------------------------------------------------
//
void CXmlUtils::SkipCharsBeforeXmlProlog(const TUint8** aString, TInt& aLen) const
    {
    _LIT8(KStart, "<?xml");
    
    const xmlChar*  origStr = *aString;
    TInt            origLen = aLen;

    while (aLen > 0)
        {
        if ((aLen >= 5) && (xmlStrncasecmp(*aString, KStart().Ptr(), 5) == 0))
            {
            break;
            }
        else
            {
            (*aString)++;
            aLen--;
            }
        }

    // If the xml prolog is missing then do nothing.
    if (aLen == 0)
        {
        *aString = origStr;
        aLen = origLen;
        }
    }


// -----------------------------------------------------------------------------
// CXmlUtils::CleanupUrlL
// 
// Resolves any entities and escaped chars in the given url.  
// -----------------------------------------------------------------------------
//
void CXmlUtils::CleanupUrlL(TDes& aUrl) const
    {
    HBufC*  temp = NULL;

    // Resolve entities.
    (void) ResolveEntitiesL(aUrl);

    // Unescape the url.
    temp = EscapeUtils::EscapeDecodeL(aUrl);
    CleanupStack::PushL(temp);

    // Trim leading and trailing whitespace.
    temp->Des().Trim();

    // Copy temp back to aUrl.  This is always safe because temp->Des().Length()
    // is always <= aUrl.Length().
    aUrl.Copy(*temp);

    CleanupStack::PopAndDestroy(temp);
    }    


// -----------------------------------------------------------------------------
// CXmlUtils::CleanupMarkupL
// 
// Resolves any html entities and removes any markup found in the given descriptor.
// -----------------------------------------------------------------------------
//
TBool CXmlUtils::CleanupMarkupL(TDes& aBuffer, TInt aNewLineChar) const
    {
    TBool   modified = EFalse;

    // Strip all markup.
    modified |= StripMarkupL(aBuffer, aNewLineChar);
    
    // Resolve entities.
    modified |= ResolveEntitiesL(aBuffer);

    // Strip the CDATA markers.
    modified |= StripCDataMarkers(aBuffer);

    // Strip all markup created after entities were resolved.
    modified |= StripMarkupL(aBuffer, aNewLineChar);
    
    // Strip leading and trailing whitespace.
    aBuffer.Trim();

    return modified;
    }


// -----------------------------------------------------------------------------
// CXmlUtils::ResolveEntitiesL
// 
// Resolves any entities found in aOrig.
// -----------------------------------------------------------------------------
//
TBool CXmlUtils::ResolveEntitiesL(TDes& aBuffer) const
	{
    const TUint  KEntityStart =	'&';
    const TUint  KEntityEnd = ';';

	TInt         entityStart = -1;
    TInt         currentPos =  0;    
    TBool        entityFound = EFalse;
	TBool        textHasChanged = EFalse;

    while (currentPos < aBuffer.Length())
        {
        // If this is the beginning of an entity...
        if (aBuffer[currentPos] == KEntityStart) 
            {
            entityStart = currentPos;
            entityFound = ETrue;
            currentPos++;
            }

        // If this is the end of an entity...
        else if ((aBuffer[currentPos] == KEntityEnd) && entityFound)
            {
            TUint16    ucs2Value;
            TInt       entityLength = currentPos - entityStart + 1;
            TPtrC      entityStr(aBuffer.Mid(entityStart + 1, entityLength - 2));
            TBuf16<1>  entityCode;
            
            // Resolve the entity inline.
            iXmlEntity->ResolveL(entityStr, ucs2Value);            
            entityCode.SetLength(1);
            entityCode[0] = ucs2Value;

            aBuffer.Replace(entityStart, entityLength, entityCode);

            currentPos = entityStart;
			textHasChanged = ETrue;
            entityStart = -1;
            entityFound = EFalse;
            }

        // Otherwise just move on.
        else
            {
            currentPos++;
            }
        }

	return textHasChanged;
	}


// -----------------------------------------------------------------------------
// CXmlUtils::StripMarkup
// 
// Remove any markup found in the given descriptor.
// -----------------------------------------------------------------------------
//
TBool CXmlUtils::StripMarkupL(TDes& aBuffer, TInt aNewLineChar) const
    {
	TInt    tagStart = -1;
    TInt    currentPos = 0;    
    TBool   tagStartFound = EFalse;
	TBool   textHasChanged = EFalse;

    // TODO: 1) Remove xml comments as well.

    while (currentPos < aBuffer.Length())
        {
        // It's at the beginning of a tag.
        if (aBuffer.Mid(currentPos, 1).Compare(_L("<")) == 0)
            {
            tagStartFound = ETrue;
            tagStart = currentPos;
            currentPos++;
            }

        // It's at the end of a tag.
        else if ((aBuffer.Mid(currentPos,1).Compare(_L(">")) == 0) && 
                tagStartFound)
            {
			TBuf<6>  tag;
			TInt     tagLength;

            // Extract enough of the tag to determine if it needs to insert a
            // newline.
            tagLength = Min(6, currentPos - tagStart + 1);
			tag.Copy(aBuffer.Mid(tagStart, tagLength));
			tag.LowerCase();

            // If it found a <p> or <br> tag insert a newline.
            // TODO: This fails if the tag includes attributes (i.e. style).
			if ((aNewLineChar != 0) && 
			        ((tag.Compare(_L("<br>")) == 0) ||
                    (tag.Compare(_L("<br/>")) == 0) ||
                    (tag.Compare(_L("<br />")) == 0) ||
                    (tag.Compare(_L("<p>")) == 0)))
				{
                // Delete the tag.
				aBuffer.Delete(tagStart, currentPos - tagStart + 1);

                // Insert the newline.
				TBuf<1> chr;
				_LIT(KFormat,"%c");

				chr.Format(KFormat, aNewLineChar);
				aBuffer.Insert(tagStart, chr);

				currentPos = tagStart + 1;
				textHasChanged = ETrue;
				}

            // Otherwise just delete the tag.
			else
				{
				aBuffer.Delete(tagStart, currentPos - tagStart + 1);
				currentPos = tagStart;
				textHasChanged = ETrue;
				}

            tagStart = -1;
			tagStartFound = EFalse;
            }

        // Otherwise just skip to the next char.
        else
            {
            currentPos++;
            }
        }

    if (textHasChanged)
        {
        aBuffer.TrimAll();
        }

	return textHasChanged;
    }


// -----------------------------------------------------------------------------
// CXmlUtils::StripCDataMarkers
// 
// Remove any CDATA markers in the given descriptor.
// -----------------------------------------------------------------------------
//
TBool CXmlUtils::StripCDataMarkers(TDes& aBuffer) const
    {
    _LIT(KMarkerBegin, "<![CDATA[");
    _LIT(KMarkerEnd, "]]>");

    const TInt KMarkerBeginLen = KMarkerBegin().Length();
    const TInt KMarkerEndLen = KMarkerEnd().Length();
    
    TInt    currentPos = 0;
    TBool   markerStartFound = EFalse;
	TBool   textHasChanged = EFalse;

    while (currentPos < aBuffer.Length())
        {
        // Handle the the beginning of a marker.                
        if ((currentPos <= (aBuffer.Length() - KMarkerBeginLen)) && 
                aBuffer.Mid(currentPos, KMarkerBeginLen).Compare(KMarkerBegin) == 0)
            {
            markerStartFound = ETrue;
            aBuffer.Delete(currentPos, KMarkerBeginLen);
            textHasChanged = ETrue;
            }

        // Handle the the end of a marker.                
        else if (markerStartFound && (currentPos <= (aBuffer.Length() - KMarkerEndLen)) && 
                aBuffer.Mid(currentPos, KMarkerEndLen).Compare(KMarkerEnd) == 0)
            {
            markerStartFound = EFalse;
            aBuffer.Delete(currentPos, KMarkerEndLen);
            textHasChanged = ETrue;
            }

        // Otherwise just skip to the next char.
        else
            {
            currentPos++;
            }
        }

    if (textHasChanged)
        {
        aBuffer.TrimAll();
        }

	return textHasChanged;
    }


// -----------------------------------------------------------------------------
// CXmlUtils::ExtractTextL
//
// Performs a deep extraction of the text children of the given node.  The result
// is returned as a 16-bit descriptor.    The char encoding is always ucs2.
// -----------------------------------------------------------------------------
//
HBufC* CXmlUtils::ExtractTextL(TXmlEngElement aElement) const
    {
    TInt     size;
    HBufC*   ucs2Des = NULL;
    RBuf8    rbuf;
    
    if (aElement.IsNull())
        {
        return NULL;
        }
        
    // Perform the deep extraction.
    size = aElement.InnerXmlL( rbuf );
    rbuf.CleanupClosePushL();

    // Resolve any remaining entities and strip any CDATA markers.
    if (size > 0)
        {
        ucs2Des = HBufC::NewLC( rbuf.Length() );
        TPtr  ptr(ucs2Des->Des());
        //ptr.Copy( rbuf );
        TInt unconverted = CnvUtfConverter::ConvertToUnicodeFromUtf8(ptr,rbuf);
        while( unconverted>0)
        {
        	
        	ucs2Des->ReAllocL(rbuf.Length()+unconverted);
        	ptr.Set(ucs2Des->Des());
        	unconverted = CnvUtfConverter::ConvertToUnicodeFromUtf8(ptr,rbuf);
        }
        
        ResolveEntitiesL(ptr);
        StripCDataMarkers(ptr);
        ptr.TrimAll();

        CleanupStack::Pop(ucs2Des);
        }

    CleanupStack::PopAndDestroy( /*rbuf*/ );

    return ucs2Des;
    }


// -----------------------------------------------------------------------------
// CXmlUtils::ExtractSimpleTextL
//
// Performs a deep extraction of the text children of the given node.  The result
// is returned as a 16-bit descriptor.    The char encoding is always ucs2.  This
// method is different from ExtractTextL in that it doesn't resolve entities or
// do any other clean up.  It also has a length param.
// -----------------------------------------------------------------------------
//
HBufC* CXmlUtils::ExtractSimpleTextL(TXmlEngElement aElement, TInt aMaxLength, 
        TBool aFromEnd) const
    {
    xmlChar*  xmlStr;
    HBufC*    ucs2Des = NULL;
    RBuf8   rbuf;

    if (aElement.IsNull())
        {
        return NULL;
        }
        
    // Extract the text from the element.
    aElement.WholeTextContentsCopyL( rbuf );
    rbuf.CleanupClosePushL();
    xmlStr = (xmlChar*) rbuf.Ptr();
        
    // If need be, trim the string.
    if (aMaxLength > 0)
        {
        if (aFromEnd)
            {
            if (rbuf.Length() < aMaxLength)
                {
                xmlStr = xmlUTF8Strndup(xmlStr, aMaxLength);
                }
            else
                {
                xmlStr = xmlUTF8Strndup(xmlStr + rbuf.Length() - aMaxLength, aMaxLength);
                }
            }
        else
            {            
            xmlStr = xmlUTF8Strndup(xmlStr, aMaxLength);
            }
        CleanupLibXml2::PushL(xmlStr);
        }

    // Convert the value into a TDesC
    TXmlEngString  temp((char*) xmlStr);
    
    ucs2Des = temp.AllocL();
    ucs2Des->Des().Trim();

    if (aMaxLength > 0)
        {
        CleanupStack::PopAndDestroy(/*xmlStr*/);
        }
        
    CleanupStack::PopAndDestroy( /*rbuf*/ );

    return ucs2Des;
    }


// -----------------------------------------------------------------------------
// CXmlUtils::GetDocumentFirstElement
// 
// Returns the first element in the given document.
// -----------------------------------------------------------------------------
//
TXmlEngElement CXmlUtils::GetDocumentFirstElement(RXmlEngDocument aDocument) const
    {
    const TXmlEngElement  KNullElement;
    TXmlEngElement        element;    

    // Get the first child.
    element = aDocument.DocumentElement();
        
    // Loop through the children looking for the first element.
    while (element.NotNull())
        {
        if ((element.NodeType() == TXmlEngNode::EElement))
            {
            return element;
            }
            
        element = element.NextSibling().AsElement();
        }
        
    // Element not found, return a "null" node.
    return KNullElement;
    }


// -----------------------------------------------------------------------------
// CXmlUtils::GetFirstElementChild
// 
// Returns the first child of the given node.
// -----------------------------------------------------------------------------
//
TXmlEngElement CXmlUtils::GetFirstElementChild(TXmlEngElement aElement) const
    {
    const TXmlEngElement      KNullElement;
    RXmlEngNodeList<TXmlEngElement> children;
    TXmlEngElement            child;
        
    // Get the first element -- children only contains TElements...
    aElement.GetChildElements(children);
    if (children.HasNext())
        {
        return children.Next();
        }
            
    return KNullElement;
    }


// -----------------------------------------------------------------------------
// CXmlUtils::GetFirstNamedChild
// 
// Returns the first child of the given node with the provided name.  aUtf8Name
// MUST be null terminated.
// -----------------------------------------------------------------------------
//
TXmlEngElement CXmlUtils::GetFirstNamedChild(TXmlEngElement aElement, const TDesC8& aUtf8Name) const
    {
    const TXmlEngElement      KNullElement;
    RXmlEngNodeList<TXmlEngElement> children;
    TXmlEngElement            child;
    
    // Get the first element -- children only contains TElements...
    aElement.GetChildElements(children);
    while (children.HasNext())
        {
        child = children.Next();
        
        if (IsNamed(child, aUtf8Name))
            {
            return child;
            }
        }
            
    return KNullElement;
    }


// -----------------------------------------------------------------------------
// CXmlUtils::GetNextSiblingElement
// 
// Returns the next sibling of the given node.
// -----------------------------------------------------------------------------
//
TXmlEngElement CXmlUtils::GetNextSiblingElement(TXmlEngElement aElement)
    {
    const TXmlEngElement  KNullElement;
    TXmlEngElement        node;
    
    if (aElement.NotNull())
        {
        node = aElement.NextSibling().AsElement();

        // Loop through the siblings looking for the next element.
        while (node.NotNull())
            {
            if ((node.NodeType() == TXmlEngNode::EElement))
                {
                return node;
                }
            
            node = node.NextSibling().AsElement();
            }
        }
        
    return KNullElement;
    }


// -----------------------------------------------------------------------------
// CXmlUtils::IsNamed
// 
// Returns true if the given node has a name of aName.  This method doesn't 
// compare the namespace.
// -----------------------------------------------------------------------------
//
TBool CXmlUtils::IsNamed(TXmlEngElement aElement, const TDesC8& aUtf8Name) const
    {
    return (xmlStrcasecmp((const xmlChar *) aElement.Name().Ptr(), 
            (const xmlChar *) aUtf8Name.Ptr()) == 0);
    }


// -----------------------------------------------------------------------------
// CXmlUtils::IsNamed
// 
// Returns true if the given node has match the given namespace and name.
// -----------------------------------------------------------------------------
//
TBool CXmlUtils::IsNamed(TXmlEngElement aElement, const TDesC8& aUtf8NameSpace, 
        const TDesC8& aUtf8Name) const
    {
    TPtrC8  ns = aElement.Prefix();
    
    // First ensure the names match.
    if (!IsNamed(aElement, aUtf8Name))
        {
        return EFalse;
        }

    // Return false if one of the namespaces is NULL and the other isn't.
    if ((ns.Length() != 0 && (aUtf8NameSpace.Length() == 0)) ||
            (ns.Length() == 0 && (aUtf8NameSpace.Length() > 0)))
        {
        return EFalse;
        }
        
    // Return false if both are non-null, but don't match.
    if (ns.Length() != 0 && (aUtf8NameSpace.Length() >= 0) &&
            (xmlStrcasecmp((const xmlChar *) ns.Ptr(), 
            (const xmlChar *) aUtf8NameSpace.Ptr()) != 0))
        {
        return EFalse;
        }

    // Its a match.
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CXmlUtils::AttributeL
// 
// Returns the attribute's value or NULL if the attribute wasn't present.
// -----------------------------------------------------------------------------
//
HBufC* CXmlUtils::AttributeL(TXmlEngElement aElement, const TDesC8& aAttribute) const
    {    
    TXmlEngAttr     attr;
    HBufC*          value = NULL;
    RBuf8           rbuf;
    
    // Get the value.
    attr = aElement.AttributeNodeL( aAttribute );    
    if (attr.NotNull())
        {
        attr.WholeValueCopyL( rbuf );
        rbuf.CleanupClosePushL();
        value = HBufC::NewL( rbuf.Length() + 32 );
        TPtr ptr(value->Des());
        TInt unconverted = CnvUtfConverter::ConvertToUnicodeFromUtf8(ptr,rbuf);
        CleanupStack::PopAndDestroy( /*rbuf*/ );
        }
    return value;
    }


