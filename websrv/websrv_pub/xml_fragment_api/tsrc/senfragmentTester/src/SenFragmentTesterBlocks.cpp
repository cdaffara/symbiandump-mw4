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
#include "SenFragmentTester.h"
//  EXTERNAL INCLUDES
#include <SenDomFragmentBase.h>
#include <SenFragmentBase.h>
#include <SenParser.h>
#include <RSenDocument.h>
#include <xml/dom/xmlengdomutils.h>

#if defined( __SERIES60_31__ ) || (!defined( __SERIES60_30__ ) && defined( SYMBIAN_SWI_POST_INSTALL_REVOCATION) )
    #include <xml/matchdata.h>
#endif    

#include <xml/parserfeature.h>           // for TParserFeature enumeration
#include <xml/dom/xmlengserializationoptions.h> // needed for TSerializationOptions
#include <xml/dom/xmlengbinarycontainer.h>
#include <s32mem.h>
#include <xml/dom/xmlengdom.h>

//  INTERNAL INCLUDES
#include "maindomfragment.h"
#include "mainfragment.h"
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
// CSenFragmentTester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CSenFragmentTester::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CSenFragmentTester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CSenFragmentTester::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
		ENTRY("SDFB-NewL-test_CSenDomFragmentBase_NewL", MT_CSenDomFragmentBase_NewLL),
		ENTRY("SDFB-NewL1-test_CSenDomFragmentBase_NewL", MT_CSenDomFragmentBase_NewL_1L),
		ENTRY("SDFB-NewL2-test_CSenDomFragmentBase_NewL", CSenFragmentTester::MT_CSenDomFragmentBase_NewL_2L),
		ENTRY("SDFB-NewL3-test_CSenDomFragmentBase_NewL", CSenFragmentTester::MT_CSenDomFragmentBase_NewL_3L),
		ENTRY("SDFB-NewL4-test_CSenDomFragmentBase_NewL", CSenFragmentTester::MT_CSenDomFragmentBase_NewL_4L),
		ENTRY("SDFB-NewL5-test_CSenDomFragmentBase_NewL", CSenFragmentTester::MT_CSenDomFragmentBase_NewL_5L),
		ENTRY("SDFB-NewL6-test_CSenDomFragmentBase_NewL", CSenFragmentTester::MT_CSenDomFragmentBase_NewL_6L),
		ENTRY("SDFB-NewL7-test_CSenDomFragmentBase_NewL", CSenFragmentTester::MT_CSenDomFragmentBase_NewL_7L),
		ENTRY("SDFB-ExpandL-test_CSenDomFragmentBase_ExpandL", CSenFragmentTester::MT_CSenDomFragmentBase_ExpandLL),
		ENTRY("SDFB-OnResumeParsingFromL-test_CSenDomFragmentBase_OnResumeParsingFromL", CSenFragmentTester::MT_CSenDomFragmentBase_OnResumeParsingFromLL),
		ENTRY("SDFB-AddAttributesL-test_CSenDomFragmentBase_AddAttributesL", CSenFragmentTester::MT_CSenDomFragmentBase_AddAttributesLL),
		ENTRY("SDFB-AsXmlL-test_CSenDomFragmentBase_AsXmlL", CSenFragmentTester::MT_CSenDomFragmentBase_AsXmlLL),
		ENTRY("SDFB-OnStartElementL-test_CSenDomFragmentBase_OnStartElementL", CSenFragmentTester::MT_CSenDomFragmentBase_OnStartElementLL),
		ENTRY("SDFB-OnContentL-test_CSenDomFragmentBase_OnContentL", CSenFragmentTester::MT_CSenDomFragmentBase_OnContentLL),
		ENTRY("SDFB-OnWriteStartElementL-test_CSenDomFragmentBase_OnWriteStartElementL", CSenFragmentTester::MT_CSenDomFragmentBase_OnWriteStartElementLL),
		ENTRY("SDFB-OnWriteEndElementL-test_CSenDomFragmentBase_OnWriteEndElementL", CSenFragmentTester::MT_CSenDomFragmentBase_OnWriteEndElementLL),
		ENTRY("SDFB-OnEndElementL-test_CSenDomFragmentBase_OnEndElementL", CSenFragmentTester::MT_CSenDomFragmentBase_OnEndElementLL),
		ENTRY("SDFB-OnStartDocumentL-test_CSenDomFragmentBase_OnStartDocumentL", CSenFragmentTester::MT_CSenDomFragmentBase_OnStartDocumentLL),
		ENTRY("SDFB-OnEndDocumentL-test_CSenDomFragmentBase_OnEndDocumentL", CSenFragmentTester::MT_CSenDomFragmentBase_OnEndDocumentLL),
		ENTRY("SDFB-OnStartPrefixMappingL-test_CSenDomFragmentBase_OnStartPrefixMappingL", CSenFragmentTester::MT_CSenDomFragmentBase_OnStartPrefixMappingLL),
		ENTRY("SDFB-OnEndPrefixMappingL-test_CSenDomFragmentBase_OnEndPrefixMappingL", CSenFragmentTester::MT_CSenDomFragmentBase_OnEndPrefixMappingLL),
		ENTRY("SDFB-OnIgnorableWhiteSpaceL-test_CSenDomFragmentBase_OnIgnorableWhiteSpaceL", CSenFragmentTester::MT_CSenDomFragmentBase_OnIgnorableWhiteSpaceLL),
		ENTRY("SDFB-OnSkippedEntityL-test_CSenDomFragmentBase_OnSkippedEntityL", CSenFragmentTester::MT_CSenDomFragmentBase_OnSkippedEntityLL),
		ENTRY("SDFB-OnProcessingInstructionL-test_CSenDomFragmentBase_OnProcessingInstructionL", CSenFragmentTester::MT_CSenDomFragmentBase_OnProcessingInstructionLL),
		ENTRY("SDFB-OnError-test_CSenDomFragmentBase_OnError", CSenFragmentTester::MT_CSenDomFragmentBase_OnErrorL),
		ENTRY("SDFB-GetExtendedInterface-test_CSenDomFragmentBase_GetExtendedInterface", CSenFragmentTester::MT_CSenDomFragmentBase_GetExtendedInterfaceL),
		ENTRY("SDFB-ContentL-test_CSenDomFragmentBase_ContentL", CSenFragmentTester::MT_CSenDomFragmentBase_ContentLL),
		ENTRY("SDFB-Namespace-test_CSenDomFragmentBase_Namespace", CSenFragmentTester::MT_CSenDomFragmentBase_NamespaceL),
		ENTRY("SDFB-ResetContentL-test_CSenDomFragmentBase_ResetContentL", CSenFragmentTester::MT_CSenDomFragmentBase_ResetContentLL),
		ENTRY("SDFB-LocalName-test_CSenDomFragmentBase_LocalName", CSenFragmentTester::MT_CSenDomFragmentBase_LocalNameL),
		ENTRY("SDFB-NsUri-test_CSenDomFragmentBase_NsUri", CSenFragmentTester::MT_CSenDomFragmentBase_NsUriL),
		ENTRY("SDFB-NsPrefix-test_CSenDomFragmentBase_NsPrefix", CSenFragmentTester::MT_CSenDomFragmentBase_NsPrefixL),
		ENTRY("SDFB-AsElementL-test_CSenDomFragmentBase_AsElementL", CSenFragmentTester::MT_CSenDomFragmentBase_AsElementLL),
		ENTRY("SDFB-AsDocumentL-test_CSenDomFragmentBase_AsDocumentL", CSenFragmentTester::MT_CSenDomFragmentBase_AsDocumentLL),
		ENTRY("SDFB-ExtractElement-test_CSenDomFragmentBase_ExtractElement", CSenFragmentTester::MT_CSenDomFragmentBase_ExtractElementL),
		ENTRY("SDFB-OnDelegateParsingL-test_CSenDomFragmentBase_OnDelegateParsingL", CSenFragmentTester::MT_CSenDomFragmentBase_OnDelegateParsingLL),
		ENTRY("SDFB-OnDelegateParsingL-test_CSenDomFragmentBase_OnDelegateParsingL", CSenFragmentTester::MT_CSenDomFragmentBase_OnDelegateParsingL_1L),
		ENTRY("SDFB-SetOwner-test_CSenDomFragmentBase_SetOwner", CSenFragmentTester::MT_CSenDomFragmentBase_SetOwnerL),
		ENTRY("SDFB-AsXmlUnicodeL-test_CSenDomFragmentBase_AsXmlUnicodeL", CSenFragmentTester::MT_CSenDomFragmentBase_AsXmlUnicodeLL),
		ENTRY("SDFB-WriteAsXMLToL-test_CSenDomFragmentBase_WriteAsXMLToL", CSenFragmentTester::MT_CSenDomFragmentBase_WriteAsXMLToLL),
		ENTRY("SDFB-ConsistsOfL-test_CSenDomFragmentBase_ConsistsOfL", CSenFragmentTester::MT_CSenDomFragmentBase_ConsistsOfLL),
		ENTRY("SDFB-SetContentHandler-test_CSenDomFragmentBase_SetContentHandler", CSenFragmentTester::MT_CSenDomFragmentBase_SetContentHandlerL),
		ENTRY("SDFB-Parsing-test_CSenDomFragmentBase_Parsing", CSenFragmentTester::MT_CSenDomFragmentBase_Parsing1L),
		ENTRY("SDFB-Parsingdelegate-test_CSenDomFragmentBase_Parsing", CSenFragmentTester::MT_CSenDomFragmentBase_Parsing2L),
		ENTRY("SDFB-Parsingnamespaces-test_CSenDomFragmentBase_Parsing", CSenFragmentTester::MT_CSenDomFragmentBase_Parsing3L),
		ENTRY("SDFB-Parsingnestedelements-test_CSenDomFragmentBase_Parsing", CSenFragmentTester::MT_CSenDomFragmentBase_Parsing4L),
		ENTRY("SDFB-Parsingnullednamespaces-test_CSenDomFragmentBase_Parsing", CSenFragmentTester::MT_CSenDomFragmentBase_Parsing5L),
		ENTRY("SDFB-Parsingusingemptyfragment-test_CSenDomFragmentBase_Parsing", CSenFragmentTester::MT_CSenDomFragmentBase_Parsing6L),
		ENTRY("SDFB-ParsingWSmessage-test_CSenDomFragmentBase_Parsing", CSenFragmentTester::MT_CSenDomFragmentBase_Parsing7L),
		ENTRY("SDFB-Parsing2.WSmessage-test_CSenDomFragmentBase_Parsing", CSenFragmentTester::MT_CSenDomFragmentBase_Parsing8L),
		ENTRY("SFB-NewL-test_CSenFragmentBase_NewL", CSenFragmentTester::MT_CSenFragmentBase_NewLL),
		ENTRY("SFB-NewL1-test_CSenFragmentBase_NewL", CSenFragmentTester::MT_CSenFragmentBase_NewL_1L),
		ENTRY("SFB-NewL2-test_CSenFragmentBase_NewL", CSenFragmentTester::MT_CSenFragmentBase_NewL_2L),
		ENTRY("SFB-NewL3-test_CSenFragmentBase_NewL", CSenFragmentTester::MT_CSenFragmentBase_NewL_3L),
		ENTRY("SFB-NewL4-test_CSenFragmentBase_NewL", CSenFragmentTester::MT_CSenFragmentBase_NewL_4L),
		ENTRY("SFB-NewL5-test_CSenFragmentBase_NewL", CSenFragmentTester::MT_CSenFragmentBase_NewL_5L),
		ENTRY("SFB-NewL6-test_CSenFragmentBase_NewL", CSenFragmentTester::MT_CSenFragmentBase_NewL_6L),
		ENTRY("SFB-NewL7-test_CSenFragmentBase_NewL", CSenFragmentTester::MT_CSenFragmentBase_NewL_7L),
		ENTRY("SFB-OnStartElementL-test_CSenFragmentBase_OnStartElementL", CSenFragmentTester::MT_CSenFragmentBase_OnStartElementLL),
		ENTRY("SFB-OnEndElementL-test_CSenFragmentBase_OnEndElementL", CSenFragmentTester::MT_CSenFragmentBase_OnEndElementLL),
		ENTRY("SFB-OnStartDocumentL-test_CSenFragmentBase_OnStartDocumentL", CSenFragmentTester::MT_CSenFragmentBase_OnStartDocumentLL),
		ENTRY("SFB-OnEndDocumentL-test_CSenFragmentBase_OnEndDocumentL", CSenFragmentTester::MT_CSenFragmentBase_OnEndDocumentLL),
		ENTRY("SFB-OnContentL-test_CSenFragmentBase_OnContentL", CSenFragmentTester::MT_CSenFragmentBase_OnContentLL),
		ENTRY("SFB-OnStartPrefixMappingL-test_CSenFragmentBase_OnStartPrefixMappingL", CSenFragmentTester::MT_CSenFragmentBase_OnStartPrefixMappingLL),
		ENTRY("SFB-OnEndPrefixMappingL-test_CSenFragmentBase_OnEndPrefixMappingL", CSenFragmentTester::MT_CSenFragmentBase_OnEndPrefixMappingLL),
		ENTRY("SFB-OnIgnorableWhiteSpaceL-test_CSenFragmentBase_OnIgnorableWhiteSpaceL", CSenFragmentTester::MT_CSenFragmentBase_OnIgnorableWhiteSpaceLL),
		ENTRY("SFB-OnSkippedEntityL-test_CSenFragmentBase_OnSkippedEntityL", CSenFragmentTester::MT_CSenFragmentBase_OnSkippedEntityLL),
		ENTRY("SFB-OnProcessingInstructionL-test_CSenFragmentBase_OnProcessingInstructionL", CSenFragmentTester::MT_CSenFragmentBase_OnProcessingInstructionLL),
		ENTRY("SFB-OnError-test_CSenFragmentBase_OnError", CSenFragmentTester::MT_CSenFragmentBase_OnErrorL),
		ENTRY("SFB-GetExtendedInterface-test_CSenFragmentBase_GetExtendedInterface", CSenFragmentTester::MT_CSenFragmentBase_GetExtendedInterfaceL),
		ENTRY("SFB-ContentL-test_CSenFragmentBase_ContentL", CSenFragmentTester::MT_CSenFragmentBase_ContentLL),
		ENTRY("SFB-Namespace-test_CSenFragmentBase_Namespace", CSenFragmentTester::MT_CSenFragmentBase_NamespaceL),
		ENTRY("SFB-ResetContentL-test_CSenFragmentBase_ResetContentL", CSenFragmentTester::MT_CSenFragmentBase_ResetContentLL),
		ENTRY("SFB-LocalName-test_CSenFragmentBase_LocalName", CSenFragmentTester::MT_CSenFragmentBase_LocalNameL),
		ENTRY("SFB-NsUri-test_CSenFragmentBase_NsUri", CSenFragmentTester::MT_CSenFragmentBase_NsUriL),
		ENTRY("SFB-NsPrefix-test_CSenFragmentBase_NsPrefix", CSenFragmentTester::MT_CSenFragmentBase_NsPrefixL),
		ENTRY("SFB-AsElementL-test_CSenFragmentBase_AsElementL", CSenFragmentTester::MT_CSenFragmentBase_AsElementLL),
		ENTRY("SFB-AsDocumentL-test_CSenFragmentBase_AsDocumentL", CSenFragmentTester::MT_CSenFragmentBase_AsDocumentLL),
		ENTRY("SFB-ExtractElement-test_CSenFragmentBase_ExtractElement", CSenFragmentTester::MT_CSenFragmentBase_ExtractElementL),
		ENTRY("SFB-OnDelegateParsingL-test_CSenFragmentBase_OnDelegateParsingL", CSenFragmentTester::MT_CSenFragmentBase_OnDelegateParsingLL),
		ENTRY("SFB-OnDelegateParsingL-test_CSenFragmentBase_OnDelegateParsingL", CSenFragmentTester::MT_CSenFragmentBase_OnDelegateParsingL_1L),
		ENTRY("SFB-SetOwner-test_CSenFragmentBase_SetOwner", CSenFragmentTester::MT_CSenFragmentBase_SetOwnerL),
		ENTRY("SFB-OnResumeParsingFromL-test_CSenFragmentBase_OnResumeParsingFromL", CSenFragmentTester::MT_CSenFragmentBase_OnResumeParsingFromLL),
		ENTRY("SFB-OnWriteStartElementL-test_CSenFragmentBase_OnWriteStartElementL", CSenFragmentTester::MT_CSenFragmentBase_OnWriteStartElementLL),
		ENTRY("SFB-OnWriteEndElementL-test_CSenFragmentBase_OnWriteEndElementL", CSenFragmentTester::MT_CSenFragmentBase_OnWriteEndElementLL),
		ENTRY("SFB-AsXmlUnicodeL-test_CSenFragmentBase_AsXmlUnicodeL", CSenFragmentTester::MT_CSenFragmentBase_AsXmlUnicodeLL),
		ENTRY("SFB-AsXmlL-test_CSenFragmentBase_AsXmlL", CSenFragmentTester::MT_CSenFragmentBase_AsXmlLL),
		ENTRY("SFB-WriteAsXMLToL-test_CSenFragmentBase_WriteAsXMLToL", CSenFragmentTester::MT_CSenFragmentBase_WriteAsXMLToLL),
		ENTRY("SFB-ConsistsOfL-test_CSenFragmentBase_ConsistsOfL", CSenFragmentTester::MT_CSenFragmentBase_ConsistsOfLL),
		ENTRY("SFB-SetContentHandler-test_CSenFragmentBase_SetContentHandler", CSenFragmentTester::MT_CSenFragmentBase_SetContentHandlerL),
		ENTRY("SFB-Parsing-test_CSenFragmentBase_Parsing", CSenFragmentTester::MT_CSenFragmentBase_Parsing1L),
		ENTRY("SFB-Parsingdelegate-test_CSenFragmentBase_Parsing", CSenFragmentTester::MT_CSenFragmentBase_Parsing2L),
		ENTRY("SFB-Parsingnamespaces-test_CSenFragmentBase_Parsing", CSenFragmentTester::MT_CSenFragmentBase_Parsing3L),
		ENTRY("SFB-Parsingnestedelements-test_CSenFragmentBase_Parsing", CSenFragmentTester::MT_CSenFragmentBase_Parsing4L),
		ENTRY("SFB-ParsingelementinsideDOMtree-test_CSenFragmentBase_Parsing", CSenFragmentTester::MT_CSenFragmentBase_Parsing5L),
		ENTRY("CSP-NewL-test_CSenParser_NewL", CSenFragmentTester::MT_CSenParser_NewLL),
		ENTRY("CSP-NewLC-test_CSenParser_NewLC", CSenFragmentTester::MT_CSenParser_NewLCL),
		ENTRY("CSP-NewL1-test_CSenParser_NewL", CSenFragmentTester::MT_CSenParser_NewL_1L),
		ENTRY("CSP-NewLC1-test_CSenParser_NewLC", CSenFragmentTester::MT_CSenParser_NewLC_1L),
		ENTRY("CSP-NewL2-test_CSenParser_NewL", CSenFragmentTester::MT_CSenParser_NewL_2L),
		ENTRY("CSP-NewLC2-test_CSenParser_NewLC", CSenFragmentTester::MT_CSenParser_NewLC_2L),
		ENTRY("RSD-NewL-test_RSenDocument_NewL", MT_RSenDocument_NewLL),
		ENTRY("RSD-NewLC-test_RSenDocument_NewLC", MT_RSenDocument_NewLCL),
		ENTRY("RSD-NewL2-test_RSenDocument_NewL", MT_RSenDocument_NewL_1L),
		ENTRY("RSD-NewLC2-test_RSenDocument_NewLC", MT_RSenDocument_NewLC_1L),
		ENTRY("RSD-Copy-test_RSenDocument_Copy", MT_RSenDocument_CopyL),
		ENTRY("RSD-Close-test_RSenDocument_Close", MT_RSenDocument_CloseL),
		ENTRY("RSD-Destroy-test_RSenDocument_Destroy", MT_RSenDocument_DestroyL),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_NewLL(  )
    {
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL();
    delete pFragment;
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_NewL_1L(  )
    {
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
    LOCAL_ASSERT( *pAsXml == _L8("<Test>&lt;Element/&gt;</Test>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&document);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_NewL_2L(  )
    {
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    LOCAL_ASSERT( *pAsXml == _L8("<Test/>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_NewL_3L(  )
    {
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"));
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    LOCAL_ASSERT( *pAsXml == _L8("<Test xmlns=\"NsUri\"/>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_NewL_4L(  )
    {
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    LOCAL_ASSERT( *pAsXml == _L8("<t:Test xmlns:t=\"NsUri\"/>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_NewL_5L(  )
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
    LOCAL_ASSERT( *pAsXml == _L8("<t:Test xmlns:t=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_NewL_6L(  )
    {
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
    LOCAL_ASSERT( *pAsXml == _L8("<t:Test xmlns:t=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") );

    RBuf8 buffer;
    parentElement.OuterXmlL(buffer);
    
    LOCAL_ASSERT( buffer == _L8("<Parent><t:Test xmlns:t=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/></Parent>") );
    buffer.Close();
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    CleanupStack::PopAndDestroy(&document);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_NewL_7L(  )
    {
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
    LOCAL_ASSERT( *pAsXml == _L8("<Test xmlns=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") );

    RBuf8 buffer;
    parentElement.OuterXmlL(buffer);
    
    LOCAL_ASSERT( buffer == _L8("<Parent><Test xmlns=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/></Parent>") );
    buffer.Close();
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    CleanupStack::PopAndDestroy(&document);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_ExpandLL(  )
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnResumeParsingFromLL(  )
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_AddAttributesLL(  )
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
    LOCAL_ASSERT( *pAsXml == _L8("<Test xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") );
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pFragment);
    
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_AsXmlLL(  )
    {
    // Test serialization of Dom tree which has two child elements
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement element2 = element.AddNewElementL(_L8("Child"), _L8("nsuri"), _L8("prefix"));

    element2.AddNewElementL(_L8("Child2"), _L8("nsuri2"), _L8("prefix2"));
    
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    LOCAL_ASSERT( *pAsXml == _L8("<Test xmlns:prefix=\"nsuri\"><prefix:Child xmlns:prefix2=\"nsuri2\"><prefix2:Child2/></prefix:Child></Test>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnStartElementLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnContentLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnWriteStartElementLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnWriteEndElementLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnEndElementLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnStartDocumentLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnEndDocumentLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnStartPrefixMappingLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnEndPrefixMappingLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnIgnorableWhiteSpaceLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnSkippedEntityLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnProcessingInstructionLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnErrorL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_GetExtendedInterfaceL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_ContentLL(  )
    {
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement element = document.CreateDocumentElementL(_L8("Test"));
    element.SetEscapedTextL(_L8("<Element/>"));
    
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(element);
    CleanupStack::PushL(pFragment);
    
    TPtrC8 content = pFragment->ContentL();
    LOCAL_ASSERT( content == _L8("<Element/>") );
    
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&document);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_NamespaceL(  )
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
    LOCAL_ASSERT( ns.Uri() == _L8("nsuri") );

    TXmlEngNamespace ns2 = pFragment->Namespace(_L8("x"));
    LOCAL_ASSERT( ns2.IsNull() );
    
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(); // attribute
    CleanupStack::PopAndDestroy(); // attrArray
    CleanupStack::PopAndDestroy(); // stringPool    
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_ResetContentLL(  )
    {
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement element = document.CreateDocumentElementL(_L8("Test"));
    element.SetEscapedTextL(_L8("Content"));
    
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(element);
    CleanupStack::PushL(pFragment);
    
    // Test that CSenFragmentBase has content before reset
    TPtrC8 content = pFragment->ContentL();
    LOCAL_ASSERT( content == _L8("Content") );
    
    pFragment->ResetContentL();
    
    // Test that CSenFragmentBase has no content after reset
    TPtrC8 content2 = pFragment->ContentL();
    LOCAL_ASSERT( content2 == KNullDesC8 );
    
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&document);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_LocalNameL(  )
    {
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    TPtrC8 localName = pFragment->LocalName();
    LOCAL_ASSERT( localName == _L8("Test") );
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_NsUriL(  )
    {
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    TPtrC8 localName = pFragment->NsUri();
    LOCAL_ASSERT( localName == _L8("NsUri") );
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_NsPrefixL(  )
    {
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    TPtrC8 localName = pFragment->NsPrefix();
    LOCAL_ASSERT( localName == _L8("t") );
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_AsElementLL(  )
    {
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    RBuf8 buffer;
    CleanupClosePushL(buffer);
    element.OuterXmlL(buffer);
    // Serialized element should contain all the Fragment data as XML.
    LOCAL_ASSERT( buffer == _L8("<t:Test xmlns:t=\"NsUri\"/>") );
    CleanupStack::PopAndDestroy(&buffer);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_AsDocumentLL(  )
    {
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
    LOCAL_ASSERT( asXml == _L8("<t:Test xmlns:t=\"NsUri\"/>") );

    CleanupStack::PopAndDestroy(&asXml);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_ExtractElementL(  )
    {
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("NsUri"),
                                                               _L8("Test"),
                                                               _L8("t"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->ExtractElement();
    LOCAL_ASSERT( element.Name() == _L8("Test") );
    element.Remove();
    CleanupStack::PopAndDestroy(pFragment);    
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnDelegateParsingLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_OnDelegateParsingL_1L(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_SetOwnerL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_AsXmlUnicodeLL(  )
    {
    // Test serialization of Dom tree which has two child elements
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement element2 = element.AddNewElementL(_L8("Child"), _L8("nsuri"), _L8("prefix"));

    element2.AddNewElementL(_L8("Child2"), _L8("nsuri2"), _L8("prefix2"));
    
    HBufC* pAsXml = pFragment->AsXmlUnicodeL();
    CleanupStack::PushL(pAsXml);
    LOCAL_ASSERT( *pAsXml == _L("<Test xmlns:prefix=\"nsuri\"><prefix:Child xmlns:prefix2=\"nsuri2\"><prefix2:Child2/></prefix:Child></Test>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_WriteAsXMLToLL(  )
    {
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


    LOCAL_ASSERT( pBuf->Ptr(0) == _L8("<Test xmlns:prefix=\"nsuri\"><prefix:Child xmlns:prefix2=\"nsuri2\"><prefix2:Child2/></prefix:Child></Test>") );
    
    CleanupStack::PopAndDestroy(&bufWs);
    CleanupStack::PopAndDestroy(pBuf);
    
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_ConsistsOfLL(  )
    {
    TDomUtils domUtils;
    CSenDomFragmentBase* pFragment = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement elementchild1 = element.AddNewElementL(_L8("DirectChild"));
    elementchild1.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //elementchild1.RenameNodeL(_L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    
    domUtils.XmlEngRenameElementL(elementchild1, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));

    TXmlEngElement elementchild2 = element.AddNewElementL(_L8("DirectChild"));
    elementchild2.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //elementchild2.RenameNodeL(_L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    domUtils.XmlEngRenameElementL(elementchild2, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));

    elementchild2.AddNewElementL(_L8("Child"), _L8("nsuri2"), _L8("prefix2"));

    CSenDomFragmentBase* pFragment2 = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment2);
    TXmlEngElement element2 = pFragment2->AsElementL();
    
    TXmlEngElement elementchild22 = element2.AddNewElementL(_L8("DirectChild"));
    elementchild22.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //elementchild22.RenameNodeL(_L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    domUtils.XmlEngRenameElementL(elementchild22, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));

    elementchild22.AddNewElementL(_L8("Child"), _L8("nsuri2"), _L8("prefix2"));

    // ConsistOfL should return ETrue because
    // root elements of both fragments match and the only element
    // of pFragment2 = elementchild22 is exactly the same element as
    // elementchild2.
    LOCAL_ASSERT( pFragment->ConsistsOfL(*pFragment2) );

    // ConsistOfL should return EFalse because
    // Even though root elements of both fragments match
    // there is extra element (= elementchild1) in pFragment.
    //
    // elementchild1 (of pFragment) can not be found from pFragment2
    // and because of that pFragment2 does not consist of pFragment.
    LOCAL_ASSERT( !pFragment2->ConsistsOfL(*pFragment) );
    CleanupStack::PopAndDestroy(pFragment2);

    CSenDomFragmentBase* pFragment3 = CSenDomFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment3);
    TXmlEngElement element3 = pFragment3->AsElementL();
    
    TXmlEngElement elementchild32 = element3.AddNewElementL(_L8("DirectChild"));
    elementchild32.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //elementchild32.RenameNodeL(_L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    domUtils.XmlEngRenameElementL(elementchild32, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));

    elementchild32.AddNewElementL(_L8("Child"), _L8("nsuri2"), _L8("prefix2"));
    elementchild32.SetEscapedTextL(_L8("Text"));
    
    // ConsistOfL should return EFalse because
    // elementchild32 has content ("Text") which doesn't match
    // to any child element of pFragment.
    LOCAL_ASSERT( !pFragment->ConsistsOfL(*pFragment3) );
    CleanupStack::PopAndDestroy(pFragment3);
    
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_SetContentHandlerL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_Parsing1L(  )
    {
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

    LOCAL_ASSERT( *pAsXml == KOutputString1 );

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
/*
	pParser->EnableFeature(EReportNamespaceMapping);
	//tma
	pParser->DisableFeature(EReportNamespaceMapping);
	TBool fEneabled(EFalse);
	fEneabled = pParser->IsFeatureEnabled(EReportNamespaceMapping);
	LOCAL_ASSERT( fEneabled == EFalse);
	pParser->EnableFeature(EReportNamespaceMapping);
	//tma
*/	
	pParser->EnableFeature(EReportNamespaceMapping);
	
    CSenDomFragmentBase* pFragment2 = CSenDomFragmentBase::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment2);

	pParser->ParseL(KInputString, *pFragment2);
	
	HBufC8* pAsXml2 = pFragment2->AsXmlL();
	CleanupStack::PushL(pAsXml2);
	                   
    LOCAL_ASSERT( *pAsXml2 == KOutputString2 );
    
    CleanupStack::PopAndDestroy(pAsXml2);
    CleanupStack::PopAndDestroy(pFragment2);

    CleanupStack::PopAndDestroy(pParser);
    return KErrNone;
    }
    
TInt CSenFragmentTester::MT_CSenDomFragmentBase_Parsing2L(  )
    {
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
    LOCAL_ASSERT( *pAsXml == KOutputString1 );
    CleanupStack::PopAndDestroy(pAsXml);
    
    CDelegateDomFragment& delegate = pFragment->DelegateFragment();
    HBufC8* pDelegateAsXml = delegate.AsXmlL();
    CleanupStack::PushL(pDelegateAsXml);
    // Note: When EReportNamespaceMapping is _NOT_ enabled namespace declaration
    //       is added to delegate even though that namespace declaration was
    //       originally declared (but not used) in root element of parsed document. 
    LOCAL_ASSERT( *pDelegateAsXml ==
                 _L8("<DelegateFragment xmlns=\"nsuri\">DelegateContent</DelegateFragment>") );
    CleanupStack::PopAndDestroy(pDelegateAsXml);
    
    CleanupStack::PopAndDestroy(pFragment);

	pParser->EnableFeature(EReportNamespaceMapping);
	
    CMainDomFragment* pFragment2 = CMainDomFragment::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment2);

	pParser->ParseL(KInputString, *pFragment2);
	
	HBufC8* pAsXml2 = pFragment2->AsXmlL();
	CleanupStack::PushL(pAsXml2);
    LOCAL_ASSERT( *pAsXml2 == KOutputString2 );
    CleanupStack::PopAndDestroy(pAsXml2);
    
    CDelegateDomFragment& delegate2 = pFragment2->DelegateFragment();
    HBufC8* pDelegateAsXml2 = delegate2.AsXmlL();
    CleanupStack::PushL(pDelegateAsXml2);
    LOCAL_ASSERT( *pDelegateAsXml2 ==
                 _L8("<DelegateFragment>DelegateContent</DelegateFragment>") );
    CleanupStack::PopAndDestroy(pDelegateAsXml2);
    
    CleanupStack::PopAndDestroy(pFragment2);

    CleanupStack::PopAndDestroy(pParser);
    return KErrNone;
    }
    
TInt CSenFragmentTester::MT_CSenDomFragmentBase_Parsing3L(  )
    {
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

    LOCAL_ASSERT( *pAsXml == KOutputString );

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(pParser);
    return KErrNone;
    }    

TInt CSenFragmentTester::MT_CSenDomFragmentBase_Parsing4L(  )
    {
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

    LOCAL_ASSERT( *pAsXml == KOutputString );

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);

    pFragment = CSenDomFragmentBase::NewL(_L8("nsuri"),_L8("Nested"));
    CleanupStack::PushL(pFragment);
	pParser->EnableFeature(EReportNamespaceMapping);
	
	pParser->ParseL(KInputString2, *pFragment);
	
	pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);

    LOCAL_ASSERT( *pAsXml == KInputString2 );

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);

    CleanupStack::PopAndDestroy(pParser);
    return KErrNone;
    }
    
TInt CSenFragmentTester::MT_CSenDomFragmentBase_Parsing5L(  )
    {
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

    LOCAL_ASSERT( *pAsXml == KOutputString );

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);*/
    CleanupStack::PopAndDestroy(pParser);
    return KErrNone;
    }  
    
TInt CSenFragmentTester::MT_CSenDomFragmentBase_Parsing6L(  )
    {
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

    LOCAL_ASSERT( *pAsXml == KOutputString );

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);

    CleanupStack::PopAndDestroy(pParser);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_Parsing7L(  )
    {
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
    LOCAL_ASSERT( *pAsXml == KOutputString );
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
    LOCAL_ASSERT( *pAsXml == KInputString );
    CleanupStack::PopAndDestroy(pAsXml);
    
    CleanupStack::PopAndDestroy(pBase);
    CleanupStack::PopAndDestroy(parser);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenDomFragmentBase_Parsing8L(  )
    {
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
    LOCAL_ASSERT( *pAsXml == KOutputString1 );
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
    LOCAL_ASSERT( *pAsXml == KOutputString2 );
    CleanupStack::PopAndDestroy(pAsXml);
    
    CleanupStack::PopAndDestroy(pBase);
    CleanupStack::PopAndDestroy(parser);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_NewLL(  )
    {
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
    LOCAL_ASSERT( *pAsXml == _L8("<Test><Element/></Test>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&document);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_NewL_1L(  )
    {
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    LOCAL_ASSERT( *pAsXml == _L8("<Test/>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_NewL_2L(  )
    {
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"));
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    LOCAL_ASSERT( *pAsXml == _L8("<Test xmlns=\"NsUri\"/>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_NewL_3L(  )
    {
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"));
    CleanupStack::PushL(pFragment);
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    LOCAL_ASSERT( *pAsXml == _L8("<t:Test xmlns:t=\"NsUri\"/>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_NewL_4L(  )
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
    LOCAL_ASSERT( *pAsXml == _L8("<t:Test xmlns:t=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);    
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_NewL_5L(  )
    {
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
    LOCAL_ASSERT( *pAsXml == _L8("<t:Test xmlns:t=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") );

    RBuf8 buffer;
    parentElement.OuterXmlL(buffer);
    
    LOCAL_ASSERT( buffer == _L8("<Parent><t:Test xmlns:t=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/></Parent>") );
    buffer.Close();
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool); 
    CleanupStack::PopAndDestroy(&document);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_NewL_6L(  )
    {
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
    LOCAL_ASSERT( *pAsXml == _L8("<Test xmlns=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") );

    RBuf8 buffer;
    parentElement.OuterXmlL(buffer);
    
    LOCAL_ASSERT( buffer == _L8("<Parent><Test xmlns=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/></Parent>") );
    buffer.Close();
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool); 
    CleanupStack::PopAndDestroy(&document);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_NewL_7L(  )
    {
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
    LOCAL_ASSERT( *pAsXml == _L8("<Test xmlns=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/>") );

    RBuf8 buffer;
    parentElement.OuterXmlL(buffer);
    
    LOCAL_ASSERT( buffer == _L8("<Parent><Test xmlns=\"NsUri\" xmlns:pr=\"nsuri\" pr:LocalName=\"Value\"/></Parent>") );
    buffer.Close();
    
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment2);
    CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool); 
    CleanupStack::PopAndDestroy(&document);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnStartElementLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnEndElementLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnStartDocumentLL(  )
    {
    // There is no functionality in this method in CSenFragmentBase implementation
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnEndDocumentLL(  )
    {
    // There is no functionality in this method in CSenFragmentBase implementation
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnContentLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnStartPrefixMappingLL(  )
    {
    // There is no functionality in this method in CSenFragmentBase implementation
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnEndPrefixMappingLL(  )
    {
    // There is no functionality in this method in CSenFragmentBase implementation
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnIgnorableWhiteSpaceLL(  )
    {
    // There is no functionality in this method in CSenFragmentBase implementation
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnSkippedEntityLL(  )
    {
    // There is no functionality in this method in CSenFragmentBase implementation
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnProcessingInstructionLL(  )
    {
    // There is no functionality in this method in CSenFragmentBase implementation
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnErrorL(  )
    {
    // There is no functionality in this method in CSenFragmentBase implementation
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_GetExtendedInterfaceL(  )
    {
    // There is no functionality in this method in CSenFragmentBase implementation
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_ContentLL(  )
    {
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement element = document.CreateDocumentElementL(_L8("Test"));
    element.SetEscapedTextL(_L8("<Element/>"));
    
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(element);
    CleanupStack::PushL(pFragment);
    
    TPtrC8 content = pFragment->ContentL();
    LOCAL_ASSERT( content == _L8("<Element/>") );
    
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&document);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_NamespaceL(  )
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
    LOCAL_ASSERT( ns.Uri() == _L8("nsuri") );

    TXmlEngNamespace ns2 = pFragment->Namespace(_L8("x"));
    LOCAL_ASSERT( ns2.IsUndefined() );
    
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(); // attribute
    CleanupStack::PopAndDestroy(); // attrArray
    CleanupStack::PopAndDestroy(); // stringPool    
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_ResetContentLL(  )
    {
    RSenDocument document = RSenDocument::NewL();
    CleanupClosePushL(document);
    TXmlEngElement element = document.CreateDocumentElementL(_L8("Test"));
    element.SetEscapedTextL(_L8("Content"));
    
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(element);
    CleanupStack::PushL(pFragment);
    
    // Test that CSenFragmentBase has content before reset
    TPtrC8 content = pFragment->ContentL();
    LOCAL_ASSERT( content == _L8("Content") );
    
    pFragment->ResetContentL();
    
    // Test that CSenFragmentBase has no content after reset
    TPtrC8 content2 = pFragment->ContentL();
    LOCAL_ASSERT( content2 == KNullDesC8 );
    
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(&document);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_LocalNameL(  )
    {
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"));
    CleanupStack::PushL(pFragment);
    TPtrC8 localName = pFragment->LocalName();
    LOCAL_ASSERT( localName == _L8("Test") );
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_NsUriL(  )
    {
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"));
    CleanupStack::PushL(pFragment);
    TPtrC8 localName = pFragment->NsUri();
    LOCAL_ASSERT( localName == _L8("NsUri") );
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_NsPrefixL(  )
    {
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"));
    CleanupStack::PushL(pFragment);
    TPtrC8 localName = pFragment->NsPrefix();
    LOCAL_ASSERT( localName == _L8("t") );
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_AsElementLL(  )
    {
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    RBuf8 buffer;
    CleanupClosePushL(buffer);
    element.OuterXmlL(buffer);
    // Serialized element should contain all the Fragment data as XML.
    LOCAL_ASSERT( buffer == _L8("<t:Test xmlns:t=\"NsUri\"/>") );
    CleanupStack::PopAndDestroy(&buffer);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_AsDocumentLL(  )
    {
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
    LOCAL_ASSERT( asXml == _L8("<t:Test xmlns:t=\"NsUri\"/>") );

    CleanupStack::PopAndDestroy(&asXml);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_ExtractElementL(  )
    {
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("NsUri"),
                                                         _L8("Test"),
                                                         _L8("t"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->ExtractElement();
    LOCAL_ASSERT( element.Name() == _L8("Test") );
    element.Remove();
    CleanupStack::PopAndDestroy(pFragment);    
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnDelegateParsingLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnDelegateParsingL_1L(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_SetOwnerL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnResumeParsingFromLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnWriteStartElementLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_OnWriteEndElementLL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_AsXmlUnicodeLL(  )
    {
    // Test serialization of Dom tree which has two child elements
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement element2 = element.AddNewElementL(_L8("Child"), _L8("nsuri"), _L8("prefix"));

    element2.AddNewElementL(_L8("Child2"), _L8("nsuri2"), _L8("prefix2"));
    
    HBufC* pAsXml = pFragment->AsXmlUnicodeL();
    CleanupStack::PushL(pAsXml);
    LOCAL_ASSERT( *pAsXml == _L("<Test xmlns:prefix=\"nsuri\"><prefix:Child xmlns:prefix2=\"nsuri2\"><prefix2:Child2/></prefix:Child></Test>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_AsXmlLL(  )
    {
    // Test serialization of Dom tree which has two child elements
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement element2 = element.AddNewElementL(_L8("Child"), _L8("nsuri"), _L8("prefix"));

    element2.AddNewElementL(_L8("Child2"), _L8("nsuri2"), _L8("prefix2"));
    
    HBufC8* pAsXml = pFragment->AsXmlL();
    CleanupStack::PushL(pAsXml);
    LOCAL_ASSERT( *pAsXml == _L8("<Test xmlns:prefix=\"nsuri\"><prefix:Child xmlns:prefix2=\"nsuri2\"><prefix2:Child2/></prefix:Child></Test>") );
    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_WriteAsXMLToLL(  )
    {
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


    LOCAL_ASSERT( pBuf->Ptr(0) == _L8("<Test xmlns:prefix=\"nsuri\"><prefix:Child xmlns:prefix2=\"nsuri2\"><prefix2:Child2/></prefix:Child></Test>") );
    
    CleanupStack::PopAndDestroy(&bufWs);
    CleanupStack::PopAndDestroy(pBuf);
    
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_ConsistsOfLL(  )
    {
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    TXmlEngElement element = pFragment->AsElementL();
    
    TXmlEngElement elementchild1 = element.AddNewElementL(_L8("DirectChild"));
    elementchild1.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //elementchild1.RenameNodeL(_L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    TDomUtils domUtils;
    domUtils.XmlEngRenameElementL(elementchild1, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));


    TXmlEngElement elementchild2 = element.AddNewElementL(_L8("DirectChild"));
    elementchild2.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //elementchild2.RenameNodeL(_L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    domUtils.XmlEngRenameElementL(elementchild2, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));

    elementchild2.AddNewElementL(_L8("Child"), _L8("nsuri2"), _L8("prefix2"));

    CSenFragmentBase* pFragment2 = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment2);
    TXmlEngElement element2 = pFragment2->AsElementL();
    
    TXmlEngElement elementchild22 = element2.AddNewElementL(_L8("DirectChild"));
    elementchild22.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //elementchild22.RenameNodeL(_L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    domUtils.XmlEngRenameElementL(elementchild22, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));

    elementchild22.AddNewElementL(_L8("Child"), _L8("nsuri2"), _L8("prefix2"));

    // ConsistOfL should return ETrue because
    // root elements of both fragments match and the only element
    // of pFragment2 = elementchild22 is exactly the same element as
    // elementchild2.
    LOCAL_ASSERT( pFragment->ConsistsOfL(*pFragment2) );

    // ConsistOfL should return EFalse because
    // Even though root elements of both fragments match
    // there is extra element (= elementchild1) in pFragment.
    //
    // elementchild1 (of pFragment) can not be found from pFragment2
    // and because of that pFragment2 does not consist of pFragment.
    LOCAL_ASSERT( !pFragment2->ConsistsOfL(*pFragment) );
    CleanupStack::PopAndDestroy(pFragment2);

    CSenFragmentBase* pFragment3 = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment3);
    TXmlEngElement element3 = pFragment3->AsElementL();
    
    TXmlEngElement elementchild32 = element3.AddNewElementL(_L8("DirectChild"));
    elementchild32.AddNamespaceDeclarationL(_L8("nsuri"), _L8("prefix"));
    //elementchild32.RenameNodeL(_L8("DirectChild"), _L8("nsuri"), _L8("prefix"));
    domUtils.XmlEngRenameElementL(elementchild32, _L8("DirectChild"), _L8("nsuri"), _L8("prefix"));

    elementchild32.AddNewElementL(_L8("Child"), _L8("nsuri2"), _L8("prefix2"));
    elementchild32.SetEscapedTextL(_L8("Text"));
    
    // ConsistOfL should return EFalse because
    // elementchild32 has content ("Text") which doesn't match
    // to any child element of pFragment.
    LOCAL_ASSERT( !pFragment->ConsistsOfL(*pFragment3) );
    CleanupStack::PopAndDestroy(pFragment3);
    
    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenFragmentBase_SetContentHandlerL(  )
    {
    // Will be tested in Parse test cases
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }
    
TInt CSenFragmentTester::MT_CSenFragmentBase_Parsing1L(  )
    {
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

    LOCAL_ASSERT( *pAsXml == KOutputString1 );

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);

	pParser->EnableFeature(EReportNamespaceMapping);
	
    CSenFragmentBase* pFragment2 = CSenFragmentBase::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment2);

	pParser->ParseL(KInputString, *pFragment2);
	
	HBufC8* pAsXml2 = pFragment2->AsXmlL();
	CleanupStack::PushL(pAsXml2);
	                   
    LOCAL_ASSERT( *pAsXml2 == KOutputString2 );
    
    CleanupStack::PopAndDestroy(pAsXml2);
    CleanupStack::PopAndDestroy(pFragment2);

    CleanupStack::PopAndDestroy(pParser);
    return KErrNone;
    }
    
TInt CSenFragmentTester::MT_CSenFragmentBase_Parsing2L(  )
    {
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
    LOCAL_ASSERT( *pAsXml == KOutputString1 );
    CleanupStack::PopAndDestroy(pAsXml);
    
    CDelegateFragment& delegate = pFragment->DelegateFragment();
    HBufC8* pDelegateAsXml = delegate.AsXmlL();
    CleanupStack::PushL(pDelegateAsXml);
    LOCAL_ASSERT( *pDelegateAsXml ==
                 _L8("<DelegateFragment xmlns=\"nsuri\">DelegateContent</DelegateFragment>") );
    CleanupStack::PopAndDestroy(pDelegateAsXml);
    
    CleanupStack::PopAndDestroy(pFragment);

	pParser->EnableFeature(EReportNamespaceMapping);
	
    CMainFragment* pFragment2 = CMainFragment::NewL(_L8("nasuria"),_L8("Test"),_L8("a"));
    CleanupStack::PushL(pFragment2);

	pParser->ParseL(KInputString, *pFragment2);
	
	HBufC8* pAsXml2 = pFragment2->AsXmlL();
	CleanupStack::PushL(pAsXml2);
    LOCAL_ASSERT( *pAsXml2 == KOutputString2 );
    CleanupStack::PopAndDestroy(pAsXml2);
    
    CDelegateFragment& delegate2 = pFragment2->DelegateFragment();
    HBufC8* pDelegateAsXml2 = delegate2.AsXmlL();
    CleanupStack::PushL(pDelegateAsXml2);
    LOCAL_ASSERT( *pDelegateAsXml2 ==
                 _L8("<DelegateFragment xmlns=\"nsuri\">DelegateContent</DelegateFragment>") );
    CleanupStack::PopAndDestroy(pDelegateAsXml2);
    
    CleanupStack::PopAndDestroy(pFragment2);

    CleanupStack::PopAndDestroy(pParser);
    return KErrNone;
    }
    
TInt CSenFragmentTester::MT_CSenFragmentBase_Parsing3L(  )
    {
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

    LOCAL_ASSERT( *pAsXml == KOutputString );

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);
    CleanupStack::PopAndDestroy(pParser);
    return KErrNone;
    }    

TInt CSenFragmentTester::MT_CSenFragmentBase_Parsing4L(  )
    {
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

    LOCAL_ASSERT( *pAsXml == KOutputString );

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);

    pFragment = CSenFragmentBase::NewL(_L8("nsuri"),_L8("Nested"));
    CleanupStack::PushL(pFragment);
	pParser->EnableFeature(EReportNamespaceMapping);
	
	pParser->ParseL(KInputString2, *pFragment);
	
	pAsXml = pFragment->AsXmlL();
	CleanupStack::PushL(pAsXml);

    LOCAL_ASSERT( *pAsXml == KInputString2 );

    CleanupStack::PopAndDestroy(pAsXml);
    CleanupStack::PopAndDestroy(pFragment);

    CleanupStack::PopAndDestroy(pParser);
    return KErrNone;
    }    

TInt CSenFragmentTester::MT_CSenFragmentBase_Parsing5L(  )
    {
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
    LOCAL_ASSERT( *pAsXml == KOutputString1 );
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
    LOCAL_ASSERT( *pAsXml == KOutputString2 );
    CleanupStack::PopAndDestroy(pAsXml);
    
    CleanupStack::PopAndDestroy(pBase);
    CleanupStack::PopAndDestroy(parser);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenParser_NewLL(  )
    {
    CSenParser* pParser = CSenParser::NewL();
    delete pParser;
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenParser_NewLCL(  )
    {
    CSenParser* pParser = CSenParser::NewLC();
    CleanupStack::PopAndDestroy(pParser);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenParser_NewL_1L(  )
    {
    _LIT8(KXmlParserMimeType,   "text/xml");
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    CParser* pParser = CParser::NewL(KXmlParserMimeType, *pFragment);

    // Ownership of the pParser is transfered to pSenParser
    CSenParser* pSenParser = CSenParser::NewL(pParser);
    delete pSenParser;

    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenParser_NewLC_1L(  )
    {
    _LIT8(KXmlParserMimeType,   "text/xml");
    CSenFragmentBase* pFragment = CSenFragmentBase::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    CParser* pParser = CParser::NewL(KXmlParserMimeType, *pFragment);

    // Ownership of the pParser is transfered to pSenParser
    CSenParser* pSenParser = CSenParser::NewLC(pParser);
    CleanupStack::PopAndDestroy(pSenParser);

    CleanupStack::PopAndDestroy(pFragment);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenParser_NewL_2L(  )
    {
    _LIT8(KXmlParserMimeType,   "text/xml");
    CSenParser* pParser = CSenParser::NewL(KXmlParserMimeType);
    delete pParser;
    return KErrNone;
    }

TInt CSenFragmentTester::MT_CSenParser_NewLC_2L(  )
    {
    _LIT8(KXmlParserMimeType,   "text/xml");
    CSenParser* pParser = CSenParser::NewLC(KXmlParserMimeType);
    CleanupStack::PopAndDestroy(pParser);
    return KErrNone;
    }

TInt CSenFragmentTester::MT_RSenDocument_NewLL(  )
    {
    RSenDocument document = RSenDocument::NewL();
    document.Close();
    LOCAL_ASSERT( document.IsNull() );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_RSenDocument_NewLCL(  )
    {
    RSenDocument document = RSenDocument::NewLC();
    CleanupStack::PopAndDestroy();
    
    // Can't use IsNull() function because iInternal pointer
    // (of document) is not NULLed. Only data for which iInternal
    // points to is deleted.
    //LOCAL_ASSERT( document.IsNull() );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_RSenDocument_NewL_1L(  )
    {
    // Can't test RSenDocument::NewL(void* aInternal)
    // because there is no way to get proper aInternal
    // using public API.
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_RSenDocument_NewLC_1L(  )
    {
    // Can't test RSenDocument::NewLC(void* aInternal)
    // because there is no way to get proper aInternal
    // using public API.
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_RSenDocument_CopyL(  )
    {
    RSenDocument document1 = RSenDocument::NewL();
    TXmlEngElement element = document1.CreateDocumentElementL(_L8("Test"));
    
    element.AddTextL(_L8("testcontent"));
    
    RSenDocument document2 = document1.Copy();
    
    // Check that main elements of document1 and document2 
    // are pointing to the same "in memory node" after copying.
    LOCAL_ASSERT( element.IsSameNode(document2.DocumentElement()) );
    
    // Test that document2 contains the same data as document1
    // did contain before closing document1.
    document1.Close();
    RBuf8 asXml;
    TXmlEngSerializationOptions options;
    options.iOptions = options.iOptions |
                       TXmlEngSerializationOptions::KOptionOmitXMLDeclaration;
    document2.SaveL(asXml, element, options);
    CleanupClosePushL(asXml);
    LOCAL_ASSERT( asXml == _L8("<Test>testcontent</Test>") );
    CleanupStack::PopAndDestroy(&asXml);
    
    document2.Close();
    LOCAL_ASSERT( document1.IsNull() && document2.IsNull() );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_RSenDocument_CloseL(  )
    {
    RSenDocument document = RSenDocument::NewL();
    document.Close();
    LOCAL_ASSERT( document.IsNull() );
    return KErrNone;
    }

TInt CSenFragmentTester::MT_RSenDocument_DestroyL(  )
    {
    RSenDocument document = RSenDocument::NewL();
    document.Destroy();
    LOCAL_ASSERT( document.IsNull() );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSenFragmentTester::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CSenFragmentTester::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
