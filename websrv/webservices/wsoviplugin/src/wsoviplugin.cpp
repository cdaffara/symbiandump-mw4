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
* Description:           
*
*/












#include "wsoviplugin.h"
#include "SenServiceConnection.h" // framework IDs, error codes
#include "sendebug.h"
#include "senlogger.h"
#include "MSenServiceDescription.h"
#include "wsoviservicesession.h"
#include "SenXmlUtils.h"
#include "senguidgen.h"
#include "wsovitokencreationresponse.h"
#include <SenHttpTransportProperties.h>
#include "sencryptoutils.h"
#include "wsoviserviceupdatehandler.h"
#include "wsovivalidatehandler.h"
using namespace WSOviConfig;
using namespace OAuth;
// Local CONSTANTS
const TInt KMaxLengthXml  = 1280; 

// ---------------------------------------------------------------------------
// Create instance of concrete ECOM interface implementation
// ---------------------------------------------------------------------------
//
CWSOviPlugin* CWSOviPlugin::NewL(TAny* aManager)
    {
    MSenCoreServiceManager* manager =
        reinterpret_cast<MSenCoreServiceManager*>(aManager);

    CWSOviPlugin* self = new (ELeave) CWSOviPlugin(*manager);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CWSOviPlugin::CWSOviPlugin(MSenCoreServiceManager& aManager)
    : iManager(aManager),
      iVersions(ETrue, ETrue)
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CWSOviPlugin::~CWSOviPlugin()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::~CWSOviPlugin()")));
    iMsgHandlers.ResetAndDestroy();
    iSessionHandlers.ResetAndDestroy();
    iHandlerContexts.ResetAndDestroy();
    iVersions.Reset();
    iIMSI.Close();
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::~CWSOviPlugin() Completed")));
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CWSOviPlugin::ConstructL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::ConstructL()")));

    BaseConstructL(KFrameworkXMLNS, KFrameworkLocalName);
    //load default setup of handlers
    AddHandlerL(KValidateValue, KValidateHandlerVersion, SenHandler::ESessionHandler);
    AddHandlerL(KRegisterValue, KNullDesC8, SenHandler::ESessionHandler);
    
    AddHandlerL(KCredentialCollectorValue, KNullDesC8, SenHandler::EMessageHandler);
    AddHandlerL(KDeviceValue, KNullDesC8, SenHandler::EMessageHandler);
    AddHandlerL(KSecurityValue, KNullDesC8, SenHandler::EMessageHandler);
    AddHandlerL(KEncoderValue, KNullDesC8, SenHandler::EMessageHandler);
    AddHandlerL(KServiceUpdateValue, KNullDesC8, SenHandler::ESessionHandler);
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::ConstructL() Completed")));
    }

//========================================================
//                MSIF SPecific
//========================================================

//---------------------------------------------------------------------------
// Attempt to register the ServiceDescription to the ServiceManager 
//---------------------------------------------------------------------------
//
TInt CWSOviPlugin::RegisterServiceDescriptionL(
    MSenServiceDescription& aServiceDescription )
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::RegisterServiceDescriptionL(.)")));
    TInt error(KErrNone);

    CWSOviSessionContext* pSessionRegisterCtx = CWSOviSessionContext::NewLC();
    pSessionRegisterCtx->Add(WSOviContextKeys::KServiceDescription(),
            (CSenWSDescription*)&aServiceDescription);
    pSessionRegisterCtx->Add(WSOviContextKeys::KRegisterAction(),
            WSOviContextValues::KActionRegister());

//SOA for S60 Composable Handler Framework v0.1 - 7.5.3.1 point 1)            
    CSenSessionHandler* pregisterHandler = 
                (CSenSessionHandler*)Handler(KRegisterValue());
        
    TRAPD(errorL,error= pregisterHandler->InvokeL(*pSessionRegisterCtx));
    if (errorL!=KErrNone)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::RegisterServiceDescriptionL() !!!!leave from RegisterHandler")));
        error = errorL;
        }
    CleanupStack::PopAndDestroy(pSessionRegisterCtx);
    return error;
    }

//---------------------------------------------------------------------------
// Attempt to unregister the ServiceDescription from the ServiceManager 
//---------------------------------------------------------------------------
//
TInt CWSOviPlugin::UnregisterServiceDescriptionL(
    MSenServiceDescription& aServiceDescription)
    {
    
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::UnRegisterServiceDescriptionL(.)")));
//SOA for S60 Composable Handler Framework v0.1  - 7.5.3.1 point 1)            
    TInt error(KErrNone);
    CSenSessionHandler* pregisterHandler = 
                (CSenSessionHandler*)Handler(KRegisterValue());
        
    CWSOviSessionContext* pSessionRegisterCtx = CWSOviSessionContext::NewLC();
    pSessionRegisterCtx->Add(WSOviContextKeys::KServiceDescription(),
            (CSenWSDescription*)&aServiceDescription);
    pSessionRegisterCtx->Add(WSOviContextKeys::KRegisterAction(),
            WSOviContextValues::KActionUnregister());
    TRAPD(errorL,error = pregisterHandler->InvokeL(*pSessionRegisterCtx));
    if (errorL!=KErrNone)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::UnregisterServiceDescriptionL() !!!!leave from RegisterHandler")));
        error = errorL;
        }
    CleanupStack::PopAndDestroy(pSessionRegisterCtx);
    return error;
    }

//---------------------------------------------------------------------------
// Getter of framework ("WS-Ovi")
//---------------------------------------------------------------------------
//
const TDesC8& CWSOviPlugin::Id()
    {
    return KDefaultOviFrameworkID();
    }

//---------------------------------------------------------------------------
// Try to find one or more services that match the given ServiceDescription.
//    Adding is only invoked if NO! session in cache. 
//    Its mean that we have create new one.
//---------------------------------------------------------------------------
//
TInt CWSOviPlugin::AddServiceDescriptionL( MSenServiceDescription& aPattern,
                                            MSenRemoteServiceConsumer& aRemoteConsumer,
                                            HBufC8*& aErrorMessage )
    {
    TInt addedSD(0);
    //-----1.no disco phase
    //-----2.establish credential and put to session
    //-----3.set validator. Enables re-auth for cases when credential is expired between init and send
    if(aPattern.FrameworkId() != KDefaultOviFrameworkID)
        {
        TLSLOG_L(KSenClientSessionLogChannelBase+aRemoteConsumer.ConnectionId(),KNormalLogLevel ,"CWSOviPlugin::AddServiceDescriptionL - this is not KDefaultWSOviFrameworkID");
        addedSD = 0; // zero added
        }
    else if( aPattern.Endpoint().Length() > 0 && aPattern.Contract().Length() > 0 )
        {
        CWSOviServiceSession* pNewSession = CWSOviServiceSession::NewL(*this);
        CleanupStack::PushL(pNewSession);

        // InitializeFromL copies the contract and endpoint of concrete service
        //  also context will be set
        pNewSession->InitializeFromL(aPattern);
        pNewSession->AddConsumerL(aRemoteConsumer);
        pNewSession->SessionContext()->Update(WSOviContextKeys::KOnlySharing, EFalse);
//SOA for S60 Composable Handler Framework v0.1  - 7.5.3.1 point 3)        
        TInt error = ProcessOutboundValidationL( aPattern, 
                                                (MSenRemoteServiceSession*)pNewSession,
                                                aErrorMessage );
        TLSLOG_FORMAT((KSenClientSessionLogChannelBase+aRemoteConsumer.ConnectionId(), KMinLogLevel , _L8("CWSOviPlugin::AddServiceDescriptionL - ProcessOutboundValidation returned: %d"), error));

        if ( error )
            {
            if (error>KErrNone)
                {
                addedSD = KErrSenAuthenticationFault;
                }
            else
                {
                addedSD = error;
                }
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
        TLSLOG_L(KSenClientSessionLogChannelBase+aRemoteConsumer.ConnectionId(),KMinLogLevel ,"CWSOviPlugin::AddServiceDescriptionL - No endpoint AND no contract; new session could not be added.");
        addedSD = 0; // zero added
        }
    return addedSD;
    }

TBool CWSOviPlugin::IsTrustedClient(MSenRemoteServiceConsumer& aRemoteConsumer)
    {
    TSecureId sid = aRemoteConsumer.SecureId();    
    //_LIT_SECURE_ID(nokia_chateventmonitorserver, 0x2001B2C5 );
    
    TVendorId vendor = aRemoteConsumer.VendorId();
	_LIT_VENDOR_ID(trustedVendor,VID_DEFAULT);
    
    if (vendor.iId == trustedVendor.iId )
        return ETrue;
    else
        return EFalse;
    }

//---------------------------------------------------------------------------
// Create a ServiceSession based upon the given ServiceDescription.
//---------------------------------------------------------------------------
//
TInt CWSOviPlugin::CreateServiceSessionL( MSenServiceDescription& aServiceDescription,
                                           MSenServiceDescription& aPattern,
                                           CSenServiceSession*& aNewSession,
                                           MSenRemoteServiceConsumer& aRemoteConsumer,
                                           HBufC8*& aErrorMessage )
    {
    if (!IsTrustedClient(aRemoteConsumer)) return KErrPermissionDenied;
    TInt error(KErrNone);
    if( aServiceDescription.DescriptionClassType() == MSenServiceDescription::EOviServiceSession )
        {
        //usecase:  when init connection with not registered description, nothing in db
        //usecase:  when bootup and description was already in db (registered during parsing db.file)
        aNewSession = (CWSOviServiceSession*) &aServiceDescription;
        aNewSession->SetStatusL();
        TLSLOG_L(KSenClientSessionLogChannelBase+aRemoteConsumer.ConnectionId(),KMinLogLevel ,"CWSOviPlugin::ServiceDescription is already a ServiceSession");
        }
    else
        {
        aNewSession = (CWSOviServiceSession*) NewServiceDescriptionL();
        error = ((CWSOviServiceSession*)aNewSession)->InitializeFromL( aServiceDescription );
        }
    if(aNewSession && !error)//if session was just created, its possible that its not validate
        {
        if(!aNewSession->IsReadyL())
            {
            TLSLOG_L(KSenClientSessionLogChannelBase+aRemoteConsumer.ConnectionId(),KMinLogLevel ,"CWSOviPlugin::ServiceSession is not ready");
            //usecase: when credential was parsed(exiNAUTH) and additionally expired.
            //         We don't need revalidate if credentials doesnt exist at all
            
            
            CWSOviSessionContext* pSessionValidateCtx = ((CWSOviServiceSession*)aNewSession)->SessionContext();
            if (!pSessionValidateCtx) //sessionCts exist if revalidation is processing
                {
                pSessionValidateCtx = CWSOviSessionContext::NewLC();
                pSessionValidateCtx->Add(WSOviContextKeys::KServiceSession(), *(MSenRemoteServiceSession*)aNewSession);
                ((CWSOviServiceSession*)aNewSession)->SetSessionContext(pSessionValidateCtx);
                CleanupStack::Pop(pSessionValidateCtx);
                }
                
            pSessionValidateCtx->Update(WSOviContextKeys::KRemoteConsumer(),
                                        &aRemoteConsumer);
            aNewSession->AddConsumerL(aRemoteConsumer);
            pSessionValidateCtx->Update(WSOviContextKeys::KOnlySharing, EFalse);   
            error = ProcessOutboundValidationL( aPattern, 
                                               (MSenRemoteServiceSession*)aNewSession,
                                               aErrorMessage );
            if (error) aNewSession->RemoveConsumerL(aRemoteConsumer);
            if (error>KErrNone)
                {
                error = KErrSenAuthenticationFault;
                }
            }
        else
            {
            //usecase: when credential was parsed(exiNAUTH) and credential is valid.
            // We start credential monitor and revalidation is start when credential expired. 
            TLSLOG_L(KSenClientSessionLogChannelBase+aRemoteConsumer.ConnectionId(),KMinLogLevel ,"CWSOviPlugin::Call ActiveTicketObserverL()");
            ((CWSOviServiceSession*)aNewSession)->ActiveTicketObserverL();
            }
        }
    return error;
    }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//
TInt CWSOviPlugin::OnEvent( const TInt aEvent, TAny* /*apArgument*/ )
    {
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSOviPlugin::OnEvent");
    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("  -event code: %d"), aEvent));
    TInt retCode( KErrNone );

    switch( aEvent)
        {
        
        case KSenEventWsfReady:
            {
            TPtrC8 imsi = iManager.IMSI();
            if (iIMSI.Compare(imsi))
            	{
            	const RPointerArray<CSenIdentityProvider> list = iManager.IdentityProvidersL();
        	    for (TInt i = 0; i<list.Count(); i++)
    	            {
    	            CSenElement* el(NULL);
    	            list[i]->AccountExtensions(el);
    	            if (el)
    	            	{
    	            	delete (el->RemoveElement(KSenAccAutoSignIn));
    	            	((MSenCredentialManager&)iManager).RemoveCredentialsL(list[i]->ProviderID());
    	            	}
    	            //((MSenCredentialManager&)iManager).RemoveCredentialsL(list[i]->ProviderID());
    	            }
        	    iIMSI.Close();
        	    iIMSI.Create(imsi);
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
                retCode = policyLeave;
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


void CWSOviPlugin::VerifyPermissionOnEachSessionL()
    {

    CSenWSDescription* pOpenMask = CSenWSDescription::NewLC( KNullDesC8, KNullDesC8 );
    pOpenMask->SetFrameworkIdL(KDefaultOviFrameworkID); // now it matches with any WS-Ovi session
    
    RWSDescriptionArray all;
    iManager.ServiceDescriptionsL(all, *pOpenMask);
    CleanupStack::PopAndDestroy(pOpenMask);
    CleanupClosePushL(all);

    TInt count(all.Count());

    for(TInt i=0; i<count; i++)
        {   
        CSenWSDescription* pSD = (CSenWSDescription*)all[i];

        if(pSD->DescriptionClassType() == MSenServiceDescription::EOviServiceSession)
            {
            CWSOviServiceSession* pSession = (CWSOviServiceSession*)pSD;
            pSession->VerifyPermissionL(); // accessible through friendship
            }
        }
    CleanupStack::PopAndDestroy(&all);
    }

//---------------------------------------------------------------------------
// There is no extended interface in WSOvi framework at the moment.
//---------------------------------------------------------------------------
//
TAny* CWSOviPlugin::ExtendedInterface()
    {
    return NULL;
    }

//---------------------------------------------------------------------------
// Return a new instance of a framework specific class that
//     implements the ServiceDesccription interface.
//---------------------------------------------------------------------------
//
CSenWSDescription* CWSOviPlugin::NewServiceDescriptionL()
    {
    CWSOviServiceSession* pSD =
        CWSOviServiceSession::NewL(*this);
    CleanupStack::PushL(pSD);    
    pSD->SetFrameworkIdL(KDefaultOviFrameworkID);
    CleanupStack::Pop(pSD);
    return pSD;
    }

//---------------------------------------------------------------------------
// Getter for manager
//---------------------------------------------------------------------------
//
MSenCoreServiceManager& CWSOviPlugin::Manager()
    {
    return iManager;
    }
//---------------------------------------------------------------------------
// Setter for manager
//---------------------------------------------------------------------------
//
void CWSOviPlugin::SetManager(MSenCoreServiceManager& aServiceManager)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::SetManager(.)")));
    iManager = aServiceManager;
    }
//---------------------------------------------------------------------------
// Getter for configParser
//---------------------------------------------------------------------------
//
CSenBaseFragment& CWSOviPlugin::ConfigParser()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::ConfigParser()")));
    return *this;
    //return callbacks which will be invoked during parsing rest of config
    //this callbacks will be responsible for loading features
    }

//---------------------------------------------------------------------------
// UCS2 serializer
//---------------------------------------------------------------------------
HBufC* CWSOviPlugin::AsXmlUnicodeL()
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
HBufC8* CWSOviPlugin::AsXmlL()
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
//add IMIS digest
   	ptr.Append(KSenLessThan);
   	ptr.Append(KIMSILocalName);
   	ptr.Append(KSenSpace);
      //class attr
   	ptr.Append(KIMSIDigestAttr);
   	ptr.Append(KSenEqualsDblQuot);
   	ptr.Append(iIMSI);
   	ptr.Append(KSenDblQuot);
   	ptr.Append(KSenSlashGreaterThan);
    
   
   
    ptr.Append(KSenLessThanSlash);
    ptr.Append(KFrameworkLocalName);
    ptr.Append(KSenGreaterThan);
    CleanupStack::Pop(retVal);
    return retVal;
    }



//---------------------------------------------------------------------------
//  used during reading configuration of WSOvi framework in order to load 
//  proper handlers.
//---------------------------------------------------------------------------
//
void CWSOviPlugin::StartElementL(const TDesC8& /*aNsUri*/,
                                    const TDesC8& aLocalName,
                                    const TDesC8& /*aQName*/,
                                    const RAttributeArray& aAttrs)
    {
    if(aLocalName.Compare(KHandlerLocalName) == 0)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::Found handler")));
        TPtrC8 pClass = SenXmlUtils::AttrValue(aAttrs, KClassAttr);
        TPtrC8 pVersion = SenXmlUtils::AttrValue(aAttrs, KVersionAttr);
        
        //message handlers
        if ((pClass == KDeviceValue ) || 
            (pClass == KSecurityValue ))
            {
            AddHandlerL(pClass, pVersion, SenHandler::EMessageHandler);
            }
        //session handlers
        else if((pClass == KValidateValue) ||
                (pClass == KRegisterValue)
                )
            {
            AddHandlerL(pClass, pVersion, SenHandler::ESessionHandler);
            }
        else
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("Found unknown handler!")));
            }
             
        }
    else if (! aLocalName.Compare(KIMSILocalName))
    	{
    	iIMSI.Close();
    	iIMSI.Create(SenXmlUtils::AttrValue(aAttrs, KIMSIDigestAttr));
    	}
    }
//---------------------------------------------------------------------------
//  used during reading configuration of WSOvi framework 
//---------------------------------------------------------------------------
//
void CWSOviPlugin::EndElementL(const TDesC8& aNsUri,
                                    const TDesC8& aLocalName,
                                    const TDesC8& aQName)
    {
    CSenBaseFragment::EndElementL(aNsUri,aLocalName,aQName);
    }
    
//---------------------------------------------------------------------------
// getter of handler, using its cue
//---------------------------------------------------------------------------
//    

MSenHandler* CWSOviPlugin::Handler(const TDesC8& aCue)
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
void CWSOviPlugin::RemoveHandler(const TDesC8& aCue)
    {
    TInt count = iMsgHandlers.Count();
    for(TInt i=0;i<count; i++)
        {
        if (iMsgHandlers[i]->Name() == aCue)
            {
            delete iMsgHandlers[i];
            iMsgHandlers.Remove(i);
            return;
            }
        }
    count = iSessionHandlers.Count();
    for(TInt i=0;i<count; i++)
        {
        if (iSessionHandlers[i]->Name() == aCue)
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
TInt CWSOviPlugin::ProcessOutboundValidationL( MSenServiceDescription& /*aPattern*/, 
                                                MSenRemoteServiceSession* aRemoteServiceSession,
                                                HBufC8*& aErrorMessage )
    {
    TInt error(KErrNone);
    CSenSessionHandler* pValidatehandler = (CSenSessionHandler*)Handler(KValidateValue);
    CWSOviSessionContext* pSessionValidateCtx = ((CWSOviServiceSession*)aRemoteServiceSession)->SessionContext();
    if (!pSessionValidateCtx) //sessionCts exist if revalidation is processing
        {
        pSessionValidateCtx = CWSOviSessionContext::NewLC();
        pSessionValidateCtx->Add(WSOviContextKeys::KServiceSession(), *(MSenRemoteServiceSession*)aRemoteServiceSession);
        ((CWSOviServiceSession*)aRemoteServiceSession)->SetSessionContext(pSessionValidateCtx);
        CleanupStack::Pop(pSessionValidateCtx);
        }
    else
        {
        pSessionValidateCtx->Update(WSOviContextKeys::KServiceSession(), *(MSenRemoteServiceSession*)aRemoteServiceSession);
        }
    TRAPD(errorL, error = pValidatehandler->InvokeL(*pSessionValidateCtx));
    if ( error )
        {
        const TDesC8* pNotOwnedErrorMsg = pSessionValidateCtx->GetDesC8L(WSOviContextKeys::KErrMessage);
        if ( pNotOwnedErrorMsg )
            {
            aErrorMessage = pNotOwnedErrorMsg->AllocL();
            }
        }
    if (errorL!=KErrNone)
        {
        TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::ProcessOutboundValidationL() !!!!leave from validate handler")));
        error = errorL;
        }
    return error;
    }
    
    
    
//---------------------------------------------------------------------------
// Process Outbound message phase
//---------------------------------------------------------------------------
//                                          
TInt CWSOviPlugin::ProcessOutboundMessageL( CWSOviMessageContext*& aMsgCtx)
    {

//--------------------device handler
    TInt error(KErrNone);
    CSenMessageHandler* pHandler = NULL;
    pHandler = (CSenMessageHandler*)Handler(KDeviceValue);
    if (pHandler)
        {
        TRAPD(errorL, error = pHandler->InvokeL(*aMsgCtx));
        if (errorL!=KErrNone)
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::ProcessOutboundMessageL() !!!!leave from device handler")));
            error = errorL;
            }
        }


//--------------------security handler
    pHandler = (CSenMessageHandler*)Handler(KSecurityValue);
    if (pHandler)
        {
        TRAPD(errorL, error = pHandler->InvokeL(*aMsgCtx));
        if (errorL!=KErrNone)
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::ProcessOutboundMessageL() !!!!leave from security handler")));
            error = errorL;
            }
        }

//--------------------credential collector handler
    pHandler = (CSenMessageHandler*)Handler(KCredentialCollectorValue);
    if (pHandler)
        {
        TRAPD(errorL, error = pHandler->InvokeL(*aMsgCtx));
        if (errorL!=KErrNone)
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::ProcessOutboundMessageL() !!!!leave from Credential Collector handler")));
            error = errorL;
            }
        }

    //--------------------encoder handler
        pHandler = (CSenMessageHandler*)Handler(KEncoderValue);
        if (pHandler)
            {
            TRAPD(errorL, error = pHandler->InvokeL(*aMsgCtx));
            if (errorL!=KErrNone)
                {
                TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::ProcessOutboundMessageL() !!!!leave from Encoder handler")));
                error = errorL;
                }
            }

    return error;
    }




//---------------------------------------------------------------------------
// Process Ibound message phase (body as element)
//---------------------------------------------------------------------------
//   
TInt CWSOviPlugin::ProcessInboundMessageL()
    {
    return KErrNotSupported;
    }
        
        
//---------------------------------------------------------------------------
// Process Ibound dispatch phase (in fact updatinf session)
//---------------------------------------------------------------------------
//        
TInt CWSOviPlugin::ProcessInboundDispatchL( MSenRemoteServiceSession* aRemoteServiceSession,
                                               const TInt aErrorCode,
                                               HBufC8* aMessage,
                                               MSenProperties* aResponseTransportProperties)
    {
    TInt error(KErrNone);
    CSenSessionHandler* pHandler = NULL;
    CWSOviSessionContext* pSessionCtx = ((CWSOviServiceSession*)aRemoteServiceSession)->SessionContext();
    pSessionCtx->Update(WSOviContextKeys::KServiceSession(), *(MSenRemoteServiceSession*)aRemoteServiceSession);
    TLSLOG_FORMAT((KSenClientSessionLogChannelBase, KMinLogLevel , _L8("CWSOviPlugin::ProcessInboundDispatchL - aErrorCode: %d"), aErrorCode));
    if ( aMessage ) 
        {
        pSessionCtx->Update( WSOviContextKeys::KMessageBody, *aMessage );
        TLSLOG_ALL(KSenClientSessionLogChannelBase, KMinLogLevel , *aMessage);
        }
    if ( aResponseTransportProperties ) 
        {
        pSessionCtx->Update( WSOviContextKeys::KTP, (TAny*)aResponseTransportProperties );
        }
        
    pSessionCtx->Update(WSOviContextKeys::KErrCode, aErrorCode);
    
    //--------------------encoder handler
    pHandler = (CSenSessionHandler*)Handler(KServiceUpdateValue);
    if (pHandler)
        {
        TRAPD(errorL, error = pHandler->InvokeL(*pSessionCtx));
        if (errorL!=KErrNone)
            {
            TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::ProcessOutboundMessageL() !!!!leave from Service Update handler")));
            error = errorL;
            }
        const TInt* diff = pSessionCtx->GetIntL(WSOviContextKeys::KRetryNeeded);
        if (diff && *diff > 0)
        	{
        	CWSOviValidateHandler* validateHandler = (CWSOviValidateHandler*)Handler(KValidateValue);
        	validateHandler->HandlerContext().Update(HandlerContextKey::KDiff, *diff);
        	}
        }
    return error;
    }


//---------------------------------------------------------------------------
// Add handler tag into session.xml
//---------------------------------------------------------------------------
//        
void CWSOviPlugin::AddHandlerTag(TPtr8& aXml, const TDesC8& aHandlerClassName)
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
// Add handler into array, WSOvi SIF implementation keep handlers after loading
//---------------------------------------------------------------------------
//        
void CWSOviPlugin::AddHandlerL(const TDesC8& aHandlerClassName, const TDesC8& aVersion, SenHandler::THandlerType aType)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSOviPlugin::AddHandlerL")));
    TInt index = iVersions.Find(aHandlerClassName);
    if (index == KErrNotFound)
        {
        MSenHandler* handler  = NULL;
        CWSOviHandlerContext* ctx = NULL;
        ctx = CWSOviHandlerContext::NewL();
        CleanupStack::PushL(ctx);
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
TInt CWSOviPlugin::SetTransportPropertiesL(MSenTransport& /* aTransport */)
    {
    return KErrNotSupported;
    }
    
// END OF FILE

