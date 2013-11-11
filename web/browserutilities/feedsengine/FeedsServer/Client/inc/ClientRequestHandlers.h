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
* Description:  FeedsServer related client side request handlings.
*
*/


#ifndef CLIENT_REQUEST_HANDLER_H
#define CLIENT_REQUEST_HANDLER_H

// Deprecation warning
#warning This header file has been deprecated. Will be removed in one of the next SDK releases.

// INCLUDES
#include <FeedsServerMsg.h>
#include <FeedsServerRequestHandler.h>


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  FeedsServer related client side request handlings.
*
*  \b Library: FeedsEngine.lib
*
*  @since CCB (7.1)
*/
class CClientRequestHandler : public CRequestHandler
	{
public:
    /**
     * Two-phased constructor.
     */
    static CClientRequestHandler* NewL(RFeedsServer *aFeedServer, TInt aFolderListId  = 0);    
    
    /**
     * Destructor
     */
    virtual ~CClientRequestHandler();


private:
    /**
     * C++ default constructor.
     */
    CClientRequestHandler(RFeedsServer *aFeedServer, TInt aFolderListId);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

public:
    
    /**
     * Gets the FeedInterface
     */
     RFeedsInterface* GetFeedInterface();

    /**
     * Gets FeedsServerFeed
     */
     RFeed* GetFeedsServerFeed(); 
     
    /**
     * Sets FeedsServerFeed
     */
    void SetFeedsServerFeed(RFeed* aFeedsServerFeed); 
    
    /**
     * Gets FeedsServerFolderItem
     */
     RFolderItem* GetFeedsServerFolderItem();
     
    /**
     * Sets FeedsServerFolderItem
     */
     void SetFeedsServerFolderItem(RFolderItem* aFeedsServerFolderItem);

private:
    RFeed* iFeedsServerFeed;
    RFolderItem* iFeedsServerFolderItem;
    RFeedsInterface iFeedsInterface;		 		 	
    };

#endif  // CLIENT_REQUEST_HANDLER_H
// End of File
