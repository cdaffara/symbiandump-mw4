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
* Description:  Definition of SynchLoaderClient 
*
*/


#ifndef SYNCHLOADERCLIENT_H_
#define SYNCHLOADERCLIENT_H_

#include <e32base.h>

#include "Frame.h"
#include "ResourceHandle.h"
#include "ResourceHandleClient.h"
#include "ResourceLoader.h"
#include "SubresourceLoaderClient.h"
#include <wtf/PassRefPtr.h>

namespace WebCore {

class SynchResourceHandleClient;

class SynchLoader : public ResourceLoader {
    
public:
    static PassRefPtr<SynchLoader> create(Frame*, const ResourceRequest&, SynchResourceHandleClient*);
    virtual ~SynchLoader();

    virtual bool load(const ResourceRequest&);

    bool isDone() const { return m_isdone; }

    virtual void didReceiveResponse(const ResourceResponse&);
    virtual void didReceiveData(const char*, int, long long lengthReceived, bool allAtOnce);
    virtual void didFinishLoading();
    virtual void didFail(const ResourceError&);
    virtual void didCancel(const ResourceError&);

    void removeSubresourceLoader();

private:
    SynchLoader(Frame*, SynchResourceHandleClient*, bool sendResourceLoadCallbacks, bool shouldContentSniff);
    
private:
   SynchResourceHandleClient* m_client;
    bool m_isdone;
}; // end class SynchLoader


class SynchResourceHandleClient : public Shared<SynchResourceHandleClient>, public ResourceHandleClient {

public:
    static PassRefPtr<SynchResourceHandleClient> create(TRequestStatus&, const ResourceRequest&, Frame*);
    ~SynchResourceHandleClient();

    virtual void didReceiveResponse(ResourceHandle*, const ResourceResponse&);
    virtual void didReceiveData(ResourceHandle*, const char*, int, int lengthReceived);
    virtual void didFinishLoading(ResourceHandle*);
    virtual void didFail(ResourceHandle*, const ResourceError&);

    void cancel();
    void cancel(const ResourceError&);
    ResourceResponse resourceResponse() const { return m_response; }
    ResourceError resourceError() const { return m_error; }
    Vector<char> data() const { return m_data; }
    ResourceHandle* resourceHandle() const { return m_resourceHandle.get(); }

private:
    SynchResourceHandleClient(TRequestStatus&, const ResourceRequest&, Frame*);
    
    RefPtr<SynchLoader> loader() { return m_loader; }
    
    void finish(const ResourceError&);
    
private:
    bool m_finished;
    ResourceResponse m_response;
    ResourceError m_error;
    Vector<char> m_data;
    RefPtr<SynchLoader> m_loader;
    RefPtr<ResourceHandle> m_resourceHandle;
    TRequestStatus* m_status;
        
}; // end class SynchResourceHandleClient

} // End namespace WebCore

#endif /*SYNCHLOADERCLIENT_H_*/
