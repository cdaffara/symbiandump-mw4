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
* Description:    Header declaration
*
*/











#ifndef SEN_HOSTLET_TRANSPORT_PLUGIN
#define SEN_HOSTLET_TRANSPORT_PLUGIN

#include <e32base.h>
#include <s32strm.h>
#include <e32std.h>

#include <MSenProperties.h>

#include "sentransport.h"
#include "senpointermap.h" // from Utils\inc


// CONSTANTS
_LIT8(KSenHostletTransportUriScheme, "hostlet");

// FORWARD DECLARE
class MSenProvider;
class CSenProvider;
class CSenActiveHostletRequest;
class CSenHostletRequest;
class CSenHostletResponse;
class CSenSyncRequester;
class CSenLayeredXmlProperties;
class CSenTransportProperties;
class MSenTransportContext;

// TYPEDEFS
typedef RSenPointerMap<MSenRemoteServiceConsumer, TInt> RConsumerMap;
typedef RSenPointerMap<MSenServiceSession, TInt> RSessionMap;

typedef RSenPointerMap<CSenActiveHostletRequest, TInt> RLocalRequestMap;



class CSenHostletTransportPlugin : public CSenTransport
    {
    public: 

        // Constructors:
        static CSenHostletTransportPlugin* NewL(CSenTransportContext* apCtx);
        static CSenHostletTransportPlugin* NewLC(CSenTransportContext* apCtx);

        // Destructor:
        virtual ~CSenHostletTransportPlugin();

         // from MSenTransport
        virtual TInt SubmitL(const TDesC8& aEndpoint,
                             const TDesC8& aMessage,
                             const TDesC8& aTransportProperties,
                             HBufC8*& aResponse,
                             MSenRemoteServiceConsumer& aConsumer);

        virtual TInt SendL(const TDesC8& aEndpoint,
                           const TDesC8& aMessage,
                           const TDesC8& aTransportProperties,
                           MSenServiceSession& aReplyTo,
                           MSenRemoteServiceConsumer& aConsumer, // "addressee", could be CSenIdentifier
                           TInt& aTxnId);

        virtual TInt SetPropertiesL(const TDesC8& aProperties, 
                                    MSenLayeredProperties::TSenPropertiesLayer aTargetLayer,
									MSenRemoteServiceConsumer* aConsumer);
        virtual TInt PropertiesL(HBufC8*& aProperties);
        virtual TInt CompleteTransaction(const TInt aTxnId, const TInt aStatus = KErrNone);
        virtual TInt CancelTransaction(const TInt aTxnId);
        virtual TPtrC8 CSenHostletTransportPlugin::UriSchemeL();
        
        virtual MSenProperties& PropertiesL();
        
        //RConsumerMap ConsumerMap();
        //RSessionMap SessionMap();

    private:
        void ConstructL();
        CSenHostletTransportPlugin(CSenTransportContext* apCtx);

        // Helper, to locally "proxy" the provider, which was invoked *last* time
        MSenProvider& ProviderL(const TDesC8& aEndpoint);

    private: // member data
        CSenSyncRequester* ipRequester;
        CSenProvider* iInvokedProvider;
        TInt iStatusCode ;
        TInt iTransactionID;
        //RConsumerMap iConsumerMap; // values owned (TxnIds), keys not (consumers)
        //RSessionMap iSessionMap;   // values NOT owned (TxnIds), neither keys (sessions)
        RLocalRequestMap iRequestMap;
        CSenLayeredXmlProperties* ipLayeredXmlProperties;
    } ;

class CSenActiveHostletRequest : public CActive
    {
    friend class CSenHostletTransportPlugin;

    

    public:

        enum TLocalRequestState
            {
            EInvokeService = 1,
            ECompleteTransaction
            };

        // Standard 2 phased constructor
        static CSenActiveHostletRequest* NewLC(const TDesC8& aEndpoint,
                                               MSenServiceSession& aReplyTo,
                                               MSenRemoteServiceConsumer& aConsumer,
                                               TInt aTxnId,
                                               RLocalRequestMap& aRequestMap,
                                               MSenTransport& aParent,
                                               MSenTransportContext& aParentCtx);

        // C++ destructor
        virtual ~CSenActiveHostletRequest();

        MSenServiceSession& Session() const;
        MSenRemoteServiceConsumer& Consumer() const;
        TInt TxnId() const;
        // TPtrC8 TransportProperties() const;
        RLocalRequestMap& RequestMap();
        
        // This method is called when txn is ready to be completed
        // Note that this sets this AO in active state(!)
        TInt Complete(TRequestStatus*& aStatus, TInt aCompletionCode = KErrNone); 
        
        virtual void RunL(); 
        virtual void DoCancel();
        virtual TInt RunError(TInt aError);

    private:
        // 2nd phase constructor
        void ConstructL();

        // C++ constructor
        CSenActiveHostletRequest(const TDesC8& aEndpoint,
                                 MSenServiceSession& aReplyTo,
                                 MSenRemoteServiceConsumer& aConsumer,
                                 TInt aTxnId,
                                 RLocalRequestMap& aRequestMap,
                                 MSenTransport& aParent,
                                 MSenTransportContext& aParentCtx);

        CSenChunk* CSenActiveHostletRequest::FindChunkL(const TInt aTxnId, TInt& aIndex) ;                      

        CSenTransportProperties* NewPropertiesByTypeL( MSenProperties::TSenPropertiesClassType aType, const TDesC8& aPropertiesAsXml );

    private: // Data
        const TDesC8&               iEndpoint;
        TInt                        iCompletionCode;
        MSenServiceSession&         iReplyTo;
        MSenRemoteServiceConsumer&  iConsumer;
        TInt                        iTxnId;
        RLocalRequestMap&           iRequestMap;
        MSenTransport&              iParent;
        MSenTransportContext&       iParentContext;
        TPtrC8                      iMessage;
        TPtrC8                      iProperties; 
        
    };

#endif // SEN_HOSTLET_TRANSPORT_PLUGIN
