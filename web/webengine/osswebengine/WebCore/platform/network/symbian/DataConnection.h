/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/

#ifndef __DATACONNECTION_H__
#define __DATACONNECTION_H__

#include <e32base.h>
#include "UrlConnection.h"


class DataConnection : public MUrlConnection
{
public:
    DataConnection(WebCore::ResourceHandle* _handle);
    virtual ~DataConnection();
    virtual int submit();
    virtual void cancel();
    /**
    */
    virtual void download(WebCore::ResourceHandle* handle, const WebCore::ResourceRequest& request, const WebCore::ResourceResponse& response);
    virtual int totalContentSize() { return m_maxSize; }
    static TInt sendResponseCb(TAny* aPtr);

private:
    void submitL();
    void parseUrlLC(HBufC8*& contentType, HBufC8*& encoding, HBufC8*& body);
    void sendResponseL();
private:
    CPeriodic* m_scheduler;
    int m_maxSize;
};

#endif // __DATACONNECTION_H__
// end of file
