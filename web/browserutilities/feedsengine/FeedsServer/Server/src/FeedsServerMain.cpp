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
* Description:  The FeedsServer main server class.
*
*/


#include <e32std.h>

#include "FeedsServer.h"
#include "LeakTracker.h"

// -----------------------------------------------------------------------------
// E32Main
//
// Main entry point.
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
	
	CTrapCleanup*  cleanup = CTrapCleanup::New();
	TInt           r = KErrNoMemory;

	if (cleanup != NULL)
		{
#ifdef TRACK_LEAKS
        // Create and use the LeakTracker instance.
        TRAP_IGNORE(gLeakTracker = CLeakTracker::NewL(_L("FeedsServer")));
#endif
		
		// Run the server.
		TRAP(r, CFeedsServer::RunServerL());

#ifdef TRACK_LEAKS
        delete gLeakTracker;
        gLeakTracker = NULL;
#endif
		delete cleanup;
		}
	
	return r;
    }
