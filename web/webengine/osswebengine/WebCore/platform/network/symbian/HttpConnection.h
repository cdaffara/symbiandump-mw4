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

#ifndef __HTTPCONNECTION_H__
#define __HTTPCONNECTION_H__

#include <e32base.h>
#include <http/rhttptransaction.h>
#include "UrlConnection.h"

class CHttpCacheSupply;
class HttpPostDataSupplier;
class MultipartContentHandler;
class UnknownContentHandler;

namespace WebCore {
    class ResourceHandle;
    class Frame;
    class ResourceResponse;
};

typedef void (*DefersDataCallback)(void* ctx);

class DefersData: public CActive {
    public:
        DefersData(void* ctx, DefersDataCallback callback);
        ~DefersData();
        void DoCancel() {}
        void RunL();
        TInt RunError(TInt aError);
        void Activate();
        
        WebCore::ResourceResponse* m_response;
        Vector<HBufC8*> m_bodyParts;
        int m_error;
        bool m_done;
        void* m_ctx;
        DefersDataCallback m_callback;
};


typedef void (*ReceivedFinishedCallback)(void* ctx, TInt error);

class ReceivedFinished: public CActive {
    public:
        ReceivedFinished(void* ctx, ReceivedFinishedCallback callback);
        ~ReceivedFinished();
        void DoCancel() {}
        void RunL();
        TInt RunError(TInt aError);
        void Activate(TInt errorCode);
        
        bool isDone() { return m_done; }
        void done(bool);
        
        TInt m_error;
        bool m_done;
        void* m_ctx;
        ReceivedFinishedCallback m_callback;
};


class HttpConnection : public MUrlConnection
{
public:
    /**
    */
    HttpConnection(WebCore::ResourceHandle* _handle, WebCore::Frame* _frame);
    /**
    */
    virtual ~HttpConnection();
    /**
    */
    static HttpConnection* connectionFromTransaction(RHTTPTransaction& transaction);
    /**
    */
    virtual int submit();
    /**
    called by WebCore side
    */
    virtual void cancel();
    /**
    */
    virtual void download(WebCore::ResourceHandle* handle, const WebCore::ResourceRequest& request, const WebCore::ResourceResponse& response);

    virtual int totalContentSize() { return m_accumulatedSize; }

    virtual void setDefersLoading(bool defers);

    virtual void handleDownloadCommand(unsigned int transId, TBrCtlDefs::TBrCtlDownloadCmd command) {}
    /**
    called by HTTP side
    */
    void handleError(int error);
    /**
    */
    void MHFRunL(const THTTPEvent &aEvent);
    /**
    */
    RHTTPTransaction* HttpTransaction() { return m_transaction; }

    RHTTPTransaction* takeOwnershipHttpTransaction();
    /**
    */
    WebCore::Frame* frame() { return m_frame; }

    /**
    * Set authentication properties on this transaction
    * @since 3.X
    * @param aUsername user name
    * @param aPassword password
    * @param aProxy ETrue if this is a proxy authentication
    * @return void
    */
    void AddAuthenticationPropertiesL( TPtr& aUsername, TPtr& aPassword,  bool aProxy );

    /**
    * Called when the authentication dialog box is closed.
    * @since 3.X
    * @param aUsername The username entered in the authentication dialog
    * @param aPassword The password entered in the authentication dialog
    * @param aProxy A flag if authentication was requested by server or proxy.
    * @param aError The status code as a result of this dialog box.
    * Can be KErrNone, KErrCancel or KErrNoMemory.
    * @return void
    */
    void AuthenticationResponse( TPtr& aUsername, TPtr& aPassword, bool aProxy, int aError );

    /**
    */
    int CheckForSecurityStatusChange();

    /**
    */
    int CheckForNonHttpRedirect();

    /**  
     * Add IMEI notifier flag to the transaction  
     * Might not be needed as IMEI Notification check is done in   
     * UA-Prof Filter  
     * @since 3.X  
     * @return void  
     */  
     // void addIMEINotifyPropertiesL();  
    static void processDefersData(void* ctx);
    bool isDefered() { return (m_defersData != NULL); }

    /**
     * 
     */
    void activateReceivedFinished(TInt errorCode);
    static void processReceivedFinished(void* ctx, TInt errorCode);

private:
    /**
    */
    void submitL();
    /**
    */
    void addRequestHeadersL();
    /**
    */
    int HandleSpecialEvent(int event);
    /**
    */
    void complete(int error);

    /**
    * Handle authentication requests.
    * @since 3.X
    * @param HTTP authentication Status
    * @return bool
    */
    int handleAuthRequestL( int status );

    /**
    * Send authentication request to the browser client
    * @since 3.X
    * @param aTrans 
    * @param aUsernameVal User name
    * @param aRealmVal Realm
    * @param aIsProxy ETrue if this is a proxy authentication request
    * @param aStale ETrue if stale
    * @param aPasswordVal Password
    * @return void
    */
    void SendAuthRequestL( THTTPHdrVal& aUsernameVal, THTTPHdrVal& aRealmVal, 
            bool aIsProxy, bool aStale, THTTPHdrVal& aPasswordVal );

    /**
    * Check if the url is in the cache
    * @since 3.X
    * @param aUrl Url to check
    * @return ETrue if url is in the cache
    */
    bool IsUrlInCacheL( const TDesC8& aUrl );

private: // data
    bool m_isDone;
    HBufC8* m_frag; // owned
    RHTTPTransaction* m_transaction; // owned
    HBufC8* m_urlResponse; // owned
    HBufC8* m_contentType; // owned
    HBufC8* m_encoding; // owned
    CHttpCacheSupply* m_cacheSupply; // owned
    HttpPostDataSupplier* m_postDataSupplier; // owned
    int m_maxSize;
    WebCore::Frame* m_frame; // not owned
    bool m_IsMultipart;
    MultipartContentHandler* m_MultipartContentHandler;         // owned
    // The certificate information
    TCertInfo* m_certInfo;
    int m_accumulatedSize;
    DefersData* m_defersData;
    ReceivedFinished* m_receivedFinished;
    UnknownContentHandler* m_unknownContentHandler;
	TBool m_isInCache;
};


#endif // __HTTPCONNECTION_H__
// end of file
