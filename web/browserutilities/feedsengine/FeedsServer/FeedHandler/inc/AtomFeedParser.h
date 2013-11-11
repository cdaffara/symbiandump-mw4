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
* Description:  An Atom parser.
*
*/


#ifndef ATOM_FEED_PARSER_H
#define ATOM_FEED_PARSER_H


// INCLUDES
#include "FeedParser.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CXmlUtils;
class RXmlEngDocument;

// CLASS DECLARATION


/**
*  An Atom parser.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class CAtomFeedParser: public CFeedParser
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CAtomFeedParser* NewL(CXmlUtils& aXmlUtils);
        
        /**
        * Destructor.
        */        
        virtual ~CAtomFeedParser();
        
        
    public: // From FeedParser
        /**
        * Creates a Feed instance from the given document.
        *
        * @since 3.0
        * @param aDocument A document.
        * @param aObserver The parser's observer.
        * @return void
        */
        virtual void ParseFeedL(RXmlEngDocument aDocument, 
                MFeedParserObserver& aObserver);


    public: // New methods
        /**
        * Returns true if this feed parser can process the given document. 
        *
        * @since 3.0
        * @param aXmlUtils The XmlUtils instance.
        * @param aDocument A document.
        * @param aContentType The content-type.
        * @return true or false.
        */
        static TBool IsFeedSupported(CXmlUtils& aXmlUtils,
                RXmlEngDocument aDocument, const TDesC& aContentType);


    private: // New methods
        /**
        * C++ default constructor.
        */
        CAtomFeedParser(CXmlUtils& aXmlUtils);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * A local ElementHandler function that populates the Item instance with the 
        * values from the given item node.
        *
        * @since 3.1
        * @param aParser The parser calling this method.
        * @param aXmlUtils The xml util instance.
        * @param aNode A node.
        * @param aValueId The normalized value-id.
        * @param aObserver The parser's observer.
        * @return void.
        */
        static void ElementHandlerItemL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
                TXmlEngElement aNode,  TInt aValueId, MFeedParserObserver& aObserver);
        
        /**
        * A local ElementHandler function that handles the link element.
        *
        * @since 3.1
        * @param aParser The parser calling this method.
        * @param aXmlUtils The xml util instance.
        * @param aNode A node.
        * @param aValueId The normalized value-id.
        * @param aObserver The parser's observer.
        * @return void.
        */
        static void ElementHandlerLinkL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
                TXmlEngElement aNode,  TInt aValueId, MFeedParserObserver& aObserver);
    
        /**
        * A local ElementHandler function that determines the timestamp to use.  The
        * point is to track the most relevant timestamp (updated and modified take 
        * precedence over published and issued).
        *
        * @since 3.1
        * @param aParser The parser calling this method.
        * @param aXmlUtils The xml util instance.
        * @param aNode A node.
        * @param aValueId The normalized value-id.
        * @param aObserver The parser's observer.
        * @return void.
        */
        static void ElementHandlerTimestampL(const CFeedParser& aParser, CXmlUtils& aXmlUtils, 
                TXmlEngElement aNode, TInt aValueId, MFeedParserObserver& aObserver);

        /**
        * Determine and set the unique IdStr attribute (unique to the feed that is).
        *
        * @since 3.1
        * @param aItemNode A libxml2 node.
        * @param aObserver The parser's observer.
        * @return void.
        */
        void SetItemIdStrAttributeL(TXmlEngElement aItemNode, MFeedParserObserver& aObserver);


    private:
        TLeakTracker  iLeakTracker;
        
        TXmlEngElement  iFeedTimestampNode;        
        TXmlEngElement  iItemTimestampNode;        
    };
    
#endif      // ATOM_FEED_PARSER_H
            
// End of File
