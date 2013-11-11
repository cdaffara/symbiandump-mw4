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
* Description:  Feeds application instance.
*
*/


#include "FeedsApplication.h"
#include "FeedsDocument.h"
#include "FeedsViews.h"
#include "LeakTracker.h"


// -----------------------------------------------------------------------------
// CFeedsApplication::~CFeedsApplication
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CFeedsApplication::CFeedsApplication()
    {
    // Create the LeakTracker instance.
#ifdef TRACK_LEAKS
    TRAP_IGNORE(gLeakTracker = CLeakTracker::NewL(_L("Test App")));
#endif
    }

// -----------------------------------------------------------------------------
// CFeedsApplication::~CFeedsApplication
//
// Deconstructor.
// -----------------------------------------------------------------------------
//
CFeedsApplication::~CFeedsApplication()
    {
#ifdef TRACK_LEAKS
        delete gLeakTracker;
        gLeakTracker = NULL;
#endif
    }

// -----------------------------------------------------------------------------
// CFeedsApplication::CreateDocumentL()
// 
// Create an Feeds document, and return a pointer to it
// -----------------------------------------------------------------------------
//
CApaDocument* CFeedsApplication::CreateDocumentL()
    {
    return( static_cast< CApaDocument* >( CFeedsDocument::NewL( *this ) ) );
    }

// -----------------------------------------------------------------------------
// CFeedsApplication::AppDllUid()
//
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CFeedsApplication::AppDllUid() const
    {
    return TUid::Uid(EFeedsAppUid);
    }
