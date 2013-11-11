/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header declaration
*
*/











#ifndef WSOvi_SESSION_CONSUMER_H
#define WSOvi_SESSION_CONSUMER_H

// INCLUDES
#include <e32base.h>
#include "msenremoteserviceconsumer.h"
#include "senpointermap.h"
#include "SenSoapMessage.h"
#include "wsovimessagecontext.h"



// DATA TYPES
typedef RSenPointerMap<TDesC8, HBufC8> RSentMessagesPointerMap;


// FORWARD DECLARATIONS
class RFileLogger;
class CSenHttpTransportProperties;


// CLASS DECLARATION
/*
    Its wrapper for remote consumer. In the same time it inherits from remote consumer. Why?
    Because we can not cast inside framework pluging to our WSOvi session a remote consumer, 
    which was created in core
    
    Object can ot be extended to more complex class. Can be cast to narrow class, but not in other way
*/
class CWSOviSessionConsumer : public CBase, public MSenRemoteServiceConsumer
    {
public: // Constructors and destructor
    
    static CWSOviSessionConsumer* NewL(MSenRemoteServiceConsumer& aServiceConsumer);

    static CWSOviSessionConsumer* NewLC(MSenRemoteServiceConsumer& aServiceConsumer);

    virtual ~CWSOviSessionConsumer();
 // From MSenRemoteServiceConsumer:
    TInt HandleMessageL(HBufC8* aMessage,
                        const TInt aTxnId,
                        MSenProperties* aResponseTransportProperties);

    TInt HandleErrorL(HBufC8* aError,
                      const TInt aErrorCode,
                      const TInt aTxnId,
                      MSenProperties* aResponseTransportProperties);

    void SetStatusL(const TInt status);



    TInt MessageForSendingL(CSenHttpTransportProperties*& aTP,
                            CWSOviMessageContext*& aCtx);
// New functions

    const TDesC8& Id() const;
    TInt SetSessionL(MSenRemoteServiceSession& iServiceSession);

    CSenIdentifier& Identifier() const;
    const TDesC8& TransportProperties(const TDesC8& aMessageID) const;
    MSenTransport& TransportL();
    virtual TInt ChunkByTxnIdL(TInt aTxnId, CSenChunk*& aChunk);
    virtual void FileProgress(TInt aTxnId, TBool aIncoming, TBool aIsSoap,
                const TDesC8& aSoapOrCid, TInt aProgress);    
    TBool HasAuthenticationCallback();
	TInt ReauthenticationNeededL(CSenIdentityProvider*& aIdp);

	virtual void SetDataTrafficDetails(TSenDataTrafficDetails& aDetails) ;

private: 

    /**
    * C++ default constructor.
    */
    CWSOviSessionConsumer(MSenRemoteServiceConsumer& aServiceConsumer);
    
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();



private: // Data
    MSenRemoteServiceConsumer& iConsumer;
    RSentMessagesPointerMap iSentMessages; // owned
    RMapDescriptors         iSendReplyTo; // owned
    RMapDescriptors iTranspProp;
    };
    
#endif // WSOvi_SESSION_CONSUMER_H

// End of File

