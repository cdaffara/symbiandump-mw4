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
* Description:  Definition of CSynchRequest 
*
*/


#ifndef CSYNCHREQUEST_H
#define CSYNCHREQUEST_H

//  INCLUDES
#include <e32base.h>

#include "Frame.h"
#include "ResourceHandle.h"
#include "ResourceRequest.h"
#include "ResourceResponse.h"

#include "SynchLoader.h"

namespace WebCore
{

class ResourceError;

/**
 * @class CSynchRequest
 * @short AO to process synchronous messages
 * @since 5.0
 */
class CSynchRequest : public CActive
{
public:
    // Constructors and destructor

    /**
     * Create new CSynchRequest object.
     * @return a pointer to the created instance of CSynchRequest
     */
    static CSynchRequest* NewL();

    /**
     * Create new CSynchRequest object.
     * @return a pointer to the created instance of CSynchRequest
     */
    static CSynchRequest* NewLC();

    /**
     * Destroy the object and release all memory of the objects.
     */
    virtual ~CSynchRequest();

private:
    // Default constructors

    /**
     * Default constructor
     */
    CSynchRequest();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

protected:
    // From CActive

    /**
     * Handles an active object's request completion event.
     */
    void DoCancel();

    /**
     * Implements cancellation of an outstanding request.
     */
    void RunL();

    /**
     * Handles a leave occurring in the request completion event handler RunL().
     * @param aError The default implementation returned aError.
     * @return The error codes.
     */
    TInt RunError(TInt);

public:
    // New functions

    void CreateResource(const ResourceRequest&,
                        ResourceError&,
                        ResourceResponse&,
                        Vector<char>&,
                        Frame*);

private:
    // New functions


private:
    // Member data

    /**
     * Controls a single scheduling loop in the current active scheduler
     * Owned by CSynchRequest object.
     */
    CActiveSchedulerWait* iActiveSchedulerWait;
    RefPtr<SynchResourceHandleClient> m_synchLoader;
    ResourceError m_error;

};

} // end namespace WebCore

#endif      // CSYNCHREQUEST_H
// End of File
