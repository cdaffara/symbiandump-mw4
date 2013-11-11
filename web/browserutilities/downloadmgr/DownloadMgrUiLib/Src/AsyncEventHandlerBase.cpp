/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for event handlers
*
*/



// INCLUDE FILES
#include    "AsyncEventHandlerBase.h"
#include    "CDownloadMgrUiLibRegistry.h"
#include    "AsyncEventHandlerArray.h"
#include    "UiLibLogger.h"
#include    "DMgrUiLibPanic.h"
#include    "CUserInteractionsUtils.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAsyncEventHandlerBase::CAsyncEventHandlerBase
// -----------------------------------------------------------------------------
//
CAsyncEventHandlerBase::CAsyncEventHandlerBase
    ( RHttpDownload& aDownload, THttpDownloadEvent aEvent, 
      MDownloadMgrUiLibRegModel& aRegistryModel, 
      CAsyncEventHandlerArray& aOwner )
:       CActive( KUiLibAOPriority ),
    iDownload( aDownload ),
    iEvent( aEvent ),
    iRegistryModel( aRegistryModel ),
    iOwner( aOwner )
    {
    CActiveScheduler::Add( this );
    // Invoke RunL
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

// Destructor
CAsyncEventHandlerBase::~CAsyncEventHandlerBase()
    {
    CLOG_ENTERFN("CAsyncEventHandlerBase::~CAsyncEventHandlerBase");
    Cancel();
    iOwner.Remove( this );
    CLOG_LEAVEFN("CAsyncEventHandlerBase::~CAsyncEventHandlerBase");
    }

// -----------------------------------------------------------------------------
// CAsyncEventHandlerBase::Download
// -----------------------------------------------------------------------------
//
RHttpDownload& CAsyncEventHandlerBase::Download()
    {
    return iDownload;
    }

// -----------------------------------------------------------------------------
// CAsyncEventHandlerBase::Download
// -----------------------------------------------------------------------------
//
const RHttpDownload& CAsyncEventHandlerBase::Download() const
    {
    return iDownload;
    }

// End of file.
