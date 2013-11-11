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
#include <e32math.h>
#include <libc/assert.h>
#include <e32panic.h>
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "newsoapclassesTester.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

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
// CnewsoapclassesTester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CnewsoapclassesTester::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CnewsoapclassesTester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CnewsoapclassesTester::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "NewL-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_NewLL ),
        ENTRY( "NewLC-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_NewLCL ),
        //ENTRY( "NewL-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_NewL_1L ),
        //ENTRY( "NewLC-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_NewLC_1L ),
        ENTRY( "NewL-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_NewL_2L ),
        ENTRY( "NewLC-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_NewLC_2L ),
        ENTRY( "Type-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_TypeL ),
        ENTRY( "Direction-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_DirectionL ),
        ENTRY( "TxnId-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_TxnIdL ),
        ENTRY( "Clone-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_CloneL ),
        ENTRY( "SoapVersion-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_SoapVersionL ),
        //ENTRY( "SetContext-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_SetContextL ),
        //ENTRY( "Context-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_ContextL ),
        ENTRY( "SetProperties-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_SetPropertiesL ),
        ENTRY( "SetBodyL-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_SetBodyLL ),
        ENTRY( "Properties-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_PropertiesL ),
        ENTRY( "IsSafeToCast-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_IsSafeToCastL ),
        
        ENTRY( "SetBodyL-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_SetBodyL_1L ),
        ENTRY( "BodyL-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_BodyLL ),
        ENTRY( "HeaderL-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_HeaderLL ),
        ENTRY( "AddHeaderL-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_AddHeaderLL ),
        ENTRY( "BodyAsStringL-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_BodyAsStringLL ),
        ENTRY( "IsFault-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_IsFaultL ),
        ENTRY( "DetachFaultL-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_DetachFaultLL ),
        ENTRY( "FaultL-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_FaultLL ),
        ENTRY( "SetSoapActionL-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_SetSoapActionLL ),
        ENTRY( "SoapAction-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_SoapActionL ),
        ENTRY( "HasHeader-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_HasHeaderL ),
        ENTRY( "HasBody-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_HasBodyL ),
        ENTRY( "Parse1-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_Parse1L ),
        ENTRY( "Parse2-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_Parse2L ),
        ENTRY( "Parse2-CSenSoapEnvelope2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_Parse3L ),
        ENTRY( "NewL-CSenSoapFault2", CnewsoapclassesTester::MT_CSenSoapFault2_NewLL ),
        ENTRY( "NewL-CSenSoapFault2", CnewsoapclassesTester::MT_CSenSoapFault2_NewL_1L ),
        ENTRY( "NewL-CSenSoapFault2", CnewsoapclassesTester::MT_CSenSoapFault2_NewL_2L ),
        ENTRY( "FaultCode-CSenSoapFault2", CnewsoapclassesTester::MT_CSenSoapFault2_FaultCodeL ),
        ENTRY( "FaultSubcode-CSenSoapFault2", CnewsoapclassesTester::MT_CSenSoapFault2_FaultSubcodeL ),
        ENTRY( "FaultString-CSenSoapFault2", CnewsoapclassesTester::MT_CSenSoapFault2_FaultStringL ),
        ENTRY( "FaultActor-CSenSoapFault2", CnewsoapclassesTester::MT_CSenSoapFault2_FaultActorL ),
        ENTRY( "Detail-CSenSoapFault2", CnewsoapclassesTester::MT_CSenSoapFault2_DetailL ),
        ENTRY( "NewL-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewLL ),
        ENTRY( "NewLC-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewLCL ),
        ENTRY( "NewL-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewL_1L ),
        ENTRY( "NewLC-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewLC_1L ),
        ENTRY( "NewL-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewL_2L ),
        ENTRY( "NewLC-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewLC_2L ),
        ENTRY( "NewL-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewL_3L ),
        ENTRY( "NewLC-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewLC_3L ),
        //ENTRY( "NewL-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewL_4L ),
        //ENTRY( "NewLC-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewLC_4L ),
        //ENTRY( "NewL-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewL_5L ),
        //ENTRY( "NewLC-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewLC_5L ),
        //ENTRY( "NewL-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewL_6L ),
        //ENTRY( "NewLC-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_NewLC_6L ),
        ENTRY( "NewL-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapEnvelope2_NewLL ),
        ENTRY( "Type-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_TypeL ),
        ENTRY( "Clone-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_CloneL ),
		ENTRY( "Parse-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_Parse1L ),        
        ENTRY( "SetSecurityHeaderL-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_SetSecurityHeaderLL ),
        ENTRY( "AddSecurityTokenL-CSenSoapMessage2", CnewsoapclassesTester::MT_CSenSoapMessage2_AddSecurityTokenLL ),
        ENTRY( "NewL-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_NewLL ),
        ENTRY( "NewLC-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_NewLCL ),
        ENTRY( "NewL-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_NewL_1L ),
        ENTRY( "NewLC-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_NewLC_1L ),
        ENTRY( "NewL-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_NewL_2L ),
        ENTRY( "NewLC-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_NewLC_2L ),
        ENTRY( "UsernameTokenL-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_UsernameTokenLL ),
        ENTRY( "UsernameTokenL-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_UsernameTokenL_1L ),
        ENTRY( "UsernameTokenL-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_UsernameTokenL_2L ),
        ENTRY( "UsernameTokenL-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_UsernameTokenL_3L ),
        ENTRY( "UsernameTokenL-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_UsernameTokenL_4L ),
        ENTRY( "TimestampL-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_TimestampLL ),
        ENTRY( "TimestampL-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_TimestampL_1L ),
        ENTRY( "XmlNs-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_XmlNsL ),
        ENTRY( "XmlNsPrefix-CSenWsSecurityHeader2", CnewsoapclassesTester::MT_CSenWsSecurityHeader2_XmlNsPrefixL ),
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", CnewsoapclassesTester::LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( CnewsoapclassesTester::PrintTest, ETrue, 1, 3 ),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CnewsoapclassesTester::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CnewsoapclassesTester::SetupL(  ){

	} 

void CnewsoapclassesTester::Teardown(  ){

	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_NewLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KEnvelopeString, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);
    HBufC8* pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);
    
    if(!( *pAsXml == KEnvelopeString )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
    }
    
TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_NewLCL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KEnvelopeString, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewLC();
    HBufC8* pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);
    
    if(!( *pAsXml == KEnvelopeString )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pEnvelope);
    //CleanupStack::PopAndDestroy();
    
    Teardown();
    return KErrNone;    
    }    
/*    
TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_NewL_1L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KEnvelopeString, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
	MSenMessageContext* mContext = NULL;
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL(*mContext);
    CleanupStack::PushL(pEnvelope);
    HBufC8* pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);
    
    if(!( *pAsXml == KEnvelopeString )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pEnvelope);    
    Teardown();
    return KErrNone;    
	}


TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_NewLC_1L( CStifItemParser& aItem )
	{
	 SetupL();
	 _LIT8(KEnvelopeString, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
	 MSenMessageContext* mContext = NULL;
	 CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewLC(*mContext);
	 HBufC8* pAsXml = pEnvelope->AsXmlL();
	 CleanupStack::PushL(pAsXml);
     if(!( *pAsXml == KEnvelopeString )) return KErrArgument;
     CleanupStack::PopAndDestroy(pAsXml);
     CleanupStack::PopAndDestroy(pEnvelope);
     //CleanupStack::PopAndDestroy();
    Teardown();
    return KErrNone;    
	}
 */   
TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_NewL_2L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KEnvelopeString, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    CSenSoapEnvelope2* pEnvelope, *pEnvelope1 = CSenSoapEnvelope2::NewL();
    pEnvelope = CSenSoapEnvelope2::NewL(*pEnvelope1);
    CleanupStack::PushL(pEnvelope);

    
    HBufC8* pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);
    
    if(!( *pAsXml == KEnvelopeString )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pEnvelope);
    delete pEnvelope1;
    pEnvelope1 = NULL;
    Teardown();
    return KErrNone;    
	}
    
TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_NewLC_2L( CStifItemParser& aItem )
    {
	SetupL();
   _LIT8(KEnvelopeString, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    CSenSoapEnvelope2* pEnvelope, *pEnvelope1 = CSenSoapEnvelope2::NewL();
    pEnvelope = CSenSoapEnvelope2::NewLC(*pEnvelope1);
    
    HBufC8* pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);
    
    if(!( *pAsXml == KEnvelopeString )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pAsXml);
    
    CleanupStack::PopAndDestroy(pEnvelope);
    //CleanupStack::PopAndDestroy();
    delete pEnvelope1;
    pEnvelope1 = NULL;
    Teardown();
    return KErrNone;    
	}
    
TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_TypeL( CStifItemParser& aItem )
	{
	SetupL();
	MSenMessage::TClass var = MSenMessage::ESoapEnvelope2;
	_LIT8(KEnvelopeString, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);
    HBufC8* pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);
    
    if(!( *pAsXml == KEnvelopeString )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    
    TL(var == pEnvelope->Type());
    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}
	
TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_DirectionL( CStifItemParser& aItem )
	{
	SetupL();
	MSenMessage::TDirection var = MSenMessage::EOutbound;
	_LIT8(KEnvelopeString, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);
    HBufC8* pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);
    
    if(!( *pAsXml == KEnvelopeString )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    
    TL(var == pEnvelope->Direction());
    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}
	
TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_TxnIdL( CStifItemParser& aItem )
	{
	SetupL();
	_LIT8(KEnvelopeString, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    CSenSoapEnvelope2* pEnvelope, *pEnvelope1 = CSenSoapEnvelope2::NewL();
    pEnvelope = CSenSoapEnvelope2::NewL(*pEnvelope1);
    
    HBufC8* pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);
    
    if(!( *pAsXml == KEnvelopeString )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pAsXml);
    
    TInt var = pEnvelope->TxnId();
    TBool Flag;
    
    if(var >= 0)
    if(!(Flag)) return KErrArgument;
    
    __ASSERT_ALWAYS_NO_LEAVE(delete pEnvelope);
    pEnvelope = NULL;
    delete pEnvelope1;
    pEnvelope1 = NULL;
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_CloneL( CStifItemParser& aItem )
	{
	SetupL();
	TBool Flag;
	CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
	CleanupStack::PushL(pEnvelope);
	CSenSoapEnvelope2* pClone = NULL;
    pClone = (CSenSoapEnvelope2*)pEnvelope->CloneL(); 
    if(pClone != NULL)
    	Flag = 1;
    if(!(Flag)) return KErrArgument;
   
	delete pClone;
    CleanupStack::PopAndDestroy(pEnvelope);
    
    Teardown();
    return KErrNone;    
	}
	
TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_SoapVersionL( CStifItemParser& aItem )
	{
	SetupL();
	TSOAPVersion var = ESOAP11;
	_LIT8(KEnvelopeString, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);
    HBufC8* pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);
    
    if(!( *pAsXml == KEnvelopeString )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    
    TL(var == pEnvelope->SoapVersion());
    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}
/*	
TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_SetContextL( CStifItemParser& aItem )
	{
	SetupL();
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);
    MSenMessageContext* mContext = NULL;
   
	CSenXmlProperties* prop = CSenXmlProperties::NewL();
	CSenMessageContext* mContext1 = CSenMessageContext::NewL(SenContext::EOutgoing, prop);
    TL(KErrArgument == pEnvelope->SetContext(mContext));
    TL(KErrNone == pEnvelope->SetContext(mContext1));
    TL(mContext1 == pEnvelope->Context());
    CleanupStack::PopAndDestroy(pEnvelope);
    delete mContext1;
    Teardown();
    return KErrNone;    
	}
	
TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_ContextL( CStifItemParser& aItem )
	{
	SetupL();
	TTestResult Result;	
	CnewsoapclassesTester::MT_CSenSoapEnvelope2_SetContextL(Result);
    Teardown();
    return KErrNone;    
	}
*/
TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_SetPropertiesL( CStifItemParser& aItem )
	{
	SetupL();
    CSenXmlProperties* prop = CSenXmlProperties::NewL();
    CleanupStack::PushL(prop);
    
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    
    CleanupStack::Pop();
    CleanupStack::PushL(pEnvelope);
    
    
    TL(KErrNone == pEnvelope->SetProperties(prop));
    CleanupStack::PopAndDestroy();
    
    
    Teardown();
    return KErrNone;    
	}
	
TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_PropertiesL( CStifItemParser& aItem )
	{
	SetupL();
    CSenXmlProperties* prop = CSenXmlProperties::NewL();
    CleanupStack::PushL(prop);
    
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    
    CleanupStack::Pop();
    CleanupStack::PushL(pEnvelope);
    
    
    TL(KErrNone == pEnvelope->SetProperties(prop));
    TL(prop == pEnvelope->Properties());
    CleanupStack::PopAndDestroy();
    
    
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_IsSafeToCastL( CStifItemParser& aItem )
	{
	SetupL();
	MSenMessage::TClass var = MSenMessage::ESoapEnvelope2;
	TBool retVal = ETrue;
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    
    CleanupStack::PushL(pEnvelope);
    
    
    retVal == pEnvelope->IsSafeToCast(var);


    if(retVal == EFalse)
    	return KErrArgument;
    
    CleanupStack::PopAndDestroy();
    Teardown();
    return KErrNone;    
	}
	

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_SetBodyLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KBodyContentString, "<Test>Content</Test>");

    _LIT8(KEnvelopeString, "<S:Envelope \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Body><Test>Content</Test></S:Body>\
</S:Envelope>");

    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);
    pEnvelope->SetBodyL(KBodyContentString);
    HBufC8* pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);

    if(!( *pAsXml == KEnvelopeString )) return KErrArgument;

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_SetBodyL_1L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KBodyElementName,   "BodyElement");
    _LIT8(KBodyContent,       "BodyContent");
    _LIT8(KBodyElementName2,  "BodyElement2");
    _LIT8(KBodyContent2,      "BodyContent2");

    _LIT8(KEnvelopeString, "<S:Envelope \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Body><BodyElement>BodyContent</BodyElement></S:Body>\
</S:Envelope>");

    _LIT8(KEnvelopeString2, "<S:Envelope \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Body><BodyElement2>BodyContent2</BodyElement2></S:Body>\
</S:Envelope>");

    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement bodyElement = document.CreateDocumentElementL(KBodyElementName());
    bodyElement.AddTextL(KBodyContent());
    pEnvelope->SetBodyL(bodyElement);

    HBufC8* pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KEnvelopeString )) KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    TXmlEngElement bodyElement2 = document.CreateDocumentElementL(KBodyElementName2());
    bodyElement2.AddTextL(KBodyContent2());
    pEnvelope->SetBodyL(bodyElement2);

    pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KEnvelopeString2 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(1); // document
    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_BodyLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KBodyContentString, "Test");
    _LIT8(KBodyString, "<S:Body>Test</S:Body>");

    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);
    pEnvelope->SetBodyL(KBodyContentString);

    TXmlEngElement element = pEnvelope->BodyL();
    RSenDocument document = pEnvelope->AsDocumentL();

    TXmlEngSerializationOptions options;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    options.iOptions = options.iOptions |
                       TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;
    RBuf8 asXml;
    CleanupClosePushL(asXml);
    document.SaveL(asXml, element, options);

    // Serialized body should contain only body as XML.
    if(!( asXml == KBodyString )) return KErrArgument;

    CleanupStack::PopAndDestroy(&asXml);

    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_HeaderLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KHeaderString, "<S:Header/>");

    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

    TXmlEngElement element = pEnvelope->HeaderL();

    RSenDocument document = pEnvelope->AsDocumentL();

    TXmlEngSerializationOptions options;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    options.iOptions = options.iOptions |
                       TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;
    RBuf8 asXml;
    CleanupClosePushL(asXml);
    document.SaveL(asXml, element, options);

    // Serialized header should contain only header as XML.
    if(!( asXml == KHeaderString )) return KErrArgument;

    CleanupStack::PopAndDestroy(&asXml);

    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_AddHeaderLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KHeaderElementName,   "HeaderElement");
    _LIT8(KHeaderContent,       "HeaderContent");
    _LIT8(KHeaderElementName2,  "HeaderElement2");
    _LIT8(KHeaderContent2,      "HeaderContent2");

    _LIT8(KEnvelope,            "<S:Envelope \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header>\
<HeaderElement>HeaderContent</HeaderElement>\
</S:Header>\
<S:Body/></S:Envelope>");

    _LIT8(KEnvelope2,            "<S:Envelope \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header>\
<HeaderElement>HeaderContent</HeaderElement>\
<HeaderElement2>HeaderContent2</HeaderElement2>\
</S:Header>\
<S:Body/></S:Envelope>");

    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement headerElement = document.CreateDocumentElementL(KHeaderElementName());
    headerElement.AddTextL(KHeaderContent());
    pEnvelope->AddHeaderL(headerElement);

    HBufC8* pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KEnvelope )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    TXmlEngElement headerElement2 = document.CreateDocumentElementL(KHeaderElementName2());
    headerElement2.AddTextL(KHeaderContent2());
    pEnvelope->AddHeaderL(headerElement2);

    pAsXml = pEnvelope->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KEnvelope2 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(1); // document
    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_BodyAsStringLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KBodyElementName,    "BodyElement");
    _LIT8(KBodyElementContent, "BodyElementContent");
    _LIT8(KBodyContent,        "BodyContent");

    _LIT8(KBodyAsString1, "<S:Body xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">BodyContent</S:Body>");
    _LIT8(KBodyAsString2, "<S:Body xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<BodyElement>BodyElementContent</BodyElement>\
</S:Body>");

    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement bodyElement = document.CreateDocumentElementL(KBodyElementName());
    bodyElement.AddTextL(KBodyElementContent());

    pEnvelope->SetBodyL(KBodyContent);

    // If Body element does not contain any elements
    // whole Body element is returned (and detached
    // => needed namespace declarations are moved into
    //    Body)
    HBufC8* pAsXml = pEnvelope->BodyAsStringL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KBodyAsString1 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    pEnvelope->SetBodyL(bodyElement);

    // If Body element contains element(s)
    // only first element (and all its child elements)
    // from Body element is returned and detached
    // (not Body element).
    pAsXml = pEnvelope->BodyAsStringL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KBodyAsString2 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(1); // document
    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}
	

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_IsFaultL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KFaultElementName,  "Fault");
    _LIT8(KFaultContent,      "FaultContent");
    _LIT8(KFaultInputString,  "<SOAP:Envelope \
xmlns:SOAP=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<SOAP:Header>\
<HeaderElement>HeaderContent</HeaderElement>\
</SOAP:Header>\
<SOAP:Body><SOAP:Fault>FaultContent</SOAP:Fault></SOAP:Body>\
</SOAP:Envelope>");

    // 1. Test that IsFault return EFalse by default
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

    //if(!( pEnvelope->IsFault() == EFalse )) return KErrArgument;
    TL( pEnvelope->IsFault() == EFalse )

    // 2. Test that IsFault returns ETrue when fault is added
    //    using SetBodyL method
    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement faultElement = document.CreateDocumentElementL(KFaultElementName(),
                                                            pEnvelope->NsUri());
    faultElement.AddTextL(KFaultContent());
    pEnvelope->SetBodyL(faultElement);

    if(!( pEnvelope->IsFault())) return KErrArgument;

    CleanupStack::PopAndDestroy(1); // document
    CleanupStack::PopAndDestroy(pEnvelope);

    // 3. Test that IsFault returns ETrue if fault is added
    //    when Envelope is parsed

    CSenParser* pParser = CSenParser::NewLC();
    pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

	pParser->ParseL(KFaultInputString, *pEnvelope);
	if(!( pEnvelope->IsFault())) return KErrArgument;

    CleanupStack::PopAndDestroy(pEnvelope);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_DetachFaultLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KFaultElementName,  "Fault");
    _LIT8(KFaultContent,      "FaultContent");
    _LIT8(KFaultElement,       "<S:Fault xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">FaultContent</S:Fault>");

    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

    TXmlEngElement body = pEnvelope->BodyL();
    TXmlEngElement faultElement = body.AddNewElementWithNsL(KFaultElementName(),
                                                      pEnvelope->NsUri());
    faultElement.AddTextL(KFaultContent());

    CSenSoapFault2* pFault = pEnvelope->DetachFaultL();
    CleanupStack::PushL(pFault);

    HBufC8* pAsXml = pFault->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KFaultElement )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pFault);

    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}
	
	

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_FaultLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KFaultElementName,  "Fault");
    _LIT8(KFaultContent,      "FaultContent");
    _LIT8(KFaultElement,       "<S:Fault>FaultContent</S:Fault>");

    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

    TXmlEngElement body = pEnvelope->BodyL();
    TXmlEngElement faultElement = body.AddNewElementWithNsL(KFaultElementName(),
                                                            pEnvelope->NsUri());
    faultElement.AddTextL(KFaultContent());

    CSenSoapFault2* pFault = pEnvelope->FaultL();
    CleanupStack::PushL(pFault);

    HBufC8* pAsXml = pFault->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KFaultElement )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pFault);

    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_SetSoapActionLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSoapActionValue,         "SoapActionValue");
    _LIT8(KSoapActionValueInQuotes, "\"SoapActionValue\"");

    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

    TPtrC8 retVal = pEnvelope->SetSoapActionL(KSoapActionValue);
    if(!( retVal == KSoapActionValueInQuotes )) return KErrArgument;

    TPtrC8 soapAction = pEnvelope->SoapAction();
    if(!( soapAction == KSoapActionValueInQuotes )) return KErrArgument;

    TPtrC8 retVal2 = pEnvelope->SetSoapActionL(KSoapActionValueInQuotes);
    if(!( retVal2 == KSoapActionValueInQuotes )) return KErrArgument;

    TPtrC8 soapAction2 = pEnvelope->SoapAction();
    if(!( soapAction2 == KSoapActionValueInQuotes )) return KErrArgument;

    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_SoapActionL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSoapActionValue,         "SoapActionValue");
    _LIT8(KSoapActionValueInQuotes, "\"SoapActionValue\"");

    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

    TPtrC8 retVal = pEnvelope->SetSoapActionL(KSoapActionValue);
    if(!( retVal == KSoapActionValueInQuotes )) return KErrArgument;

    TPtrC8 soapAction = pEnvelope->SoapAction();
    if(!( soapAction == KSoapActionValueInQuotes )) return KErrArgument;

    TPtrC8 retVal2 = pEnvelope->SetSoapActionL(KSoapActionValueInQuotes) ;
    if(!( retVal2 == KSoapActionValueInQuotes )) return KErrArgument;

    TPtrC8 soapAction2 = pEnvelope->SoapAction();
    if(!( soapAction2 == KSoapActionValueInQuotes )) return KErrArgument;

    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_HasHeaderL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KHeaderElementName,   "HeaderElement");
    _LIT8(KHeaderContent,       "HeaderContent");

    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

    if(!( pEnvelope->HasHeader() == EFalse )) return KErrArgument;

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement headerElement = document.CreateDocumentElementL(KHeaderElementName());
    headerElement.AddTextL(KHeaderContent());
    pEnvelope->AddHeaderL(headerElement);

    if(!( pEnvelope->HasHeader())) return KErrArgument;

    CleanupStack::PopAndDestroy(1); // document
    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_HasBodyL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KBodyElementName,    "BodyElement");
    _LIT8(KBodyElementContent, "BodyElementContent");

    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

    if(!( pEnvelope->HasBody() == EFalse )) return KErrArgument;

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement bodyElement = document.CreateDocumentElementL(KBodyElementName());
    bodyElement.AddTextL(KBodyElementContent());

    pEnvelope->SetBodyL(bodyElement);

    if(!( pEnvelope->HasBody())) return KErrArgument;

    CleanupStack::PopAndDestroy(1); // document
    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_Parse1L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KInputString,             "<S:Envelope \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header>\
<sb:Correlation xmlns:sb=\"urn:liberty:sb:2003-08\" \
messageID=\"URN:UUID:860949DC-134D-A989-E328-2FD7F20E31CE\" timestamp=\"2006-06-01T14:53:19Z\"/>\
<wsse:Security xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\"/>\
</S:Header>\
<S:Body>\
<sa:SASLRequest xmlns:sa=\"urn:liberty:sa:2004-04\" mechanism=\"ANONYMOUS PLAIN CRAM-MD5\" authzID=\"testuser1\"/>\
</S:Body>\
</S:Envelope>");
    _LIT8(KBodyAsString,     "\
<S:Body xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:sa=\"urn:liberty:sa:2004-04\">\
<sa:SASLRequest xmlns:sa=\"urn:liberty:sa:2004-04\" mechanism=\"ANONYMOUS PLAIN CRAM-MD5\" authzID=\"testuser1\"/>\
</S:Body>");

    _LIT8(KEmptyBodyAsString,       "<S:Body/>");

    _LIT8(KEmptyBodyWithNsAsString, "<S:Body xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");

    CSenParser* pParser = CSenParser::NewLC();
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

	pParser->ParseL(KInputString, *pEnvelope);

	// 1) Check that parsed SoapEnvelope can be
	//    serialized correctly
	HBufC8* pAsXml = pEnvelope->AsXmlL();
	CleanupStack::PushL(pAsXml);
	if(!( *pAsXml == KInputString )) return KErrArgument;
	CleanupStack::PopAndDestroy(pAsXml);

	// 2) Check that body can be detached from SoapEnvelope
	//    correctly.
	// Note: Following "BodyAsStringL()" call will detach
	//       Body from SoapEnvelope
	HBufC8* pBodyAsString = pEnvelope->BodyAsStringL();
	CleanupStack::PushL(pBodyAsString);
	if(!( *pBodyAsString == KBodyAsString )) return KErrArgument;
	CleanupStack::PopAndDestroy(pBodyAsString);

	// 3) Check that body can be detached from SoapEnvelope
	//    twice
	//    In this case body does not have child elements.
	//    => Empty body should be returned.
	//       Note: Empty body should contain namespace
	//             declaration because body is detached
	pBodyAsString = pEnvelope->BodyAsStringL();
	CleanupStack::PushL(pBodyAsString);
	if(!( *pBodyAsString == KEmptyBodyWithNsAsString )) return KErrArgument;
	CleanupStack::PopAndDestroy(pBodyAsString);

	// 4) Check that body was detached from SoapEnvelope
	//    correctly
	//    => Getting body again should result empty body to be returned.
	TXmlEngElement bodyElement = pEnvelope->BodyL();
	RSenDocument document = pEnvelope->AsDocumentL();

    TUint optionFlags = 0;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    optionFlags = optionFlags | TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;

    // Allow encoding declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionEncoding;

    // Allow standalone declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionStandalone;

    TXmlEngSerializationOptions options(optionFlags);

    RBuf8 asXml;
    CleanupClosePushL(asXml);
    document.SaveL(asXml, bodyElement, options);

    // Serielized body should be empty because "BodyAsStringL()"
    // previously detached body.
    if(!( asXml == KEmptyBodyAsString )) return KErrArgument;

    CleanupStack::PopAndDestroy(&asXml);

    CleanupStack::PopAndDestroy(pEnvelope);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_Parse2L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KInputString,         "<S:Envelope \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header>\
<sb:Correlation xmlns:sb=\"urn:liberty:sb:2003-08\" \
messageID=\"URN:UUID:860949DC-134D-A989-E328-2FD7F20E31CE\" timestamp=\"2006-06-01T14:53:19Z\"/>\
<wsse:Security xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\"/>\
</S:Header>\
<S:Body>\
<sa:SASLRequest xmlns:sa=\"urn:liberty:sa:2004-04\" mechanism=\"ANONYMOUS PLAIN CRAM-MD5\" authzID=\"testuser1\"/>\
</S:Body>\
</S:Envelope>");

    _LIT8(KWholeBodyAsString,   "<S:Body>\
<sa:SASLRequest xmlns:sa=\"urn:liberty:sa:2004-04\" mechanism=\"ANONYMOUS PLAIN CRAM-MD5\" authzID=\"testuser1\"/>\
</S:Body>");

    _LIT8(KWholeHeaderAsString, "<S:Header>\
<sb:Correlation xmlns:sb=\"urn:liberty:sb:2003-08\" \
messageID=\"URN:UUID:860949DC-134D-A989-E328-2FD7F20E31CE\" timestamp=\"2006-06-01T14:53:19Z\"/>\
<wsse:Security xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\"/>\
</S:Header>");

    CSenParser* pParser = CSenParser::NewLC();
    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

	pParser->ParseL(KInputString, *pEnvelope);

	// 1) Check that parsed SoapEnvelope can be
	//    serialized correctly
	HBufC8* pAsXml = pEnvelope->AsXmlL();
	CleanupStack::PushL(pAsXml);
	if(!( *pAsXml == KInputString )) return KErrArgument;
	CleanupStack::PopAndDestroy(pAsXml);

	// 2) Check that body can be got from SoapEnvelope
	//    correctly
	// Note: Body won't be detached. Only reference for
	//       body TXmlEngElement inside SoapEnvelope is returned.
	TXmlEngElement bodyElement = pEnvelope->BodyL();
	RSenDocument document = pEnvelope->AsDocumentL();

    TUint optionFlags = 0;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    optionFlags = optionFlags | TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;

    // Allow encoding declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionEncoding;

    // Allow standalone declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionStandalone;

    TXmlEngSerializationOptions options(optionFlags);

    RBuf8 asXml;
    CleanupClosePushL(asXml);
    document.SaveL(asXml, bodyElement, options);

    // Serielized body should be in exactly the same form
    // as in original SoapEnvelope
    if(!( asXml == KWholeBodyAsString )) return KErrArgument;

    CleanupStack::PopAndDestroy(&asXml);

    // 3) Check that header TXmlEngElement can be got from SoapEnvelope
	//    correctly
    TXmlEngElement headerElement = pEnvelope->HeaderL();

    RBuf8 headerAsXml;
    CleanupClosePushL(headerAsXml);
    document.SaveL(headerAsXml, headerElement, options);

    // Serielized header should be in exactly the same form
    // as in original SoapEnvelope
    if(!( headerAsXml == KWholeHeaderAsString )) return KErrArgument;

    CleanupStack::PopAndDestroy(&headerAsXml);

    CleanupStack::PopAndDestroy(pEnvelope);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapEnvelope2_Parse3L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KInputString,         "<S:Envelope \
xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\" \
xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">\
<S:Header>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/>\
<wsa:MessageID>urn:uuid:7837d733-b9f8-3eb1-5c3f-7a509797e6b2</wsa:MessageID>\
<wsa:To>http://schemas.live.com/mws/2005/08/spaces</wsa:To>\
<wsa:Action>http://schemas.live.com/mws/2005/08/spaces/Post</wsa:Action>\
<wsa:ReplyTo><wsa:Address>id:7bbd5c5b-7857-44c4-b609-53498d0cab3b</wsa:Address></wsa:ReplyTo>\
</S:Header>\
<S:Body>\
<PostRequest xmlns=\"http://schemas.live.com/mws/2005/08/spaces\">\
<Item><Filename>sample1.wav</Filename><ItemType>8</ItemType><Image><Data></Data></Image></Item>\
</PostRequest>\
</S:Body>\
</S:Envelope>");

    _LIT8(KOuterXml,            "<S:Body>\
<PostRequest xmlns=\"http://schemas.live.com/mws/2005/08/spaces\">\
<Item><Filename>sample1.wav</Filename><ItemType>8</ItemType><Image><Data/></Image></Item>\
</PostRequest>\
</S:Body>");

    CSenParser* pParser = CSenParser::NewL();
    CleanupStack::PushL(pParser);

    CSenSoapEnvelope2* pEnvelope = CSenSoapEnvelope2::NewL();
    CleanupStack::PushL(pEnvelope);

    pParser->ParseL(KInputString, *pEnvelope);
    TXmlEngElement element = pEnvelope->BodyL();

    RBuf8 outerXml;
    CleanupClosePushL(outerXml);
    element.OuterXmlL(outerXml);

    if(!( outerXml == KOuterXml )) return KErrArgument;

    CleanupStack::PopAndDestroy(&outerXml);
    CleanupStack::PopAndDestroy(pEnvelope);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;    
	}


TInt CnewsoapclassesTester::MT_CSenSoapFault2_NewLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KFaultElementName,    "Fault");
    _LIT8(KFaultElementContent, "FaultContent");
    _LIT8(KFaultElement,        "<Fault>FaultContent</Fault>");

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement faultElement = document.CreateDocumentElementL(KFaultElementName());
    faultElement.AddTextL(KFaultElementContent());

    CSenSoapFault2* pSoapFault = CSenSoapFault2::NewL(faultElement);
    CleanupStack::PopAndDestroy(1); // document
    CleanupStack::PushL(pSoapFault);

    HBufC8* pAsXml = pSoapFault->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KFaultElement )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapFault);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapFault2_NewL_1L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KFaultElementName,    "Fault");
    _LIT8(KFaultElementContent, "FaultContent");
    _LIT8(KFaultElement,        "<Fault>FaultContent</Fault>");

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement faultElement = document.CreateDocumentElementL(KFaultElementName());
    faultElement.AddTextL(KFaultElementContent());

    CSenSoapFault2* pSoapFault = CSenSoapFault2::NewL(faultElement, document);
    CleanupStack::PopAndDestroy(1); // document
    CleanupStack::PushL(pSoapFault);

    HBufC8* pAsXml = pSoapFault->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KFaultElement )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapFault);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapFault2_NewL_2L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KParentName,          "Parent");

    _LIT8(KAttributeNsUri,      "nsuri");
    _LIT8(KAttributeNsPrefix,   "pr");
    _LIT8(KAttributeLocalName,  "AttrName");
    _LIT8(KAttributeValue,      "Value");

    _LIT8(KFaultNsUri,          "http://schemas.xmlsoap.org/soap/envelope/");
    _LIT8(KFaultNsPrefix,       "S");
    _LIT8(KFaultLocalName,      "Fault");

    _LIT8(KFaultElement,        "<S:Fault \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\" \
xmlns:pr=\"nsuri\" pr:AttrName=\"Value\"/>");

    _LIT8(KParentDocument,      "<Parent><S:Fault \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\" \
xmlns:pr=\"nsuri\" pr:AttrName=\"Value\"/>\
</Parent>");

    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement parentElement = document.CreateDocumentElementL(KParentName());

    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);

    RString nsUriRString        = stringPool.OpenStringL(KAttributeNsUri);
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(KAttributeNsPrefix);
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(KAttributeLocalName);
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(KAttributeValue);
    CleanupClosePushL(valueRString);

    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString,
                   valueRString);
    // attribute took ownership of all RStrings
    // => All RStrings can be pop from CleanupStack
    CleanupStack::Pop(&valueRString);
    CleanupStack::Pop(&localNameRString);
    CleanupStack::Pop(&nsPrefixRString);
    CleanupStack::Pop(&nsUriRString);

    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    // append the namespace attribute (declaration)
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray);

    CSenSoapFault2* pSoapFault = CSenSoapFault2::NewL(KFaultNsUri,
                                                      KFaultLocalName,
                                                      KFaultNsPrefix,
                                                      attrArray,
                                                      parentElement,
                                                      document);
    CleanupStack::PushL(pSoapFault);
    HBufC8* pAsXml = pSoapFault->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KFaultElement )) return KErrArgument;

    RBuf8 buffer;
    CleanupClosePushL(buffer);
    parentElement.OuterXmlL(buffer);

    if(!( buffer == KParentDocument )) return KErrArgument;
    CleanupStack::PopAndDestroy(&buffer);

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pSoapFault);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    CleanupStack::PopAndDestroy(&document);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapFault2_FaultCodeL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSenSoapEnvelopeXmlns,               "http://schemas.xmlsoap.org/soap/envelope/");
    _LIT8(KSenSoap12EnvelopeXmlns,             "http://www.w3.org/2003/05/soap-envelope");

    _LIT8(KFaultElementLocalname,   "Fault");
    _LIT8(KFaultNsPrefix,           "S");

    _LIT8(KFaultCodeLocalname,      "faultcode");

    _LIT8(KFault12CodeLocalname,    "Code");    // Soap 1.2
    _LIT8(KFault12ValueLocalname,	"Value");	//	Soap1.2

    _LIT8(KFaultCodeValue,          "123");


    // 1) Fault code in Soap other than 1.2
    //    Note: In SOAP older than 1.2 fault elements are _NOT_ namespace qualified
    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement faultElement = document.CreateDocumentElementL(KFaultElementLocalname(),
                                                                  KSenSoapEnvelopeXmlns(),
                                                                  KFaultNsPrefix());
    TXmlEngElement faultCodeElement = faultElement.AddNewElementL(KFaultCodeLocalname());
    faultCodeElement.AddTextL(KFaultCodeValue());

    CSenSoapFault2* pSoapFault = CSenSoapFault2::NewL(faultElement, document);
    CleanupStack::PopAndDestroy(1); // document
    CleanupStack::PushL(pSoapFault);

    TPtrC8 faultCode = pSoapFault->FaultCode();
    if(!( faultCode == KFaultCodeValue )) return KErrArgument;

    CleanupStack::PopAndDestroy(pSoapFault);

    // 2) Fault code in Soap 1.2
    //    Note: SOAP 1.2 faults are structured differently to SOAP 1.1.
    //          In particular all fault elements are now namespace _qualified_,
    //          many have been renamed and fault codes are now hierarchical
    RSenDocument document2 = RSenDocument::NewLC();
    TXmlEngElement faultElement2 = document2.CreateDocumentElementL(KFaultElementLocalname(),
                                                                    KSenSoap12EnvelopeXmlns(),
                                                                    KFaultNsPrefix());
    TXmlEngElement faultCodeElement2 = faultElement2.AddNewElementSameNsL(KFault12CodeLocalname());
    TXmlEngElement faultValueElement2 = faultCodeElement2.AddNewElementSameNsL(KFault12ValueLocalname());
    faultValueElement2.AddTextL(KFaultCodeValue());

    CSenSoapFault2* pSoapFault2 = CSenSoapFault2::NewL(faultElement2, document2);
    CleanupStack::PopAndDestroy(1); // document2
    CleanupStack::PushL(pSoapFault2);

    TPtrC8 faultCode2 = pSoapFault2->FaultCode();
    if(!( faultCode2 == KFaultCodeValue )) return KErrArgument;

    CleanupStack::PopAndDestroy(pSoapFault2);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapFault2_FaultSubcodeL( CStifItemParser& aItem )
    {
	SetupL();
    // FaultSubcodeL is supported only in Soap 1.2
    // If other version of Soap is used return value will always be KNullDesC
    _LIT8(KSenSoapEnvelopeXmlns,               "http://schemas.xmlsoap.org/soap/envelope/");
    _LIT8(KSenSoap12EnvelopeXmlns,             "http://www.w3.org/2003/05/soap-envelope");

    _LIT8(KFaultElementLocalname,   "Fault");
    _LIT8(KFaultNsPrefix,           "S");

//    _LIT8(KFaultCodeLocalname,      "faultcode");

    _LIT8(KFault12CodeLocalname,    "Code");    // Soap 1.2
    _LIT8(KFault12SubcodeLocalname,	"Subcode"); // Soap 1.2
    _LIT8(KFault12ValueLocalname,	"Value");	// Soap1.2

    _LIT8(KFaultCodeValue,          "123");


    // 1) Fault subcode in Soap other than 1.2
    //    Note: In SOAP older than 1.2 there is _NO_ sobcode
    //          => KNullDesC8 should be returned.
    //    Note: In SOAP older than 1.2 fault elements are _NOT_ namespace qualified
    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement faultElement = document.CreateDocumentElementL(KFaultElementLocalname(),
                                                                  KSenSoapEnvelopeXmlns(),
                                                                  KFaultNsPrefix());
    TXmlEngElement faultCodeElement    = faultElement.AddNewElementL(KFault12CodeLocalname());
    TXmlEngElement faultSubCodeElement = faultCodeElement.AddNewElementSameNsL(KFault12SubcodeLocalname());
    TXmlEngElement faultValueElement   = faultSubCodeElement.AddNewElementSameNsL(KFault12ValueLocalname());
    faultValueElement.AddTextL(KFaultCodeValue());

    CSenSoapFault2* pSoapFault = CSenSoapFault2::NewL(faultElement, document);
    CleanupStack::PopAndDestroy(1); // document
    CleanupStack::PushL(pSoapFault);

    TPtrC8 faultCode = pSoapFault->FaultSubcode();
    if(!( faultCode == KNullDesC8 )) return KErrArgument;

    CleanupStack::PopAndDestroy(pSoapFault);

    // 2) Fault subcode in Soap 1.2
    //    Note: SOAP 1.2 faults are structured differently to SOAP 1.1.
    //          In particular all fault elements are now namespace _qualified_,
    //          many have been renamed and fault codes are now hierarchical
    RSenDocument document2 = RSenDocument::NewLC();
    TXmlEngElement faultElement2 = document2.CreateDocumentElementL(KFaultElementLocalname(),
                                                                    KSenSoap12EnvelopeXmlns(),
                                                                    KFaultNsPrefix());
    TXmlEngElement faultCodeElement2    = faultElement2.AddNewElementSameNsL(KFault12CodeLocalname());
    TXmlEngElement faultSubCodeElement2 = faultCodeElement2.AddNewElementSameNsL(KFault12SubcodeLocalname());
    TXmlEngElement faultValueElement2   = faultSubCodeElement2.AddNewElementSameNsL(KFault12ValueLocalname());
    faultValueElement2.AddTextL(KFaultCodeValue());

    CSenSoapFault2* pSoapFault2 = CSenSoapFault2::NewL(faultElement2, document2);
    CleanupStack::PopAndDestroy(1); // document2
    CleanupStack::PushL(pSoapFault2);

    TPtrC8 faultCode2 = pSoapFault2->FaultSubcode();
    if(!( faultCode2 == KFaultCodeValue )) return KErrArgument;

    CleanupStack::PopAndDestroy(pSoapFault2);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapFault2_FaultStringL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSenSoapEnvelopeXmlns,               "http://schemas.xmlsoap.org/soap/envelope/");
    _LIT8(KSenSoap12EnvelopeXmlns,             "http://www.w3.org/2003/05/soap-envelope");

    _LIT8(KFaultElementLocalname,   "Fault");
    _LIT8(KFaultNsPrefix,           "S");

    _LIT8(KFaultStringLocalname,    "faultstring");

    _LIT8(KFault12ReasonLocalname,	"Reason");  // Soap1.2
    _LIT8(KFault12TextLocalname,	"Text");    // Soap1.2


    _LIT8(KFaultStringValue,        "Fault code string");


    // 1) Fault code in Soap other than 1.2
    //    Note: In SOAP older than 1.2 fault elements are _NOT_ namespace qualified
    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement faultElement = document.CreateDocumentElementL(KFaultElementLocalname(),
                                                                  KSenSoapEnvelopeXmlns(),
                                                                  KFaultNsPrefix());
    TXmlEngElement faultStringElement = faultElement.AddNewElementL(KFaultStringLocalname());
    faultStringElement.AddTextL(KFaultStringValue());

    CSenSoapFault2* pSoapFault = CSenSoapFault2::NewL(faultElement, document);
    CleanupStack::PopAndDestroy(1); // document
    CleanupStack::PushL(pSoapFault);

    TPtrC8 faultCode = pSoapFault->FaultString();
    if(!( faultCode == KFaultStringValue )) return KErrArgument;

    CleanupStack::PopAndDestroy(pSoapFault);

    // 2) Fault code in Soap 1.2
    //    Note: SOAP 1.2 faults are structured differently to SOAP 1.1.
    //          In particular all fault elements are now namespace _qualified_,
    //          many have been renamed and fault codes are now hierarchical
    RSenDocument document2 = RSenDocument::NewLC();
    TXmlEngElement faultElement2 = document2.CreateDocumentElementL(KFaultElementLocalname(),
                                                                    KSenSoap12EnvelopeXmlns(),
                                                                    KFaultNsPrefix());
    TXmlEngElement faultReasonElement2 = faultElement2.AddNewElementSameNsL(KFault12ReasonLocalname());
    TXmlEngElement faultTextElement2 = faultReasonElement2.AddNewElementSameNsL(KFault12TextLocalname());
    faultTextElement2.AddTextL(KFaultStringValue());

    CSenSoapFault2* pSoapFault2 = CSenSoapFault2::NewL(faultElement2, document2);
    CleanupStack::PopAndDestroy(1); // document2
    CleanupStack::PushL(pSoapFault2);

    TPtrC8 faultCode2 = pSoapFault2->FaultString();
    if(!( faultCode2 == KFaultStringValue )) return KErrArgument;

    CleanupStack::PopAndDestroy(pSoapFault2);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapFault2_FaultActorL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSenSoapEnvelopeXmlns,               "http://schemas.xmlsoap.org/soap/envelope/");
    _LIT8(KSenSoap12EnvelopeXmlns,             "http://www.w3.org/2003/05/soap-envelope");

    _LIT8(KFaultElementLocalname,   "Fault");
    _LIT8(KFaultNsPrefix,           "S");

    _LIT8(KFaultActorLocalname,     "faultactor");

    _LIT8(KFault12NodeLocalname,	"Node");    // Node represents faultactor in Soap 1.2

    _LIT8(KFaultActorValue,          "http://www.wrox.com/heroes/endpoint.asp");


    // 1) Faultactor in Soap other than 1.2
    //    "faultactor" is intended to provide information about which SOAP node
    //    on the SOAP message path caused the fault to happen.
    //    The value of the "faultactor" is a URI identifying the source of the fault.

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement faultElement = document.CreateDocumentElementL(KFaultElementLocalname(),
                                                                  KSenSoapEnvelopeXmlns(),
                                                                  KFaultNsPrefix());
    TXmlEngElement faultCodeElement = faultElement.AddNewElementL(KFaultActorLocalname());
    faultCodeElement.AddTextL(KFaultActorValue());

    CSenSoapFault2* pSoapFault = CSenSoapFault2::NewL(faultElement, document);
    CleanupStack::PopAndDestroy(1); // document
    CleanupStack::PushL(pSoapFault);

    TPtrC8 faultActor = pSoapFault->FaultActor();
    if(!( faultActor == KFaultActorValue )) return KErrArgument;

    CleanupStack::PopAndDestroy(pSoapFault);

    // 2) Faultactor in Soap 1.2
    //    "Node" is intended to provide information about which SOAP node on the
    //    SOAP message path caused the fault to happen.
    //    "Node" contains the URI of the SOAP node that generated the fault.
    //
    //    Note: In Soap 1.2 "Node" represents "faultactor" from previous SOAP versions.

    RSenDocument document2 = RSenDocument::NewLC();
    TXmlEngElement faultElement2 = document2.CreateDocumentElementL(KFaultElementLocalname(),
                                                                    KSenSoap12EnvelopeXmlns(),
                                                                    KFaultNsPrefix());
    TXmlEngElement faultNodeElement2 = faultElement2.AddNewElementSameNsL(KFault12NodeLocalname());
    faultNodeElement2.AddTextL(KFaultActorValue());

    CSenSoapFault2* pSoapFault2 = CSenSoapFault2::NewL(faultElement2, document2);
    CleanupStack::PopAndDestroy(1); // document2
    CleanupStack::PushL(pSoapFault2);

    HBufC8* pAsXml = pSoapFault2->AsXmlL();
    delete pAsXml;

    TPtrC8 faultCode2 = pSoapFault2->FaultActor();
    if(!( faultCode2 == KFaultActorValue )) return KErrArgument;

    CleanupStack::PopAndDestroy(pSoapFault2);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapFault2_DetailL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSenSoapEnvelopeXmlns,               "http://schemas.xmlsoap.org/soap/envelope/");
    _LIT8(KSenSoap12EnvelopeXmlns,             "http://www.w3.org/2003/05/soap-envelope");

    _LIT8(KFaultElementLocalname,   "Fault");
    _LIT8(KFaultNsPrefix,           "S");

    _LIT8(KDetailLocalname,         "detail");

    _LIT8(KFault12DetailLocalname,	"Detail");    // Soap 1.2

    _LIT8(KFaultDetailValue,          "Detailed information");


    // 1) Fault code in Soap other than 1.2
    //    Note: In SOAP older than 1.2 fault elements are _NOT_ namespace qualified
    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement faultElement = document.CreateDocumentElementL(KFaultElementLocalname(),
                                                                  KSenSoapEnvelopeXmlns(),
                                                                  KFaultNsPrefix());
    TXmlEngElement faultDetailElement = faultElement.AddNewElementL(KDetailLocalname());
    faultDetailElement.AddTextL(KFaultDetailValue());

    CSenSoapFault2* pSoapFault = CSenSoapFault2::NewL(faultElement, document);
    CleanupStack::PopAndDestroy(1); // document
    CleanupStack::PushL(pSoapFault);

    TPtrC8 faultDetail = pSoapFault->Detail();
    if(!( faultDetail == KFaultDetailValue )) return KErrArgument;

    CleanupStack::PopAndDestroy(pSoapFault);

    // 2) Fault code in Soap 1.2
    //    Note: SOAP 1.2 faults are structured differently to SOAP 1.1.
    //          In particular all fault elements are now namespace _qualified_,
    //          many have been renamed and fault codes are now hierarchical
    RSenDocument document2 = RSenDocument::NewLC();
    TXmlEngElement faultElement2 = document2.CreateDocumentElementL(KFaultElementLocalname(),
                                                                    KSenSoap12EnvelopeXmlns(),
                                                                    KFaultNsPrefix());
    TXmlEngElement faultDetailElement2 = faultElement2.AddNewElementSameNsL(KFault12DetailLocalname());
    faultDetailElement2.AddTextL(KFaultDetailValue());

    CSenSoapFault2* pSoapFault2 = CSenSoapFault2::NewL(faultElement2, document2);
    CleanupStack::PopAndDestroy(1); // document2
    CleanupStack::PushL(pSoapFault2);

    TPtrC8 faultDetail2 = pSoapFault2->Detail();
    if(!( faultDetail2 == KFaultDetailValue )) return KErrArgument;

    CleanupStack::PopAndDestroy(pSoapFault2);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSoapMessage11,   "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");

    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewL();
    CleanupStack::PushL(pSoapMessage);

    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);
    Teardown();
    return KErrNone;    
	}
    
TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewLCL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSoapMessage11,   "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");

    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewLC();

    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pSoapMessage);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewL_1L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSoapMessage11,   "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    _LIT8(KSoapMessage12,   "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\"/>");

    // 1) Test creating of SOAP 1.1 version SoapMessage
    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewL(ESOAP11);
    CleanupStack::PushL(pSoapMessage);

    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2) Test creating of SOAP 1.2 version SoapMessage
    pSoapMessage = CSenSoapMessage2::NewL(ESOAP12);
    CleanupStack::PushL(pSoapMessage);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage12 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);
    Teardown();
    return KErrNone;    
	}
    
TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewLC_1L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSoapMessage11,   "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    _LIT8(KSoapMessage12,   "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\"/>");

    // 1) Test creating of SOAP 1.1 version SoapMessage
    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewLC(ESOAP11);

    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2) Test creating of SOAP 1.2 version SoapMessage
    pSoapMessage = CSenSoapMessage2::NewLC(ESOAP12);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage12 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewL_2L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KIllegalNamespace,            "illegalNamespace");
    _LIT8(KSecuritySchemeXmlNamespace,  "http://schemas.xmlsoap.org/ws/2003/06/secext");
    _LIT8(KSecurityXmlNamespace,        "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");

    _LIT8(KSoapMessage11,    "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    _LIT8(KSoapMessage12,    "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header><wsse:Security xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\"/></S:Header>\
<S:Body/>\
</S:Envelope>");
    _LIT8(KSoapMessage13,    "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header><wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/></S:Header>\
<S:Body/>\
</S:Envelope>");
    _LIT8(KSoapMessage21,    "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\"/>");
    _LIT8(KSoapMessage22,    "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<S:Header><wsse:Security xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\"/></S:Header>\
<S:Body/>\
</S:Envelope>");
    _LIT8(KSoapMessage23,    "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<S:Header><wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/></S:Header>\
<S:Body/>\
</S:Envelope>");

    // 1.1) Test creating of SOAP 1.1 version SoapMessage with illegal namespace
    //      => No security header should be added.
    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewL(ESOAP11, KIllegalNamespace);
    CleanupStack::PushL(pSoapMessage);

    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 1.2) Test creating of SOAP 1.1 version SoapMessage with scheme namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewL(ESOAP11, KSecuritySchemeXmlNamespace);
    CleanupStack::PushL(pSoapMessage);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage12 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 1.3) Test creating of SOAP 1.1 version SoapMessage with security namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewL(ESOAP11, KSecurityXmlNamespace);
    CleanupStack::PushL(pSoapMessage);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage13 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2.1) Test creating of SOAP 1.2 version SoapMessage with illegal namespace
    //      => No security header should be added.
    pSoapMessage = CSenSoapMessage2::NewL(ESOAP12, KIllegalNamespace);
    CleanupStack::PushL(pSoapMessage);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage21 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2.2) Test creating of SOAP 1.2 version SoapMessage with scheme namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewL(ESOAP12, KSecuritySchemeXmlNamespace);
    CleanupStack::PushL(pSoapMessage);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage22 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2.3) Test creating of SOAP 1.2 version SoapMessage with security namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewL(ESOAP12, KSecurityXmlNamespace);
    CleanupStack::PushL(pSoapMessage);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage23 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);
    Teardown();
    return KErrNone;    
	}
    
TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewLC_2L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KIllegalNamespace,            "illegalNamespace");
    _LIT8(KSecuritySchemeXmlNamespace,  "http://schemas.xmlsoap.org/ws/2003/06/secext");
    _LIT8(KSecurityXmlNamespace,        "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");

    _LIT8(KSoapMessage11,    "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    _LIT8(KSoapMessage12,    "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header><wsse:Security xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\"/></S:Header>\
<S:Body/>\
</S:Envelope>");
    _LIT8(KSoapMessage13,    "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header><wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/></S:Header>\
<S:Body/>\
</S:Envelope>");
    _LIT8(KSoapMessage21,    "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\"/>");
    _LIT8(KSoapMessage22,    "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<S:Header><wsse:Security xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\"/></S:Header>\
<S:Body/>\
</S:Envelope>");
    _LIT8(KSoapMessage23,    "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<S:Header><wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/></S:Header>\
<S:Body/>\
</S:Envelope>");

    // 1.1) Test creating of SOAP 1.1 version SoapMessage with illegal namespace
    //      => No security header should be added.
    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewLC(ESOAP11, KIllegalNamespace);

    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 1.2) Test creating of SOAP 1.1 version SoapMessage with scheme namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewLC(ESOAP11, KSecuritySchemeXmlNamespace);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage12 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 1.3) Test creating of SOAP 1.1 version SoapMessage with security namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewLC(ESOAP11, KSecurityXmlNamespace);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage13 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2.1) Test creating of SOAP 1.2 version SoapMessage with illegal namespace
    //      => No security header should be added.
    pSoapMessage = CSenSoapMessage2::NewLC(ESOAP12, KIllegalNamespace);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage21 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2.2) Test creating of SOAP 1.2 version SoapMessage with scheme namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewLC(ESOAP12, KSecuritySchemeXmlNamespace);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage22 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2.3) Test creating of SOAP 1.2 version SoapMessage with security namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewLC(ESOAP12, KSecurityXmlNamespace);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage23 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewL_3L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSoapMessage11,   "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    CSenSoapMessage2* pSoapMessage, *pSoapMessage1 = CSenSoapMessage2::NewL();
    pSoapMessage = CSenSoapMessage2::NewL(*pSoapMessage1);
    
    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pAsXml);
    
    
    __ASSERT_ALWAYS_NO_LEAVE(delete pSoapMessage);
    pSoapMessage = NULL;
    delete pSoapMessage1;
    pSoapMessage1 = NULL;
    Teardown();
    return KErrNone;    
	}
    
TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewLC_3L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSoapMessage11,   "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    CSenSoapMessage2* pSoapMessage, *pSoapMessage1 = CSenSoapMessage2::NewL();
    pSoapMessage = CSenSoapMessage2::NewLC(*pSoapMessage1);
    
    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pSoapMessage);
    delete pSoapMessage1;
    pSoapMessage1 = NULL;
    Teardown();
    return KErrNone;    
	}
/*    
TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewL_4L( CStifItemParser& aItem )
	{
	SetupL();
    _LIT8(KSoapMessage11,   "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
	MSenMessageContext* mContext = NULL;
    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewL(*mContext);
    CleanupStack::PushL(pSoapMessage);
    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pSoapMessage);
    Teardown();
    return KErrNone;    
	}
	
TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewLC_4L( CStifItemParser& aItem )
	{
	SetupL();
	_LIT8(KSoapMessage11,   "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
	MSenMessageContext* mContext = NULL;
    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewLC(*mContext);
    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    
    CleanupStack::PopAndDestroy(pSoapMessage);

    Teardown();
    return KErrNone;    
	}
	
TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewL_5L( CStifItemParser& aItem )
	{
	SetupL();
	_LIT8(KSoapMessage11,   "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    _LIT8(KSoapMessage12,   "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\"/>");
	
    MSenMessageContext* mContext = NULL;
    // 1) Test creating of SOAP 1.1 version SoapMessage
    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewL(*mContext, ESOAP11);
    CleanupStack::PushL(pSoapMessage);

    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2) Test creating of SOAP 1.2 version SoapMessage
    pSoapMessage = CSenSoapMessage2::NewL(*mContext, ESOAP12 );
    CleanupStack::PushL(pSoapMessage);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage12 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);
    Teardown();
    return KErrNone;    
	}
	
TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewLC_5L( CStifItemParser& aItem )
	{
	SetupL();
	_LIT8(KSoapMessage11,   "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    _LIT8(KSoapMessage12,   "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\"/>");
    MSenMessageContext* mContext = NULL;

  
    // 1) Test creating of SOAP 1.1 version SoapMessage
    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewLC(*mContext, ESOAP11);
    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);


    // 2) Test creating of SOAP 1.2 version SoapMessage
    pSoapMessage = CSenSoapMessage2::NewLC(*mContext, ESOAP12);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage12 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);
    Teardown();
    return KErrNone;    
	}
	
TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewL_6L( CStifItemParser& aItem )
	{
	SetupL();
	MSenMessageContext* mContext = NULL;

    
	_LIT8(KIllegalNamespace,            "illegalNamespace");
    _LIT8(KSecuritySchemeXmlNamespace,  "http://schemas.xmlsoap.org/ws/2003/06/secext");
    _LIT8(KSecurityXmlNamespace,        "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");

    _LIT8(KSoapMessage11,    "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    _LIT8(KSoapMessage12,    "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header><wsse:Security xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\"/></S:Header>\
<S:Body/>\
</S:Envelope>");
    _LIT8(KSoapMessage13,    "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header><wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/></S:Header>\
<S:Body/>\
</S:Envelope>");
    _LIT8(KSoapMessage21,    "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\"/>");
    _LIT8(KSoapMessage22,    "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<S:Header><wsse:Security xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\"/></S:Header>\
<S:Body/>\
</S:Envelope>");
    _LIT8(KSoapMessage23,    "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<S:Header><wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/></S:Header>\
<S:Body/>\
</S:Envelope>");

    // 1.1) Test creating of SOAP 1.1 version SoapMessage with illegal namespace
    //      => No security header should be added.
    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewL(*mContext, ESOAP11, KIllegalNamespace);
    CleanupStack::PushL(pSoapMessage);

    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 1.2) Test creating of SOAP 1.1 version SoapMessage with scheme namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewL(*mContext,ESOAP11, KSecuritySchemeXmlNamespace);
    CleanupStack::PushL(pSoapMessage);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage12 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 1.3) Test creating of SOAP 1.1 version SoapMessage with security namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewL(*mContext,ESOAP11, KSecurityXmlNamespace);
    CleanupStack::PushL(pSoapMessage);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage13 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2.1) Test creating of SOAP 1.2 version SoapMessage with illegal namespace
    //      => No security header should be added.
    pSoapMessage = CSenSoapMessage2::NewL(*mContext,ESOAP12, KIllegalNamespace);
    CleanupStack::PushL(pSoapMessage);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage21 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2.2) Test creating of SOAP 1.2 version SoapMessage with scheme namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewL(*mContext,ESOAP12, KSecuritySchemeXmlNamespace);
    CleanupStack::PushL(pSoapMessage);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage22 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2.3) Test creating of SOAP 1.2 version SoapMessage with security namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewL(*mContext,ESOAP12, KSecurityXmlNamespace);
    CleanupStack::PushL(pSoapMessage);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage23 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);
    
    Teardown();
    return KErrNone;    
	}
	
TInt CnewsoapclassesTester::MT_CSenSoapMessage2_NewLC_6L(CStifItemParser& aItem )
	{
	SetupL();
	MSenMessageContext* mContext = NULL;
    
	 _LIT8(KIllegalNamespace,            "illegalNamespace");
    _LIT8(KSecuritySchemeXmlNamespace,  "http://schemas.xmlsoap.org/ws/2003/06/secext");
    _LIT8(KSecurityXmlNamespace,        "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");

    _LIT8(KSoapMessage11,    "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    _LIT8(KSoapMessage12,    "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header><wsse:Security xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\"/></S:Header>\
<S:Body/>\
</S:Envelope>");
    _LIT8(KSoapMessage13,    "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header><wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/></S:Header>\
<S:Body/>\
</S:Envelope>");
    _LIT8(KSoapMessage21,    "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\"/>");
    _LIT8(KSoapMessage22,    "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<S:Header><wsse:Security xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\"/></S:Header>\
<S:Body/>\
</S:Envelope>");
    _LIT8(KSoapMessage23,    "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<S:Header><wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/></S:Header>\
<S:Body/>\
</S:Envelope>");

    // 1.1) Test creating of SOAP 1.1 version SoapMessage with illegal namespace
    //      => No security header should be added.
    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewLC(*mContext,ESOAP11, KIllegalNamespace);

    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 1.2) Test creating of SOAP 1.1 version SoapMessage with scheme namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewLC(*mContext,ESOAP11, KSecuritySchemeXmlNamespace);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage12 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 1.3) Test creating of SOAP 1.1 version SoapMessage with security namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewLC(*mContext,ESOAP11, KSecurityXmlNamespace);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage13 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2.1) Test creating of SOAP 1.2 version SoapMessage with illegal namespace
    //      => No security header should be added.
    pSoapMessage = CSenSoapMessage2::NewLC(*mContext,ESOAP12, KIllegalNamespace);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage21 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2.2) Test creating of SOAP 1.2 version SoapMessage with scheme namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewLC(*mContext,ESOAP12, KSecuritySchemeXmlNamespace);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage22 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2.3) Test creating of SOAP 1.2 version SoapMessage with security namespace
    //      => Security header should be created with given namespace.
    pSoapMessage = CSenSoapMessage2::NewLC(*mContext,ESOAP12, KSecurityXmlNamespace);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage23 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    Teardown();
    return KErrNone;    
	}
*/	
 
TInt CnewsoapclassesTester::MT_CSenSoapMessage2_TypeL( CStifItemParser& aItem )
	{
	SetupL();
	CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewL();
    CleanupStack::PushL(pSoapMessage);
    
    TL(MSenMessage::ESoapMessage2 == pSoapMessage->Type());
    CleanupStack::PopAndDestroy(pSoapMessage);
    Teardown();
    return KErrNone;    
    
	}

TInt CnewsoapclassesTester::MT_CSenSoapMessage2_CloneL( CStifItemParser& aItem )
	{
	SetupL();
	TBool Flag;
	CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewL();
    CleanupStack::PushL(pSoapMessage);

	CSenSoapMessage2* pClone = NULL;
    pClone = 	(CSenSoapMessage2*)pSoapMessage->CloneL(); 
    if(pClone != NULL)
    	Flag = 1;
    if(!(Flag)) return KErrArgument;
    delete pClone;
    CleanupStack::PopAndDestroy(pSoapMessage);
    Teardown();
    return KErrNone;    
	}
	
TInt CnewsoapclassesTester::MT_CSenSoapMessage2_Parse1L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KInputString,             "<S:Envelope \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header>\
<sb:Correlation xmlns:sb=\"urn:liberty:sb:2003-08\" \
messageID=\"URN:UUID:860949DC-134D-A989-E328-2FD7F20E31CE\" timestamp=\"2006-06-01T14:53:19Z\"/>\
<wsse:Security xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\"/>\
</S:Header>\
<S:Body>\
<sa:SASLRequest xmlns:sa=\"urn:liberty:sa:2004-04\" mechanism=\"ANONYMOUS PLAIN CRAM-MD5\" authzID=\"testuser1\"/>\
</S:Body>\
</S:Envelope>");
    _LIT8(KBodyAsString,     "\
<S:Body xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:sa=\"urn:liberty:sa:2004-04\">\
<sa:SASLRequest xmlns:sa=\"urn:liberty:sa:2004-04\" mechanism=\"ANONYMOUS PLAIN CRAM-MD5\" authzID=\"testuser1\"/>\
</S:Body>");
    

    _LIT8(KEmptyBodyAsString,       "<S:Body/>");

    _LIT8(KEmptyBodyWithNsAsString, "<S:Body xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");

    CSenParser* pParser = CSenParser::NewLC();
    CSenSoapMessage2* pMessage = CSenSoapMessage2::NewL();
    CleanupStack::PushL(pMessage);

	pParser->ParseL(KInputString, *pMessage);

	// 1) Check that parsed SoapEnvelope can be
	//    serialized correctly
	HBufC8* pAsXml = pMessage->AsXmlL();
	CleanupStack::PushL(pAsXml);
	if(!( *pAsXml == KInputString )) return KErrArgument;
	CleanupStack::PopAndDestroy(pAsXml);

	// 2) Check that body can be detached from SoapEnvelope
	//    correctly.
	// Note: Following "BodyAsStringL()" call will detach
	//       Body from SoapEnvelope
	HBufC8* pBodyAsString = pMessage->BodyAsStringL();
	CleanupStack::PushL(pBodyAsString);
	if(!( *pBodyAsString == KBodyAsString )) return KErrArgument;
	CleanupStack::PopAndDestroy(pBodyAsString);

	// 3) Check that body can be detached from SoapEnvelope
	//    twice
	//    In this case body does not have child elements.
	//    => Empty body should be returned.
	//       Note: Empty body should contain namespace
	//             declaration because body is detached
	pBodyAsString = pMessage->BodyAsStringL();
	CleanupStack::PushL(pBodyAsString);
	if(!( *pBodyAsString == KEmptyBodyWithNsAsString )) return KErrArgument;
	CleanupStack::PopAndDestroy(pBodyAsString);

	// 4) Check that body was detached from SoapEnvelope
	//    correctly
	//    => Getting body again should result empty body to be returned.
	TXmlEngElement bodyElement = pMessage->BodyL();
	RSenDocument document = pMessage->AsDocumentL();

    TUint optionFlags = 0;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    optionFlags = optionFlags | TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;

    // Allow encoding declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionEncoding;

    // Allow standalone declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionStandalone;

    TXmlEngSerializationOptions options(optionFlags);

    RBuf8 asXml;
    CleanupClosePushL(asXml);
    document.SaveL(asXml, bodyElement, options);

    // Serielized body should be empty because "BodyAsStringL()"
    // previously detached body.
    if(!( asXml == KEmptyBodyAsString )) return KErrArgument;

    CleanupStack::PopAndDestroy(&asXml);

    CleanupStack::PopAndDestroy(pMessage);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;    
	}
	

TInt CnewsoapclassesTester::MT_CSenSoapMessage2_SetSecurityHeaderLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSoapMessage11_1, "<S:Envelope \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
<Credential/>\
</wsse:Security>\
</S:Header>\
<S:Body/>\
</S:Envelope>");

    _LIT8(KSoapMessage11_2, "<S:Envelope \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/>\
</S:Header>\
<S:Body/>\
</S:Envelope>");

    _LIT8(KSoapMessage12_1, "<S:Envelope \
xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<S:Header>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
<Credential/>\
</wsse:Security>\
</S:Header>\
<S:Body/>\
</S:Envelope>");

    _LIT8(KSoapMessage12_2, "<S:Envelope \
xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<S:Header>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/>\
</S:Header>\
<S:Body/>\
</S:Envelope>");

    _LIT8(KCredential,      "<Credential/>");

    // 1) Soap 1.1 - Test setting of SecurityHeader WITH Credential
    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewL(ESOAP11);
    CleanupStack::PushL(pSoapMessage);

    pSoapMessage->SetSecurityHeaderL(KCredential);

    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11_1 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2) Soap 1.1 - Test setting of SecurityHeader WITHOUT Credential
    pSoapMessage = CSenSoapMessage2::NewL(ESOAP11);
    CleanupStack::PushL(pSoapMessage);

    pSoapMessage->SetSecurityHeaderL(KNullDesC8);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11_2 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 3) Soap 1.2 - Test setting of SecurityHeader WITH Credential
    pSoapMessage = CSenSoapMessage2::NewL(ESOAP12);
    CleanupStack::PushL(pSoapMessage);

    pSoapMessage->SetSecurityHeaderL(KCredential);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage12_1 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 4) Soap 1.2 - Test setting of SecurityHeader WITH Credential
    pSoapMessage = CSenSoapMessage2::NewL(ESOAP12);
    CleanupStack::PushL(pSoapMessage);

    pSoapMessage->SetSecurityHeaderL(KNullDesC8);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage12_2 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenSoapMessage2_AddSecurityTokenLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KSoapMessage11_1, "<S:Envelope \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
<SecurityToken/>\
</wsse:Security>\
</S:Header>\
<S:Body/>\
</S:Envelope>");

    _LIT8(KSoapMessage11_2, "<S:Envelope \
xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">\
<S:Header>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
<SecurityToken/>\
<SecurityToken2/>\
</wsse:Security>\
</S:Header>\
<S:Body/>\
</S:Envelope>");

    _LIT8(KSoapMessage12_1, "<S:Envelope \
xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<S:Header>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
<SecurityToken/>\
</wsse:Security>\
</S:Header>\
<S:Body/>\
</S:Envelope>");

    _LIT8(KSoapMessage12_2, "<S:Envelope \
xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<S:Header>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
<SecurityToken/>\
<SecurityToken2/>\
</wsse:Security>\
</S:Header>\
<S:Body/>\
</S:Envelope>");

    _LIT8(KSecurityToken,   "<SecurityToken/>");
    _LIT8(KSecurityToken2,   "<SecurityToken2/>");

    // 1) Soap 1.1 - Test adding of SecurityToken
    CSenSoapMessage2* pSoapMessage = CSenSoapMessage2::NewL(ESOAP11);
    CleanupStack::PushL(pSoapMessage);

    pSoapMessage->AddSecurityTokenL(KSecurityToken);

    HBufC8* pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11_1 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 2) Soap 1.1 - Test adding of two SecurityTokens
    pSoapMessage = CSenSoapMessage2::NewL(ESOAP11);
    CleanupStack::PushL(pSoapMessage);

    pSoapMessage->AddSecurityTokenL(KSecurityToken);
    pSoapMessage->AddSecurityTokenL(KSecurityToken2);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage11_2 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 3) Soap 1.2 - Test adding of SecurityToken
    pSoapMessage = CSenSoapMessage2::NewL(ESOAP12);
    CleanupStack::PushL(pSoapMessage);

    pSoapMessage->AddSecurityTokenL(KSecurityToken);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage12_1 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);

    // 4) Soap 1.2 - Test adding of two SecurityTokens
    pSoapMessage = CSenSoapMessage2::NewL(ESOAP12);
    CleanupStack::PushL(pSoapMessage);

    pSoapMessage->AddSecurityTokenL(KSecurityToken);
    pSoapMessage->AddSecurityTokenL(KSecurityToken2);

    pAsXml = pSoapMessage->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSoapMessage12_2 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pSoapMessage);
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_NewLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElement,     "<RootElement>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/>\
</RootElement>");

    _LIT8(KSecurityHeader,  "\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/>");

    _LIT8(KRootElementName, "RootElement");


    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewL(document,
                                                                      rootElement);
    CleanupStack::PushL(pWsSecHeader);

    TUint optionFlags = 0;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    optionFlags = optionFlags | TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;

    // Allow encoding declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionEncoding;

    // Allow standalone declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionStandalone;

    TXmlEngSerializationOptions options(optionFlags);

    RBuf8 asXml;
    CleanupClosePushL(asXml);
    document.SaveL(asXml, rootElement, options);

    // Serialized document should contain all the Fragment data as XML.
    if(!( asXml == KRootElement )) return KErrArgument;

    CleanupStack::PopAndDestroy(&asXml);

    HBufC8* pAsXml = pWsSecHeader->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSecurityHeader )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pWsSecHeader);
    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_NewLCL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElement,     "<RootElement>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/>\
</RootElement>");

    _LIT8(KSecurityHeader,  "\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/>");

    _LIT8(KRootElementName, "RootElement");


    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewLC(document,
                                                                       rootElement);
    TUint optionFlags = 0;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    optionFlags = optionFlags | TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;

    // Allow encoding declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionEncoding;

    // Allow standalone declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionStandalone;

    TXmlEngSerializationOptions options(optionFlags);

    RBuf8 asXml;
    CleanupClosePushL(asXml);
    document.SaveL(asXml, rootElement, options);

    // Serialized document should contain all the Fragment data as XML.
    if(!( asXml == KRootElement )) return KErrArgument;

    CleanupStack::PopAndDestroy(&asXml);

    HBufC8* pAsXml = pWsSecHeader->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSecurityHeader )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pWsSecHeader);
    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_NewL_1L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElement,     "<RootElement>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
SecurityToken\
</wsse:Security>\
</RootElement>");

    _LIT8(KSecurityHeader,  "\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
SecurityToken\
</wsse:Security>");

    _LIT8(KRootElementName, "RootElement");
    _LIT8(KSecurityToken,   "SecurityToken");


    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewL(KSecurityToken,
                                                                      document,
                                                                      rootElement);
    CleanupStack::PushL(pWsSecHeader);

    TUint optionFlags = 0;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    optionFlags = optionFlags | TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;

    // Allow encoding declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionEncoding;

    // Allow standalone declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionStandalone;

    TXmlEngSerializationOptions options(optionFlags);

    RBuf8 asXml;
    CleanupClosePushL(asXml);
    document.SaveL(asXml, rootElement, options);

    // Serialized document should contain all the Fragment data as XML.
    if(!( asXml == KRootElement )) return KErrArgument;

    CleanupStack::PopAndDestroy(&asXml);

    HBufC8* pAsXml = pWsSecHeader->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSecurityHeader )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pWsSecHeader);
    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_NewLC_1L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElement,     "<RootElement>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
SecurityToken\
</wsse:Security>\
</RootElement>");

    _LIT8(KSecurityHeader,  "\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
SecurityToken\
</wsse:Security>");

    _LIT8(KRootElementName, "RootElement");
    _LIT8(KSecurityToken,   "SecurityToken");


    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewLC(KSecurityToken,
                                                                       document,
                                                                       rootElement);
    TUint optionFlags = 0;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    optionFlags = optionFlags | TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;

    // Allow encoding declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionEncoding;

    // Allow standalone declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionStandalone;

    TXmlEngSerializationOptions options(optionFlags);

    RBuf8 asXml;
    CleanupClosePushL(asXml);
    document.SaveL(asXml, rootElement, options);

    // Serialized document should contain all the Fragment data as XML.
    if(!( asXml == KRootElement )) return KErrArgument;

    CleanupStack::PopAndDestroy(&asXml);

    HBufC8* pAsXml = pWsSecHeader->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSecurityHeader )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pWsSecHeader);
    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_NewL_2L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElement,     "<RootElement>\
<wsse:Security xmlns:wsse=\"namespace\">\
SecurityToken\
</wsse:Security>\
</RootElement>");

    _LIT8(KSecurityHeader,  "\
<wsse:Security xmlns:wsse=\"namespace\">\
SecurityToken\
</wsse:Security>");

    _LIT8(KRootElementName, "RootElement");
    _LIT8(KSecurityToken,   "SecurityToken");
    _LIT8(KNamespace,       "namespace");


    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewL(KSecurityToken,
                                                                      KNamespace,
                                                                      document,
                                                                      rootElement);
    CleanupStack::PushL(pWsSecHeader);

    TUint optionFlags = 0;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    optionFlags = optionFlags | TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;

    // Allow encoding declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionEncoding;

    // Allow standalone declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionStandalone;

    TXmlEngSerializationOptions options(optionFlags);

    RBuf8 asXml;
    CleanupClosePushL(asXml);
    document.SaveL(asXml, rootElement, options);

    // Serialized document should contain all the Fragment data as XML.
    if(!( asXml == KRootElement )) return KErrArgument;

    CleanupStack::PopAndDestroy(&asXml);

    HBufC8* pAsXml = pWsSecHeader->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSecurityHeader )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pWsSecHeader);
    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_NewLC_2L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElement,     "<RootElement>\
<wsse:Security xmlns:wsse=\"namespace\">\
SecurityToken\
</wsse:Security>\
</RootElement>");

    _LIT8(KSecurityHeader,  "\
<wsse:Security xmlns:wsse=\"namespace\">\
SecurityToken\
</wsse:Security>");

    _LIT8(KRootElementName, "RootElement");
    _LIT8(KSecurityToken,   "SecurityToken");
    _LIT8(KNamespace,       "namespace");

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewLC(KSecurityToken,
                                                                       KNamespace,
                                                                       document,
                                                                       rootElement);
    TUint optionFlags = 0;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    optionFlags = optionFlags | TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;

    // Allow encoding declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionEncoding;

    // Allow standalone declaration (if KOptionOmitXMLDeclaration is _not_ set)
    //optionFlags = optionFlags | TSerializationOptions::KOptionStandalone;

    TXmlEngSerializationOptions options(optionFlags);

    RBuf8 asXml;
    CleanupClosePushL(asXml);
    document.SaveL(asXml, rootElement, options);

    // Serialized document should contain all the Fragment data as XML.
    if(!( asXml == KRootElement )) return KErrArgument;

    CleanupStack::PopAndDestroy(&asXml);

    HBufC8* pAsXml = pWsSecHeader->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KSecurityHeader )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pWsSecHeader);
    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_UsernameTokenLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElementName, "RootElement");
    _LIT8(KUserName,        "UserName");
    _LIT8(KUserNameToken,   "<wsse:UsernameToken>\
<wsse:Username>UserName</wsse:Username>\
</wsse:UsernameToken>");

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewLC(document,
                                                                       rootElement);

    HBufC8* pUserNameToken = NULL;
    pWsSecHeader->UsernameTokenL(KUserName, pUserNameToken);
    CleanupStack::PushL(pUserNameToken);
    if(!( *pUserNameToken == KUserNameToken )) return KErrArgument;
    CleanupStack::PopAndDestroy(pUserNameToken);

    CleanupStack::PopAndDestroy(pWsSecHeader);

    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_UsernameTokenL_1L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElementName,     "RootElement");

    _LIT8(KFakeEndPoint,        "http://www.fake_endpoint.com/");
    _LIT8(KUsername,            "Username");
    _LIT8(KPassword,            "Password");

    _LIT8(KUserNameToken,       "<wsse:UsernameToken>\
<wsse:Username>Username</wsse:Username>\
<wsse:Password>Password</wsse:Password>\
</wsse:UsernameToken>");

    _LIT8(KUserNameTokenDigest, "<wsse:UsernameToken>\
<wsse:Username>Username</wsse:Username>\
<wsse:Password Type=\"wsse:PasswordDigest\">Password</wsse:Password>\
</wsse:UsernameToken>");

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenIdentityProvider* pIdP = CSenIdentityProvider::NewLC(KFakeEndPoint());
    pIdP->SetUserInfoL(KUsername, KNullDesC8, KPassword);

    // 1) Test using CSenWsSecurityHeader2::EText
    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewLC(document,
                                                                       rootElement);

    HBufC8* pUserNameToken = pWsSecHeader->UsernameTokenL(*pIdP, CSenWsSecurityHeader2::EText);
    CleanupStack::PushL(pUserNameToken);
    if(!( *pUserNameToken == KUserNameToken )) return KErrArgument;
    CleanupStack::PopAndDestroy(pUserNameToken);

    CleanupStack::PopAndDestroy(pWsSecHeader);

    // 2) Test using CSenWsSecurityHeader2::EDigest
    pWsSecHeader = CSenWsSecurityHeader2::NewLC(document, rootElement);

    pUserNameToken = pWsSecHeader->UsernameTokenL(*pIdP, CSenWsSecurityHeader2::EDigest);
    CleanupStack::PushL(pUserNameToken);
    if(!( *pUserNameToken == KUserNameTokenDigest )) return KErrArgument;
    CleanupStack::PopAndDestroy(pUserNameToken);

    CleanupStack::PopAndDestroy(pWsSecHeader);

    CleanupStack::PopAndDestroy(pIdP);

    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_UsernameTokenL_2L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElementName, "RootElement");
    _LIT8(KUserName,        "UserName");
    _LIT8(KPassword,        "Password");
    _LIT8(KUserNameToken,   "<wsse:UsernameToken>\
<wsse:Username>UserName</wsse:Username>\
<wsse:Password>Password</wsse:Password>\
</wsse:UsernameToken>");

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewLC(document,
                                                                       rootElement);

    HBufC8* pUserNameToken = NULL;
    pWsSecHeader->UsernameTokenL(KUserName, KPassword, pUserNameToken);
    CleanupStack::PushL(pUserNameToken);
    if(!( *pUserNameToken == KUserNameToken )) return KErrArgument;
    CleanupStack::PopAndDestroy(pUserNameToken);

    CleanupStack::PopAndDestroy(pWsSecHeader);

    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_UsernameTokenL_3L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElementName,     "RootElement");
    _LIT8(KUserName,            "UserName");
    _LIT8(KPassword,            "Password");
    _LIT8(KUserNameToken,       "<wsse:UsernameToken>\
<wsse:Username>UserName</wsse:Username>\
<wsse:Password>Password</wsse:Password>\
</wsse:UsernameToken>");

    _LIT8(KUserNameTokenDigest, "<wsse:UsernameToken>\
<wsse:Username>UserName</wsse:Username>\
<wsse:Password Type=\"wsse:PasswordDigest\">Password</wsse:Password>\
</wsse:UsernameToken>");

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    // 1) Test using CSenWsSecurityHeader2::EText
    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewLC(document,
                                                                       rootElement);

    HBufC8* pUserNameToken = NULL;
    pWsSecHeader->UsernameTokenL(KUserName, KPassword, CSenWsSecurityHeader2::EText,
                                 pUserNameToken);
    CleanupStack::PushL(pUserNameToken);
    if(!( *pUserNameToken == KUserNameToken )) return KErrArgument;
    CleanupStack::PopAndDestroy(pUserNameToken);

    CleanupStack::PopAndDestroy(pWsSecHeader);

    // 2) Test using CSenWsSecurityHeader2::EDigest
    pWsSecHeader = CSenWsSecurityHeader2::NewLC(document, rootElement);

    pUserNameToken = NULL;
    pWsSecHeader->UsernameTokenL(KUserName, KPassword, CSenWsSecurityHeader2::EDigest,
                                 pUserNameToken);
    CleanupStack::PushL(pUserNameToken);
    if(!( *pUserNameToken == KUserNameTokenDigest )) return KErrArgument;
    CleanupStack::PopAndDestroy(pUserNameToken);

    CleanupStack::PopAndDestroy(pWsSecHeader);

    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_UsernameTokenL_4L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElementName, "RootElement");

    _LIT8(KFakeEndPoint,    "http://www.fake_endpoint.com/");
    _LIT8(KUsername,        "Username");
    _LIT8(KPassword,        "Password");

    _LIT8(KUserNameToken,   "<wsse:UsernameToken>\
<wsse:Username>Username</wsse:Username>\
</wsse:UsernameToken>");

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewLC(document,
                                                                       rootElement);

    CSenIdentityProvider* pIdP = CSenIdentityProvider::NewLC(KFakeEndPoint());
    pIdP->SetUserInfoL(KUsername, KNullDesC8, KPassword);

    HBufC8* pUserNameToken = pWsSecHeader->UsernameTokenL(*pIdP);
    CleanupStack::PushL(pUserNameToken);
    if(!( *pUserNameToken == KUserNameToken )) return KErrArgument;
    CleanupStack::PopAndDestroy(pUserNameToken);

    CleanupStack::PopAndDestroy(pIdP);

    CleanupStack::PopAndDestroy(pWsSecHeader);

    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_TimestampLL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElementName, "RootElement");
    _LIT8(KCreated,         "2001-09-13T08:42:00Z");
    _LIT8(KExpires,         "2002-09-13T08:42:00Z");
    _LIT8(KTimeStamp,       "\
<wsu:Timestamp xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">\
<wsu:Created>2001-09-13T08:42:00Z</wsu:Created>\
<wsu:Expires>2002-09-13T08:42:00Z</wsu:Expires>\
</wsu:Timestamp>");

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewLC(document,
                                                                       rootElement);
    HBufC8* pTimeStamp;
    pWsSecHeader->TimestampL(KCreated, KExpires, pTimeStamp);
    CleanupStack::PushL(pTimeStamp);
    if(!( *pTimeStamp == KTimeStamp )) return KErrArgument;
    CleanupStack::PopAndDestroy(pTimeStamp);

    CleanupStack::PopAndDestroy(pWsSecHeader);

    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_TimestampL_1L( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElementName, "RootElement");
    _LIT8(KCreated,         "2001-09-13T08:42:00Z");
    _LIT8(KTimeStamp,       "\
<wsu:Timestamp xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">\
<wsu:Created>2001-09-13T08:42:00Z</wsu:Created>\
</wsu:Timestamp>");

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewLC(document,
                                                                       rootElement);
    HBufC8* pTimeStamp;
    pWsSecHeader->TimestampL(KCreated, pTimeStamp);
    CleanupStack::PushL(pTimeStamp);
    if(!( *pTimeStamp == KTimeStamp )) return KErrArgument;
    CleanupStack::PopAndDestroy(pTimeStamp);

    CleanupStack::PopAndDestroy(pWsSecHeader);

    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_XmlNsL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElementName,     "RootElement");
    _LIT8(KSecurityNamespace,   "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewLC(document,
                                                                       rootElement);
    TPtrC8 nameSpace = pWsSecHeader->XmlNs();
    if(!( nameSpace == KSecurityNamespace )) return KErrArgument;

    CleanupStack::PopAndDestroy(pWsSecHeader);

    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

TInt CnewsoapclassesTester::MT_CSenWsSecurityHeader2_XmlNsPrefixL( CStifItemParser& aItem )
    {
	SetupL();
    _LIT8(KRootElementName,         "RootElement");
    _LIT8(KSecurityNamespacePrefix, "wsse");

    RSenDocument document = RSenDocument::NewLC();
    TXmlEngElement rootElement = document.CreateDocumentElementL(KRootElementName());

    CSenWsSecurityHeader2* pWsSecHeader = CSenWsSecurityHeader2::NewLC(document,
                                                                       rootElement);
    TPtrC8 prefix = pWsSecHeader->XmlNsPrefix();
    if(!( prefix == KSecurityNamespacePrefix )) return KErrArgument;

    CleanupStack::PopAndDestroy(pWsSecHeader);

    CleanupStack::PopAndDestroy(1); // document
    Teardown();
    return KErrNone;    
	}

// -----------------------------------------------------------------------------
// CnewsoapclassesTester::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CnewsoapclassesTester::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
