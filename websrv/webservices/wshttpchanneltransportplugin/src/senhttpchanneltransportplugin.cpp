/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:           
*
*/











// INCLUDE FILES
#include "SenServiceConnection.h"
#include "senhttpchanneltransportplugin.h"
#include "senhttpchannelimpl.h"
#include "senhttpsyncrequester.h"
#include "SenXmlUtils.h"
#include "SenIdentityProvider.h"
#include "senwspattern.h"
#include "SenFragment.h"
#include "SenElement.h"
#include "sendebug.h"
#include "MSenProperty.h"
#include "SenHttpTransportProperties.h" // Utils\inc
#include "sentransportcontext.h"
#include "senlayeredhttptransportproperties.h" // internal Framework\inc
#include "msenmessagecontext.h"
#include "MSenMessage.h"
#include "SenSoapMessage2.h"
#include "RSenDocument.h"
#include "SenParser.h"
#include "senservicesession.h"
#include "senlogger.h"
#include "senclientsession.h"
#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengserializer.h>



namespace
    {
/* Fix for compiler warning  #177-D
#ifdef _SENDEBUG 
    // logging constants
    _LIT(KLogDir,                   "SenHttpChannelObserver");
    _LIT(KLogFile,                  "SenHttpChannelObserver.log");
#endif*/

    _LIT8(KContentType,             "text/xml; charset=UTF-8");
    }



// Create instance of concrete ECOM interface implementation
//CSenHttpChannelTransportPlugin* CSenHttpChannelTransportPlugin::NewL(
//                                    MSenCoreServiceManager& aServiceManager)
//    {
//    CSenHttpChannelTransportPlugin* pNew = CSenHttpChannelTransportPlugin::NewLC(aServiceManager);

CSenHttpChannelTransportPlugin* CSenHttpChannelTransportPlugin::NewL(CSenTransportContext* apCtx)
    {
    CSenHttpChannelTransportPlugin* pNew = CSenHttpChannelTransportPlugin::NewLC(apCtx);
    CleanupStack::Pop();
    return pNew;
    }

CSenHttpChannelTransportPlugin* CSenHttpChannelTransportPlugin::NewLC(CSenTransportContext* apCtx)
    {
    if(apCtx == NULL)
        {
        User::Leave( KErrArgument );
        }

    MSenCoreServiceManager& core = apCtx->GetCoreL();
    CSenHttpChannelTransportPlugin* pNew = new (ELeave) CSenHttpChannelTransportPlugin(apCtx, core);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

CSenHttpChannelTransportPlugin::CSenHttpChannelTransportPlugin(CSenTransportContext* apCtx,
                                                               MSenCoreServiceManager& aCore)
:   CSenTransport(apCtx),
    iServiceManager(aCore),
    iRequester(NULL),
    iHttpChannel(NULL),
    iProperties(NULL),
    iConsumerMap(EFalse, ETrue), 
    iSessionMap(EFalse, EFalse), // iConsumerMap deletes the txnIds (using same "new reserved" TInt here!)
    iStatusCode(0),
    iIsConnectionFailed(EFalse)
    {
    }

CSenHttpChannelTransportPlugin::~CSenHttpChannelTransportPlugin()
    {
    TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::~CSenHttpChannelTransportPlugin()");
    iConsumerMap.Reset(); // deletes the txnIds!
    iSessionMap.Reset(); 

    delete iHttpChannel;
    iHttpChannel = NULL;
    delete iRequester;
    iRequester = NULL;
    delete iProperties;
    iProperties = NULL;

    delete ipCtx;
    ipCtx = NULL;

    // Close the log file and the connection to the server.
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("SenHttpChannel - Log file closed.")));
    TLSLOG_CLOSE(KSenHttpChannelLogChannelBase);

    TLSLOG(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,(_L("Got last data chunk.")));
    TLSLOG_CLOSE(KSenHttpChannelObserverLogChannelBase);
    }

void CSenHttpChannelTransportPlugin::ConstructL()
    {
    TLSLOG_OPEN(KSenHttpChannelObserverLogChannelBase,KSenHttpChannelObserverLogLevel,KSenHttpChannelObserverLogDir,KSenHttpChannelObserverLogFile	);
    // Open connection to the file logger server
    TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::ConstructL(): log file opened.");
    TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::ConstructL - Version 2 [2006-05-09]");

    // Open connection to the file logger server
    TLSLOG_OPEN(KSenHttpChannelLogChannelBase, KSenHttpChannelLogLevel, KSenHttpChannelLogDir, KSenHttpChannelLogFile);
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("SenHttpChannel - Log file opened")));

    if( ipCtx )
        {
        // Attempt to find pre-defined IAP ID from transport context
        CSenWSDescription& initializer = ipCtx->GetInitializerL();


        TUint32 predefinedIapId;
        if( initializer.DescriptionClassType() == MSenServiceDescription::EWSPattern )
            {
            // Check if <ConsumerPolicy/> was defined in SC initializer (constructor argument)
            // and if IAP was predefined
            TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- Initializer XML SD type is: EWSPattern.");
            CSenWSPattern* pConsumerPolicy = (CSenWSPattern*)&initializer;

            TInt retVal = pConsumerPolicy->ConsumerIapId( predefinedIapId );
            if( retVal==KErrNone && predefinedIapId < (TUint)KErrNotFound ) // IAP ID must be 0...INT_MAX, since (TUInt)KErrNotFound == INT_MAX+1
                {
                iHttpChannel = CSenHttpChannelImpl::NewL(iServiceManager, predefinedIapId);
                }
            else
                {
                // Check if <ProviderPolicy/> was defined in SC initializer (constructor argument)
                // and if IAP was predefined
                retVal = initializer.IapId( predefinedIapId );
                if( retVal==KErrNone && predefinedIapId < (TUint)KErrNotFound )
                    {
                    iHttpChannel = CSenHttpChannelImpl::NewL(iServiceManager, predefinedIapId);
                    }
                }
            }
        else if( initializer.DescriptionClassType() == MSenServiceDescription::EWSDescription )
            {
            TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- Initializer XML SD type is EWSDescription.");
            TUint32 predefinedIapId(0);
            TInt retVal = initializer.IapId( predefinedIapId );
            if( retVal==KErrNone && predefinedIapId < (TUint)KErrNotFound ) // IAP ID must be 0...INT_MAX, since (TUInt)KErrNotFound == INT_MAX+1
                {
                iHttpChannel = CSenHttpChannelImpl::NewL(iServiceManager, predefinedIapId);
                }
            }
            
        if( !iHttpChannel )
            {
            // Check if **the service session** contains <ProviderPolicy/>
            CSenServiceSession* pSession = (CSenServiceSession*)ipCtx->GetSession();
            if( pSession )
                {
                // Check if IAP was predefined / try to figure out if IAP ID was
                // set in <ProviderPolicy/> of this session (XML SD):
                TInt retVal = pSession->IapId( predefinedIapId );
                if( retVal == KErrNone && predefinedIapId < (TUint)KErrNotFound ) // IAP ID must be 0...INT_MAX, since (TUInt)KErrNotFound == INT_MAX+1
                    {
                    iHttpChannel = CSenHttpChannelImpl::NewL(iServiceManager, predefinedIapId);
                    }
                }
            }
        }
    if( !iHttpChannel )
        {
        // IAP ID was not found from transport context
        iHttpChannel = CSenHttpChannelImpl::NewL( iServiceManager );
        }
    
    iRequester = CSenHttpSyncRequester::NewL( iHttpChannel, this );
    }

// Messaging methods start:
TInt CSenHttpChannelTransportPlugin::SubmitL(const TDesC8& aEndpoint,
                                      const TDesC8& aMessage,
                                      const TDesC8& aTransportProperties,
                                      HBufC8*& aResponse,
                                      MSenRemoteServiceConsumer& aConsumer )
    {
    TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::SubmitL:");

    // Sanity check the endpoint
    if (aEndpoint.Length() <= 0)
        {
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- No endpoint!");
        return KErrSenNoEndpoint;
        }
    TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMinLogLevel, _L8("- Endpoint: %S"), &aEndpoint));
    TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMaxLogLevel,"- Message:");
    TLSLOG_ALL(KSenHttpChannelObserverLogChannelBase,KMaxLogLevel,( aMessage ));

    CSenLayeredHttpTransportProperties& properties = LayeredPropertiesL();
    properties.ReadFromL(aTransportProperties, MSenLayeredProperties::ESenMessageLayer);
    
    ApplyPropertiesL();    

    HBufC8* pRespContentType = NULL;

    // Uses HttpSyncRequester class:

    TInt statusCode(KErrNone);
    TInt leaveCode(KErrNone);
    TRAP( leaveCode, iRequester->SubmitL(aEndpoint,           // endpoint
                                         properties,
                                         aMessage,            // request body
                                         pRespContentType,    // response content type
                                         aResponse,           // response body
                                         statusCode); )

	// Propagate data traffic counters to consumer
	
    aConsumer.SetDataTrafficDetails(iDetails);  	

    properties.ShrinkTo(MSenLayeredProperties::ESenConsumerSessionLayer);                                                                                
    if(leaveCode!=KErrNone) 
        {
        // SubmitL leaved, return leave code instead of status code
        statusCode = leaveCode;
        }
    delete pRespContentType; // not used currently
    return statusCode;
    }

// async request
TInt CSenHttpChannelTransportPlugin::SendL( const TDesC8& aEndpoint,
                                            const TDesC8& aMessage,
                                            const TDesC8& aTransportProperties,
                                            MSenServiceSession& aReplyTo,
                                            MSenRemoteServiceConsumer& aConsumer, // "adressee", could be CSenIdentifier
                                            TInt& aTxnId )
    {
    TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::SendL:");

    // Sanity check the endpoint
    if (aEndpoint.Length() <= 0)
        {
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- No endpoint!");
        return KErrSenNoEndpoint;
        }
    TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMinLogLevel, _L8("- Endpoint: %S"), &aEndpoint));
    TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMaxLogLevel,"- Content:");
    TLSLOG_ALL(KSenHttpChannelObserverLogChannelBase,KMaxLogLevel,(aMessage));
    TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMaxLogLevel,"- Properties:");
    TLSLOG_ALL(KSenHttpChannelObserverLogChannelBase,KMaxLogLevel,(aTransportProperties));
    
    CSenLayeredHttpTransportProperties& properties = LayeredPropertiesL();
    properties.ReadFromL(aTransportProperties, MSenLayeredProperties::ESenMessageLayer);
    
    ApplyPropertiesL();    

    // Reset the status code to zero. Async callbacks may then later update iStatusCode.

    iStatusCode = 0;
    
    // Note: regardless of name, SubmitL() is an async method in HttpChannel class
    TInt* submitID = new (ELeave) TInt(KErrNone);
    CleanupStack::PushL(submitID);
    TInt leaveCode(KErrNone);
    
    
    CSenSoapEnvelope2* pSoapEnvelope2 = NULL;
    TInt err(KErrNone);
    MSenMessageContext* pMsgCtx = aConsumer.MessageContextByTxnIdL(aTxnId, err);
    TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMinLogLevel, _L8("- aConsumer.MessageContextByTxnIdL(%d) returned: %d"), aTxnId, err));
    if( pMsgCtx )
        {
        MSenMessage* pMessage = pMsgCtx->MessageL();
        if( pMessage )
            {
            if( pMessage->IsSafeToCast(MSenMessage::ESoapEnvelope2 ))
                {
                pSoapEnvelope2 = (CSenSoapEnvelope2*)pMessage;
                
                properties.SetPropertyL(KAcceptLocalName, KMimeAccept);
 
                TRAP( leaveCode, (*submitID) = iHttpChannel->SendL( *this, aEndpoint, *pSoapEnvelope2, properties ); ) 

               }
            else if( pMessage->IsSafeToCast(MSenMessage::EAtomMessage ))
                {
                CSenAtomEntry* pAtomEntry = (CSenAtomEntry*)pMessage;
                
                TRAP( leaveCode, (*submitID) = iHttpChannel->SendL( *this, aEndpoint, *pAtomEntry, properties ); ) 

               }
               
            else
                {
                TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KNormalLogLevel, _L8("- MSenMessage type is: %d"), pMessage->Type()));
                TRAP( leaveCode, (*submitID) = iHttpChannel->SendL(*this, aEndpoint, aMessage, properties); )
                }
            }
        else
            {
            TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- MSenMessage arch. is not used.");            
            
			RFile &file = pMsgCtx->ChunkL()->RequestFileHandle() ; //pMsgCtx->ChunkL() must not be null !!
			if (file.SubSessionHandle())
				{
				TRAP( leaveCode, (*submitID) = iHttpChannel->SendL(*this, aEndpoint, file, properties);)
				}
			else
				{
				TRAP( leaveCode, (*submitID) = iHttpChannel->SendL(*this, aEndpoint, aMessage, properties);)
				}
			}
        }
    else
        {
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"Fatal(!): message context not found!");
        }    
    
    TInt timeOutSec(0);
    LayeredPropertiesL().MaxTimeToLiveL(timeOutSec);
    properties.ShrinkTo(MSenLayeredProperties::ESenConsumerSessionLayer);

                    
    if(leaveCode==KErrNone)
        {
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- iHttpChannel returned OK.");
        // No leave occurred
        aTxnId = *submitID;
        if (timeOutSec)
           {
           iHttpChannel->EnableTimeOutL(aTxnId, timeOutSec);
           }

        TInt retVal = iSessionMap.Append(&aReplyTo, submitID);
        if(retVal==KErrNone)
            {
            retVal = iConsumerMap.Append(&aConsumer, submitID);
            if(retVal!=KErrNone)
                {
                TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMinLogLevel, _L8("- iConsumerMap.Append failed: %d"), retVal ));

                // Last entry to iSessionMap must be removed
                TInt removeRetVal = iSessionMap.Remove(*submitID);
#ifdef _SENDEBUG
                if(removeRetVal==KErrNotFound)
                    {
                    TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMinLogLevel, _L8("- MAJOR: Could not remove session by txnId(%d)"), *submitID));
                    }
#endif // _SENDEBUG
                removeRetVal = 0; // not used in release builds
                }
            }
        else
            {
            TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMinLogLevel, _L8("- FATAL: iConsumerMap.Append failed: %d"), retVal ));
            }
        CleanupStack::Pop(submitID);
        }
    else
        {
        CleanupStack::PopAndDestroy(submitID);
        }
    return leaveCode;
    }

void CSenHttpChannelTransportPlugin::ResponseReceivedL(TInt aRequestId,
                                                       const TAny* /* aContentType */,
                                                       HBufC8* apContent, 
                                                       CSenHttpTransportProperties* aHttpProperties)
    {

    iHttpChannel->DisableTimeOutL(aRequestId);
    CleanupStack::PushL(apContent);
    
#ifdef _SENDEBUG    
    TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::ResponseReceivedL:");
    TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KNormalLogLevel, _L8("- Request's txnID: %d"), aRequestId));
    if ( apContent )
        {
        TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMaxLogLevel, _L8("- apContent:")));
        TLSLOG_ALL(KSenHttpChannelObserverLogChannelBase,KMaxLogLevel,(*apContent));
        }
    else
        {
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"Fatal(!): - apContent == NULL.");
        }
#endif // _SENDEBUG

    TInt index = iSessionMap.FindValue(aRequestId);
    if(index != KErrNotFound)
        {
        // session found OK
        MSenServiceSession* pSession =
            iSessionMap.KeyAt(index);
        iSessionMap.Remove(aRequestId); // does not delete anything

        index = iConsumerMap.FindValue(aRequestId);

        if(index != KErrNotFound)
            {
            // consumer found OK
            MSenRemoteServiceConsumer* pConsumer =
                iConsumerMap.KeyAt(index);

            iConsumerMap.Remove(aRequestId); // deletes TxnId
            
            CleanupStack::Pop(apContent);
            pSession->SendToConsumerL(apContent, aRequestId, *pConsumer, aHttpProperties);
	        pConsumer->SetDataTrafficDetails(iDetails);                                              
            }
        else
            {
            CleanupStack::PopAndDestroy(apContent);
#ifdef _SENDEBUG
            TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::ResponseReceivedL:");
            TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- pending consumer not found, txn cancelled?");
#endif // _SENDEBUG
            }
        }
    else
        {
        CleanupStack::PopAndDestroy(apContent);
#ifdef _SENDEBUG
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::ResponseReceivedL:");
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- pending session not found, txn cancelled?");
#endif // _SENDEBUG
        }
    }

// Method to catch async error responses from httpchannel
void CSenHttpChannelTransportPlugin::ResponseErrorL(TInt aRequestId, TInt aErrorCode, HBufC8* apErrorBody,CSenHttpTransportProperties* aHttpProperties)
    {
    TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMinLogLevel, _L8("CSenHttpChannelTransportPlugin::ResponseErrorL() - aErrorCode [%d]"), aErrorCode));
    CleanupStack::PushL(apErrorBody);
    if( (aErrorCode < KErrNone) && (apErrorBody == NULL) || aErrorCode == KErrAbort)
    	{
    	TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::ResponseErrorL() Set iIsConnectionFailed True");
    	iHttpChannel->ResetIapId();
    	iHttpChannel->ResetUsedIapId();
    	iHttpChannel->SetExplicitIapDefined(EFalse);
    	iIsConnectionFailed = ETrue;
    	TUint32 zeroIap = 0;
			TBuf8<128> buf;
	    buf.AppendFormat(_L8("%u"), zeroIap);
    	LayeredPropertiesL().SetPropertyL(KIapIdLocalName, buf); //Reset the layered properties in case of wrong IAP
    	}
    else
    	{
    	iHttpChannel->SetExplicitIapDefined(ETrue);
    	}	
    	iHttpChannel->DisableTimeOutL(aRequestId);
#ifdef _SENDEBUG    
    TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KNormalLogLevel, _L8("- Request's txnID: %d"), aRequestId));
    if ( apErrorBody )
        {
        TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMaxLogLevel, _L8("- apErrorBody:")));
        TLSLOG_ALL(KSenHttpChannelObserverLogChannelBase,KMaxLogLevel,(*apErrorBody));
        }
    else
        {
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"Fatal(!): - apErrorBody == NULL.");
        }
#endif // _SENDEBUG
    
    iStatusCode = aErrorCode;

    TInt index = iSessionMap.FindValue(aRequestId);
    if(index != KErrNotFound)
        {
        // session found OK
        MSenServiceSession* pSession =
            iSessionMap.KeyAt(index);
        iSessionMap.Remove(aRequestId); // does not delete anything
    
        index = iConsumerMap.FindValue(aRequestId);
        if(index != KErrNotFound)
            {
            // consumer found OK
            MSenRemoteServiceConsumer* pConsumer =
                iConsumerMap.KeyAt(index);

            iConsumerMap.Remove(aRequestId); // deletes TxnId
        
            CleanupStack::Pop(apErrorBody);
            pSession->SendErrorToConsumerL(aErrorCode, 
                                          apErrorBody, 
                                          aRequestId, 
                                          *pConsumer,
                                          aHttpProperties);
            apErrorBody = NULL;
            pConsumer->SetDataTrafficDetails(iDetails);                                                                                        
            }
        else
            {
            CleanupStack::PopAndDestroy(apErrorBody);
#ifdef _SENDEBUG
       		TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::ResponseErrorL:");
            TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- pending consumer not found, txn cancelled?");
#endif // _SENDEBUG
            }
        }
    else
        {
        //CleanupStack::PopAndDestroy(apErrorBody);
        CleanupStack::Pop(apErrorBody); // for EPSK-76XFAN
#ifdef _SENDEBUG
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::ResponseErrorL:");
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- pending session not found, txn cancelled?");
#endif // _SENDEBUG
        }
    }

/*
RFileLogger* CSenHttpChannelTransportPlugin::Log() const
    {
    return (RFileLogger*) &iLogger;
    }
*/

TInt CSenHttpChannelTransportPlugin::SetPropertiesL(const TDesC8& aProperties,
                                                    MSenLayeredProperties::TSenPropertiesLayer aTargetLayer,
                                                    MSenRemoteServiceConsumer* /*aConsumer*/)
    {
    TInt retVal(KErrNone);
    if ( aTargetLayer == MSenLayeredProperties::ESenTransportLayer )
        {
        retVal = KErrNotSupported;
        }
    else
        {
        LayeredPropertiesL().ReadFromL(aProperties, aTargetLayer);
        retVal = ApplyPropertiesL();
        }
    return retVal;
    }

TInt CSenHttpChannelTransportPlugin::PropertiesL(HBufC8*& aProperties)
    {
    aProperties = PropertiesL().AsUtf8L();
    return KErrNone;
    }

TInt CSenHttpChannelTransportPlugin::ApplyPropertiesL()
    {
    TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::ApplyPropertiesL()");

    TInt retVal(KErrNone);

    // ALWAYS HANDLE IAP ID FIRST, SINCE IT *CAN RESET* HTTPCHANNEL!
    
    TUint32 iapId(KErrNone);
    retVal = LayeredPropertiesL().IapIdL(iapId);
    TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMinLogLevel, _L8("- CSenHttpChannelTransportPlugin::ApplyPropertiesL() - iapId [%d]"), iapId));
	TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMinLogLevel, _L8("- LayeredPropertiesL() retVal [%d]"), retVal)); 
    if(iIsConnectionFailed)
        {
		TInt resetPerformed = ResetHttpChannelByIapIdL(0); //Incase of connection error set Iap ID 0
#ifdef _SENDEBUG
        if(resetPerformed == KErrNone)
            {
            TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KNormalLogLevel, _L8("- IAP re-set performed: (%d)"), iapId));
            }
#endif // _SENDEBUG
        }
    else if(retVal==KErrNone)
        {
       	TInt resetPerformed = ResetHttpChannelByIapIdL(iapId);
#ifdef _SENDEBUG
        if(resetPerformed == KErrNone)
            {
            TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KNormalLogLevel, _L8("- IAP re-set performed: (%d)"), 0));
            }
#endif // _SENDEBUG				
        }

    // Handle ProxyHost and ProxyPort
    TPtrC8 proxyHost;
    retVal = LayeredPropertiesL().ProxyHostL(proxyHost);
    if ( retVal == KErrNone )
        {
        TInt proxyPort(KErrNotFound);
        retVal = LayeredPropertiesL().ProxyPortL(proxyPort);
        iHttpChannel->SetProxyL(proxyHost, proxyPort);
        }


    // HttpVersion
    CSenLayeredHttpTransportProperties::TSenHttpVersion httpVersion;
    retVal = LayeredPropertiesL().HttpVersionL(httpVersion);
    if ( retVal == KErrNone )
        {
        switch ( httpVersion )
            {
            case CSenLayeredHttpTransportProperties::ESenHttp10:
                {
                TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- SetHttpVersionL(ESenHttp10)");
                iHttpChannel->SetHttpVersionL(0);
                }
                break;
            case CSenLayeredHttpTransportProperties::ESenHttp11:
                {
                TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- SetHttpVersionL(ESenHttp11)");
                iHttpChannel->SetHttpVersionL(1);
                }
                break;
            default:
                // Not possible
                break;
            }            
        }
    return KErrNone; 
    }

TInt CSenHttpChannelTransportPlugin::ResetHttpChannelByIapIdL(TUint32 aIapId)
    {
    TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::ResetHttpChannelByIapIdL()");
    TUint32 effectiveIapId(KErrNone);
    TBool explicitIapIdDefined(EFalse);
    if(iHttpChannel)
        {
        explicitIapIdDefined = iHttpChannel->EffectiveIapId(effectiveIapId);
        }

    TInt retVal(KErrNone);
    TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMinLogLevel, _L8("- explicitIapIdDefined[%d], effectiveIapId[%d], aIapId[%d]"), explicitIapIdDefined, effectiveIapId, aIapId));
    if(!explicitIapIdDefined || (explicitIapIdDefined && (effectiveIapId != aIapId) ) || iIsConnectionFailed)
        {
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::ResetHttpChannel():");
        iIsConnectionFailed = EFalse;
        delete iHttpChannel;
        iHttpChannel = NULL;
        delete iRequester;
        iRequester = NULL;
        TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMinLogLevel, _L8("- New IAP ID: %d"), aIapId));
        CSenHttpChannel* pHttpChannel = CSenHttpChannelImpl::NewL(iServiceManager, aIapId);
        
        CleanupStack::PushL(pHttpChannel);

        CSenHttpSyncRequester* pRequester = CSenHttpSyncRequester::NewL(pHttpChannel, this);

        iHttpChannel = pHttpChannel;
        iRequester = pRequester;

        CleanupStack::Pop(pHttpChannel);
        }
    else
        {
        retVal = KErrAlreadyExists;
        }
    TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMinLogLevel, _L8("CSenHttpChannelTransportPlugin::ResetHttpChannelByIapIdL() - returns [%d]"), retVal));
    return retVal;
    }

TInt CSenHttpChannelTransportPlugin::CompleteTransaction(const TInt aTxnId,
                                                         const TInt /* aCompletionCode */)
    {
    TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KNormalLogLevel, _L8("CSenHttpChannelTransportPlugin::CompleteTransaction(txn ID: %d)"), aTxnId /*, aCompletionCode */));

    // Remove the session and consumer from arrays to prevent
    // any unneccessary processing of completed transactions..

    // Handle completed session
    TInt index = iSessionMap.FindValue(aTxnId);
    if(index != KErrNotFound)
        {
        // Session found
        TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KNormalLogLevel, _L8("- Pending session found, removing it from list."), index));

        //MSenServiceSession* pSession =
        //    iSessionMap.KeyAt(index);

        index = iSessionMap.Remove(aTxnId); // does not delete anything
        TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KNormalLogLevel, _L8("- Remove() returned: %d"), index));
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- Cannot find pending session with given ID.");
        }
#endif
    // Handle cancelled consumer
    TInt index2 = iConsumerMap.FindValue(aTxnId);
    if(index2 != KErrNotFound)
        {
        // Consumer found
        TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KNormalLogLevel, _L8("- Pending consumer (%d) found, removing it from list."), 
            index2));

        //MSenRemoteServiceConsumer* pConsumer =
        //    iConsumerMap.KeyAt(index2);

        index2 = iConsumerMap.Remove(aTxnId); // deletes TxnId
        TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KNormalLogLevel, _L8("- Remove() returned: %d"), index2));
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- Cannot find pending consumer with given ID.");
        }
#endif
    return index<index2?index:index2;
    }

TInt CSenHttpChannelTransportPlugin::CancelTransaction(const TInt aTxnId)
    {
    TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KNormalLogLevel, _L8("CSenHttpChannelTransportPlugin::CancelTransaction(%d)"), aTxnId));
    TInt index = CompleteTransaction(aTxnId, KErrSenCancelled);
    if(iHttpChannel)
        {
        iHttpChannel->CancelTransaction(aTxnId);
        }
    return index;
    }
    
TPtrC8 CSenHttpChannelTransportPlugin::UriSchemeL()
    {
    return KSenHttpChannelTransportUriScheme();
    }
   

// Private helper
CSenLayeredHttpTransportProperties& CSenHttpChannelTransportPlugin::LayeredPropertiesL()
    {
    if ( !iProperties )
        {
        iProperties = CSenLayeredHttpTransportProperties::NewL();
        iProperties->SetPropertyL(KAcceptLocalName, KSenHttpChannelAcceptHeaderDefault, KHttpHeaderType);
        iProperties->SetPropertyL(KUserAgentLocalName, KSenHttpChannelUserAgentHeaderDefault, KHttpHeaderType);
        iProperties->SetPropertyL(KContentTypeLocalName, KContentType, KHttpHeaderType);
        }
    iProperties->SetReader(*iServiceManager.XMLReader());
    return *iProperties;
    }

MSenProperties& CSenHttpChannelTransportPlugin::PropertiesL()
    {
    CSenLayeredHttpTransportProperties& properties = LayeredPropertiesL();
    if( iHttpChannel )
    	{
    	TUint32 iapId(0);
    	TInt error = properties.IapIdL( iapId );
    	if ( error || iapId == 0 || iapId >=(TUint32)KErrNotFound ) // IAP is either not found from properties, or it is 0 or > MAX_INT (signed)
	    	{
		    TBool isIapSet = iHttpChannel->EffectiveIapId( iapId );
		    if( isIapSet && iapId > 0 )
		    	{
			    TBuf8<128> buf;
			    buf.AppendFormat(_L8("%u"), iapId);
			    properties.SetPropertyL( KIapIdLocalName, buf );	    	
		    	}
            if( iHttpChannel->UsedIap() )
                {
                TBuf8<128> buf;
                buf.AppendFormat(_L8("%u"), iHttpChannel->UsedIap());
                properties.SetPropertyL( KIapIdLocalName, buf );            
                }		    
	    	}
    	}
    return properties;
    }

// Propagate http status code to remote service consumer
void CSenHttpChannelTransportPlugin::StateChanged(TInt aRequestId, 
													TInt aState)
	{
	TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMaxLogLevel,"CSenHttpChannelTransportPlugin::StateChanged:");
    TLSLOG_FORMAT((KSenHttpChannelObserverLogChannelBase,KMaxLogLevel, _L8("- request txnID: %d"), aRequestId));
        
    TInt index = iConsumerMap.FindValue(aRequestId);
    if(index != KErrNotFound)
        {
		// propagate the Status code	
		MSenRemoteServiceConsumer* pConsumer = iConsumerMap.KeyAt(index);
		

		TInt state = KSenTransportStatusCodeOffsetBaseHttp + aState;
		pConsumer->StateChanged(state);
        }
#ifdef _SENDEBUG
    else
        {
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"CSenHttpChannelTransportPlugin::StateChanged:");
        TLSLOG_L(KSenHttpChannelObserverLogChannelBase,KMinLogLevel,"- pending consumer not found, txn cancelled?");
        }
#endif // _SENDEBUG
	}

void CSenHttpChannelTransportPlugin::FileProgress(TInt aTxnId, TBool aIncoming,
        TBool aIsSoap, const TDesC8& aSoapOrCid, TInt aProgress)
    {
    TInt index = iConsumerMap.FindValue(aTxnId);
    if (index != KErrNotFound)
        {
        iConsumerMap.KeyAt(index)->FileProgress(aTxnId, aIncoming, aIsSoap,
                aSoapOrCid, aProgress);
        }
    }

void CSenHttpChannelTransportPlugin::SetTrafficDetails(TSenDataTrafficDetails& aDetails)	
	{
	iDetails = aDetails;
	}
	
void CSenHttpChannelTransportPlugin::DataTrafficDetails(TSenDataTrafficDetails& aDetails) 
	{
	aDetails = iDetails;
	}
// End of File
