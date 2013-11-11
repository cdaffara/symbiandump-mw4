/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Application definition.
*
*/



#include "alfapplication.h"
#include "alfuids.h"
#include "alf/alfappserver.h"
#include "alfdocument.h"

#include <eikstart.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// From class CAknApplication.
// Allocates the server instance.
// ---------------------------------------------------------------------------
//
void CAlfApplication::NewAppServerL(CApaAppServer*& aAppServer)
    {
    aAppServer = CAlfAppServer::NewAppServerL();
    aAppServer->SetPriority(EActivePriorityIpcEventsHigh);
    }
    
// ---------------------------------------------------------------------------
// From class CAknApplication.
// Allocates the ducument instance.
// ---------------------------------------------------------------------------
//
CApaDocument* CAlfApplication::CreateDocumentL()
    {
    return new (ELeave) CAlfDocument( *this );
    }
    
// ---------------------------------------------------------------------------
// From class CAknApplication.
// Returns the application UID.
// ---------------------------------------------------------------------------
//
TUid CAlfApplication::AppDllUid() const
    {
    return TUid::Uid(KAlfAppServerInterfaceUid3);
    }

// ======== GLOBAL FUNCTIONS ========


// ---------------------------------------------------------------------------
// NewApplication
// ---------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
	{
	return new CAlfApplication();
	}

// ---------------------------------------------------------------------------
// E32Main
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }
