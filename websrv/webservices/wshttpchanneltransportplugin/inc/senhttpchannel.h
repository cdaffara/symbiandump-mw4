/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header description       
*
*/











#ifndef SEN_HTTP_CHANNEL_H
#define SEN_HTTP_CHANNEL_H

//  INCLUDES
#include <e32base.h> // for CBase

#include "senresponseobserver.h"
#include "MSenElement.h"

//FORWARD DECLARATION
class MSenIdentityManager;
class CSenSoapEnvelope2;
class CSenAtomEntry;
class CSenLayeredHttpTransportProperties;


// CLASS DECLARATION

/**
 * The class for submitting multiple simultaneous HTTP requests.
 * Accept header and User-Agent header have to be set before
 * the class is used. You should call the set methods only after
 * construction, not after any requests are submitted.
 *
 */

class CSenHttpChannel : public CBase
    {
public:
    enum TPanics
        {
        ENoAcceptHeader = 1,
        ENoUserAgentHeader,
        EAddedHeadersCount,
        ETxnStateNull,
        ERequestUriNull,
        EPostContentTypeNull,
        EPostContentNull,
        ERespBodyNull,
		EContentTypeUnrecognisedValueType
        };

public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSenHttpChannel* NewL(MSenIdentityManager& aManager);

    /**
     * Two-phased constructor.
     */
    static CSenHttpChannel* NewLC(MSenIdentityManager& aManager);

    static CSenHttpChannel* NewL(MSenIdentityManager& aManager,
                                            TUint32 aIapId);

    static CSenHttpChannel* NewLC(MSenIdentityManager& aManager,
                                            TUint32 aIapId);
    // New functions

    /**
     * Submit HTTP POST request to a given URI with the given
     * content type and the message body. Observer is called
     * when the response is ready. The method leaves if an
     * error occurs.
     * @param aObserver observer is called when the response
     * @param aUri outgoing request URI.
     * @param aContent outgoing request content.
     * @param aProps outgoing transport properties for the
     * submitted message.
     * @return submit id which identifies the request.
     */
    virtual TInt SendL(MSenResponseObserver& aObserver,
                       const TDesC8& aUri,
                       const TDesC8& aContent,
                       CSenLayeredHttpTransportProperties& aProps) = 0;
                       
                       
    virtual TInt SendL(MSenResponseObserver& aObserver,
                       const TDesC8& aUri,
                       CSenSoapEnvelope2& aContent,
                       CSenLayeredHttpTransportProperties& aProps) = 0;
    virtual TInt SendL(MSenResponseObserver& aObserver,
                       const TDesC8& aUri,
                       CSenAtomEntry& aContent,
                       CSenLayeredHttpTransportProperties& aProps) = 0;
                       
	virtual TInt SendL(MSenResponseObserver& aObserver,
						const TDesC8& aUri,
						const RFile& aFile,
						CSenLayeredHttpTransportProperties& aProps) = 0;                       
    
    /**
     * Set web proxy to be used when sending requests.
     * If you try to connect with the phone emulator to the Internet
     * from Nokia Intranet you need to set this. Internally, this
     * method will call the version, which takes in the complete
     * proxy address:
     * @see SetProxyL(const TDesC8& aProxyAddrStr)
     *
     * @param aProxyHostBaseAddr, e.g. 172.19.160.50
     * @param aProxyPort, e.g. 8080. Note that, if a negative value
     * is provided, then the HTTP port is not used in forming of
     * the host address. In such case, the default proxy port
     * value of HTTP stack implemtation will be used.
     */
    virtual void SetProxyL(const TDesC8& aProxyHostBaseAddr,
                           TInt aProxyPort) = 0;
    /**
     * Set web proxy to be used when sending requests.
     * If you try to connect with the phone emulator to the Internet
     * from Nokia Intranet you need to set this.
     *
     * @param aProxyAddrStr proxy address, e.g. 131.228.55.140:8080.
     */
    virtual void SetProxyL(const TDesC8& aProxyAddrStr) = 0;

    /**
     * By default HTTP v. 1.1 is used. If you want to set
     * set HTTP 1.0 call SetHttpVersionL(0).
     *
     * @param aVersion HTTP version, give x for HTTP 1.x.
     */
    virtual void SetHttpVersionL(TInt aVersion) = 0;

    virtual void SetProxyUsageL(TBool aProxyUsage) = 0;
    virtual void SetSecureDialogL(TBool aSecureDialog) = 0;
    virtual TBool EffectiveIapId(TUint32 &aIapId) = 0;
    virtual TInt CancelTransaction(const TInt aTxnId) = 0;
    virtual void CancelAllTransactions() = 0;
    virtual void EnableTimeOutL(TInt aTxnId, TInt aTimeOutSec) = 0;
    virtual void DisableTimeOutL(TInt aTxnId) = 0;
    virtual TInt32 UsedIap() = 0;
    virtual void ResetIapId() = 0;
    virtual void ResetUsedIapId() = 0;
    virtual void SetExplicitIapDefined(TBool aExplicitIapDefined) = 0;
    };

#endif // SEN_HTTP_CHANNEL_H

// End of File
