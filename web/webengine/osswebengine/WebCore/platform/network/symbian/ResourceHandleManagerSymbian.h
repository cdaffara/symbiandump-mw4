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
* Description:  CResourceHandleManager
*
*/

#ifndef ResourceHandleManagerSymbian_H
#define ResourceHandleManagerSymbian_H

#include "UrlConnection.h"

class CWebCoreKeyValuePair;
class MUrlConnection;

namespace WebCore {

void cleanupHandleManager();

struct ResourceRequest;
class ResourceResponse;

/**
 * @class ResourceHandleManager
 * @short Download Controller: handle ResourceHandles using a pool of threads
 */
class CResourceHandleManager : public CBase
{
public:
    static CResourceHandleManager* self();

    int add(ResourceHandle*);
    void cancel(ResourceHandle*);
    virtual ~CResourceHandleManager();
public :
    void receivedResponse(ResourceHandle* resource, ResourceResponse& response, MUrlConnection* connection);
    void receivedData(ResourceHandle* resource, const TDesC8&, int contentLength, MUrlConnection* connection);
    void receivedFinished(ResourceHandle* resource, TInt error, MUrlConnection* connection);

private:
    CResourceHandleManager();
};


}

#endif
