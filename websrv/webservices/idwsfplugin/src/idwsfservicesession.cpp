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
* Description:        
*
*/








// INCLUDE
#include <SenServiceConnection.h> // error code definitions
#include <SenDateUtils.h>
#include <SenSoapFault.h>
#include <SenWsSecurityHeader.h>
#include <SenTransportProperties.h>
#include <SenSoapConstants.h> // SOAPAction constants, like KSenSoapActionHeaderName

#include "sendebug.h"
#include "senlogger.h"

#include "msencoreservicemanager.h"
#include "senservicemanagerdefines.h"
#include "senserviceinvocationframework.h"

#include "midwsfsessionvalidator.h"
#include "idwsfservicesession.h"
#include "idwsfsessionconsumer.h"
#include "idwsfserviceinstance.h"
#include "seninternalcredential.h"
#include <SenXmlReader.h>

namespace
    {
    _LIT8(KSAML,                        "SAML");
    _LIT8(KBEARER,                      "Bearer");

    _LIT8(KProviderID,                  "<ProviderID>");
    _LIT8(KProviderIDEnd,               "</ProviderID>");
    _LIT8(KResourceID,                  "<ResourceID>");
    _LIT8(KResourceIDEnd,               "</ResourceID>");
    _LIT8(KTrustAnchor,                 "<TrustAnchor>");
    _LIT8(KTrustAnchorEnd,              "</TrustAnchor>");
    _LIT8(KServiceInterval,             "<ServiceInterval>");
    _LIT8(KServiceIntervalEnd,          "</ServiceInterval>");

    _LIT8(KProviderIdElementLocalName,  "ProviderID");
    _LIT8(KResourceIDElementLocalName,  "ResourceID");
    _LIT8(TrustAnchorElementLocalName,  "TrustAnchor");
    //_LIT8(KServiceIntervalElementLocalName, "ServiceInterval");

    const TInt KSubmitStateOK                       = 1;
    const TInt KSubmitStateInvalidCredentials       = 2;
    const TInt KSubmitStateRevalidationAttempted    = 3;
    const TInt KSubmitStateRevalidationFailed       = 4;
    //const TInt KSubmitSateResend					= 5;
    //const TInt KSubmitSateResendFailed			= 6;
    }

CIdWsfServiceSession* CIdWsfServiceSession::NewL(MSIF& aFramework)
    {
    CIdWsfServiceSession* self = CIdWsfServiceSession::NewLC(aFramework);
    CleanupStack::Pop(self);
    return self;
    }

CIdWsfServiceSession* CIdWsfServiceSession::NewLC(MSIF& aFramework)
    {
    CIdWsfServiceSession* self = new (ELeave) CIdWsfServiceSession(
        MSenServiceDescription::EIdWSFServiceSession, aFramework);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CIdWsfServiceSession::CIdWsfServiceSession(TDescriptionClassType aType,
                                           MSIF& aSIF)
    : CSenWebServiceSession(aType, aSIF),
        iProvider(NULL),
        ipResourceId(NULL),
        ipTrustAnchor(NULL),
        ipReceivedMessageId(NULL),
        ipSenSecurityMechanism(NULL),
        iMessageThread(EFalse),
        ipValidator(NULL),
        iSubmitState(KSubmitStateOK)
    {
    }

CIdWsfServiceSession::~CIdWsfServiceSession()
    {
    delete iProvider;
    delete ipResourceId;
    delete ipTrustAnchor;
    delete ipReceivedMessageId;
    delete ipSenSecurityMechanism;

    TInt count(iConsumerList.Count());
    for(TInt i=0; i<count; i++)
        {
        // we can assume that every remote consumer
        // has been wrapped inside IdWsfSessionConsumer
        // -wrapperclass. Cast and destroy:
        CIdWsfSessionConsumer* pConsumer
            = (CIdWsfSessionConsumer*) iConsumerList[i];

        // this destroys the wrapper, but the remote
        // consumer objects ownership remains in either
        // XMLDao or ClientSession (etc)
        delete pConsumer;
        }
    }

void CIdWsfServiceSession::ConstructL()
    {
    // sets local name to "ServiceDescription" and
    // initiates the inner ipElement
    CSenWebServiceSession::ConstructL();
    }

TInt CIdWsfServiceSession::AddConsumerL(MSenRemoteServiceConsumer& aConsumer)
    {
    const TInt consumerCount(iConsumerList.Count());
    for(TInt i=0; i<consumerCount; i++)
        {
        if(iConsumerList[i]->Id() == aConsumer.Id())
            return KErrAlreadyExists; // already added, nothing to do
        }

    CIdWsfSessionConsumer* pSessionConsumer =
        CIdWsfSessionConsumer::NewL(aConsumer, *Log());

    iConsumerList.Append(pSessionConsumer);

    // clears the array and inserts item as the only one..
    // remove next line, when NEW routing is in use:
    return KErrNone;
    }

TInt CIdWsfServiceSession::RemoveConsumerL(MSenRemoteServiceConsumer& aConsumer)
    {
    const TInt consumerCount(iConsumerList.Count());
    for(TInt i=0; i<consumerCount; i++)
        {
        if(iConsumerList[i]->Id() == aConsumer.Id())
            {
            CIdWsfSessionConsumer* pConsumer
                            = (CIdWsfSessionConsumer*) iConsumerList[i];
            delete pConsumer;
            iConsumerList.Remove(i);
            break;
            }
        }
    return CSenServiceSession::RemoveConsumerL(aConsumer);
    }

TPtrC8 CIdWsfServiceSession::FrameworkId()
    {
    return iFramework.Id();
    }

TInt CIdWsfServiceSession::MessageForSendingL(const TDesC8& aBody,
                                              const TDesC8& aSenderID,
                                              CSenSoapMessage*& aMessage)
    {
    CIdWsfSessionConsumer* pConsumer = NULL;
    TInt retVal = SessionConsumerL(aSenderID, pConsumer);
    if (retVal != KErrNone)
        {
        return retVal; 
        }
    if(ipReceivedMessageId)
        {
        retVal = pConsumer->MessageForSendingL(aBody,
                                                *ipReceivedMessageId,
                                                (CIdWsfMessage*&)aMessage);
        }
    else
        {
        retVal = pConsumer->MessageForSendingL( aBody,
                                                KNullDesC8(),
                                                (CIdWsfMessage*&)aMessage);
        }
    if (retVal != KErrNone)
        {
        return retVal; 
        }

    SetFrameworkHeadersL(*aMessage);
    return KErrNone;
    }

TInt CIdWsfServiceSession::NewMessageL(CSenSoapMessage*& aMessage)
    {
    aMessage = CIdWsfMessage::NewL();
    return KErrNone;
    }

// from XML service description
TInt CIdWsfServiceSession::InitializeFromL(
                    MSenServiceDescription& aDescription)
    {
    CSenWebServiceSession::SetSecurityL(KNullDesC8);
    CSenWebServiceSession::InitializeFromL(aDescription);
        
    if ( StatusL() != KSenConnectionStatusReady )
        {
        if ( TryToSearchValidCredentialL() == KErrNone )	//codescannerwarnings
            {
            SetStatusL();
            }
        }

    aDescription.HasFacetL(KMessageThread,iMessageThread);

    TDescriptionClassType classType = aDescription.DescriptionClassType();
    if( classType == MSenServiceDescription::EWSDescription
                                            ||
        classType == MSenServiceDescription::EWSPattern
                                            ||
        classType == MSenServiceDescription::EIdentityProvider
        )
        {
        MSenElement& xmlSdAsElement = (
            (CSenWSDescription*)&aDescription)->AsElement();


        MSenElement* pElement =
                        xmlSdAsElement.Element(KProviderIdElementLocalName);
        if(pElement)
            {
            TPtrC8 providerId = pElement->Content();
            TPtrC8 endPoint = aDescription.Endpoint();

            CIdWsfServiceProvider* pNewProvider =
                CIdWsfServiceProvider::NewL(providerId, endPoint);
            // now it is safe to free & re-assign
            delete iProvider;
            iProvider = pNewProvider;
            }

        pElement = xmlSdAsElement.Element(KResourceIDElementLocalName);
        if(pElement)
            {
            HBufC8* pNewResourceId = (pElement->Content()).AllocL();
            // now it is safe to free & re-assign
            delete ipResourceId;
            ipResourceId = pNewResourceId;
            }

        pElement = xmlSdAsElement.Element(TrustAnchorElementLocalName);
        if(pElement)
            {
            HBufC8* pNewTrustAnchor = (pElement->Content()).AllocL();
            delete ipTrustAnchor;
            ipTrustAnchor = pNewTrustAnchor;
            }
        }
    TPtrC8  contract = Contract(); 
    if (contract == KNullDesC8)
        {
        return KErrSenNoContract;
        }
    return KErrNone;
    }

TInt CIdWsfServiceSession::ParseMessageL(CSenSoapMessage& aSOAPMessage)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfServiceSession::ParseMessageL");

    if (iMessageThread)
        {
        delete ipReceivedMessageId;
        ipReceivedMessageId =
                        ((CIdWsfMessage&)aSOAPMessage).MessageId().Alloc();
        if (!ipReceivedMessageId)
            return KErrNoMemory;
        }

    TInt error = CSenWebServiceSession::ParseMessageL(aSOAPMessage);
    if (error != KErrNone)
        {
        return error;
        }

    CIdWsfMessage& message = (CIdWsfMessage&)aSOAPMessage;

    TTime serverTime;
    if (message.Timestamp().Length() > 0)
        {
        serverTime = SenDateUtils::FromXmlDateTimeL(message.Timestamp());
        }
    else serverTime = Time::NullTTime();

    TTime clientTime;
    clientTime.UniversalTime();

#ifdef _SENDEBUG
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfServiceSession::ParseMessageL:");
    if (clientTime != Time::NullTTime())
        {
        TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
        SenDateUtils::ToXmlDateTimeUtf82L(ts, clientTime);
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L8("- Client (device) time : %S"), &ts));
        }
#endif // _SENDEBUG

    if ( serverTime != Time::NullTTime() )
        {
#ifdef _SENDEBUG
        TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts2;
        SenDateUtils::ToXmlDateTimeUtf82L(ts2, serverTime);
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L8("- Server (remote service) time : %S"), &ts2));

#endif // _SENDEBUG

        iClientServerInterval = serverTime.MicroSecondsFrom(clientTime);
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L8("- Interval in microseconds : %d"),  iClientServerInterval.Int64()));
        }

    delete ipReceivedMessageId;
    ipReceivedMessageId = NULL;
    ipReceivedMessageId = message.MessageId().AllocL();

    CIdWsfSiuHeader* siuheader = message.SiuHeader();
    if (siuheader)
        {
        error = UpdateFromL(*siuheader);
        if (error != KErrNone) return error;
        }
    return KErrNone;
    }

TInt CIdWsfServiceSession::SendSoapMessageToConsumerL( CSenSoapMessage* apMessage,
                                                       const TInt aTxnId,
                                                       MSenRemoteServiceConsumer& aConsumer,
                                                       MSenProperties* aResponseTransportProperties ) 
    {
    CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CIdWsfServiceSession::SendSoapMessageToConsumerL");
    TInt retVal(KErrNone);
    
    if( apMessage )
        {
        CleanupStack::PushL( apMessage ); // ownership is here
        
        CIdWsfMessage* pIdWsfMessage = (CIdWsfMessage*)apMessage;
    
        if ( iMessageThread )
            {
            // store the Id of the last received message here:
            delete ipReceivedMessageId;
            ipReceivedMessageId = pIdWsfMessage->MessageId().Alloc();
            if ( !ipReceivedMessageId ) 
                {
                CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"- OOM occured!");
                CleanupStack::PopAndDestroy( apMessage ); // de-alloc the response immediately
                retVal = SendErrorToConsumerL( KErrNoMemory, NULL, aTxnId, aConsumer, aResponseTransportProperties );
                CSLOG_FORMAT((aConsumer.ConnectionId(), KMinLogLevel , _L8("- OOM occured. Sent KErrNoMemory to remote consumer. SendErrorToConsumerL returned: %d "), retVal));
                return KErrNoMemory;
                }
            }
    
        CIdWsfSessionConsumer* pConsumer = NULL;
        // This takes care of messageIDs:
        retVal = SessionConsumerL( *pIdWsfMessage, pConsumer );
        if (!pConsumer)
            {
            CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"- Session consumer not found! Invoking remote consumer via a call to base class functionality.");
            CleanupStack::Pop( apMessage ); // Next method takes ownership:
            retVal = CSenWebServiceSession::SendSoapMessageToConsumerL( apMessage, aTxnId, aConsumer, aResponseTransportProperties );
            CSLOG_FORMAT((aConsumer.ConnectionId(), KNormalLogLevel , _L8("- SendSoapMessageToConsumerL returned: %d "), retVal));
            return KErrNotFound;
            }
        else
            {
            //TSW ID: EMKY-6S4CGS & ID:EMKY-6N3AGA
            //check for completemessage flag
            TBool completeServerMessages;
            HasFacetL(KCompleteMessagesFacet, completeServerMessages);
            CleanupStack::Pop( apMessage ); // Next method takes ownership:
            retVal = pConsumer->HandleIdWsfMessageL( pIdWsfMessage, aTxnId, aResponseTransportProperties, completeServerMessages );
            }
        }
    else // apMessage == NULL!
        {
        // There is no SOAP to handle
        CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"- Fatal(!): CIdWsfServiceSession::SendSoapMessageToConsumerL, apMessage == NULL, sending internal error to remote consumer!");
        retVal = SendErrorToConsumerL( KErrSenInternal, NULL, aTxnId, aConsumer, aResponseTransportProperties );
        }        
    return retVal;
    }

void CIdWsfServiceSession::SetEndPointL(const TDesC8& aURI)
    {
    CSenWebServiceSession::SetEndPointL(aURI);
    if(iProvider)
        {
        iProvider->SetEndPointL(aURI);
        }
    }

void CIdWsfServiceSession::SetFrameworkHeadersL(CSenSoapMessage& aMsg)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfServiceSession::SetFrameworkHeaders(CSenSoapMessage& aMsg)");

#ifdef _SENDEBUG
    if(ipSenSecurityMechanism)
        {
        TPtrC8 secMec = ipSenSecurityMechanism->Des();
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,"Security mechanism: \n");
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMaxLogLevel ,(secMec));
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel ,"Note: ipSenSecurityMechanism == NULL!");
        }

    if (ipSenSecurityMechanism &&
         (*ipSenSecurityMechanism).Compare(KSecTlsBearer11) != 0 &&
         (*ipSenSecurityMechanism).Compare(KSecNullBearer11) != 0 )
        {
       TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfServiceSession::SetFrameworkHeaders");
       TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ," - header for ID-WSF 1.0 needs to be used");
        }
    else
        {
       TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfServiceSession::SetFrameworkHeaders");
       TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ," - header for ID-WSF 1.1 needs to be used");
        }
#endif

    CIdWsfMessage& idWsfMessage = (CIdWsfMessage&)aMsg;
    idWsfMessage.SetSecurityMechanism(ipSenSecurityMechanism);
    CSenWebServiceSession::SetFrameworkHeadersL(aMsg);
    


        /*

    //Changes for IDWSF 1.1 support
    // Added the condition to add the old-style WsSecurity header 
    // for non-IDWSF11 security mechanisms

    if (ipSenSecurityMechanism &&
             (*ipSenSecurityMechanism).Compare(KSecTlsBearer11) != 0 &&
             (*ipSenSecurityMechanism).Compare(KSecNullBearer11) != 0 )
        {
           TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"CIdWsfServiceSession::SetFrameworkHeaders \
               - Setting header for ID-WSF 1.0");
          // Add new OLD WsSecurityHeader to msg

          
          CIdWsfSecurityHeader* pHeader = NULL;
          if(iSecurity)
            {
            pHeader = 
                CIdWsfSecurityHeader::NewL(*iSecurity); 
            }
          else
            {
            pHeader = 
                CIdWsfSecurityHeader::NewL(); 
            }
          aMsg.AddHeaderL( pHeader->AsElement() ) ;
          
          // Above is equal with next line, if an aMsg argument 
          // is actually SenIdWsfSoapMessage(!), ID-WSF 1.1, matmatt
          //CSenWebServiceSession::SetFrameworkHeaders(aMsg);
        }
       else // NOTE: now we default to 1.1 if !ipSenSecurityMechanism 
       {
           TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"CIdWsfServiceSession::SetFrameworkHeaders \
               - Setting header for ID-WSF 1.1");
            // We need to use (newer) security header for ID-WSF 1.1
            // this could be done in IdWsfSoapMessage class, if it would
            // somehow know in NewSecurityHeaderL() method, that it is
            // actually 1.0 or 1.1. But this current solution is rather
            // clear also. Each new header type, we need a new 
            // else-if code segment inside this session class method,
            // -> in here.
           CSenWsSecurityHeader* pHeader = NULL;

          if(iSecurity)
            {
            // ID-WSF 1.1, matmatt
            pHeader = 
                CSenWsSecurityHeader::NewL(*iSecurity); 
            }
          else
            {
            pHeader = 
                CSenWsSecurityHeader::NewL(); 
            }
           aMsg.AddHeaderL( pHeader->AsElement() ) ;
        }
       */

    // CorrelationHeader adjustment is already done in the SessionConsumer

    }

void CIdWsfServiceSession::WriteExtensionsAsXMLToL(RWriteStream& aWriteStream)
    {
    CSenWebServiceSession::WriteExtensionsAsXMLToL(aWriteStream);

    if(iProvider)
        {
        aWriteStream.WriteL(KProviderID);
        aWriteStream.WriteL(ProviderId());
        aWriteStream.WriteL(KProviderIDEnd);
        }

    if(ipResourceId)
        {
        aWriteStream.WriteL(KResourceID);
        aWriteStream.WriteL(*ipResourceId);
        aWriteStream.WriteL(KResourceIDEnd);
        }

    if(ipTrustAnchor)
        {
        aWriteStream.WriteL(KTrustAnchor);
        aWriteStream.WriteL(*ipTrustAnchor);
        aWriteStream.WriteL(KTrustAnchorEnd);
        }

    if(iClientServerInterval.Int64() != 0)
        {
        aWriteStream.WriteL(KServiceInterval);
        TBuf8<64> buf;
        buf.AppendNum(iClientServerInterval.Int64());
        aWriteStream.WriteL(buf);
        aWriteStream.WriteL(KServiceIntervalEnd);
        }
    }

// Note: no ref-impl for this method in Java either yet
TInt CIdWsfServiceSession::ConstructSecurityTokenL(
                                               const TDesC& /* aToken */,
                                               HBufC8*& /* aSecurityToken */)
    {
    return KErrNotSupported;
    }

// from service instance
// NOTE(!): returns KErrNotFound if no known security mechanism was found.
TInt CIdWsfServiceSession::InitializeFromL(
                                CIdWsfServiceInstance& aServiceInstance)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("CIdWsfServiceSession::InitializeFromL")));
    CSenWebServiceSession::SetSecurityL(KNullDesC8);
    CSenWebServiceSession::InitializeFromL(
        (MSenServiceDescription&)aServiceInstance);

    TPtrC8 providerid = aServiceInstance.ProviderId();
    if (providerid.Length()>0)
        {
        TPtrC8 endPoint = aServiceInstance.Endpoint();
        CIdWsfServiceProvider* pProvider = CIdWsfServiceProvider::NewL(providerid,
                                                endPoint);
        delete iProvider;
        iProvider = pProvider;
        }

    // the description should support one, and only one(!) of mechanisms:

    if(aServiceInstance.SupportsMechanism(KSecNullBearer))
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(KSecNullBearer()));
        delete ipSenSecurityMechanism;
        ipSenSecurityMechanism = KSecNullBearer().AllocL();
        }
    else if(aServiceInstance.SupportsMechanism(KSecTlsBearer))
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(KSecTlsBearer()));
        delete ipSenSecurityMechanism;
        ipSenSecurityMechanism = KSecTlsBearer().AllocL();
        }
    else if(aServiceInstance.SupportsMechanism(KSecTlsNull))
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(KSecTlsNull()));
        delete ipSenSecurityMechanism;
        ipSenSecurityMechanism = KSecTlsNull().AllocL();
        }
    else if(aServiceInstance.SupportsMechanism(KSecNullNull))
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(KSecNullNull()));
        delete ipSenSecurityMechanism;
        ipSenSecurityMechanism = KSecNullNull().AllocL();
        }

        //Added following two conditions for ID-WSF 1.1 support
    else if(aServiceInstance.SupportsMechanism(KSecNullBearer11)) 
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(KSecNullBearer11()));
        delete ipSenSecurityMechanism;
        ipSenSecurityMechanism = KSecNullBearer11().AllocL();
        }
    else if(aServiceInstance.SupportsMechanism(KSecTlsBearer11)) 
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(KSecTlsBearer11()));
        delete ipSenSecurityMechanism;
        ipSenSecurityMechanism = KSecTlsBearer11().AllocL();
        }
        
    if(!ipSenSecurityMechanism)
        {
        _LIT(KInvalidDate,"19000101:");	//CodeScannerWarnigs
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(_L("FATAL ERROR in CIdWsfServiceSession::InitializeFromL: \
            ipSenSecurityMechanism == NULL!")));

        // invalidate the session, because we don't know any mechanism
        // and we have no credential 
        iValidUntil.Set(KInvalidDate); // way back in history: January 1st 1900 // CodeScannerWarnig
        SetStatusL();
        return KErrNotFound;
        }

    if(IsBearerL(*ipSenSecurityMechanism) || IsSAMLL(*ipSenSecurityMechanism))
        {
        TPtrC8 endpoint = Endpoint();
        if(endpoint.Length() > 0)
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ," Adding credential, endpoint is:");
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,(endpoint));
            }
        else
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ," Endpoint <not set>, adding credential.");
            }

        CSenCredential* pCredential = (CSenCredential *) aServiceInstance.Credential();
                                    
        if(pCredential)
            {
            /*
            if(IsBearerL(*ipSenSecurityMechanism))
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"Sun does not send validUntil, so let's trust Bearer mech creds valid. Service will tell when they do expire");
                iValidUntil.Set(_L("22000101:"));
                pCredential->SetValidUntil(iValidUntil);
                }
            */
            AddCredentialL(*pCredential);
            }
        }

    TPtrC8 resourceId = aServiceInstance.ResourceId();
    HBufC8* pResourceId = resourceId.AllocL();
    delete ipResourceId;
    ipResourceId = pResourceId;
    SetOptionsFromL(aServiceInstance);

    /*
    if(IsBearerL(*ipSenSecurityMechanism))
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"Sun does not send validUntil, so let's trus Bearer mech creds valid. Service will tell when they do expire");
        iValidUntil.Set(_L("22000101:")); // way back in history: January 1st 1900
        }
    */
    SetStatusL();

    return KErrNone;
    }

TBool CIdWsfServiceSession::IsSAMLL(const TDesC8& aSecMechURI)
    {
    TInt index(KErrNotFound);

    index = aSecMechURI.Find(KSAML);

    TInt rightPosition = aSecMechURI.Length()-KSAML().Length();

    if(rightPosition > 0 && index == rightPosition)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

TBool CIdWsfServiceSession::IsBearerL(const TDesC8& aSecMechURI)
    {
    TInt index(KErrNotFound);

    index = aSecMechURI.Find(KBEARER);

    TInt rightPosition = aSecMechURI.Length()-KBEARER().Length();

    if(rightPosition > 0 && index == rightPosition)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

TPtrC8 CIdWsfServiceSession::ProviderId()
    {
    if(iProvider) 
        {
        return iProvider->ProviderId();
        }
    else
        {
        return KNullDesC8();
        }
    }

TPtrC8 CIdWsfServiceSession::ServiceType()
    {
    return CSenWebServiceSession::Contract();
    }

TInt CIdWsfServiceSession::UpdateFromL(CIdWsfSiuHeader& aHeader)
    {
    TInt retVal = KErrNone;

    if (aHeader.Endpoint().Length()>0)
        {
        TPtrC8 endpoint = aHeader.Endpoint();
        SetEndPointL(endpoint);
        }

    if (aHeader.SecurityMechId().Length()>0)
        {
        TPtrC8 securityMechId = aHeader.SecurityMechId();
        SetSenSecurityMechanismL(securityMechId);
        }

    if (aHeader.Credential())
        {
        CSenWebServiceSession::SetSecurityL(KNullDesC8());
        retVal = AddCredentialL(*aHeader.Credential());
        }

    SaveL();

    return retVal;
    }

void CIdWsfServiceSession::SetProviderL(CIdWsfServiceProvider *aProvider)
    {
    delete iProvider;
    iProvider = aProvider;
    CSenWebServiceSession::SetEndPointL(aProvider->Endpoint());
    }

CIdWsfServiceProvider* CIdWsfServiceSession::Provider()
    {
    return iProvider;
    }

void CIdWsfServiceSession::SetResourceIdL(const TDesC8& aURI)
    {
    HBufC8* pNew = NULL;
    if(aURI.Length()>0)
        pNew = aURI.AllocL();
    delete ipResourceId;
    ipResourceId = pNew;
    }

TPtrC8 CIdWsfServiceSession::ResourceId()
    {
    if(ipResourceId)
        return *ipResourceId;
    else
        return KNullDesC8();
    }

void CIdWsfServiceSession::SetServiceTypeL(const TDesC8& aURI)
    {
    if(aURI.Length()>0)
        {
        CSenWebServiceSession::SetContractL(aURI);
        }
    else
        {
        CSenWebServiceSession::SetContractL(KNullDesC8());
        }
    }

void CIdWsfServiceSession::SetTrustAnchorL(const TDesC8& aURI)
    {

    HBufC8* pNew = NULL;
    if(aURI.Length()>0)
        {
        pNew = aURI.AllocL();
        }
    delete ipTrustAnchor;
    ipTrustAnchor = pNew;
    }
TPtrC8 CIdWsfServiceSession::TrustAnchor()
    {
    if(ipTrustAnchor)
        return *ipTrustAnchor;
    else
        return KNullDesC8();
    }

void CIdWsfServiceSession::SetSenSecurityMechanismL(
                                        const TDesC8& aSenSecurityMechanism)
    {

    HBufC8* pNew = NULL;
    if(aSenSecurityMechanism.Length()>0)
        pNew = aSenSecurityMechanism.AllocL();

    delete ipSenSecurityMechanism;
    ipSenSecurityMechanism = pNew;
    }

TPtrC8 CIdWsfServiceSession::SenSecurityMechanism()
    {
    if(ipSenSecurityMechanism)
        {
        return *ipSenSecurityMechanism;
        }
    else
        {
        return KNullDesC8();
        }
    }

// Override the method from SenServiceSession to provide simple search from
// list, because ID-WSF service sessions may have several consumers.
MSenRemoteServiceConsumer* CIdWsfServiceSession::RemoteConsumerL(
        const TDesC8& aSenderID)
    {
    TInt consumersCount(iConsumerList.Count());

    for(TInt i=0; i<consumersCount; i++)
        {
        if(iConsumerList[i]->Id() == aSenderID)
            {
            return iConsumerList[i];
            }
        }
    return NULL; // not found
    }


TInt CIdWsfServiceSession::SessionConsumerL(
                            CIdWsfMessage& aMessage,
                            CIdWsfSessionConsumer*& aSessionConsumer)
    {
    TInt consumersCount(iConsumerList.Count());

    for(TInt i=0; i<consumersCount; i++)
        {
        CIdWsfSessionConsumer* pConsumer =
                            (CIdWsfSessionConsumer*) iConsumerList[i];
        if (pConsumer->Expects(aMessage.RefToMessageId()))
            {
            aSessionConsumer = pConsumer;
            return KErrNone;
            }
        }

    aSessionConsumer = NULL; // not found
    return KErrNotFound;
    }

TInt CIdWsfServiceSession::SessionConsumerL(const TDesC8& aSenderID,
                                    CIdWsfSessionConsumer*& aSessionConsumer)
    {
    aSessionConsumer = (CIdWsfSessionConsumer*) RemoteConsumerL(aSenderID);
    if(aSessionConsumer)
        {
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

// SYNC IMPLEMENTATION(!)
// aSOAPMessage.IsFault() should always be TRUE.

/**
* Method checks if ID-WSF framework can handle this fault, if not, then
* error is delivered to session consumer
*/
TInt CIdWsfServiceSession::HandleSoapFaultL(CSenSoapMessage* apSOAPMessage,
                                            HBufC8*& aResponse)
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfServiceSession::HandleSoapFaultL [sync]");
    
    TInt retVal(KErrNone);
    
    if ( apSOAPMessage )
    	{
    	 
	    CIdWsfMessage& idWsfMessage = (CIdWsfMessage&)*apSOAPMessage;
		CleanupStack::PushL( apSOAPMessage ); // ownerhip is here 
		
	    TInt answer = CanHandleErrorL(idWsfMessage);
	    
	    if(answer == KErrSenReinitRequired) // ESenReAuthAndResendNeeded
	        {
	        TBool status(EFalse);
	        SetValidator(NULL);
	        iFramework.Manager().NotifyFrameworksL(
	            KDefaultIdWsfFrameworkID,
	            KSenEventIdWsfSessionValidatorRequested);

	        MIdWsfSessionValidator* pValidator = Validator();
	        if (pValidator)
	            {
	            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel ,"* validator is available *");
	            TInt retVal(KErrNone);
	            iSubmitState = KSubmitStateInvalidCredentials;
	            // invalidate the current session
	            SetStatusL();
	            iSubmitState = KSubmitStateRevalidationAttempted;
	            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L8("Validator ptr: 0x%x"), pValidator));
	            
	            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"Looking for session consumer.");
	            CIdWsfSessionConsumer* pConsumer = NULL;
	    
	            // this takes care of messageIDs
	            TInt getConsumerRetVal = SessionConsumerL(idWsfMessage, pConsumer);
	            if(getConsumerRetVal != KErrNone) 
	                {
	                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,
	                    _L8(" - SessionConsumerL returned an error: %d"), 
	                    getConsumerRetVal));
	                status = EFalse;
	                }
	            else if(!pConsumer)
	                {
	                // consumer not found
	                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ," - Consumer not found!");
	                status = EFalse;
	                }                   
	            else
	                {
	                TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,_L8("Revalidating needed")));
	                retVal = pValidator->ValidateL(*this, *pConsumer); // this actually can call either ASC or DSC
	                status = (retVal == KErrNone) && IsReadyL();
	                }
	            }
	        
	#ifdef _SENDEBUG
	        if(status)
	            {
	            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"HandleErrorL: InappropriateCredentials -> TRUE");
	            }
	        else
	            {
	            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"HandleErrorL: InappropriateCredentials -> FALSE");
	            }
	#endif
	        if( !status ) 
	            {
	            // notify consumers (applications) about expired connection
	            SetStatusL();
	            answer = KErrNone;
	            // Change state to "re-validation failed" in order to indicate other, 
	            // pending async messages  that there is no use re-sending as one lacks 
	            // valid credential(s)
	            iSubmitState = KSubmitStateRevalidationFailed;
	            }
	        }

	    if (iSubmitState != KSubmitStateRevalidationFailed && ( answer > KErrNone || answer == KErrSenReinitRequired || answer == KErrSenResendRequired ))
	        {
	        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CanHandleErrorL == TRUE");

	        CIdWsfSessionConsumer* pConsumer = NULL;

	        // this takes care of messageIDs
	        retVal = SessionConsumerL(idWsfMessage, pConsumer);
	        if(retVal != KErrNone) 
	            {
	            CleanupStack::PopAndDestroy( apSOAPMessage ); // not needed 
	            return retVal;
	            }
	        else if(!pConsumer)
	            {
	            CleanupStack::PopAndDestroy( apSOAPMessage ); // not needed 
	            // consumer not found
	            return KErrNotFound;
	            }
	        
	        TDesC8* pBody = pConsumer->BodyL(idWsfMessage.RefToMessageId());
	        if (pBody)
	            {
	            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"Re-submitting the request");
	            retVal = SubmitL(*pBody, KNullDesC8, *pConsumer, aResponse);
	            }
	        }
	    else 
	        {
	        CSenSoapFault* pDetached = idWsfMessage.DetachFaultL();
	        if(pDetached)
	            {
	            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"Detached a SOAP fault");
	            CleanupStack::PushL(pDetached);
	            aResponse = pDetached->AsXmlL();
	            CleanupStack::PopAndDestroy(); // pDetached
	            CleanupStack::PopAndDestroy( apSOAPMessage ); // not needed 
	            return KErrSenSoapFault; // this is 2nd OK return code!
	            }
	        else
	            {
	            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"No SOAP fault was received.");
	            retVal = KErrSenInternal;
	            }
	        }
	    CleanupStack::PopAndDestroy( apSOAPMessage ); // not needed 
	    }
	    
	return retVal;
    }

/**
* Method checks if ID-WSF framework can handle this fault, if not, then
* error is delivered to session consumer
*/
TInt CIdWsfServiceSession::HandleSoapFaultL(CSenSoapMessage* apSOAPMessage,
                                            const TInt aErrorCode,
                                            const TInt aTxnId,
                                            MSenRemoteServiceConsumer& aConsumer,
                                            MSenProperties* aResponseTransportProperties) 
    {
    CSLOG_L(aConsumer.ConnectionId() ,KMinLogLevel ,"CIdWsfServiceSession::HandleSoapFaultL [async]");
    //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"CIdWsfServiceSession::HandleSoapFaultL [async]");

    TInt retVal(KErrNone);
    
    if ( apSOAPMessage )
        {
	    CIdWsfMessage& idWsfMessage = (CIdWsfMessage&)*apSOAPMessage;
		CleanupStack::PushL( apSOAPMessage ); // ownerhip is here 

	    TInt answer = CanHandleErrorL(idWsfMessage);
	    if ( answer > KErrNone || answer == KErrSenReinitRequired || answer == KErrSenResendRequired )
	        {
	        CSLOG_L(aConsumer.ConnectionId(), KNormalLogLevel ,"CanHandleErrorL == TRUE");
	        //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"CanHandleErrorL == TRUE");
	        
	        aConsumer.HandleErrorL(NULL, answer, aTxnId, aResponseTransportProperties);
	        }
	    else
	        {
	        CSenSoapFault* pDetached = idWsfMessage.DetachFaultL();
	        if(pDetached)
	            {
	            CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"Detached a SOAP fault");
	            //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"Detached a SOAP fault");
	            CleanupStack::PushL(pDetached);
	            // pass KErrSenSoapFault // which is 2nd "OK" return code!
	            HBufC8* pAsXml = pDetached->AsXmlL();
	            if(pAsXml)
	                {
	                aConsumer.HandleErrorL(pAsXml, KErrSenSoapFault, aTxnId, aResponseTransportProperties);
	                }
	            CleanupStack::PopAndDestroy(); // pDetached
	            retVal = KErrNone; // OK
	            }
	        else
	            {
	            CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"No SOAP fault was received.");
	            //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"No SOAP fault was received.");
	            aConsumer.HandleErrorL(NULL, aErrorCode, aTxnId, aResponseTransportProperties);
	            retVal = KErrSenInternal; // one *could* pass on the orig error code
	            }
        	}
		CleanupStack::PopAndDestroy( apSOAPMessage ); // not needed 
        }
        
    return retVal;
    }
    
TInt CIdWsfServiceSession::CanHandleErrorL(CIdWsfMessage& aMessage)
    {
    TInt answer(KErrNone);

    const TDesC8* pCode = aMessage.StatusCodeL();

    if(pCode)
        {
        if(SenXmlUtils::EndsWith(*pCode, KStatusEndpointMoved))
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CanHandleErrorL: Endpoint moved -> TRUE");
            // We could probably re-submit the body of our outgoing message
            // check if we received a SIU header with an endpoint
            CIdWsfSiuHeader* pSIUHeader = aMessage.SiuHeader();
            if(pSIUHeader && pSIUHeader->Endpoint().Length() > 0)
                {
                answer = KErrSenResendRequired; // ESenResendNeeded;
                }
            }
        else if(SenXmlUtils::EndsWith(*pCode, KStatusInappropriateCredentials))
            {
            _LIT(KInvalidDate,"18000101:");
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CanHandleErrorL: InappropriateCredentials -> invalidating this session");
            // ensure that session is CERTAINLY INVALID!
            TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,
                _L8("Credentials expired. Handling SOAP fault %S"),
                &KStatusInappropriateCredentials() ));

            iValidUntil.Set(KInvalidDate); // way way back in history: January 1st 1800 //CodescannerWarnings


            if (iSubmitState == KSubmitStateOK)
                {
                // Clear the current validator
                SetValidator(NULL);
                iFramework.Manager().NotifyFrameworksL(
                    KDefaultIdWsfFrameworkID,
                    KSenEventIdWsfSessionValidatorRequested);

                MIdWsfSessionValidator* pValidator = Validator();
                if (pValidator)
                    {
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel ,"* validator is available *");
                    iSubmitState = KSubmitStateInvalidCredentials;
                    // Invalidate the current session
                    SetStatusL();
                    iSubmitState = KSubmitStateRevalidationAttempted;
                    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KNormalLogLevel  ,_L8("Validator ptr: 0x%x"), pValidator));
                    
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"Looking for session consumer.");
                    CIdWsfSessionConsumer* pConsumer = NULL;
            
                    // This takes care of messageIDs
                    TInt getConsumerRetVal = SessionConsumerL(aMessage, pConsumer);
                    if(getConsumerRetVal != KErrNone) 
                        {
                        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,
                            _L8(" - SessionConsumerL returned an error: %d"), 
                            getConsumerRetVal));
                        answer = KErrNone;
                        }
                    else if(!pConsumer)
                        {
                        // Consumer not found
                        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ," - Consumer not found!");
                        answer = KErrNone;
                        }                   
                    else
                        {
                        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase   , KMinLogLevel  ,_L8("- Revalidation needed")));
                        answer = KErrSenReinitRequired; // ESenReAuthAndResendNeeded;
                        }
                    }

                }
            else if ( iSubmitState == KSubmitStateRevalidationAttempted )
                {
                // Another message already triggered the re-validation phase, 
                // so: this operation requires re-sending only
                answer = KErrSenResendRequired; // ESenResendNeeded; 
                iSubmitState = KSubmitStateRevalidationFailed; //ESenResend
                }
			//else if ( iSubmitState == KSubmitSateResend )  
			//	{
			//	answer = KErrSenResendComplete; //ESenResendFailed
			//	iSubmitState = KSubmitSateResendFailed; //ESenResendFailed
            //    }
            }
        }

    return answer;
    }

TBool CIdWsfServiceSession::Matches(MSenServiceDescription& aPattern)
    {
    TBool matches = CSenWebServiceSession::Matches(aPattern);

    // for now the only interesting facet is the messageThread facet
    // if this session uses messageThreads it should not match any
    // description, as it essentially makes the session unique
    // (unless nobody is using this session).
    if (iMessageThread && HasConsumer())
        {
        // a new session is needed in any case
        matches = EFalse;
        }
    else
        {
        TBool hasFacet(EFalse);
        TInt leaveCode(KErrNone);
        TRAP( leaveCode, aPattern.HasFacetL(KMessageThread, hasFacet); )
        if (leaveCode != KErrNone) 
            {
            TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel ,"CIdWsfServiceSession::Matches - HasFacetL() leaved!");
            matches = EFalse;
            }
        if (hasFacet)
            {
            if (matches && !HasConsumer()) matches = ETrue;
            else matches = EFalse;
            }
        }
    return matches;
    }

void CIdWsfServiceSession::CopyFacetsFromL(MSenServiceDescription& aPattern)
    {
    aPattern.HasFacetL(KMessageThread,iMessageThread);

    if (iMessageThread)
        {
        CSenFacet* pNewFacet = CSenFacet::NewL();
        CleanupStack::PushL(pNewFacet);
        pNewFacet->SetValueL(KSenFacetValTrue);
        SetFacetL(*pNewFacet);
        CleanupStack::Pop(); // pNewFacet
        }
    else
        {
        RemoveFacet(KMessageThread);
        }
    }

TInt CIdWsfServiceSession::SetOptionsFromL(MSenServiceDescription& aPattern)
    {
    RFacetArray options;
    CleanupClosePushL(options);

    CIdWsfDiscoveryServiceClient::DSOptionsL(options, aPattern);
    if (options.Count() > 0)
        {
        TInt count(options.Count());
        for (TInt i=0; i<count; i++)
            {
            SetFacetL(*options[i]);
            }
        }
    options.ResetAndDestroy();
    CleanupStack::Pop(); //options

    return KErrNone;
    }

void CIdWsfServiceSession::StartTransaction()
    {
    iMessageThread = ETrue;
    }

void CIdWsfServiceSession::TransactionCompleted()
    {
    delete ipReceivedMessageId;
    ipReceivedMessageId = NULL;
    }

void CIdWsfServiceSession::SetValidator(MIdWsfSessionValidator* aValidator)
    {
    ipValidator = aValidator;
    }

MIdWsfSessionValidator* CIdWsfServiceSession::Validator()
    {
    return ipValidator;
    }

TInt CIdWsfServiceSession::SendL( const TDesC8& aMessage, 
                                  const TDesC8& aTransportProperties,
                                  MSenRemoteServiceConsumer& aConsumer,
                                  TInt& aTxnId,
                                  HBufC8*& aRevalidationError )
    {
    CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CIdWsfServiceSession::SendL");
    //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"CIdWsfServiceSession::SendL");
    // Temporary START
    MSenTransport& transport = aConsumer.TransportL();
    SetFrameworkPropertiesL( transport ); // retVal surpressed
    // Temporary END
    TInt retVal = CSenWebServiceSession::SendL( aMessage, aTransportProperties, aConsumer, aTxnId, aRevalidationError );
    CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CIdWsfServiceSession::SendL:");
    //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"CIdWsfServiceSession::SendL:");
    CSLOG_FORMAT((aConsumer.ConnectionId(), KNormalLogLevel , _L8("- Return value from CSenWebServiceSession::SendL %d: "), retVal));
    //TLSLOG_FORMAT(KSenCoreServiceManagerLogChannelBase   , KSenCoreServiceManagerLogLevel  ,(_L8("- Return value from CSenWebServiceSession::SendL %d: "), retVal));
    CSLOG_FORMAT((aConsumer.ConnectionId(), KNormalLogLevel , _L8("- Transaction ID (from transport): %d"), aTxnId));
    //TLSLOG_FORMAT(KSenCoreServiceManagerLogChannelBase   , KSenCoreServiceManagerLogLevel  ,(_L8("- Transaction ID (from transport): %d"), aTxnId));
    return retVal;
    }

TInt CIdWsfServiceSession::SubmitL( const TDesC8& aMessage, 
                                    const TDesC8& aTransportProperties,
                                    MSenRemoteServiceConsumer& aConsumer,
                                    HBufC8*& aResponse )
    {
    CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CIdWsfServiceSession::SubmitL");
    //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"CIdWsfServiceSession::SubmitL");
    MSenTransport& transport = aConsumer.TransportL();
    SetFrameworkPropertiesL(transport); // retVal surpressed
    TInt retVal = CSenWebServiceSession::SubmitL( aMessage, aTransportProperties, aConsumer, aResponse);
    if (retVal == KErrNone)
        {
        iSubmitState = KSubmitStateOK; 
        }
    return retVal;
    }

TInt CIdWsfServiceSession::SetStatusL()
    {
    // check if we are re-submitting
    if ( iSubmitState == KSubmitStateInvalidCredentials )
        {
        TInt count(iConsumerList.Count());

        // Create a local array to hold pointer(s) to consumer(s)
        RServiceConsumerArray consumerList;
        CleanupClosePushL(consumerList);

        // Copy the consumers into temporary array
        for (TInt i=0; i<count; i++)
            {
            TInt errorCode = consumerList.Append(iConsumerList[i]);
            if(errorCode!=KErrNone)
                {
                User::Leave(errorCode); // out-of-memory occurred
                }
            }
        // Clear the actual array to prevent false
        // callbacks to SetStatusL() etc:
        iConsumerList.Reset();

        // Now it is safe to calculate the status
        TInt retVal = CSenWebServiceSession::SetStatusL();

        // The iConsumerList may be populated with original 
        // consumer(s)
        for ( TInt j = 0; j < count; j++ )
            {
            iConsumerList.Append(consumerList[j]);
            }
        
        // Reset the temporary array and let it go out of scope
        consumerList.Reset();
        CleanupStack::Pop(); //consumerList
        return retVal;
        } 

    // Note: call to SetStatusL() can always result consumers being
    // notified of their state changed
    return CSenWebServiceSession::SetStatusL();
    }
    
/*
* ID-WSF always expect that consumer application  is providing the application body only.
* So, we are extracting the Soap body from aSoapMessage and SendL of
* ID-WSF has been called
*/
TInt CIdWsfServiceSession::SendSoapL( const TDesC8& aSoapMessage, 
                                      const TDesC8& aTransportProperties,
                                      MSenRemoteServiceConsumer& aConsumer,
                                      TInt& aTxnId,
                                      HBufC8*& aRevalidationError )
    {
    CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CIdWsfServiceSession::SendSoapL(TDesC8&, ...)");
    //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"CIdWsfServiceSession::SendSoapL(TDesC8&, ...)");
	
    TInt retVal(KErrNone);    	
    CSenXmlReader* pReader = iFramework.Manager().XMLReader(); // re-use the XML reader instance owned by core 
    CSenSoapMessage* pMsg = CSenSoapMessage::NewL();
    CleanupStack::PushL( pMsg );
    pMsg->SetReader( *pReader ); 
    pReader->SetContentHandler( *pMsg );
    TInt leaveCode( KErrNone );
    TRAP( leaveCode,  pMsg->ParseL( aSoapMessage ); )
    if ( leaveCode )
        {
        CSLOG_FORMAT((aConsumer.ConnectionId(), KMinLogLevel , _L8("- Parsing descriptor into SOAP-MSG object failed, leave code: %d"), leaveCode ));
        //TLSLOG_FORMAT(KSenCoreServiceManagerLogChannelBase   , KSenCoreServiceManagerLogLevel  ,( _L("- Parsing descriptor into SOAP-MSG object failed, leave code: %d"), leaveCode ));
        CleanupStack::PopAndDestroy( pMsg ); // de-alloc the SOAP-MSG object immediately
	  retVal = leaveCode;
        }
    else // parsing ok
        { 
        CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"- Successfully parsed descriptor into SOAP-MSG.");
        //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"- Successfully parsed descriptor into SOAP-MSG.");
        HBufC8* pHttpBody = pMsg->BodyAsStringL(); // extract the SOAP body
        CleanupStack::PopAndDestroy( pMsg );
        CleanupStack::PushL( pHttpBody );
        retVal = SendL( *pHttpBody, aTransportProperties, aConsumer, aTxnId, aRevalidationError );    
        CleanupStack::PopAndDestroy( pHttpBody ); // assert in proper order
        }	
    return retVal;
    }

/*
* ID-WSF always expect that consumer application  is providing the application body only.
* So, we are extracting the Soap body from aSoapMessage and SubmitL of 
* ID-WSF has been called
*/
TInt CIdWsfServiceSession::SubmitSoapL( const TDesC8& aSoapMessage, 
                                        const TDesC8& aTransportProperties,
                                        MSenRemoteServiceConsumer& aConsumer,
                                        HBufC8*& aResponse )
    {
#ifdef _SENDEBUG
    CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"CIdWsfServiceSession::SubmitSoapL(TDesC8, ...");
    //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"CIdWsfServiceSession::SubmitSoapL(TDesC8, ...");
    CSLOG_L(aConsumer.ConnectionId(), KMaxLogLevel ,"///////////////////////////////////////////////////////");
    //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"///////////////////////////////////////////////////////");
    CSLOG_FORMAT((aConsumer.ConnectionId(), KMaxLogLevel , _L8("- Message (%d bytes):"), aSoapMessage.Length()));
    //TLSLOG_FORMAT(KSenCoreServiceManagerLogChannelBase   , KSenCoreServiceManagerLogLevel  ,(_L8("- Message (%d bytes):"), aSoapMessage.Length()));
    CSLOG_ALL(aConsumer.ConnectionId(), KMaxLogLevel ,(aSoapMessage));
    //TLSLOG_ALL(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,(aSoapMessage));
    CSLOG_L(aConsumer.ConnectionId(), KMaxLogLevel ,"///////////////////////////////////////////////////////");
    //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"///////////////////////////////////////////////////////");
#endif
    TInt retVal(KErrNone);    	
    CSenXmlReader* pReader = iFramework.Manager().XMLReader(); // re-use the XML reader instance owned by core 
    CSenSoapMessage* pMsg = CSenSoapMessage::NewL();
    CleanupStack::PushL( pMsg );
    pMsg->SetReader( *pReader ); 
    pReader->SetContentHandler( *pMsg );
    TInt leaveCode( KErrNone );
    TRAP( leaveCode,  pMsg->ParseL( aSoapMessage ); ) 
    if ( leaveCode )
        {
        CSLOG_FORMAT((aConsumer.ConnectionId(), KMinLogLevel , _L8("- Parsing descriptor into SOAP-MSG object failed, leave code: %d"), leaveCode ));
        //TLSLOG_FORMAT(KSenCoreServiceManagerLogChannelBase   , KSenCoreServiceManagerLogLevel  ,( _L("- Parsing descriptor into SOAP-MSG object failed, leave code: %d"), leaveCode ));
        CleanupStack::PopAndDestroy( pMsg ); // de-alloc the SOAP-MSG object immediately
	  retVal = leaveCode;
        }
    else // parsing ok
        { 
        CSLOG_L(aConsumer.ConnectionId(), KMinLogLevel ,"- Successfully parsed descriptor into SOAP-MSG.");
        //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel ,"- Successfully parsed descriptor into SOAP-MSG.");
        HBufC8* pHttpBody = pMsg->BodyAsStringL(); //Extracting the body
        CleanupStack::PopAndDestroy( pMsg );
        CleanupStack::PushL( pHttpBody );
        retVal = SubmitL(*pHttpBody, aTransportProperties, aConsumer, aResponse);    
        CleanupStack::PopAndDestroy( pHttpBody ); // assert proper order
        }
    return retVal;
    }
    

TBool CIdWsfServiceSession::HasSuperClass( TDescriptionClassType aType )
    {
    if ( aType == MSenServiceDescription::EWebServiceSession ) // direct superclass!    
    {
        // If asked type is the know *direct* father/mother, return true:
        return ETrue; 
    } 
    else
        {
        // Otherwise, ask from superclass (chain, recursively)
        return CSenWebServiceSession::HasSuperClass( aType ); 
        }
    }

// Temporary
TInt CIdWsfServiceSession::SetFrameworkPropertiesL(MSenTransport& aTransport)
    {
    return iFramework.SetTransportPropertiesL(aTransport);
    }

void CIdWsfServiceSession::FillCredentialIdentifierL(CSenCredentialIdentifier& aIdentifier)
    {
    CSenWebServiceSession::FillCredentialIdentifierL(aIdentifier);

    _LIT8(KProviderIdElementLocalName,  "ProviderID");
    aIdentifier.SetPropertyL(KProviderIdElementLocalName, ProviderId());
    }    


// End of file


