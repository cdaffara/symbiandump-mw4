/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:          ?Description
*
*/











// INCLUDE FILES
#include <StifTestModule.h>
#include <e32math.h>
#include <libc/assert.h>
#include <e32panic.h>
#include "senutilsbctest.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSenUtilsBCTest::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo CSenUtilsBCTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    SenUtils.cpp file and to SenUtils.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        
        ENTRY( "NewL - CSenSoapEnvelope", CSenUtilsBCTest::CSenSoapEnvelope_NewLL ),
        ENTRY( "SetBodyL - CSenSoapEnvelope ", CSenUtilsBCTest::CSenSoapEnvelope_SetBodyLL ),
        ENTRY( "BodyL - CSenSoapEnvelope ", CSenUtilsBCTest::CSenSoapEnvelope_BodyLL ),
        ENTRY( "HeaderL - CSenSoapEnvelope ", CSenUtilsBCTest::CSenSoapEnvelope_HeaderLL ),        
        ENTRY( "AddHeaderL - CSenSoapEnvelope", CSenUtilsBCTest::CSenSoapEnvelope_AddHeaderLL ),
        ENTRY( "BodyAsStringL - CSenSoapEnvelope ", CSenUtilsBCTest::CSenSoapEnvelope_BodyAsStringLL ),
        ENTRY( "DetachFaultL - CSenSoapEnvelope ", CSenUtilsBCTest::CSenSoapEnvelope_DetachFaultLL ),
        ENTRY( "FaultL - CSenSoapEnvelope ", CSenUtilsBCTest::CSenSoapEnvelope_FaultLL ),
        ENTRY( "SoapAction - CSenSoapEnvelope ", CSenUtilsBCTest::CSenSoapEnvelope_SoapActionL ),
 //       ENTRY( "SoapAction2 - CSenSoapEnvelope ", CSenUtilsBCTest::CSenSoapEnvelope_SoapAction2L ),
        ENTRY( "HasHeader - CSenSoapEnvelope ", CSenUtilsBCTest::CSenSoapEnvelope_HasHeaderL ),
        ENTRY( "SoapVersion - CSenSoapEnvelope ", CSenUtilsBCTest::CSenSoapEnvelope_SoapVersionL ),
        ENTRY( "HasBody - CSenSoapEnvelope ", CSenUtilsBCTest::CSenSoapEnvelope_HasBodyL ),
        ENTRY( "IsFault - CSenSoapEnvelope ", CSenUtilsBCTest::CSenSoapEnvelope_IsFaultL ),
        ENTRY( "ParseL - CSenSoapEnvelope ", CSenUtilsBCTest::CSenSoapEnvelope_ParseLL ),
        
        ENTRY( "NewL - CSenSoapFault ", CSenUtilsBCTest::CSenSoapFault_NewLL ),
        ENTRY( "NewL - CSenSoapFault ", CSenUtilsBCTest::CSenSoapFault_NewL_1L ),
        ENTRY( "FaultCode - CSenSoapFault ", CSenUtilsBCTest::CSenSoapFault_FaultCodeL ),
        ENTRY( "FaultSubcode - CSenSoapFault ", CSenUtilsBCTest::CSenSoapFault_FaultSubcodeL ),
        
        ENTRY( "FaultString - CSenSoapFault ", CSenUtilsBCTest::CSenSoapFault_FaultStringL ),
        ENTRY( "FaultActor - CSenSoapFault ", CSenUtilsBCTest::CSenSoapFault_FaultActorL ),
        ENTRY( "Detail - CSenSoapFault ", CSenUtilsBCTest::CSenSoapFault_DetailL ),
        ENTRY( "NewL - CSenSoapMessage ", CSenUtilsBCTest::CSenSoapMessage_NewLL ),
        ENTRY( "SetSecurityHeaderL - CSenSoapMessage ", CSenUtilsBCTest::CSenSoapMessage_SetSecurityHeaderLL ),
        ENTRY( "AddSecurityTokenL - CSenSoapMessage ", CSenUtilsBCTest::CSenSoapMessage_AddSecurityTokenLL ),
        ENTRY( "NewL - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_NewLL ),
        ENTRY( "ParseL - CSenSoapMessage ", CSenUtilsBCTest::CSenSoapMessage_ParseLL ),
        
        ENTRY( "NewLC - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_NewLCL ),
        ENTRY( "NewL - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_NewL_1L ),
        ENTRY( "NewLC - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_NewLC_1L ),
        ENTRY( "NewL - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_NewL_2L ),
        ENTRY( "NewLC - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_NewLC_2L ),
        ENTRY( "BinarySecurityToken - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_BinarySecurityTokenL_L ),
        ENTRY( "BinarySecurityToken1 - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_BinarySecurityTokenL_1L ),
//        ENTRY( "UsernameTokenL - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_UsernameTokenLL ),
        ENTRY( "TimestampL - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_TimestampL_L ),
        ENTRY( "TimestampL - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_TimestampL_1L ),        
//        ENTRY( "UsernameTokenL - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_UsernameTokenL_1L ),
//        ENTRY( "UsernameTokenL - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_UsernameTokenL_2L ),
//        ENTRY( "UsernameTokenL - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_UsernameTokenL_3L ),
//        ENTRY( "UsernameTokenL - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_UsernameTokenL_4L ),
        ENTRY( "XmlNs - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_XmlNsL ),
        ENTRY( "XmlNsPrefix - CSenWsSecurityHeader ", CSenUtilsBCTest::CSenWsSecurityHeader_XmlNsPrefixL ),
        ENTRY( "FromXmlDateTimeL - SenDateUtils ", CSenUtilsBCTest::SenDateUtils_FromXmlDateTimeLL ),
        ENTRY( "ToXmlDateTimeUtf8L - SenDateUtils ", CSenUtilsBCTest::SenDateUtils_ToXmlDateTimeUtf8LL ),
        ENTRY( "ToXmlDateTimeUtf82L - SenDateUtils ", CSenUtilsBCTest::SenDateUtils_ToXmlDateTimeUtf82LL ),
        
        ENTRY( "FileAttachmentL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_FileAttachmentLL ),
        ENTRY( "NewL - CSenXmlProperties", CSenUtilsBCTest::SenXmlProperties_NewLL ),
        ENTRY( "NewLC - CSenXmlProperties", CSenUtilsBCTest::SenXmlProperties_NewLCL ),
        ENTRY( "NewL - CSenXmlProperties", CSenUtilsBCTest::SenXmlProperties_NewL_1L ),
        ENTRY( "NewLC - CSenXmlProperties", CSenUtilsBCTest::SenXmlProperties_NewLC_1L ),
        ENTRY( "NewL - CSenXmlProperties", CSenUtilsBCTest::SenXmlProperties_NewL_2L ),
        ENTRY( "NewLC - CSenXmlProperties", CSenUtilsBCTest::SenXmlProperties_NewLC_2L ),
        ENTRY( "NewL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_NewLL ),
        ENTRY( "NewLC - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_NewLCL ),
        ENTRY( "NewL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_NewL_1L ),
        ENTRY( "NewLC - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_NewLC_1L ),
        ENTRY( "NewL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_NewL_2L ),
        ENTRY( "NewLC - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_NewLC_2L ),
        ENTRY( "AsUtf8L - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_AsUtf8LL ),
        ENTRY( "SetPropertyL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetPropertyLL ),
        ENTRY( "PropertyL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_PropertyLL ),
        ENTRY( "SetIntPropertyL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetIntPropertyLL ),
        ENTRY( "IntPropertyL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_IntPropertyLL ),
        ENTRY( "SetBoolPropertyL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetBoolPropertyLL ),
        ENTRY( "BoolPropertyL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_BoolPropertyLL ),
        ENTRY( "SetOmittedL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetOmittedLL ),
        ENTRY( "RemovePropertyL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_RemovePropertyLL ),
        ENTRY( "HeartbeatL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_HeartbeatLL ),
        ENTRY( "SetHeartbeatL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetHeartbeatLL ),
        ENTRY( "IapIdL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_IapIdLL ),
        ENTRY( "SetIapIdL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetIapIdLL ),
        ENTRY( "ProxyPortL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_ProxyPortLL ),
        ENTRY( "SetProxyPortL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetProxyPortLL ),
        ENTRY( "ProxyHostL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_ProxyHostLL ),
        ENTRY( "SetProxyHostL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetProxyHostLL ),
        ENTRY( "ProxyUsageL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_ProxyUsageLL ),
        ENTRY( "SetProxyUsageL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetProxyUsageLL ),
        ENTRY( "SecureDialogL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SecureDialogLL ),
        ENTRY( "SetSecureDialogL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetSecureDialogLL ),
        ENTRY( "UserAgentL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_UserAgentLL ),
        ENTRY( "SetUserAgentL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetUserAgentLL ),
        ENTRY( "DeviceIDL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_DeviceIDLL ),
        ENTRY( "SetDeviceIDL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetDeviceIDLL ),
        ENTRY( "SoapActionL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SoapActionLL ),
        ENTRY( "SetSoapActionL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetSoapActionLL ),
        ENTRY( "DownloadFolderL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_DownloadFolderLL ),
        ENTRY( "SetDownloadFolderL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetDownloadFolderLL ),
        ENTRY( "SetFileAttachmentL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetFileAttachmentLL ),
        ENTRY( "MwsNamespaceL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_MwsNamespaceLL ),
        ENTRY( "SetMwsNamespaceL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetMwsNamespaceLL ),
        ENTRY( "MessageIdL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_MessageIdLL ),
        ENTRY( "SetMessageIdL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetMessageIdLL ),
        ENTRY( "OnewayMessageOnOffL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_OnewayMessageOnOffLL ),
        ENTRY( "SetOnewayMessageOnOffL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetOnewayMessageOnOffLL ),
        ENTRY( "SetMaxTimeToLiveL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetMaxTimeToLiveLL ),
        ENTRY( "MaxTimeToLiveL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_MaxTimeToLiveLL ),
        ENTRY( "SetMaxTimeToLiveL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetMaxTimeToLiveLL),
        ENTRY( "MinTimeToLiveL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_MinTimeToLiveLL ),
        ENTRY( "SetMinTimeToLiveL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetMinTimeToLiveLL ),
        ENTRY( "SetReaderL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_SetReaderL ),
        ENTRY( "PropertiesClassTypeL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_PropertiesClassTypeL ),
        ENTRY( "WriteToLL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_WriteToLL ),
        ENTRY( "ReadFromLL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_ReadFromLL ),
        ENTRY( "CloneL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_CloneL ),
        ENTRY( "IsSafeToCastL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_IsSafeToCastL ),
        ENTRY( "CloneLL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_CloneLL ),
        ENTRY( "ApplyBindingLL - CSenTransportProperties", CSenUtilsBCTest::SenTransportProperties_ApplyBindingLL ),
        
        ENTRY( "NewL - CSenHttpTransportProperties", CSenUtilsBCTest::SenHttpTransportProperties_NewLL ),
        ENTRY( "NewLC - CSenHttpTransportProperties", CSenUtilsBCTest::SenHttpTransportProperties_NewLCL ),
        ENTRY( "NewL - CSenHttpTransportProperties", CSenUtilsBCTest::SenHttpTransportProperties_NewL_1L ),
        ENTRY( "NewLC - CSenHttpTransportProperties", CSenUtilsBCTest::SenHttpTransportProperties_NewLC_1L ),
        ENTRY( "NewL - CSenHttpTransportProperties", CSenUtilsBCTest::SenHttpTransportProperties_NewL_2L ),
        ENTRY( "NewLC - CSenHttpTransportProperties", CSenUtilsBCTest::SenHttpTransportProperties_NewLC_2L),
        //ENTRY( "NewL - CSenVtcpTransportProperties", CSenUtilsBCTest::SenVtcpTransportProperties_NewLL ),
        //ENTRY( "NewLC - CSenVtcpTransportProperties", CSenUtilsBCTest::SenVtcpTransportProperties_NewLCL ),
        //ENTRY( "NewL - CSenVtcpTransportProperties", CSenUtilsBCTest::SenVtcpTransportProperties_NewL_1L ),
        //ENTRY( "NewLC - CSenVtcpTransportProperties", CSenUtilsBCTest::SenVtcpTransportProperties_NewLC_1L ),
        //ENTRY( "NewL - CSenVtcpTransportProperties", CSenUtilsBCTest::SenVtcpTransportProperties_NewL_2L ),
        //ENTRY( "NewLC - CSenVtcpTransportProperties", CSenUtilsBCTest::SenVtcpTransportProperties_NewLC_2L ),
        //ENTRY( "OnewayMessageOnOffL - CSenVtcpTransportProperties", CSenUtilsBCTest::SenVtcpTransportProperties_OnewayMessageOnOffLL ),
        //ENTRY( "SetOnewayMessageOnOffL - CSenVtcpTransportProperties", CSenUtilsBCTest::SenVtcpTransportProperties_SetOnewayMessageOnOffLL ),
        //ENTRY( "SetMaxTimeToLiveL - CSenVtcpTransportProperties", CSenUtilsBCTest::SenVtcpTransportProperties_SetMaxTimeToLiveLL ),
        //ENTRY( "MaxTimeToLiveL - CSenVtcpTransportProperties", CSenUtilsBCTest::SenVtcpTransportProperties_MaxTimeToLiveLL ),
        //ENTRY( "MinTimeToLiveL - CSenVtcpTransportProperties", CSenUtilsBCTest::SenVtcpTransportProperties_MinTimeToLiveLL ),
        //ENTRY( "SetMinTimeToLiveL - CSenVtcpTransportProperties", CSenUtilsBCTest::SenVtcpTransportProperties_SetMinTimeToLiveLL ),*/
        

        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", CSenUtilsBCTest::LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( CSenUtilsBCTest::PrintTest, ETrue, 1, 3 ),
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;

    }

void CSenUtilsBCTest::SetupL(  ){
    if ( iXmlReader ){
        delete iXmlReader;
        iXmlReader = NULL;
    }
    iXmlReader = CSenXmlReader::NewL();
} 

void CSenUtilsBCTest::Teardown(  ){
    if ( iXmlReader){
        delete iXmlReader;
        iXmlReader = NULL;
    }
}


TPtr16 CSenUtilsBCTest::ConvertToPtr16LC(CSenBaseFragment &fragment){
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

TPtr16 CSenUtilsBCTest::ConvertToPtr16LC(MSenElement &element){
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

TInt CSenUtilsBCTest::CSenSoapEnvelope_NewLL( TTestResult& aResult )
    {
    SetupL();
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
    Teardown();
    return KErrNone;
    }

TInt CSenUtilsBCTest::CSenSoapEnvelope_SetBodyLL( TTestResult& aResult )
    {
    SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS(env, (CSenSoapEnvelope*)NULL); */
    TL(env != (CSenSoapEnvelope*)NULL);
    _LIT8(KBody,"ComarchBody");
    TPtrC8 result = env->SetBodyL(KBody);
    
//    _LIT16(KRef2, "ComarchBody");
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

TInt CSenUtilsBCTest::CSenSoapEnvelope_BodyLL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapEnvelope_HeaderLL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapEnvelope_AddHeaderLL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapEnvelope_BodyAsStringLL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapEnvelope_DetachFaultLL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapEnvelope_FaultLL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapEnvelope_SoapActionL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapEnvelope_SoapAction2L( TTestResult& aResult )
    {
    SetupL();
    CSenSoapEnvelope* env = CSenSoapEnvelope::NewL();
    /*EUNIT_ASSERT_NOT_EQUALS*/TL(env != (CSenSoapEnvelope*)NULL);

    _LIT8(KRef, "");
    _LIT8(KRef2, "\"URI-referenceRFC2396\"");

    TL(env->SoapAction2() == KRef);
    env->SetSoapActionL(_L8("URI-referenceRFC2396"));
    TL(env->SoapAction2() == KRef2);
    env->SetSoapActionL(KRef2);
    TL(env->SoapAction2() == KRef2);
    

    /*EUNIT_ASSERT_NO_LEAVE*/
    __ASSERT_ALWAYS_NO_LEAVE(delete env);
    env = NULL;
    Teardown();
    return KErrNone;
    }

TInt CSenUtilsBCTest::CSenSoapEnvelope_HasHeaderL( TTestResult& aResult )
    {
    SetupL();
    
//    _LIT8(KHeaderElementName,   "HeaderElement");
//    _LIT8(KHeaderContent,       "HeaderContent");

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
   
TInt CSenUtilsBCTest::CSenSoapEnvelope_SoapVersionL( TTestResult& aResult )
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
    
TInt CSenUtilsBCTest::CSenSoapEnvelope_HasBodyL( TTestResult& aResult )
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
    
TInt CSenUtilsBCTest::CSenSoapEnvelope_IsFaultL( TTestResult& aResult )
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
	
TInt CSenUtilsBCTest::CSenSoapEnvelope_ParseLL( TTestResult& aResult )
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
/*    _LIT8(KBodyAsString,     "\
<S:Body xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:sa=\"urn:liberty:sa:2004-04\">\
<sa:SASLRequest xmlns:sa=\"urn:liberty:sa:2004-04\" mechanism=\"ANONYMOUS PLAIN CRAM-MD5\" authzID=\"testuser1\"/>\
</S:Body>");*/

    //_LIT8(KEmptyBodyAsString,       "<S:Body/>");

  //  _LIT8(KEmptyBodyWithNsAsString, "<S:Body xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
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
	


TInt CSenUtilsBCTest::CSenSoapFault_NewLL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapFault_NewL_1L( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapFault_FaultCodeL( TTestResult& aResult )
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
    
TInt CSenUtilsBCTest::CSenSoapFault_FaultSubcodeL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapFault_FaultStringL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapFault_FaultActorL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapFault_DetailL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapMessage_NewLL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapMessage_SetSecurityHeaderLL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapMessage_AddSecurityTokenLL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenSoapMessage_ParseLL( TTestResult& aResult )
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
/*    _LIT8(KBodyAsString,     "\
<S:Body xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:sa=\"urn:liberty:sa:2004-04\">\
<sa:SASLRequest xmlns:sa=\"urn:liberty:sa:2004-04\" mechanism=\"ANONYMOUS PLAIN CRAM-MD5\" authzID=\"testuser1\"/>\
</S:Body>");
*/
  //  _LIT8(KEmptyBodyAsString,       "<S:Body/>");

    //_LIT8(KEmptyBodyWithNsAsString, "<S:Body xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"/>");
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


TInt CSenUtilsBCTest::CSenWsSecurityHeader_NewLL( TTestResult& aResult )
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
    
 TInt CSenUtilsBCTest::CSenWsSecurityHeader_NewLCL( TTestResult& aResult )
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

  
TInt CSenUtilsBCTest::CSenWsSecurityHeader_NewLC_1L( TTestResult& aResult )
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
    
TInt CSenUtilsBCTest::CSenWsSecurityHeader_NewL_2L( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenWsSecurityHeader_NewLC_2L( TTestResult& aResult )
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
    
    
TInt CSenUtilsBCTest::CSenWsSecurityHeader_BinarySecurityTokenL_L( TTestResult& aResult )
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
    
TInt CSenUtilsBCTest::CSenWsSecurityHeader_BinarySecurityTokenL_1L( TTestResult& aResult )
    {
    SetupL();
//testing NULL
    HBufC8* token=NULL;
    TL(CSenWsSecurityHeader::BinarySecurityTokenL(KNullDesC8, KNullDesC8, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef, "<wsse:BinarySecurityToken wsu:Id=\"token\" ValueType=\"\" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\"></wsse:BinarySecurityToken>");
    TPtrC8 ptrRef(KRef);
    TPtr8 ptrToken = token->Des();
    TL(ptrToken == ptrRef);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;

    
//testing normal    
    TL(CSenWsSecurityHeader::BinarySecurityTokenL(KText, KText, token)==KErrNone);
    TL(token != (HBufC8*)NULL);
    //-----checking xml
    _LIT8(KRef2, "<wsse:BinarySecurityToken wsu:Id=\"token\" ValueType=\"text\" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">text</wsse:BinarySecurityToken>");
    TPtrC8 ptrRef2(KRef2);
    ptrToken = token->Des();
    TL(ptrToken == ptrRef2);
    __ASSERT_ALWAYS_NO_LEAVE(delete token);
    token = NULL;
    Teardown();
    return KErrNone;
    }
    
TInt CSenUtilsBCTest::CSenWsSecurityHeader_TimestampL_L( TTestResult& aResult )
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
    
TInt CSenUtilsBCTest::CSenWsSecurityHeader_TimestampL_1L( TTestResult& aResult )
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




    
TInt CSenUtilsBCTest::CSenWsSecurityHeader_UsernameTokenL_2L( TTestResult& aResult )
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
    
TInt CSenUtilsBCTest::CSenWsSecurityHeader_UsernameTokenL_3L( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenWsSecurityHeader_UsernameTokenL_4L( TTestResult& aResult )
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
    

TInt CSenUtilsBCTest::CSenWsSecurityHeader_NewL_1L( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenWsSecurityHeader_UsernameTokenLL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenWsSecurityHeader_UsernameTokenL_1L( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenWsSecurityHeader_XmlNsL( TTestResult& aResult )
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

TInt CSenUtilsBCTest::CSenWsSecurityHeader_XmlNsPrefixL( TTestResult& aResult )
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


TInt CSenUtilsBCTest::SenDateUtils_FromXmlDateTimeLL( TTestResult& aResult )
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
//    _LIT8(KIn15, "xxxx-02-28T13:20:65Z");
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

TInt CSenUtilsBCTest::SenDateUtils_ToXmlDateTimeUtf8LL( TTestResult& aResult )
    {
    SetupL();
    //KXmlDateTimeMaxLength = 25
    _LIT(KDate, "23:34.56");
    _LIT8(KRef, "0000-01-01T23:34:56Z");
    TTime ttime;
    TInt res(0);
    HBufC8* writeBuf;
    TInt error  = ttime.Parse(KDate);
    
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
		
		TRAP(res,
					//SetupL();
    			//CleanupStack::PushL(iXmlReader);
    	    //KErrOverflow
    	    writeBuf=HBufC8::NewLC(SenDateUtils::KXmlDateTimeMaxLength - 20);
    	    TPtr8 ptrResult = writeBuf->Des();
    	    SenDateUtils::ToXmlDateTimeUtf8L(ptrResult, ttime);
    	    CleanupStack::PopAndDestroy();//writeBuf
    	    //CleanupStack::Pop();//iXmlReader
    	    //Teardown();
    	    );
    if(res != KErrOverflow) 
    	return res;
			   
    return KErrNone;
    }
    
TInt CSenUtilsBCTest::SenDateUtils_ToXmlDateTimeUtf82LL( TTestResult& aResult )
    {
    SetupL();

    //KXmlDateTimeMaxLength = 25
    _LIT(KDate, "23:34.56");
    _LIT8(KRef, "0000-01-01T23:34:56.000000Z");
    TTime ttime;
    TInt res(0);
    HBufC8* writeBuf;
    TInt error  = ttime.Parse(KDate);
    
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
		
		TRAP(res,
    	    //KErrOverflow
    	    writeBuf=HBufC8::NewLC(SenDateUtils::KXmlDateTimeMaxLength - 20);
    	    TPtr8 ptrResult = writeBuf->Des();
    	    SenDateUtils::ToXmlDateTimeUtf82L(ptrResult, ttime);
    	    CleanupStack::PopAndDestroy();//writeBuf
    	    );    				
    if(res != KErrOverflow) 
    	return res;
    	

    
    return KErrNone;
    }
    
TInt CSenUtilsBCTest::SenTransportProperties_FileAttachmentLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenXmlProperties_NewLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenXmlProperties_NewLCL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenXmlProperties_NewL_1L(TTestResult& aResult )
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
	
TInt CSenUtilsBCTest::SenXmlProperties_NewLC_1L(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenXmlProperties_NewL_2L(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenXmlProperties_NewLC_2L(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_NewLL(TTestResult& aResult )
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
	
TInt CSenUtilsBCTest::SenTransportProperties_NewLCL(TTestResult& aResult )
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
	
TInt CSenUtilsBCTest::SenTransportProperties_NewL_1L(TTestResult& aResult )
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
	
TInt CSenUtilsBCTest::SenTransportProperties_NewLC_1L(TTestResult& aResult )
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
	
TInt CSenUtilsBCTest::SenTransportProperties_NewL_2L(TTestResult& aResult )
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
	
TInt CSenUtilsBCTest::SenTransportProperties_NewLC_2L(TTestResult& aResult )
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
	if(!( Flag )) 
		{
	  CleanupStack::PopAndDestroy(pXmlElement);
	  CleanupStack::PopAndDestroy(pElement);
	  return KErrArgument;
		}
  CleanupStack::PopAndDestroy(pXmlElement);
	CleanupStack::PopAndDestroy(pElement);
  Teardown();
  return KErrNone;
}
	

TInt CSenUtilsBCTest::SenTransportProperties_AsUtf8LL(TTestResult& aResult )
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
	        
TInt CSenUtilsBCTest::SenTransportProperties_SetPropertyLL(TTestResult& aResult )
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
TInt CSenUtilsBCTest::SenTransportProperties_PropertyLL(TTestResult& aResult )
{
    SetupL();
	//_LIT8(KValue,"Value");
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
TInt CSenUtilsBCTest::SenTransportProperties_SetIntPropertyLL(TTestResult& aResult )
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
TInt CSenUtilsBCTest::SenTransportProperties_IntPropertyLL(TTestResult& aResult )
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
TInt CSenUtilsBCTest::SenTransportProperties_SetBoolPropertyLL(TTestResult& aResult )
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
TInt CSenUtilsBCTest::SenTransportProperties_BoolPropertyLL(TTestResult& aResult )
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
TInt CSenUtilsBCTest::SenTransportProperties_SetOmittedLL(TTestResult& aResult )
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
TInt CSenUtilsBCTest::SenTransportProperties_RemovePropertyLL(TTestResult& aResult )
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
TInt CSenUtilsBCTest::SenTransportProperties_CloneLL()
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
TInt CSenUtilsBCTest::SenTransportProperties_HeartbeatLL(TTestResult& aResult )
{
    SetupL();
	TInt Delta=10;
	TInt Delta1;
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

TInt CSenUtilsBCTest::SenTransportProperties_SetHeartbeatLL(TTestResult& aResult )
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
        
TInt CSenUtilsBCTest::SenTransportProperties_IapIdLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_SetIapIdLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_ProxyPortLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_SetProxyPortLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_ProxyHostLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_SetProxyHostLL(TTestResult& aResult )
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
        
TInt CSenUtilsBCTest::SenTransportProperties_ProxyUsageLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_SetProxyUsageLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_SecureDialogLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_SetSecureDialogLL(TTestResult& aResult )
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
        
TInt CSenUtilsBCTest::SenTransportProperties_UserAgentLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_SetUserAgentLL(TTestResult& aResult )
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
        
TInt CSenUtilsBCTest::SenTransportProperties_DeviceIDLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_SetDeviceIDLL(TTestResult& aResult )
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
        
TInt CSenUtilsBCTest::SenTransportProperties_SoapActionLL(TTestResult& aResult )
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
        
TInt CSenUtilsBCTest::SenTransportProperties_SetSoapActionLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_DownloadFolderLL(TTestResult& aResult )
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
        
TInt CSenUtilsBCTest::SenTransportProperties_SetDownloadFolderLL(TTestResult& aResult )
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
        
   
TInt CSenUtilsBCTest::SenTransportProperties_SetFileAttachmentLL(TTestResult& aResult )
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



TInt CSenUtilsBCTest::SenTransportProperties_MwsNamespaceLL(TTestResult& aResult )
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
        
TInt CSenUtilsBCTest::SenTransportProperties_SetMwsNamespaceLL(TTestResult& aResult )
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


TInt CSenUtilsBCTest::SenTransportProperties_MessageIdLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_SetMessageIdLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_OnewayMessageOnOffLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_SetOnewayMessageOnOffLL( TTestResult& aResult)
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

TInt CSenUtilsBCTest::SenTransportProperties_SetMaxTimeToLiveLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_MaxTimeToLiveLL(TTestResult& aResult )
{
	TTestResult Result;
	return SenTransportProperties_SetMaxTimeToLiveLL( Result);
}

TInt CSenUtilsBCTest::SenTransportProperties_SetMinTimeToLiveLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_SetReaderL(TTestResult& aResult )
{
    SetupL();

	CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetReader(*iXmlReader);
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    
}


TInt CSenUtilsBCTest::SenTransportProperties_PropertiesClassTypeL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_WriteToLL(TTestResult& aResult )
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

TInt CSenUtilsBCTest::SenTransportProperties_ReadFromLL(TTestResult& aResult )
{
    SetupL();
	CSenTransportProperties* pElement= CSenTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->ReadFromL(_L8(""));
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    
}

TInt CSenUtilsBCTest::SenTransportProperties_CloneL(TTestResult& aResult )
{
	SetupL();
	TBool Flag = 0;
	TInt var;
	CSenTransportProperties* pElement= CSenTransportProperties::NewL();
	CleanupStack::PushL(pElement);
	CSenTransportProperties* pClone = NULL;
    pClone = (CSenTransportProperties*)pElement->Clone(var); 
    if(pClone != NULL)
    	Flag = 1;
    if(!(Flag)) 
    	{
  		delete pClone;
    	CleanupStack::PopAndDestroy(pElement);
     	return KErrArgument;
    	}
    delete pClone;
    CleanupStack::PopAndDestroy(pElement);
    
    Teardown();
    return KErrNone;    
    
}

TInt CSenUtilsBCTest::SenTransportProperties_IsSafeToCastL(TTestResult& aResult )
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
    	{
    		CleanupStack::PopAndDestroy(pElement);
   	    return KErrArgument; 
			}
    //TO resolve RVCT4.0 warning
    if (Flag);
    CleanupStack::PopAndDestroy(pElement);
    
    Teardown();
    return KErrNone;    
    
}


TInt CSenUtilsBCTest::SenTransportProperties_CloneLL(TTestResult& aResult )
{
	SetupL();
	TBool Flag = 0;
	CSenTransportProperties* pElement= CSenTransportProperties::NewL();
	CleanupStack::PushL(pElement);
	CSenTransportProperties* pClone = NULL;
  pClone = (CSenTransportProperties*)pElement->CloneL(); 
  if(pClone != NULL)
  	Flag = 1;
  if(!(Flag)) 
  	{
  	delete pClone;
    CleanupStack::PopAndDestroy(pElement);
  	return KErrArgument;
    } 
	delete pClone;
  CleanupStack::PopAndDestroy(pElement);
    
  Teardown();
  return KErrNone;    
    
}

TInt CSenUtilsBCTest::SenTransportProperties_ApplyBindingLL(TTestResult& aResult )
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


TInt CSenUtilsBCTest::SenTransportProperties_MinTimeToLiveLL(TTestResult& aResult )
{
	TTestResult Result;
	return SenTransportProperties_SetMinTimeToLiveLL(Result);
}

TInt CSenUtilsBCTest::SenHttpTransportProperties_NewLL(TTestResult& aResult)
{
    SetupL();

	TBool Flag;
	CSenHttpTransportProperties	*pProperties = CSenHttpTransportProperties::NewL();
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

TInt CSenUtilsBCTest::SenHttpTransportProperties_NewLCL(TTestResult& aResult)
{
    SetupL();

	TBool Flag;
	CSenHttpTransportProperties	*pProperties = CSenHttpTransportProperties::NewLC();
	if(pProperties)
		Flag=1;
	else
		Flag=0;
	if(! Flag) return KErrArgument;
	CleanupStack::PopAndDestroy(pProperties );
    Teardown();
    return KErrNone;
	
}

TInt CSenUtilsBCTest::SenHttpTransportProperties_NewL_1L(TTestResult& aResult)
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

TInt CSenUtilsBCTest::SenHttpTransportProperties_NewLC_1L(TTestResult& aResult)
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

TInt CSenUtilsBCTest::SenHttpTransportProperties_NewL_2L(TTestResult& aResult)
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

TInt CSenUtilsBCTest::SenHttpTransportProperties_NewLC_2L(TTestResult& aResult)
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
/*
TInt CSenUtilsBCTest::SenVtcpTransportProperties_NewLL(TTestResult& aResult)
{
    SetupL();

	TBool Flag;
	CSenVtcpTransportProperties	*pProperties = CSenVtcpTransportProperties::NewL();
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

TInt CSenUtilsBCTest::SenVtcpTransportProperties_NewLCL(TTestResult& aResult)
{
    SetupL();

	TBool Flag;
	CSenVtcpTransportProperties	*pProperties = CSenVtcpTransportProperties::NewLC();
	if(pProperties)
		Flag=1;
	else
		Flag=0;
	if(! Flag) return KErrArgument;
	CleanupStack::PopAndDestroy(pProperties );
    Teardown();
    return KErrNone;
	
}


TInt CSenUtilsBCTest::SenVtcpTransportProperties_NewL_1L(TTestResult& aResult)
{
    SetupL();

	TBool Flag;
	_LIT8(KString,"<parent><element1/><element2/><element3/>parent content plus some other content</parent>");
	CSenXmlReader* pParser = CSenXmlReader::NewL();
	CleanupStack::PushL(pParser);
	CSenVtcpTransportProperties *pProperties=CSenVtcpTransportProperties::NewL(KString,*pParser);
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
		
TInt CSenUtilsBCTest::SenVtcpTransportProperties_NewLC_1L(TTestResult& aResult)
{
    SetupL();

	TBool Flag;
	_LIT8(KString,"<parent><element1/><element2/><element3/>parent content plus some other content</parent>");
	CSenXmlReader* pParser = CSenXmlReader::NewL();
	CleanupStack::PushL(pParser);
	CSenVtcpTransportProperties *pProperties=CSenVtcpTransportProperties::NewLC(KString,*pParser);
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

TInt CSenUtilsBCTest::SenVtcpTransportProperties_NewL_2L(TTestResult& aResult)
{
    SetupL();

	TBool Flag;
	CSenBaseElement* pElement= CSenBaseElement::NewL(_L8("Nokia"));
	CleanupStack::PushL(pElement);
	CSenVtcpTransportProperties *pProperties=CSenVtcpTransportProperties::NewL(*pElement);
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

TInt CSenUtilsBCTest::SenVtcpTransportProperties_NewLC_2L(TTestResult& aResult)
{
    SetupL();

	TBool Flag;
	CSenBaseElement* pElement= CSenBaseElement::NewL(_L8("Nokia"));
	CleanupStack::PushL(pElement);
	CSenVtcpTransportProperties *pProperties=CSenVtcpTransportProperties::NewLC(*pElement);
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
    
TInt CSenUtilsBCTest::SenVtcpTransportProperties_OnewayMessageOnOffLL(TTestResult& aResult )
{
    SetupL();

	TBool OnRoff;
	CSenVtcpTransportProperties* pElement= CSenVtcpTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetOnewayMessageOnOffL(TRUE);
    TInt retval=pElement->OnewayMessageOnOffL(OnRoff);
    TL(retval == KErrNone);
	TL(OnRoff == TRUE);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    
}

TInt CSenUtilsBCTest::SenVtcpTransportProperties_SetOnewayMessageOnOffLL( TTestResult& aResult)
{

    SetupL();

	_LIT8(KElementText,"<Properties><OneWayMessage>true</OneWayMessage></Properties>");
	CSenVtcpTransportProperties* pElement= CSenVtcpTransportProperties::NewL();
    CleanupStack::PushL(pElement);
    pElement->SetOnewayMessageOnOffL(TRUE);
    HBufC8* ElementBuf=pElement->AsUtf8L();
    TL(*ElementBuf == KElementText);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    
}

TInt CSenUtilsBCTest::SenVtcpTransportProperties_SetMaxTimeToLiveLL(TTestResult& aResult )
{
    SetupL();

	TInt PropValue;
	CSenVtcpTransportProperties* pElement= CSenVtcpTransportProperties::NewL();
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

TInt CSenUtilsBCTest::SenVtcpTransportProperties_MaxTimeToLiveLL(TTestResult& aResult )
{
	TTestResult Result;
	SenVtcpTransportProperties_SetMaxTimeToLiveLL( Result);
}

TInt CSenUtilsBCTest::SenVtcpTransportProperties_SetMinTimeToLiveLL(TTestResult& aResult )
{
    SetupL();

	TInt PropValue;
	CSenVtcpTransportProperties* pElement= CSenVtcpTransportProperties::NewL();
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

TInt CSenUtilsBCTest::SenVtcpTransportProperties_MinTimeToLiveLL(TTestResult& aResult )
{
	TTestResult Result;
	SenVtcpTransportProperties_SetMinTimeToLiveLL(Result);
}
*/
//  End of File
