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










#include <e32std.h>
#include "wsstarpolicyhandler.h"

#include "senpointermap.h"
#include "wspolicy.h"
#include "sendebug.h"
#include "wsstardictionary.h"
#include "MSenServicePolicy.h"
#include "wsstarpolicymanager.h"
#include "senlogger.h"

//---------------------------------------------------------------------------
// Create instance of concrete ECOM interface implementation
//---------------------------------------------------------------------------
//
CWSStarPolicyHandler* CWSStarPolicyHandler::NewL(TAny* aHandlerCtx)
    {    
    MSenHandlerContext* handlerCtx = reinterpret_cast<MSenHandlerContext*>(aHandlerCtx);
    CWSStarPolicyHandler* self   = new (ELeave) CWSStarPolicyHandler(*handlerCtx);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
//
CWSStarPolicyHandler::CWSStarPolicyHandler(MSenHandlerContext& aCtx):CSenSessionHandler(aCtx)
    {
    }

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
//
CWSStarPolicyHandler::~CWSStarPolicyHandler()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("~CWSStarPolicyHandler::~CWSStarPolicyHandler()")));
    delete iPolicyManager;
    }

//---------------------------------------------------------------------------
// Second phase construction.
//---------------------------------------------------------------------------
//
void CWSStarPolicyHandler::ConstructL()
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPolicyHandler::ConstructL()")));
    iPolicyManager = CWSStarPolicyManager::NewL((TAny*)&iHandlerContext);
    }

//---------------------------------------------------------------------------
// the most emergent method in message handler interface.
// Each sub-class must implement this method thus providing message processing routine.
//---------------------------------------------------------------------------
//
TInt CWSStarPolicyHandler::InvokeL(MSenSessionContext& /*aCtx*/)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPolicyHandler::InvokeL(MSenMessageContext& aCtx)")));
//This method will be impelented when we want to apply some particular policy on some session
//right now we wil just use the InvokeL(const TDesC8& aContract) method to register a contract for Policy
    return KErrNone;
    }

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//  
TInt CWSStarPolicyHandler::InvokeL(const TDesC8& aContract)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPolicyHandler::InvokeL(MSenMessageContext& aCtx)")));
    return iPolicyManager->InvokeL(aContract);
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//           
TInt CWSStarPolicyHandler::UnRegisterWsPolicyL(MSenServiceDescription& aServiceDescription)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPolicyHandler::UnRegisterWsPolicyL")));
    TInt error(KErrGeneral);
    return iPolicyManager->UnRegisterWsPolicyL(aServiceDescription, error);
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//           
TInt CWSStarPolicyHandler::PolicyConsumerAddedL(CSenServiceSession* aNewSession, MSenRemoteServiceConsumer& aRemoteConsumer)
    {
    TLSLOG(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,(_L("CWSStarPolicyHandler::PolicyConsumerAddedL")));
    return iPolicyManager->PolicyConsumerAddedL(aNewSession,aRemoteConsumer);
    }    
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//       
TInt CWSStarPolicyHandler::DownloadPolicyL(CSenWSDescription* apArgument)
    {
    return iPolicyManager->DownloadPolicyL( apArgument );
    }

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//  
CSenElement* CWSStarPolicyHandler::PolicyL(MSenServiceDescription* aSD)	//codescannerwarnings
    {
        return iPolicyManager->PolicyL(aSD);	//codescannerwarnings
    }
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//      
CSenElement* CWSStarPolicyHandler::PolicyL(const TDesC8& aUri)	//codescannerwarnings
    {
        return iPolicyManager->PolicyL(aUri);	//codescannerwarnings
    }        
//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//       
TInt CWSStarPolicyHandler::UpdatePolicyL()
    {
    return iPolicyManager->UpdatePolicyL(this);
    }

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//       
SenHandler::THandlerDirection CWSStarPolicyHandler::Direction() const
    {
    return SenHandler::EBoth;
    }

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//    
SenHandler::THandlerPhase CWSStarPolicyHandler::Phase()
    {
    return SenHandler::EValidate;
    }

//---------------------------------------------------------------------------
// Init implementation
//---------------------------------------------------------------------------
//
TInt CWSStarPolicyHandler::InitL(MSenHandlerContext& aCtx)
    {
    iHandlerContext = aCtx;
    return KErrNone;
    }

//---------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------
//    
RFileLogger* CWSStarPolicyHandler::Log() const
    {
    RFileLogger* pLog = NULL;
    TRAP_IGNORE( pLog = (RFileLogger*)iHandlerContext.GetAnyL(HandlerContextKey::KLogger); )
    return pLog;
    }
void CWSStarPolicyHandler::DataTrafficDetails(TSenDataTrafficDetails& aDetails) 		          
	{
	iPolicyManager->DataTrafficDetails(aDetails);
	}
// END OF FILE


