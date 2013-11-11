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











#ifndef SEN_LOCAL_TRANSPORT_PLUGIN
#define SEN_LOCAL_TRANSPORT_PLUGIN

#include <e32base.h>
#include <s32strm.h>
#include <e32std.h>
#include <flogger.h>

#include "sentransport.h"
#include "senpointermap.h" // from Utils\inc
#include "senlayeredxmlproperties.h"

// CONSTANTS
_LIT8(KSenLocalTransportUriScheme, "local");

// FORWARD DECLARE
class MSenProvider;
class CSenProvider;
class CSenLocalRequest;
class CSenHostletRequest;
class CSenHostletResponse;
class CSenSyncRequester;

// TYPEDEFS
typedef RSenPointerMap<CSenLocalRequest, TInt> RLocalRequestMap;



class CSenLocalTransportPlugin : public CSenTransport
    {
    public: 

        // Constructors:
        static CSenLocalTransportPlugin* NewL(CSenTransportContext* apCtx);

        static CSenLocalTransportPlugin* NewLC(CSenTransportContext* apCtx);

        // Destructor:
        virtual ~CSenLocalTransportPlugin();

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
        virtual TInt CompleteTransaction(const TInt aTxnId, const TInt aStatus);
        virtual TInt CancelTransaction(const TInt aTxnId);
        virtual TPtrC8 CSenLocalTransportPlugin::UriSchemeL();

        virtual MSenProperties& PropertiesL();
        

    private:
        void ConstructL();
        CSenLocalTransportPlugin(CSenTransportContext* apCtx);
        //RFileLogger* Log() const;

        // Helper, to locally "proxy" the provider, which was invoked *last* time
        MSenProvider& ProviderL(const TDesC8& aEndpoint);

    private: // member data
        CSenSyncRequester* ipRequester;
        CSenProvider* iInvokedProvider;
        //RFileLogger iLogger;    
        TInt iStatusCode ;
        TInt iTransactionID;
        RLocalRequestMap iRequestMap;
        CSenLayeredXmlProperties* ipTransportProperties;
    } ;

class CSenLocalRequest : public CActive
    {
    friend class CSenLocalTransportPlugin;

    

    public:

        enum TLocalRequestState
            {
            EInvokeService = 1,
            EDeliverResponse,
            EDeliverComplete
            };

        // Standard 2 phased constructor
        static CSenLocalRequest* NewLC(const TDesC8& aEndpoint, //MSenProvider& aRequestTo,
                                       const TDesC8& aMessage,
                                       const TDesC8& aTransportProperties,
                                       MSenServiceSession& aReplyTo,
                                       MSenRemoteServiceConsumer& aConsumer, // "addressee", could be CSenIdentifier
                                       MSenCoreServiceManager& aProviderProxy,
                                       TInt aTxnId);

        // C++ destructor
        virtual ~CSenLocalRequest();

        MSenServiceSession& Session() const;
        MSenRemoteServiceConsumer& Consumer() const;
        TInt TxnId() const;

        TPtrC8 TransportProperties() const;

        virtual void RunL(); 
        virtual void DoCancel();

        //void SetLogger(RFileLogger& aLogger);

        void ExecuteL(); // called by thread

        const TThreadId OwnerThreadId() const;

        CSenHostletResponse& Response();

        TBool Delivered() const;
        
		static void LocalRequestExceptionHandlerL(TExcType aType);
        

    private:
        // 2nd phase constructor
        void ConstructL(const TDesC8& aMessage, 
                        const TDesC8& aTransportProperties);

        // C++ constructor
        CSenLocalRequest(const TDesC8& aEndpoint, //MSenProvider& aRequestTo,
                         MSenServiceSession& aReplyTo,
                         MSenRemoteServiceConsumer& aConsumer, // "addressee", could be CSenIdentifier
                         MSenCoreServiceManager& aProviderProxy,
                         TInt aTxnId
                         );
        //RFileLogger* Log() const;

        void DeliverL(); // Send the response back to the consumer, who invoked the service provider

    private: // Data
        MSenProvider*               ipProvider;  // not owned
        const TDesC8&               iEndpoint;
        MSenCoreServiceManager&     iProviderProxy;
        CSenHostletRequest*         ipRequestImpl;
        CSenHostletResponse*        ipResponseImpl;

        HBufC8*                     ipTransportProperties;
        MSenServiceSession&         iReplyTo;
        MSenRemoteServiceConsumer&  iConsumer; // holds CSenIdentifier, the requester
        TInt                        iTxnId;
        //RFileLogger*                iLogger;
        TThreadId                   iOwnerThreadId;
        RThread                     iThread;
        TInt                        iLeaveCode;    // place holder for leave code
        TInt                        iRetVal;       // place holder for return code
        TBool                       iDelivered;
        // ownership is transferred if response is delivered otherwise to be cleaned up
        HBufC8* 					ipResponse; 
    };

#endif // SEN_LOCAL_TRANSPORT_PLUGIN
