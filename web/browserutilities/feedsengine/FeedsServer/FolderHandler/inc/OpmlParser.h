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
* Description:  An Opml parser.
*
*/


#ifndef OPML_PARSER_H
#define OPML_PARSER_H


// INCLUDES
#include <e32base.h>
#include <xmlengdom.h>

#include "LeakTracker.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CPackedFolder;
class CXmlUtils;
class RXmlEngDocument;
class TXmlEngElement;

// CLASS DECLARATION

/**
*  An Opml parser.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class COpmlParser: public CBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static COpmlParser* NewL(CXmlUtils& aXmlUtils);

        /**
        * Destructor.
        */        
        virtual ~COpmlParser();

    public:  // New Methods
        /**
        * Returns true if this parser can process the given document. 
        *
        * @since 3.0
        * @param aDocument A libxml2 document.
        * @param aContentType The content-type.
        * @return true or false.
        */
        TBool IsSupported(RXmlEngDocument aDocument, const TDesC& aContentType) const;

        /**
        * Parses the given buffer.
        *
        * @since 3.0
        * @param aBuffer The buffer to parse.
        * @param aContentType The buffer's content-type.
        * @param aCharSet The buffer's char-set.
        * @param aImportRootFolder The root folder name
        * @return void.
        */
        CPackedFolder* ParseL(TDesC8& aBuffer, const TDesC& aContentType, 
                const TDesC& aCharSet, const TDesC& aImportRootFolder) const;
                
    private:  // New Methods
        /**
        * C++ default constructor.
        */
        COpmlParser(CXmlUtils& aXmlUtils);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Populates the PackedFolder with the values from the given node. 
        *
        * @since 3.0
        * @param aNode A libxml2 node.
        * @param aPackedFolder The packed folder.
        * @return void.
        */
        void ProcessFolderL(TXmlEngElement aNode, CPackedFolder& aPackedFolder) const;

        /**
        * Search for the feed URL from a set of feed URLs present in the current folder seperated by commas
        *
        * @since 3.2
        * @param aURLsInCurFolder Feed URLs in the current folder
        * @param aFeedURL Feed URL to be serached
        * @param aFound Will be set to ETrue if found otherwise EFalse
        * @return void.
        */
        void SearchURLInCurrentFolderL(HBufC* aURLsInCurFolder, HBufC* aFeedURL, TBool& aFound) const;

        
    private:  // Data
        // WARNING!!! This class must be "Thread (ActiveObject) safe".  Meaning
        //            that it must not contain any member variables that can't
        //            be safely shared between all "threads" using it.

        TLeakTracker  iLeakTracker;

		CXmlUtils&  iXmlUtils;
    };
    
#endif      // OPML_PARSER_H
            
// End of File
