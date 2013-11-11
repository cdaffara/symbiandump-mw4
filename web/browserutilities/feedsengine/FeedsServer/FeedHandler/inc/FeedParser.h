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
* Description:  Feeds parsers base class.
*
*/


#ifndef FEED_PARSER_H
#define FEED_PARSER_H


// INCLUDES
#include <e32base.h>
#include <xmlengdom.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CFeedParser;
class CXmlUtils;
class RXmlEngDocument;
class TXmlEngElement;
class MFeedParserObserver;

// DATA TYPES
typedef void (*ElementHandlerFunctionL)(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
        TXmlEngElement aNode, TInt aValueId, MFeedParserObserver& aObserver);

struct ElementHandlerMapEntry
    {
    TPtrC8                   iElementNamespace;
    TPtrC8                   iElementName;
    TInt                     iValueId;
    ElementHandlerFunctionL  iElementHandlerL;
    };

// FUNCTION PROTOTYPES

// CLASS DECLARATION


/**
*  Feeds parsers base class.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CFeedParser: public CBase
    {
    public:  // Destructor
        /**
        * Destructor.
        */        
        virtual ~CFeedParser();
        
        
    protected:
        /**
        * C++ default constructor.
        */
        CFeedParser(CXmlUtils& aXmlUtils);


    public:  // New Methods
        /**
        * Creates a Feed instance from the given document.
        *
        * @since 3.0
        * @param aDocument A document.
        * @param aObserver The parser's observer.
        * @return void
        */
        virtual void ParseFeedL(RXmlEngDocument aDocument,
                MFeedParserObserver& aObserver) = 0;

        
    protected:  // New Methods
        /**
        * Add a feed ElementHandler mapping.  See AddMappingL.
        *
        * @since 3.0
        * @param aNamespace The namespace for this mapping -- utf8 encoding.
        * @param aElementName The element name for this mapping -- utf8 encoding.
        * @param aValueId The normalized value-id.
        * @param aHandler The element handler function.
        * @return void.
        */
        void AddFeedMappingL(const TDesC8& aNamespace, const TDesC8& aElementName,
                TInt aValueId, ElementHandlerFunctionL aHandler);

        /**
        * Add a feed ElementHandler mapping.  See AddMappingL.
        *
        * @since 3.0
        * @param aNode A node.
        * @param aObserver The parser's observer.
        * @return void.
        */
        void HandleFeedChildL(TXmlEngElement aNode, MFeedParserObserver& aObserver) const;
        
        /**
        * Add a item ElementHandler mapping.  See AddMappingL.
        *
        * @since 3.0
        * @param aNamespace The namespace for this mapping -- utf8 encoding.
        * @param aElementName The element name for this mapping -- utf8 encoding.
        * @param aValueId The normalized value-id.
        * @param aHandler The element handler function.
        * @return void.
        */
        void AddItemMappingL(const TDesC8& aNamespace, const TDesC8& aElementName,
                TInt aValueId, ElementHandlerFunctionL aHandler);

        /**
        * Add an item ElementHandler mapping.  See AddMappingL.
        *
        * @since 3.0
        * @param aNode A node.
        * @param aObserver The parser's observer.
        * @return void.
        */
        void HandleItemChildL(TXmlEngElement aNode, MFeedParserObserver& aObserver) const;

         /**
        * An ElementHandler function that extracts the value from the 
        * child text nodes.
        *
        * @since 3.0
        * @param aParser The parser calling this method.
        * @param aXmlUtils The xml util instance.
        * @param aNode A node.
        * @param aValueId The normalized value-id.
        * @param aObserver The parser's observer.
        * @return void.
        */
        static void ElementHandlerCDataL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
                TXmlEngElement aNode, TInt aValueId, MFeedParserObserver& aObserver);

         /**
        * An ElementHandler function that extracts the value from the 
        * child text nodes then resolves any html entities and removes any markup.
        *
        * @since 3.0
        * @param aParser The parser calling this method.
        * @param aXmlUtils The xml util instance.
        * @param aNode A node.
        * @param aValueId The normalized value-id.
        * @param aObserver The parser's observer.
        * @return void.
        */
        static void ElementHandlerTextL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
                TXmlEngElement aNode, TInt aValueId, MFeedParserObserver& aObserver);

        /**
        * An ElementHandler function that extracts the value from a child
        * url element.  If the element doesn't contain any elements and it contains
        * text it is extracted instead.
        *
        * @since 3.0
        * @param aParser The parser calling this method.
        * @param aXmlUtils The xml util instance.
        * @param aNode A node.
        * @param aValueId The normalized value-id.
        * @param aObserver The parser's observer.
        * @return void.
        */
        static void ElementHandlerUrlChildL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
                TXmlEngElement aNode,  TInt aValueId, MFeedParserObserver& aObserver);

        /**
        * An ElementHandler function that extracts the value from the 
        * child text nodes.  It further performs url related clean up.
        *
        * @since 3.0
        * @param aParser The parser calling this method.
        * @param aXmlUtils The xml util instance.
        * @param aNode A node.
        * @param aValueId The normalized value-id.
        * @param aObserver The parser's observer.
        * @return void.
        */
        static void ElementHandlerUrlL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
                TXmlEngElement aNode, TInt aValueId, MFeedParserObserver& aObserver);
            
        /**
        * An ElementHandler function that extracts the date from the 
        * child text nodes.  It can handle date formats defined in RFC 3339, RFC 822,
        * RFC 1123, RFC 850, and RFC 1036
        *
        * @since 3.0
        * @param aParser The parser calling this method.
        * @param aXmlUtils The xml util instance.
        * @param aNode A node.
        * @param aValueId The normalized value-id.
        * @param aObserver The parser's observer.
        * @return void.
        */
        static void ElementHandlerDateL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
                TXmlEngElement aNode, TInt aValueId, MFeedParserObserver& aObserver);
            
                            
    private:  // New Methods
        /**
        * Makes a mapping between an element's namespance and name with the normalized
        * value-id (i.e. title or description) and a function which extracts out
        * relevant data and applies it to a provided ValueHolder using the value-id 
        * (see HandleChildL).
        *
        * @since 3.0
        * @param aMappings Either iFeedMappings or iItemMapping.
        * @param aNamespace The namespace for this mapping.
        * @param aElementName The element name for this mapping.
        * @param aValueId The normalized value-id.
        * @param aHandler The element handler function.
        * @return void.
        */
        void AddMappingL(RArray<ElementHandlerMapEntry>& aMappings, const TDesC8& aNamespace, 
                const TDesC8& aElementName, TInt aValueId, ElementHandlerFunctionL aHandler);

        /**
        * Using the mapping defined by calls to AddMappingL it passes aNode, 
        * aValueHolder and associated value-id to the associated function.  The method
        * does what is needed to extract the relevant values from the node and applies
        * them on aValueHolder (using the normalized value-id).
        *
        * @since 3.0
        * @param aMappings Either iFeedMappings or iItemMapping.
        * @param aNode A node.
        * @param aObserver The parser's observer.
        * @return void.
        */
        void HandleChildL(const RArray<ElementHandlerMapEntry>& aMappings, TXmlEngElement aNode, 
                MFeedParserObserver& aObserver) const;

        /**
        * Converts the given RFC 3339 date into a TTime.
        *
        * @since 3.1
        * @param aDateStr The date string.
        * @param aDate The resulting TTime.
        * @return void.
        */
        static void ParseRFC3339L(const TDesC8& aDateStr, TTime& aDate);

    
    private:  // Data
        RArray<ElementHandlerMapEntry> iFeedMappings;
        RArray<ElementHandlerMapEntry> iItemMappings;
        
        
    protected:  // Data
        CXmlUtils&                     iXmlUtils;
    };

#endif      // FEED_PARSER_H
            
// End of File