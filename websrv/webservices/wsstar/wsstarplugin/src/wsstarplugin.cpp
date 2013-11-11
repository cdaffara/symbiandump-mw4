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
* Description:           
*
*/












#include "wsstarplugin.h"
#include "SenServiceConnection.h" // framework IDs, error codes
#include "sendebug.h"
#include "senlogger.h"
#include "MSenServiceDescription.h"
#include "wsstarservicesession.h"
#include "SenXmlUtils.h"
#include "wsstarpolicy.h"
#include "senguidgen.h"

#include "wsstarpolicyhandler.h"
using namespace WSStarConfig;

// Local CONSTANTS
const TInt KMaxLengthXml  = 1280; 

// ---------------------------------------------------------------------------
// Create instance of concrete ECOM interface implementation
// ---------------------------------------------------------------------------
//
CWSStarPlugin* CWSStarPlugin::NewL(TAny* aManager)
    {
    MSenCoreServiceManager* manager =
        reinterpret_cast<MSenCoreServiceManager*>(aManager);

    CWSStarPlugin* self = new (ELeave) CWSStarPlugin(*manager);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CWSStarPlugin::CWSStarPlugin(MSenCoreServiceManager& aManager)
    : iManager(aManager),
      iVersions(ETrue, ETrue),
      iDbReadDone(EFalse)
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CWSStarPlugin::~CWSStarPlugin()
    {
    iMsgHandlers.ResetAndDestroy();
    iSessionHandlers.ResetAndDestroy();
    iHandlerContexts.ResetAndDestroy();
    iVersions.Reset();
    delete iDeviceId;
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::~CWSStarPlugin()")));
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CWSStarPlugin::ConstructL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::ConstructL")));

    BaseConstructL(KFrameworkXMLNS, KFrameworkLocalName);
    GenerateDeviceIdL();
    //load default setup of handlers
    AddHandlerL(KValidateValue, KNullDesC8, SenHandler::ESessionHandler);//old http://schemas.xmlsoap.org/ws/2004/04/trust
    AddHandlerL(KRegisterValue, KNullDesC8, SenHandler::ESessionHandler);
    AddHandlerL(KServiceUpdateValue, KNullDesC8, SenHandler::ESessionHandler);//new http://schemas.xmlsoap.org/ws/2004/08/addressing
    
    AddHandlerL(KEnveloperValue, KNullDesC8, SenHandler::EMessageHandler);//old http://schemas.xmlsoap.org/soap/envelope
    AddHandlerL(KCredentialCollectorValue, KNullDesC8, SenHandler::EMessageHandler);
    AddHandlerL(KAddressingValue, KNullDesC8, SenHandler::EMessageHandler);//new http://schemas.xmlsoap.org/ws/2004/08/addressing
    AddHandlerL(KPassportValue, KNullDesC8, SenHandler::EMessageHandler);
    AddHandlerL(KSecurityValue, KNullDesC8, SenHandler::EMessageHandler);
    AddHandlerL(KPolicyValue, KNullDesC8, SenHandler::ESessionHandler); //Initalize policy
#ifdef RD_SEN_VTCP_SUPPORT        
    AddHandlerL(KCoBrandingValue, KNullDesC8, SenHandler::ESessionHandler); //Initalize co-branding
#endif//RD_SEN_VTCP_SUPPORT	
    }

// ---------------------------------------------------------------------------
// Getter for logger. Works only in debug mode.
// ---------------------------------------------------------------------------
//
/*RFileLogger* CWSStarPlugin::Log() const
    {
    return iManager.Log();
    }
*/

//========================================================
//                MSIF SPecific
//========================================================

//---------------------------------------------------------------------------
// Attempt to register the ServiceDescription to the ServiceManager 
//---------------------------------------------------------------------------
//
TInt CWSStarPlugin::RegisterServiceDescriptionL(
    MSenServiceDescription& aServiceDescription )
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::RegisterServiceDescriptionL(.)")));
    TInt error(KErrNone);

    CWSStarPolicyHandler* pPolicyHandler = (CWSStarPolicyHandler*)Handler(WSStarConfig::KPolicyValue);    
    CWSStarSessionContext* pSessionRegisterCtx = CWSStarSessionContext::NewLC(Manager().XMLReader(), &aServiceDescription, pPolicyHandler);
    pSessionRegisterCtx->Add(WSStarContextKeys::KServiceDescription(),
            (CSenWSDescription*)&aServiceDescription);
    pSessionRegisterCtx->Add(WSStarContextKeys::KRegisterAction(),
            WSStarContextValues::KActionRegister());
//policy initalization    
    TPtrC8 contract = aServiceDescription.Contract();
    TRAPD(policyError, pPolicyHandler->InvokeL(contract));
    if (policyError!=KErrNone)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::RegisterServiceDescriptionL() !!!!leave from PolicyHandler")));
        }

//SOA for S60 Composable Handler Framework v0.1 - 7.5.3.1 point 1)            
    CSenSessionHandler* pregisterHandler = 
                (CSenSessionHandler*)Handler(KRegisterValue());
        
    TRAPD(errorL,error= pregisterHandler->InvokeL(*pSessionRegisterCtx));
    if (errorL!=KErrNone)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::RegisterServiceDescriptionL() !!!!leave from RegisterHandler")));
        error = errorL;
        }
    CleanupStack::PopAndDestroy(pSessionRegisterCtx);
    return error;
    }

//---------------------------------------------------------------------------
// Attempt to unregister the ServiceDescription from the ServiceManager 
//---------------------------------------------------------------------------
//
TInt CWSStarPlugin::UnregisterServiceDescriptionL(
    MSenServiceDescription& aServiceDescription)
    {
    
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::UnRegisterServiceDescriptionL(.)")));
//SOA for S60 Composable Handler Framework v0.1  - 7.5.3.1 point 1)            
    TInt error(KErrNone);
    CSenSessionHandler* pregisterHandler = 
                (CSenSessionHandler*)Handler(KRegisterValue());
        
    CWSStarSessionContext* pSessionRegisterCtx = CWSStarSessionContext::NewLC(Manager().XMLReader(), &aServiceDescription);
    pSessionRegisterCtx->Add(WSStarContextKeys::KServiceDescription(),
            (CSenWSDescription*)&aServiceDescription);
    pSessionRegisterCtx->Add(WSStarContextKeys::KRegisterAction(),
            WSStarContextValues::KActionUnregister());
    CWSStarPolicyHandler* pPolicyHandler = (CWSStarPolicyHandler*)Handler(WSStarConfig::KPolicyValue);    
    TRAPD(policyError, pPolicyHandler->UnRegisterWsPolicyL(aServiceDescription));
    if (policyError!=KErrNone)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::UnRegisterServiceDescriptionL() !!!!leave from PolicyHandler")));
        }
        
    TRAPD(errorL,error = pregisterHandler->InvokeL(*pSessionRegisterCtx));
    if (errorL!=KErrNone)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::UnregisterServiceDescriptionL() !!!!leave from RegisterHandler")));
        error = errorL;
        }
    CleanupStack::PopAndDestroy(pSessionRegisterCtx);
    return error;
    }

//---------------------------------------------------------------------------
// Getter of framework (like "WS-STAR")
//---------------------------------------------------------------------------
//
const TDesC8& CWSStarPlugin::Id()
    {
    return KDefaultWSStarFrameworkID();
    }

//---------------------------------------------------------------------------
// Try to find one or more services that match the given ServiceDescription.
//    Adding is only invoked if NO! session in cache. 
//    Its mean that we have create new one.
//---------------------------------------------------------------------------
//
TInt CWSStarPlugin::AddServiceDescriptionL( MSenServiceDescription& aPattern,
                                            MSenRemoteServiceConsumer& aRemoteConsumer,
                                            HBufC8*& aErrorMessage )
    {
    TInt addedSD(0);
    //-----1.no disco phase
    //-----2.establish credential and put to session
    //-----3.set validator. Enables re-auth for cases when credential is expired between init and send
    if(aPattern.FrameworkId() != KDefaultWSStarFrameworkID)
        {
        CSLOG_L(aRemoteConsumer.ConnectionId(),KNormalLogLevel ,"CWSStarPlugin::AddServiceDescriptionL - this is not KDefaultWSStarFrameworkID");
        addedSD = 0; // zero added
        }
    else if( aPattern.Endpoint().Length() > 0 && aPattern.Contract().Length() > 0 )
        {
        CWSStarServiceSession* pNewSession = CWSStarServiceSession::NewL(*this);
        CleanupStack::PushL(pNewSession);

        // InitializeFromL copies the contract and endpoint of concrete service
        //  also context will be set
        CWSStarPolicyHandler* policyHandler = (CWSStarPolicyHandler*)Handler(WSStarConfig::KPolicyValue);
        pNewSession->InitializeFromL(aPattern, policyHandler);
        pNewSession->AddConsumerL(aRemoteConsumer);
        TSenDataTrafficDetails policyDetails; 
        policyHandler->DataTrafficDetails(policyDetails);
        aRemoteConsumer.SetDataTrafficDetails(policyDetails);
        pNewSession->SessionContext()->Update(WSStarContextKeys::KOnlySharing, EFalse);
//SOA for S60 Composable Handler Framework v0.1  - 7.5.3.1 point 3)        
        TInt error = ProcessOutboundValidationL( aPattern, 
                                                (MSenRemoteServiceSession*)pNewSession,
                                                aErrorMessage );
        CSLOG_FORMAT((aRemoteConsumer.ConnectionId(), KMinLogLevel , _L8("CWSStarPlugin::AddServiceDescriptionL - ProcessOutboundValidation returned: %d"), error));
        if ( !error )
            {
            //CWSStarSessionContext* pSessionRegisterCtx = pNewSession->SessionContext();            
            TPtrC8 cont = aPattern.Contract();
            TInt policyLeaveCode(KErrNone);
            TRAP( policyLeaveCode, error = policyHandler->InvokeL(cont); )
            if ( policyLeaveCode )
                {
                CSLOG_FORMAT((aRemoteConsumer.ConnectionId(), KMinLogLevel , _L8("CWSStarPlugin::AddServiceDescriptionL - PolicyHandler->Invoke() leaved: %d"), policyLeaveCode));
                CSLOG_L(aRemoteConsumer.ConnectionId(),KMinLogLevel ,"CWSStarPlugin::AddServiceDescriptionL() - FATAL: leave from PolicyHandler->Invoke()");
                error = policyLeaveCode;
                }
#ifdef _SENDEBUG
            else
                {
                CSLOG_FORMAT((aRemoteConsumer.ConnectionId(), KMinLogLevel , _L8("CWSStarPlugin::AddServiceDescriptionL - PolicyHandler->Invoke() returned: %d"), error));
                }
#endif // _SENDEBUG
            }
        if ( error )
            {
            addedSD = error;
            CleanupStack::PopAndDestroy(pNewSession);
            }
        else // BOTH outbound validation and policy handler invokation OK(!):
            {
            Manager().AddServiceDescriptionL((CSenWSDescription*)pNewSession);
            addedSD = 1;
            CleanupStack::Pop(pNewSession); //now session is owned by Core / XML DAO
            }
        }
    else
        {
        CSLOG_L(aRemoteConsumer.ConnectionId(),KMinLogLevel ,"CWSStarPlugin::AddServiceDescriptionL - No endpoint AND no contract; new session could not be added.");
        addedSD = 0; // zero added
        }
    return addedSD;
    }

//---------------------------------------------------------------------------
// Create a ServiceSession based upon the given ServiceDescription.
//---------------------------------------------------------------------------
//
TInt CWSStarPlugin::CreateServiceSessionL( MSenServiceDescription& aServiceDescription,
                                           MSenServiceDescription& aPattern,
                                           CSenServiceSession*& aNewSession,
                                           MSenRemoteServiceConsumer& aRemoteConsumer,
                                           HBufC8*& aErrorMessage )
    {
    TInt error(KErrNone);
    if( aServiceDescription.DescriptionClassType() == MSenServiceDescription::EWSStarServiceSession )
        {
        //example:  when init connection with not registered description, nothing in db
        //example:  when bootup and description was already in db (registered during parsing db.file)
        aNewSession = (CWSStarServiceSession*) &aServiceDescription;
        CSLOG_L(aRemoteConsumer.ConnectionId(),KMinLogLevel ,"CWSStarPlugin::ServiceDescription is already a ServiceSession");
        }
    else
        {
        aNewSession = (CWSStarServiceSession*) NewServiceDescriptionL();
        error = ((CWSStarServiceSession*)aNewSession)->InitializeFromL( aServiceDescription );
        CSLOG_FORMAT((aRemoteConsumer.ConnectionId(), KMinLogLevel , _L8("CWSStarPlugin::ServiceDescription is not a ServiceSession, creating new one returned: %d"), error));
        }
    if(aNewSession && !error)//if session was just created, its possible that its not validate
        {
        //((CWSStarServiceSession*)aNewSession)->DetachCredentialsL() ; //invalidating session to recompute the status
        TBool isHostlet = ((CWSStarServiceSession*)aNewSession)->AmIHostletSession() ;
		TInt credentialValidityError = KErrNone ;
		TBool delegateToSTS = EFalse ;
        if (!isHostlet)
            {
			credentialValidityError = ((CWSStarServiceSession*)aNewSession)->TryToSearchValidCredentialL() ;
			if (KErrNone == credentialValidityError)
				{
				CSLOG_L(aRemoteConsumer.ConnectionId(),KMinLogLevel ,"CWSStarPlugin::CreateServiceSessionL() Valid credential found");
				}
			else
				{
				delegateToSTS = ETrue ;
				CSLOG_FORMAT((aRemoteConsumer.ConnectionId(), KMinLogLevel , _L8("CWSStarPlugin::CreateServiceSessionL() No Valid credential found credentialValidityError = [%d]"), credentialValidityError));
				}
			}      
		if(!aNewSession->IsReadyL())
			{
			delegateToSTS = ETrue ;
			CSLOG_L(aRemoteConsumer.ConnectionId(),KMinLogLevel ,"CWSStarPlugin::CreateServiceSessionL() ServiceSession is not ready !!");
			}
		else
			{
			CSLOG_L(aRemoteConsumer.ConnectionId(),KMinLogLevel ,"CWSStarPlugin::CreateServiceSessionL() ServiceSession is ready");
			}

//        if(!aNewSession->IsReadyL())
        if(delegateToSTS)
            {
//            CSLOG_L(aRemoteConsumer.ConnectionId(),KMinLogLevel ,"CWSStarPlugin::ServiceSession is not ready");
            //example: when credentail was parsed(exist) and additionally expired.
            //         We dont need revalidate if credentials doesnt exist at all
            
            
            CWSStarSessionContext* pSessionValidateCtx = ((CWSStarServiceSession*)aNewSession)->SessionContext();
            if (!pSessionValidateCtx) //sessionCts exist if revalidation is processing
                {
                CWSStarPolicyHandler* policyHandler = (CWSStarPolicyHandler*)Handler(WSStarConfig::KPolicyValue);
                pSessionValidateCtx = CWSStarSessionContext::NewLC(Manager().XMLReader(), &aPattern, policyHandler);
                pSessionValidateCtx->Add(WSStarContextKeys::KServiceSession(), *(MSenRemoteServiceSession*)aNewSession);
                ((CWSStarServiceSession*)aNewSession)->SetSessionContext(pSessionValidateCtx);
                CleanupStack::Pop(pSessionValidateCtx);
                }
                
            pSessionValidateCtx->Update(WSStarContextKeys::KRemoteConsumer(),
                                        &aRemoteConsumer);
            aNewSession->AddConsumerL(aRemoteConsumer);
            pSessionValidateCtx->Update(WSStarContextKeys::KOnlySharing, EFalse);    
            error = ProcessOutboundValidationL( aPattern, 
                                               (MSenRemoteServiceSession*)aNewSession,
                                               aErrorMessage );
            if (error) aNewSession->RemoveConsumerL(aRemoteConsumer);
            }
        }
    return error;
    }

TInt CWSStarPlugin::IssuePolicyDownloadL( CSenWSDescription* apSession )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarPlugin::IssuePolicyDownloadL");
    TInt retCode(KErrNone);
    CWSStarPolicyHandler* pPolicyHandler =  (CWSStarPolicyHandler*)Handler(KPolicyValue());
    TInt policyLeave(KErrNone);
    TRAP(policyLeave, retCode = pPolicyHandler->DownloadPolicyL( apSession ));
    if (policyLeave!=KErrNone)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarPlugin::IssuePolicyDownloadL - FATAL: leave from PolicyHandler");
        retCode = policyLeave;
        }
    retCode = pPolicyHandler->UpdatePolicyL(); 
    return retCode;
    }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//
TInt CWSStarPlugin::OnEvent( const TInt aEvent, TAny* apArgument )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarPlugin::OnEvent");
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("  -event code: %d"), aEvent));
    TInt retCode(KErrNone);
    TInt leaveCode( KErrNone );
    
    switch( aEvent)
        {
        
        case KSenEventConsumerAdded:
            {
            ++iConsumerCount;
            CSenWSDescription* pSD = NULL; // not required
            TRAP( leaveCode, retCode = IssuePolicyDownloadL( pSD ); )
            if( leaveCode )
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarPlugin::OnEvent( KSenEventConsumerAdded ) - leave from IssuePolicyDownloadL");
                if( !retCode )
                    {
                    retCode = leaveCode;
                    }
                }
            break;
            }
            
        case KSenEventConsumerRemoved:
            {
            --iConsumerCount;
            break;
            }
        
        case KSenEventWsfReady:
            {
            iDbReadDone = ETrue;
            }
            break;
            
        case KSenEventRegisterNewServiceDescription:
            {
            if( iDbReadDone/* && iConsumerCount > 0*/ )
                {
                CSenWSDescription* pSD = (CSenWSDescription*)apArgument;
                TInt policyLeave(KErrNone);
                TRAP(policyLeave, retCode = IssuePolicyDownloadL( pSD ); )
                if ( policyLeave & !retCode )
                    {
                    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarPlugin::OnEvent( KSenEventSessionsDbReadComplete ) - leave from IssuePolicyDownloadL");
                    retCode = policyLeave;
                    }
                }
            }        
            break;
            
        case KSenEventSessionsDbReadComplete:
            {
            // Current implementation downloads policy independent of (whether or not)
            // the session database has been read or not
            if( iConsumerCount > 0 )
               {
            CSenWSDescription* pSD = (CSenWSDescription*)apArgument;
            TInt policyLeave(KErrNone);
            TRAP(policyLeave, retCode = IssuePolicyDownloadL( pSD ); )
            if ( policyLeave & !retCode )
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarPlugin::OnEvent( KSenEventSessionsDbReadComplete ) - leave from IssuePolicyDownloadL");
                    if( !retCode )
                        {
                retCode = policyLeave;
                        }
                 }
               }
            }
            break;
            
        case KSenEventRegisterIdentityProvider:
        case KSenEventUnregisterIdentityProvider:
            {
            TInt policyLeave(KErrNone);
            TRAP(policyLeave, VerifyPermissionOnEachSessionL(); )
            if ( policyLeave & !retCode )
                {
                TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarPlugin::OnEvent( KSenEventSessionsDbReadComplete ) - leave from VerifyPermissionOnEachSessionL");
                if( !retCode )
                    {
                retCode = policyLeave;
                    }
                }
            }
            break;
            
        default:
            {
            // do nothing
            }
        }
    return retCode;
    }


void CWSStarPlugin::VerifyPermissionOnEachSessionL()
    {
//--    LOG_WRITE_L("CWSStarPlugin::VerifyPermissionOnEachSessionL");

    CSenWSDescription* pOpenMask = CSenWSDescription::NewLC( KNullDesC8, KNullDesC8 ); // this matches with *any* ServDescription
    pOpenMask->SetFrameworkIdL(KDefaultWSStarFrameworkID); // now it matches with any WS-* session
    
    RWSDescriptionArray all;
    iManager.ServiceDescriptionsL(all, *pOpenMask);
    CleanupStack::PopAndDestroy(pOpenMask);
    CleanupClosePushL(all);

    TInt count(all.Count());
    
//--    LOG_WRITEFORMAT((_L("- %d WS-* service session(s) found."), count));

#ifdef _SENDEBUG
//--    TInt verified = 0;
#endif // _SENDEBUG    

    for(TInt i=0; i<count; i++)
        {   
        CSenWSDescription* pSD = (CSenWSDescription*)all[i];

        if(pSD->DescriptionClassType() == MSenServiceDescription::EWSStarServiceSession)
            {
            CWSStarServiceSession* pSession = (CWSStarServiceSession*)pSD;
            pSession->VerifyPermissionL(); // accessible through friendship
#ifdef _SENDEBUG            
//--            verified++;
//--            LOG_WRITEFORMAT((_L("- Verified a session (#%d)"), verified));
#endif // _SENDEBUG              
            }
        }
    CleanupStack::PopAndDestroy(&all);
    }

//---------------------------------------------------------------------------
// There is no extended interface in WSStar framework at the moment.
//---------------------------------------------------------------------------
//
TAny* CWSStarPlugin::ExtendedInterface()
    {
    return NULL;
    }

//---------------------------------------------------------------------------
// Return a new instance of a framework specific class that
//     implements the ServiceDesccription interface.
//---------------------------------------------------------------------------
//
CSenWSDescription* CWSStarPlugin::NewServiceDescriptionL()
    {
    CWSStarServiceSession* pSD =
        CWSStarServiceSession::NewL(*this);
    CleanupStack::PushL(pSD);    
    pSD->SetFrameworkIdL(KDefaultWSStarFrameworkID);
    CleanupStack::Pop(pSD);
    return pSD;
    }

//---------------------------------------------------------------------------
// Getter for manager
//---------------------------------------------------------------------------
//
MSenCoreServiceManager& CWSStarPlugin::Manager()
    {
    return iManager;
    }
//---------------------------------------------------------------------------
// Setter for manager
//---------------------------------------------------------------------------
//
void CWSStarPlugin::SetManager(MSenCoreServiceManager& aServiceManager)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::SetManager(.)")));
    iManager = aServiceManager;
    }
//---------------------------------------------------------------------------
// Getter for configParser
//---------------------------------------------------------------------------
//
CSenBaseFragment& CWSStarPlugin::ConfigParser()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::ConfigParser()")));
    return *this;
    //return callbacks which will be invoked during parsing rest of config
    //this callbacks will be responsible for loading features
    }

//---------------------------------------------------------------------------
// UCS2 serializer
//---------------------------------------------------------------------------
HBufC* CWSStarPlugin::AsXmlUnicodeL()
    {
    HBufC8* pUtf8 = AsXmlL();
    CleanupStack::PushL(pUtf8);
    HBufC* pAsAxlInUnicode = SenXmlUtils::ToUnicodeLC(*pUtf8);
    CleanupStack::Pop(pAsAxlInUnicode);
    CleanupStack::PopAndDestroy(pUtf8); 
    return pAsAxlInUnicode;
    }

//---------------------------------------------------------------------------
// UTF8 serializer
//---------------------------------------------------------------------------
HBufC8* CWSStarPlugin::AsXmlL()
    {
    HBufC8* retVal = HBufC8::NewLC(KMaxLengthXml);
    TPtr8 ptr = retVal->Des();
    ptr.Append(KSenLessThan);
    ptr.Append(KFrameworkLocalName);
    ptr.Append(KSenSpace);
    ptr.Append(KSenSpaceXmlns);
    ptr.Append(KSenEqualsDblQuot);
    ptr.Append(KFrameworkXMLNS);
    ptr.Append(KSenDblQuot);
    
    ptr.Append(KSenSpace);
    ptr.Append(KClassAttr);
    ptr.Append(KSenEqualsDblQuot);
    ptr.Append(KFrameworkCue);
    ptr.Append(KSenDblQuot);
    ptr.Append(KSenGreaterThan);

   
//deviceId
    ptr.Append(KSenLessThan);
    ptr.Append(KDeviceIdLocalName);
    ptr.Append(KSenGreaterThan);
    if (iDeviceId)
        {
        ptr.Append(*iDeviceId);
        }
    ptr.Append(KSenLessThanSlash);	
    ptr.Append(KDeviceIdLocalName);
    ptr.Append(KSenGreaterThan);
    
//--message handlers    
    for (TInt i=0; i < iMsgHandlers.Count(); i++)
        {
        AddHandlerTag(ptr, iMsgHandlers[i]->Name());
        }
//--session handlers    
   for (TInt i=0; i < iSessionHandlers.Count(); i++)
        {
        AddHandlerTag(ptr, iSessionHandlers[i]->Name());
        }  

    ptr.Append(KSenLessThanSlash);
    ptr.Append(KFrameworkLocalName);
    ptr.Append(KSenGreaterThan);
    CleanupStack::Pop(retVal);
    return retVal;
    }



//---------------------------------------------------------------------------
//  used during reading configuration of WSStar framework in order to load 
//  proper handlers.
//---------------------------------------------------------------------------
//
void CWSStarPlugin::StartElementL(const TDesC8& /*aNsUri*/,
                                    const TDesC8& aLocalName,
                                    const TDesC8& /*aQName*/,
                                    const RAttributeArray& aAttrs)
    {
    if(aLocalName.Compare(KDeviceIdLocalName)==0)
        {
            ResetContentL();
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::Found deviceId tag")));
        }
    else if(aLocalName.Compare(KHandlerLocalName) == 0)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::Found handler")));
        TPtrC8 pClass = SenXmlUtils::AttrValue(aAttrs, KClassAttr);
        TPtrC8 pVersion = SenXmlUtils::AttrValue(aAttrs, KVersionAttr);
        
        //message handlers
        if ((pClass == KAddressingValue) ||
            (pClass == KEnveloperValue) ||
            (pClass == KCredentialCollectorValue )||
            (pClass == KSecurityValue) ||
            (pClass == KPassportValue))
            {
            AddHandlerL(pClass, pVersion, SenHandler::EMessageHandler);
            }
        //session handlers
        else if((pClass == KValidateValue) ||
                (pClass == KServiceUpdateValue) ||
                (pClass == KRegisterValue) || 
                (pClass == KPolicyValue) )
            {
            AddHandlerL(pClass, pVersion, SenHandler::ESessionHandler);
            }
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin but can not load it")));
            }
             
        }
    }
//---------------------------------------------------------------------------
//  used during reading configuration of WSStar framework 
//---------------------------------------------------------------------------
//
void CWSStarPlugin::EndElementL(const TDesC8& aNsUri,
                                    const TDesC8& aLocalName,
                                    const TDesC8& aQName)
    {
    if(aLocalName.Compare(KDeviceIdLocalName)==0)
        {
        TPtrC8 content = Content();
        if (content.Length()>0)
            {
            delete iDeviceId;
            iDeviceId = NULL;
            iDeviceId = HBufC8::NewL(content.Length());
            TPtr8 deviceIdDes = iDeviceId->Des();
            deviceIdDes.Copy(content);
            }
        else
            {
            GenerateDeviceIdL();
            }
        }
    else
        {
        CSenBaseFragment::EndElementL(aNsUri,aLocalName,aQName);
        }
    }
    
//---------------------------------------------------------------------------
// getter of handler, using its cue
//---------------------------------------------------------------------------
//    

MSenHandler* CWSStarPlugin::Handler(const TDesC8& aCue)
    {
    TInt count = iMsgHandlers.Count();
    for(TInt i=0;i<count; i++)
        {
        if (iMsgHandlers[i]->Name() == aCue)
            {
            return iMsgHandlers[i];
            }
        }
    count = iSessionHandlers.Count();
    for(TInt i=0;i<count; i++)
        {
        if (iSessionHandlers[i]->Name() == aCue)
            {
            return iSessionHandlers[i];
            }
        }
    return NULL;
    }
//---------------------------------------------------------------------------
// remove handler by cue, used during reloading (another version of handler is 
//      used than deafault setup)
//---------------------------------------------------------------------------
//    
void CWSStarPlugin::RemoveHandler(const TDesC8& aCue)
    {
    TInt count = iMsgHandlers.Count();
    for(TInt i=0;i<count; i++)
        {
        if( iMsgHandlers[i] && (iMsgHandlers[i]->Name() == aCue) )
            {
            delete iMsgHandlers[i];
            iMsgHandlers.Remove(i);
            return;
            }
        }
    count = iSessionHandlers.Count();
    for(TInt i=0;i<count; i++)
        {
        if( iSessionHandlers[i] && (iSessionHandlers[i]->Name() == aCue) )
            {
            delete iSessionHandlers[i];
            iSessionHandlers.Remove(i);
            return;
            }
        }
    }



//---------------------------------------------------------------------------
// Process Outbound validation phase
//---------------------------------------------------------------------------
//     
TInt CWSStarPlugin::ProcessOutboundValidationL( MSenServiceDescription& aPattern, 
                                                MSenRemoteServiceSession* aRemoteServiceSession,
                                                HBufC8*& aErrorMessage )
    {
    CSenSessionHandler* pSTShandler = (CSenSessionHandler*)Handler(KValidateValue);
    CWSStarSessionContext* pSessionValidateCtx = ((CWSStarServiceSession*)aRemoteServiceSession)->SessionContext();
    if (!pSessionValidateCtx) //sessionCts exist if revalidation is processing
        {
        CWSStarPolicyHandler* policyHandler = (CWSStarPolicyHandler*)Handler(WSStarConfig::KPolicyValue);
        pSessionValidateCtx = CWSStarSessionContext::NewLC(Manager().XMLReader(), &aPattern, policyHandler);
        if(pSessionValidateCtx)
        	pSessionValidateCtx->Add(WSStarContextKeys::KServiceSession(), *(MSenRemoteServiceSession*)aRemoteServiceSession);
        ((CWSStarServiceSession*)aRemoteServiceSession)->SetSessionContext(pSessionValidateCtx);
        CleanupStack::Pop(pSessionValidateCtx);
        }
    else
        {
        if(pSessionValidateCtx)
        	pSessionValidateCtx->Update(WSStarContextKeys::KServiceSession(), *(MSenRemoteServiceSession*)aRemoteServiceSession);
        }
    TInt error(KErrNone);
    TRAPD(errorL, error = pSTShandler->InvokeL(*pSessionValidateCtx));
    if ( error )
        {
        const TDesC8* pNotOwnedErrorMsg = pSessionValidateCtx->GetDesC8L(WSStarContextKeys::KErrMessage);
        if ( pNotOwnedErrorMsg )
            {
            aErrorMessage = pNotOwnedErrorMsg->AllocL();
            }
        }
    if (errorL!=KErrNone)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::ProcessOutboundValidationL() !!!!leave from validate handler")));
        error = errorL;
        }
#ifdef RD_SEN_VTCP_SUPPORT		
	if(error)         
       {
        return error;        
       }
    CSenSessionHandler* pCoBrandinghandler = (CSenSessionHandler*)Handler(KCoBrandingValue);
    CWSStarSessionContext* pSessionCoBrandingCtx = ((CWSStarServiceSession*)aRemoteServiceSession)->SessionContext();
    if (!pSessionCoBrandingCtx) //sessionCts exist if revalidation is processing
        {
        CWSStarPolicyHandler* policyHandler = (CWSStarPolicyHandler*)Handler(WSStarConfig::KPolicyValue);
        pSessionCoBrandingCtx = CWSStarSessionContext::NewLC(Manager().XMLReader(), &aPattern, policyHandler);
        pSessionCoBrandingCtx->Add(WSStarContextKeys::KServiceSession(), *(MSenRemoteServiceSession*)aRemoteServiceSession);
        ((CWSStarServiceSession*)aRemoteServiceSession)->SetSessionContext(pSessionCoBrandingCtx);
        CleanupStack::Pop(pSessionCoBrandingCtx);
        }
    else
        {
        pSessionCoBrandingCtx->Update(WSStarContextKeys::KServiceSession(), *(MSenRemoteServiceSession*)aRemoteServiceSession);
        }

    TRAPD(cberrorL, error = pCoBrandinghandler->InvokeL(*pSessionCoBrandingCtx));
    if ( error )
        {
        const TDesC8* pNotOwnedErrorMsg = pSessionCoBrandingCtx->GetDesC8L(WSStarContextKeys::KErrMessage);
        if ( pNotOwnedErrorMsg )
            {
            aErrorMessage = pNotOwnedErrorMsg->AllocL();
            }
        }

    if (cberrorL!=KErrNone)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::ProcessOutboundValidationL() !!!!leave from validate handler")));
        error = cberrorL;
        }
#endif//RD_SEN_VTCP_SUPPORT		
    return error;
    }
    
    
    
//---------------------------------------------------------------------------
// Process Outbound message phase
//---------------------------------------------------------------------------
//                                          
TInt CWSStarPlugin::ProcessOutboundMessageL( CWSStarMessageContext*& aMsgCtx,
                                             const TDesC8* aBody,
                                             CSenSoapMessage*& aReadyMsg )
    {
//----------------enveloper
    TInt error(KErrNone);
    CSenMessageHandler* pHandler = NULL;
    aMsgCtx->Update(WSStarContextKeys::KBody, *aBody);
    if (aReadyMsg)
        {
        aMsgCtx->Update(SenContext::KSenCurrentSoapMessageCtxKey, aReadyMsg);
        }
    else
        {
        pHandler = (CSenMessageHandler*)Handler(KEnveloperValue);
        if (pHandler)
            {
            TRAPD(errorL, error = pHandler->InvokeL(*aMsgCtx));
            if (errorL!=KErrNone)
                {
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::ProcessOutboundMessageL() !!!!leave from enveloper handler")));
                error = errorL;
                }
            }
        aReadyMsg = aMsgCtx->GetCurrentSoapMessage();
        }

//----------------passport
    if (error) return error;
    pHandler = (CSenMessageHandler*)Handler(KPassportValue);
    if (pHandler)
        {
        TRAPD(errorL, error = pHandler->InvokeL(*aMsgCtx));
        if (errorL!=KErrNone)
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::ProcessOutboundMessageL() !!!!leave from passport handler")));
            error = errorL;
            }
        }
    if (error) return error;
//----------------addresing 
//some  logic which will load oall hanlders and in iteration will produce whole messge
//      see axis2\axis2-M2-src\modules\core\src\org\apache\axis\engine\Phase.java
//      now just addressing
    pHandler = (CSenMessageHandler*)Handler(KAddressingValue);
    if (pHandler)
        {
        TRAPD(errorL, error = pHandler->InvokeL(*aMsgCtx));
        if (errorL!=KErrNone)
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::ProcessOutboundMessageL() !!!!leave from addressing handler")));
            error = errorL;
            }
        }
    if (error) return error;
//--------------------credential collector handler
//adding credentials
    pHandler = (CSenMessageHandler*)Handler(KCredentialCollectorValue);
    if (pHandler)
        {
        TRAPD(errorL, error = pHandler->InvokeL(*aMsgCtx));
        if (errorL!=KErrNone)
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::ProcessOutboundMessageL() !!!!leave from credentail collector handler")));
            error = errorL;
            }
        }
    if (error) return error;
 
//--------------------signatureContext handler
//getting pop and sign
    pHandler = (CSenMessageHandler*)Handler(KSecurityValue);
    if (pHandler)
        {
        TRAPD(errorL, error = pHandler->InvokeL(*aMsgCtx));
        if (errorL!=KErrNone)
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::ProcessOutboundMessageL() !!!!leave from security handler")));
            error = errorL;
            }
        }
    return error;
    }




//---------------------------------------------------------------------------
// Process Ibound message phase (body as element)
//---------------------------------------------------------------------------
//   
TInt CWSStarPlugin::ProcessInboundMessageL(CWSStarMessageContext*& aMsgCtx,
                             CSenSoapMessage*& aMsg)
    {
        TInt error(KErrNone);
        aMsgCtx->Add(SenContext::KSenCurrentSoapMessageCtxKey, aMsg);
        CSenMessageHandler* pHandler(NULL);
        
//----------------addresing 
//some  logic which will load oall hanlders and in iteration will produce whole messge
//      see axis2\axis2-M2-src\modules\core\src\org\apache\axis\engine\Phase.java
//      now just addressing
        pHandler = (CSenMessageHandler*)Handler(KAddressingValue);
        if (pHandler)
            {
            TRAPD(errorL, error = pHandler ->InvokeL(*aMsgCtx));            
            if (errorL!=KErrNone)
                {
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::ProcessInboundMessageL() !!!!leave from addressing handler")));
                error = errorL;
                }
            }
        //----------------passport
        if (error) return error;
        pHandler = (CSenMessageHandler*)Handler(KPassportValue);
        if (pHandler)
            {
            TRAPD(errorL, error = pHandler->InvokeL(*aMsgCtx));
            if (errorL!=KErrNone)
                {
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::ProcessInboundMessageL() !!!!leave from passport handler")));
                error = errorL;
                }
            }
        return error;
    }
        
        
//---------------------------------------------------------------------------
// Process Ibound dispatch phase (in fact updatinf session)
//---------------------------------------------------------------------------
//        
TInt CWSStarPlugin::ProcessInboundDispatchL(MSenRemoteServiceSession* aRemoteServiceSession,
                                           CSenSoapMessage*& aMsg)
    {
    CSenSessionHandler* pServiceUpdate = 
                    (CSenSessionHandler*)Handler(KServiceUpdateValue);
    CWSStarSessionContext* pSessionCtx = ((CWSStarServiceSession*)aRemoteServiceSession)->SessionContext();
    if (pSessionCtx)
        {
        if (!pSessionCtx->GetSenRemoteServiceSessionL(WSStarContextKeys::KServiceSession))
            {
            pSessionCtx->Add(WSStarContextKeys::KServiceSession, *(MSenRemoteServiceSession*)aRemoteServiceSession);
            }
        pSessionCtx->Update(SenContext::KSenCurrentSoapMessageCtxKey, aMsg);
        TInt error(KErrNone);
        TRAPD(errorL, error = pServiceUpdate->InvokeL(*pSessionCtx));
        if (errorL!=KErrNone)
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPlugin::ProcessInboundDispatchL() !!!!leave from Servide Update handler")));
            error = errorL;
            }
        return error;
        }
    else
        {
        //in STS session , sessionContext is not set, no dispatching yet in STS session
        return KErrNotFound;
        }
    /*
    handle error will know after that revalidateNeeded=ETrue and make ValidateOutboud again
        this call Create session in WS*Plugin which checking credential*/

    }


//---------------------------------------------------------------------------
// Getter for Device Id.
//---------------------------------------------------------------------------
//        
TInt CWSStarPlugin::DeviceId(TPtrC8& aValue)
    {
    if (iDeviceId)
	    {
        aValue.Set(*iDeviceId);
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

//---------------------------------------------------------------------------
// Generate Device Id
//---------------------------------------------------------------------------
//        
void CWSStarPlugin::GenerateDeviceIdL()
    {
    CSenGuidGen* pGuidGenerator = CSenGuidGen::NewLC();
    delete iDeviceId;
    iDeviceId = NULL;
    if(pGuidGenerator)
    	iDeviceId = pGuidGenerator->GetRandomGuid8LC();
    TPtr8 deviceIdDes = iDeviceId->Des();
    deviceIdDes.LowerCase();
    CleanupStack::Pop(iDeviceId);
    CleanupStack::PopAndDestroy(pGuidGenerator);
    }
//---------------------------------------------------------------------------
// Add handler tag into session.xml
//---------------------------------------------------------------------------
//        
void CWSStarPlugin::AddHandlerTag(TPtr8& aXml, const TDesC8& aHandlerClassName)
    {
    aXml.Append(KSenLessThan);
    aXml.Append(KHandlerLocalName);
    aXml.Append(KSenSpace);
    //class attr
    aXml.Append(KClassAttr);
    aXml.Append(KSenEqualsDblQuot);
    aXml.Append(aHandlerClassName);
    aXml.Append(KSenDblQuot);
    TInt index = iVersions.Find(aHandlerClassName);
    if (index != KErrNotFound)
        {
        HBufC8* value = (HBufC8*)iVersions.ValueAt(index);
        if (value->Length())
            {
            aXml.Append(KSenSpace);
            aXml.Append(KVersionAttr);
            aXml.Append(KSenEqualsDblQuot);
            aXml.Append(*value);
            aXml.Append(KSenDblQuot);    
            }
        }   
    aXml.Append(KSenSlashGreaterThan);
    }
    
//---------------------------------------------------------------------------
// Add handler into array, WS* SIF implementation keep handlers after loading
//---------------------------------------------------------------------------
//        
void CWSStarPlugin::AddHandlerL(const TDesC8& aHandlerClassName, const TDesC8& aVersion, SenHandler::THandlerType aType)
    {
    TInt index = iVersions.Find(aHandlerClassName);
    if (index == KErrNotFound)
        {
        MSenHandler* handler  = NULL;
        CWSStarHandlerContext* ctx = NULL;
        ctx = CWSStarHandlerContext::NewL();
        CleanupStack::PushL(ctx);
        //ctx->Add(HandlerContextKey::KLogger, Log());
        ctx->Add(iManager);
        ctx->Update(HandlerContextKey::KVersion, aVersion);
        iVersions.Append(aHandlerClassName.AllocL(), aVersion.AllocL());
        if(aType == SenHandler::EMessageHandler)
            {
            TRAPD(loadError, handler = CSenMessageHandler::NewL(aHandlerClassName, *ctx));    
            if (loadError != KErrNone)
                {
                User::Leave(loadError);
                }
            else
                {
                iMsgHandlers.Append((CSenMessageHandler*)handler);    
                }    
            }
        else if (aType == SenHandler::ESessionHandler)
            {
            ctx->Add(HandlerContextKey::KSIF, this);
            TRAPD(loadError, handler = CSenSessionHandler::NewL(aHandlerClassName, *ctx));
            if (loadError != KErrNone)
                {
                User::Leave(loadError);
                }
            else
                {
                iSessionHandlers.Append((CSenSessionHandler*)handler);
                }        
            }
        CleanupStack::Pop(ctx);
        iHandlerContexts.Append(ctx);
        }
    else
    //reload handler.
    //user info from session.xml (in fact this is only for testing , nobody can have access to session.xml)
        {
        HBufC8* value = (HBufC8*)iVersions.ValueAt(index);
        if (*value != aVersion)
            {//reload
            iVersions.RemoveByKey(aHandlerClassName);
            RemoveHandler(aHandlerClassName);
            //potentail infinitve reccurence!
            AddHandlerL(aHandlerClassName, aVersion, aType);    
            }
        }
    }

//---------------------------------------------------------------------------
// SetTransportPropertiesL
//---------------------------------------------------------------------------
//        
TInt CWSStarPlugin::SetTransportPropertiesL(MSenTransport& /* aTransport */)
    {
    return KErrNotSupported; 
    }
    
MSenHandler* CWSStarPlugin::PolicyHandler()
{
    MSenHandler* pPolicyHandler =  Handler(KPolicyValue());
    if(pPolicyHandler)
    return pPolicyHandler;
    else
    return NULL;
}
TInt CWSStarPlugin::PolicyConsumerAddedL(CSenServiceSession* aNewSession, MSenRemoteServiceConsumer& aRemoteConsumer)
{
    CWSStarPolicyHandler* policyHandler = (CWSStarPolicyHandler*)Handler(WSStarConfig::KPolicyValue);
    return policyHandler->PolicyConsumerAddedL(aNewSession,aRemoteConsumer);
    
}
// END OF FILE

