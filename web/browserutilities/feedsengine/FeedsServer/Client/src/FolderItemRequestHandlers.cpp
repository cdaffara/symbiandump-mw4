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


#include <e32base.h>
#include <FeedsServerFolderItem.h>
#include <s32mem.h>
#include <es_sock.h>

#include "FolderItemRequestHandlers.h"

// -----------------------------------------------------------------------------
// CFolderItemRequestHandler::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
CFolderItemRequestHandler* CFolderItemRequestHandler::NewL()
    {
    CFolderItemRequestHandler* self = new (ELeave) CFolderItemRequestHandler();
    self->ConstructL();	  
    return self;
    }

// -----------------------------------------------------------------------------
// CFolderItemRequestHandler::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFolderItemRequestHandler::ConstructL()
    {   
    }

// -----------------------------------------------------------------------------
// CFolderItemRequestHandler::CFolderItemRequestHandler
//
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CFolderItemRequestHandler::CFolderItemRequestHandler():iOperationRequested(EFalse), iFeedsInterface(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CFolderItemRequestHandler::~CFolderItemRequestHandler
//
// Deconstructor.
// -----------------------------------------------------------------------------
CFolderItemRequestHandler::~CFolderItemRequestHandler()
    {
    iFeedsInterface = NULL;		
    }
    
// -----------------------------------------------------------------------------
// CFolderItemRequestHandler::GetFeedInterface
//
// Deconstructor.
// -----------------------------------------------------------------------------
 RFeedsInterface* CFolderItemRequestHandler::GetFeedInterface()
    {
    return iFeedsInterface;
    }	

// -----------------------------------------------------------------------------
// CFolderItemRequestHandler::SetFeedInterface
//
// Deconstructor.
// -----------------------------------------------------------------------------
 void CFolderItemRequestHandler::SetFeedInterface(RFeedsInterface*  aFeedsInterface)
    {
    iFeedsInterface = aFeedsInterface;
    }

// -----------------------------------------------------------------------------
// CFolderItemRequestHandler::GetOperationRequested
//
// Deconstructor.
// -----------------------------------------------------------------------------
 TBool CFolderItemRequestHandler::GetOperationRequested()
    {
    return iOperationRequested;
    }

// -----------------------------------------------------------------------------
// CFolderItemRequestHandler::SetOperationRequested
//
// Deconstructor.
// -----------------------------------------------------------------------------
 void CFolderItemRequestHandler::SetOperationRequested(TInt aOperationRequested)
    {
    iOperationRequested = aOperationRequested;
    }