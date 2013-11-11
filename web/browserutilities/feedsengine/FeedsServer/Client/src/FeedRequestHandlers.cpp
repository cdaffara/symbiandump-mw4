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


#include <FeedsServerFeed.h>
#include <s32mem.h>
#include <es_sock.h>

#include "FeedRequestHandlers.h"

// -----------------------------------------------------------------------------
// CFeedRequestHandler::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
CFeedRequestHandler* CFeedRequestHandler::NewL()
    {
    CFeedRequestHandler* self = new (ELeave) CFeedRequestHandler();
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CFeedRequestHandler::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFeedRequestHandler::ConstructL()
    {   
    }

// -----------------------------------------------------------------------------
// CFeedRequestHandler::CFeedRequestHandler
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------	
CFeedRequestHandler::CFeedRequestHandler():iOperationRequested(EFalse), iFeedsInterface(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CFeedRequestHandler::~CFeedRequestHandler
//
// Deconstructor.
// -----------------------------------------------------------------------------
CFeedRequestHandler::~CFeedRequestHandler()
    {
    iFeedsInterface = NULL;	
    }

// -----------------------------------------------------------------------------
// CFeedRequestHandler::GetFeedInterface
//
// Deconstructor.
// -----------------------------------------------------------------------------
 RFeedsInterface* CFeedRequestHandler::GetFeedInterface()
    {
    return iFeedsInterface;
    }

// -----------------------------------------------------------------------------
// CFeedRequestHandler::SetFeedInterface
//
// Deconstructor.
// -----------------------------------------------------------------------------
 void CFeedRequestHandler::SetFeedInterface(RFeedsInterface*  aFeedsInterface)
    {
    iFeedsInterface = aFeedsInterface;
    }
	
// -----------------------------------------------------------------------------
// CFeedRequestHandler::GetOperationRequested
//
// Deconstructor.
// -----------------------------------------------------------------------------
 TBool CFeedRequestHandler::GetOperationRequested()
    {
    return iOperationRequested;
    }

// -----------------------------------------------------------------------------
// CFeedRequestHandler::SetOperationRequested
//
// Deconstructor.
// -----------------------------------------------------------------------------
 void CFeedRequestHandler::SetOperationRequested(TInt aOperationRequested)
    {
    iOperationRequested = aOperationRequested;
    }