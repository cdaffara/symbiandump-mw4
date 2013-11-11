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
#include "senfragmentbctest.h"
#include "maindomfragment.h"
#include "mainfragment.h"
#include "TestFragment.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSenFragmentBCTest::Case
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
const TCaseInfo CSenFragmentBCTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    SenFragment.cpp file and to SenFragment.h 
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
        
        ENTRY( "NewL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_NewLL ),
        ENTRY( "NewL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_1L ),
        ENTRY( "NewL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_2L ),
        ENTRY( "NewL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_3L ),
        ENTRY( "NewL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_4L ),
        ENTRY( "NewL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_5L ),
        ENTRY( "NewL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_6L ),
        ENTRY( "NewL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_7L ),
        ENTRY( "AddAttributesL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_AddAttributesLL ),
        ENTRY( "AsXmlL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_AsXmlLL ),
        ENTRY( "ContentL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_ContentLL ),
        ENTRY( "Namespace - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_NamespaceL ),
        ENTRY( "ResetContentL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_ResetContentLL ),
        ENTRY( "LocalName - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_LocalNameL ),
        ENTRY( "NsUri - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_NsUriL ),
        ENTRY( "NsPrefix - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_NsPrefixL ),
        ENTRY( "AsElementL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_AsElementLL ),
        ENTRY( "AsDocumentL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_AsDocumentLL ),
        ENTRY( "ExtractElement - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_ExtractElementL ),
        ENTRY( "AsXmlUnicodeL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_AsXmlUnicodeLL ),
        ENTRY( "WriteAsXMLToL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_WriteAsXMLToLL ),
        ENTRY( "ConsistsOfL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_ConsistsOfLL ),
        ENTRY( "Parsing1 - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing1L ),
        ENTRY( "Parsing2 - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing2L ),
        ENTRY( "Parsing3 - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing3L ),
        ENTRY( "Parsing4 - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing4L ),
        ENTRY( "Parsing5 - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing5L ),
        ENTRY( "Parsing6 - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing6L ),
        ENTRY( "Parsing7 - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing7L ),
        ENTRY( "Parsing8 - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing8L ),
        //ENTRY( "SetContentOfL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_SetContentOfLL ),
        //ENTRY( "ContentOf - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_ContentOfL ),
        ENTRY( "OnWriteStartElementL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_OnWriteStartElementLL ),
        ENTRY( "OnWriteEndElementL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_OnWriteEndElementLL ),
        ENTRY( "OnIgnorableWhiteSpaceL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_OnIgnorableWhiteSpaceLL ),
        ENTRY( "OnSkippedEntityL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_OnSkippedEntityLL ),
        ENTRY( "OnProcessingInstructionL - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_OnProcessingInstructionLL ),
        ENTRY( "OnError - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_OnErrorL ),
        ENTRY( "GetExtendedInterface - CSenDomFragmentBase", CSenFragmentBCTest::MT_CSenDomFragmentBase_GetExtendedInterfaceL ),
        ENTRY( "NewL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_NewLL ),
        ENTRY( "NewL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_NewL_2L ),
        ENTRY( "NewL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_NewL_3L ),
        ENTRY( "NewL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_NewL_4L ),
        ENTRY( "NewL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_NewL_5L ),
        ENTRY( "NewL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_NewL_6L ),
        ENTRY( "NewL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_NewL_7L ),
        ENTRY( "SetDocument - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_SetDocumentL ),
        
        ENTRY( "ContentL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_ContentLL ),
        ENTRY( "Namespace - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_NamespaceL ),
        ENTRY( "ResetContentL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_ResetContentLL ),
        ENTRY( "LocalName - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_LocalNameL ),
        ENTRY( "NsUri - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_NsUriL ),
        ENTRY( "NsPrefix - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_NsPrefixL ),
        ENTRY( "AsElementL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_AsElementLL ),
        ENTRY( "AsDocumentL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_AsDocumentLL ),
        ENTRY( "ExtractElement - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_ExtractElementL ),
        ENTRY( "AsXmlUnicodeL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_AsXmlUnicodeLL ),
        ENTRY( "AsXmlL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_AsXmlLL ),
        ENTRY( "WriteAsXMLToL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_WriteAsXMLToLL ),
        ENTRY( "ConsistsOfL - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_ConsistsOfLL ),
        ENTRY( "Parsing1 - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_Parsing1L ),
        ENTRY( "Parsing2 - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_Parsing2L ),
        ENTRY( "Parsing3 - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_Parsing3L ),
        ENTRY( "Parsing4 - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_Parsing4L ),
        ENTRY( "Parsing5 - CSenFragmentBase", CSenFragmentBCTest::MT_CSenFragmentBase_Parsing5L ),
        ENTRY( "NewL - CSenParser", CSenFragmentBCTest::MT_CSenParser_NewLL ),
        ENTRY( "NewLC - CSenParser", CSenFragmentBCTest::MT_CSenParser_NewLCL ),
        ENTRY( "NewL - CSenParser", CSenFragmentBCTest::MT_CSenParser_NewL_1L ),
        ENTRY( "NewLC - CSenParser", CSenFragmentBCTest::MT_CSenParser_NewLC_1L ),
        ENTRY( "NewL - CSenParser", CSenFragmentBCTest::MT_CSenParser_NewL_2L ),
        ENTRY( "NewLC - CSenParser", CSenFragmentBCTest::MT_CSenParser_NewLC_2L ),
//        ENTRY( "NewL - CSenParser", CSenFragmentBCTest::MT_CSenParser_NewL_3L ),
//        ENTRY( "NewLC - CSenParser", CSenFragmentBCTest::MT_CSenParser_NewLC_3L ),
        ENTRY( "NewL - RSenDocument", CSenFragmentBCTest::MT_RSenDocument_NewLL ),
        ENTRY( "NewLC - RSenDocument", CSenFragmentBCTest::MT_RSenDocument_NewLCL ),
        ENTRY( "NewL - RSenDocument", CSenFragmentBCTest::MT_RSenDocument_NewL_1L ),
        ENTRY( "NewLC - RSenDocument", CSenFragmentBCTest::MT_RSenDocument_NewLC_1L ),
        ENTRY( "Copy - RSenDocument", CSenFragmentBCTest::MT_RSenDocument_CopyL ),
        ENTRY( "Close - RSenDocument", CSenFragmentBCTest::MT_RSenDocument_CloseL ),
        ENTRY( "Destroy - RSenDocument", CSenFragmentBCTest::MT_RSenDocument_DestroyL ),
        ENTRY( "Destroy - RSenDocument", CSenFragmentBCTest::MT_RSenDocument_DestroyL ),
        
        ENTRY( "ManualXmlEngineTlsAttachL - RSenDocument", CSenFragmentBCTest::MT_RSenDocument_ManualXmlEngineTlsAttachLL ),
        ENTRY( "ManualXmlEngineTlsAttachL - RSenDocument", CSenFragmentBCTest::MT_RSenDocument_ManualXmlEngineTlsCleanupL ),
        ENTRY( "SetContentL - TestFragmentBase", CSenFragmentBCTest::MT_TestFragmentBase_SetContentLL ),
        ENTRY( "AddContentL - TestFragmentBase", CSenFragmentBCTest::MT_TestFragmentBase_AddContentLL ),
        ENTRY( "SetContentOfL - TestDomFragmentBase", CSenFragmentBCTest::MT_TestDomFragmentBase_SetContentOfLL ),
        //ENTRY( "ContentOf - TestDomFragmentBase", CSenFragmentBCTest::MT_TestDomFragmentBase_ContentOfL ),
        
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", CSenFragmentBCTest::LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( CSenFragmentBCTest::PrintTest, ETrue, 1, 3 ),
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

void CSenFragmentBCTest::SetupL(  ){
    if ( iXmlReader ){
        delete iXmlReader;
        iXmlReader = NULL;
    }
    iXmlReader = CSenXmlReader::NewL();
} 

void CSenFragmentBCTest::Teardown(  ){
    if ( iXmlReader){
        delete iXmlReader;
        iXmlReader = NULL;
    }
}
    
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_NewLL(TTestResult& aResult)
	{
    SetupL();
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL();
    TL(pFragment != (CSenDomFragmentBase*)NULL);
    __ASSERT_ALWAYS_NO_LEAVE(delete pFragment);
    pFragment = NULL;
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_1L(TTestResult& aResult)
	{
    SetupL();
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement element = document.CreateDocumentElementL(_L8("Test"));
    element.SetEscapedTextL(_L8("<Element/>"));
    
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(element);
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    // Test that when CSenDomFragmentBase is serialized '<' and '>' characters
    // are encoded (as all basic entities in content should be).
    if(!( *pAsXml == _L8("<Test>&lt;Element/&gt;</Test>") ))return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::Pop(1);
    //CleanupStack::PopAndDestroy(&document);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_2L(TTestResult& aResult)
	{
    SetupL();	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<Test/>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_3L(TTestResult& aResult)
	{
    SetupL();	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"));
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<Test xmlns=\"NsUri\"/>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_4L(TTestResult& aResult)
	{
    SetupL();	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<t:Test xmlns:t=\"NsUri\"/>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_5L(TTestResult& aResult)
	{
    SetupL();	
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
    attribute.Open( nsUriRString, nsPrefixRString, localNameRString,
                    valueRString );
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
    
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"),
                                                               attrArray);
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<t:Test xmlns:t=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_6L(TTestResult& aResult)
	{
    SetupL();	
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement parentElement = document.CreateDocumentElementL(_L8("Parent"));
    
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
        
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"),
                                                               attrArray,
                                                               parentElement);
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<t:Test xmlns:t=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") )) return KErrArgument;

    RBuf8 buffer;
    parentElement.OuterXmlL(buffer);
    
    if(!( buffer == _L8("<Parent><t:Test xmlns:t=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/></Parent>") )) return KErrArgument;
    buffer.Close();
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    CleanupStack::PopAndDestroy(&document);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_NewL_7L(TTestResult& aResult)
	{
    SetupL();	
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement parentElement = document.CreateDocumentElementL(_L8("Parent"));
    
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
        
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8(""),
                                                               attrArray,
                                                               parentElement,
                                                               document);
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<Test xmlns=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") )) return KErrArgument;

    RBuf8 buffer;
    parentElement.OuterXmlL(buffer);
    
    if(!( buffer == _L8("<Parent><Test xmlns=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/></Parent>") )) return KErrArgument;
    buffer.Close();
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    CleanupStack::PopAndDestroy(&document);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_AddAttributesLL(TTestResult& aResult)
	{
    SetupL();	
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
    // attribute took ownership of all RStrings
    // => All RStrings can be pop from CleanupStack
    CleanupStack::Pop(4); // nsUriRString, nsPrefixRString, localNameRString,
                          // valueRString
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    // append the namespace attribute (declaration)
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray);    
    
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    
    pFragment->AddAttributesL(attrArray);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<Test xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pFragment);
    
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_AsXmlLL(TTestResult& aResult)
	{
    SetupL();	
    // Test serialization of Dom tree which has two child elements
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement element2 = element.AddNewElementL(_L8("Child"), _L8("nsuri"), _L8("prefix"));

    element2.AddNewElementL(_L8("Child2"), _L8("nsuri2"), _L8("prefix2"));
    
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<Test xmlns:prefix=\"nsuri\"><prefix:Child xmlns:prefix2=\"nsuri2\"><prefix2:Child2/></prefix:Child></Test>") ) ) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_ContentLL(TTestResult& aResult)
	{
    SetupL();	
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement element = document.CreateDocumentElementL(_L8("Test"));
    element.SetEscapedTextL(_L8("<Element/>"));
    
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(element);
    CleanupStack::PushL(pFragment);
    
    TPtrC8 content = pFragment->ContentL();
    if(!( content == _L8("<Element/>") ) ) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&document);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_NamespaceL(TTestResult& aResult)
	{
    SetupL();	
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
    attribute.Open(stringPool.OpenStringL(_L8("nsuri")), 
                   stringPool.OpenStringL(_L8("pr")),
                   stringPool.OpenStringL(_L8("LocalName")),
                   stringPool.OpenStringL(_L8("Value")) );
    // attribute took copies of all RStrings
    // => All RStrings can be destroyed
    CleanupStack::PopAndDestroy(4); // nsUriRString, nsPrefixRString, localNameRString,
                                    // valueRString
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    // append the namespace attribute (declaration)
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray);
    
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"),
                                                               attrArray);
    CleanupStack::PushL(pFragment);
    
    TXmlEngNamespace ns = pFragment->Namespace(_L8("pr"));
    if(!( ns.Uri() == _L8("nsuri") ) ) return KErrArgument;

    TXmlEngNamespace ns2 = pFragment->Namespace(_L8("x"));
    if(!( ns2.IsNull() ) ) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(); // attribute
    CleanupStack::PopAndDestroy(); // attrArray
    CleanupStack::PopAndDestroy(); // stringPool   
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_ResetContentLL(TTestResult& aResult)
	{
    SetupL();	
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement element = document.CreateDocumentElementL(_L8("Test"));
    element.SetEscapedTextL(_L8("Content"));
    
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(element);
    CleanupStack::PushL(pFragment);
    
    // Test that CSenFragmentBase has content before reset
    TPtrC8 content = pFragment->ContentL();
    if(!( content == _L8("Content") )) return KErrArgument;
    
    pFragment->ResetContentL();
    
    // Test that CSenFragmentBase has no content after reset
    TPtrC8 content2 = pFragment->ContentL();
    if(!( content2 == KNullDesC8 )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&document);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_LocalNameL(TTestResult& aResult)
	{
    SetupL();	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    TPtrC8 localName = pFragment->LocalName();
    if(!( localName == _L8("Test") ) ) return KErrArgument;
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_NsUriL(TTestResult& aResult)
	{
    SetupL();	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    TPtrC8 localName = pFragment->NsUri();
    if(!( localName == _L8("NsUri") ) ) return KErrArgument;
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_NsPrefixL(TTestResult& aResult)
	{
    SetupL();	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    TPtrC8 localName = pFragment->NsPrefix();
    if(!( localName == _L8("t") ) ) return KErrArgument;
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_AsElementLL(TTestResult& aResult)
	{
    SetupL();	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    RBuf8 buffer;
    CleanupClosePushL(buffer);
    element.OuterXmlL(buffer);
    // Serialized element should contain all the Fragment data as XML.
    if(!( buffer == _L8("<t:Test xmlns:t=\"NsUri\"/>") ) ) return KErrArgument;
    CleanupStack::PopAndDestroy(&buffer);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_AsDocumentLL(TTestResult& aResult)
	{
    SetupL();	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    RSenDocument document = pFragment->AsDocumentL();
    TXmlEngElement element = document.DocumentElement();
    
    TXmlEngSerializationOptions options;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    options.iOptions = options.iOptions |
                       TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;
    RBuf8 asXml;
    document.SaveL(asXml, element, options);
    CleanupClosePushL(asXml);
    
    // Serialized document should contain all the Fragment data as XML.
    if(!( asXml == _L8("<t:Test xmlns:t=\"NsUri\"/>") ) ) return KErrArgument;

    CleanupStack::PopAndDestroy(&asXml);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_ExtractElementL(TTestResult& aResult)
	{
    SetupL();	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->ExtractElement();
    if(!( element.Name() == _L8("Test") )) return KErrArgument;
    element.Remove();
    CleanupStack::PopAndDestroy(pFragment);   
    Teardown();
    return KErrNone;
	}
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_AsXmlUnicodeLL(TTestResult& aResult)
	{
    SetupL();	
    // Test serialization of Dom tree which has two child elements
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement element2 = element.AddNewElementL(_L8("Child"), _L8("nsuri"), _L8("prefix"));

    element2.AddNewElementL(_L8("Child2"), _L8("nsuri2"), _L8("prefix2"));
    
    HBufC* pAsXml = pFragment->AsXmlUnicodeL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L("<Test xmlns:prefix=\"nsuri\"><prefix:Child xmlns:prefix2=\"nsuri2\"><prefix2:Child2/></prefix:Child></Test>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_WriteAsXMLToLL(TTestResult& aResult)
	{
    SetupL();	
    // Test serialization of Dom tree which has two child elements
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement element2 = element.AddNewElementL(_L8("Child"), _L8("nsuri"), _L8("prefix"));

    element2.AddNewElementL(_L8("Child2"), _L8("nsuri2"), _L8("prefix2"));
    
    CBufFlat* pBuf = CBufFlat::NewL(200);
    CleanupStack::PushL(pBuf);
    RBufWriteStream bufWs(*pBuf);
    CleanupClosePushL(bufWs);
    pFragment->WriteAsXMLToL(bufWs);


    if(!( pBuf->Ptr(0) == _L8("<Test xmlns:prefix=\"nsuri\"><prefix:Child xmlns:prefix2=\"nsuri2\"><prefix2:Child2/></prefix:Child></Test>") )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(&bufWs);
    CleanupStack::PopAndDestroy(pBuf);
    
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_ConsistsOfLL(TTestResult& aResult)
	{
    SetupL();	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement elementchild1 = element.AddNewElementL(_L8("DirectChild"));
    elementchild1.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
   
    //TDomUtils domUtils1;
    //domUtils1.XmlEngRenameElementL(elementchild1, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));

    TXmlEngElement elementchild2 = element.AddNewElementL(_L8("DirectChild"));
    elementchild2.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //TDomUtils domUtils2;
    //domUtils2.XmlEngRenameElementL(elementchild2, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    elementchild2.AddNewElementL(_L8("Child"), _L8("nsuri2"), _L8("prefix2"));

    CSenDomFragmentBase* pFragment2 = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment2);
    TXmlEngElement element2 = pFragment2->AsElementL();
    
    TXmlEngElement elementchild22 = element2.AddNewElementL(_L8("DirectChild"));
    elementchild22.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //TDomUtils domUtils22;
    //domUtils22.XmlEngRenameElementL(elementchild22, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    elementchild22.AddNewElementL(_L8("Child"), _L8("nsuri2"), _L8("prefix2"));

    // ConsistOfL should return ETrue because
    // root elements of both fragments match and the only element
    // of pFragment2 = elementchild22 is exactly the same element as
    // elementchild2.
    if(!( pFragment->ConsistsOfL(*pFragment2) )) return KErrArgument;

    // ConsistOfL should return EFalse because
    // Even though root elements of both fragments match
    // there is extra element (= elementchild1) in pFragment.
    //
    // elementchild1 (of pFragment) can not be found from pFragment2
    // and because of that pFragment2 does not consist of pFragment.
    if(!( !pFragment2->ConsistsOfL(*pFragment) )) return KErrArgument;
    CleanupStack::PopAndDestroy(pFragment2);

    CSenDomFragmentBase* pFragment3 = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment3);
    TXmlEngElement element3 = pFragment3->AsElementL();
    
    TXmlEngElement elementchild32 = element3.AddNewElementL(_L8("DirectChild"));
    elementchild32.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //TDomUtils domUtils32;
    //domUtils32.XmlEngRenameElementL(elementchild32, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    elementchild32.AddNewElementL(_L8("Child"), _L8("nsuri2"), _L8("prefix2"));
    elementchild32.SetEscapedTextL(_L8("Text"));
    
    // ConsistOfL should return EFalse because
    // elementchild32 has content ("Text") which doesn't match
    // to any child element of pFragment.
    if(!( !pFragment->ConsistsOfL(*pFragment3) )) return KErrArgument;
    CleanupStack::PopAndDestroy(pFragment3);
    
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing1L(TTestResult& aResult)
	{
    SetupL();	
    // Parsing of XML document to CSenDomFragmentBase
    _LIT8(KInputString, "<a:Test xmlns=\"nsuri\" xmlns:a=\"nasuria\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value3\">Content1</Element1>\
<Element2 z=\"value4\">Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");

    // Default namespace declaration (xmlns="nsuri") from Parent element (Test)
    // will be moved to all child elements (Element1, Element2, Element3 and
    // Element4) when EReportNamespaceMapping is _NOT_ enabled. That's because
    // the way CParser works <=> Namespaces are reported when namespaces are used
    // for the first time.
    _LIT8(KOutputString1, "<a:Test xmlns:a=\"nasuria\" xmlns:b=\"nasurib\" \
a:x=\"value1\" b:z=\"value2\">\
<Element1 xmlns=\"nsuri\" xmlns:c=\"nsuric\" c:x=\"value3\">Content1</Element1>\
<Element2 xmlns=\"nsuri\" z=\"value4\">Content2</Element2>\
<Element3 xmlns=\"nsuri\">Content3</Element3>\
<Element4 xmlns=\"nsuri\"/></a:Test>");

    // When EReportNamespaceMapping is enabled all the namespace declarations
    // are reported in exactly the same element as they are defined in parsed
    // XML document.
    // Note: The Order of namespace declarations in root element tag may change.
    // Namespace declaration for root tag is created when fragment is created
    // and because of that namespace declaration for root tag will be the first.
    // In this particular case namespace declaration for prefix "a" will be
    // the first namespace declaration because it is declared when
    // CSenDomFragmentBase is created.
    _LIT8(KOutputString2, "<a:Test xmlns:a=\"nasuria\" xmlns=\"nsuri\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value3\">Content1</Element1>\
<Element2 z=\"value4\">Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></a:Test>"); 
    
	CSenParser* pParser = CSenParser::NewLC();
	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment);
	
	pParser->ParseL(KInputString, *pFragment);
	
	HBufC8* pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);

    if(!( *pAsXml == KOutputString1 )) return KErrArgument;

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
/*
	pParser->EnableFeature(EReportNamespaceMapping);
	//tma
	pParser->DisableFeature(EReportNamespaceMapping);
	TBool fEneabled(EFalse);
	fEneabled = pParser->IsFeatureEnabled(EReportNamespaceMapping);
	EUNIT_ASSERT( fEneabled == EFalse);
	pParser->EnableFeature(EReportNamespaceMapping);
	//tma
*/	
	pParser->EnableFeature(EReportNamespaceMapping);
	
    CSenDomFragmentBase* pFragment2 = CSenDomFragmentBase::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment2);

	pParser->ParseL(KInputString, *pFragment2);
	
	HBufC8* pAsXml2 = pFragment2->AsXmlL();
	CleanupStack::PushL(pAsXml2);
	                   
    if(!( *pAsXml2 == KOutputString2 )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pAsXml2);
    CleanupStack::PopAndDestroy(pFragment2);

    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;
	}


TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing2L(TTestResult& aResult)
	{
    SetupL();	
    // Parsing of XML document to CSenDomFragmentBase which uses
    // delegation.
    _LIT8(KInputString, "<a:Test xmlns:a=\"nasuria\" xmlns:b=\"nsurib\" \
xmlns=\"nsuri\" a:x=\"value1\" b:z=\"value2\">\
<DelegateFragment>DelegateContent</DelegateFragment>\
<Element1 x=\"value4\">Content1</Element1>\
<Element2>Content2</Element2></a:Test>");

    // Default namespace declaration (xmlns="nsuri") from Parent element (Test)
    // will be moved to all child elements (Element1, Element2, Element3 and
    // Element4) when EReportNamespaceMapping is _NOT_ enabled. That's because
    // the way CParser works <=> Namespaces are reported when namespaces are used
    // for the first time.
    _LIT8(KOutputString1, "<a:Test xmlns:a=\"nasuria\" xmlns:b=\"nsurib\" \
a:x=\"value1\" b:z=\"value2\">\
<DelegateFragment xmlns=\"nsuri\">DelegateContent</DelegateFragment>\
<Element1 xmlns=\"nsuri\" x=\"value4\">Content1</Element1>\
<Element2 xmlns=\"nsuri\">Content2</Element2></a:Test>");

    // When EReportNamespaceMapping is enabled all the namespace declarations
    // are reported in exactly the same element as they are defined in parsed
    // XML document.
    // Note: The Order of namespace declarations in root element tag may change.
    // Namespace declaration for root tag is created when fragment is created
    // and because of that namespace declaration for root tag will be the first.
    _LIT8(KOutputString2, "<a:Test xmlns:a=\"nasuria\" xmlns:b=\"nsurib\" \
xmlns=\"nsuri\" a:x=\"value1\" b:z=\"value2\">\
<DelegateFragment>DelegateContent</DelegateFragment>\
<Element1 x=\"value4\">Content1</Element1>\
<Element2>Content2</Element2></a:Test>");
     
	CSenParser* pParser = CSenParser::NewLC();
	
    CMainDomFragment* pFragment = CMainDomFragment::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment);
	
	pParser->ParseL(KInputString, *pFragment);
	
	HBufC8* pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KOutputString1 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    
    CDelegateDomFragment& delegate = pFragment->DelegateFragment();
    HBufC8* pDelegateAsXml = delegate.AsXmlL();
    CleanupStack::PushL(pDelegateAsXml);
    // Note: When EReportNamespaceMapping is _NOT_ enabled namespace declaration
    //       is added to delegate even though that namespace declaration was
    //       originally declared (but not used) in root element of parsed document. 
    if(!( *pDelegateAsXml ==
                 _L8("<DelegateFragment xmlns=\"nsuri\">DelegateContent</DelegateFragment>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pDelegateAsXml);
    
    CleanupStack::PopAndDestroy(pFragment);

	pParser->EnableFeature(EReportNamespaceMapping);
	
    CMainDomFragment* pFragment2 = CMainDomFragment::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment2);

	pParser->ParseL(KInputString, *pFragment2);
	
	HBufC8* pAsXml2 = pFragment2->AsXmlL();
	CleanupStack::PushL(pAsXml2);
    if(!( *pAsXml2 == KOutputString2 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml2);
    
    CDelegateDomFragment& delegate2 = pFragment2->DelegateFragment();
    HBufC8* pDelegateAsXml2 = delegate2.AsXmlL();
    CleanupStack::PushL(pDelegateAsXml2);
    if(!( *pDelegateAsXml2 ==
                 _L8("<DelegateFragment>DelegateContent</DelegateFragment>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pDelegateAsXml2);
    
    CleanupStack::PopAndDestroy(pFragment2);

    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;
	}
         
         
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing3L(TTestResult& aResult)
	{
    SetupL();	
    // Parsing of XML document to CSenDomFragmentBase
    // In this test same namespaceuri is defined as default namespace in
    // root element and 'c' prefixed namespace in Element2.
    _LIT8(KInputString, "<a:Test xmlns=\"nsuri\" xmlns:a=\"nasuria\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\" z=\"value3\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value4\">Content1</Element1>\
<x:Element2 xmlns:x=\"nsuri\">Content2</x:Element2>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");

    // Namespace declaration for prefix 'x' (and namespaceuri "nsuri")
    // disappears from Element2 after parsing because default namespace
    // declaration already declares namespace for uri "nsuri".
    // => Double declaration for namespaceuri "nsuri" is not needed.
    _LIT8(KOutputString, "<a:Test xmlns:a=\"nasuria\" xmlns=\"nsuri\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\" z=\"value3\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value4\">Content1</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");
    
	CSenParser* pParser = CSenParser::NewLC();
	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment);
	pParser->EnableFeature(EReportNamespaceMapping);
	
	pParser->ParseL(KInputString, *pFragment);
	
	HBufC8* pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);

    if(!( *pAsXml == KOutputString )) return KErrArgument;

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing4L(TTestResult& aResult)
	{
    SetupL();	
    // Parsing of XML document to CSenDomFragmentBase
    // In this test nested elements (which have same name) are tested.
    _LIT8(KInputString, "<a:Test xmlns=\"nsuri\" xmlns:a=\"nasuria\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\" z=\"value3\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value4\">\
<Element1>Content</Element1>\
</Element1>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");

    _LIT8(KInputString2, "<Nested xmlns=\"nsuri\">\
<Nested attr=\"attrvalue\"><Nested><Nested>Content</Nested></Nested></Nested>\
</Nested>");

    // Only namespace declaration order changes for output.
    _LIT8(KOutputString, "<a:Test xmlns:a=\"nasuria\" xmlns=\"nsuri\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\" z=\"value3\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value4\">\
<Element1>Content</Element1>\
</Element1>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");

	CSenParser* pParser = CSenParser::NewLC();
	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment);
	pParser->EnableFeature(EReportNamespaceMapping);
	
	pParser->ParseL(KInputString, *pFragment);
	
	HBufC8* pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);

    if(!( *pAsXml == KOutputString )) return KErrArgument;

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);

    pFragment = CSenDomFragmentBase::NewL(_L8("nsuri"),_L8("Nested"));
    CleanupStack::PushL(pFragment);
	pParser->EnableFeature(EReportNamespaceMapping);
	
	pParser->ParseL(KInputString2, *pFragment);
	
	pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);

    if(!( *pAsXml == KInputString2 )) return KErrArgument;

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);

    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing5L(TTestResult& aResult)
	{
    SetupL();	
    // Parsing of XML document to CSenDomFragmentBase
    // In this test nulling of default namespace is tested.
/*    
    _LIT8(KInputString, "<a:Test xmlns=\"nsuri\" xmlns:a=\"nasuria\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\" z=\"value3\">\
<Element1 xmlns="" xmlns:c=\"nsuric\" c:x=\"value4\">\
<Element1 >Content</Element1>\
</Element1>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");
*/

/*
    // Only namespace declaration order changes for output.
    _LIT8(KOutputString, "<a:Test xmlns:a=\"nasuria\" xmlns=\"nsuri\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\" z=\"value3\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value4\">\
<Element1>Content</Element1>\
</Element1>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");
*/

	CSenParser* pParser = CSenParser::NewLC();
	
    /*CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment);
	pParser->EnableFeature(EReportNamespaceMapping);
	
	pParser->ParseL(KInputString, *pFragment);
	
	HBufC8* pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);

    EUNIT_ASSERT( *pAsXml == KOutputString );

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);*/
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing6L(TTestResult& aResult)
	{
    SetupL();	
    // Parsing of XML document to CSenDomFragmentBase
    // when constructor CSenDomFragmentBase::NewL() is used.
    _LIT8(KInputString, "<a:Test xmlns=\"nsuri\" xmlns:a=\"nasuria\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value3\">Content1</Element1>\
<Element2 z=\"value4\">Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");

    // Default namespace declaration (xmlns="nsuri") from Parent element (Test)
    // will be moved to all child elements (Element1, Element2, Element3 and
    // Element4) when EReportNamespaceMapping is _NOT_ enabled. That's because
    // the way CParser works <=> Namespaces are reported when namespaces are used
    // for the first time.
    _LIT8(KOutputString, "<a:Test xmlns:a=\"nasuria\" xmlns:b=\"nasurib\" \
a:x=\"value1\" b:z=\"value2\">\
<Element1 xmlns=\"nsuri\" xmlns:c=\"nsuric\" c:x=\"value3\">Content1</Element1>\
<Element2 xmlns=\"nsuri\" z=\"value4\">Content2</Element2>\
<Element3 xmlns=\"nsuri\">Content3</Element3>\
<Element4 xmlns=\"nsuri\"/></a:Test>");

	CSenParser* pParser = CSenParser::NewLC();
	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL();
    CleanupStack::PushL(pFragment);
	
	pParser->ParseL(KInputString, *pFragment);
	
	HBufC8* pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);

    if(!( *pAsXml == KOutputString )) return KErrArgument;

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);

    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing7L(TTestResult& aResult)
	{
    SetupL();	
    // Parsing of XML document to CSenDomFragmentBase
    // when constructor CSenDomFragmentBase::NewL() is used.
    _LIT8(KInputString, "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\" \
xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">\
<S:Header>\
<wsa:MessageID>URN:UUID:ACF914D6-86A4-E5CF-4BD2-BD9CC2AD30E1</wsa:MessageID>\
<wsa:To>http://10.132.11.35/WSStar/STS.aspx</wsa:To>\
<wsa:Action>http://schemas.xmlsoap.org/ws/2005/02/trust/RST/Issue</wsa:Action>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
<wsse:UsernameToken>\
<wsse:Username>PCNSA15\\wsstar</wsse:Username>\
<wsse:Password>jaszmn\\3</wsse:Password>\
</wsse:UsernameToken>\
</wsse:Security>\
</S:Header>\
<S:Body>\
<wst:RequestSecurityToken xmlns:wst=\"http://schemas.xmlsoap.org/ws/2005/02/trust\">\
<wst:RequestType>http://schemas.xmlsoap.org/ws/2005/02/trust/Issue</wst:RequestType>\
<wst:Renewing/>\
<wst:TokenType>http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.1#SAMLV1.1</wst:TokenType>\
<wsp:AppliesTo xmlns:wsp=\"http://schemas.xmlsoap.org/ws/2004/09/policy\">\
<wsa:EndpointReference>\
<wsa:Address>http://10.132.11.35/WSStar/WSService.aspx</wsa:Address>\
</wsa:EndpointReference>\
</wsp:AppliesTo>\
</wst:RequestSecurityToken>\
</S:Body>\
</S:Envelope>");

    // Note: If EReportNamespaceMapping is not enabled output will be following:
    //       (Namespace declaration for prefix "wsa" jumps from root element to
    //        elements (MessageID, To, Action and EndpointReference) which actually
    //        use above mentioned prefix. <=> Root element Envelope does not use
    //        "wsa" prefix.)
    _LIT8(KOutputString, "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<S:Header>\
<wsa:MessageID xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">URN:UUID:ACF914D6-86A4-E5CF-4BD2-BD9CC2AD30E1</wsa:MessageID>\
<wsa:To xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">http://10.132.11.35/WSStar/STS.aspx</wsa:To>\
<wsa:Action xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">http://schemas.xmlsoap.org/ws/2005/02/trust/RST/Issue</wsa:Action>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
<wsse:UsernameToken>\
<wsse:Username>PCNSA15\\wsstar</wsse:Username>\
<wsse:Password>jaszmn\\3</wsse:Password>\
</wsse:UsernameToken>\
</wsse:Security>\
</S:Header>\
<S:Body>\
<wst:RequestSecurityToken xmlns:wst=\"http://schemas.xmlsoap.org/ws/2005/02/trust\">\
<wst:RequestType>http://schemas.xmlsoap.org/ws/2005/02/trust/Issue</wst:RequestType>\
<wst:Renewing/>\
<wst:TokenType>http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.1#SAMLV1.1</wst:TokenType>\
<wsp:AppliesTo xmlns:wsp=\"http://schemas.xmlsoap.org/ws/2004/09/policy\">\
<wsa:EndpointReference xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">\
<wsa:Address>http://10.132.11.35/WSStar/WSService.aspx</wsa:Address>\
</wsa:EndpointReference>\
</wsp:AppliesTo>\
</wst:RequestSecurityToken>\
</S:Body>\
</S:Envelope>");

    CSenParser* parser = CSenParser::NewLC();
    //create a CSenDomFragment
	CSenFragmentBase* pBase = CSenDomFragmentBase::NewL();
	CleanupStack::PushL(pBase);
	//do the parsing
	parser->ParseL(KInputString, *pBase);
	
    HBufC8* pAsXml = pBase->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KOutputString )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pBase);

    pBase = CSenDomFragmentBase::NewL();
    CleanupStack::PushL(pBase);
    //Enable NamespaceMapping to ensure that namespace declarations will
    //remain in exactly the same place as in original XML document
    parser->EnableFeature(EReportNamespaceMapping);
	//do the parsing
	parser->ParseL(KInputString, *pBase);
	
    pAsXml = pBase->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KInputString )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    
    CleanupStack::PopAndDestroy(pBase);
    CleanupStack::PopAndDestroy(parser);
    Teardown();
    return KErrNone;
	}
         
         
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_Parsing8L(TTestResult& aResult)
	{
    SetupL();	
    // Parsing of XML document to CSenDomFragmentBase
    // when constructor CSenDomFragmentBase::NewL() is used.
    _LIT8(KInputString, "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\" \
xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">\
<S:Header>\
<wsa:MessageID>URN:UUID:ACF914D6-86A4-E5CF-4BD2-BD9CC2AD30E1</wsa:MessageID>\
<wsa:To>http://10.132.11.35/WSStar/STS.aspx</wsa:To>\
<wsa:Action>http://schemas.xmlsoap.org/ws/2005/02/trust/RST/Issue</wsa:Action>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
<wsse:UsernameToken>\
<wsse:Username>PCNSA15\\wsstar</wsse:Username>\
<wsse:Password>jaszmn\\3</wsse:Password>\
</wsse:UsernameToken>\
</wsse:Security>\
</S:Header>\
<S:Body>\
<wst:RequestSecurityToken xmlns:wst=\"http://schemas.xmlsoap.org/ws/2005/02/trust\">\
<wst:RequestType>http://schemas.xmlsoap.org/ws/2005/02/trust/Issue</wst:RequestType>\
<wst:Renewing/>\
<wst:TokenType>http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.1#SAMLV1.1</wst:TokenType>\
<wsp:AppliesTo xmlns:wsp=\"http://schemas.xmlsoap.org/ws/2004/09/policy\">\
<wsa:EndpointReference>\
<wsa:Address>http://10.132.11.35/WSStar/WSService.aspx</wsa:Address>\
</wsa:EndpointReference>\
</wsp:AppliesTo>\
</wst:RequestSecurityToken>\
</S:Body>\
</S:Envelope>");

    // Note: If EReportNamespaceMapping is not enabled output will be following:
    //       (Namespace declaration for prefix "wsa" jumps from root element to
    //        elements (MessageID, To, Action and EndpointReference) which actually
    //        use above mentioned prefix. <=> Root element Envelope does not use
    //        "wsa" prefix.)
    _LIT8(KOutputString1, "<S:Body xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<wst:RequestSecurityToken xmlns:wst=\"http://schemas.xmlsoap.org/ws/2005/02/trust\">\
<wst:RequestType>http://schemas.xmlsoap.org/ws/2005/02/trust/Issue</wst:RequestType>\
<wst:Renewing/>\
<wst:TokenType>http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.1#SAMLV1.1</wst:TokenType>\
<wsp:AppliesTo xmlns:wsp=\"http://schemas.xmlsoap.org/ws/2004/09/policy\">\
<wsa:EndpointReference xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">\
<wsa:Address>http://10.132.11.35/WSStar/WSService.aspx</wsa:Address>\
</wsa:EndpointReference>\
</wsp:AppliesTo>\
</wst:RequestSecurityToken>\
</S:Body>");

    _LIT8(KOutputString2, "<S:Body xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<wst:RequestSecurityToken xmlns:wst=\"http://schemas.xmlsoap.org/ws/2005/02/trust\">\
<wst:RequestType>http://schemas.xmlsoap.org/ws/2005/02/trust/Issue</wst:RequestType>\
<wst:Renewing/>\
<wst:TokenType>http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.1#SAMLV1.1</wst:TokenType>\
<wsp:AppliesTo xmlns:wsp=\"http://schemas.xmlsoap.org/ws/2004/09/policy\">\
<wsa:EndpointReference xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">\
<wsa:Address>http://10.132.11.35/WSStar/WSService.aspx</wsa:Address>\
</wsa:EndpointReference>\
</wsp:AppliesTo>\
</wst:RequestSecurityToken>\
</S:Body>");

    CSenParser* parser = CSenParser::NewLC();
    //create a CSenDomFragment
	CSenFragmentBase* pBase = CSenDomFragmentBase::NewL(_L8("http://www.w3.org/2003/05/soap-envelope"),
	                                                    _L8("Body"),
	                                                    _L8("S"));
	CleanupStack::PushL(pBase);
	//do the parsing
	parser->ParseL(KInputString, *pBase);
	
    HBufC8* pAsXml = pBase->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KOutputString1 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pBase);

    pBase = CSenDomFragmentBase::NewL(_L8("http://www.w3.org/2003/05/soap-envelope"),
	                                  _L8("Body"),
	                                  _L8("S"));
    CleanupStack::PushL(pBase);
    //Enable NamespaceMapping to ensure that namespace declarations will
    //remain in exactly the same place as in original XML document
    parser->EnableFeature(EReportNamespaceMapping);
	//do the parsing
	parser->ParseL(KInputString, *pBase);
	
    pAsXml = pBase->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KOutputString2 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    
    CleanupStack::PopAndDestroy(pBase);
    CleanupStack::PopAndDestroy(parser);
    Teardown();
    return KErrNone;
	}
/*	
TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_SetContentOfLL(TTestResult& aResult)
	{
    SetupL();	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    pFragment->SetContentOfL(_L8("name"), _L8("content"));
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}


TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_ContentOfL(TTestResult& aResult)
	{
    SetupL();	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    pFragment->ContentOf(_L8("name"));
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
*/	

TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_OnWriteStartElementLL(TTestResult& aResult)
	{
    SetupL();
    RTagInfo info;
    RAttributeArray array;	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    pFragment->OnWriteStartElementL(info, array);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}

TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_OnWriteEndElementLL(TTestResult& aResult)
	{
    SetupL();
    RTagInfo info;
    	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    pFragment->OnWriteEndElementL(info);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}

TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_OnIgnorableWhiteSpaceLL(TTestResult& aResult)
	{
    SetupL();
    TInt var = 0;	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    pFragment->OnIgnorableWhiteSpaceL(_L8("test"), var);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}

TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_OnSkippedEntityLL(TTestResult& aResult)
	{
    SetupL();
	RString str;
    TInt var = 0;	
	   	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    pFragment->OnSkippedEntityL(str, var);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}

TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_OnProcessingInstructionLL(TTestResult& aResult)
	{
    SetupL();
    TInt var = 0;	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    pFragment->OnProcessingInstructionL(_L8("test"), _L8("test"), var);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}

TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_OnErrorL(TTestResult& aResult)
	{
    SetupL();
    TInt var = 0;
    	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    pFragment->OnError(var);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}

TInt CSenFragmentBCTest::MT_CSenDomFragmentBase_GetExtendedInterfaceL(TTestResult& aResult)
	{
    SetupL();
    const TInt32 var = 0;
    	
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    pFragment->GetExtendedInterface(var);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_NewLL(TTestResult& aResult)
	{
    SetupL();	
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement element = document.CreateDocumentElementL(_L8("Test"));
    _LIT8(KElement, "<Element/>");
    element.SetTextNoEncL(KElement);
   
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(element);
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    // Test that when CSenFragmentBase is serialized '<' and '>' characters
    // are NOT encoded => The content of CSenFragmentBase can include
    // XML elements
    if(!( *pAsXml == _L8("<Test><Element/></Test>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&document);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_NewL_1L(TTestResult& aResult)
	{
    SetupL();	
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<Test/>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_NewL_2L(TTestResult& aResult)
	{
    SetupL();	
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"));
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<Test xmlns=\"NsUri\"/>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_NewL_3L(TTestResult& aResult)
	{
    SetupL();	
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"));
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<t:Test xmlns:t=\"NsUri\"/>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_NewL_4L(TTestResult& aResult)
	{
    SetupL();	
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
                   valueRString );
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
    
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"),
                                                         attrArray);
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<t:Test xmlns:t=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool); 
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_NewL_5L(TTestResult& aResult)
	{
    SetupL();	
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement parentElement = document.CreateDocumentElementL(_L8("Parent"));
    
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
        
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"),
                                                         attrArray,
                                                         parentElement);
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<t:Test xmlns:t=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") )) return KErrArgument;

    RBuf8 buffer;
    parentElement.OuterXmlL(buffer);
    
    if(!( buffer == _L8("<Parent><t:Test xmlns:t=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/></Parent>") )) return KErrArgument;
    buffer.Close();
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool); 
    CleanupStack::PopAndDestroy(&document);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_NewL_6L(TTestResult& aResult)
	{
    SetupL();	
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement parentElement = document.CreateDocumentElementL(_L8("Parent"));
    
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
        
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8(""),
                                                         attrArray,
                                                         parentElement,
                                                         document);
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<Test xmlns=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") )) return KErrArgument;

    RBuf8 buffer;
    parentElement.OuterXmlL(buffer);
    
    if(!( buffer == _L8("<Parent><Test xmlns=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/></Parent>") )) return KErrArgument;
    buffer.Close();
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool); 
    CleanupStack::PopAndDestroy(&document);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_NewL_7L(TTestResult& aResult)
	{
    SetupL();	
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement parentElement = document.CreateDocumentElementL(_L8("Parent"));
    
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
        
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8(""),
                                                         attrArray,
                                                         parentElement,
                                                         document);
    CleanupStack::PushL(pFragment);
    
    RSenDocument doc = pFragment->AsDocumentL();
    TXmlEngElement el = pFragment->AsElementL();
    CSenFragmentBase* pFragment2 = CSenFragmentBase::NewL(el, doc);
    
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PushL(pFragment2);
        
    HBufC8* pAsXml = pFragment2->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<Test xmlns=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") )) return KErrArgument;

    RBuf8 buffer;
    parentElement.OuterXmlL(buffer);
    
    if(!( buffer == _L8("<Parent><Test xmlns=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/></Parent>") )) return KErrArgument;
    buffer.Close();
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment2);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool); 
    CleanupStack::PopAndDestroy(&document);
    Teardown();
    return KErrNone;
    
	}
	
TInt CSenFragmentBCTest::MT_CSenFragmentBase_SetDocumentL(TTestResult& aResult)
	{
    SetupL();	
    RSenDocument document ;
    document = RSenDocument::NewL();
    
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    
    pFragment->SetDocument(document);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
	
   
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_ContentLL(TTestResult& aResult)
	{
    SetupL();	
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement element = document.CreateDocumentElementL(_L8("Test"));
    element.SetEscapedTextL(_L8("<Element/>"));
    
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(element);
    CleanupStack::PushL(pFragment);
    
    TPtrC8 content = pFragment->ContentL();
    if(!( content == _L8("<Element/>") )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&document);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_NamespaceL(TTestResult& aResult)
	{
    SetupL();	
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
    attribute.Open(stringPool.OpenStringL(_L8("nsuri")), 
                   stringPool.OpenStringL(_L8("pr")),
                   stringPool.OpenStringL(_L8("LocalName")),
                   stringPool.OpenStringL(_L8("Value")) );
    // attribute took copies of all RStrings
    // => All RStrings can be destroyed
    CleanupStack::PopAndDestroy(4); // nsUriRString, nsPrefixRString, localNameRString,
                                    // valueRString
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    // append the namespace attribute (declaration)
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray);
    
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"),
                                                         attrArray);
    CleanupStack::PushL(pFragment);
    
    TXmlEngNamespace ns = pFragment->Namespace(_L8("pr"));
    if(!( ns.Uri() == _L8("nsuri") )) return KErrArgument;

    TXmlEngNamespace ns2 = pFragment->Namespace(_L8("x"));
    if(!( ns2.IsUndefined() )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(); // attribute
    CleanupStack::PopAndDestroy(); // attrArray
    CleanupStack::PopAndDestroy(); // stringPool  
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_ResetContentLL(TTestResult& aResult)
	{
    SetupL();	
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement element = document.CreateDocumentElementL(_L8("Test"));
    element.SetEscapedTextL(_L8("Content"));
    
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(element);
    CleanupStack::PushL(pFragment);
    
    // Test that CSenFragmentBase has content before reset
    TPtrC8 content = pFragment->ContentL();
    if(!( content == _L8("Content") )) return KErrArgument;
    
    pFragment->ResetContentL();
    
    // Test that CSenFragmentBase has no content after reset
    TPtrC8 content2 = pFragment->ContentL();
    if(!( content2 == KNullDesC8 )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&document);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_LocalNameL(TTestResult& aResult)
	{
    SetupL();	
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"));
    CleanupStack::PushL(pFragment);
    TPtrC8 localName = pFragment->LocalName();
    if(!( localName == _L8("Test") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_NsUriL(TTestResult& aResult)
	{
    SetupL();	
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"));
    CleanupStack::PushL(pFragment);
    TPtrC8 localName = pFragment->NsUri();
    if(!( localName == _L8("NsUri") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_NsPrefixL(TTestResult& aResult)
	{
	SetupL();	
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"));
    CleanupStack::PushL(pFragment);
    TPtrC8 localName = pFragment->NsPrefix();
    if(!( localName == _L8("t") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;    
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_AsElementLL(TTestResult& aResult)
	{
	SetupL();	
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    RBuf8 buffer;
    CleanupClosePushL(buffer);
    element.OuterXmlL(buffer);
    // Serialized element should contain all the Fragment data as XML.
    if(!( buffer == _L8("<t:Test xmlns:t=\"NsUri\"/>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(&buffer);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;    

	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_AsDocumentLL(TTestResult& aResult)
	{
	SetupL();
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"));
    CleanupStack::PushL(pFragment);
    RSenDocument document = pFragment->AsDocumentL();
    TXmlEngElement element = document.DocumentElement();
    
    TXmlEngSerializationOptions options;
    // Omit following declarations from the beginning of XML Document:
    // <?xml version=\"1.0\...
    //   encoding="..."
    //   standalone="..."
    // ?>
    options.iOptions = options.iOptions |
                       TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;
    RBuf8 asXml;
    document.SaveL(asXml, element, options);
    CleanupClosePushL(asXml);
    
    // Serialized document should contain all the Fragment data as XML.
    if(!( asXml == _L8("<t:Test xmlns:t=\"NsUri\"/>") )) return KErrArgument;

    CleanupStack::PopAndDestroy(&asXml);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;    
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_ExtractElementL(TTestResult& aResult)
	{
	SetupL();
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->ExtractElement();
    if(!( element.Name() == _L8("Test") )) return KErrArgument;
    element.Remove();
    CleanupStack::PopAndDestroy(pFragment);    
    Teardown();
    return KErrNone;    
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_AsXmlUnicodeLL(TTestResult& aResult)
	{
	SetupL();
    // Test serialization of Dom tree which has two child elements
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement element2 = element.AddNewElementL(_L8("Child"), _L8("nsuri"), _L8("prefix"));

    element2.AddNewElementL(_L8("Child2"), _L8("nsuri2"), _L8("prefix2"));
    
    HBufC* pAsXml = pFragment->AsXmlUnicodeL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L("<Test xmlns:prefix=\"nsuri\"><prefix:Child xmlns:prefix2=\"nsuri2\"><prefix2:Child2/></prefix:Child></Test>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;    
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_AsXmlLL(TTestResult& aResult)
	{
	SetupL();
    // Test serialization of Dom tree which has two child elements
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement element2 = element.AddNewElementL(_L8("Child"), _L8("nsuri"), _L8("prefix"));

    element2.AddNewElementL(_L8("Child2"), _L8("nsuri2"), _L8("prefix2"));
    
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == _L8("<Test xmlns:prefix=\"nsuri\"><prefix:Child xmlns:prefix2=\"nsuri2\"><prefix2:Child2/></prefix:Child></Test>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;    
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_WriteAsXMLToLL(TTestResult& aResult)
	{
	SetupL();
    // Test serialization of Dom tree which has two child elements
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement element2 = element.AddNewElementL(_L8("Child"), _L8("nsuri"), _L8("prefix"));

    element2.AddNewElementL(_L8("Child2"), _L8("nsuri2"), _L8("prefix2"));
    
    CBufFlat* pBuf = CBufFlat::NewL(200);
    CleanupStack::PushL(pBuf);
    RBufWriteStream bufWs(*pBuf);
    CleanupClosePushL(bufWs);
    pFragment->WriteAsXMLToL(bufWs);


    if(!( pBuf->Ptr(0) == _L8("<Test xmlns:prefix=\"nsuri\"><prefix:Child xmlns:prefix2=\"nsuri2\"><prefix2:Child2/></prefix:Child></Test>") )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(&bufWs);
    CleanupStack::PopAndDestroy(pBuf);
    
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;    
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_ConsistsOfLL(TTestResult& aResult)
	{
	SetupL();
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement elementchild1 = element.AddNewElementL(_L8("DirectChild"));
    elementchild1.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //elementchild1.RenameNodeL(_L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    //TDomUtils domUtils1;
    //domUtils1.XmlEngRenameElementL(elementchild1, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));

    TXmlEngElement elementchild2 = element.AddNewElementL(_L8("DirectChild"));
    elementchild2.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //elementchild2.RenameNodeL(_L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    //TDomUtils domUtils2;
    //domUtils2.XmlEngRenameElementL(elementchild2, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    elementchild2.AddNewElementL(_L8("Child"), _L8("nsuri2"), _L8("prefix2"));

    CSenFragmentBase* pFragment2 = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment2);
    TXmlEngElement element2 = pFragment2->AsElementL();
    
    TXmlEngElement elementchild22 = element2.AddNewElementL(_L8("DirectChild"));
    elementchild22.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //elementchild22.RenameNodeL(_L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    //TDomUtils domUtils22;
    //domUtils22.XmlEngRenameElementL(elementchild22, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    elementchild22.AddNewElementL(_L8("Child"), _L8("nsuri2"), _L8("prefix2"));

    // ConsistOfL should return ETrue because
    // root elements of both fragments match and the only element
    // of pFragment2 = elementchild22 is exactly the same element as
    // elementchild2.
    if(!( pFragment->ConsistsOfL(*pFragment2) )) return KErrArgument;

    // ConsistOfL should return EFalse because
    // Even though root elements of both fragments match
    // there is extra element (= elementchild1) in pFragment.
    //
    // elementchild1 (of pFragment) can not be found from pFragment2
    // and because of that pFragment2 does not consist of pFragment.
    if(!( !pFragment2->ConsistsOfL(*pFragment) )) return KErrArgument;
    CleanupStack::PopAndDestroy(pFragment2);

    CSenFragmentBase* pFragment3 = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment3);
    TXmlEngElement element3 = pFragment3->AsElementL();
    
    TXmlEngElement elementchild32 = element3.AddNewElementL(_L8("DirectChild"));
    elementchild32.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //elementchild32.RenameNodeL(_L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    //TDomUtils domUtils32;
    //domUtils32.XmlEngRenameElementL(elementchild32, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    elementchild32.AddNewElementL(_L8("Child"), _L8("nsuri2"), _L8("prefix2"));
    elementchild32.SetEscapedTextL(_L8("Text"));
    
    // ConsistOfL should return EFalse because
    // elementchild32 has content ("Text") which doesn't match
    // to any child element of pFragment.
    if(!( !pFragment->ConsistsOfL(*pFragment3) )) return KErrArgument;
    CleanupStack::PopAndDestroy(pFragment3);
    
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;    
	}
        
        
TInt CSenFragmentBCTest::MT_CSenFragmentBase_Parsing1L(TTestResult& aResult)
	{
	SetupL();
    // Parsing of XML document to CSenFragmentBase
    _LIT8(KInputString, "<a:Test xmlns=\"nsuri\" xmlns:a=\"nasuria\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value4\">Content1</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");

    // Default namespace declaration (xmlns="nsuri") from Parent element (Test)
    // will be moved to all child elements (Element1, Element2, Element3 and
    // Element4) when EReportNamespaceMapping is _NOT_ enabled. That's because
    // the way CParser works <=> Namespaces are reported when namespaces are used.
    _LIT8(KOutputString1, "<a:Test xmlns:a=\"nasuria\" xmlns:b=\"nasurib\" \
a:x=\"value1\" b:z=\"value2\">\
<Element1 xmlns=\"nsuri\" xmlns:c=\"nsuric\" c:x=\"value4\">Content1</Element1>\
<Element2 xmlns=\"nsuri\">Content2</Element2>\
<Element3 xmlns=\"nsuri\">Content3</Element3>\
<Element4 xmlns=\"nsuri\"/></a:Test>");

    // When EReportNamespaceMapping is enabled all the namespace declarations
    // are reported in exactly the same element as they are defined in parsed
    // XML document.
    // Note: The Order of namespace declarations in root element tag may change.
    // Namespace declaration for root tag is created when fragment is created
    // and because of that namespace declaration for root tag will be the first.
    // In this particular case namespace declaration for prefix "a" will be
    // the first namespace declaration because it is declared when
    // CSenFragmentBase is created.
    _LIT8(KOutputString2, "<a:Test xmlns:a=\"nasuria\" xmlns=\"nsuri\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value4\">Content1</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");
    
	CSenParser* pParser = CSenParser::NewLC();
	
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment);

/*	
	//tma
	pParser->ParseBeginL();
	
	_LIT8(KXmlParserMimeType,   "text/xml");
	pParser->ParseBeginL(KXmlParserMimeType);
		
    CMatchData* pMatchData = CMatchData::NewLC();
    pMatchData->SetMimeTypeL(KXmlParserMimeType);
    pParser->ParseBeginL(*pMatchData);
    CleanupStack::PopAndDestroy(pMatchData);
	//tma	
*/

	pParser->ParseL(KInputString, *pFragment);
	
	HBufC8* pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);

    if(!( *pAsXml == KOutputString1 )) return KErrArgument;

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);

	pParser->EnableFeature(EReportNamespaceMapping);
	
    CSenFragmentBase* pFragment2 = CSenFragmentBase::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment2);

	pParser->ParseL(KInputString, *pFragment2);
	
	HBufC8* pAsXml2 = pFragment2->AsXmlL();
	CleanupStack::PushL(pAsXml2);
	                   
    if(!( *pAsXml2 == KOutputString2 )) return KErrArgument;
    
    CleanupStack::PopAndDestroy(pAsXml2);
    CleanupStack::PopAndDestroy(pFragment2);

    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;    
	}


TInt CSenFragmentBCTest::MT_CSenFragmentBase_Parsing2L(TTestResult& aResult)
	{
	SetupL();
    // Parsing of XML document to CSenFragmentBase which uses
    // delegation.
    _LIT8(KInputString, "<a:Test xmlns:a=\"nasuria\" xmlns:b=\"nsurib\" \
xmlns=\"nsuri\" a:x=\"value1\" b:z=\"value2\">\
<DelegateFragment>DelegateContent</DelegateFragment>\
<Element1 x=\"value4\">Content1</Element1>\
<Element2>Content2</Element2></a:Test>");

    // Default namespace declaration (xmlns="nsuri") from Parent element (Test)
    // will be moved to all child elements (Element1, Element2, Element3 and
    // Element4) when EReportNamespaceMapping is _NOT_ enabled. That's because
    // the way CParser works <=> Namespaces are reported when namespaces are used
    // for the first time.
    // Note: In CSenFragmentBase case DelegateFragment won't be in
    //       content of parent fragment which did the delegation.
    _LIT8(KOutputString1, "<a:Test xmlns:a=\"nasuria\" xmlns:b=\"nsurib\" \
a:x=\"value1\" b:z=\"value2\">\
<Element1 xmlns=\"nsuri\" x=\"value4\">Content1</Element1>\
<Element2 xmlns=\"nsuri\">Content2</Element2></a:Test>");

    // When EReportNamespaceMapping is enabled all the namespace declarations
    // are reported in exactly the same element as they are defined in parsed
    // XML document.
    // Note: The Order of namespace declarations in root element tag may change.
    // Namespace declaration for root tag is created when fragment is created
    // and because of that namespace declaration for root tag will be the first.
    // Note: In CSenFragmentBase case DelegateFragment won't be in
    //       content of parent fragment which did the delegation.
    _LIT8(KOutputString2, "<a:Test xmlns:a=\"nasuria\" xmlns:b=\"nsurib\" \
xmlns=\"nsuri\" a:x=\"value1\" b:z=\"value2\">\
<Element1 x=\"value4\">Content1</Element1>\
<Element2>Content2</Element2></a:Test>");
     
	CSenParser* pParser = CSenParser::NewLC();
	
    CMainFragment* pFragment = CMainFragment::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment);
	
	pParser->ParseL(KInputString, *pFragment);
	
	HBufC8* pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KOutputString1 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    
    CDelegateFragment& delegate = pFragment->DelegateFragment();
    HBufC8* pDelegateAsXml = delegate.AsXmlL();
    CleanupStack::PushL(pDelegateAsXml);
    if(!( *pDelegateAsXml ==
                 _L8("<DelegateFragment xmlns=\"nsuri\">DelegateContent</DelegateFragment>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pDelegateAsXml);
    
    CleanupStack::PopAndDestroy(pFragment);

	pParser->EnableFeature(EReportNamespaceMapping);
	
    CMainFragment* pFragment2 = CMainFragment::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment2);

	pParser->ParseL(KInputString, *pFragment2);
	
	HBufC8* pAsXml2 = pFragment2->AsXmlL();
	CleanupStack::PushL(pAsXml2);
    if(!( *pAsXml2 == KOutputString2 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml2);
    
    CDelegateFragment& delegate2 = pFragment2->DelegateFragment();
    HBufC8* pDelegateAsXml2 = delegate2.AsXmlL();
    CleanupStack::PushL(pDelegateAsXml2);
    if(!( *pDelegateAsXml2 ==
                 _L8("<DelegateFragment xmlns=\"nsuri\">DelegateContent</DelegateFragment>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(pDelegateAsXml2);
    
    CleanupStack::PopAndDestroy(pFragment2);

    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;    
	}
         
         
TInt CSenFragmentBCTest::MT_CSenFragmentBase_Parsing3L(TTestResult& aResult)
	{
	SetupL();
    // Parsing of XML document to CSenFragmentBase
    _LIT8(KInputString, "<a:Test xmlns=\"nsuri\" xmlns:a=\"nasuria\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\" z=\"value3\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value4\">Content1</Element1>\
<x:Element2 xmlns:x=\"nsuri\">Content2</x:Element2>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");

    _LIT8(KOutputString, "<a:Test xmlns:a=\"nasuria\" xmlns=\"nsuri\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\" z=\"value3\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value4\">Content1</Element1>\
<x:Element2 xmlns:x=\"nsuri\">Content2</x:Element2>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");
    
	CSenParser* pParser = CSenParser::NewLC();
	
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment);
	pParser->EnableFeature(EReportNamespaceMapping);
	
	pParser->ParseL(KInputString, *pFragment);
	
	HBufC8* pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);

    if(!( *pAsXml == KOutputString )) return KErrArgument;

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;    
	}


TInt CSenFragmentBCTest::MT_CSenFragmentBase_Parsing4L(TTestResult& aResult)
	{
	SetupL();
    // Parsing of XML document to CSenFragmentBase
    // In this test nested elements (which have same name) are tested.
    _LIT8(KInputString, "<a:Test xmlns=\"nsuri\" xmlns:a=\"nasuria\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\" z=\"value3\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value4\">\
<Element1>Content</Element1>\
</Element1>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");

    _LIT8(KInputString2, "<Nested xmlns=\"nsuri\">\
<Nested attr=\"attrvalue\"><Nested><Nested>Content</Nested></Nested></Nested>\
</Nested>");

    // Only namespace declaration order changes for output.
    // Element4 which has no content will be output with
    // start and end tags.
    _LIT8(KOutputString, "<a:Test xmlns:a=\"nasuria\" xmlns=\"nsuri\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\" z=\"value3\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value4\">\
<Element1>Content</Element1>\
</Element1>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");

	CSenParser* pParser = CSenParser::NewLC();
	
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment);
	pParser->EnableFeature(EReportNamespaceMapping);
	
	pParser->ParseL(KInputString, *pFragment);
	
	HBufC8* pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);

    if(!( *pAsXml == KOutputString )) return KErrArgument;

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);

    pFragment = CSenFragmentBase::NewL(_L8("nsuri"),_L8("Nested"));
    CleanupStack::PushL(pFragment);
	pParser->EnableFeature(EReportNamespaceMapping);
	
	pParser->ParseL(KInputString2, *pFragment);
	
	pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);

    if(!( *pAsXml == KInputString2 )) return KErrArgument;

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);

    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;    
	}


TInt CSenFragmentBCTest::MT_CSenFragmentBase_Parsing5L(TTestResult& aResult)
	{
	SetupL();
    // Parsing of XML document to CSenDomFragmentBase
    // when constructor CSenDomFragmentBase::NewL() is used.
    _LIT8(KInputString, "<S:Envelope xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\" \
xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">\
<S:Header>\
<wsa:MessageID>URN:UUID:ACF914D6-86A4-E5CF-4BD2-BD9CC2AD30E1</wsa:MessageID>\
<wsa:To>http://10.132.11.35/WSStar/STS.aspx</wsa:To>\
<wsa:Action>http://schemas.xmlsoap.org/ws/2005/02/trust/RST/Issue</wsa:Action>\
<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
<wsse:UsernameToken>\
<wsse:Username>PCNSA15\\wsstar</wsse:Username>\
<wsse:Password>jaszmn\\3</wsse:Password>\
</wsse:UsernameToken>\
</wsse:Security>\
</S:Header>\
<S:Body>\
<wst:RequestSecurityToken xmlns:wst=\"http://schemas.xmlsoap.org/ws/2005/02/trust\">\
<wst:RequestType>http://schemas.xmlsoap.org/ws/2005/02/trust/Issue</wst:RequestType>\
<wst:Renewing/>\
<wst:TokenType>http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.1#SAMLV1.1</wst:TokenType>\
<wsp:AppliesTo xmlns:wsp=\"http://schemas.xmlsoap.org/ws/2004/09/policy\">\
<wsa:EndpointReference>\
<wsa:Address>http://10.132.11.35/WSStar/WSService.aspx</wsa:Address>\
</wsa:EndpointReference>\
</wsp:AppliesTo>\
</wst:RequestSecurityToken>\
</S:Body>\
</S:Envelope>");

    // Note: If EReportNamespaceMapping is not enabled output will be following:
    //       (Namespace declaration for prefix "wsa" jumps from root element to
    //        elements (MessageID, To, Action and EndpointReference) which actually
    //        use above mentioned prefix. <=> Root element Envelope does not use
    //        "wsa" prefix.)
    _LIT8(KOutputString1, "<S:Body xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\">\
<wst:RequestSecurityToken xmlns:wst=\"http://schemas.xmlsoap.org/ws/2005/02/trust\">\
<wst:RequestType>http://schemas.xmlsoap.org/ws/2005/02/trust/Issue</wst:RequestType>\
<wst:Renewing/>\
<wst:TokenType>http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.1#SAMLV1.1</wst:TokenType>\
<wsp:AppliesTo xmlns:wsp=\"http://schemas.xmlsoap.org/ws/2004/09/policy\">\
<wsa:EndpointReference xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">\
<wsa:Address>http://10.132.11.35/WSStar/WSService.aspx</wsa:Address>\
</wsa:EndpointReference>\
</wsp:AppliesTo>\
</wst:RequestSecurityToken>\
</S:Body>");

    _LIT8(KOutputString2, "<S:Body xmlns:S=\"http://www.w3.org/2003/05/soap-envelope\" \
xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\" \
xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
<wst:RequestSecurityToken xmlns:wst=\"http://schemas.xmlsoap.org/ws/2005/02/trust\">\
<wst:RequestType>http://schemas.xmlsoap.org/ws/2005/02/trust/Issue</wst:RequestType>\
<wst:Renewing/>\
<wst:TokenType>http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.1#SAMLV1.1</wst:TokenType>\
<wsp:AppliesTo xmlns:wsp=\"http://schemas.xmlsoap.org/ws/2004/09/policy\">\
<wsa:EndpointReference>\
<wsa:Address>http://10.132.11.35/WSStar/WSService.aspx</wsa:Address>\
</wsa:EndpointReference>\
</wsp:AppliesTo>\
</wst:RequestSecurityToken>\
</S:Body>");

    CSenParser* parser = CSenParser::NewLC();
    //create a CSenDomFragment
	CSenFragmentBase* pBase = CSenFragmentBase::NewL(_L8("http://www.w3.org/2003/05/soap-envelope"),
	                                                 _L8("Body"),
	                                                 _L8("S"));
	CleanupStack::PushL(pBase);
	//do the parsing
	parser->ParseL(KInputString, *pBase);
	
    HBufC8* pAsXml = pBase->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KOutputString1 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pBase);

    pBase = CSenFragmentBase::NewL(_L8("http://www.w3.org/2003/05/soap-envelope"),
	                               _L8("Body"),
	                               _L8("S"));
    CleanupStack::PushL(pBase);
    //Enable NamespaceMapping to ensure that namespace declarations will
    //remain in exactly the same place as in original XML document
    parser->EnableFeature(EReportNamespaceMapping);
	//do the parsing
	parser->ParseL(KInputString, *pBase);
	
    pAsXml = pBase->AsXmlL();
    CleanupStack::PushL(pAsXml);
    if(!( *pAsXml == KOutputString2 )) return KErrArgument;
    CleanupStack::PopAndDestroy(pAsXml);
    
    CleanupStack::PopAndDestroy(pBase);
    CleanupStack::PopAndDestroy(parser);
    Teardown();
    return KErrNone;    
	}


TInt CSenFragmentBCTest::MT_CSenParser_NewLL(TTestResult& aResult)
	{
	SetupL();
    CSenParser* pParser = CSenParser::NewL();
    delete pParser;
    Teardown();
    return KErrNone;    
	}
        
        
TInt CSenFragmentBCTest::MT_CSenParser_NewLCL(TTestResult& aResult)
	{
	SetupL();
    CSenParser* pParser = CSenParser::NewLC();
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;    
	}
        
        
TInt CSenFragmentBCTest::MT_CSenParser_NewL_1L(TTestResult& aResult)
	{
	SetupL();
    _LIT8(KXmlParserMimeType,   "text/xml");
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    CParser* pParser = CParser::NewL(KXmlParserMimeType, *pFragment);

    // Ownership of the pParser is transfered to pSenParser
    CSenParser* pSenParser = CSenParser::NewL(pParser);
    delete pSenParser;

    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;    
	}
        
        
TInt CSenFragmentBCTest::MT_CSenParser_NewLC_1L(TTestResult& aResult)
	{
	SetupL();
    _LIT8(KXmlParserMimeType,   "text/xml");
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    CParser* pParser = CParser::NewL(KXmlParserMimeType, *pFragment);

    // Ownership of the pParser is transfered to pSenParser
    CSenParser* pSenParser = CSenParser::NewLC(pParser);
    CleanupStack::PopAndDestroy(pSenParser);

    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;    
	}
        
        
TInt CSenFragmentBCTest::MT_CSenParser_NewL_2L(TTestResult& aResult)
	{
	SetupL();
    _LIT8(KXmlParserMimeType,   "text/xml");
    CSenParser* pParser = CSenParser::NewL(KXmlParserMimeType);
    delete pParser;
    Teardown();
    return KErrNone;    
	}
        
        
TInt CSenFragmentBCTest::MT_CSenParser_NewLC_2L(TTestResult& aResult)
	{
	SetupL();
    _LIT8(KXmlParserMimeType,   "text/xml");
    CSenParser* pParser = CSenParser::NewLC(KXmlParserMimeType);
    CleanupStack::PopAndDestroy(pParser);
    Teardown();
    return KErrNone;    
	}
/*        
#if defined( __SERIES60_31__ ) || (!defined( __SERIES60_30__ ) && defined( SYMBIAN_SWI_POST_INSTALL_REVOCATION) )

TInt CSenFragmentBCTest::MT_CSenParser_NewL_3L(TTestResult& aResult)
	{
	SetupL();
    _LIT8(KXmlParserMimeType,   "text/xml");
    CMatchData* pMatchData = CMatchData::NewLC();
    pMatchData->SetMimeTypeL(KXmlParserMimeType);
    CSenParser* pParser = CSenParser::NewL(*pMatchData);
    delete pParser;
    CleanupStack::PopAndDestroy(pMatchData);
    Teardown();
    return KErrNone;    
	}

TInt CSenFragmentBCTest::MT_CSenParser_NewLC_3L(TTestResult& aResult)
	{
	SetupL();
    _LIT8(KXmlParserMimeType,   "text/xml");
    CMatchData* pMatchData = CMatchData::NewLC();
    pMatchData->SetMimeTypeL(KXmlParserMimeType);
    CSenParser* pParser = CSenParser::NewLC(*pMatchData);
    CleanupStack::PopAndDestroy(pParser);
    CleanupStack::PopAndDestroy(pMatchData);
    Teardown();
    return KErrNone;    
	}

#endif 
        
*/        
TInt CSenFragmentBCTest::MT_RSenDocument_NewLL(TTestResult& aResult)
	{
	SetupL();
    RSenDocument document = RSenDocument::NewL();
    document.Close();
    if(!( document.IsNull() )) return KErrArgument;
    Teardown();
    return KErrNone;    
	}
        
        
TInt CSenFragmentBCTest::MT_RSenDocument_NewLCL(TTestResult& aResult)
	{
	SetupL();
    RSenDocument document = RSenDocument::NewLC();
    CleanupStack::PopAndDestroy();
    
    // Can't use IsNull() function because iInternal pointer
    // (of document) is not NULLed. Only data for which iInternal
    // points to is deleted.
    //EUNIT_ASSERT( document.IsNull() );
    Teardown();
    return KErrNone;    
	}
         
TInt CSenFragmentBCTest::MT_RSenDocument_NewL_1L(TTestResult& aResult)
	{
	SetupL();
    // Can't test RSenDocument::NewL(void* aInternal)
    // because there is no way to get proper aInternal
    // using public API.
    if(!( ETrue )) return KErrArgument;
    Teardown();
    return KErrNone;    
	}
         
TInt CSenFragmentBCTest::MT_RSenDocument_NewLC_1L(TTestResult& aResult)
	{
	SetupL();
    // Can't test RSenDocument::NewLC(void* aInternal)
    // because there is no way to get proper aInternal
    // using public API.
    if(!( ETrue )) return KErrArgument;
    Teardown();
    return KErrNone;    
	}
        
TInt CSenFragmentBCTest::MT_RSenDocument_CopyL(TTestResult& aResult)
	{
	SetupL();
    RSenDocument document1 = RSenDocument::NewL();
    TXmlEngElement element = document1.CreateDocumentElementL(_L8("Test"));
    
    element.AddTextL(_L8("testcontent"));
    
    RSenDocument document2 = document1.Copy();
    
    // Check that main elements of document1 and document2 
    // are pointing to the same "in memory node" after copying.
    if(!( element.IsSameNode(document2.DocumentElement()) )) return KErrArgument;
    
    // Test that document2 contains the same data as document1
    // did contain before closing document1.
    document1.Close();
    RBuf8 asXml;
    TXmlEngSerializationOptions options;
    options.iOptions = options.iOptions |
                       TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;
    document2.SaveL(asXml, element, options);
    CleanupClosePushL(asXml);
    if(!( asXml == _L8("<Test>testcontent</Test>") )) return KErrArgument;
    CleanupStack::PopAndDestroy(&asXml);
    
    document2.Close();
    if(!( document1.IsNull() && document2.IsNull() )) return KErrArgument;
    Teardown();
    return KErrNone;    
	}
                
TInt CSenFragmentBCTest::MT_RSenDocument_CloseL(TTestResult& aResult)
	{
	SetupL();
    RSenDocument document = RSenDocument::NewL();
    document.Close();
    if(!( document.IsNull() )) return KErrArgument;
    Teardown();
    return KErrNone;    
	}
                
TInt CSenFragmentBCTest::MT_RSenDocument_DestroyL(TTestResult& aResult)
	{
	SetupL();
    RSenDocument document = RSenDocument::NewL();
    document.Destroy();
    if(!( document.IsNull() )) return KErrArgument;
    Teardown();
    return KErrNone;    
	}
	
TInt CSenFragmentBCTest::MT_RSenDocument_ManualXmlEngineTlsAttachLL(TTestResult& aResult)
	{
	SetupL();
    RSenDocument document = RSenDocument::NewL();
    TInt retVal = document.ManualXmlEngineTlsAttachL();
    TL(retVal == KErrNone || retVal == KErrUnknown || retVal == KErrAlreadyExists);
    Teardown();
    return KErrNone;    
	}
	
TInt CSenFragmentBCTest::MT_RSenDocument_ManualXmlEngineTlsCleanupL(TTestResult& aResult)
	{
	SetupL();
    RSenDocument document = RSenDocument::NewL();
    TInt retVal = document.ManualXmlEngineTlsCleanup();
    TL(retVal == KErrNone || retVal == KErrNotFound );
    Teardown();
    return KErrNone;    
	}


TInt CSenFragmentBCTest::MT_TestFragmentBase_SetContentLL(TTestResult& aResult)
	{
	SetupL();
    CTestFragmentBase* pFragment = CTestFragmentBase::NewL(_L8("Test"));
	pFragment->SetContentL(_L8("Test"));
    Teardown();
    return KErrNone;    
	}

	
TInt CSenFragmentBCTest::MT_TestFragmentBase_AddContentLL(TTestResult& aResult)
	{
	SetupL();
    CTestFragmentBase* pFragment = CTestFragmentBase::NewL(_L8("Test"));
	pFragment->AddContentL(_L8("Test"));
    Teardown();
    return KErrNone;    
	}
	
	
TInt CSenFragmentBCTest::MT_TestDomFragmentBase_SetContentOfLL(TTestResult& aResult)
	{
	SetupL();
	_LIT8(KText, "Test1");
    CTestDomFragmentBase* pFragment = CTestDomFragmentBase::NewL(_L8("Test"));
    pFragment->SetContentOfL(_L8("Test"), _L8("Test1"));
    TPtrC8 ptr = pFragment->ContentOf(_L8("Test"));
    TL(ptr == KText);
    Teardown();
    return KErrNone;    
	}
/*	
TInt CSenFragmentBCTest::MT_TestDomFragmentBase_ContentOfL(TTestResult& aResult)
	{
	SetupL();
    Teardown();
    return KErrNone;    
	}
	
*/

//  End of File
