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
* Description:  FolderItem related client side request handlings.
*
*/


#ifndef FOLDER_ITEM_REQUEST_HANDLER_H
#define FOLDER_ITEM_REQUEST_HANDLER_H

// Deprecation warning
#warning This header file has been deprecated. Will be removed in one of the next SDK releases.

// INCLUDES
#include <f32file.h>
#include <FeedsServerMsg.h>
#include "FeedsEntity.h"
#include "FeedsServerRequestHandler.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Handles async folder item requests.
*
*  \b Library: FeedsEngine.lib
*
*  @since CCB (7.1)
*/

class CFolderItemRequestHandler : public CRequestHandler
	{
	
public:
    /**
     * Two-phased constructor.
     */
    static CFolderItemRequestHandler* NewL();
   
private:

     /**
     * C++ default constructor.
     */
    CFolderItemRequestHandler();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

public:	
    /**
     * Destructor
     */	
    virtual ~CFolderItemRequestHandler();

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
    TBool      iOperationRequested;
    RFeedsInterface *iFeedsInterface;

    };	

#endif  // FOLDER_ITEM_REQUEST_HANDLER_H
// End of File
