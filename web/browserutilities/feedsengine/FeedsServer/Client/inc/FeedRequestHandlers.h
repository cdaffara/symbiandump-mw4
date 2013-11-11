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
* Description:  Feed related client side request handlings.
*
*/


#ifndef FEED_REQUEST_HANDLER_H
#define FEED_REQUEST_HANDLER_H

// Deprecation warning
#warning This header file has been deprecated. Will be removed in one of the next SDK releases.

// INCLUDES
#include <FeedsServerMsg.h>

#include "FeedsServerRequestHandler.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION




/**
*  Handles the fetching of a feed.
*
*  \b Library: FeedsEngine.lib
*
*  @since CCB (7.1)
*/

class CFeedRequestHandler : public CRequestHandler
	{
    public: // Construct/Destructor
    /**
     * Two-phased constructor.
     */
    static	CFeedRequestHandler* NewL();    
    
    /**
     * Destructor
     */
    virtual ~CFeedRequestHandler();	


private:

    /**
     * C++ default constructor.
     */
    CFeedRequestHandler();
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

public:    
    
    /**
     * Gets the FeedInterface
     */
     RFeedsInterface *GetFeedInterface();
     
     /**
     * Sets the FeedInterface
     */
     void SetFeedInterface(RFeedsInterface* aFeedsInterface);

    /**
     * Get the OperationRequested
     */	
     TBool GetOperationRequested();
     
     /**
     * set the OperationRequested
     */	
     void SetOperationRequested(TBool aOperationRequested);
    
private:
    TBool    iOperationRequested;
    RFeedsInterface *iFeedsInterface;

    };

#endif  // FEED_REQUEST_HANDLER_H
// End of File
