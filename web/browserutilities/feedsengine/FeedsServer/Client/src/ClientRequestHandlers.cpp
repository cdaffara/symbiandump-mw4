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


#include <FeedsServerClient.h>
#include <s32mem.h>

#include "ClientRequestHandlers.h"

// -----------------------------------------------------------------------------
// CClientRequestHandler::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
CClientRequestHandler* CClientRequestHandler::NewL(RFeedsServer *aFeedServer, TInt aFolderListId  )
    {
    CClientRequestHandler* self = new (ELeave) CClientRequestHandler(aFeedServer, aFolderListId);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CClientRequestHandler::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CClientRequestHandler::ConstructL()
    {   
    }

// -----------------------------------------------------------------------------
// CClientRequestHandler::CClientRequestHandler
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------	
CClientRequestHandler::CClientRequestHandler(RFeedsServer *aFeedServer, TInt aFolderListId):
            iFeedsServerFeed(NULL), iFeedsServerFolderItem(NULL), iFeedsInterface(*aFeedServer, aFolderListId)
    {
    }

// -----------------------------------------------------------------------------
// CClientRequestHandler::~CClientRequestHandler
//
// Deconstructor.
// -----------------------------------------------------------------------------
CClientRequestHandler::~CClientRequestHandler() 
    {
    }

// -----------------------------------------------------------------------------
// CClientRequestHandler::GetFeedInterface
//
// Deconstructor.
// -----------------------------------------------------------------------------
 RFeedsInterface* CClientRequestHandler::GetFeedInterface()
    {
    return &iFeedsInterface;
	}	

// -----------------------------------------------------------------------------
// CClientRequestHandler::GetFeedsServerFeed
//
// Deconstructor.
// -----------------------------------------------------------------------------
RFeed* CClientRequestHandler::GetFeedsServerFeed()
    {
    return iFeedsServerFeed;
    }

// -----------------------------------------------------------------------------
// CClientRequestHandler::SetFeedsServerFeed
//
// Deconstructor.
// -----------------------------------------------------------------------------
void CClientRequestHandler::SetFeedsServerFeed(RFeed* aFeedsServerFeed)
    {
    iFeedsServerFeed = aFeedsServerFeed;
    }

// -----------------------------------------------------------------------------
// CClientRequestHandler::GetFeedsServerFolderItem
//
// Deconstructor.
// -----------------------------------------------------------------------------
RFolderItem* CClientRequestHandler::GetFeedsServerFolderItem()
    {
    return iFeedsServerFolderItem;
    }

// -----------------------------------------------------------------------------
// CClientRequestHandler::SetFeedsServerFolderItem
//
// Deconstructor.
// -----------------------------------------------------------------------------
void CClientRequestHandler::SetFeedsServerFolderItem(RFolderItem* aFeedsServerFolderItem)
    {
    iFeedsServerFolderItem = aFeedsServerFolderItem;
    }