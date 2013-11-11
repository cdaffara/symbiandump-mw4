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
#include <StifTestModule.h>
#include <e32math.h>
#include <libc/assert.h>
#include <e32panic.h>
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "senutilsTestCases.h"
#include <SenSoapEnvelope.h>

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
// CsenutilsTestCases::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CsenutilsTestCases::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CsenutilsTestCases::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CsenutilsTestCases::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "NewL-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_NewLL ),
        ENTRY( "SetBodyL-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_SetBodyLL ),
        ENTRY( "BodyL-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_BodyLL ),
        ENTRY( "HeaderL-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_HeaderLL ),        
        ENTRY( "AddHeaderL-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_AddHeaderLL ),
        ENTRY( "BodyAsStringL-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_BodyAsStringLL ),
        ENTRY( "DetachFaultL-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_DetachFaultLL ),
        ENTRY( "FaultL-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_FaultLL ),
        ENTRY( "SoapAction-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_SoapActionL ),
        ENTRY( "SoapAction2-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_SoapAction2L ),
        ENTRY( "HasHeader-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_HasHeaderL ),
        ENTRY( "SoapVersion-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_SoapVersionL ),
        ENTRY( "HasBody-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_HasBodyL ),
        ENTRY( "IsFault-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_IsFaultL ),
        ENTRY( "ParseL-CSenSoapEnvelope", CsenutilsTestCases::CSenSoapEnvelope_ParseLL ),
        
        ENTRY( "NewL-CSenSoapFault", CsenutilsTestCases::CSenSoapFault_NewLL ),
        ENTRY( "NewL-CSenSoapFault", CsenutilsTestCases::CSenSoapFault_NewL_1L ),
        ENTRY( "FaultCode-CSenSoapFault", CsenutilsTestCases::CSenSoapFault_FaultCodeL ),
        ENTRY( "FaultSubcode-CSenSoapFault", CsenutilsTestCases::CSenSoapFault_FaultSubcodeL ),
        
        ENTRY( "FaultString-CSenSoapFault", CsenutilsTestCases::CSenSoapFault_FaultStringL ),
        ENTRY( "FaultActor-CSenSoapFault", CsenutilsTestCases::CSenSoapFault_FaultActorL ),
        ENTRY( "Detail-CSenSoapFault", CsenutilsTestCases::CSenSoapFault_DetailL ),
        ENTRY( "NewL-CSenSoapMessage", CsenutilsTestCases::CSenSoapMessage_NewLL ),
        ENTRY( "SetSecurityHeaderL-CSenSoapMessage", CsenutilsTestCases::CSenSoapMessage_SetSecurityHeaderLL ),
        ENTRY( "AddSecurityTokenL-CSenSoapMessage", CsenutilsTestCases::CSenSoapMessage_AddSecurityTokenLL ),
        ENTRY( "NewL-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_NewLL ),
        ENTRY( "ParseL-CSenSoapMessage", CsenutilsTestCases::CSenSoapMessage_ParseLL ),
        ENTRY( "NewLC-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_NewLCL ),
        ENTRY( "NewL-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_NewL_1L ),
        ENTRY( "NewLC-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_NewLC_1L ),
        ENTRY( "NewL-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_NewL_2L ),
        ENTRY( "NewLC-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_NewLC_2L ),
        ENTRY( "BinarySecurityToken-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_BinarySecurityTokenL_L ),
        ENTRY( "BinarySecurityToken1-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_BinarySecurityTokenL_1L ),
        ENTRY( "UsernameTokenL-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_UsernameTokenLL ),
        ENTRY( "TimestampL-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_TimestampL_L ),
        ENTRY( "TimestampL-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_TimestampL_1L ),        
        ENTRY( "UsernameTokenL-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_UsernameTokenL_1L ),
        ENTRY( "UsernameTokenL-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_UsernameTokenL_2L ),
        ENTRY( "UsernameTokenL-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_UsernameTokenL_3L ),
        ENTRY( "UsernameTokenL-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_UsernameTokenL_4L ),
        ENTRY( "XmlNs-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_XmlNsL ),
        ENTRY( "XmlNsPrefix-CSenWsSecurityHeader", CsenutilsTestCases::CSenWsSecurityHeader_XmlNsPrefixL ),
        ENTRY( "FromXmlDateTimeL-SenDateUtils", CsenutilsTestCases::SenDateUtils_FromXmlDateTimeLL ),
        ENTRY( "ToXmlDateTimeUtf8L-SenDateUtils", CsenutilsTestCases::SenDateUtils_ToXmlDateTimeUtf8LL ),
        ENTRY( "ToXmlDateTimeUtf82L-SenDateUtils", CsenutilsTestCases::SenDateUtils_ToXmlDateTimeUtf82LL ),
        
        ENTRY( "FileAttachmentL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_FileAttachmentLL ),
        ENTRY( "NewL-CSenXmlProperties", CsenutilsTestCases::SenXmlProperties_NewLL ),
        ENTRY( "NewLC-CSenXmlProperties", CsenutilsTestCases::SenXmlProperties_NewLCL ),
        ENTRY( "NewL-CSenXmlProperties", CsenutilsTestCases::SenXmlProperties_NewL_1L ),
        ENTRY( "NewLC-CSenXmlProperties", CsenutilsTestCases::SenXmlProperties_NewLC_1L ),
        ENTRY( "NewL-CSenXmlProperties", CsenutilsTestCases::SenXmlProperties_NewL_2L ),
        ENTRY( "NewLC-CSenXmlProperties", CsenutilsTestCases::SenXmlProperties_NewLC_2L ),
        ENTRY( "NewL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_NewLL ),
        ENTRY( "NewLC-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_NewLCL ),
        ENTRY( "NewL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_NewL_1L ),
        ENTRY( "NewLC-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_NewLC_1L ),
        ENTRY( "NewL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_NewL_2L ),
        ENTRY( "NewLC-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_NewLC_2L ),
        ENTRY( "AsUtf8L-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_AsUtf8LL ),
        ENTRY( "SetPropertyL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetPropertyLL ),
        ENTRY( "PropertyL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_PropertyLL ),
        ENTRY( "SetIntPropertyL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetIntPropertyLL ),
        ENTRY( "PropertyL1-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_PropertyL1L ),
        ENTRY( "IntPropertyL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_IntPropertyLL ),
        ENTRY( "SetBoolPropertyL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetBoolPropertyLL ),
        ENTRY( "BoolPropertyL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_BoolPropertyLL ),
        ENTRY( "SetOmittedL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetOmittedLL ),
        ENTRY( "RemovePropertyL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_RemovePropertyLL ),
        ENTRY( "HeartbeatL_CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_HeartbeatLL ),
        ENTRY( "SetHeartbeatL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetHeartbeatLL ),
        ENTRY( "IapIdL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_IapIdLL ),
        ENTRY( "SetIapIdL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetIapIdLL ),
        ENTRY( "ProxyPortL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_ProxyPortLL ),
        ENTRY( "SetProxyPortL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetProxyPortLL ),
        ENTRY( "ProxyHostL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_ProxyHostLL ),
        ENTRY( "SetProxyHostL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetProxyHostLL ),
        ENTRY( "ProxyUsageL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_ProxyUsageLL ),
        ENTRY( "SetProxyUsageL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetProxyUsageLL ),
        ENTRY( "SecureDialogL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SecureDialogLL ),
        ENTRY( "SetSecureDialogL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetSecureDialogLL ),
        ENTRY( "UserAgentL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_UserAgentLL ),
        ENTRY( "SetUserAgentL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetUserAgentLL ),
        ENTRY( "DeviceIDL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_DeviceIDLL ),
        ENTRY( "SetDeviceIDL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetDeviceIDLL ),
        ENTRY( "SoapActionL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SoapActionLL ),
        ENTRY( "SetSoapActionL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetSoapActionLL ),
        ENTRY( "DownloadFolderL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_DownloadFolderLL ),
        ENTRY( "SetDownloadFolderL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetDownloadFolderLL ),
        ENTRY( "SetFileAttachmentL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetFileAttachmentLL ),
        ENTRY( "MwsNamespaceL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_MwsNamespaceLL ),
        ENTRY( "SetMwsNamespaceL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetMwsNamespaceLL ),
        ENTRY( "MessageIdL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_MessageIdLL ),
        ENTRY( "SetMessageIdL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetMessageIdLL ),
        ENTRY( "OnewayMessageOnOffL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_OnewayMessageOnOffLL ),
        ENTRY( "SetOnewayMessageOnOffL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetOnewayMessageOnOffLL ),
        ENTRY( "SetMaxTimeToLiveL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetMaxTimeToLiveLL ),
        ENTRY( "MaxTimeToLiveL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_MaxTimeToLiveLL ),
        ENTRY( "SetMaxTimeToLiveL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetMaxTimeToLiveLL),
        ENTRY( "MinTimeToLiveL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_MinTimeToLiveLL ),
        ENTRY( "SetMinTimeToLiveL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetMinTimeToLiveLL ),
        ENTRY( "SetReaderL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_SetReaderL ),
        ENTRY( "PropertiesClassTypeL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_PropertiesClassTypeL ),
        ENTRY( "WriteToLL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_WriteToLL ),
        ENTRY( "ReadFromLL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_ReadFromLL ),
        ENTRY( "CloneL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_CloneL ),
        ENTRY( "IsSafeToCastL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_IsSafeToCastL ),
        ENTRY( "CloneLL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_CloneLL ),
        ENTRY( "ApplyBindingLL-CSenTransportProperties", CsenutilsTestCases::SenTransportProperties_ApplyBindingLL ),
        ENTRY( "NewL-CSenHttpTransportProperties", CsenutilsTestCases::SenHttpTransportProperties_NewLL ),
        ENTRY( "NewLC-CSenHttpTransportProperties", CsenutilsTestCases::SenHttpTransportProperties_NewLCL ),
        ENTRY( "NewL-CSenHttpTransportProperties", CsenutilsTestCases::SenHttpTransportProperties_NewL_1L ),
        ENTRY( "NewLC-CSenHttpTransportProperties", CsenutilsTestCases::SenHttpTransportProperties_NewLC_1L ),
        ENTRY( "NewL-CSenHttpTransportProperties", CsenutilsTestCases::SenHttpTransportProperties_NewL_2L ),
        ENTRY( "NewLC-CSenHttpTransportProperties", CsenutilsTestCases::SenHttpTransportProperties_NewLC_2L),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CsenutilsTestCases::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CsenutilsTestCases::SetupL(  ){
    if ( iXmlReader ){
        delete iXmlReader;
        iXmlReader = NULL;
    }
    iXmlReader = CSenXmlReader::NewL();
} 

void CsenutilsTestCases::Teardown(  ){
    if ( iXmlReader){
        delete iXmlReader;
        iXmlReader = NULL;
    }
}


TPtr16 CsenutilsTestCases::ConvertToPtr16LC(CSenBaseFragment &fragment){
    HBufC16* xmlBuffer = fragment.AsElement().AsXmlUnicodeL();//Parent()->
    CleanupStack::PushL( xmlBuffer );
    TPtr16 ptr = xmlBuffer->Des();
    if (ptr.Length()){
        RDebug::Print( _L( "WSModTester: xmlBuffer length [%d]"),ptr.Length());
        if (ptr.Length()<=254){//below string must be =< max byte value
            RDebug::Print( _L( "WSModTester: xmlBuffer #START#"));
            RDebug::Print( _L( "[%S]"),&ptr);
            RDebug::Print( _L( "WSModTester: xmlBuffer #END#"));
        }
        else
            RDebug::Print( _L( "WSModTester: xmlBuffer too long to display, must be <=256- 2//[]"));
    }
    else
        RDebug::Print( _L( "WSModTester: xmlBuffer empty"));
    return ptr;
}

TPtr16 CsenutilsTestCases::ConvertToPtr16LC(MSenElement &element){
    HBufC16* xmlBuffer = element.AsXmlUnicodeL();//Parent()->
    CleanupStack::PushL( xmlBuffer );
    TPtr16 ptr = xmlBuffer->Des();
    if (ptr.Length() > 0){
        RDebug::Print( _L( "WSModTester: xmlBuffer length [%d]"),ptr.Length());
        if (ptr.Length()<=254){//below string must be =< max byte value
            RDebug::Print( _L( "WSModTester: xmlBuffer #START#"));
            RDebug::Print( _L( "[%S]"),&ptr);
            RDebug::Print( _L( "WSModTester: xmlBuffer #END#"));
        }
        else
            RDebug::Print( _L( "WSModTester: xmlBuffer too long to display, must be <=256- 2//[]"));
    }
    else
        RDebug::Print( _L( "WSModTester: xmlBuffer empty"));
    return ptr;
}

TInt CsenutilsTestCases::CSenSoapEnvelope_NewLL( CStifItemParser& aItem )
    {
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    CleanupStack::PushL(env);
    /*EUNIT_ASSERT_NOT_EQUALS(env, (CSenSoapEnvelope*)NULL);*/
    TL(env != (CSenSoapEnvelope*)NULL);
    //checking xml
    _LIT16(KRef, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    TPtrC16 ptrRef = KRef();
    /*EUNIT_ASSERT_EQUALS(ConvertToPtr16LC(*env), ptrRef);*/
    TL(ConvertToPtr16LC(*env) == ptrRef);
    CleanupStack::PopAndDestroy();//Ptr
    CleanupStack::Pop(env);
    /*EUNIT_ASSERT_NO_LEAVE(delete env);*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
  //  Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapEnvelope_SetBodyLL( CStifItemParser& aItem )
    {
    SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS(env, (CSenSoapEnvelope*)NULL); */
    TL(env != (CSenSoapEnvelope*)NULL);
    _LIT8(KBody,"ComarchBody");
    TPtrC8 result = env->SetBodyL(KBody);
    
    _LIT16(KRef2, "ComarchBody");
    TPtrC16 result16(reinterpret_cast<const TUint16*>(result.Ptr()),(result.Size()/2));
    RDebug::Print( _L( "WSModTester: body [%S]"),&result16);
    /*EUNIT_ASSERT_EQUALS(result, KBody);*/
    TL(result == KBody);
    _LIT16(KRef, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Body>ComarchBody</S:Body></S:Envelope>");
    /*EUNIT_ASSERT_EQUALS(ConvertToPtr16LC(*env), KRef);*/
    TL(ConvertToPtr16LC(*env) == KRef);
    CleanupStack::PopAndDestroy();//Ptr

    /*EUNIT_ASSERT_NO_LEAVE(delete env);*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapEnvelope_BodyLL( CStifItemParser& aItem )
    {
    SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS(env, (CSenSoapEnvelope*)NULL); */
    TL(env != (CSenSoapEnvelope*)NULL);
    _LIT8(KBody,"ComarchBody");
    env->SetBodyL(KBody);
    
    CSenElement& element = env->BodyL();
    
    _LIT16(KRef, "<S:Body>ComarchBody</S:Body>");
    /*EUNIT_ASSERT_EQUALS(ConvertToPtr16LC(element), KRef);*/
    TL(ConvertToPtr16LC(element) == KRef);
    CleanupStack::PopAndDestroy();//Ptr

    /*EUNIT_ASSERT_NO_LEAVE(delete env);*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapEnvelope_HeaderLL( CStifItemParser& aItem )
    {
    SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS(env, (CSenSoapEnvelope*)NULL); */
    TL(env != (CSenSoapEnvelope*)NULL);
    CSenBaseElement* el = CSenBaseElement::NewL(KText);
    env->AddHeaderL(*el);
    el = &(CSenBaseElement&)env->HeaderL();
    
    _LIT16(KRef, "<S:Header><text/></S:Header>");
    /*EUNIT_ASSERT_EQUALS(ConvertToPtr16LC(*el), KRef);*/
    TL(ConvertToPtr16LC(*el)== KRef);
    CleanupStack::PopAndDestroy();//Ptr
    
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapEnvelope_AddHeaderLL( CStifItemParser& aItem )
    {
    SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(env != (CSenSoapEnvelope*)NULL);

    CSenBaseElement* el = CSenBaseElement::NewL(KText);
    //el = &(CSenBaseElement&) env->AddHeaderL(*el);//ok
    el = &(CSenBaseElement&) env->AddHeaderL(*el);//ok
    //checking xml
    _LIT16(KRef, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Header><text/></S:Header><S:Body/></S:Envelope>");
    TL(ConvertToPtr16LC(*env) == KRef);
    CleanupStack::PopAndDestroy();//Ptr
    
    _LIT16(KRef2, "<text/>");
    TL(ConvertToPtr16LC(*el)== KRef2);
    CleanupStack::PopAndDestroy();//Ptr
    
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapEnvelope_BodyAsStringLL( CStifItemParser& aItem )
    {
    SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(env != (CSenSoapEnvelope*)NULL);

    _LIT8(KBody,"ComarchBody");
    env->SetBodyL(KBody);
    HBufC8* str = env->BodyAsStringL();
    
    _LIT8(KRef, "<S:Body xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\">ComarchBody</S:Body>");
    TPtrC8 ptrRef = KRef();
    TPtr8 ptr = str->Des();
    TL(ptr == ptrRef);
    
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete str);
    str = NULL;
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapEnvelope_DetachFaultLL( CStifItemParser& aItem )
    {
    SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    env->SetReader( *iXmlReader );
    iXmlReader->SetContentHandler(*env);
    env->ParseL(KFaultMessage);
    CSenSoapFault* fault = env->DetachFaultL();

    //checking Envelope without Fault
    _LIT16(KRefEnvFault, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Body/></S:Envelope>");
    TPtrC16 ptrRefEnvFault = KRefEnvFault();
    TL(ConvertToPtr16LC(*env) == ptrRefEnvFault);
    CleanupStack::PopAndDestroy();//Ptr

    //checking Fault
    _LIT16(KRefFault, "<S:Fault xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><faultcode>VersionMismatch</faultcode><faultstring>some string</faultstring><faultactor>some actor</faultactor><detail>bla bla</detail></S:Fault>");
    TPtrC16 ptrRefFault = KRefFault();
    TL(ConvertToPtr16LC(*fault) == ptrRefFault);
    CleanupStack::PopAndDestroy();//Ptr
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete fault);
    fault = NULL;
    

    env = CSenSoapEnvelope::NewL();
    env->SetReader( *iXmlReader );
    iXmlReader->SetContentHandler(*env);
    env->ParseL(KSOAPMessage);
    TL(env->DetachFaultL() == (CSenSoapFault* )NULL);
    //checking Envelope not touched should be
    _LIT16(KRefEnv, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Header/><S:Body><QueryResponse><Data>Hard Coded response, alway the same</Data></QueryResponse></S:Body></S:Envelope>");
    TPtrC16 ptrRefEnv = KRefEnv();
    TL(ConvertToPtr16LC(*env) == ptrRefEnv);
    CleanupStack::PopAndDestroy();//Ptr
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapEnvelope_FaultLL( CStifItemParser& aItem )
    {
    SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    env->SetReader( *iXmlReader );
    iXmlReader->SetContentHandler(*env);


    env->ParseL(KFaultMessage);
    CSenSoapFault* fault = env->FaultL();

    //checking Envelope, still Fault exist
    _LIT16(KRefEnvFault, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Body><S:Fault><faultcode>VersionMismatch</faultcode><faultstring>some string</faultstring><faultactor>some actor</faultactor><detail>bla bla</detail></S:Fault></S:Body></S:Envelope>");
    TL(ConvertToPtr16LC(*env)== KRefEnvFault);
    CleanupStack::PopAndDestroy();//Ptr
    //checking Fault
    _LIT16(KRefFault, "<S:Fault><faultcode>VersionMismatch</faultcode><faultstring>some string</faultstring><faultactor>some actor</faultactor><detail>bla bla</detail></S:Fault>");
    TL(ConvertToPtr16LC(*fault) == KRefFault);
    CleanupStack::PopAndDestroy();//Ptr
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    
    
    env = CSenSoapEnvelope::NewL();
    env->SetReader( *iXmlReader );
    iXmlReader->SetContentHandler(*env);
    env->ParseL(KSOAPMessage);
    TL(env->FaultL() == (CSenSoapFault* )NULL);
    //checking Envelope not touched should be
    _LIT16(KRefEnv, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Header/><S:Body><QueryResponse><Data>Hard Coded response, alway the same</Data></QueryResponse></S:Body></S:Envelope>");
    TL(ConvertToPtr16LC(*env) == KRefEnv);
    CleanupStack::PopAndDestroy();//Ptr
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapEnvelope_SoapActionL( CStifItemParser& aItem )
    {
    SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(env != (CSenSoapEnvelope*)NULL);

    _LIT8(KRef, "\"\"");
    _LIT8(KRef2, "\"URI-referenceRFC2396\"");
    _LIT8(KRef3, "");
    TL(env->SoapAction() == KRef);
    env->SetSoapActionL(_L8("URI-referenceRFC2396"));
    TL(env->SoapAction() == KRef2);
    env->SetSoapActionL(KRef2);
    TL(env->SoapAction() == KRef2);
    
    env->SetSoapActionL(KRef3);
    TL(env->SoapAction() == KRef3);
    

    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapEnvelope_SoapAction2L( CStifItemParser& aItem )
    {
    SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    TL(env != (CSenSoapEnvelope*)NULL);

    _LIT8(KRef, "");
    _LIT8(KRef2, "\"URI-referenceRFC2396\"");

    TL(env->SoapAction2() == KRef);
    env->SetSoapActionL(_L8("URI-referenceRFC2396"));
    TL(env->SoapAction2() == KRef2);
    env->SetSoapActionL(KRef2);
    TL(env->SoapAction2() == KRef2);
    

    
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapEnvelope_HasHeaderL( CStifItemParser& aItem )
    {
    SetupL();
    
    _LIT8(KHeaderElementName,   "HeaderElement");
    _LIT8(KHeaderContent,       "HeaderContent");

    CSenSoapEnvelope* pEnvelope = CSenSoapEnvelope::NewL();
    
    if(!( pEnvelope->HasHeader() == EFalse )) return KErrArgument;
    CSenBaseElement* el = CSenBaseElement::NewL(KText);
    pEnvelope->AddHeaderL(*el);
    if(( pEnvelope->HasHeader() == EFalse )) return KErrArgument;
    
    
    __ASSERT_ALWAYS_NO_LEAVE(delete pEnvelope);
    pEnvelope = NULL;
    Teardown();
    return KErrNone;
    
    //EUNIT_ASSERT_NO_LEAVE(delete el);
    //el = NULL;
    }
   
TInt CsenutilsTestCases::CSenSoapEnvelope_SoapVersionL( CStifItemParser& aItem )
    {
    SetupL();
    
    TSOAPVersion var = ESOAP11;
    CSenSoapEnvelope* pEnvelope = CSenSoapEnvelope::NewL();
    CleanupStack::PushL(pEnvelope);
    
    TL(var ==  pEnvelope->SoapVersion());
    CleanupStack::PopAndDestroy(pEnvelope);
    Teardown();
    return KErrNone;
    
    }
    
TInt CsenutilsTestCases::CSenSoapEnvelope_HasBodyL( CStifItemParser& aItem )
    {
    SetupL();
    
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    TL(env != (CSenSoapEnvelope*)NULL);

    _LIT8(KBody,"ComarchBody");
    TPtrC8 result = env->SetBodyL(KBody);
    if(( env->HasBody() == EFalse )) return KErrArgument;
    
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    
    CSenSoapEnvelope* env1 = CSenSoapEnvelope::NewL();
    TL(env1 != (CSenSoapEnvelope*)NULL);

    if(!( env1->HasBody() == EFalse )) return KErrArgument;
    
    __ASSERT_ALWAYS_NO_LEAVE(delete env1);
    env1 = NULL;
    Teardown();
    return KErrNone;
    
    }
    
TInt CsenutilsTestCases::CSenSoapEnvelope_IsFaultL( CStifItemParser& aItem )
    {
    SetupL();
    
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    TL(env!= (CSenSoapEnvelope*)NULL);
    if(!( env->IsFault() == EFalse )) return KErrArgument;
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    Teardown();
    return KErrNone;
    
    }
    
TInt CsenutilsTestCases::CSenSoapEnvelope_ParseLL( CStifItemParser& aItem )
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
    CSenSoapEnvelope* pEnvelope = CSenSoapEnvelope::NewL();
    pEnvelope->SetReader( *iXmlReader );
    iXmlReader->SetContentHandler(*pEnvelope);
    
    CleanupStack::PushL(pEnvelope);
    
    
    pEnvelope->ParseL(KInputString);

    CleanupStack::PopAndDestroy(pEnvelope);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;    
    }
    


TInt CsenutilsTestCases::CSenSoapFault_NewLL( CStifItemParser& aItem )
    {
    SetupL();
    RAttributeArray attributeArray;
    _LIT8(KFault, "Fault");
    CSenSoapFault* soapFault = CSenSoapFault::NewL(KFault, KFault, KFault, attributeArray);
    CleanupStack::PushL(soapFault);
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(soapFault != (CSenSoapFault*)NULL);

    //checking xml
    _LIT16(KRef, "<Fault xmlns=\"Fault\"/>");
    TPtrC16 ptrRef = KRef();
    TL(ConvertToPtr16LC(*soapFault) == ptrRef);
    CleanupStack::PopAndDestroy();//Ptr
    CleanupStack::Pop(soapFault);
    attributeArray.Close();
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete soapFault);
    soapFault = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapFault_NewL_1L( CStifItemParser& aItem )
    {
    SetupL();
    RAttributeArray attributeArray;
    _LIT8(KFault, "Fault");
    CSenSoapFault* orgSoapFault = CSenSoapFault::NewL(KFault, KFault, KFault, attributeArray);
    CSenSoapFault* soapFault = CSenSoapFault::NewL(*orgSoapFault);
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(soapFault != (CSenSoapFault*)NULL);

    //checking xml
    _LIT16(KRef, "<Fault xmlns=\"Fault\"/>");
    TL(ConvertToPtr16LC(*soapFault) == KRef);
    CleanupStack::PopAndDestroy();//Ptr
    
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete orgSoapFault);
    orgSoapFault = NULL;
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete soapFault);
    soapFault = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapFault_FaultCodeL( CStifItemParser& aItem )
    {
    SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    env->SetReader( *iXmlReader );
    iXmlReader->SetContentHandler(*env);
    env->ParseL(KFaultMessage);
    CSenSoapFault* fault = env->DetachFaultL();
//not empty
    _LIT8(KCode, "VersionMismatch");
    
    __ASSERT_ALWAYS_NO_LEAVE(fault->FaultCode());
    TL(fault->FaultCode()== KCode);
    //TPtrC16 result16(reinterpret_cast<const TUint16*>(fault->FaultCode().Ptr()),(fault->FaultCode().Size()/2));
    //RDebug::Print( _L( "WSModTester: faultCode [%S]"),&result16);
    
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete fault);
    fault = NULL;
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;

//empty
    RAttributeArray attributeArray;
    fault = CSenSoapFault::NewL(KText, KText, KText, attributeArray);
    __ASSERT_ALWAYS_NO_LEAVE(fault->FaultCode());
    TL(fault->FaultCode() == KNullDesC8);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete fault);
    fault = NULL;
    Teardown();
    return KErrNone;
    }
    
TInt CsenutilsTestCases::CSenSoapFault_FaultSubcodeL( CStifItemParser& aItem )
    {
    SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    env->SetReader( *iXmlReader );
    iXmlReader->SetContentHandler(*env);
    env->ParseL(KFaultMessage);
    CSenSoapFault* fault = env->DetachFaultL();
    

//empty
    RAttributeArray attributeArray;
    fault = CSenSoapFault::NewL(KText, KText, KText, attributeArray);
    __ASSERT_ALWAYS_NO_LEAVE(fault->FaultSubcode());
    TL(fault->FaultSubcode() == KNullDesC8);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete fault);
    fault = NULL;
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapFault_FaultStringL( CStifItemParser& aItem )
    {
        SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    env->SetReader( *iXmlReader );
    iXmlReader->SetContentHandler(*env);
    env->ParseL(KFaultMessage);
    CSenSoapFault* fault = env->DetachFaultL();
//not empty
    _LIT8(KString, "some string");
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(fault->FaultString());
    TL(fault->FaultString() == KString);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete fault);
    fault = NULL;
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;

//empty
    RAttributeArray attributeArray;
    fault = CSenSoapFault::NewL(KText, KText, KText, attributeArray);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(fault->FaultString());
    TL(fault->FaultString() == KNullDesC8);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete fault);
    fault = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapFault_FaultActorL( CStifItemParser& aItem )
    {
        SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    env->SetReader( *iXmlReader );
    iXmlReader->SetContentHandler(*env);
    env->ParseL(KFaultMessage);
    CSenSoapFault* fault = env->DetachFaultL();
//not empty
    _LIT8(KActor, "some actor");
    /*EUNIT_ASSERT_NO_LEAVEP*/
    __ASSERT_ALWAYS_NO_LEAVE(fault->FaultActor());
    TL(fault->FaultActor() == KActor);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete fault);
    fault = NULL;
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;

//empty
    RAttributeArray attributeArray;
    fault = CSenSoapFault::NewL(KText, KText, KText, attributeArray);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(fault->FaultActor());
    TL(fault->FaultActor() == KNullDesC8);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete fault);
    fault = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapFault_DetailL( CStifItemParser& aItem )
    {
        SetupL();
CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    env->SetReader( *iXmlReader );
    iXmlReader->SetContentHandler(*env);
    env->ParseL(KFaultMessage);
    CSenSoapFault* fault = env->DetachFaultL();
//not empty
    _LIT8(KDetail, "bla bla");
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(fault->Detail());
    TL(fault->Detail() == KDetail);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete fault);
    fault = NULL;
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;

//empty
    RAttributeArray attributeArray;
    fault = CSenSoapFault::NewL(KText, KText, KText, attributeArray);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(fault->Detail());
    TL(fault->Detail() == KNullDesC8);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete fault);
    fault = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapMessage_NewLL( CStifItemParser& aItem )
    {
        SetupL();
    CSenSoapMessage* msg = CSenSoapMessage::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(msg != (CSenSoapMessage*)NULL);

    //checking xml
    _LIT16(KRef, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
    TL(ConvertToPtr16LC(*msg) == KRef);
    CleanupStack::PopAndDestroy();//Ptr
    
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete msg);
    msg = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapMessage_SetSecurityHeaderLL( CStifItemParser& aItem )
    {
    SetupL();
    CSenSoapMessage* msg = CSenSoapMessage::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(msg != (CSenSoapMessage*)NULL);

    
    msg->SetSecurityHeaderL(KText);
    //checking xml
    _LIT16(KRef, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Header><wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">text</wsse:Security></S:Header><S:Body/></S:Envelope>");
    TPtrC16 ptrRef = KRef();
    TL(ConvertToPtr16LC(*msg) == ptrRef);
    CleanupStack::PopAndDestroy();//Ptr
    
    msg->SetSecurityHeaderL(KText2);
    //checking xml
    _LIT16(KRef2, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Header><wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">text2</wsse:Security></S:Header><S:Body/></S:Envelope>");
    TPtrC16 ptrRef2 = KRef2();
    TL(ConvertToPtr16LC(*msg) == ptrRef2);
    CleanupStack::PopAndDestroy();//Ptr
    

    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete msg);
    msg = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapMessage_AddSecurityTokenLL( CStifItemParser& aItem )
    {
            SetupL();
    CSenSoapMessage* msg = CSenSoapMessage::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(msg != (CSenSoapMessage*)NULL);

    
    /*EUNIT_ASSERT*/TL(msg->AddSecurityTokenL(KText)==KErrNone);
    //checking xml
    _LIT16(KRef, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Header><wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">text</wsse:Security></S:Header><S:Body/></S:Envelope>");
    TPtrC16 ptrRef = KRef();
    TL(ConvertToPtr16LC(*msg) == ptrRef);
    CleanupStack::PopAndDestroy();//Ptr
    
    /*EUNIT_ASSERT*/TL(msg->AddSecurityTokenL(KText2)==KErrNone);
    //checking xml
    _LIT16(KRef2, "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Header><wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">texttext2</wsse:Security></S:Header><S:Body/></S:Envelope>");
    TPtrC16 ptrRef2 = KRef2();
    TL(ConvertToPtr16LC(*msg) == ptrRef2);
    CleanupStack::PopAndDestroy();//Ptr
    

    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete msg);
    msg = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenSoapMessage_ParseLL( CStifItemParser& aItem )
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
    CSenSoapMessage* pMessage = CSenSoapMessage::NewL();
    pMessage->SetReader( *iXmlReader ); 
    iXmlReader->SetContentHandler(*pMessage);
    
    CleanupStack::PushL(pMessage);
    
    
    pMessage->ParseL(KInputString);

    CleanupStack::PopAndDestroy(pMessage);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;    
    }


TInt CsenutilsTestCases::CSenWsSecurityHeader_NewLL( CStifItemParser& aItem )
    {
    SetupL();
    CSenWsSecurityHeader* hdr = CSenWsSecurityHeader::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(hdr != (CSenWsSecurityHeader*)NULL);
    CleanupStack::PushL(hdr);
    //checking xml
    _LIT16(KRef, "<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/>");
    TPtrC16 ptrRef = KRef();
    TL(ConvertToPtr16LC(*hdr) == ptrRef);
    CleanupStack::PopAndDestroy();//Ptr
    CleanupStack::Pop(hdr);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete hdr);
    hdr = NULL;
    Teardown();
    return KErrNone;
    }
    
 TInt CsenutilsTestCases::CSenWsSecurityHeader_NewLCL( CStifItemParser& aItem )
    {
    SetupL();
    CSenWsSecurityHeader* hdr = CSenWsSecurityHeader::NewLC();
    TL(hdr!= (CSenWsSecurityHeader*)NULL);
    
    //checking xml
    _LIT16(KRef, "<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\"/>");
    TPtrC16 ptrRef(KRef);
    TL(ConvertToPtr16LC(*hdr)== ptrRef);
    CleanupStack::PopAndDestroy(2);//Ptr
    Teardown();
    return KErrNone;
    }

  
TInt CsenutilsTestCases::CSenWsSecurityHeader_NewLC_1L( CStifItemParser& aItem )
    {
    SetupL();
    CSenWsSecurityHeader* hdr = CSenWsSecurityHeader::NewLC(KText);
    TL(hdr!= (CSenWsSecurityHeader*)NULL);

    //checking xml
    _LIT16(KRef, "<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">text</wsse:Security>");
    TPtrC16 ptrRef(KRef);
    TL(ConvertToPtr16LC(*hdr)== ptrRef);
    CleanupStack::PopAndDestroy(2);
    
    //EUNIT_ASSERT_NO_LEAVE(delete hdr);
    hdr = NULL;
    Teardown();
    return KErrNone;
    }
    
TInt CsenutilsTestCases::CSenWsSecurityHeader_NewL_2L( CStifItemParser& aItem )
    {
    SetupL();
    CSenWsSecurityHeader* hdr = CSenWsSecurityHeader::NewL(KText, KText);
    TL(hdr!= (CSenWsSecurityHeader*)NULL);
    CleanupStack::PushL(hdr);
    //checking xml
    _LIT16(KRef, "<wsse:Security xmlns:wsse=\"text\">text</wsse:Security>");
    TPtrC16 ptrRef(KRef);
    TL(ConvertToPtr16LC(*hdr)== ptrRef);
    CleanupStack::PopAndDestroy(2); 
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenWsSecurityHeader_NewLC_2L( CStifItemParser& aItem )
    {
    CSenWsSecurityHeader* hdr = CSenWsSecurityHeader::NewLC(KText, KText);
    TL(hdr != (CSenWsSecurityHeader*)NULL);

    //checking xml
    _LIT16(KRef, "<wsse:Security xmlns:wsse=\"text\">text</wsse:Security>");
    TPtrC16 ptrRef(KRef);
    TL(ConvertToPtr16LC(*hdr)== ptrRef);
    CleanupStack::PopAndDestroy(2);
    Teardown();
    return KErrNone;
    }
    
    
TInt CsenutilsTestCases::CSenWsSecurityHeader_BinarySecurityTokenL_L( CStifItemParser& aItem )
    {
    SetupL();
//testing NULL
    HBufC8* token=NULL;
    TL(CSenWsSecurityHeader::BinarySecurityTokenL(KNullDesC8, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef, "<wsse:BinarySecurityToken wsu:Id=\"token\" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"></wsse:BinarySecurityToken>");
    TPtrC8 ptrRef(KRef);
    TPtr8 ptrToken = token->Des();
    TL(ptrToken == ptrRef);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;

    
//testing normal    
    TL(CSenWsSecurityHeader::BinarySecurityTokenL(KText, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef2, "<wsse:BinarySecurityToken wsu:Id=\"token\" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">text</wsse:BinarySecurityToken>");
    TPtrC8 ptrRef2(KRef2);
    ptrToken = token->Des();
    TL(ptrToken == ptrRef2);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;
    Teardown();
    return KErrNone;
    }
    
TInt CsenutilsTestCases::CSenWsSecurityHeader_BinarySecurityTokenL_1L( CStifItemParser& aItem )
    {
    SetupL();
//testing NULL
    HBufC8* token=NULL;
    TL(CSenWsSecurityHeader::BinarySecurityTokenL(KNullDesC8, KNullDesC8, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef, "<wsse:BinarySecurityToken wsu:Id=\"token\" wsse:ValueType=\"\" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"></wsse:BinarySecurityToken>");
    TPtrC8 ptrRef(KRef);
    TPtr8 ptrToken = token->Des();
    TL(ptrToken == ptrRef);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;

    
//testing normal    
    TL(CSenWsSecurityHeader::BinarySecurityTokenL(KText, KText, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef2, "<wsse:BinarySecurityToken wsu:Id=\"token\" wsse:ValueType=\"text\" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">text</wsse:BinarySecurityToken>");
    TPtrC8 ptrRef2(KRef2);
    ptrToken = token->Des();
    TL(ptrToken == ptrRef2);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;
    Teardown();
    return KErrNone;
    }
    
TInt CsenutilsTestCases::CSenWsSecurityHeader_TimestampL_L( CStifItemParser& aItem )
    {
    SetupL();
//testing NULL
    HBufC8* timestamp=NULL;
    TL(CSenWsSecurityHeader::TimestampL(KNullDesC8, timestamp)==KErrNone);
    TL(timestamp != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef, "<wsu:Timestamp xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"><wsu:Created></wsu:Created></wsu:Timestamp>");
    TPtrC8 ptrRef(KRef);
    TPtr8 ptrToken = timestamp->Des();
    TL(ptrToken == ptrRef);
    __ASSERT_ALWAYS_NO_LEAVE(delete timestamp);
    timestamp = NULL;

    
//testing normal    
    TL(CSenWsSecurityHeader::TimestampL(KText, timestamp)==KErrNone);
    TL(timestamp != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef2, "<wsu:Timestamp xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"><wsu:Created>text</wsu:Created></wsu:Timestamp>");
    TPtrC8 ptrRef2(KRef2);
    ptrToken = timestamp->Des();
    TL(ptrToken == ptrRef2);
    __ASSERT_ALWAYS_NO_LEAVE(delete timestamp);
    timestamp = NULL;
    Teardown();
    return KErrNone;
    }
    
TInt CsenutilsTestCases::CSenWsSecurityHeader_TimestampL_1L( CStifItemParser& aItem )
    {
    SetupL();
//testing NULL
    HBufC8* timestamp=NULL;
    TL(CSenWsSecurityHeader::TimestampL(KNullDesC8, KNullDesC8, timestamp)==KErrNone);
    TL(timestamp != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef, "<wsu:Timestamp xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"><wsu:Created></wsu:Created><wsu:Expires></wsu:Expires></wsu:Timestamp>");
    TPtrC8 ptrRef(KRef);
    TPtr8 ptrToken = timestamp->Des();
    TL(ptrToken == ptrRef);
    __ASSERT_ALWAYS_NO_LEAVE(delete timestamp);
    timestamp = NULL;

     
    TL(CSenWsSecurityHeader::TimestampL(KNullDesC8, KText, timestamp)==KErrNone);
    TL(timestamp != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef2, "<wsu:Timestamp xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"><wsu:Created></wsu:Created><wsu:Expires>text</wsu:Expires></wsu:Timestamp>");
    TPtrC8 ptrRef2(KRef2);
    ptrToken = timestamp->Des();
    TL(ptrToken == ptrRef2);
    __ASSERT_ALWAYS_NO_LEAVE(delete timestamp);
    timestamp = NULL;
    
    TL(CSenWsSecurityHeader::TimestampL(KText, KNullDesC8, timestamp)==KErrNone);
    TL(timestamp != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef3, "<wsu:Timestamp xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"><wsu:Created>text</wsu:Created><wsu:Expires></wsu:Expires></wsu:Timestamp>");
    TPtrC8 ptrRef3(KRef3);
    ptrToken = timestamp->Des();
    TL(ptrToken == ptrRef3);
    __ASSERT_ALWAYS_NO_LEAVE(delete timestamp);
    timestamp = NULL;
    
//testing normal    
    TL(CSenWsSecurityHeader::TimestampL(KText, KText, timestamp)==KErrNone);
    TL(timestamp != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef4, "<wsu:Timestamp xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"><wsu:Created>text</wsu:Created><wsu:Expires>text</wsu:Expires></wsu:Timestamp>");
    TPtrC8 ptrRef4(KRef4);
    ptrToken = timestamp->Des();
    TL(ptrToken == ptrRef4);
    __ASSERT_ALWAYS_NO_LEAVE(delete timestamp);
    timestamp = NULL;
    Teardown();
    return KErrNone;
    }




    
TInt CsenutilsTestCases::CSenWsSecurityHeader_UsernameTokenL_2L( CStifItemParser& aItem )
    {
    SetupL();
//testing NULL
    HBufC8* token=NULL;
    TL(CSenWsSecurityHeader::UsernameTokenL(KNullDesC8, KNullDesC8, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef, "<wsse:UsernameToken><wsse:Username></wsse:Username><wsse:Password></wsse:Password></wsse:UsernameToken>");
    TPtrC8 ptrRef(KRef);
    TPtr8 ptrToken = token->Des();
    TL(ptrToken == ptrRef);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;

     
    TL(CSenWsSecurityHeader::UsernameTokenL(KNullDesC8, KText, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef2, "<wsse:UsernameToken><wsse:Username></wsse:Username><wsse:Password>text</wsse:Password></wsse:UsernameToken>");
    TPtrC8 ptrRef2(KRef2);
    ptrToken = token->Des();
    TL(ptrToken == ptrRef2);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;
    
    TL(CSenWsSecurityHeader::UsernameTokenL(KText, KNullDesC8, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef3, "<wsse:UsernameToken><wsse:Username>text</wsse:Username><wsse:Password></wsse:Password></wsse:UsernameToken>");
    TPtrC8 ptrRef3(KRef3);
    ptrToken = token->Des();
    TL(ptrToken == ptrRef3);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;
    
//testing normal    
    TL(CSenWsSecurityHeader::UsernameTokenL(KText, KText, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef4, "<wsse:UsernameToken><wsse:Username>text</wsse:Username><wsse:Password>text</wsse:Password></wsse:UsernameToken>");
    TPtrC8 ptrRef4(KRef4);
    ptrToken = token->Des();
    TL(ptrToken == ptrRef4);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;
    Teardown();
    return KErrNone;
    }
    
TInt CsenutilsTestCases::CSenWsSecurityHeader_UsernameTokenL_3L( CStifItemParser& aItem )
    {
    SetupL();
    CSenWsSecurityHeader::TPasswordType aType = CSenWsSecurityHeader::EText;

//testing NULL
    HBufC8* token=NULL;
    TL(CSenWsSecurityHeader::UsernameTokenL(KNullDesC8, KNullDesC8, aType, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef, "<wsse:UsernameToken><wsse:Username></wsse:Username><wsse:Password></wsse:Password></wsse:UsernameToken>");
    TPtrC8 ptrRef(KRef);
    TPtr8 ptrToken = token->Des();
    TL(ptrToken == ptrRef);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;

  
    TL(CSenWsSecurityHeader::UsernameTokenL(KNullDesC8, KText, aType, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef2, "<wsse:UsernameToken><wsse:Username></wsse:Username><wsse:Password>text</wsse:Password></wsse:UsernameToken>");
    TPtrC8 ptrRef2(KRef2);
    ptrToken = token->Des();
    TL(ptrToken == ptrRef2);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;
    
 
    TL(CSenWsSecurityHeader::UsernameTokenL(KText, KNullDesC8, aType, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef3, "<wsse:UsernameToken><wsse:Username>text</wsse:Username><wsse:Password></wsse:Password></wsse:UsernameToken>");
    TPtrC8 ptrRef3(KRef3);
    ptrToken = token->Des();
    TL(ptrToken == ptrRef3);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;
    
//testing normal    
    TL(CSenWsSecurityHeader::UsernameTokenL(KText, KText, aType, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef4, "<wsse:UsernameToken><wsse:Username>text</wsse:Username><wsse:Password>text</wsse:Password></wsse:UsernameToken>");
    TPtrC8 ptrRef4(KRef4);
    ptrToken = token->Des();
    TL(ptrToken == ptrRef4);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenWsSecurityHeader_UsernameTokenL_4L( CStifItemParser& aItem )
    {
    SetupL();
   
    CSenWsSecurityHeader* hdr = CSenWsSecurityHeader::NewL(KText);
    TL(hdr!= (CSenWsSecurityHeader*)NULL);

    CSenWsSecurityHeader::TPasswordType aType = CSenWsSecurityHeader::EText;

//testing NULL
    CSenIdentityProvider* provider = CSenIdentityProvider::NewL(KText, KText);
    HBufC8* token=NULL;
    token = hdr->UsernameTokenL(*provider, aType);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef, "<wsse:UsernameToken><wsse:Username></wsse:Username><wsse:Password></wsse:Password></wsse:UsernameToken>");
    TPtrC8 ptrRef(KRef);
    TPtr8 ptrToken = token->Des();
    TL(ptrToken == ptrRef);
    __ASSERT_ALWAYS_NO_LEAVE(delete provider);
    provider = NULL;
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;

    
//testing normal    
    provider = CSenIdentityProvider::NewL(KText, KText);
    provider->SetUserInfoL(KText, KText2, KText);
    token = hdr->UsernameTokenL(*provider, aType);
    TL(token != (HBufC8*)NULL);
    //checking xml
    _LIT8(KRef2, "<wsse:UsernameToken><wsse:Username>text</wsse:Username><wsse:Password>text</wsse:Password></wsse:UsernameToken>");
    TPtrC8 ptrRef2(KRef2);
    TPtr8 ptrToken2 = token->Des();
    TL(ptrToken2 == ptrRef2);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;
    __ASSERT_ALWAYS_NO_LEAVE(delete provider);
    provider = NULL;
    __ASSERT_ALWAYS_NO_LEAVE(delete hdr);
    hdr = NULL;
    Teardown();
    return KErrNone;
    
    }
    

TInt CsenutilsTestCases::CSenWsSecurityHeader_NewL_1L( CStifItemParser& aItem )
    {
            SetupL();
    CSenWsSecurityHeader* hdr = CSenWsSecurityHeader::NewL(KText);
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(hdr != (CSenWsSecurityHeader*)NULL);

    //checking xml
    _LIT16(KRef, "<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">text</wsse:Security>");
    TPtrC16 ptrRef = KRef();
    TL(ConvertToPtr16LC(*hdr) == ptrRef);
    CleanupStack::PopAndDestroy();//Ptr
    
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete hdr);
    hdr = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenWsSecurityHeader_UsernameTokenLL( CStifItemParser& aItem )
    {
            SetupL();
    CSenWsSecurityHeader* hdr = CSenWsSecurityHeader::NewL(KText);
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(hdr != (CSenWsSecurityHeader*)NULL);

//testing NULL
    CSenIdentityProvider* provider = CSenIdentityProvider::NewL(KText, KText);
    HBufC8* token=NULL;
    token = hdr->UsernameTokenL(*provider);
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef, "<wsse:UsernameToken><wsse:Username></wsse:Username></wsse:UsernameToken>");
    TPtrC8 ptrRef = KRef();
    TPtr8 ptrToken = token->Des();
    TL(ptrToken == ptrRef);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete provider);
    provider = NULL;
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;

    
//testing normal    
    provider = CSenIdentityProvider::NewL(KText, KText);
    provider->SetUserInfoL(KText, KText2, KText);
    token = hdr->UsernameTokenL(*provider);
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(token != (HBufC8*)NULL);
    //checking xml
    _LIT8(KRef2, "<wsse:UsernameToken><wsse:Username>text</wsse:Username></wsse:UsernameToken>");
    TPtrC8 ptrRef2 = KRef2();
    TPtr8 ptrToken2 = token->Des();
    TL(ptrToken2 == ptrRef2);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete provider);
    provider = NULL;
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete hdr);
    hdr = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenWsSecurityHeader_UsernameTokenL_1L( CStifItemParser& aItem )
    {
    SetupL();
//testing NULL
    HBufC8* token=NULL;
    /*EUNIT_ASSERT*/TL(CSenWsSecurityHeader::UsernameTokenL(KNullDesC8, token)==KErrNone);
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef, "<wsse:UsernameToken><wsse:Username></wsse:Username></wsse:UsernameToken>");
    TPtrC8 ptrRef = KRef();
    TPtr8 ptrToken = token->Des();
    TL(ptrToken == ptrRef);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;

    
//testing normal    
    /*EUNIT_ASSERT*/TL(CSenWsSecurityHeader::UsernameTokenL(KText, token)==KErrNone);
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef2, "<wsse:UsernameToken><wsse:Username>text</wsse:Username></wsse:UsernameToken>");
    TPtrC8 ptrRef2 = KRef2();
    ptrToken = token->Des();
    TL(ptrToken == ptrRef2);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenWsSecurityHeader_XmlNsL( CStifItemParser& aItem )
    {
            SetupL();
    CSenWsSecurityHeader* hdr = CSenWsSecurityHeader::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(hdr != (CSenWsSecurityHeader*)NULL);
    _LIT8(KSecurityXmlNsRef, "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd");
        /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(hdr->XmlNs());
    TL(hdr->XmlNs() == KSecurityXmlNsRef);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete hdr);
    hdr = NULL;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::CSenWsSecurityHeader_XmlNsPrefixL( CStifItemParser& aItem )
    {
            SetupL();
    CSenWsSecurityHeader* hdr = CSenWsSecurityHeader::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(hdr != (CSenWsSecurityHeader*)NULL);
    _LIT8(KSecurityXmlNsPrefixRef, "wsse");
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(hdr->XmlNsPrefix());
    TL(hdr->XmlNsPrefix() == KSecurityXmlNsPrefixRef);
    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete hdr);
    hdr = NULL;
    Teardown();
    return KErrNone;
    }


TInt CsenutilsTestCases::SenDateUtils_FromXmlDateTimeLL( CStifItemParser& aItem )
    {
            SetupL();
    //normal
    TTime ttime, ttime2;
    _LIT8(KIn1, "2005-02-28T13:20:05Z");
    _LIT(KDateTimeString1, "28-feb/05 01:20.05pm");
    ttime = SenDateUtils::FromXmlDateTimeL(KIn1);
    ttime2.Parse(KDateTimeString1);
    TL(ttime == ttime2);
    
    //-5
    _LIT8(KIn2, "2005-02-28T13:20:05-05:00");
    _LIT(KDateTimeString2, "28-feb/05 08:20.05am");
    ttime = SenDateUtils::FromXmlDateTimeL(KIn2);
    ttime2.Parse(KDateTimeString2);
    TL(ttime == ttime2);
    
    //+5
    _LIT8(KIn3, "2005-02-28T13:20:05+05:00");
    _LIT(KDateTimeString3, "28-feb/05 06:20.05pm");
    ttime = SenDateUtils::FromXmlDateTimeL(KIn3);
    ttime2.Parse(KDateTimeString3);
    TL(ttime == ttime2);
    
    //1 increase precision
    _LIT8(KIn4, "2005-02-28T13:20:05.5Z");
    _LIT(KDateTimeString4, "20050127:132005.500000");//The month and day values are offset from zero.
    ttime = SenDateUtils::FromXmlDateTimeL(KIn4);
    ttime2 = TTime(KDateTimeString4);
    TL(ttime == ttime2);
    
    
    //3 increase precision
    _LIT8(KIn5, "2005-02-28T13:20:05.231Z");
    _LIT(KDateTimeString5, "20050127:132005.231000");//The month and day values are offset from zero.
    ttime = SenDateUtils::FromXmlDateTimeL(KIn5);
    ttime2 = TTime(KDateTimeString5);
    TL(ttime == ttime2);

    //-----------wrong
    
    //month
    _LIT8(KIn6, "2005-55-28T13:20:05Z");
    TRAPD( err, SenDateUtils::FromXmlDateTimeL(KIn6));
    if(err != KErrOverflow) return err;
    
  //day
    _LIT8(KIn7, "2005-02-58T13:20:05Z");
    TRAP(err, SenDateUtils::FromXmlDateTimeL(KIn7));
    if(err != KErrOverflow) return err;

    
    //hour
    _LIT8(KIn9, "2005-02-28T33:20:05Z");
    TRAP(err, SenDateUtils::FromXmlDateTimeL(KIn9));
    if(err != KErrOverflow) return err;

    //minute
    _LIT8(KIn10, "2005-02-28T13:70:05Z");
    TRAP(err, SenDateUtils::FromXmlDateTimeL(KIn10));
    if(err != KErrOverflow) return err;

    //second
    _LIT8(KIn11, "2005-02-28T13:20:65Z");
    TRAP(err, SenDateUtils::FromXmlDateTimeL(KIn11));
    if(err != KErrOverflow) return err;

    //+hour
    _LIT8(KIn12, "2005-02-28T13:20:05+88:00");
    TRAP(err, SenDateUtils::FromXmlDateTimeL(KIn12));
    if(err != KErrOverflow) return err;

    //UTC wrong designator
    _LIT8(KIn13, "2005-02-28T13:20:65Z05:00");
    TRAP(err, SenDateUtils::FromXmlDateTimeL(KIn13));
    if(err != KErrOverflow) return err;

    
    //short garbage
    _LIT8(KIn14, "xx");
    TRAP(err, SenDateUtils::FromXmlDateTimeL(KIn14));
    if(err != KErrUnderflow) return err;

    //garbage year
    _LIT8(KIn15, "xxxx-02-28T13:20:65Z");
    //EUNIT_ASSERT_LEAVE
    //ASSERT(SenDateUtils::FromXmlDateTimeL(KIn15));

    //"-"
    _LIT8(KIn16, "2005x02-28T13:20:65Z");
    TRAP(err, SenDateUtils::FromXmlDateTimeL(KIn16));
        if(err != KErrGeneral) return err;

    _LIT8(KIn17, "2005-02x28T13:20:65Z");
    TRAP(err, SenDateUtils::FromXmlDateTimeL(KIn17));
        if(err != KErrGeneral) return err;

    _LIT8(KIn18, "2005-02-28X13:20:65Z");
    TRAP(err, SenDateUtils::FromXmlDateTimeL(KIn18));
        if(err != KErrGeneral) return err;

    _LIT8(KIn19, "2005-02-28X13:20:65Z");
    TRAP(err, SenDateUtils::FromXmlDateTimeL(KIn19));
        if(err != KErrGeneral) return err;

    _LIT8(KIn20, "2005-02-28T13-20:65Z");
    TRAP(err, SenDateUtils::FromXmlDateTimeL(KIn20));
        if(err != KErrGeneral) return err;

    _LIT8(KIn21, "2005-02-28T13:20-65Z");
    TRAP(err, SenDateUtils::FromXmlDateTimeL(KIn21));
        if(err != KErrGeneral) return err;

    //31 II
    _LIT8(KIn8, "2005-02-31T13:20:05Z");
//    EUNIT_ASSERT_PANIC(SenDateUtils::FromXmlDateTimeL(KIn8),"USER", 3 );
        TestModuleIf().SetExitReason (TestModuleIf(). EPanic, ETDateTimeBadDateTime);
        SenDateUtils::FromXmlDateTimeL(KIn8);
                
    Teardown();
    return KErrNone;
}

TInt CsenutilsTestCases::SenDateUtils_ToXmlDateTimeUtf8LL( CStifItemParser& aItem )
    {
    SetupL();
    //KXmlDateTimeMaxLength = 25
    _LIT(KDate, "23:34.56");
    _LIT8(KRef, "0000-01-01T23:34:56Z");
    TTime ttime;
    TInt error  = ttime.Parse(KDate);

    //KErrOverflow
    HBufC8* writeBuf=HBufC8::New(SenDateUtils::KXmlDateTimeMaxLength - 20);
    TPtr8 ptrResult = writeBuf->Des();
    TRAPD(err, SenDateUtils::ToXmlDateTimeUtf8L(ptrResult, ttime));
    if(err != KErrOverflow) return err;
    delete writeBuf;
    writeBuf = NULL;
    
    //OK, more
    writeBuf=HBufC8::New(SenDateUtils::KXmlDateTimeMaxLength+10);
    //ptrResult = writeBuf->Des(); it wrong, becouse = operator will only copy, creation has been done in previous case
    TPtr8 ptrResult2 = writeBuf->Des();
    // EUNIT_ASSERT_NO_LEAVE 
    __ASSERT_ALWAYS_NO_LEAVE(SenDateUtils::ToXmlDateTimeUtf8L(ptrResult2, ttime));
    TL(ptrResult2 == KRef);
    delete writeBuf;
    writeBuf = NULL; 

    //OK,equal
    writeBuf=HBufC8::New(SenDateUtils::KXmlDateTimeMaxLength);
    //ptrResult = writeBuf->Des(); it wrong, becouse = operator will only copy, creation has been done in previous case
    TPtr8 ptrResult3 = writeBuf->Des();
    //EUNIT_ASSERT_NO_LEAVE
    __ASSERT_ALWAYS_NO_LEAVE(SenDateUtils::ToXmlDateTimeUtf8L(ptrResult3, ttime));
    TL(ptrResult3 == KRef);
    delete writeBuf;
    writeBuf = NULL; 

    Teardown();
    return KErrNone;
    }

    
TInt CsenutilsTestCases::SenDateUtils_ToXmlDateTimeUtf82LL( CStifItemParser& aItem )
    {
    SetupL();
    //KXmlDateTimeMaxLength = 25
    _LIT(KDate, "23:34.56");
    _LIT8(KRef, "0000-01-01T23:34:56.000000Z");
    TTime ttime;
    TInt error  = ttime.Parse(KDate);

    //KErrOverflow
    HBufC8* writeBuf=HBufC8::New(SenDateUtils::KXmlDateTimeMaxLength - 20);
    TPtr8 ptrResult = writeBuf->Des();
    TRAPD(err, SenDateUtils::ToXmlDateTimeUtf82L(ptrResult, ttime));
    if(err != KErrOverflow) return err;
    delete writeBuf;
    writeBuf = NULL;
    
    //OK, more
    writeBuf=HBufC8::New(SenDateUtils::KXmlDateTimeMaxLength+10);
    //ptrResult = writeBuf->Des(); it wrong, becouse = operator will only copy, creation has been done in previous case
    TPtr8 ptrResult2 = writeBuf->Des();
    // EUNIT_ASSERT_NO_LEAVE 
    __ASSERT_ALWAYS_NO_LEAVE(SenDateUtils::ToXmlDateTimeUtf82L(ptrResult2, ttime));
    TL(ptrResult2 == KRef);
    delete writeBuf;
    writeBuf = NULL; 

    //OK,equal
    writeBuf=HBufC8::New(SenDateUtils::KXmlDateTimeMaxLength);
    //ptrResult = writeBuf->Des(); it wrong, becouse = operator will only copy, creation has been done in previous case
    TPtr8 ptrResult3 = writeBuf->Des();
    //EUNIT_ASSERT_NO_LEAVE
    __ASSERT_ALWAYS_NO_LEAVE(SenDateUtils::ToXmlDateTimeUtf82L(ptrResult3, ttime));
    TL(ptrResult3 == KRef);
    delete writeBuf;
    writeBuf = NULL; 

    Teardown();
    return KErrNone;
    }
    
TInt CsenutilsTestCases::SenTransportProperties_FileAttachmentLL(CStifItemParser& aItem )
{
    SetupL();

    CSenTransportProperties* transProp = CSenTransportProperties::NewLC();
    HBufC8* value = NULL;
    TL(transProp->FileAttachmentL(_L8("1"),value) == KErrNotFound);
    TL(value == (HBufC8*)NULL);
    
    transProp->SetFileAttachmentL(_L8("1"),_L8("abcd"));
    TL(transProp->FileAttachmentL(_L8("1"),value) == KErrNone);
    TL(*value ==  _L8("abcd"));

    delete value;
    CleanupStack::PopAndDestroy(transProp);
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenXmlProperties_NewLL(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    CSenXmlProperties *pXmlElement=CSenXmlProperties::NewL();
    CleanupStack::PushL(pXmlElement);
    if(pXmlElement)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pXmlElement);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenXmlProperties_NewLCL(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    CSenXmlProperties *pXmlElement=CSenXmlProperties::NewLC();

    if(pXmlElement)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pXmlElement);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenXmlProperties_NewL_1L(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    _LIT8(KString,"<parent><element1/><element2/><element3/>parent content</parent>");
    CSenXmlReader* pParser = CSenXmlReader::NewL();
    CleanupStack::PushL(pParser);
    CSenXmlProperties *pXmlElement=CSenXmlProperties::NewL(KString,*pParser);
    CleanupStack::PushL(pXmlElement);
    if(pXmlElement)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pXmlElement);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;
    }
    
TInt CsenutilsTestCases::SenXmlProperties_NewLC_1L(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    _LIT8(KString,"<parent><element1/><element2/><element3/>parent content</parent>");
    CSenXmlReader* pParser = CSenXmlReader::NewL();
    CleanupStack::PushL(pParser);
    CSenXmlProperties *pXmlElement=CSenXmlProperties::NewLC(KString,*pParser);
    if(pXmlElement)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pXmlElement);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenXmlProperties_NewL_2L(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement= CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    CSenXmlProperties *pXmlElement=CSenXmlProperties::NewL(*pElement);
    CleanupStack::PushL(pXmlElement);
    if(pXmlElement)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pXmlElement);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenXmlProperties_NewLC_2L(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement= CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    CSenXmlProperties *pXmlElement=CSenXmlProperties::NewLC(*pElement);
    if(pXmlElement)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pXmlElement);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenTransportProperties_NewLL(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);  
    if(pElement)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CsenutilsTestCases::SenTransportProperties_NewLCL(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    CSenTransportProperties* pElement= CSenTransportProperties::NewLC();
    if(pElement)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CsenutilsTestCases::SenTransportProperties_NewL_1L(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    _LIT8(KString,"<parent><element1/><element2/><element3/>parent content</parent>");
    CSenXmlReader* pParser = CSenXmlReader::NewL();
    CleanupStack::PushL(pParser);
    CSenTransportProperties *pXmlElement=CSenTransportProperties::NewL(KString,*pParser);
    CleanupStack::PushL(pXmlElement);
    if(pXmlElement)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pXmlElement);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;
    }
    
TInt CsenutilsTestCases::SenTransportProperties_NewLC_1L(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    _LIT8(KString,"<parent><element1/><element2/><element3/>parent content</parent>");
    CSenXmlReader* pParser = CSenXmlReader::NewL();
    CleanupStack::PushL(pParser);
    CSenTransportProperties *pXmlElement=CSenTransportProperties::NewLC(KString,*pParser);

    if(pXmlElement)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pXmlElement);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;
    }
    
TInt CsenutilsTestCases::SenTransportProperties_NewL_2L(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement= CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    CSenTransportProperties *pXmlElement=CSenTransportProperties::NewL(*pElement);
    CleanupStack::PushL(pXmlElement);
    if(pXmlElement)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pXmlElement);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CsenutilsTestCases::SenTransportProperties_NewLC_2L(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement= CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    CSenTransportProperties *pXmlElement=CSenTransportProperties::NewLC(*pElement);
    if(pXmlElement)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) KErrArgument;
    CleanupStack::PopAndDestroy(pXmlElement);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    

TInt CsenutilsTestCases::SenTransportProperties_AsUtf8LL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><Property>value</Property></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetPropertyL(_L8("Property"),_L8("value"));
    HBufC8* Utf8Buf=pElement->AsUtf8L();
    TL(*Utf8Buf == KElementText);
    CleanupStack::PopAndDestroy(pElement);    
    delete Utf8Buf;
    Teardown();
    return KErrNone;
    }
            
TInt CsenutilsTestCases::SenTransportProperties_SetPropertyLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><Property>value</Property></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    TInt retval=pElement->SetPropertyL(_L8("Property"),_L8("value"));
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(retval == KErrNone);
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    }
TInt CsenutilsTestCases::SenTransportProperties_PropertyLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KValue,"Value");
    TPtrC8 ValuePtr;
    TPtrC8 ValuePtr1;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetPropertyL(_L8("Property"),_L8("value"));
    
    //true case
    TInt retval=pElement->PropertyL(_L8("Property"),ValuePtr);
    TL(retval == KErrNone);
      
    //false case
    retval=pElement->PropertyL(_L8("NoProperty"),ValuePtr1);
    TL(retval == KErrNotFound);
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;

    }
TInt CsenutilsTestCases::SenTransportProperties_PropertyL1L(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KValue,"Value");
    TPtrC8 ValuePtr;
    TPtrC8 ValuePtr1;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetPropertyL(_L8("Property"),_L8("value"), _L8("type"));
    
    //true case
    TInt retval=pElement->PropertyL(_L8("Property"),ValuePtr, ValuePtr1);
    TL(retval == KErrNone);
      
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenTransportProperties_SetIntPropertyLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><Property>10</Property></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    TInt retval=pElement->SetIntPropertyL(_L8("Property"),10);
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(retval==KErrNone);
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    }
TInt CsenutilsTestCases::SenTransportProperties_IntPropertyLL(CStifItemParser& aItem )
{
    SetupL();
    TInt PropValue;
    TInt PropValue1;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetIntPropertyL(_L8("Property"),10);
    //true case
    TInt retval=pElement->IntPropertyL(_L8("Property"),PropValue);
    TL(retval == KErrNone);
    TL(PropValue == 10);
    //false case
    retval=pElement->IntPropertyL(_L8("NoProperty"),PropValue1);
    TL(retval == KErrNotFound);
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
TInt CsenutilsTestCases::SenTransportProperties_SetBoolPropertyLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><Property>true</Property></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    TInt retval=pElement->SetBoolPropertyL(_L8("Property"),1);
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(retval == KErrNone);
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);      
    delete ElementBuf;
    Teardown();
    return KErrNone;
    }
TInt CsenutilsTestCases::SenTransportProperties_BoolPropertyLL(CStifItemParser& aItem )
{
    SetupL();
    TBool BoolValue=1;
    TBool BoolValue1;
    TBool BoolValue2;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetBoolPropertyL(_L8("Property"),BoolValue);
    //true case
    TInt retval=pElement->BoolPropertyL(_L8("Property"),BoolValue1);
    TL(retval == KErrNone);
    TL(BoolValue == BoolValue1);
    //false case
    retval=pElement->BoolPropertyL(_L8("NoProperty"),BoolValue2);
    TL(retval == KErrNotFound);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
TInt CsenutilsTestCases::SenTransportProperties_SetOmittedLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KText,"<Properties><Property Omitted=\"true\">value</Property></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetPropertyL(_L8("Property"),_L8("value"));
    TInt retval=pElement->SetOmittedL(_L8("Property"),1);
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(retval == KErrNone);
    TL(*ElementBuf == KText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    }
TInt CsenutilsTestCases::SenTransportProperties_RemovePropertyLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KText,"<Properties><Property1>value</Property1><Property2>value</Property2></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetPropertyL(_L8("Property"),_L8("value"));
    pElement->SetPropertyL(_L8("Property1"),_L8("value"));
    pElement->SetPropertyL(_L8("Property2"),_L8("value"));
    //True
    TInt retval=pElement->RemovePropertyL(_L8("Property"));
    HBufC8* ElementBuf=pElement->AsUtf8L(); 
    TL(retval == KErrNone);
    //False
    retval=pElement->RemovePropertyL(_L8("NoProperty"));
    TL(retval == KErrNotFound);
    TL(*ElementBuf == KText);
    CleanupStack::PopAndDestroy(pElement);  
    delete ElementBuf;          
    ElementBuf=NULL;
    Teardown();
    return KErrNone;
    }
    /*
TInt CsenutilsTestCases::SenTransportProperties_CloneLL()
    {
    _LIT8(KCloneText,"<Properties><Property>value</Property><Property1>value</Property1></Properties>");
    TInt okRnotok;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetPropertyL(_L8("Property"),_L8("value"));
    pElement->SetPropertyL(_L8("Property1"),_L8("value"));
    MSenProperties* pCloneElement=pElement->Clone(okRnotok);
    CleanupStack::PushL(pCloneElement);
    HBufC8* CloneElementBuf=pCloneElement->AsUtf8L();
    EUNIT_ASSERT_EQUALS(*CloneElementBuf,KCloneText);
    EUNIT_ASSERT_EQUALS(okRnotok,KErrNone);
    delete CloneElementBuf;             
    CleanupStack::PopAndDestroy(pCloneElement);
    CleanupStack::PopAndDestroy(pElement);
    }
*/
TInt CsenutilsTestCases::SenTransportProperties_HeartbeatLL( CStifItemParser& aItem )
{
    SetupL();
    TInt Delta;;
    TInt Delta1;
    aItem.GetNextInt( Delta );
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetHeartbeatL(Delta);
    TInt retval=pElement->HeartbeatL(Delta1);
    TL(retval == KErrNone);
    TL(Delta1 == Delta);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenTransportProperties_SetHeartbeatLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><Heartbeat>10</Heartbeat></Properties>");
    TInt Delta=10;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    TInt retval=pElement->SetHeartbeatL(Delta);
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(retval == KErrNone);
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);  
    delete ElementBuf;
    Teardown();
    return KErrNone;
    }
        
TInt CsenutilsTestCases::SenTransportProperties_IapIdLL(CStifItemParser& aItem )
{
    SetupL();
    TUint32 Delta;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetIapIdL(101);
    TInt retval=pElement->IapIdL(Delta);
    TL(retval == KErrNone);
    TL(Delta == 101);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenTransportProperties_SetIapIdLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><IapId>101</IapId></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetIapIdL(101);
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenTransportProperties_ProxyPortLL(CStifItemParser& aItem )
{
    SetupL();
    TInt PortNum;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetProxyPortL(8080);
    TInt retval=pElement->ProxyPortL(PortNum);
    TL(retval == KErrNone);
    TL(PortNum == 8080);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenTransportProperties_SetProxyPortLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><ProxyPort>8080</ProxyPort></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetProxyPortL(8080);
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenTransportProperties_ProxyHostLL(CStifItemParser& aItem )
{
    SetupL();
    TPtrC8 host;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetProxyHostL(_L8("tcp.mws.mobile.msn-ppe.com"));
    TInt retval=pElement->ProxyHostL(host);
    TL(retval == KErrNone);
    TL(host == _L8("tcp.mws.mobile.msn-ppe.com"));
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenTransportProperties_SetProxyHostLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><ProxyHost>tcp.mws.mobile.msn-ppe.com</ProxyHost></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetProxyHostL(_L8("tcp.mws.mobile.msn-ppe.com"));
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    }   
        
TInt CsenutilsTestCases::SenTransportProperties_ProxyUsageLL(CStifItemParser& aItem )
{
    SetupL();
    TBool Usage;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetProxyUsageL(TRUE);
    TInt retval=pElement->ProxyUsageL(Usage);
    TL(retval == KErrNone);
    TL(Usage == TRUE);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenTransportProperties_SetProxyUsageLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><ProxyUsage>true</ProxyUsage></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetProxyUsageL(TRUE);
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenTransportProperties_SecureDialogLL(CStifItemParser& aItem )
{
    SetupL();
    TBool Security;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetSecureDialogL(TRUE);
    TInt retval=pElement->SecureDialogL(Security);
    TL(retval == KErrNone);
    TL(Security == TRUE);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenTransportProperties_SetSecureDialogLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><SecureDialog>true</SecureDialog></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetSecureDialogL(TRUE);
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    }
        
TInt CsenutilsTestCases::SenTransportProperties_UserAgentLL(CStifItemParser& aItem )
{
    SetupL();
    TPtrC8 UserAgent;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetUserAgentL(_L8("TestAgent"));
    TInt retval=pElement->UserAgentL(UserAgent);
    TL(retval == KErrNone);
    TL(UserAgent ==_L8("TestAgent"));
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenTransportProperties_SetUserAgentLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><User-Agent>TestAgent</User-Agent></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetUserAgentL(_L8("TestAgent"));
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    }
        
TInt CsenutilsTestCases::SenTransportProperties_DeviceIDLL(CStifItemParser& aItem )
{
    SetupL();
    TPtrC8 DeviceID;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetDeviceIDL(_L8("TestDevice"));
    TInt retval=pElement->DeviceIDL(DeviceID);
    TL(retval == KErrNone);
    TL(DeviceID == _L8("TestDevice"));
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CsenutilsTestCases::SenTransportProperties_SetDeviceIDLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><DeviceID>TestDevice</DeviceID></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetDeviceIDL(_L8("TestDevice"));
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
}
        
TInt CsenutilsTestCases::SenTransportProperties_SoapActionLL(CStifItemParser& aItem )
{
    SetupL();
    TPtrC8 SoapURI;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetSoapActionL(_L8("URI-referenceRFC2396"));
    TInt retval=pElement->SoapActionL(SoapURI);
    TL(retval == KErrNone);
    TL(SoapURI == _L8("URI-referenceRFC2396"));
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
}
        
TInt CsenutilsTestCases::SenTransportProperties_SetSoapActionLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><SOAPAction>URI-referenceRFC2396</SOAPAction></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetSoapActionL(_L8("URI-referenceRFC2396"));
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
}

TInt CsenutilsTestCases::SenTransportProperties_DownloadFolderLL(CStifItemParser& aItem )
{
    SetupL();
    TPtrC8 Download;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetDownloadFolderL(_L8("Download"));
    TInt retval=pElement->DownloadFolderL(Download);
    TL(retval == KErrNone);
    TL(Download ==_L8("Download"));
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
}
        
TInt CsenutilsTestCases::SenTransportProperties_SetDownloadFolderLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><DownloadFolder>Download</DownloadFolder></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetDownloadFolderL(_L8("Download"));
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf==KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
}
        
   
TInt CsenutilsTestCases::SenTransportProperties_SetFileAttachmentLL(CStifItemParser& aItem )
{
    SetupL();
    _LIT8(KElementText,"<Properties><FileAttachments>&lt;Attachments&gt;&lt;Attachment0 cid=&quot;test:cid&quot;&gt;c:\\test.file.txt&lt;/Attachment0&gt;&lt;/Attachments&gt;</FileAttachments></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetFileAttachmentL(_L8("test:cid"),_L8("c:\\test.file.txt"));
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf==KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
}



TInt CsenutilsTestCases::SenTransportProperties_MwsNamespaceLL(CStifItemParser& aItem )
{
    SetupL();
    
    TPtrC8 NameSpace;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetMwsNamespaceL(_L8("Mwsnsuri"));
    TInt retval=pElement->MwsNamespaceL(NameSpace);
    TL(retval == KErrNone);
    TL(NameSpace == _L8("Mwsnsuri"));
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    
}
        
TInt CsenutilsTestCases::SenTransportProperties_SetMwsNamespaceLL(CStifItemParser& aItem )
{
    SetupL();

    
    _LIT8(KElementText,"<Properties><MwsNamespace>Mwsnsuri</MwsNamespace></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetMwsNamespaceL(_L8("Mwsnsuri"));
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    

}


TInt CsenutilsTestCases::SenTransportProperties_MessageIdLL(CStifItemParser& aItem )
{
    SetupL();

    TPtrC8 MsgId;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetMessageIdL(_L8("test-message"));
    TInt retval=pElement->MessageIdL(MsgId);
    TL(retval == KErrNone);
    TL(MsgId == _L8("test-message"));
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenTransportProperties_SetMessageIdLL(CStifItemParser& aItem )
{
    SetupL();

    _LIT8(KElementText,"<Properties><MessageID>test-message</MessageID></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetMessageIdL(_L8("test-message"));
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenTransportProperties_OnewayMessageOnOffLL(CStifItemParser& aItem )
{
    SetupL();

    TBool OnRoff;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetOnewayMessageOnOffL(TRUE);
    TInt retval=pElement->OnewayMessageOnOffL(OnRoff);
    TL(retval == KErrNone);
    TL(OnRoff == TRUE);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenTransportProperties_SetOnewayMessageOnOffLL( CStifItemParser& aItem)
{

    SetupL();

    _LIT8(KElementText,"<Properties><OneWayMessage>true</OneWayMessage></Properties>");
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetOnewayMessageOnOffL(TRUE);
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenTransportProperties_SetMaxTimeToLiveLL(CStifItemParser& aItem )
{
    SetupL();

    TInt PropValue;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetMaxTimeToLiveL(10);
    //true case
    TInt retval=pElement->MaxTimeToLiveL(PropValue);
    TL(retval == KErrNone);
    TL(PropValue == 10);
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenTransportProperties_MaxTimeToLiveLL(CStifItemParser& aItem )
{
    //TTestResult Result;
    //SenTransportProperties_SetMaxTimeToLiveLL( Result);
    SenTransportProperties_SetMaxTimeToLiveLL( aItem);
}

TInt CsenutilsTestCases::SenTransportProperties_SetMinTimeToLiveLL(CStifItemParser& aItem )
{
    SetupL();

    TInt PropValue;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetMinTimeToLiveL(10);
    //true case
    TInt retval=pElement->MinTimeToLiveL(PropValue);
    TL(retval == KErrNone);
    TL(PropValue == 10);
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenTransportProperties_SetReaderL(CStifItemParser& aItem )
{
    SetupL();

    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetReader(*iXmlReader);
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    
}


TInt CsenutilsTestCases::SenTransportProperties_PropertiesClassTypeL(CStifItemParser& aItem )
{
    SetupL();
    MSenProperties::TSenPropertiesClassType var = MSenProperties::ESenTransportProperties;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    TL(var == pElement->PropertiesClassType());
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenTransportProperties_WriteToLL(CStifItemParser& aItem )
{
    SetupL();

    TBuf8<50> StreamBuf;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->AddElementL(_L8("webservices"));
    RWriteStream& ElemntStream=pElement->ContentWriteStreamL();
    
    CSenTransportProperties* pElement1= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement1);
    pElement1->WriteToL(ElemntStream);
    
    CleanupStack::PopAndDestroy(pElement1);
    CleanupStack::PopAndDestroy(pElement);
    
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenTransportProperties_ReadFromLL(CStifItemParser& aItem )
{
    SetupL();
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->ReadFromL(_L8(""));
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenTransportProperties_CloneL(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    TInt var;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    CSenTransportProperties* pClone = NULL;
    pClone = (CSenTransportProperties*)pElement->Clone(var); 
    if(pClone != NULL)
        Flag = 1;
    if(!(Flag)) return KErrArgument;
   
    delete pClone;
    CleanupStack::PopAndDestroy(pElement);
    
    Teardown();
    return KErrNone;    
    
}

TInt CsenutilsTestCases::SenTransportProperties_IsSafeToCastL(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag, retVal;
    MSenProperties::TSenPropertiesClassType pVar = MSenProperties::ESenTransportProperties;
    
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    
    retVal = pElement->IsSafeToCast(pVar); 
    if(retVal != EFalse)
        Flag = 1;
    else 
        Flag = 0; 

    MSenProperties::TSenPropertiesClassType xVar = MSenProperties::ESenXmlProperties;
    retVal = pElement->IsSafeToCast(xVar); 
    if(retVal != EFalse)
        Flag = 1;
    else 
        return KErrArgument; 
    
    CleanupStack::PopAndDestroy(pElement);
    
    Teardown();
    return KErrNone;    
    
}


TInt CsenutilsTestCases::SenTransportProperties_CloneLL(CStifItemParser& aItem )
{
    SetupL();
    TBool Flag;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    CSenTransportProperties* pClone = NULL;
    pClone = (CSenTransportProperties*)pElement->CloneL(); 
    if(pClone != NULL)
        Flag = 1;
    if(!(Flag)) return KErrArgument;
   
    delete pClone;
    CleanupStack::PopAndDestroy(pElement);
    
    Teardown();
    return KErrNone;    
    
}

TInt CsenutilsTestCases::SenTransportProperties_ApplyBindingLL(CStifItemParser& aItem )
{
    SetupL();
    TSOAPVersion var = ESOAP11;
    CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->ApplyBindingL(var); 
    CleanupStack::PopAndDestroy(pElement);
    
    Teardown();
    return KErrNone;    
    
}


TInt CsenutilsTestCases::SenTransportProperties_MinTimeToLiveLL(CStifItemParser& aItem )
{
    //TTestResult Result;
    //SenTransportProperties_SetMinTimeToLiveLL(Result);
    SenTransportProperties_SetMinTimeToLiveLL(aItem);
}

TInt CsenutilsTestCases::SenHttpTransportProperties_NewLL(CStifItemParser& aItem)
{
    SetupL();

    TBool Flag;
    CSenHttpTransportProperties *pProperties = CSenHttpTransportProperties::NewL();
    CleanupStack::PushL(pProperties );
    if(pProperties)
        Flag=1;
    else
        Flag=0;
    if(! Flag) return KErrArgument;
    CleanupStack::PopAndDestroy(pProperties );
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenHttpTransportProperties_NewLCL(CStifItemParser& aItem)
{
    SetupL();

    TBool Flag;
    CSenHttpTransportProperties *pProperties = CSenHttpTransportProperties::NewLC();
    if(pProperties)
        Flag=1;
    else
        Flag=0;
    if(! Flag) return KErrArgument;
    CleanupStack::PopAndDestroy(pProperties );
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenHttpTransportProperties_NewL_1L(CStifItemParser& aItem)
{
    SetupL();

    TBool Flag;
    _LIT8(KString,"<parent><element1/><element2/><element3/>parent content</parent>");
    CSenXmlReader* pParser = CSenXmlReader::NewL();
    CleanupStack::PushL(pParser);
    CSenHttpTransportProperties *pProperties=CSenHttpTransportProperties::NewL(KString,*pParser);
    CleanupStack::PushL(pProperties);
    if(pProperties)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pProperties);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenHttpTransportProperties_NewLC_1L(CStifItemParser& aItem)
{
    SetupL();

    TBool Flag;
    _LIT8(KString,"<parent><element1/><element2/><element3/>parent content</parent>");
    CSenXmlReader* pParser = CSenXmlReader::NewL();
    CleanupStack::PushL(pParser);
    CSenHttpTransportProperties *pProperties=CSenHttpTransportProperties::NewLC(KString,*pParser);
    if(pProperties)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pProperties);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;
    
}

TInt CsenutilsTestCases::SenHttpTransportProperties_NewL_2L(CStifItemParser& aItem)
{
    SetupL();

    
    TBool Flag;
    CSenBaseElement* pElement= CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    CSenHttpTransportProperties *pProperties=CSenHttpTransportProperties::NewL(*pElement);
    CleanupStack::PushL(pProperties);
    if(pProperties)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pProperties);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
        
}

TInt CsenutilsTestCases::SenHttpTransportProperties_NewLC_2L(CStifItemParser& aItem)
{
    SetupL();

    
    TBool Flag;
    CSenBaseElement* pElement= CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    CSenHttpTransportProperties *pProperties=CSenHttpTransportProperties::NewLC(*pElement);
    if(pProperties)
        Flag=1;
    else
        Flag=0;
    if(!( Flag )) return KErrArgument;
    CleanupStack::PopAndDestroy(pProperties);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    
}
