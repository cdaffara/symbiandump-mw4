/** @file
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
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


#ifndef UPNPCPBCURRENTHTTPCLIENT_H_
#define UPNPCPBCURRENTHTTPCLIENT_H_

#include <upnpdispatcher.h>

#include "upnpcpbhttpmessagecontroller.h"
#include "upnphttpclientengine.h"

class CUpnpGenaMessage;
class CUpnpServiceInfo;
class CUpnpControlPoint;
class CUpnpHTTPTransaction;

class MUpnpCpHttpRequestor;
class CUpnpCpHttpRequestorImplBase;
class MUpnpCpStackRequestor;
class CUpnpCpStackRequestorImplBase;

class CUpnpFileSender;

NONSHARABLE_CLASS( CUpnpCpbCurrentHttpClient ) : public CBase, public MUpnpHttpClientObserver,
    public MUpnpCpbHttpMessageController
    {
public:
    static const TInt KDefaultSubscriptionTimeout = 300; // 5 minutes

public:

    virtual ~CUpnpCpbCurrentHttpClient();

    static CUpnpCpbCurrentHttpClient* NewL(CUpnpControlPoint& aControlPoint,
            MUpnpCpHttpRequestor* aHttpRequestor, MUpnpCpStackRequestor* aStackRequestor );

    void StartIPFilteringL();

    void StopIPFiltering();

    void SendFileByPostL( CUpnpHttpMessage* aMessage );
    /**
     * It is called from bottom layer when wlan lost event occurs [callback method]     
     * @result None
     */
    void WlanLostOccurs();
    
    /**
     * It is called from bottom layer when address change event occurs [callback method]        
     */
    void AddressChangedL();
    
public:  //from MUpnpCpbHttpMessageController

    virtual void SendL( CUpnpHttpMessage* aMessage );

    virtual void SendActionL(CUpnpAction* aAction);

    virtual void SendSubscribeL(CUpnpServiceInfo* aServiceInfo);

    virtual void SendUnsubscribeL(CUpnpServiceInfo* aServiceInfo);

    virtual void SendResubscribeL(CUpnpServiceInfo* aServiceInfo);

    virtual TInt GetServiceDescriptionL(CUpnpDevice* aDevice, const TDesC8& aServiceUrl);

    virtual TInt GetDeviceDescriptionL(CUpnpDevice* aDevice);

    virtual void SearchL(const TDesC8& aSearchString);

    /**
     * Stops ignoring all embedded devices of specified device
     * @since Series60 3.2
     * @param aRoot Root device
     */
    virtual void StopIgnoringEmbeddedL( CUpnpDevice* aRoot );

    virtual void StopIgnoringUuidL(const TDesC8& aUuids);

    /**
     * Stops ignoring all device tree
     * @since Series60 3.2
     * @param aRoot Root device
     */
    virtual void StopIgnoringL( CUpnpDevice* aDevice );

private://from MUpnpDispatcherCustomer that is derived by MUpnpCpbHttpMessageController

    virtual void MessageReceivedLD(CUpnpHttpMessage* aMessage);

    virtual const TDesC8& Path();

private: //from MUpnpHTTPClientObserver

    /**
     * Callback indicating that response was received within transaction.
     *
     * @param aCompletedTrasaction completed transaction that has response message
     */
    virtual void ClientResponseRecivedLD( CUpnpHttpTransaction& aCompletedTrasaction );

private:
    CUpnpCpbCurrentHttpClient( CUpnpControlPoint& aControlPoint,
        MUpnpCpHttpRequestor* aHttpRequestor, MUpnpCpStackRequestor* aStackRequestor );

    void ConstructL();

    /**
     * Set requestors which leaves with KErrNotReady or do nothing.
     * This is used when WLan Lost occurs.
     * @result None
     */
    void SetNullRequestorsL();

private:

    // control point, not owned
    CUpnpControlPoint& iControlPoint;

    // pointer to http requestor interface
    MUpnpCpHttpRequestor* iHttpRequestor;

    // default implementation of http requestor
    CUpnpCpHttpRequestorImplBase* iHttpRequestorDefaultImpl;

    // iStackRequestor uses DLNA webserver API for http, and dispacher to communicate with stack
    MUpnpCpStackRequestor* iStackRequestor;

    // default implementation of stack requestor
    CUpnpCpStackRequestorImplBase* iStackRequestorDefaultImpl;

    // value of USER-AGENT header in sent SOAP requests
    HBufC8* iUserAgent;

    // Current state of discovery engine
    TInt iResult;

    // The pending message list
    RPointerArray<CUpnpHttpMessage> iPendingHttpMessages;

    // pending transactions array
    RPointerArray<CUpnpHttpTransaction> iPendingTransactions;
    
    CUpnpFileSender* iFileSender;
    
    };

#endif /*UPNPCPBCURRENTHTTPCLIENT_H_*/

//  End of File

