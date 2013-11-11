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


#ifndef FEED_HANDLER_H
#define FEED_HANDLER_H


// INCLUDES
#include <e32base.h>

#include "FeedParser.h"
#include "LeakTracker.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CXmlUtils;

/**
*  The top-level handler of Feeds.  It takes a buffer and uses an
*  appropriate feeds parser to create a Feed instance.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*  @attention WARNING!!! This class must be "Thread (ActiveObject) safe".  Meaning
*            that it must not contain any member variables that can't
*            be safely shared between all "threads" using it.
*/
class CFeedHandler: public CBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CFeedHandler* NewL(CXmlUtils& aXmlUtils);
        
        /**
        * Destructor.
        */        
        virtual ~CFeedHandler();
        
        
    public:  // New methods
        /**
        * Creates a Feed instance from the buffer.  This method
        * may modify aBuffer.
        *
        * @since 3.0
        * @param aBuffer A utf8 buffer.  -- TODO: pass be ref
        * @param aContentType The content-type.
        * @param aCharSet The char-encoding.
        * @return A Feed instance.
        */
        void ParseL(TDesC8& aBuffer, const TDesC& aContentType, 
                const TDesC& aCharSet, MFeedParserObserver& aObserver);
        
        
    private:
        /**
        * C++ default constructor.
        */
        CFeedHandler(CXmlUtils& aXmlUtils);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:
        
        TLeakTracker  iLeakTracker;
        CXmlUtils&    iXmlUtils;
    };

#endif      // FEED_HANDLER_H
            
// End of File