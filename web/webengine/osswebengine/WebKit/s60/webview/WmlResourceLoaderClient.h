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
* Description:  
*
*/

#ifndef WML_RESOURCELOADERLCLIENT_H
#define WML_RESOURCELOADERLCLIENT_H

//  INCLUDES
#include <e32base.h>
#include <w32std.h>
#include "BrCtlDefs.h"

#include "BrCtl.h"
#include "SelfDownloadContentHandler.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
 


class CWmlDispatcher;

namespace WebCore {
class Frame;
class String;
class ResourceError;
class ResourceResponse;  
class WmlResourceLoader;
}//namespace


// CLASS DECLARATION

/**
*
*  @lib resLoader.lib
*  @since 3.0
*/
class WmlResourceLoaderClient
{

public:  // Constructors and destructor        
    static WmlResourceLoaderClient* CreateResourceLoaderClient(CWmlDispatcher* wmldispatcher);
    virtual ~WmlResourceLoaderClient();
    
public:
    void start(TDesC8& url, WebCore::Frame* frame);
    void stop();
    void cancelWithError(const WebCore::ResourceError& error);

    void didReceiveResponse(const WebCore::ResourceResponse&);
    void didReceiveData(const char*, int, long long);
    void didFinishLoading();
    void didFail(const WebCore::ResourceError& error);
    void didCancel(const WebCore::ResourceError& error);

    
private:
    WmlResourceLoaderClient(CWmlDispatcher* wmldispatcher);  
    CWmlDispatcher* m_wmldispatcher;
    WebCore::WmlResourceLoader* m_loader; 
    
    HBufC*     m_contentType;           // owned
    HBufC*     m_charset;               // owned
    HBufC*     m_url;                   // owned
        
    TInt	   m_error;
    TInt       m_contentLength;
    TInt       m_restLength;
    TInt       m_httpStatus; 
    
};

#endif      // WML_STREAMLOADERLCLIENT_H            
// End of File
