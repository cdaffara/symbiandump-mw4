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








//  CLASS HEADER
#include "myxml.h"
//  EXTERNAL INCLUDES
#include <wspolicyregistry.h>
#include <wspolicy.h>
#include <wspolicyutils.h>
#include <TestInternalWSPolicy.h>
#include <stringpool.h>
#include <senwsdescription.h>

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "wsPolicyTester.h"
#include <StifTestModule.h>
// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
namespace
{

_LIT8( KWsPolicyNsUri , "http://schemas.xmlsoap.org/ws/2004/09/policy"); // Namespace of WS-Policy
_LIT8( KWsPolicy , "Policy");                       // Tag name of Policy
_LIT8( KWsPolicyPrefix , "wsp");                    // Prefix of WS-Policy namespace
_LIT8( KWsPolicyQName , "wsp:Policy");                    // Prefix of WS-Policy namespace

//registry resources
_LIT(KReg1, "c:\\testing\\data\\registry1.xml");
_LIT(KReg2, "c:\\testing\\data\\registry2.xml");
_LIT(KReg3, "c:\\testing\\data\\registry3.xml");
_LIT8(KID1, "ID1");
_LIT8(KID2, "ID2");
_LIT8(KID3, "ID3");
_LIT8(KP1, "P1");
_LIT8(KP2, "P2");
_LIT8(KP3, "P3");
_LIT8(KURI1, "URI1");
_LIT8(KURI2, "URI2");
_LIT8(KURI3, "URI3");

//WSPolicy resource
_LIT(KPolicy1, "c:\\testing\\data\\policy1.xml");
_LIT(KPolicy2, "c:\\testing\\data\\policy2.xml"); //
_LIT(KExpPolicy1, "c:\\testing\\data\\expiry1.xml"); //for expiry tag validation
_LIT(KPolicy3, "c:\\testing\\data\\policy3.xml"); //for optional element = true
_LIT(KPolicy3Ref, "c:\\testing\\data\\policy3Ref.xml");
_LIT(KPolicy4, "c:\\testing\\data\\policy4.xml"); //for nesting
_LIT(KPolicy4Ref,"c:\\testing\\data\\policy4Ref.xml");
_LIT(KPolicy5, "c:\\testing\\data\\policy5.xml"); //for multiple nesting
_LIT(KPolicy5Ref,"c:\\testing\\data\\policy5Ref.xml");

// Policy, name and its normalized reference    
_LIT8(KPolicyName, "P1");
_LIT8(KPolicy,   "<wsp:Policy xmlns:wsp=\"http://schemas.xmlsoap.org/ws/2004/09/policy\" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\" Name=\"P1\" wsu:Id=\"ID1\" MetadataEndpoint=\"URI1\"><wsp:ExactlyOne><wsp:All/></wsp:ExactlyOne></wsp:Policy>");
_LIT8(KPolicyRef,"<wsp:Policy xmlns:wsp=\"http://schemas.xmlsoap.org/ws/2004/09/policy\" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\" Name=\"P1\" wsu:Id=\"ID1\" MetadataEndpoint=\"URI1\"><wsp:ExactlyOne><wsp:All/></wsp:ExactlyOne></wsp:Policy>");
const TInt KRefLength = 267;

}


// MACROS
#define LOCAL_ASSERT(expression)  {if(!(expression)){return KErrArgument;}}
#define LOCAL_VOID_ASSERT(expression)	{if(!(expression)){return ;}}
#define STIFF_ASSERT_NOT_EQUALS_DESC(expression1, expression2, desc){TL((expression1) != (expression2)){RDebug::Print(_L(desc)) ;}}
#define STIFF_ASSERT_DESC(expression, desc)	{if(!(expression)){RDebug::Print(_L(desc)) ;return KErrArgument;}}

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
// CwsPolicyTester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CwsPolicyTester::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CwsPolicyTester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CwsPolicyTester::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
  
        ENTRY("NewL-test_CWSPolicyUtils_PolicyIdL", CwsPolicyTester::MT_CWSPolicyUtils_PolicyIdLL),
        ENTRY("NewL-test_CWSPolicyUtils_PolicyNameL", CwsPolicyTester::MT_CWSPolicyUtils_PolicyNameLL),
        ENTRY("NewL-test_CWSPolicyUtils_IsExpiredPolicyL", CwsPolicyTester::MT_CWSPolicyUtils_PolicyUriLL),
        ENTRY("NewL-test_CWSPolicyUtils_IsExpiredPolicyL", CwsPolicyTester::MT_CWSPolicyUtils_IsExpiredPolicyLL),
        ENTRY("NewL-test_CPolicyRegistry_NewL", CwsPolicyTester::MT_CPolicyRegistry_NewLL),
        ENTRY("NewLC-test_CPolicyRegistry_NewLC", CwsPolicyTester::MT_CPolicyRegistry_NewLCL),
        ENTRY("LookupPolicy-test_CPolicyRegistry_LookupPolicy", CwsPolicyTester::MT_CPolicyRegistry_LookupPolicyL),
        ENTRY("LookupPolicyByName-test_CPolicyRegistry_LookupPolicyByName", CwsPolicyTester::MT_CPolicyRegistry_LookupPolicyByNameL),
        ENTRY("RegisterPolicy-test_CPolicyRegistry_RegisterPolicy", CwsPolicyTester::MT_CPolicyRegistry_RegisterPolicyL),
        ENTRY("UnregisterPolicy-test_CPolicyRegistry_UnregisterPolicy", CwsPolicyTester::MT_CPolicyRegistry_UnregisterPolicyL),
        ENTRY("PopulateRegistry-test_CPolicyRegistry_PopulateRegistry", CwsPolicyTester::MT_CPolicyRegistry_PopulateRegistryL),
        ENTRY("ResetRegistry-test_CPolicyRegistry_ResetRegistry", CwsPolicyTester::MT_CPolicyRegistry_ResetRegistryL),
        ENTRY("NewL-test_CWSPolicy_NewL", CwsPolicyTester::MT_CWSPolicy_NewLL),
        ENTRY("NewLC-test_CWSPolicy_NewLC", CwsPolicyTester::MT_CWSPolicy_NewLCL),
        ENTRY("NewL-test_CWSPolicy_NewL", CwsPolicyTester::MT_CWSPolicy_NewL_1L),
        ENTRY("NewLC-test_CWSPolicy_NewLC", CwsPolicyTester::MT_CWSPolicy_NewLC_1L),
        ENTRY("NormalizePolicyL-test_CWSPolicy_NormalizePolicyL", CwsPolicyTester::MT_CWSPolicy_NormalizePolicyLL),
        ENTRY("IntersectPolicyL-test_CWSPolicy_IntersectPolicyL", CwsPolicyTester::MT_CWSPolicy_IntersectPolicyLL),
        ENTRY("MergePolicyL-test_CWSPolicy_MergePolicyL", CwsPolicyTester::MT_CWSPolicy_MergePolicyLL),
        ENTRY("Normalized-test_CWSPolicy_Normalized", CwsPolicyTester::MT_CWSPolicy_NormalizedL),
        ENTRY("Policy-test_CWSPolicy_Policy", CwsPolicyTester::MT_CWSPolicy_Policy),
        ENTRY("AsXmlL-test_CWSPolicy_AsXmlL", CwsPolicyTester::MT_CWSPolicy_NormalizedAsXmlLL),
        ENTRY("NormalizedPolicy-test_CWSPolicy_NormalizedPolicy", CwsPolicyTester::MT_CWSPolicy_NormalizedPolicy),
        ENTRY("IsExpiredPolicy-test_CWSPolicy_IsExpiredPolicy", CwsPolicyTester::MT_CWSPolicy_IsExpiredPolicyL),
        ENTRY("NewL-test_CSenInternalWsPolicy_NewL", CwsPolicyTester::MT_CSenInternalWsPolicy_NewLL),
        ENTRY("NewLC-test_CSenInternalWsPolicy_NewLC", CwsPolicyTester::MT_CSenInternalWsPolicy_NewLCL),
        ENTRY("NewL-test_CSenInternalWsPolicy_NewL", CwsPolicyTester::MT_CSenInternalWsPolicy_NewL_1L),
        ENTRY("NewLC-test_CSenInternalWsPolicy_NewLC", CwsPolicyTester::MT_CSenInternalWsPolicy_NewLC_1L),
        ENTRY("NewLC-test_CSenInternalWsPolicy_NewLC", CwsPolicyTester::MT_CSenInternalWsPolicy_NewL_2L),
        ENTRY("NewLC-test_CSenInternalWsPolicy_NewLC", CwsPolicyTester::MT_CSenInternalWsPolicy_NewLC_2L),
        ENTRY("IsApplicableL-test_CSenInternalWsPolicy_IsApplicableL", CwsPolicyTester::MT_CSenInternalWsPolicy_IsApplicableLL),
        ENTRY("IsApplicableL-test_CSenInternalWsPolicy_IsApplicableL", CwsPolicyTester::MT_CSenInternalWsPolicy_IsApplicableL_1L),
        ENTRY("IsApplicableL-test_CSenInternalWsPolicy_IsApplicableL", CwsPolicyTester::MT_CSenInternalWsPolicy_IsApplicableL_2L),
        ENTRY("UpdateMetadataEndpointL-test_CSenInternalWsPolicy_UpdateMetadataEndpointL", CwsPolicyTester::MT_CSenInternalWsPolicy_UpdateMetadataEndpointLL),
        ENTRY("UpdateMetadataEndpointL-test_CSenInternalWsPolicy_AddMetadataPolicyL", CwsPolicyTester::MT_CSenInternalWsPolicy_AddMetadataPolicyLL),
        ENTRY("UpdateMetadataEndpointL-test_CSenInternalWsPolicy_RemoveMetadataPolicyL", CwsPolicyTester::MT_CSenInternalWsPolicy_RemoveMetadataPolicyLL),
        ENTRY("WsPolicyByUriL-test_CSenInternalWsPolicy_WsPolicyByUriL", CwsPolicyTester::MT_CSenInternalWsPolicy_WsPolicyByUriLL),
        ENTRY("WsPolicyByName-test_CSenInternalWsPolicy_WsPolicyByName", CwsPolicyTester::MT_CSenInternalWsPolicy_WsPolicyByNameL),
        ENTRY("ValidUntilL-test_CSenInternalWsPolicy_ValidUntilL", CwsPolicyTester::MT_CSenInternalWsPolicy_ValidUntilLL),
        ENTRY("NewL-test_CSenWSPolicyIdentifier_NewL", CwsPolicyTester::MT_CSenWSPolicyIdentifier_NewLL),
        ENTRY("NewLC-test_CSenWSPolicyIdentifier_NewLC", CwsPolicyTester::MT_CSenWSPolicyIdentifier_NewLCL),
        ENTRY("NewL-test_CSenWSPolicyIdentifier_NewL", CwsPolicyTester::MT_CSenWSPolicyIdentifier_NewL_1L),
        ENTRY("NewLC-test_CSenWSPolicyIdentifier_NewLC", CwsPolicyTester::MT_CSenWSPolicyIdentifier_NewLC_1L),
        ENTRY("NewL-test_CSenWSPolicyIdentifier_NewL", CwsPolicyTester::MT_CSenWSPolicyIdentifier_NewL_2L),
        ENTRY("NewLC-test_CSenWSPolicyIdentifier_NewLC", CwsPolicyTester::MT_CSenWSPolicyIdentifier_NewLC_2L),
        ENTRY("SetPropertyL-test_CSenWSPolicyIdentifier_SetPropertyL", CwsPolicyTester::MT_CSenWSPolicyIdentifier_SetPropertyLL),
        ENTRY("PropertyL-test_CSenWSPolicyIdentifier_PropertyL", CwsPolicyTester::MT_CSenWSPolicyIdentifier_PropertyLL),
        ENTRY("SetIdL-test_CSenWSPolicyIdentifier_SetIdL", CwsPolicyTester::MT_CSenWSPolicyIdentifier_SetIdLL),
        ENTRY("IdL-test_CSenWSPolicyIdentifier_IdL", CwsPolicyTester::MT_CSenWSPolicyIdentifier_IdLL),
        ENTRY("AddMetadataEndpointL-test_CSenWSPolicyIdentifier_AddMetadataEndpointL", CwsPolicyTester::MT_CSenWSPolicyIdentifier_AddMetadataEndpointLL),
        ENTRY("MetadataEndpointL-test_CSenWSPolicyIdentifier_MetadataEndpointL", CwsPolicyTester::MT_CSenWSPolicyIdentifier_MetadataEndpointLL),
        ENTRY("MetadataEndpointElementL-test_CSenWSPolicyIdentifier_MetadataEndpointElementL", CwsPolicyTester::MT_CSenWSPolicyIdentifier_MetadataEndpointElementLL),
        ENTRY("ElementAttribValueL-test_CSenWSPolicyIdentifier_ElementAttribValueL", CwsPolicyTester::MT_CSenWSPolicyIdentifier_ElementAttribValueLL),
        ENTRY("HasAttributeL-test_CSenWSPolicyIdentifier_HasAttributeL", CwsPolicyTester::MT_CSenWSPolicyIdentifier_HasAttributeLL),
        ENTRY("NewL-test_CWSPolicyWriter_NewL", CwsPolicyTester::MT_CWSPolicyWriter_NewLL),
        ENTRY("NewLC-test_CWSPolicyWriter_NewLC", CwsPolicyTester::MT_CWSPolicyWriter_NewLCL),
        ENTRY("NewL-test_CWSPolicyReader_NewL", CwsPolicyTester::MT_CWSPolicyReader_NewLL),
        ENTRY("NewLC-test_CWSPolicyReader_NewLC", CwsPolicyTester::MT_CWSPolicyReader_NewLCL),
        ENTRY("GetPolicyL-test_CWSPolicyReader_GetPolicyL", CwsPolicyTester::MT_CWSPolicyReader_GetPolicyLL),
        ENTRY("WriteToFileAllL-test_CWSPolicyWriter_WriteToFileAllL", CwsPolicyTester::MT_CWSPolicyWriter_WriteToFileAllLL),
        ENTRY("WriteToFileAllL-test_CWSPolicyWriter_WriteToFileAllL", CwsPolicyTester::MT_CWSPolicyWriter_WriteToFileAllL_1L),
        ENTRY("WriteAllAsXmlL-test_CWSPolicyWriter_WriteAllAsXmlL", CwsPolicyTester::MT_CWSPolicyWriter_WriteAllAsXmlLL),

        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

void CwsPolicyTester::SetupL(  )
    {
//    _LIT(KDir,"XML_App");
//    TBufC<30> aDirBuf(KDir);
//    _LIT(KFile,"WSPolicy_Test.txt");
//    TBufC<30> aNameBuf(KFile);
//    TInt connectRes=fL.Connect();
//    TRAPD(createRes,fL.CreateLog(aDirBuf,aNameBuf,EFileLoggingModeOverwrite));

    }

void CwsPolicyTester::Teardown(  )
    {
//      fL.CloseLog();
//    fL.Close();

    }

TInt CwsPolicyTester::MT_CWSPolicyUtils_PolicyIdLL(CStifItemParser& aItem)
{  
     TPtrC8 p1(KID1);
     TPtrC8 p2(KID2);
     TPtrC8 p3(KID3);

    CMyXmlEle* doc1 = CMyXmlEle::NewL();
    TFileName name1(KPolicy1);
    doc1->ReadFileL(name1);
    CSenElement* policy1 = doc1->XMLDocL();
    TL(policy1 != (CSenElement*)NULL);
    TPtrC8 id1(CWSPolicyUtils::PolicyIdL(policy1));
    TL(id1 == p1);
    
    CMyXmlEle* doc2 = CMyXmlEle::NewL();
    TFileName name2(KPolicy2);
    doc2->ReadFileL(name2);
    CSenElement* policy2 = doc2->XMLDocL();
    TL(policy2 != (CSenElement*)NULL);
    TPtrC8 id2(CWSPolicyUtils::PolicyIdL(policy2));
    TL(id2 == p2);
    
     delete doc1;
     delete doc2;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CWSPolicyUtils_PolicyNameLL(CStifItemParser& aItem )
{
     TPtrC8 p1(KP1);
     TPtrC8 p2(KP2);
     TPtrC8 p3(KP3);

    CMyXmlEle* doc1 = CMyXmlEle::NewL();
    TFileName name1(KPolicy1);
    doc1->ReadFileL(name1);
    CSenElement* policy1 = doc1->XMLDocL();
    TL(policy1 != (CSenElement*)NULL);
    TPtrC8 id1(CWSPolicyUtils::PolicyNameL(policy1));
    TL(id1 == p1);
    
    CMyXmlEle* doc2 = CMyXmlEle::NewL();
    TFileName name2(KPolicy2);
    doc2->ReadFileL(name2);
    CSenElement* policy2 = doc2->XMLDocL();
    TL(policy2 != (CSenElement*)NULL);
    TPtrC8 id2(CWSPolicyUtils::PolicyNameL(policy2));
    TL(id2 == p2);
    
    delete doc1;
    delete doc2;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CWSPolicyUtils_PolicyUriLL(CStifItemParser& aItem)
{
     TPtrC8 p1(KURI1);
     TPtrC8 p2(KURI2);
     TPtrC8 p3(KURI3);

    CMyXmlEle* doc1 = CMyXmlEle::NewL();
    TFileName name1(KPolicy1);
    doc1->ReadFileL(name1);
    CSenElement* policy1 = doc1->XMLDocL();
    TL(policy1 != (CSenElement*)NULL);
    TPtrC8 uri1(CWSPolicyUtils::PolicyUriL(policy1));
    TL(uri1 == p1);
    
    CMyXmlEle* doc2 = CMyXmlEle::NewL();
    TFileName name2(KPolicy2);
    doc2->ReadFileL(name2);
    CSenElement* policy2 = doc2->XMLDocL();
    TL(policy2 != (CSenElement*)NULL);
    TPtrC8 uri2(CWSPolicyUtils::PolicyUriL(policy2));
    TL(uri2 == p2);
    
    delete doc1;
    delete doc2;
    return KErrNone;
}
TInt CwsPolicyTester::MT_CWSPolicyUtils_IsExpiredPolicyLL(CStifItemParser& aItem)
{
    CMyXmlEle* doc1 = CMyXmlEle::NewL();
    TFileName name1(KExpPolicy1);
    doc1->ReadFileL(name1);
    CSenElement* policy1 = doc1->XMLDocL();
    TBool result = EFalse;
    result = CWSPolicyUtils::IsExpiredPolicyL(policy1);
    TL(result == EFalse);

    delete doc1;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CPolicyRegistry_NewLL( CStifItemParser& aItem  )
    {
     CPolicyRegistry* registry = CPolicyRegistry::NewL();
     TL(registry != (CPolicyRegistry*)NULL);

     __ASSERT_ALWAYS_NO_LEAVE(delete registry);
     registry = NULL;
     return KErrNone;
    }

TInt CwsPolicyTester::MT_CPolicyRegistry_NewLCL( CStifItemParser& aItem  )
    {
     CPolicyRegistry* registry = CPolicyRegistry::NewLC();
     TL(registry != (CPolicyRegistry*)NULL);
     
     CleanupStack::Pop();
     __ASSERT_ALWAYS_NO_LEAVE(delete registry);
     registry = NULL;
     return KErrNone;
    }

TInt CwsPolicyTester::MT_CPolicyRegistry_LookupPolicyL( CStifItemParser& aItem  )
    {
    CPolicyRegistry* registry = CPolicyRegistry::NewL();
    TL(registry != (CPolicyRegistry*)NULL);
         
    CMyXmlEle* doc1 = CMyXmlEle::NewL();
    TFileName name(KReg1);
    doc1->ReadFileL(name);
    CSenElement* policy = doc1->XMLDocL();
    
    registry->PopulateRegistryL(policy);  //codescannerwarnings
     
     TPtrC8 p1(KID1);
     TPtrC8 p2(KID2);
     TPtrC8 p3(KID3);
     
     CSenElement* ele1 = registry->LookupPolicy(p1);
     TPtrC8 policyId1(CWSPolicyUtils::PolicyIdL(ele1));
     TL(policyId1 == p1);

     CSenElement* ele2 = registry->LookupPolicy(p2);
     TPtrC8 policyId2(CWSPolicyUtils::PolicyIdL(ele2));
     TL(policyId2 == p2);
          
     __ASSERT_ALWAYS_NO_LEAVE(delete registry);
     registry = NULL;
     delete doc1;
     return KErrNone;
    }

TInt CwsPolicyTester::MT_CPolicyRegistry_LookupPolicyByNameL( CStifItemParser& aItem  )
    {
    CPolicyRegistry* registry = CPolicyRegistry::NewL();
    TL(registry != (CPolicyRegistry*)NULL);
         
    CMyXmlEle* doc1 = CMyXmlEle::NewL();
    TFileName name(KReg1);
    doc1->ReadFileL(name);
    CSenElement* policy = doc1->XMLDocL();
    
    registry->PopulateRegistryL(policy);  //codescannerwarnings
     
     TPtrC8 p1Name(KP1);
     TPtrC8 p2Name(KP2);
     TPtrC8 p3Name(KP3);
     
     CSenElement* ele1 = registry->LookupPolicyByNameL(p1Name); //codescannerwarnings
     TL(ele1 != (CSenElement*)NULL);

     CSenElement* ele2 = registry->LookupPolicyByNameL(p2Name); //codescannerwarnings
     TL(ele2  != (CSenElement*)NULL);

     CSenElement* ele3 = registry->LookupPolicyByNameL(p3Name); //codescannerwarnings
     TL(ele3 == (CSenElement*)NULL);
          
     __ASSERT_ALWAYS_NO_LEAVE(delete registry);
     registry = NULL;
     delete doc1;
     return KErrNone;
    }

TInt CwsPolicyTester::MT_CPolicyRegistry_RegisterPolicyL( CStifItemParser& aItem  )
    {
    CPolicyRegistry* registry = CPolicyRegistry::NewL();
    TL(registry != (CPolicyRegistry*)NULL);
    
  
    CMyXmlEle* doc2 = CMyXmlEle::NewL();
    TFileName name2(KReg1);
    doc2->ReadFileL(name2);
    CSenElement* policy2 = doc2->XMLDocL();

    CMyXmlEle* doc3 = CMyXmlEle::NewL();
    TFileName name3(KReg3);
    doc3->ReadFileL(name3);
    CSenElement* policy3 = doc3->XMLDocL();
    
     TPtrC8 p1(KID1);
     TPtrC8 p2(KID2);
     TPtrC8 p3(KID3);
    
    registry->RegisterPolicy(p1,policy2);

    
     CSenElement* ele2 = registry->LookupPolicy(p1);
     TL(ele2 != (CSenElement*)NULL);
     CSenElement* ele3 = registry->LookupPolicy(p2);
     TL(ele3 == (CSenElement*)NULL);

    registry->RegisterPolicy(p2,policy3);

     CSenElement* ele2_1 = registry->LookupPolicy(p1);
     TL(ele2_1 != (CSenElement*)NULL);
     CSenElement* ele3_1 = registry->LookupPolicy(p2);
     TL(ele3_1 != (CSenElement*)NULL);
     
          
     __ASSERT_ALWAYS_NO_LEAVE(delete registry);
     registry = NULL;
     delete doc2;
     delete doc3;
     return KErrNone;
    }

TInt CwsPolicyTester::MT_CPolicyRegistry_UnregisterPolicyL( CStifItemParser& aItem  )
    {
   CPolicyRegistry* registry = CPolicyRegistry::NewL();
    TL(registry != (CPolicyRegistry*)NULL);
         
    CMyXmlEle* doc1 = CMyXmlEle::NewL();
    TFileName name(KReg1);
    doc1->ReadFileL(name);
    CSenElement* policy = doc1->XMLDocL();
    
    registry->PopulateRegistryL(policy);  //codescannerwarnings
     
     TPtrC8 p1(KID1);
     TPtrC8 p2(KID2);
     TPtrC8 p3(KID3);
     
     CSenElement* ele1 = registry->LookupPolicy(p1);
     TL(ele1 != (CSenElement*)NULL);
     CSenElement* ele2 = registry->LookupPolicy(p2);
     TL(ele2 != (CSenElement*)NULL);
     registry->UnregisterPolicy(p2);
     ele1 = NULL;
     ele2 = NULL;

     ele1 = registry->LookupPolicy(p1);
     TL(ele1 != (CSenElement*)NULL);
     ele2 = registry->LookupPolicy(p2);
     TL(ele2 == (CSenElement*)NULL);
     registry->UnregisterPolicy(p1);
     
     ele1 = NULL;
     ele2 = NULL;
     ele1 = registry->LookupPolicy(p1);
     TL(ele1 == (CSenElement*)NULL);
     ele2 = registry->LookupPolicy(p2);
     TL(ele2 == (CSenElement*)NULL);

          
     __ASSERT_ALWAYS_NO_LEAVE(delete registry);
     registry = NULL;
     delete doc1;
     return KErrNone;
    }

TInt CwsPolicyTester::MT_CPolicyRegistry_PopulateRegistryL( CStifItemParser& aItem  )
    {
    
    CPolicyRegistry* registry = CPolicyRegistry::NewL();
    TL(registry != (CPolicyRegistry*)NULL);
         
    CMyXmlEle* doc1 = CMyXmlEle::NewL();
    TFileName name(KReg1);
    doc1->ReadFileL(name);
    CSenElement* policy = doc1->XMLDocL();
    
    registry->PopulateRegistryL(policy);  //codescannerwarnings
     
     TPtrC8 p1(KID1);
     TPtrC8 p2(KID2);
     TPtrC8 p3(KID3);
     
     CSenElement* ele1 = registry->LookupPolicy(p1);
     TL(ele1 != (CSenElement*)NULL);
     CSenElement* ele2 = registry->LookupPolicy(p2);
     TL(ele2 != (CSenElement*)NULL);
     CSenElement* ele3 = registry->LookupPolicy(p3);
     TL(ele3 == (CSenElement*)NULL);
          
     __ASSERT_ALWAYS_NO_LEAVE(delete registry);
     registry = NULL;
     delete doc1;
     return KErrNone;
    }

TInt CwsPolicyTester::MT_CPolicyRegistry_ResetRegistryL( CStifItemParser& aItem  )
    {
    
    CPolicyRegistry* registry = CPolicyRegistry::NewL();
    TL(registry != (CPolicyRegistry*)NULL);
    
    CMyXmlEle* doc1 = CMyXmlEle::NewL();
    TFileName name(KReg1);
    doc1->ReadFileL(name);
    CSenElement* policy = doc1->XMLDocL();
    
    registry->PopulateRegistryL(policy);  //codescannerwarnings
     
     TPtrC8 p1(KID1);
     TPtrC8 p2(KID2);
          
     CSenElement* ele1 = registry->LookupPolicy(p1);
     TL(ele1 != (CSenElement*)NULL);
     CSenElement* ele2 = registry->LookupPolicy(p2);
     TL(ele2 != (CSenElement*)NULL);
     
     registry->ResetRegistry();
     
     CSenElement* ele3 = registry->LookupPolicy(p1);
     TL(ele3 == (CSenElement*)NULL);
     CSenElement* ele4 = registry->LookupPolicy(p2);
     TL(ele4 == (CSenElement*)NULL);
          
     __ASSERT_ALWAYS_NO_LEAVE(delete registry);
     registry = NULL;
     delete doc1;
     return KErrNone;
    }

TInt CwsPolicyTester::MT_CWSPolicy_NewLL( CStifItemParser& aItem  )
    {
    CWSPolicy* xmlPolicy = CWSPolicy::NewL();
    LOCAL_ASSERT(xmlPolicy);

    __ASSERT_ALWAYS_NO_LEAVE(delete xmlPolicy);
    xmlPolicy = NULL;
    return KErrNone;
    }

TInt CwsPolicyTester::MT_CWSPolicy_NewLCL( CStifItemParser& aItem  )
    {
    CWSPolicy* xmlPolicy = CWSPolicy::NewLC(); 
    LOCAL_ASSERT(xmlPolicy);
    CleanupStack::Pop();
    __ASSERT_ALWAYS_NO_LEAVE(delete xmlPolicy);
    xmlPolicy = NULL;
    return KErrNone;
    }

TInt CwsPolicyTester::MT_CWSPolicy_NewL_1L( CStifItemParser& aItem  )
    {
    TPtrC8 name(KPolicyName);
    TPtrC8 policy(KPolicy);
    CWSPolicy* xmlPolicy = CWSPolicy::NewL(name, policy);
    LOCAL_ASSERT(xmlPolicy);

    __ASSERT_ALWAYS_NO_LEAVE(delete xmlPolicy);
    xmlPolicy = NULL;
    return KErrNone;
    }

TInt CwsPolicyTester::MT_CWSPolicy_NewLC_1L( CStifItemParser& aItem  )
    {
    TPtrC8 name(KPolicyName);
    TPtrC8 policy(KPolicy);
    CWSPolicy* xmlPolicy = CWSPolicy::NewLC(name, policy); //KText
    LOCAL_ASSERT(xmlPolicy);
    CleanupStack::Pop();
    __ASSERT_ALWAYS_NO_LEAVE(delete xmlPolicy);
    xmlPolicy = NULL;
    return KErrNone;
    }

void CwsPolicyTester::DoNormalizePolicyL(TFileName aFile, TInt count)
{
    CMyXmlEle* doc1 = CMyXmlEle::NewL();
    TFileName name1(aFile);
    doc1->ReadFileL(name1);
    CSenElement* policy1 = doc1->XMLDocL();
    
    HBufC8* policyBuf = policy1->AsXmlL();
    
    TPtrC8 name(CWSPolicyUtils::PolicyNameL(policy1));
    TPtrC8 policy(policyBuf->Des());
    
    CWSPolicy* xmlPolicy = CWSPolicy::NewL(name, policy); //KText
    
    //LOCAL_ASSERT(xmlPolicy->NormalizePolicyL(NULL) == TRUE);
    HBufC8* xmlBuffer = xmlPolicy->PolicyAsXmlL();
    
    CleanupStack::PushL( xmlBuffer );
    TPtr8 ptr = xmlBuffer->Des();
    
    if (ptr.Length() > 0)
        RDebug::Print( _L( " xmlBuffer [%S]"),&ptr);
    else
        RDebug::Print( _L( " xmlBuffer empty"));
    
    switch (count)
    {
        case (3):
        {
            CMyXmlEle* doc3 = CMyXmlEle::NewL();
            TFileName name3(KPolicy3Ref);
            doc3->ReadFileL(name3);
            CSenElement* policy3 = doc3->XMLDocL();
            HBufC8* policyBuf3 = policy3->AsXmlL();
            TPtr8 policy3Ptr = policyBuf3->Des();
            TL(ptr == policy3Ptr);
            delete policyBuf3;
            delete doc3;
            break;            
        }
        case 4:
        {
            CMyXmlEle* doc4 = CMyXmlEle::NewL();
            TFileName name4(KPolicy4Ref);
            doc4->ReadFileL(name4);
            CSenElement* policy4 = doc4->XMLDocL();
            HBufC8* policyBuf4 = policy4->AsXmlL();
            TPtr8 policy4Ptr = policyBuf4->Des();
            TL(ptr == policy4Ptr);
            delete policyBuf4;
            delete doc4;
            break;            
        }
        case 5:
        {
            CMyXmlEle* doc5 = CMyXmlEle::NewL();
            TFileName name5(KPolicy5Ref);
            doc5->ReadFileL(name5);
            CSenElement* policy5 = doc5->XMLDocL();
            HBufC8* policyBuf5 = policy5->AsXmlL();
            TPtr8 policy5Ptr = policyBuf5->Des();
            TL(ptr == policy5Ptr);
            delete policyBuf5;
            delete doc5;
            break;            
        }

    }

    CleanupStack::PopAndDestroy();
    
    __ASSERT_ALWAYS_NO_LEAVE(delete xmlPolicy);
    xmlPolicy = NULL;
    delete policyBuf;
    delete doc1;
    
}
TInt CwsPolicyTester::MT_CWSPolicy_NormalizePolicyLL( CStifItemParser& aItem  )
    {
    
    TBuf<255> file;
    _LIT( KFileName, "c:\\testing\\data\\Policy%d.xml");
    for(TInt i = 3; i< 6 ; i++)
    {
        file.Format(KFileName, i);
        RDebug::Print( _L( "NormalizePolicyL: File [%S]"),&file);
        DoNormalizePolicyL(file, i);
    }
    return KErrNone;

    }
    
void CwsPolicyTester::TestIntersectPolicyL(TInt i, TInt j)
{

    _LIT(KFile, "c:\\testing\\data\\Policy%d.xml"); //BigSecurityPolicy3.xml");
    _LIT( KresultFile, "c:\\testing\\data\\inter%d-%d.xml");
    TBuf<50> file1;
    file1.Format(KFile, i);
    TBuf<50> file2;
    file2.Format(KFile, j);
//Read first file
    CMyXmlEle* doc1 = CMyXmlEle::NewL();
    TFileName name1(file1);
    doc1->ReadFileL(name1);
    CSenElement* policy1 = doc1->XMLDocL();
    HBufC8* policyBuf1 = policy1->AsXmlL();
    TPtrC8 uri1(CWSPolicyUtils::PolicyUriL(policy1));
    TPtrC8 policyPtr1(policyBuf1->Des());
    
    CWSPolicy* xmlPolicy1 = CWSPolicy::NewL(uri1, policyPtr1); //KText
    xmlPolicy1->NormalizePolicyL(NULL);
    CSenElement* normPolicy1 = xmlPolicy1->PolicyL(); //codescannerwarnings
    delete policyBuf1;
    delete doc1;

//Read second file  
    CMyXmlEle* doc2 = CMyXmlEle::NewL();
    TFileName name2(file2);
    doc2->ReadFileL(name2);
    CSenElement* policy2 = doc2->XMLDocL();
    HBufC8* policyBuf2 = policy2->AsXmlL();
    TPtrC8 uri2(CWSPolicyUtils::PolicyUriL(policy2));
    TPtrC8 policyPtr2(policyBuf2->Des());
    
    CWSPolicy* xmlPolicy2 = CWSPolicy::NewL(uri2, policyPtr2); //KText
    xmlPolicy2->NormalizePolicyL(NULL);
    CSenElement* normPolicy2 = xmlPolicy2->PolicyL(); //codescannerwarnings
    delete policyBuf2;
    delete doc2;

////Intersecting

    CWSPolicy* mergedPolicy = xmlPolicy2->IntersectPolicyL(xmlPolicy1);
    if(mergedPolicy)
    {

        TBuf<50> resfile;
        resfile.Format(KresultFile, i, j);

        CSenElement* intpolicy = mergedPolicy->PolicyL(); //codescannerwarnings
        HBufC8* xmlBuffer = intpolicy->AsXmlL();       
        CleanupStack::PushL( xmlBuffer );
        TPtr8 ptr = xmlBuffer->Des();

        CMyXmlEle* doc3 = CMyXmlEle::NewL();
        TFileName name3(resfile);
        doc3->ReadFileL(name3);
        CSenElement* policy3 = doc3->XMLDocL();
        HBufC8* policyBuf3 = policy3->AsXmlL();
        TPtr8 policy3Ptr = policyBuf3->Des();
        TL(ptr.Length() == policy3Ptr.Length());
        delete policyBuf3;
        delete doc3;
        
        CleanupStack::PopAndDestroy(xmlBuffer);
    }
    delete mergedPolicy;
    delete xmlPolicy2;  
    delete xmlPolicy1;    
}
TInt CwsPolicyTester::MT_CWSPolicy_IntersectPolicyLL( CStifItemParser& aItem  )
    {
    
    TInt start = 3;
    TInt last = 5;

    for (TInt i = start; i < last+1; i++)
    {
        for (TInt j = start; j < last+1; j++)
        {
             TestIntersectPolicyL(i , j) ;  
        }
        
    }
    return KErrNone;
    }

void TestMergePolicyL(TInt i, TInt j)
{

    _LIT(KFile, "c:\\testing\\data\\Policy%d.xml"); //BigSecurityPolicy3.xml");
    _LIT( KresultFile, "c:\\testing\\data\\merge%d-%d.xml");
    TBuf<50> file1;
    file1.Format(KFile, i);
    TBuf<50> file2;
    file2.Format(KFile, j);
//Read first file
    CMyXmlEle* doc1 = CMyXmlEle::NewL();
    TFileName name1(file1);
    doc1->ReadFileL(name1);
    CSenElement* policy1 = doc1->XMLDocL();
    HBufC8* policyBuf1 = policy1->AsXmlL();
    TPtrC8 uri1(CWSPolicyUtils::PolicyUriL(policy1));
    TPtrC8 policyPtr1(policyBuf1->Des());
    
    CWSPolicy* xmlPolicy1 = CWSPolicy::NewL(uri1, policyPtr1); //KText
    xmlPolicy1->NormalizePolicyL(NULL);
    CSenElement* normPolicy1 = xmlPolicy1->PolicyL(); //codescannerwarnings
    delete policyBuf1;
    delete doc1;

//Read second file  
    CMyXmlEle* doc2 = CMyXmlEle::NewL();
    TFileName name2(file2);
    doc2->ReadFileL(name2);
    CSenElement* policy2 = doc2->XMLDocL();
    HBufC8* policyBuf2 = policy2->AsXmlL();
    TPtrC8 uri2(CWSPolicyUtils::PolicyUriL(policy2));
    TPtrC8 policyPtr2(policyBuf2->Des());
    
    CWSPolicy* xmlPolicy2 = CWSPolicy::NewL(uri2, policyPtr2); //KText
    xmlPolicy2->NormalizePolicyL(NULL);
    CSenElement* normPolicy2 = xmlPolicy2->PolicyL(); //codescannerwarnings
    delete policyBuf2;
    delete doc2;

////Intersecting

    CWSPolicy* mergedPolicy = xmlPolicy2->MergePolicyL(xmlPolicy1);
    if(mergedPolicy)
    {

        TBuf<50> resfile;
        resfile.Format(KresultFile, i, j);

        CSenElement* intpolicy = mergedPolicy->PolicyL(); //codescannerwarnings
        HBufC8* xmlBuffer = intpolicy->AsXmlL();       
        CleanupStack::PushL( xmlBuffer );
        TPtr8 ptr = xmlBuffer->Des();

        CMyXmlEle* doc3 = CMyXmlEle::NewL();
        TFileName name3(resfile);
        doc3->ReadFileL(name3);
        CSenElement* policy3 = doc3->XMLDocL();
        HBufC8* policyBuf3 = policy3->AsXmlL();
        TPtr8 policy3Ptr = policyBuf3->Des();
     //   TL(ptr.Length() == policy3Ptr.Length());
        delete policyBuf3;
        delete doc3;
        
        CleanupStack::PopAndDestroy(xmlBuffer);
    }
    delete mergedPolicy;
    delete xmlPolicy2;  
    delete xmlPolicy1;    
    
}
TInt CwsPolicyTester::MT_CWSPolicy_MergePolicyLL( CStifItemParser& aItem  )
    {
    TInt start = 3;
    TInt last = 5;

    for (TInt i = start; i < last+1; i++)
    {
        for (TInt j = start; j < last+1; j++)
        {
             TestMergePolicyL(i , j) ;  
        }
        
    }
    return KErrNone;
    }

TInt CwsPolicyTester::MT_CWSPolicy_NormalizedL( CStifItemParser& aItem  )
    {
    
    TPtrC8 name(KPolicyName);
    TPtrC8 policy(KPolicy);
    CWSPolicy* xmlPolicy = CWSPolicy::NewL(name, policy); //KText
   
    TL( xmlPolicy->Normalized() == FALSE);
    LOCAL_ASSERT(xmlPolicy->NormalizePolicyL(NULL) == TRUE);
    TL( xmlPolicy->Normalized() == TRUE);     
    
    __ASSERT_ALWAYS_NO_LEAVE(delete xmlPolicy);
    xmlPolicy = NULL;
    return KErrNone;
    }

TInt CwsPolicyTester::MT_CWSPolicy_Policy( CStifItemParser& aItem )
    {
    TPtrC8 name(KPolicyName);
    TPtrC8 policy(KPolicy);
    CWSPolicy* xmlPolicy = CWSPolicy::NewL(name, policy); //KText

    CSenElement* origPolicy = xmlPolicy->PolicyL();     //codescannerwarnings
    TL( origPolicy != (CSenElement*)NULL);     

    __ASSERT_ALWAYS_NO_LEAVE(delete xmlPolicy);
    xmlPolicy = NULL;
    return KErrNone;
    }

TInt CwsPolicyTester::MT_CWSPolicy_NormalizedAsXmlLL( CStifItemParser& aItem  )
    {
    TPtrC8 name(KPolicyName);
    TPtrC8 policy(KPolicy);
    
    CWSPolicy* xmlPolicy = CWSPolicy::NewL(name, policy); //KText
    
    LOCAL_ASSERT(xmlPolicy->NormalizePolicyL(NULL) == TRUE);
    
    CSenElement* normPolicy = xmlPolicy->PolicyL();         //codescannerwarnings
    HBufC8* xmlBuffer = normPolicy->AsXmlL();    
    CleanupStack::PushL( xmlBuffer );
    
    TInt data = xmlBuffer->Length();
    TInt ref_data(KRefLength);
    
    TPtr8 ptr = xmlBuffer->Des();
    if (ptr.Length() > 0)
        RDebug::Print( _L( "xmlBuffer [%S]"),&ptr);
    else
        RDebug::Print( _L( "xmlBuffer empty"));
    
  
    TL(data == ref_data);
    
    CleanupStack::PopAndDestroy(xmlBuffer);

    
    __ASSERT_ALWAYS_NO_LEAVE(delete xmlPolicy);
    
    xmlPolicy = NULL;
    return KErrNone;
    }

TInt CwsPolicyTester::MT_CWSPolicy_NormalizedPolicy( CStifItemParser& aItem  )
    {
    TPtrC8 name(KPolicyName);
    TPtrC8 policy(KPolicy);
    CWSPolicy* xmlPolicy = CWSPolicy::NewL(name, policy); //KText

    TL( xmlPolicy->PolicyL() != (CSenElement*)NULL);      //codescannerwarnings
    LOCAL_ASSERT(xmlPolicy->NormalizePolicyL(NULL) == TRUE);
    TL( xmlPolicy->PolicyL() != (CSenElement*)NULL);     //codescannerwarnings

    __ASSERT_ALWAYS_NO_LEAVE(delete xmlPolicy);
    xmlPolicy = NULL;
    return KErrNone;
    }

TInt CwsPolicyTester::MT_CWSPolicy_IsExpiredPolicyL( CStifItemParser& aItem  )
    {

    CMyXmlEle* doc1 = CMyXmlEle::NewL();
    TFileName name1(KExpPolicy1);
    doc1->ReadFileL(name1);
    
    CSenElement* policy1 = doc1->XMLDocL();
    TPtrC8 name(CWSPolicyUtils::PolicyNameL(policy1));

    HBufC8* policyBuf = policy1->AsXmlL();
    CleanupStack::PushL( policyBuf );
    TPtrC8 policy(policyBuf->Des());
    
    CWSPolicy* xmlPolicy = CWSPolicy::NewL(name, policy);
    
    CleanupStack::PopAndDestroy();
    LOCAL_ASSERT(xmlPolicy);
    TBool result = EFalse;
    result = xmlPolicy->IsExpiredPolicyL(); //codescannerwarnings
    TL(result == EFalse);
    __ASSERT_ALWAYS_NO_LEAVE(delete xmlPolicy);
    xmlPolicy = NULL;
    delete doc1;
    return KErrNone;
    }

TInt CwsPolicyTester::MT_CSenInternalWsPolicy_NewLL( CStifItemParser& aItem )
{
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewL();
    LOCAL_ASSERT( pInternalPolicy );
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CSenInternalWsPolicy_NewLCL( CStifItemParser& aItem )
{
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewLC();
    LOCAL_ASSERT( pInternalPolicy );
    CleanupStack::Pop(pInternalPolicy);
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CSenInternalWsPolicy_NewL_1L( CStifItemParser& aItem )
{
  RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
  
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewL(_L8("ansuri"),_L8("Nokia"),_L8("a:Nokia"),attrArray);
    LOCAL_ASSERT( pInternalPolicy );
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CSenInternalWsPolicy_NewLC_1L( CStifItemParser& aItem )
{
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
  
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewLC(_L8("ansuri"),_L8("Nokia"),_L8("a:Nokia"),attrArray);
    LOCAL_ASSERT( pInternalPolicy );
    CleanupStack::Pop(pInternalPolicy);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CSenInternalWsPolicy_NewL_2L( CStifItemParser& aItem )
{
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParentElement);
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewL(_L8("ansuri"),_L8("Nokia"),_L8("a:Nokia"),attrArray,*pParentElement);
    LOCAL_ASSERT( pInternalPolicy );
    CleanupStack::PopAndDestroy(pParentElement);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CSenInternalWsPolicy_NewLC_2L( CStifItemParser& aItem )
{
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParentElement);
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewLC(_L8("ansuri"),_L8("Nokia"),_L8("a:Nokia"),attrArray,*pParentElement);
    LOCAL_ASSERT( pInternalPolicy );
    CleanupStack::Pop(pInternalPolicy);
    CleanupStack::PopAndDestroy(pParentElement);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CSenInternalWsPolicy_IsApplicableLL( CStifItemParser& aItem )
{
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewL();
    LOCAL_ASSERT( pInternalPolicy );
    TBool avail=pInternalPolicy->IsApplicableL(_L8(""));
    TL(avail == TRUE);
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CSenInternalWsPolicy_IsApplicableL_1L( CStifItemParser& aItem )
{
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParentElement);
    CTestInternalWSPolicy* pInternalWSPolicy = CTestInternalWSPolicy::NewL(_L8("ansuri"),_L8("Nokia"),_L8("a:Nokia"),attrArray,*pParentElement);
    CleanupStack::PushL(pInternalWSPolicy );
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewL();
    CleanupStack::PushL(pInternalPolicy );
    LOCAL_ASSERT( pInternalPolicy );
    TBool avail=pInternalPolicy->IsApplicableL(pInternalWSPolicy);
    TL(avail == TRUE);
    CleanupStack::Pop(pInternalPolicy);
    CleanupStack::Pop(pInternalWSPolicy);
    CleanupStack::PopAndDestroy(pParentElement);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    delete pInternalWSPolicy ;
    pInternalWSPolicy = NULL;
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}
TInt CwsPolicyTester::MT_CSenInternalWsPolicy_IsApplicableL_2L( CStifItemParser& aItem )
{
    CSenWSDescription* pPattern = CSenWSDescription::NewL(_L8("http://radio.etenee.net:9080/tfs/IDPSSO_IDWSF"),_L8("urn:liberty:as:2004-04"));
    CleanupStack::PushL(pPattern);
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewL();
    CleanupStack::PushL(pInternalPolicy);
    LOCAL_ASSERT( pInternalPolicy );
    TBool avail=pInternalPolicy->IsApplicableL(*pPattern);
    TL(avail == FALSE);
    CleanupStack::PopAndDestroy(pInternalPolicy);
    CleanupStack::PopAndDestroy(pPattern);
    return KErrNone;
}
TInt CwsPolicyTester::MT_CSenInternalWsPolicy_UpdateMetadataEndpointLL( CStifItemParser& aItem )
{
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParentElement);
    CTestInternalWSPolicy* pInternalWSPolicy = CTestInternalWSPolicy::NewL(_L8("ansuri"),_L8("Nokia"),_L8("a:Nokia"),attrArray,*pParentElement);
    CleanupStack::PushL(pInternalWSPolicy );
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewL();
    CleanupStack::PushL(pInternalPolicy );
    LOCAL_ASSERT( pInternalPolicy );
    TInt num(1);
    TBool avail=pInternalPolicy->UpdateMetadataEndpointL(pInternalWSPolicy,num);
    TL(avail == TRUE);
    CleanupStack::Pop(pInternalPolicy);
    CleanupStack::Pop(pInternalWSPolicy);
    CleanupStack::PopAndDestroy(pParentElement);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    delete pInternalWSPolicy ;
    pInternalWSPolicy = NULL;
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CSenInternalWsPolicy_AddMetadataPolicyLL( CStifItemParser& aItem )
{
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParentElement);
    CTestInternalWSPolicy* pInternalWSPolicy = CTestInternalWSPolicy::NewL(_L8("ansuri"),_L8("MetadataEndpoint"),_L8("a:MetadataEndpoint"),attrArray,*pParentElement);
    CleanupStack::PushL(pInternalWSPolicy );
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewL();
    CleanupStack::PushL(pInternalPolicy );
    LOCAL_ASSERT( pInternalPolicy );
    //CSenElement* pMetaElement = CSenBaseElement::NewL(_L8("MetadataEndpoint"));
    //CleanupStack::PushL(pMetaElement );
    CSenElement* pElement = CSenBaseElement::NewL(_L8("Element"));
    CleanupStack::PushL(pElement );
    TInt avail=pInternalPolicy->AddMetadataPolicyL(pElement );
    CleanupStack::PopAndDestroy(pElement);
    //CleanupStack::Pop(pMetaElement);
    TL(avail == -1);
    CleanupStack::Pop(pInternalPolicy);
    CleanupStack::Pop(pInternalWSPolicy);
    CleanupStack::PopAndDestroy(pParentElement);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    delete pInternalWSPolicy ;
    pInternalWSPolicy = NULL;
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CSenInternalWsPolicy_RemoveMetadataPolicyLL(CStifItemParser& aItem)
{
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParentElement);
    CTestInternalWSPolicy* pInternalWSPolicy = CTestInternalWSPolicy::NewL(_L8("nsuri"),_L8("MetadataEndpoint"),_L8("a:MetadataEndpoint"),attrArray,*pParentElement);
    CleanupStack::PushL(pInternalWSPolicy );
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewL();
    CleanupStack::PushL(pInternalPolicy );
    LOCAL_ASSERT( pInternalPolicy );
    TInt avail=pInternalWSPolicy->RemoveMetadataPolicyL(_L8("nsuri"));
    TL(avail == 1);
    CleanupStack::Pop(pInternalPolicy);
    CleanupStack::Pop(pInternalWSPolicy);
    CleanupStack::PopAndDestroy(pParentElement);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    delete pInternalWSPolicy ;
    pInternalWSPolicy = NULL;
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}
TInt CwsPolicyTester::MT_CSenInternalWsPolicy_WsPolicyByUriLL(CStifItemParser& aItem)
{
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParentElement);
    CTestInternalWSPolicy* pInternalWSPolicy = CTestInternalWSPolicy::NewL(_L8("ansuri"),_L8("MetadataEndpoint"),_L8("a:MetadataEndpoint"),attrArray,*pParentElement);
    CleanupStack::PushL(pInternalWSPolicy );
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewL();
    CleanupStack::PushL(pInternalPolicy );
    LOCAL_ASSERT( pInternalPolicy );
    CSenElement* pPolicy =pInternalPolicy->WsPolicyByUriL(_L8(""));
    LOCAL_ASSERT(!pPolicy);
    CleanupStack::Pop(pInternalPolicy);
    CleanupStack::Pop(pInternalWSPolicy);
    CleanupStack::PopAndDestroy(pParentElement);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    delete pInternalWSPolicy ;
    pInternalWSPolicy = NULL;
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}
TInt CwsPolicyTester::MT_CSenInternalWsPolicy_WsPolicyByNameL( CStifItemParser& aItem )
{
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParentElement);
    CTestInternalWSPolicy* pInternalWSPolicy = CTestInternalWSPolicy::NewL(_L8("ansuri"),_L8("MetadataEndpoint"),_L8("a:MetadataEndpoint"),attrArray,*pParentElement);
    CleanupStack::PushL(pInternalWSPolicy );
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewL();
    CleanupStack::PushL(pInternalPolicy );
    LOCAL_ASSERT( pInternalPolicy );
    CSenElement* pPolicy =pInternalPolicy->WsPolicyByName(_L8(""));
    LOCAL_ASSERT(!pPolicy);
    CleanupStack::Pop(pInternalPolicy);
    CleanupStack::Pop(pInternalWSPolicy);
    CleanupStack::PopAndDestroy(pParentElement);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    delete pInternalWSPolicy ;
    pInternalWSPolicy = NULL;
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}
TInt CwsPolicyTester::MT_CSenInternalWsPolicy_ValidUntilLL( CStifItemParser& aItem )
{
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParentElement);
    CTestInternalWSPolicy* pInternalWSPolicy = CTestInternalWSPolicy::NewL(_L8("ansuri"),_L8("MetadataEndpoint"),_L8("a:MetadataEndpoint"),attrArray,*pParentElement);
    CleanupStack::PushL(pInternalWSPolicy );
    CTestInternalWSPolicy* pInternalPolicy = CTestInternalWSPolicy::NewL();
    CleanupStack::PushL(pInternalPolicy );
    LOCAL_ASSERT( pInternalPolicy );
    TTime validTime = pInternalPolicy->ValidUntilL(_L8(""));
    TTime tempTime = Time::NullTTime();
    TL(validTime == tempTime);
    CleanupStack::Pop(pInternalPolicy);
    CleanupStack::Pop(pInternalWSPolicy);
    CleanupStack::PopAndDestroy(pParentElement);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    delete pInternalWSPolicy ;
    pInternalWSPolicy = NULL;
    delete pInternalPolicy ;
    pInternalPolicy = NULL;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_NewLL( CStifItemParser& aItem )
{
    CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewL();
    LOCAL_ASSERT( pPolicyId );
    delete pPolicyId;
    pPolicyId = NULL;
    return KErrNone;
}

TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_NewLCL( CStifItemParser& aItem )
{
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewLC();
  LOCAL_ASSERT( pPolicyId );
  CleanupStack::Pop(pPolicyId);
  delete pPolicyId;
  pPolicyId = NULL;
  return KErrNone;
}

TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_NewL_1L( CStifItemParser& aItem )
{
  RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
  
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewL(_L8("ansuri"),_L8("Nokia"),_L8("a:Nokia"),attrArray);
  LOCAL_ASSERT( pPolicyId );
  CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
  delete pPolicyId ;
  pPolicyId = NULL;
  return KErrNone;
}

TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_NewLC_1L( CStifItemParser& aItem )
{
  RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
  
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewLC(_L8("ansuri"),_L8("Nokia"),_L8("a:Nokia"),attrArray);
  LOCAL_ASSERT( pPolicyId );
  CleanupStack::Pop( pPolicyId );
  CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
  delete pPolicyId ;
  pPolicyId = NULL;
  return KErrNone;
}

TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_NewL_2L( CStifItemParser& aItem )
{
  RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
  CleanupStack::PushL(pParentElement);
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewL(_L8("ansuri"),_L8("Nokia"),_L8("a:Nokia"),attrArray,*pParentElement);
  LOCAL_ASSERT( pPolicyId );
  CleanupStack::PopAndDestroy(pParentElement);
  CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
  delete pPolicyId ;
  pPolicyId = NULL;
  return KErrNone;
}
 
TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_NewLC_2L( CStifItemParser& aItem )
{
  RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
  CleanupStack::PushL(pParentElement);
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewLC(_L8("ansuri"),_L8("Nokia"),_L8("a:Nokia"),attrArray,*pParentElement);
  LOCAL_ASSERT( pPolicyId );
  CleanupStack::Pop(pPolicyId );
  CleanupStack::PopAndDestroy(pParentElement);
  CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
  delete pPolicyId ;
  pPolicyId = NULL;
  return KErrNone;
}

TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_SetPropertyLL( CStifItemParser& aItem )
{
  RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
  CleanupStack::PushL(pParentElement);
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewL(_L8("ansuri"),_L8("Property1"),_L8("a:Nokia"),attrArray,*pParentElement);
  LOCAL_ASSERT( pPolicyId );
  TInt val = pPolicyId->SetPropertyL(_L8("Property1"),_L8("1"));
  TL( val == KErrNone);
  CleanupStack::PopAndDestroy(pParentElement);
  CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
  delete pPolicyId ;
  pPolicyId = NULL;
  return KErrNone;
}

TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_PropertyLL( CStifItemParser& aItem )
{
  RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
  CleanupStack::PushL(pParentElement);
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewL(_L8("ansuri"),_L8("Property1"),_L8("a:Nokia"),attrArray,*pParentElement);
  LOCAL_ASSERT( pPolicyId );
  TInt val = pPolicyId->SetPropertyL(_L8("Property1"),_L8("1"));
  TL( val == KErrNone);
  TPtrC8 value;
  val = pPolicyId->PropertyL(_L8("Property1"),value);
  TL( val == KErrNotFound);
  CleanupStack::PopAndDestroy(pParentElement);
  CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
  delete pPolicyId ;
  pPolicyId = NULL;
  return KErrNone;
}
TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_SetIdLL( CStifItemParser& aItem )
{
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewL();
  LOCAL_ASSERT( pPolicyId );
  TInt val(1);
  TRAPD(res,pPolicyId ->SetIdL(val));
  TL(res == KErrNone);
  delete pPolicyId;
  pPolicyId = NULL;
  return KErrNone;
}

TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_IdLL( CStifItemParser& aItem )
{
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewL();
  LOCAL_ASSERT( pPolicyId );
  TInt val(1);
  TRAPD(res,pPolicyId ->SetIdL(val));
  TL(res  == KErrNone);
  TInt id=pPolicyId ->IdL();
  TL(id == val);
  delete pPolicyId;
  pPolicyId = NULL;
  return KErrNone;
}
TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_AddMetadataEndpointLL( CStifItemParser& aItem )
{
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewL();
  LOCAL_ASSERT( pPolicyId );
  CSenElement* pElement = CSenBaseElement::NewL(_L8("MetadataElement"));
  CleanupStack::PushL(pElement);
  pElement->SetContentL(_L8("metacontent"));
  TInt retVal = pPolicyId ->AddMetadataEndpointL( pElement);
  TL(retVal == KErrNone);
  CleanupStack::PopAndDestroy(pElement);

  delete pPolicyId;
  pPolicyId = NULL;
  return KErrNone;
}
TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_MetadataEndpointLL( CStifItemParser& aItem )      
{
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewL();
  LOCAL_ASSERT( pPolicyId );
  CSenElement* pElement = CSenBaseElement::NewL(_L8("MetadataElement"));
  CleanupStack::PushL(pElement);
  pElement->SetContentL(_L8("metacontent"));
  TInt retVal = pPolicyId ->AddMetadataEndpointL( pElement);
  TL (retVal == KErrNone);
  
  //HBufC8* pPolXML = (pPolicyId->AsElement()).AsXmlL();
  //fL.Write(*pPolXML);
  
  CleanupStack::PopAndDestroy(pElement);
  TPtrC8 metaEndPoint = pPolicyId ->MetadataEndpointL();
  TL(metaEndPoint == KNullDesC8);
  delete pPolicyId;
  pPolicyId = NULL;
  //delete pPolXML ;
  //pPolXML = NULL;
  return KErrNone;
}
TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_MetadataEndpointElementLL( CStifItemParser& aItem )        
{
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewL();
  LOCAL_ASSERT( pPolicyId );
  CSenElement* pElement = CSenBaseElement::NewL(_L8("MetadataElement"));
  CleanupStack::PushL(pElement);
  pElement->SetContentL(_L8("metacontent"));
  TInt retVal = pPolicyId ->AddMetadataEndpointL( pElement);
  TL(retVal == KErrNone);
  CleanupStack::PopAndDestroy(pElement);
  CSenElement* pMetaElement = pPolicyId->MetadataEndpointElementL();
  LOCAL_ASSERT(!pMetaElement);
  delete pPolicyId;
  pPolicyId = NULL;
  if(pMetaElement)
  {
    delete pMetaElement;
    pMetaElement = NULL;
  }
  return KErrNone;
}
TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_ElementAttribValueLL( CStifItemParser& aItem )
{

  RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
  CleanupStack::PushL(pParentElement);
  CSenElement* pElement = CSenBaseElement::NewL(_L8("ansuri"),_L8("Property1"),_L8("a:Nokia"),attrArray,*pParentElement);
  CleanupStack::PushL(pElement);
  
//  HBufC8* elementBuf = pElement->AsXmlL();
//  fL.Write(*elementBuf);
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewL();
  LOCAL_ASSERT( pPolicyId );
    
  
  TPtrC8 val = pPolicyId->ElementAttribValueL(pElement ,_L8("pr:LocalName"));
  TL( val  == _L8("Value"));
  CleanupStack::PopAndDestroy(pElement);
  CleanupStack::PopAndDestroy(pParentElement);
  CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
  delete pPolicyId ;
  pPolicyId = NULL;
  return KErrNone;
}
TInt CwsPolicyTester::MT_CSenWSPolicyIdentifier_HasAttributeLL( CStifItemParser& aItem )
{

  RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("LocalName"));
    CleanupClosePushL(localNameRString);
    RString valueRString        = stringPool.OpenStringL(_L8("Value"));
    CleanupClosePushL(valueRString);
    
    RAttribute attribute;
    attribute.Open(nsUriRString, nsPrefixRString, localNameRString, 
                   valueRString);
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
    
    CSenElement* pParentElement = CSenBaseElement::NewL(_L8("Parent"));
  CleanupStack::PushL(pParentElement);
  CSenElement* pElement = CSenBaseElement::NewL(_L8("ansuri"),_L8("Property1"),_L8("a:Nokia"),attrArray,*pParentElement);
  CleanupStack::PushL(pElement);
  
//  HBufC8* elementBuf = pElement->AsXmlL();
//  fL.Write(*elementBuf);
  CTestWSPolicyIdentifier* pPolicyId = CTestWSPolicyIdentifier::NewL();
  LOCAL_ASSERT( pPolicyId );
    
  
  TBool has= pPolicyId->HasAttributeL(pElement ,_L8("pr:LocalName"));
  LOCAL_ASSERT( has );
  CleanupStack::PopAndDestroy(pElement);
  CleanupStack::PopAndDestroy(pParentElement);
  CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
  delete pPolicyId ;
  pPolicyId = NULL;
  return KErrNone;
}

TInt CwsPolicyTester::MT_CWSPolicyWriter_NewLL( CStifItemParser& aItem )
{
  CTestPolicyWriter* pPolicyWriter = CTestPolicyWriter::NewL();
  LOCAL_ASSERT( pPolicyWriter );
  delete pPolicyWriter;
  pPolicyWriter = NULL;
  return KErrNone;
}

TInt CwsPolicyTester::MT_CWSPolicyWriter_NewLCL( CStifItemParser& aItem )
{
  CTestPolicyWriter* pPolicyWriter = CTestPolicyWriter::NewLC();
  LOCAL_ASSERT( pPolicyWriter );
  CleanupStack::Pop(pPolicyWriter);
  delete pPolicyWriter;
  pPolicyWriter = NULL;
  return KErrNone;
}

TInt CwsPolicyTester::MT_CWSPolicyReader_NewLL( CStifItemParser& aItem )
{
  CTestPolicyReader* pPolicyReader = CTestPolicyReader::NewL();
  LOCAL_ASSERT( pPolicyReader );
  delete pPolicyReader ;
  pPolicyReader = NULL;
  return KErrNone;
}

TInt CwsPolicyTester::MT_CWSPolicyReader_NewLCL( CStifItemParser& aItem )
{
  CTestPolicyReader* pPolicyReader = CTestPolicyReader::NewLC();
  LOCAL_ASSERT( pPolicyReader );
  CleanupStack::Pop(pPolicyReader);
  delete pPolicyReader ;
  pPolicyReader = NULL;
  return KErrNone;
}
TInt CwsPolicyTester::MT_CWSPolicyReader_GetPolicyLL( CStifItemParser& aItem )
{
  CTestPolicyReader* pPolicyReader = CTestPolicyReader::NewLC();
  LOCAL_ASSERT( pPolicyReader );
  
  CSenElement* pPolicy = CSenBaseElement::NewL(_L8("http://schemas.xmlsoap.org/ws/2004/09/policy"),_L8("Policy"),_L8("wsp:Policy"));
  CleanupStack::PushL(pPolicy);
  pPolicy->AddAttrL(_L8("wsu:Id"),_L8("ID1"));
  CPolicyAssertion* pPolicyAsserton = pPolicyReader->GetPolicyL(pPolicy);
  LOCAL_ASSERT( pPolicyAsserton );
  CleanupStack::PopAndDestroy(pPolicy);
  CleanupStack::Pop(pPolicyReader);
  delete pPolicyReader ;
  pPolicyReader = NULL;
  delete pPolicyAsserton;
  pPolicyAsserton = NULL;
  return KErrNone;
}

TInt CwsPolicyTester::MT_CWSPolicyWriter_WriteToFileAllLL( CStifItemParser& aItem )
{
  CTestPolicyWriter* pPolicyWriter = CTestPolicyWriter::NewLC();
  LOCAL_ASSERT( pPolicyWriter );
  
  CSenElement* pPolicy = CSenBaseElement::NewL(_L8("http://schemas.xmlsoap.org/ws/2004/09/policy"),_L8("Policy"),_L8("wsp:Policy"));
  CleanupStack::PushL(pPolicy);
  pPolicy->AddAttrL(_L8("wsu:Id"),_L8("ID1"));
  TRAPD(res,pPolicyWriter->WriteToFileAllL(pPolicy));
  TL(res == KErrNone);
  CleanupStack::PopAndDestroy(pPolicy);
  CleanupStack::Pop(pPolicyWriter);
  delete pPolicyWriter ;
  pPolicyWriter = NULL;
  return KErrNone;
}
TInt CwsPolicyTester::MT_CWSPolicyWriter_WriteToFileAllL_1L( CStifItemParser& aItem )
{
  _LIT8(KXml,"<Parent><t:Test xmlns:t=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/></Parent>");
  TBuf8<100> XmlBuf(KXml);
  CTestPolicyWriter* pPolicyWriter = CTestPolicyWriter::NewLC();
  LOCAL_ASSERT( pPolicyWriter );
  TRAPD(res,pPolicyWriter->WriteToFileAllL(XmlBuf));
  TL(res == KErrNone);
  delete pPolicyWriter ;
  pPolicyWriter = NULL;
  return KErrNone; 
}
TInt CwsPolicyTester::MT_CWSPolicyWriter_WriteAllAsXmlLL( CStifItemParser& aItem )         
{
/*  CPolicyAssertion* pPolicyAssertion = CTestPolicyAssertion::NewL();
  CleanupStack::PushL(pPolicyAssertion );
  CTestPolicyWriter* pPolicyWriter = CTestPolicyWriter::NewLC();
  LOCAL_ASSERT( pPolicyWriter );
  HBufC8* pPolicyBuf = pPolicyWriter->WriteAllAsXmlL(pPolicyAssertion);
  fL.Write(*pPolicyBuf );
  CleanupStack::PopAndDestroy( pPolicyAssertion );
  delete pPolicyWriter ;
  pPolicyWriter = NULL;
  delete pPolicyBuf ;
  pPolicyBuf = NULL;*/
  return KErrNone;
}
//  TEST TABLE


// -----------------------------------------------------------------------------
// CwsPolicyTester::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CwsPolicyTester::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
