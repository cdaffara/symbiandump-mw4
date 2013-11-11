/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CSynchRequest
*
*/


// INCLUDE FILES
#include <../bidi.h>
#include <e32std.h>
#include <eikenv.h>

#include "config.h"

#include "BrCtl.h"
#include "ResourceHandleInternal.h"
#include "ResourceHandleManagerSymbian.h"
#include "ResourceLoaderDelegate.h"
#include "StaticObjectsContainer.h"

#include "SynchRequest.h"

namespace WebCore
{

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSynchRequest::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CSynchRequest* CSynchRequest::NewL()
{
    CSynchRequest* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

// ----------------------------------------------------------------------------
// CSynchRequest::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CSynchRequest* CSynchRequest::NewLC()
{
    CSynchRequest* self = new ( ELeave ) CSynchRequest();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ----------------------------------------------------------------------------
// CSynchRequest::~CSynchRequest()
// Destructor.
// ----------------------------------------------------------------------------
//
CSynchRequest::~CSynchRequest()
{
    if (IsActive()) {
        Cancel();
    }

    if (iActiveSchedulerWait) {
        delete iActiveSchedulerWait;
        iActiveSchedulerWait = NULL;
    }
}

// ----------------------------------------------------------------------------
// CSynchRequest::CSynchRequest()
// Constructor.
// ----------------------------------------------------------------------------
//
CSynchRequest::CSynchRequest()
    : CActive(CActive::EPriorityStandard)
    , m_synchLoader(0)
    , m_error(String(), KErrNone, String(), String())
{
    CActiveScheduler::Add( this);
}

// ----------------------------------------------------------------------------
// CSynchRequest::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CSynchRequest::ConstructL()
{
    // Initializing the ActiveShedulerWait.
    iActiveSchedulerWait = new (ELeave) CActiveSchedulerWait();
}

// ----------------------------------------------------------------------------
// CSynchRequest::RunL()
// Respond to an event.
// ----------------------------------------------------------------------------
//
void CSynchRequest::RunL()
{
    if (KErrNone == iStatus.Int()) {
        m_error = ResourceError();
    } else {
        m_error = ResourceError(String(), iStatus.Int(), String(), String());
    }
    	
    if (iActiveSchedulerWait->IsStarted()) {
        iActiveSchedulerWait->AsyncStop();
    }
}

// ----------------------------------------------------------------------------
// CSynchRequest::DoCancel()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CSynchRequest::DoCancel()
{
	if (m_synchLoader) {
		m_synchLoader->cancel();
	} else {
        // Complete pending request
        if (KRequestPending == iStatus.Int()) {
        	TRequestStatus* status = &iStatus;
            User::RequestComplete(status, KErrAbort);
        }        
	}
}

// ----------------------------------------------------------------------------
// CSynchRequest::RunError(TInt aError)
// Handles a leave occurring in the request completion event handler RunL().
// ----------------------------------------------------------------------------
//
TInt CSynchRequest::RunError(TInt /*aError*/)
{
    return KErrNone;
}

// ----------------------------------------------------------------------------
//  CSynchRequest::CreateResource()
//  
// ----------------------------------------------------------------------------
//

void CSynchRequest::CreateResource(const ResourceRequest& request,
                                   ResourceError& error,
                                   ResourceResponse& response,
                                   Vector<char>& data,
                                   Frame* frame)
{
    if (IsActive()) {
        Cancel();
    }
    iStatus = KRequestPending;
        
    SetActive();

    // Initialize loader for processing responses
    m_synchLoader =  SynchResourceHandleClient::create(iStatus, request, frame);
    if (!m_synchLoader) {
        if (IsActive()) {
            Cancel();
        }
        error = m_error;
        return;
    }
    
    // Wait for request finished or other failure/cancel events
    iActiveSchedulerWait->Start();

    if (m_error.errorCode() != KErrNone) {
    	// Prefer local override of error (e.g., to signal user cancel)
    	error = m_error;
    }
    else {
    	error = m_synchLoader->resourceError();
    }
    data = m_synchLoader->data();
    response = m_synchLoader->resourceResponse();
    
    m_synchLoader = 0;
}

} // end namespace WebCore

// End of File
