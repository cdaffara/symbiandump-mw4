/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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











#ifndef WSSTAR_SESSION_CONSUMER_H
#define WSSTAR_SESSION_CONSUMER_H

// INCLUDES
#include <e32base.h>
#include "msenremoteserviceconsumer.h"
#include "senpointermap.h"
#include "SenSoapMessage.h"
#include "wsstarmessagecontext.h"



// DATA TYPES
typedef RSenPointerMap<TDesC8, HBufC8> RSentMessagesPointerMap;


// FORWARD DECLARATIONS
class RFileLogger;


// CLASS DECLARATION
/*
    Its wrapper for remote consumer. In the same time it inherits from remote consumer. Why?
    Because we can not cast inside framework pluging to our wsstar session a remote consumer, 
    which was created in core
    
    Object can ot be extended to more complex class. Can be cast to narrow class, but not in other way
*/
class CWSStarSessionConsumer : public CBase, public MSenRemoteServiceConsumer
    {
public: // Constructors and destructor
    
    static CWSStarSessionConsumer* NewL(MSenRemoteServiceConsumer& aServiceConsumer,
                                       RFileLogger& aLogger);

    static CWSStarSessionConsumer* NewLC(MSenRemoteServiceConsumer& aServiceConsumer,
                                        RFileLogger& aLogger);

    virtual ~CWSStarSessionConsumer();
 // From MSenRemoteServiceConsumer:
    TInt HandleMessageL(HBufC8* aMessage,
                        const TInt aTxnId,
                        MSenProperties* aResponseTransportProperties);

    TInt HandleErrorL(HBufC8* aError,
                      const TInt aErrorCode,
                      const TInt aTxnId,
                      MSenProperties* aResponseTransportProperties);

    void SetStatusL(const TInt status);



    TInt MessageForSendingL(const TDesC8& aBody,
                            CWSStarMessageContext*& aCtx);
// New functions

    /**
    * Expects() is called by CWSStarServiceSession to check
    * whether or not this consumer expects certain response
    */
    virtual TBool Expects(const TDesC8& aRelatesTo, const TDesC8* aTo);

    const TDesC8& Id() const;
    TInt SetSessionL(MSenRemoteServiceSession& iServiceSession);

    void HandleSoapMessageL( CSenSoapMessage* apMessage,
                             const TInt aTxnId,
                             MSenProperties* aResponseTransportProperties,
                             TBool aCompleteMessage );

   

/*
    void SetSentMessageIdL(const TDesC8& aSentMessageId);
    TPtrC8 SentMessageId();
    void SetReceivedMessageIdL(const TDesC8& aReceivedMessageId);
    TPtrC8 ReceivedMessageId();
*/
    CSenIdentifier& Identifier() const;
    TDesC8* BodyL(const TDesC8& aMessageID) const;
    const TDesC8& TransportProperties(const TDesC8& aMessageID) const;
    MSenTransport& TransportL();
    virtual TInt ChunkByTxnIdL(TInt aTxnId, CSenChunk*& aChunk);
    virtual void FileProgress(TInt aTxnId, TBool aIncoming, TBool aIsSoap,
                const TDesC8& aSoapOrCid, TInt aProgress);    
    TBool HasAuthenticationCallback();
    TInt ReauthenticationNeededL(CSenIdentityProvider*& aIdp);
	virtual void SetDataTrafficDetails(TSenDataTrafficDetails& aDetails) ; 
    TBool HasCoBrandingCallback();
    TBool OnGetBrandIdL(  const TDesC8& aBrandIdListAsXmlSnippet, RBuf8& aSelectedBrandId);

protected:
    RFileLogger* Log() const;


private: 

    /**
    * C++ default constructor.
    */
    CWSStarSessionConsumer(MSenRemoteServiceConsumer& aServiceConsumer,
                          RFileLogger& aLogger);
    
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();



private: // Data
    MSenRemoteServiceConsumer& iConsumer;
    RFileLogger& iLogger;
    //HBufC8* iSentMessageId;     // owned
    //HBufC8* iReceivedMessageId; // owned
    RSentMessagesPointerMap iSentMessages; // owned
    RMapDescriptors         iSendReplyTo; // owned
    RMapDescriptors iTranspProp;
    };
    
#endif // WSSTAR_SESSION_CONSUMER_H

// End of File
