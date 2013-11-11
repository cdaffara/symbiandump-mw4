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
* Description:  Array of event handlers
*
*/



// INCLUDE FILES
#include    "AsyncEventHandlerArray.h"
#include    "UiLibLogger.h"
#include    "DMgrUiLibPanic.h"

// CONSTANTS
const TInt32 KArrayGranularity = 6; // Let it as many THttpDownloadState is

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAsyncEventHandlerArray::CAsyncEventHandlerArray
// -----------------------------------------------------------------------------
//
CAsyncEventHandlerArray::CAsyncEventHandlerArray()
:   CArrayPtrFlat<CAsyncEventHandlerBase>( KArrayGranularity ),
    iDestructing(0)
    {
    }

// Destructor
CAsyncEventHandlerArray::~CAsyncEventHandlerArray()
    {
    CLOG_ENTERFN("CAsyncEventHandlerArray::~CAsyncEventHandlerArray");
    // Delete all owned active object.
    iDestructing = ETrue;
    ResetAndDestroy();
    CLOG_LEAVEFN("CAsyncEventHandlerArray::~CAsyncEventHandlerArray");
    }

// -----------------------------------------------------------------------------
// CAsyncEventHandlerArray::Remove
// -----------------------------------------------------------------------------
//
void CAsyncEventHandlerArray::Remove( CAsyncEventHandlerBase* aEventHandler )
    {
    CLOG_ENTERFN("CAsyncEventHandlerArray::Remove");

    if ( iDestructing )
        {
        // Do nothing. ResetAndDestroy() removes the elements.
        CLOG_WRITE(" Destructing");
        }
    else
        {
        TKeyArrayFix key(0,ECmpTInt32);
        TInt index(0);
        TInt err = Find( aEventHandler, key, index );
        CLOG_WRITE_FORMAT(" err: %d",err);
        if ( !err )
            {
            CLOG_WRITE_FORMAT(" index: %d",index);
            CArrayPtrFlat<CAsyncEventHandlerBase>::Delete( index );
            }
        }

    CLOG_LEAVEFN("CAsyncEventHandlerArray::Remove");
    }

// End of file.
