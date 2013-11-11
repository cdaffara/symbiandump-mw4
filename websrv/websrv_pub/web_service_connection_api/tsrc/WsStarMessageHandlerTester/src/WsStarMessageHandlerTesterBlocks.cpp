/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        ?Description
*
*/









// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "WsStarMessageHandlerTester.h"
#include <SenXmlUtils.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define LOCAL_ASSERT(expression)	{if(!(expression)){return KErrArgument;}}

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWsStarMessageHandlerTester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CWsStarMessageHandlerTester::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CWsStarMessageHandlerTester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CWsStarMessageHandlerTester::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
		ENTRY("NewL-test_CWSStarAddressingHandler_NewL", MT_CWSStarAddressingHandler_NewLL),
		ENTRY("InvokeL-test1_CWSStarAddressingHandler_InvokeL", MT_CWSStarAddressingHandler_InvokeL1L),
		ENTRY("InvokeL-test2_CWSStarAddressingHandler_InvokeL", MT_CWSStarAddressingHandler_InvokeL2L),
		ENTRY("InvokeL-test3_CWSStarAddressingHandler_InvokeL", MT_CWSStarAddressingHandler_InvokeL3L),
		ENTRY("InvokeL-test4_CWSStarAddressingHandler_InvokeL", MT_CWSStarAddressingHandler_InvokeL4L),
		ENTRY("InvokeL-test5_CWSStarAddressingHandler_InvokeL", MT_CWSStarAddressingHandler_InvokeL5L),
		ENTRY("InvokeL-test6_CWSStarAddressingHandler_InvokeL", MT_CWSStarAddressingHandler_InvokeL6L),
		ENTRY("Direction-test_CWSStarAddressingHandler_Direction", MT_CWSStarAddressingHandler_DirectionL),
		ENTRY("Phase-test_CWSStarAddressingHandler_Phase", MT_CWSStarAddressingHandler_PhaseL),
		ENTRY("InitL-test_CWSStarAddressingHandler_InitL", MT_CWSStarAddressingHandler_InitLL),
		ENTRY("NewL-test_CWSStarEnveloperHandler_NewL", MT_CWSStarEnveloperHandler_NewLL),
		ENTRY("InvokeL-test1_CWSStarEnveloperHandler_InvokeL", MT_CWSStarEnveloperHandler_InvokeL1L),
		ENTRY("InvokeL-test2_CWSStarEnveloperHandler_InvokeL", MT_CWSStarEnveloperHandler_InvokeL2L),
		ENTRY("Direction-test_CWSStarEnveloperHandler_Direction", MT_CWSStarEnveloperHandler_DirectionL),
		ENTRY("Phase-test_CWSStarEnveloperHandler_Phase", MT_CWSStarEnveloperHandler_PhaseL),
		ENTRY("InitL-test_CWSStarEnveloperHandler_InitL", MT_CWSStarEnveloperHandler_InitLL),
		ENTRY("NewL-test_CWSStarCredentialCollectorHandler_NewL", MT_CWSStarCredentialCollectorHandler_NewLL),
		ENTRY("InvokeL-test1_CWSStarCredentialCollectorHandler_InvokeL", MT_CWSStarCredentialCollectorHandler_InvokeL1L),
		ENTRY("InvokeL-test2_CWSStarCredentialCollectorHandler_InvokeL", MT_CWSStarCredentialCollectorHandler_InvokeL2L),
		ENTRY("InvokeL-test3_CWSStarCredentialCollectorHandler_InvokeL", MT_CWSStarCredentialCollectorHandler_InvokeL3L),
		ENTRY("Direction-test_CWSStarCredentialCollectorHandler_Direction", MT_CWSStarCredentialCollectorHandler_DirectionL),
		ENTRY("Phase-test_CWSStarCredentialCollectorHandler_Phase", MT_CWSStarCredentialCollectorHandler_PhaseL),
		ENTRY("InitL-test_CWSStarCredentialCollectorHandler_InitL", MT_CWSStarCredentialCollectorHandler_InitLL),
		ENTRY("NewL-test_CWSStarPassportHandler_NewL", MT_CWSStarPassportHandler_NewLL),
		ENTRY("InvokeL-test1_CWSStarPassportHandler_InvokeL", MT_CWSStarPassportHandler_InvokeL1L),
		ENTRY("InvokeL-test2_CWSStarPassportHandler_InvokeL", MT_CWSStarPassportHandler_InvokeL2L),
		ENTRY("Direction-test_CWSStarPassportHandler_Direction", MT_CWSStarPassportHandler_DirectionL),
		ENTRY("Phase-test_CWSStarPassportHandler_Phase", MT_CWSStarPassportHandler_PhaseL),
		ENTRY("InitL-test_CWSStarPassportHandler_InitL", MT_CWSStarPassportHandler_InitLL),
		ENTRY("NewL-test_CWSStarSecurityHandler_NewL", MT_CWSStarSecurityHandler_NewLL),
		ENTRY("InvokeL-test1_CWSStarSecurityHandler_InvokeL", MT_CWSStarSecurityHandler_InvokeL1L),
		ENTRY("InvokeL-test2_CWSStarSecurityHandler_InvokeL", MT_CWSStarSecurityHandler_InvokeL2L),
		ENTRY("Direction-test_CWSStarSecurityHandler_Direction", MT_CWSStarSecurityHandler_DirectionL),
		ENTRY("Phase-test_CWSStarSecurityHandler_Phase", MT_CWSStarSecurityHandler_PhaseL),
		ENTRY("InitL-test_CWSStarSecurityHandler_InitL", MT_CWSStarSecurityHandler_InitLL),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

void CWsStarMessageHandlerTester::SetupL()
    {
        if (iLogger!=NULL)
        {
            delete iLogger;
            iLogger = NULL;
        }
        iLogger = new (ELeave)RFileLogger();
        iLogger->Connect();
        iLogger->CreateLog(_L("CWsStarMessageHandlerTester"),_L("CWsStarMessageHandlerTester.log"),
                                                EFileLoggingModeOverwrite);
        if ( iParser!= NULL )
        {
            delete iParser;
    	    iParser = NULL;
        }
        iParser = CSenXmlReader::NewL();
        User::LeaveIfNull(iParser);

        if ( iMessageContextOut!= NULL )
        {
            delete iMessageContextOut;
    	    iMessageContextOut = NULL;
        }
        iMessageContextOut = CWSStarMessageContext::NewL(SenContext::EOutgoing, iParser);
        User::LeaveIfNull(iMessageContextOut);

        if ( iMessageContextIn!= NULL )
        {
            delete iMessageContextIn;
    	    iMessageContextIn = NULL;
        }
        iMessageContextIn = CWSStarMessageContext::NewL(SenContext::EIncoming, iParser);
        User::LeaveIfNull(iMessageContextIn);
    }

void CWsStarMessageHandlerTester::SetupAddressingL()
    {
        SetupL();
        if ( iHandler!= NULL )
        {
            delete iHandler;
    	    iHandler = NULL;
        }
        if ( iHandlerCtx!= NULL )
        {
            delete iHandlerCtx;
    	    iHandlerCtx = NULL;
        }
        iHandlerCtx = CWSStarHandlerContext::NewL();
        iHandlerCtx->Add(HandlerContextKey::KLogger(), iLogger);
        MSenHandlerContext& hCtx = (MSenHandlerContext&)(*iHandlerCtx);
        TAny* param = reinterpret_cast<TAny*>(&hCtx);
        iHandler = CWSStarAddressingHandler::NewL(param);
    }


void CWsStarMessageHandlerTester::SetupEnveloperL()
    {
        SetupL();
        if ( iHandler!= NULL )
        {
            delete iHandler;
    	    iHandler = NULL;
        }
        if ( iHandlerCtx!= NULL )
        {
            delete iHandlerCtx;
    	    iHandlerCtx = NULL;
        }
        iHandlerCtx = CWSStarHandlerContext::NewL();
        iHandlerCtx->Add(HandlerContextKey::KLogger(), iLogger);
        MSenHandlerContext& hCtx = (MSenHandlerContext&)(*iHandlerCtx);
        TAny* param = reinterpret_cast<TAny*>(&hCtx);
        iHandler = CWSStarEnveloperHandler::NewL(param);
    }


void CWsStarMessageHandlerTester::SetupCredentialCollectorL()
    {
        SetupL();
        if ( iHandler!= NULL )
        {
            delete iHandler;
    	    iHandler = NULL;
        }
        if ( iHandlerCtx!= NULL )
        {
            delete iHandlerCtx;
    	    iHandlerCtx = NULL;
        }
        iHandlerCtx = CWSStarHandlerContext::NewL();
        iHandlerCtx->Add(HandlerContextKey::KLogger(), iLogger);
        MSenHandlerContext& hCtx = (MSenHandlerContext&)(*iHandlerCtx);
        TAny* param = reinterpret_cast<TAny*>(&hCtx);
        iHandler = CWSStarCredentialCollectorHandler::NewL(param);
    }

void CWsStarMessageHandlerTester::SetupPassportL()
    {
        SetupL();
        if ( iHandler!= NULL )
        {
            delete iHandler;
    	    iHandler = NULL;
        }
        if ( iHandlerCtx!= NULL )
        {
            delete iHandlerCtx;
    	    iHandlerCtx = NULL;
        }
        iHandlerCtx = CWSStarHandlerContext::NewL();
        iHandlerCtx->Add(HandlerContextKey::KLogger(), iLogger);
        MSenHandlerContext& hCtx = (MSenHandlerContext&)(*iHandlerCtx);
        TAny* param = reinterpret_cast<TAny*>(&hCtx);
        iHandler = CWSStarPassportHandler::NewL(param);
    }

void CWsStarMessageHandlerTester::SetupSecurityL()
    {
        SetupL();
        if ( iHandler!= NULL )
        {
            delete iHandler;
    	    iHandler = NULL;
        }
        if ( iHandlerCtx!= NULL )
        {
            delete iHandlerCtx;
    	    iHandlerCtx = NULL;
        }
        iHandlerCtx = CWSStarHandlerContext::NewL();
        iHandlerCtx->Add(HandlerContextKey::KLogger(), iLogger);
        iHandler = CWSStarSecurityHandler::NewL(iHandlerCtx);
    }

void CWsStarMessageHandlerTester::Teardown(  )
    {
        if (iHandler!= NULL)
        {
            delete iHandler;
            iHandler = NULL;
        }
        if (iHandlerCtx!=NULL)
        {
            delete iHandlerCtx;
            iHandlerCtx = NULL;
        }
        if ( iParser!= NULL )
        {
            delete iParser;
        	iParser = NULL;
        }
        if ( iMessageContextOut!= NULL )
        {
            delete iMessageContextOut;
        	iMessageContextOut = NULL;
        }
        if ( iMessageContextIn!= NULL )
        {
            delete iMessageContextIn;
    	    iMessageContextIn = NULL;
        }
        if (iLogger!=NULL)
        {
            iLogger->CloseLog();
            iLogger->Close();
            delete iLogger;
            iLogger = NULL;    
        }
    }


TInt CWsStarMessageHandlerTester::MT_CWSStarAddressingHandler_NewLL(  )
    {
        CWSStarAddressingHandler* handler = NULL; 
        CWSStarHandlerContext* pCtx = CWSStarHandlerContext::NewL();
        pCtx->Add(HandlerContextKey::KLogger(), iLogger);
        MSenHandlerContext& hCtx = (MSenHandlerContext&)(*pCtx);
        TAny* param = reinterpret_cast<TAny*>(&hCtx);
        __ASSERT_ALWAYS_NO_LEAVE(handler = CWSStarAddressingHandler::NewL(param));
        TL(handler != (CWSStarAddressingHandler*)NULL);
        __ASSERT_ALWAYS_NO_LEAVE(delete handler);
        __ASSERT_ALWAYS_NO_LEAVE(delete pCtx);
        pCtx = NULL;
        handler = NULL;
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarAddressingHandler_InvokeL1L()
    {
        SetupAddressingL();
        CSenSoapMessage* message = CSenSoapMessage::NewL(ESOAP12);
        iMessageContextOut->Add(SenContext::KSenCurrentSoapMessageCtxKey,message);
        iMessageContextOut->Update(WSStarContextKeys::KTo,_L8("endpoint"));
        iMessageContextOut->Update(WSStarContextKeys::KAction,_L8("action"));
        iMessageContextOut->Update(WSStarContextKeys::KRelatesTo,_L8("relatesTo"));
        iMessageContextOut->Update(WSStarContextKeys::KFrom,_L8("from"));
        iMessageContextOut->Update(WSStarContextKeys::KReplyTo,_L8("replyTo"));
        iMessageContextOut->Update(WSStarContextKeys::KFaultTo,_L8("faultTo"));
        TL(iHandler->InvokeL(*iMessageContextOut)== KErrNone);

        message->SetReader(*iParser);
        message->BuildFrom(KFaultMessage12);
        iMessageContextIn->Add(SenContext::KSenCurrentSoapMessageCtxKey,message);
        TL(iHandler->InvokeL(*iMessageContextIn) == KErrNone);
        LOCAL_ASSERT(iMessageContextIn->GetDesC8L(WSStarContextKeys::KTo)->Compare(_L8("endpoint"))==0);
        LOCAL_ASSERT(iMessageContextIn->GetDesC8L(WSStarContextKeys::KAction)->Compare(_L8("action"))==0);
        LOCAL_ASSERT(iMessageContextIn->GetDesC8L(WSStarContextKeys::KRelatesTo)->Compare(_L8("relatesTo"))==0);
        LOCAL_ASSERT(iMessageContextIn->GetDesC8L(WSStarContextKeys::KFrom)->Compare(_L8("from"))==0);
        LOCAL_ASSERT(iMessageContextIn->GetDesC8L(WSStarContextKeys::KReplyTo)->Compare(_L8("replyTo"))==0);
        LOCAL_ASSERT(iMessageContextIn->GetDesC8L(WSStarContextKeys::KFaultTo)->Compare(_L8("faultTo"))==0);
        LOCAL_ASSERT(*iMessageContextIn->GetIntL(WSStarSession::KErrorLayer) == WSStarSession::EAddressing);
        LOCAL_ASSERT(*iMessageContextIn->GetIntL(WSStarContextKeys::KRetryAfter) == 500);
        
        iMessageContextOut->Reset();
        __ASSERT_ALWAYS_NO_LEAVE(delete message);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarAddressingHandler_InvokeL2L()
    {
    	SetupAddressingL();
        CSenSoapMessage* message = CSenSoapMessage::NewL(ESOAP12);
        TL(iHandler->InvokeL(*iMessageContextOut)== KErrNotFound);
        iMessageContextOut->Add(SenContext::KSenCurrentSoapMessageCtxKey,message);
        TL(iHandler->InvokeL(*iMessageContextOut)== KErrNotFound);
        iMessageContextOut->Update(WSStarContextKeys::KTo,_L8("endpoint"));
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNotFound);
        iMessageContextOut->Update(WSStarContextKeys::KAction,_L8("action"));
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNone);
        TL(*iMessageContextOut->GetDesC8L(WSStarContextKeys::KReplyTo) == WSStarAddressing::KReplyToAnonymous200508);
        CSenElement& body = message->BodyL();
        CSenElement& applies = body.AddElementL(WSPolicy::KPolicyNamespace200409,
                                                        WSPolicy::KAppliesToTag,
                                                        WSPolicy::KAppliesToQTag);
        SenXmlUtils::AddAttributeL(applies, WSStarContextKeys::KAddressAttrMark, _L8("appliesTo"));
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNone);
        iMessageContextOut->Reset();
        __ASSERT_ALWAYS_NO_LEAVE(delete message);
        Teardown();
        return KErrNone;
        }

TInt CWsStarMessageHandlerTester::MT_CWSStarAddressingHandler_InvokeL3L()
    {
    	SetupAddressingL();
        CSenSoapMessage* message = CSenSoapMessage::NewL(ESOAP11);
        message->SetReader(*iParser);
        message->BuildFrom(KFaultMessage11);
        iMessageContextIn->Add(SenContext::KSenCurrentSoapMessageCtxKey,message);
        TL(iHandler->InvokeL(*iMessageContextIn) == KErrNone);
        LOCAL_ASSERT(*iMessageContextIn->GetIntL(WSStarSession::KErrorLayer) == WSStarSession::EAddressing);
        iMessageContextIn->Reset();
        __ASSERT_ALWAYS_NO_LEAVE(delete message);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarAddressingHandler_InvokeL4L()
    {
    	SetupAddressingL();
        CSenSoapMessage* message = CSenSoapMessage::NewL(ESOAP11);
        message->SetReader(*iParser);
        message->BuildFrom(KFaultMessage11_1);
        iMessageContextIn->Add(SenContext::KSenCurrentSoapMessageCtxKey,message);
        TL(iHandler->InvokeL(*iMessageContextIn) == KErrNone);
        LOCAL_ASSERT(*iMessageContextIn->GetIntL(WSStarSession::KErrorLayer) == WSStarSession::EAddressing);
        iMessageContextIn->Reset();
        __ASSERT_ALWAYS_NO_LEAVE(delete message);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarAddressingHandler_InvokeL5L()
    {
    	SetupAddressingL();
        CSenSoapMessage* message = CSenSoapMessage::NewL(ESOAP11);
        message->SetReader(*iParser);
        message->BuildFrom(KFaultMessage11_2);
        iMessageContextIn->Add(SenContext::KSenCurrentSoapMessageCtxKey,message);
        TL(iHandler->InvokeL(*iMessageContextIn) == KErrNone);
        LOCAL_ASSERT(*iMessageContextIn->GetIntL(WSStarSession::KErrorLayer) == WSStarSession::EAddressing);
        iMessageContextIn->Reset();
        __ASSERT_ALWAYS_NO_LEAVE(delete message);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarAddressingHandler_InvokeL6L()
    {
    	SetupAddressingL();
        CSenSoapMessage* message = CSenSoapMessage::NewL(ESOAP11);
        message->SetReader(*iParser);
        message->BuildFrom(KFaultMessage11_3);
        iMessageContextIn->Add(SenContext::KSenCurrentSoapMessageCtxKey,message);
        TL(iHandler->InvokeL(*iMessageContextIn) == KErrNone);
        LOCAL_ASSERT(*iMessageContextIn->GetIntL(WSStarSession::KErrorLayer) == WSStarSession::EAddressing);
        iMessageContextIn->Reset();
        __ASSERT_ALWAYS_NO_LEAVE(delete message);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarAddressingHandler_DirectionL(  )
    {
    	SetupAddressingL();
        LOCAL_ASSERT(iHandler->Direction() == SenHandler::EBoth);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarAddressingHandler_PhaseL(  )
    {
    	SetupAddressingL();
        LOCAL_ASSERT(iHandler->Phase() == SenHandler::EMessage);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarAddressingHandler_InitLL(  )
    {
        SetupAddressingL();  
        CWSStarHandlerContext* pCtx = CWSStarHandlerContext::NewL();
        pCtx->Add(HandlerContextKey::KLogger(), iLogger);
        MSenHandlerContext& hCtx = (MSenHandlerContext&)(*pCtx);
        __ASSERT_ALWAYS_NO_LEAVE(iHandler->InitL(hCtx));
        __ASSERT_ALWAYS_NO_LEAVE(delete pCtx);
        pCtx = NULL;
        Teardown();
        return KErrNone;
    }

//enveloper


TInt CWsStarMessageHandlerTester::MT_CWSStarEnveloperHandler_NewLL(  )
    {
    	SetupEnveloperL();
        CWSStarEnveloperHandler* handler = NULL; 
        CWSStarHandlerContext* pCtx = CWSStarHandlerContext::NewL();
        pCtx->Add(HandlerContextKey::KLogger(), iLogger);
        MSenHandlerContext& hCtx = (MSenHandlerContext&)(*pCtx);
        TAny* param = reinterpret_cast<TAny*>(&hCtx);
        __ASSERT_ALWAYS_NO_LEAVE(handler = CWSStarEnveloperHandler::NewL(param));
        TL(handler != (CWSStarEnveloperHandler*)NULL);
        __ASSERT_ALWAYS_NO_LEAVE(delete handler);
        __ASSERT_ALWAYS_NO_LEAVE(delete pCtx);
        pCtx = NULL;
        handler = NULL;
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarEnveloperHandler_InvokeL1L()
    {
    	SetupEnveloperL();
        iMessageContextOut->Update(WSStarContextKeys::KSoapVersion,ESOAP11);
        iHandlerCtx->Update(HandlerContextKey::KVersion,KSecurityXmlNs);
        CSenBaseElement* element = CSenBaseElement::NewL(_L8("elementBody")); 
        iMessageContextOut->Update(WSStarContextKeys::KBodyElement,element);
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNone);

        CSenSoapMessage* message = iMessageContextOut->GetCurrentSoapMessage();
        __ASSERT_ALWAYS_NO_LEAVE(delete message);
        iMessageContextOut->Reset();
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarEnveloperHandler_InvokeL2L()
    {
    	SetupEnveloperL();
        iHandlerCtx->Update(HandlerContextKey::KVersion,KSecuritySchemeXmlNs);
        iMessageContextOut->Update(WSStarContextKeys::KBody,_L8("body content"));
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNone);

        CSenSoapMessage* message = iMessageContextOut->GetCurrentSoapMessage();

        __ASSERT_ALWAYS_NO_LEAVE(delete message);
        iMessageContextOut->Reset();
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarEnveloperHandler_DirectionL(  )
    {
    	SetupEnveloperL();
        LOCAL_ASSERT(iHandler->Direction() == SenHandler::EOutgoing);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarEnveloperHandler_PhaseL(  )
    {
    	SetupEnveloperL();
        LOCAL_ASSERT(iHandler->Phase() == SenHandler::EMessage);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarEnveloperHandler_InitLL(  )
    {
    	SetupEnveloperL();
        CWSStarHandlerContext* pCtx = CWSStarHandlerContext::NewL();
        pCtx->Add(HandlerContextKey::KLogger(), iLogger);
        MSenHandlerContext& hCtx = (MSenHandlerContext&)(*pCtx);
        __ASSERT_ALWAYS_NO_LEAVE(iHandler->InitL(hCtx));
        __ASSERT_ALWAYS_NO_LEAVE(delete pCtx);
        pCtx = NULL;
        Teardown();
        return KErrNone;        
    }

//credential collector


TInt CWsStarMessageHandlerTester::MT_CWSStarCredentialCollectorHandler_NewLL(  )
    {
    	SetupCredentialCollectorL();
        CWSStarCredentialCollectorHandler* handler = NULL; 
        CWSStarHandlerContext* pCtx = CWSStarHandlerContext::NewL();
        pCtx->Add(HandlerContextKey::KLogger(), iLogger);
        MSenHandlerContext& hCtx = (MSenHandlerContext&)(*pCtx);
        TAny* param = reinterpret_cast<TAny*>(&hCtx);
        __ASSERT_ALWAYS_NO_LEAVE(handler = CWSStarCredentialCollectorHandler::NewL(param));
        TL(handler != (CWSStarCredentialCollectorHandler*)NULL);
        __ASSERT_ALWAYS_NO_LEAVE(delete handler);
        __ASSERT_ALWAYS_NO_LEAVE(delete pCtx);
        pCtx = NULL;
        handler = NULL;
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarCredentialCollectorHandler_InvokeL1L()
    {
    	SetupCredentialCollectorL();
        CSenSoapMessage* message = CSenSoapMessage::NewL(ESOAP12);
        iMessageContextOut->Add(SenContext::KSenCurrentSoapMessageCtxKey,message);
        iMessageContextOut->Update(WSStarContextKeys::KSecurityToken,_L8("security token"));
        iMessageContextOut->Update(WSStarContextKeys::KTimestampCreated,_L8("2007-02-22T15:30:00Z"));
        iMessageContextOut->Update(WSStarContextKeys::KTimestampExpires,_L8("timestamp expired"));
        iMessageContextOut->Update(WSStarContextKeys::KPhoneTimeWhenMTResolved,_L8("2007-02-22T15:31:00Z"));
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNone);
        iMessageContextOut->Reset();
        __ASSERT_ALWAYS_NO_LEAVE(delete message);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarCredentialCollectorHandler_InvokeL2L()
    {
    	SetupCredentialCollectorL();
        CSenSoapMessage* message = CSenSoapMessage::NewL(ESOAP12);
        iMessageContextOut->Add(SenContext::KSenCurrentSoapMessageCtxKey,message);
        iMessageContextOut->Update(WSStarContextKeys::KSecurityToken,_L8("security token"));
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNone);

        iMessageContextOut->Reset();
        __ASSERT_ALWAYS_NO_LEAVE(delete message);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarCredentialCollectorHandler_InvokeL3L()
    {
    	SetupCredentialCollectorL();
        CSenSoapMessage* message = CSenSoapMessage::NewL(ESOAP12);
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNotFound);
        iMessageContextOut->Add(SenContext::KSenCurrentSoapMessageCtxKey,message);
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNone);

        iMessageContextOut->Reset();
        __ASSERT_ALWAYS_NO_LEAVE(delete message);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarCredentialCollectorHandler_DirectionL(  )
    {
    	SetupCredentialCollectorL();
        LOCAL_ASSERT(iHandler->Direction() == SenHandler::EOutgoing);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarCredentialCollectorHandler_PhaseL(  )
    {
    	SetupCredentialCollectorL();
        LOCAL_ASSERT(iHandler->Phase() == SenHandler::EMessage);
        Teardown();
        return KErrNone;    
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarCredentialCollectorHandler_InitLL(  )
    {
    	SetupCredentialCollectorL();
        CWSStarHandlerContext* pCtx = CWSStarHandlerContext::NewL();
        pCtx->Add(HandlerContextKey::KLogger(), iLogger);
        MSenHandlerContext& hCtx = (MSenHandlerContext&)(*pCtx);
        __ASSERT_ALWAYS_NO_LEAVE(iHandler->InitL(hCtx));
        __ASSERT_ALWAYS_NO_LEAVE(delete pCtx);
        pCtx = NULL;
        Teardown();
        return KErrNone;
    }

//passport


TInt CWsStarMessageHandlerTester::MT_CWSStarPassportHandler_NewLL(  )
    {
    	SetupPassportL();
        CWSStarPassportHandler* handler = NULL; 
        CWSStarHandlerContext* pCtx = CWSStarHandlerContext::NewL();
        pCtx->Add(HandlerContextKey::KLogger(), iLogger);
        MSenHandlerContext& hCtx = (MSenHandlerContext&)(*pCtx);
        TAny* param = reinterpret_cast<TAny*>(&hCtx);
        __ASSERT_ALWAYS_NO_LEAVE(handler = CWSStarPassportHandler::NewL(param));
        TL(handler != (CWSStarPassportHandler*)NULL);
        __ASSERT_ALWAYS_NO_LEAVE(delete handler);
        __ASSERT_ALWAYS_NO_LEAVE(delete pCtx);
        pCtx = NULL;
        handler = NULL;
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarPassportHandler_InvokeL1L()
    {
    	SetupPassportL();
        CSenSoapMessage* message = CSenSoapMessage::NewL(ESOAP11);
        CSenElement& body = message->BodyL();
        body.AddElementL(WSStarContextKeys::KMultiReqTagMark);
        iMessageContextOut->Add(SenContext::KSenCurrentSoapMessageCtxKey,message);
        iMessageContextOut->Update(WSStarContextKeys::KPassportEnabled,ETrue);
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNone);

        __ASSERT_ALWAYS_NO_LEAVE(delete message);
        iMessageContextOut->Reset();
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarPassportHandler_InvokeL2L()
    {
    	SetupPassportL();
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNotFound);
        iMessageContextOut->Reset();
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarPassportHandler_DirectionL(  )
    {
    	SetupPassportL();
        LOCAL_ASSERT(iHandler->Direction() == SenHandler::EOutgoing);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarPassportHandler_PhaseL(  )
    {
    	SetupPassportL();
        LOCAL_ASSERT(iHandler->Phase() == SenHandler::EMessage);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarPassportHandler_InitLL(  )
    {
    	SetupPassportL();
        CWSStarHandlerContext* pCtx = CWSStarHandlerContext::NewL();
        pCtx->Add(HandlerContextKey::KLogger(), iLogger);
        MSenHandlerContext& hCtx = (MSenHandlerContext&)(*pCtx);
        __ASSERT_ALWAYS_NO_LEAVE(iHandler->InitL(hCtx));
        __ASSERT_ALWAYS_NO_LEAVE(delete pCtx);
        pCtx = NULL;
        Teardown();
        return KErrNone;    
    }

//security


TInt CWsStarMessageHandlerTester::MT_CWSStarSecurityHandler_NewLL(  )
    {
    	SetupSecurityL();
        CWSStarSecurityHandler* handler = NULL; 
        CWSStarHandlerContext* pCtx = CWSStarHandlerContext::NewL();
        pCtx->Add(HandlerContextKey::KLogger(), iLogger);
        __ASSERT_ALWAYS_NO_LEAVE(handler = CWSStarSecurityHandler::NewL(pCtx));
        TL(handler != (CWSStarSecurityHandler*)NULL);
        __ASSERT_ALWAYS_NO_LEAVE(delete handler);
        __ASSERT_ALWAYS_NO_LEAVE(delete pCtx);
        pCtx = NULL;
        handler = NULL;
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarSecurityHandler_InvokeL1L()
    {
    	SetupSecurityL();
        CSenSoapMessage* message = CSenSoapMessage::NewL(ESOAP12);
        CSenElement& header = message->HeaderL();
        CSenElement& security = header.AddElementL(_L8("http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"),
                                                   _L8("Security"),
                                                   _L8("wsse"));  
        security.AddElementL(_L8("http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd"),
                             _L8("Timestamp"),
                             _L8("wsu"));
        CSenElement& body = message->BodyL();
        iMessageContextOut->Update(WSStarContextKeys::KPOPBase64,_L8("WmPjJkStecgGm0SlT7ORuQ=="));
        iMessageContextOut->Update(WSStarContextKeys::KBinaryType,WSStarContextValues::KPOPTypeSimmetric);
        iMessageContextOut->Update(WSStarContextKeys::KSTR,
                                   _L8("<wsse:SecurityTokenReference xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\"><wsse:KeyIdentifier ValueType=\"http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.0#SAMLAssertionID\">uuid-8222b7a2-3874-4884-bdb5-9c2ddd4b86b5-16</wsse:KeyIdentifier></wsse:SecurityTokenReference>"));
        iMessageContextOut->Add(SenContext::KSenCurrentSoapMessageCtxKey,message);
        iMessageContextOut->SetOwnedEnvelope();
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNone);

//        __ASSERT_ALWAYS_NO_LEAVE(delete message);
        iMessageContextOut->Reset();
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarSecurityHandler_InvokeL2L()
    {
    	SetupSecurityL();
        CSenSoapMessage* message = CSenSoapMessage::NewL(ESOAP12);
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNone);
        iMessageContextOut->Update(WSStarContextKeys::KPOPBase64,_L8("abcd!%@"));
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNone);
        iMessageContextOut->Update(WSStarContextKeys::KPOPBase64,_L8("WmPjJkStecgGm0SlT7ORuQ=="));
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNone);
        iMessageContextOut->SetOwnedEnvelope();
        iMessageContextOut->Add(SenContext::KSenCurrentSoapMessageCtxKey,message);
        iMessageContextOut->Update(WSStarContextKeys::KBinaryType,_L8("adcedf"));
        *((RPointerArray<HBufC8>*)iMessageContextOut->GetAnyL(WSStarContextKeys::KSignedPartsArray)) = 	
                iSignedParts;	//codescannerwarnings
        TL(iHandler->InvokeL(*iMessageContextOut) == KErrNone);
        CSenElement& body = message->BodyL();
        body.AddNamespaceL(_L8("wsu"),_L8("abscderf"));
        SenXmlUtils::AddAttributeL(body, _L8("wsu:qqq"), _L8("qqq"));
        //EUNIT_ASSERT_SPECIFIC_LEAVE(iHandler->InvokeL(*iMessageContextOut), KErrCorrupt);
        TRAPD(leave, iHandler->InvokeL(*iMessageContextOut));
        if(leave != KErrCorrupt)
        	User::Leave(KErrGeneral);
        
        iMessageContextOut->Reset();
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarSecurityHandler_DirectionL(  )
    {
    	SetupSecurityL();
        LOCAL_ASSERT(iHandler->Direction() == SenHandler::EBoth);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarSecurityHandler_PhaseL(  )
    {
    	SetupSecurityL();
        LOCAL_ASSERT(iHandler->Phase() == SenHandler::EMessage);
        Teardown();
        return KErrNone;
    }

TInt CWsStarMessageHandlerTester::MT_CWSStarSecurityHandler_InitLL(  )
    {
    	SetupSecurityL();
        CWSStarHandlerContext* pCtx = CWSStarHandlerContext::NewL();
        pCtx->Add(HandlerContextKey::KLogger(), iLogger);
        MSenHandlerContext& hCtx = (MSenHandlerContext&)(*pCtx);
        __ASSERT_ALWAYS_NO_LEAVE(iHandler->InitL(hCtx));
        __ASSERT_ALWAYS_NO_LEAVE(delete pCtx);
        pCtx = NULL;
        Teardown();
        return KErrNone;    
    }


// -----------------------------------------------------------------------------
// CWsStarMessageHandlerTester::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CWsStarMessageHandlerTester::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
