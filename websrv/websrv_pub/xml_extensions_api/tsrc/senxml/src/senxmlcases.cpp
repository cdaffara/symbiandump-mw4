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









// INCLUDE FILES
#include <StifTestModule.h>
#include <e32math.h>
#include <libc/assert.h>
#include <e32panic.h>
#include "SenXmlBCTest.h"
#include "TestXml.h"

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
const TCaseInfo CSenXmlBCTest::Case ( 
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
        //  TEST TABLE
		ENTRY("NewL - CSenBaseAttribute ", CSenXmlBCTest::CSenBaseAttribute_NewLL),
		ENTRY("NewL - CSenBaseAttribute ", CSenXmlBCTest::CSenBaseAttribute_NewL_1L),
		ENTRY("NewL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_NewLL),
		ENTRY("NewL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_NewL_1L),
		ENTRY("NewL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_NewL_2L),
		ENTRY("NewL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_NewL_3L),
		ENTRY("NewL - CSenBaseElement", CSenXmlBCTest::CSenBaseElement_NewL_4L),
		ENTRY("SetAttributesL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetAttributesLL),
		ENTRY("LocalName - CSenBaseElement ",CSenXmlBCTest::CSenBaseElement_LocalNameL),
		ENTRY("NamespaceURI - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_NamespaceURIL),
		ENTRY("NsPrefix - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_NsPrefixL),
		ENTRY("SetPrefixL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetPrefixLL),
		ENTRY("HasContentTrue - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_HasContentTL),
		ENTRY("HasContentFalse - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_HasContentFL),

		ENTRY("Content - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_ContentL),
    
		ENTRY("ContentNull - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_Content_NullL),

		ENTRY("ContentUnicodeL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_ContentUnicodeLL),

		ENTRY("SetContentL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetContentLL),

		ENTRY("SetContentNullL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetContentL_NullL),
    
		ENTRY("SetContentoverrightL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetContentL_OverRightL),
    
		ENTRY("SetContentEscCharL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetContentL_WithEscCharsL),

		ENTRY("ConsistsOfL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_ConsistsOfLL),

		ENTRY("SetNamespaceL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetNamespaceLL),
    
		ENTRY("SetNamespaceNullL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetNamespaceL_NullL),

		ENTRY("SetNamespaceL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetNamespaceL_1L),

		ENTRY("SetNamespace2SameL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetNamespaceL_2same1L),
    
		ENTRY("SetNamespace2DiffL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetNamespaceL_2Diff1L),
    
		ENTRY("SetNamespaceDPSUL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetNamespaceL_DPSU1L),       
    
		ENTRY("SetNamespaceSameNsforPC1L - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetNamespaceL_SNsPC1L),           

		ENTRY("AddNamespace_FalseL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddNamespaceL_FL),
    
		ENTRY("AddNamespace_TrueL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddNamespaceL_TL),    
    
		ENTRY("AddNamespace_TrueSameParentNSL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddNamespaceL_TSPL),      
    
		ENTRY("AddNamespace_TrueDiffParentNSL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddNamespaceL_TDPL),          

		ENTRY("AddNamespaceL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddNamespaceL_1L),

		ENTRY("AddNamespace2SameL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddNamespaceL_2same1L),
    
		ENTRY("AddNamespace2DiffL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddNamespaceL_2Diff1L),
    
		ENTRY("Namespace - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_NamespaceL),
    

		ENTRY("Namespace - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_Namespace_1L),
    
		ENTRY("Namespace_with parent1 - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_Namespace_WP1L),

		ENTRY("Namespace - test2 - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_Namespace_2L),
    
		ENTRY("Namespace - test3 - CSenBaseElement", CSenXmlBCTest::CSenBaseElement_Namespace_3L),

		ENTRY("ElementsL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_ElementsLL),

		ENTRY("AttributesL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AttributesLL),

		ENTRY("NamespacesL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_NamespacesLL),

		ENTRY("AttrValue - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AttrValueL),

		ENTRY("AddAttrL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddAttrLL),

		ENTRY("AddAttributeL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddAttributeLL),		
		ENTRY("Parent - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_ParentL),

		ENTRY("SetParent - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetParentL),

		ENTRY("Root - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_RootL),

		ENTRY("Element - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_ElementL),
		ENTRY("Elements1 - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_ElementsL1),
		ENTRY("Elements2 - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_ElementsL2),

		ENTRY("CreateElementL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_CreateElementLL),

		ENTRY("InsertElementL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_InsertElementLL),
    
		ENTRY("InsertElement_No_BeforeElementL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_InsertElementL_NoBeforeL),

		ENTRY("AddElementL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddElementLL),

		ENTRY("AddElementL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddElementL_1L),

		ENTRY("AddElementL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddElementL_2L),

		ENTRY("AddElementL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddElementL_3L),

		ENTRY("RemoveElement - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_RemoveElementL),

		ENTRY("RemoveElement - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_RemoveElement_1L),

		ENTRY("RemoveElement - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_RemoveElement_2L),

		ENTRY("ReplaceElementL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_ReplaceElementLL),

		ENTRY("AsXmlL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AsXmlLL),

		ENTRY("AsXmlUnicodeL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AsXmlUnicodeLL),

		ENTRY("CopyFromL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_CopyFromLL),

		ENTRY("DetachL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_DetachLL),

		ENTRY("Child - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_ChildL),

		ENTRY("Set - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_SetL),
    
		ENTRY("AddAttributesL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AddAttributesLL),

		ENTRY("AsElement - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_AsElementL),

		ENTRY("ContentWriteStreamL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_ContentWriteStreamLL),
    
		ENTRY("RemoveAttributeL - CSenBaseElement ", CSenXmlBCTest::CSenBaseElement_RemoveAttributeLL),

		ENTRY("NewL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_NewLL),

		ENTRY("NewL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_NewL_1L),

		ENTRY("NewL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_NewL_2L),

		ENTRY("SetAttributesL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_SetAttributesLL),

		ENTRY("LocalName - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_LocalNameL),

		ENTRY("NamespaceURI - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_NamespaceURIL),

		ENTRY("NsPrefix - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_NsPrefixL),

		ENTRY("SetPrefixL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_SetPrefixLL),

		ENTRY("HasContent - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_HasContentL),

		ENTRY("Content - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_ContentL),
		
		ENTRY("ContentWriteStreamL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_ContentWriteStreamLL),
		

		ENTRY("ContentUnicodeL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_ContentUnicodeLL),

		ENTRY("SetContentL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_SetContentLL),

		ENTRY("ConsistsOfL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_ConsistsOfLL),
    
		ENTRY("ConsistsOfL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_ConsistsOfL1L),
    
		ENTRY("ConsistsOfL - Extratest CSenXmlElement", CSenXmlBCTest::CSenXmlElement_ConsistsOfL1L),

		ENTRY("SetNamespaceL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_SetNamespaceLL),

		ENTRY("SetNamespaceL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_SetNamespaceL_1L),
    
		ENTRY("SetNamespaceL - Sametest CSenXmlElement", CSenXmlBCTest::CSenXmlElement_SetNamespaceL_2same1L),

		ENTRY("SetNamespaceL - Difftest CSenXmlElement", CSenXmlBCTest::CSenXmlElement_SetNamespaceL_2Diff1L),

		ENTRY("AddNamespaceL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AddNamespaceLL),

		ENTRY("AddNamespaceL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AddNamespaceL_1L),

		ENTRY("Namespace - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_NamespaceL),

		ENTRY("Namespace - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_Namespace_1L),

		ENTRY("Namespace - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_Namespace_2L),

		ENTRY("Namespace - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_Namespace_3L),

		ENTRY("ElementsL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_ElementsLL),

		ENTRY("AttributesL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AttributesLL),
		ENTRY("AttributeLL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AddAttributeLL),

		ENTRY("NamespacesL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_NamespacesLL),

		ENTRY("AttrValue - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AttrValueL),

		ENTRY("AddAttrL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AddAttrLL),

		ENTRY("Parent - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_ParentL),

		ENTRY("SetParent - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_SetParentL),

		ENTRY("Root - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_RootL),

		ENTRY("Element - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_ElementL),

		ENTRY("CreateElementL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_CreateElementLL),

		ENTRY("InsertElementL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_InsertElementLL),

		ENTRY("AddElementL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AddElementLL),

		ENTRY("AddElementL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AddElementL_1L),

		ENTRY("AddElementL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AddElementL_2L),

		ENTRY("AddElementL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AddElementL_3L),

		ENTRY("RemoveElement - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_RemoveElementL),

		ENTRY("RemoveElement - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_RemoveElement_1L),

		ENTRY("RemoveElement - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_RemoveElement_2L),

		ENTRY("ReplaceElementL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_ReplaceElementLL),

		ENTRY("AsXmlL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AsXmlLL),

		ENTRY("AsXmlUnicodeL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AsXmlUnicodeLL),

		ENTRY("CopyFromL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_CopyFromLL),

		ENTRY("DetachL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_DetachLL),

		ENTRY("Child - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_ChildL),

		ENTRY("Set - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_SetL),

		ENTRY("AddAttributesL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AddAttributesLL),
/*    
		ENTRY("RemoveAttributeL - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_RemoveAttributeLL),
*/    
		ENTRY("AsElement - CSenXmlElement ", CSenXmlBCTest::CSenXmlElement_AsElementL),
    
		ENTRY("NewL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_NewL_L),
    
		ENTRY("NewL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_NewL_1L),
    
		ENTRY("NewL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_NewL_2L),
    
		ENTRY("NewL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_NewL_3L),
    
		ENTRY("NewL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_NewL_4L),
    
		ENTRY("NewL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_NewL_5L),

		ENTRY("AsXmlUnicodeL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_AsXmlUnicodeLL),

		ENTRY("WriteAsXMLToL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_WriteAsXMLToLL),
    
    	ENTRY("Content - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_ContentL),
    	ENTRY("Namespace - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_NamespaceL),
    	ENTRY("EnsureNamespace - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_EnsureNamespaceL),
    	ENTRY("DetachL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_DetachLL),
		ENTRY("SaveNamespacesLL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_SaveNamespacesLL),
		ENTRY("ResetContentLL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_ResetContentLL),
		ENTRY("LocalNameL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_LocalNameL),
		ENTRY("NsUriL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_NsUriL),
		ENTRY("NsPrefixL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_NsPrefixL),
		ENTRY("ExtractElementL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_ExtractElementL),
		ENTRY("SetAttributesL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_SetAttributesLL),
		ENTRY("ConsistsOfL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_ConsistsOfLL),
		ENTRY("ParseL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_ParseLL),
		ENTRY("ErrorParseL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_ParseL1L),
		ENTRY("SetContentOfL- CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_SetContentOfLL),
		ENTRY("ContentOf- CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_ContentOfL),
		ENTRY("ParserFeature - CSenXMLReader", CSenXmlBCTest::CSenXMLReader_ParserFeatureL),
		ENTRY("ParseL - CSenXMLReader", CSenXmlBCTest::CSenXMLReader_ParseLL),
		ENTRY("NewL - CSenDomFragment ", CSenXmlBCTest::CSenDomFragment_NewL_1L),
		ENTRY("NewL - CSenDomFragment ", CSenXmlBCTest::CSenDomFragment_NewL_L),

		ENTRY("NewL - CSenDomFragment ", CSenXmlBCTest::CSenDomFragment_NewL_2L),

		ENTRY("NewL - CSenDomFragment ", CSenXmlBCTest::CSenDomFragment_NewL_3L),

		ENTRY("NewL - CSenDomFragment ", CSenXmlBCTest::CSenDomFragment_NewL_4L),

		ENTRY("NewL - CSenDomFragment ", CSenXmlBCTest::CSenDomFragment_NewL_5L),

		ENTRY("NewL - CSenDomFragment ", CSenXmlBCTest::CSenDomFragment_NewL_6L),

		//ENTRY("ExpandL - CSenDomFragment ", CSenXmlBCTest::CSenDomFragment_ExpandLL),
		
		//ENTRY("ResumeParsingFromL - CSenDomFragment ", CSenXmlBCTest::CSenDomFragment_ResumeParsingFromLL),
		
		ENTRY("ParseWithL - CSenDomFragment ", CSenXmlBCTest::CSenDomFragment_ParseWithLL),


		ENTRY("SetAttributesL - CSenDomFragment ", CSenXmlBCTest::CSenDomFragment_SetAttributesLL),

		ENTRY("ParseL1 - CSenDomFragment ", CSenXmlBCTest::CSenDomFragment_ParseL1L),

		ENTRY("NewL - CSenNamespace ", CSenXmlBCTest::CSenNamespace_NewL_L),    
    
		ENTRY("NewLC - CSenNamespace ", CSenXmlBCTest::CSenNamespace_NewLC_L),

		ENTRY("NewL - CSenNamespace ", CSenXmlBCTest::CSenNamespace_NewL_1L),

		ENTRY("NewLC - CSenNamespace ", CSenXmlBCTest::CSenNamespace_NewLC_1L),
    
		ENTRY("MatchesL - CSenNamespace ", CSenXmlBCTest::CSenNamespace_MatchesL_L),
		
		ENTRY("InsertL - CSenNamespace ", CSenXmlBCTest::CSenNamespace_InsertLL),
		
    
		ENTRY("NewL - CSenXmlReader ", CSenXmlBCTest::CSenXmlReader_NewL_L),
  
		ENTRY("NewLC - CSenXmlReader ", CSenXmlBCTest::CSenXmlReader_NewLC_L),
    
		ENTRY("NewL - CSenXmlReader ", CSenXmlBCTest::CSenXmlReader_NewL_1L),
    
		ENTRY("NewLC - CSenXmlReader ", CSenXmlBCTest::CSenXmlReader_NewLC_1L),
    
		ENTRY("NewL - CSenXmlReader ", CSenXmlBCTest::CSenXmlReader_NewL_2L),
    
		ENTRY("NewLC - CSenXmlReader ", CSenXmlBCTest::CSenXmlReader_NewLC_2L),
    
		ENTRY("NewL - CSenXmlReader ", CSenXmlBCTest::CSenXmlReader_NewL_3L),
    
		ENTRY("NewLC - CSenXmlReader ", CSenXmlBCTest::CSenXmlReader_NewLC_3L),

		ENTRY("EnabledParserFeature - CSenXmlReader ", CSenXmlBCTest::CSenXmlReader_EnabledParserFeature_L),
		
		ENTRY("ToUtf8LC - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_ToUtf8LCL),
		
		ENTRY("StartsWith - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_StartsWithL),

		ENTRY("EndsWith - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_EndsWithL),

		ENTRY("AttrValue - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_AttrValueL),

		ENTRY("AllocAttrValueL - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_AllocAttrValueLL),

		ENTRY("DecodeHttpCharactersL - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_DecodeHttpCharactersLL),

		ENTRY("DecodeHttpCharactersLC - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_DecodeHttpCharactersLCL),

		ENTRY("NsPrefix - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_NsPrefixL),

		ENTRY("LocalName - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_LocalNameL),

		ENTRY("RemoveAttributeL - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_RemoveAttributeLL),

		ENTRY("RemoveAttributeL1 - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_RemoveAttributeL1L),

		ENTRY("AddAttributeL - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_AddAttributeLL),

		ENTRY("AddAttributeL1 - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_AddAttributeL1L),

		ENTRY("AddAttributeL1 - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_AddAttributeL2L),
		
		ENTRY("EncodeHttpCharactersLC - CSenXmlUtils ", CSenXmlBCTest::CSenXmlUtils_EncodeHttpCharactersLCL),
		ENTRY("BaseConstructL - CSenDomFragment ", CSenXmlBCTest::CSenDomFragment_BaseConstructL),
		ENTRY("BaseConstructL - CSenBaseFragment ", CSenXmlBCTest::CSenBaseFragment_BaseConstructL),
		
    
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
        null.iMethod = 0 ;
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

void CSenXmlBCTest::SetupL(  ){
    if ( iXmlReader ){
        delete iXmlReader;
        iXmlReader = NULL;
    }
    iXmlReader = CSenXmlReader::NewL(
    );
} 

void CSenXmlBCTest::Teardown(  ){
    if ( iXmlReader){
        delete iXmlReader;
        iXmlReader = NULL;
    }
}

TInt CSenXmlBCTest::CSenBaseAttribute_NewLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseAttribute* pAttr=CSenBaseAttribute::NewL(_L8("Attr1"),_L8("value"));
    CleanupStack::PushL(pAttr);
    if(pAttr)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pAttr);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseAttribute_NewL_1L (TTestResult& aResult)
    {
    SetupL();    
    TBool Flag;
    CSenBaseAttribute* pAttr=CSenBaseAttribute::NewL(_L8("N:Attr1"),_L8("Attr1"),_L8("value"));
    CleanupStack::PushL(pAttr);
    if(pAttr)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pAttr);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_NewLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_NewL_1L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuri"),_L8("Nokia"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
	Teardown();
    return KErrNone;    
    }

TInt CSenXmlBCTest::CSenBaseElement_NewL_2L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuri"),_L8("Nokia"),_L8("n:Nokia"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseElement_NewL_3L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    RAttributeArray aAttrs;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuri"),_L8("Nokia"),_L8("n:Nokia"), aAttrs);
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseElement_NewL_4L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    RAttributeArray aAttrs;
    CSenBaseElement* ele = CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(ele);
    CSenElement* parent = ele->Parent();
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuri"),_L8("Nokia"),_L8("n:Nokia"), aAttrs, *parent);
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    CleanupStack::PopAndDestroy(ele);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_SetAttributesLL (TTestResult& aResult)
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
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
      
    CSenBaseElement *pElement = CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetAttributesL(attrArray);
    HBufC8* pElementBuf = pElement->AsXmlL();
    /*EUNIT_ASSERT( *pElementBuf == _L8("<Nokia pr:LocalName=\"Value\"/>" ));*/
    if(!( *pElementBuf == _L8("<Nokia pr:LocalName=\"Value\"/>" )))
    return KErrArgument;   

	CleanupStack::PopAndDestroy(pElement);
	CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
	delete pElementBuf;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_LocalNameL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    _LIT8(KLName,"Nokia");
 
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    const TDesC8& pLName=pElement->LocalName(); 
    if(pLName==_L8("Nokia"))
	 	Flag=1;
	else
		Flag=0;
/*    EUNIT_ASSERT(Flag);*/
    if(!Flag)
    return KErrArgument;
    
	CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_NamespaceURIL (TTestResult& aResult)
    {
    SetupL();
    _LIT8(KNSUri,"nsuria");
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuria"),_L8("Nokia"));
    CleanupStack::PushL(pElement);
    const TDesC8 &pNSUri=pElement->NamespaceURI();
    if(pNSUri==KNSUri)
    	Flag=1;
    else
    	Flag=0;
    CleanupStack::PopAndDestroy(pElement);
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_NsPrefixL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    _LIT8(KNSPrefix,"N");
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuria"),_L8("Nokia"),_L8("N:Nokia"));
    const TDesC8& pNSPrefix=pElement->NsPrefix();
    if(pNSPrefix==KNSPrefix)
    	Flag=1;
    else
    	Flag=0;
    delete pElement;    
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_SetPrefixLL (TTestResult& aResult)
    {
    SetupL();
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuria"),_L8("Nokia"));
    CleanupStack::PushL(pElement);
	pElement->SetPrefixL(_L8("N"));
	const TDesC8& pNSPrefix=pElement->NsPrefix();
    /*EUNIT_ASSERT_EQUALS(pNSPrefix, _L8("N"));*/
    TL(pNSPrefix == _L8("N"));
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_HasContentTL (TTestResult& aResult)
    {
    SetupL();	
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuria"),_L8("Nokia"),_L8("N:Nokia"));
	pElement->SetContentL(_L8("Nokia content"));
	TBool Flag=pElement->HasContent();
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    delete pElement;
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseElement_HasContentFL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuria"),_L8("Nokia"),_L8("N:Nokia"));
	TBool pHasContnt=pElement->HasContent();
	if(pHasContnt)
		Flag=0;
	else
		Flag=1;
	    /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

	delete pElement;
    Teardown();
    return KErrNone;
    }    

TInt CSenXmlBCTest::CSenBaseElement_ContentL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuria"),_L8("Nokia"),_L8("N:Nokia"));
	pElement->SetContentL(_L8("Nokia content"));
	TPtrC8 pContent=pElement->Content();
	if(pContent==_L8("Nokia content"))
		Flag=1;
	else
		Flag=0;
	
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    delete pElement;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_Content_NullL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuria"),_L8("Nokia"),_L8("N:Nokia"));
	TPtrC8 pContent=pElement->Content();
	if(pContent==KNullDesC8())
		Flag=1;
	else
		Flag=0;
	
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

   	delete pElement;
    Teardown();
    return KErrNone;
    }    

TInt CSenXmlBCTest::CSenBaseElement_ContentUnicodeLL (TTestResult& aResult)
    {    
    SetupL();
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuria"),_L8("Nokia"),_L8("N:Nokia"));
    CleanupStack::PushL(pElement);
	pElement->SetContentL(_L8("Nokia content"));
	HBufC* ElementBuf=pElement->ContentUnicodeL();
/*	EUNIT_ASSERT_EQUALS(*ElementBuf,_L("Nokia content"));*/
	TL(*ElementBuf == _L("Nokia content"));
	CleanupStack::PopAndDestroy(pElement);
	delete ElementBuf;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_SetContentLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuria"),_L8("Nokia"),_L8("N:Nokia"));
    CleanupStack::PushL(pElement);
	TPtrC8 pContent=pElement->SetContentL(_L8("Nokia Content"));
	if(pContent==_L8("Nokia Content"))
		Flag=1;
	else
		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);   
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseElement_SetContentL_NullL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuria"),_L8("Nokia"),_L8("N:Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetContentL(_L8("element content"));
	TPtrC8 pContent=pElement->SetContentL(KNullDesC8);
	if(pContent==KNullDesC8())
		Flag=1;
	else
		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);   
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseElement_SetContentL_OverRightL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuria"),_L8("Nokia"),_L8("N:Nokia"));
    CleanupStack::PushL(pElement);
	pElement->SetContentL(_L8("Nokia Content1"));
	pElement->SetContentL(_L8("Nokia Content2"));	
	TPtrC8 pContent=pElement->Content();
	if(pContent==_L8("Nokia Content2"))
		Flag=1;
	else
		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);   
    Teardown();
    return KErrNone;
    }
      
TInt CSenXmlBCTest::CSenBaseElement_SetContentL_WithEscCharsL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nsuria"),_L8("Nokia"),_L8("N:Nokia"));
    CleanupStack::PushL(pElement);
	TPtrC8 pContent=pElement->SetContentL(_L8("N&o'k\"ia <Content>"));
	if(pContent==_L8("N&o'k\"ia <Content>"))
		Flag=1;
	else
		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);   
    Teardown();
    return KErrNone;
    }      
       
TInt CSenXmlBCTest::CSenBaseElement_ConsistsOfLL (TTestResult& aResult)
    {
    SetupL();
    CSenBaseElement* pElementA=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElementA);
    CSenBaseElement* pChild1A=CSenBaseElement::NewL(_L8("Asp"));
    CleanupStack::PushL(pChild1A);
    CSenBaseElement* pChild2A=CSenBaseElement::NewL(_L8("WebServices"));
    CleanupStack::PushL(pChild2A);
    pElementA->SetContentL(_L8("Nokia organisation structure"));
    pElementA->AddElementL(*pChild1A);
    pElementA->AddElementL(*pChild2A);
    
    CSenBaseElement* pElementB=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElementB);
    CSenBaseElement* pChild1B=CSenBaseElement::NewL(_L8("Asp"));
    CleanupStack::PushL(pChild1A);
    CSenBaseElement* pChild2B=CSenBaseElement::NewL(_L8("WebServices"));
    CleanupStack::PushL(pChild2B);
    pElementB->SetContentL(_L8("Nokia organisation structure"));
    pElementB->AddElementL(*pChild1B);
    pElementB->AddElementL(*pChild2B);
    
    TBool ConsistOfFlag=pElementB->ConsistsOfL(*pElementA);
    /*EUNIT_ASSERT( ConsistOfFlag );*/
    if(!ConsistOfFlag)
    return KErrArgument;   
    
    CleanupStack::Pop(2);
    CleanupStack::PopAndDestroy(pElementB);
    CleanupStack::Pop(2);
    CleanupStack::PopAndDestroy(pElementA);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_SetNamespaceLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("namespaceuri"));
	const TDesC8& pNsURI=pElement->NamespaceURI();
	if(pNsURI==_L8("namespaceuri"))
		Flag=1;
	else
		Flag=0;
	CleanupStack::PopAndDestroy(pElement);
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_SetNamespaceL_NullL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8(""));
	const TDesC8& pNsURI=pElement->NamespaceURI();
	if(pNsURI==_L8(""))
		Flag=1;
	else
		Flag=0;
	CleanupStack::PopAndDestroy(pElement)	;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_SetNamespaceL_1L (TTestResult& aResult)
    {
    SetupL();
     TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("N"),_L8("namespaceuri"));
	const TDesC8& pNsURI=pElement->NamespaceURI();
	const TDesC8& pNsPrefix=pElement->NsPrefix();
	if(pNsURI==_L8("namespaceuri")&&pNsPrefix==_L8("N"))
		Flag=1;
	else
		Flag=0;
	CleanupStack::PopAndDestroy(pElement);
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseElement_SetNamespaceL_2same1L (TTestResult& aResult)
    {
    SetupL();
     TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("N"),_L8("namespaceuri"));
    pElement->SetNamespaceL(_L8("N"),_L8("namespaceuri"));
	const TDesC8& pNsURI=pElement->NamespaceURI();
	const TDesC8& pNsPrefix=pElement->NsPrefix();
	if(pNsURI==_L8("namespaceuri")&&pNsPrefix==_L8("N"))
		Flag=1;
	else
		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

	RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
 	/*EUNIT_ASSERT( pNsList.Count()==1 );*/
    if(!( pNsList.Count()==1 ))
    return KErrArgument;   
 	
	CleanupStack::PopAndDestroy(pElement); 
    Teardown();
    return KErrNone;
    }    
 
TInt CSenXmlBCTest::CSenBaseElement_SetNamespaceL_2Diff1L (TTestResult& aResult)
    {
    SetupL();
     TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("N1"),_L8("namespaceuri1"));
    pElement->SetNamespaceL(_L8("N2"),_L8("namespaceuri2"));
	const TDesC8& pNsURI=pElement->NamespaceURI();
	const TDesC8& pNsPrefix=pElement->NsPrefix();
	if(pNsURI==_L8("namespaceuri2")&&pNsPrefix==_L8("N2"))
		Flag=1;
	else
		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

	RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
 	/*EUNIT_ASSERT( pNsList.Count()==2 );*/
    if(!( pNsList.Count()==2 ))
    return KErrArgument;   
 	
 	CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }    
 
TInt CSenXmlBCTest::CSenBaseElement_SetNamespaceL_DPSU1L (TTestResult& aResult)
    {
    SetupL();
     TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("N1"),_L8("namespaceuri1"));
    pElement->SetNamespaceL(_L8("N2"),_L8("namespaceuri1"));
	const TDesC8& pNsURI=pElement->NamespaceURI();
	const TDesC8& pNsPrefix=pElement->NsPrefix();
	if(pNsURI==_L8("namespaceuri1")&&pNsPrefix==_L8("N2"))
		Flag=1;
	else
		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

	RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
 	/*EUNIT_ASSERT( pNsList.Count()==2 );*/
    if(!( pNsList.Count()==2 ))
    return KErrArgument;   
 	
 	CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }       
    
TInt CSenXmlBCTest::CSenBaseElement_SetNamespaceL_SNsPC1L (TTestResult& aResult)
	{
	SetupL();
	 CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
   	 CleanupStack::PushL(pElement);
     CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("Parent"));
   	 CleanupStack::PushL(pParent);
   	 pElement->SetParent(pParent);
   	 pParent->SetNamespaceL(_L8("n"),_L8("nsuri"));
   	 pElement->SetNamespaceL(_L8("n"),_L8("nsuri"));
   	 RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
/*   	 EUNIT_ASSERT(pNsList.Count()==0);*/
    if(!(pNsList.Count()==0))
    return KErrArgument;   

   	 CleanupStack::PopAndDestroy(pParent);
   	 CleanupStack::PopAndDestroy(pElement);   	 
	Teardown();
    return KErrNone;
    }
        
TInt CSenXmlBCTest::CSenBaseElement_AddNamespaceL_FL (TTestResult& aResult)
    {
    SetupL();
     TBool Flag;
     TBool CheckParent=FALSE;
     CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
   	 CleanupStack::PushL(pElement);
   	 CSenNamespace* pNs1=CSenNamespace::NewL(_L8("N"),_L8("namespaceuri"));
     CleanupStack::PushL(pNs1);
     const CSenNamespace* pNs=pElement->AddNamespaceL(*pNs1,CheckParent);
	 const TDesC8& pNsURI=pNs->URI();
	 const TDesC8& pNsPrefix=pNs->Prefix();
	 if(pNsURI==_L8("namespaceuri")&&pNsPrefix==_L8("N"))
	 	Flag=1;
	 else
		Flag=0;
     CleanupStack::PopAndDestroy(pNs1);	
	 CleanupStack::PopAndDestroy(pElement);
         /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseElement_AddNamespaceL_TL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    TBool CheckParent=TRUE;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("ParentElement"));
    CleanupStack::PushL(pParent);
    
    pElement->SetParent(pParent);
    CSenNamespace* pNs1=CSenNamespace::NewL(_L8("N"),_L8("namespaceuri"));
    CleanupStack::PushL(pNs1);
    const CSenNamespace* pNs=pElement->AddNamespaceL(*pNs1,CheckParent);
    RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
	const TDesC8& pNsURI=pNs->URI();
	const TDesC8& pNsPrefix=pNs->Prefix();
	if(pNsURI==_L8("namespaceuri")&&pNsPrefix==_L8("N"))
		Flag=1;
	else
		Flag=0;
	
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

/*    EUNIT_ASSERT( pNsList.Count()==1);*/
    if(!(pNsList.Count()==1))
    return KErrArgument;   

   	CleanupStack::PopAndDestroy(pNs1);
   	CleanupStack::PopAndDestroy(pParent);	
	CleanupStack::PopAndDestroy(pElement);
	
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseElement_AddNamespaceL_TSPL (TTestResult& aResult)
    {
    SetupL();
     TBool Flag;
     TBool CheckParent=TRUE;
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("ParentElement"));
    CleanupStack::PushL(pParent);
    pParent->AddNamespaceL(_L8("N"),_L8("namespaceuri"));
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetParent(pParent);
    CSenNamespace* pNs1=CSenNamespace::NewL(_L8("N"),_L8("namespaceuri"));
    CleanupStack::PushL(pNs1);
    const CSenNamespace* pNs=pElement->AddNamespaceL(*pNs1,CheckParent);
    RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
	const TDesC8& pNsURI=pNs->URI();
	const TDesC8& pNsPrefix=pNs->Prefix();
	if(pNsURI==_L8("namespaceuri")&&pNsPrefix==_L8("N"))
		Flag=1;
	else
		Flag=0;
	
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

/*    EUNIT_ASSERT( pNsList.Count()==0);*/
    if(!(pNsList.Count()==0))
    return KErrArgument; 

   	CleanupStack::PopAndDestroy(pNs1);	
	CleanupStack::PopAndDestroy(pElement);
	CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }    

TInt CSenXmlBCTest::CSenBaseElement_AddNamespaceL_TDPL (TTestResult& aResult)
    {
    SetupL();
     TBool Flag;
     TBool CheckParent=TRUE;
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("ParentElement"));
    CleanupStack::PushL(pParent);
    pParent->AddNamespaceL(_L8("N1"),_L8("namespaceuri1"));
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetParent(pParent);
    CSenNamespace* pNs1=CSenNamespace::NewL(_L8("N"),_L8("namespaceuri"));
    CleanupStack::PushL(pNs1);
    const CSenNamespace* pNs=pElement->AddNamespaceL(*pNs1,CheckParent);
    RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
	const TDesC8& pNsURI=pNs->URI();
	const TDesC8& pNsPrefix=pNs->Prefix();
	if(pNsURI==_L8("namespaceuri")&&pNsPrefix==_L8("N"))
		Flag=1;
	else
		Flag=0;
	
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

/*    EUNIT_ASSERT( pNsList.Count()==1);*/
    if(!(pNsList.Count()==1))
    return KErrArgument; 

   	CleanupStack::PopAndDestroy(pNs1);	
	CleanupStack::PopAndDestroy(pElement);
	CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }    

TInt CSenXmlBCTest::CSenBaseElement_AddNamespaceL_1L (TTestResult& aResult)
    {
    SetupL();
     TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    const CSenNamespace* pNs=pElement->AddNamespaceL(_L8("N"),_L8("namespaceuri"));
	const TDesC8& pNsURI=pNs->URI();
	const TDesC8& pNsPrefix=pNs->Prefix();
	if(pNsURI==_L8("namespaceuri")&&pNsPrefix==_L8("N"))
		Flag=1;
	else
		Flag=0;
	CleanupStack::PopAndDestroy(pElement);
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseElement_AddNamespaceL_2same1L (TTestResult& aResult)
    {
    SetupL();
     TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    const CSenNamespace* pNs=pElement->AddNamespaceL(_L8("N"),_L8("namespaceuri"));
    pNs=pElement->AddNamespaceL(_L8("N"),_L8("namespaceuri"));
	const TDesC8& pNsURI=pNs->URI();
	const TDesC8& pNsPrefix=pNs->Prefix();
	if(pNsURI==_L8("namespaceuri")&&pNsPrefix==_L8("N"))
		Flag=1;
	else
		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

	RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
 	/*EUNIT_ASSERT( pNsList.Count()==1 );*/
    if(!(pNsList.Count()==1))
    return KErrArgument;   
 	
	CleanupStack::PopAndDestroy(pElement); 
    Teardown();
    return KErrNone;
    }    
 
TInt CSenXmlBCTest::CSenBaseElement_AddNamespaceL_2Diff1L (TTestResult& aResult)
    {
    SetupL();
     TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
	const CSenNamespace* pNs=pElement->AddNamespaceL(_L8("N1"),_L8("namespaceuri1"));
    pNs=pElement->AddNamespaceL(_L8("N2"),_L8("namespaceuri2"));
	const TDesC8& pNsURI=pNs->URI();
	const TDesC8& pNsPrefix=pNs->Prefix();
	if(pNsURI==_L8("namespaceuri2")&&pNsPrefix==_L8("N2"))
		Flag=1;
	else
		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

	RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
 	/*EUNIT_ASSERT( pNsList.Count()==2 );*/
 	if(!(pNsList.Count()==2))
    return KErrArgument;  
 	
 	CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }    
    

TInt CSenXmlBCTest::CSenBaseElement_NamespaceL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag,Flag1,Flag2;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    //NULL
    pElement->SetNamespaceL(_L8(""));
    const CSenNamespace* pNs2=pElement->Namespace();
    if(pNs2==NULL)
    	Flag2=1;
    else
    	Flag2=0;
    /*EUNIT_ASSERT(Flag2);*/
   	if(!Flag2)
    return KErrArgument;  

   
    //only URI
    pElement->SetNamespaceL(_L8("nsuri"));
    const CSenNamespace* pNs1=pElement->Namespace();
    if(pNs1->Prefix()==_L8("")&&pNs1->URI()==_L8("nsuri"))
    	Flag1=1;
    else
    	Flag1=0;
    /*EUNIT_ASSERT(Flag1);*/
  	if(!Flag1)
    return KErrArgument;  

    //URI and Prefix
    pElement->SetNamespaceL(_L8("n"),_L8("nsuri"));
    const CSenNamespace* pNs=pElement->Namespace();
    if(pNs->Prefix()==_L8("n") && pNs->URI()==_L8("nsuri"))
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_Namespace_1L (TTestResult& aResult)
    {
    SetupL();
     TBool Flag,Flag1;
     //True case
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("n"),_L8("nsuri"));
    const CSenNamespace* pNs=pElement->Namespace(_L8("n"));
    if(pNs->Prefix()==_L8("n") && pNs->URI()==_L8("nsuri"))
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    //False case
    pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("n"),_L8("nsuri"));
    pNs=pElement->Namespace(_L8("N"));
    if(pNs==NULL)
    	Flag1=1;
    else
    	Flag1=0;
    /*EUNIT_ASSERT(Flag1);*/
  	if(!Flag1)
    return KErrArgument;  
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseElement_Namespace_WP1L (TTestResult& aResult)
    {
    SetupL();
     TBool Flag,Flag1;
 	CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("Parent")) ;
 	CleanupStack::PushL(pParent);
 	pParent->SetNamespaceL(_L8("n"),_L8("nsuri"));
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetParent(pParent);
    const CSenNamespace* pNs=pElement->Namespace(_L8("n"));
    if(pNs->Prefix()==_L8("n") && pNs->URI()==_L8("nsuri"))
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    CleanupStack::PopAndDestroy(pParent);
    //False case
    pParent=CSenBaseElement::NewL(_L8("Parent")) ;
 	CleanupStack::PushL(pParent);
 	pParent->SetNamespaceL(_L8("n"),_L8("nsuri"));
    pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetParent(pParent);
	pNs=pElement->Namespace(_L8("N"));
	if(pNs==NULL)
    	Flag1=1;
    else
    	Flag1=0;
   /* EUNIT_ASSERT( Flag1 );*/
  	if(!Flag1)
    return KErrArgument;  
    
    CleanupStack::PopAndDestroy(pElement);
    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_Namespace_2L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag,Flag1;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParent);
    pParent->SetNamespaceL(_L8("p"),_L8("nsurip"));
    pElement->SetNamespaceL(_L8("n"),_L8("nsurin"));
    pElement->SetParent(pParent);
    //aCheckInParent=FALSE and parent prefix
    const CSenNamespace* pPNs=pElement->Namespace(_L8("p"),FALSE);
    if(pPNs==NULL)
    	Flag=1;
   	else
   		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    //aCheckInParent=TRUE and parent prefix
    pPNs=pElement->Namespace(_L8("p"),TRUE);
    if(pPNs->Prefix()==_L8("p") && pPNs->URI()==_L8("nsurip"))
    	Flag1=1; 
   	else
   		Flag1=0;
    /*EUNIT_ASSERT( Flag1 );*/
  	if(!Flag1)
    return KErrArgument;  
    
    CleanupStack::PopAndDestroy(pParent);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
   
TInt CSenXmlBCTest::CSenBaseElement_Namespace_3L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->AddNamespaceL(_L8("n"),_L8("nsurin"));
    pElement->AddNamespaceL(_L8("m"),_L8("nsurim"));
    pElement->AddNamespaceL(_L8("l"),_L8("nsuril"));
    RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
/*    EUNIT_ASSERT( pNsList.Count()==3 );*/
  	if(!( pNsList.Count()==3 ))
    return KErrArgument;  
    
    const CSenNamespace* pNs=pElement->Namespace(_L8("m"),_L8("nsurim"));
    if(pNs->Prefix()==_L8("m") && pNs->URI()==_L8("nsurim"))
    	Flag=1;
    else
    	Flag=0;
    /*EUNIT_ASSERT(Flag);*/
  	if(!Flag)
    return KErrArgument;  
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_ElementsLL (TTestResult& aResult)
    {
    SetupL();
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia")) ;
    CleanupStack::PushL(pElement);
    CSenBaseElement* pChild1=CSenBaseElement::NewL(_L8("element1")) ;
    CleanupStack::PushL(pChild1);
    CSenBaseElement* pChild2=CSenBaseElement::NewL(_L8("element2")) ;
    CleanupStack::PushL(pChild2);
    CSenBaseElement* pChild3=CSenBaseElement::NewL(_L8("element3")) ;
    CleanupStack::PushL(pChild3);
    pElement->AddElementL(*pChild1);
    pElement->AddElementL(*pChild2);
    pElement->AddElementL(*pChild3);
    RPointerArray<CSenElement>& ElList=pElement->ElementsL();
/*    EUNIT_ASSERT( ElList.Count()==3);*/
  	if(!(ElList.Count()==3))
    return KErrArgument;  

    CleanupStack::Pop(3);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_AttributesLL (TTestResult& aResult)
    {
    SetupL();
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia")) ;
    CleanupStack::PushL(pElement);
    pElement->AddAttrL(_L8("Attr1"),_L8("value1"));
    pElement->AddAttrL(_L8("Attr2"),_L8("value2"));
    pElement->AddAttrL(_L8("Attr3"),_L8("value3"));
    pElement->AddAttrL(_L8("Attr4"),_L8("value4"));    
    pElement->AddAttrL(_L8("Attr4"),_L8("value4"));    
    RPointerArray<CSenBaseAttribute>& pAttrList=pElement->AttributesL();
/*    EUNIT_ASSERT( pAttrList.Count()==4 );*/
  	if(!( pAttrList.Count()==4 ))
    return KErrArgument;  

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_NamespacesLL (TTestResult& aResult)
    {
    SetupL();
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->AddNamespaceL(_L8("l"),_L8("nsuril"));
    pElement->AddNamespaceL(_L8("m"),_L8("nsurim"));
    pElement->AddNamespaceL(_L8("n"),_L8("nsurin"));
    pElement->AddNamespaceL(_L8("m"),_L8("nsurim"));
    const RPointerArray<CSenNamespace>& pNs=pElement->NamespacesL();
/*    EUNIT_ASSERT( pNs.Count()==3);*/
  	if(!(pNs.Count()==3))
    return KErrArgument;  

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_AttrValueL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag,NegFlag;
    _LIT8(KValue,"value4");
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia")) ;
    pElement->AddAttrL(_L8("Attr1"),_L8("value1"));
    pElement->AddAttrL(_L8("Attr2"),_L8("value2"));
    pElement->AddAttrL(_L8("Attr3"),_L8("value3"));
    pElement->AddAttrL(_L8("Attr4"),_L8("value4"));    
    pElement->AddAttrL(_L8("Attr4"),_L8("value4"));    
    const TDesC8* pAttrVal=pElement->AttrValue(_L8("Attr4"));
    if(pAttrVal!=NULL)
    {
    	Flag=1;
    }
    else
    	Flag=0;
/*    EUNIT_ASSERT(Flag);*/
  	if(!Flag)
    return KErrArgument;  
    
    const TDesC8* pNegAttrVal=pElement->AttrValue(_L8("attr1"));
    if(pNegAttrVal==NULL)
    	NegFlag=1;
    else
    	NegFlag=0;
/*    EUNIT_ASSERT(NegFlag);*/
  	if(!NegFlag)
    return KErrArgument;  
    
    delete pElement;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_AddAttrLL (TTestResult& aResult)
    {
    SetupL();
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->AddAttrL(_L8("Attr1"),_L8("value1"));
    RPointerArray<CSenBaseAttribute>& pAttrList=pElement->AttributesL();
/*    EUNIT_ASSERT( pAttrList.Count()==1 );*/
  	if(!( pAttrList.Count()==1 ))
    return KErrArgument;  
    
    pElement->AddAttrL(_L8("Attr1"),_L8("Duplicatevalue1"));
/*    EUNIT_ASSERT( pAttrList.Count()==1 );*/
  	if(!( pAttrList.Count()==1 ))
    return KErrArgument;  

    const TDesC8* pAttrVal=pElement->AttrValue(_L8("Attr1"));
    pElement->AddAttrL(_L8("Attr2"),_L8(" value2"));//improve here by comparing TDesC8* AttrValue()
    /*EUNIT_ASSERT( pAttrList.Count()==2 );*/
  	if(!( pAttrList.Count()==2 ))
    return KErrArgument;  
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
TInt CSenXmlBCTest::CSenBaseElement_AddAttributeLL (TTestResult& aResult)
    {
    SetupL();
    CTestBaseElement* pElement=CTestBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->AddAttributeL(_L8("Attr1"),_L8("value1"));
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_ParentL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag,PFlag,PPFlag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("Parent"));
    CSenBaseElement* pPParent=CSenBaseElement::NewL(_L8("parent of parent"));
    pElement->SetParent(pParent);
    pParent->SetParent(pPParent);
    CSenElement* pEl1= pElement->Parent();
    CSenElement* pEl2= pParent->Parent();
    CSenElement* pEl3= pPParent->Parent();
    const TDesC8& pLName1=pEl1->LocalName();
    if(pLName1==_L8("Parent"))
    	Flag=1;
   	else
   		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    const TDesC8& pLName2=pEl2->LocalName();
    if(pLName2==_L8("parent of parent"))
    	PFlag=1;
    else
    	PFlag=0;
/*    EUNIT_ASSERT(PFlag);*/
  	if(!PFlag)
    return KErrArgument;  
    
    if(pEl3==NULL)
    	PPFlag=1;
    else
    	PPFlag=0;
    /*EUNIT_ASSERT(PPFlag);*/
  	if(!PFlag)
    return KErrArgument;  
    
    delete pElement;
    delete pParent;
    delete pPParent;
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseElement_SetParentL (TTestResult& aResult)
    {
    SetupL();
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParent);
    pElement->SetNamespaceL(_L8("Nsuria"));
    pParent->SetNamespaceL(_L8("nsurip"));
    pElement->SetParent(pParent);
    CSenElement* pPar=pElement->Parent();
    const TDesC8& ParName=pPar->LocalName();
    /*EUNIT_ASSERT_EQUALS( ParName,_L8("Parent") );*/
    TL(ParName == _L8("Parent"));
    CleanupStack::PopAndDestroy(pParent);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_RootL (TTestResult& aResult)
    {
    SetupL();    
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
    const MSenElement& pElRoot=pElement->Root();
    const TDesC8& pElLName=pElRoot.LocalName();
    if(pElLName==_L8("Nokia"))
    	Flag=1;
    else
    	Flag=0;
    
    CSenBaseElement* pParent1=CSenBaseElement::NewL(_L8("Parent1"));
    pElement->SetParent(pParent1);
    const MSenElement& pElRoot1=pElement->Root();
    const TDesC8& pElLName1=pElRoot1.LocalName();
    if(pElLName1==_L8("Parent1"))
    	Flag=1;
    else
    	Flag=0;
    
    CSenBaseElement* pParent2=CSenBaseElement::NewL(_L8("Parent2"));
    pParent1->SetParent(pParent2);
    const MSenElement& pElRoot2=pElement->Root();
    const TDesC8& pElLName2=pElRoot2.LocalName();
	const MSenElement& pParRoot=pParent1->Root();
    const TDesC8& pParLName=pParRoot.LocalName();
    if(pElLName2==_L8("Parent2") && pParLName==_L8("Parent2"))
    	Flag=1;
    else
    	Flag=0;
        
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    delete pElement;
    delete pParent1;
    delete pParent2;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_ElementL (TTestResult& aResult)
    {
    SetupL();
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParent);
    CSenBaseElement* pElement1=CSenBaseElement::NewL(_L8("Nokia1"));
    CleanupStack::PushL(pElement1);
    CSenBaseElement* pElement2=CSenBaseElement::NewL(_L8("Nokia2"));
    CleanupStack::PushL(pElement2);
    CSenBaseElement* pElement3=CSenBaseElement::NewL(_L8("Nokia3"));
    CleanupStack::PushL(pElement3);
    pParent->AddElementL(*pElement1);
    pParent->AddElementL(*pElement2);
    pParent->AddElementL(*pElement3);
    CSenElement* element=pParent->Element(_L8("Nokia1"));
/*    EUNIT_ASSERT( element->LocalName()==_L8("Nokia1") );*/
  	if(!(element->LocalName()==_L8("Nokia1")))
    return KErrArgument;  

    CleanupStack::Pop(3);
    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }
TInt CSenXmlBCTest::CSenBaseElement_ElementsL1 (TTestResult& aResult)
    {
    SetupL();
    //RPointerArray<CSenElement> &array;
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParent);
    CSenBaseElement* pElement1=CSenBaseElement::NewL(_L8("Nokia1"));
    CleanupStack::PushL(pElement1);
    CSenBaseElement* pElement2=CSenBaseElement::NewL(_L8("Nokia2"));
    CleanupStack::PushL(pElement2);
    CSenBaseElement* pElement3=CSenBaseElement::NewL(_L8("Nokia3"));
    CleanupStack::PushL(pElement3);
    pParent->AddElementL(*pElement1);
    pParent->AddElementL(*pElement2);
    pParent->AddElementL(*pElement3);
    RPointerArray<CSenElement>& elementList=pParent->ElementsL();
    pParent->ElementsL(elementList, _L8("Nokia1"), _L8("Nokia1"));
/*    EUNIT_ASSERT( element->LocalName()==_L8("Nokia1") );*/
  	//if(!(element->LocalName()==_L8("Nokia1")))
    //return KErrArgument;  

    CleanupStack::Pop(3);
    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }


TInt CSenXmlBCTest::CSenBaseElement_ElementsL2 (TTestResult& aResult)
    {
    SetupL();
    //RPointerArray<CSenElement> &array;
    
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParent);
    CSenBaseElement* pElement1=CSenBaseElement::NewL(_L8("Nokia1"));
    CleanupStack::PushL(pElement1);
    CSenBaseElement* pElement2=CSenBaseElement::NewL(_L8("Nokia2"));
    CleanupStack::PushL(pElement2);
    CSenBaseElement* pElement3=CSenBaseElement::NewL(_L8("Nokia3"));
    CleanupStack::PushL(pElement3);
    pParent->AddElementL(*pElement1);
    pParent->AddElementL(*pElement2);
    pParent->AddElementL(*pElement3);
    RPointerArray<CSenElement>& elementList=pParent->ElementsL();

    pParent->ElementsL(elementList, _L8("Nokia1"));
/*    EUNIT_ASSERT( element->LocalName()==_L8("Nokia1") );*/
  	//if(!(element->LocalName()==_L8("Nokia1")))
    //return KErrArgument;  

    CleanupStack::Pop(3);
    CleanupStack::Pop(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_CreateElementLL (TTestResult& aResult)
    {
    SetupL();
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("element"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("N"),_L8("nsuri"));
    CSenElement* pNewElement=pElement->CreateElementL(_L8("N"),_L8("Nokia"));
    CleanupStack::PushL(pNewElement);
    HBufC8* ElementBuf=pNewElement->AsXmlL();
/*    EUNIT_ASSERT_EQUALS(*ElementBuf,_L8("<N:Nokia xmlns:N=\"nsuri\"/>"));*/
    TL(*ElementBuf == _L8("<N:Nokia xmlns:N=\"nsuri\"/>"));
    CleanupStack::PopAndDestroy(pNewElement);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_InsertElementLL (TTestResult& aResult)
    {
    SetupL(); 
     TBool Flag;
     CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("Parent"));
     CleanupStack::PushL(pParent);
     CSenBaseElement* pElement1=CSenBaseElement::NewL(_L8("Nokia1"));
     CleanupStack::PushL(pElement1);
     CSenBaseElement* pElement2=CSenBaseElement::NewL(_L8("Nokia2"));
     CleanupStack::PushL(pElement2);
     CSenBaseElement* pElement3=CSenBaseElement::NewL(_L8("Nokia3"));
     CleanupStack::PushL(pElement3);
     CSenBaseElement* pElement5=CSenBaseElement::NewL(_L8("Nokia5"));
     CleanupStack::PushL(pElement5);
     pParent->AddElementL(*pElement1);
     pParent->AddElementL(*pElement2);
     pParent->AddElementL(*pElement3);
     pParent->AddElementL(*pElement5);
     CSenBaseElement* pElement4=CSenBaseElement::NewL(_L8("Nokia4"));
     CleanupStack::PushL(pElement4);
     pParent->InsertElementL(*pElement4,*pElement5);
     RPointerArray<CSenElement>& elementList=pParent->ElementsL();
     const TDesC8& localName=elementList[3]->LocalName();
     if(localName==_L8("Nokia4"))
     	Flag=1;
     else
     	Flag=0;
     /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

     if(!Flag)
     return KErrArgument;
     
     CleanupStack::Pop(pElement4);
	 CleanupStack::Pop(pElement5);
     CleanupStack::Pop(pElement3);
     CleanupStack::Pop(pElement2);
     CleanupStack::Pop(pElement1);
     CleanupStack::PopAndDestroy(pParent); 
    Teardown();
    return KErrNone;
    }
	 
TInt CSenXmlBCTest::CSenBaseElement_InsertElementL_NoBeforeL (TTestResult& aResult)               
	{
    SetupL();
	TBool Flag;
	CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("parent"));
	CleanupStack::PushL(pParent);
	CSenBaseElement* pElement1=CSenBaseElement::NewL(_L8("nokia"));
	CleanupStack::PushL(pElement1);
	
	CSenBaseElement* pElement2=CSenBaseElement::NewL(_L8("nokia2"));
	CleanupStack::PushL(pElement2);
	
	CSenBaseElement* pElement3=CSenBaseElement::NewL(_L8("nokia3"));
	CleanupStack::PushL(pElement3);
	pParent->AddElementL(*pElement1);
	pParent->AddElementL(*pElement2);
	pParent->AddElementL(*pElement3);
	
	
	CSenBaseElement* pElement4=CSenBaseElement::NewL(_L8("nokia4"));
	CleanupStack::PushL(pElement4);
	
	
	CSenBaseElement* pElement5=CSenBaseElement::NewL(_L8("nokia5"));
	CleanupStack::PushL(pElement5);
	
	pParent->InsertElementL(*pElement4,*pElement5);
	RPointerArray<CSenElement>& elementList=pParent->ElementsL();
	const TDesC8& localName=elementList[(elementList.Count()-1)]->LocalName();
	if(localName==_L8("nokia4"))
		Flag=1;
	else
		Flag=0;
		
/*	EUNIT_ASSERT(Flag);*/
	if(!Flag)
    return KErrArgument;
	
	CleanupStack::PopAndDestroy(pElement5);
	CleanupStack::Pop(pElement4);
    CleanupStack::Pop(pElement3);
    CleanupStack::Pop(pElement2);
    CleanupStack::Pop(pElement1);
    CleanupStack::PopAndDestroy(pParent); 
 	Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_AddElementLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement *pParent=CSenBaseElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nokia"));
    CleanupStack::PushL(pElement);
    CSenElement& newElement=pParent->AddElementL(*pElement);
    RPointerArray<CSenElement>& elementList=pParent->ElementsL();
    const TDesC8& localName=elementList[0]->LocalName();
    if(localName==_L8("nokia") && elementList.Count()==1 )
    	Flag=1;
    else
    	Flag=0;
    /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   
    
	if(!Flag)
    return KErrArgument;
	
    CleanupStack::Pop(pElement);
    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_AddElementL_1L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement *pParent=CSenBaseElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenElement& newElement=pParent->AddElementL(_L8("element"));
    RPointerArray<CSenElement>& elementList=pParent->ElementsL();
    const TDesC8& localName=elementList[0]->LocalName(); 
    if(localName==_L8("element") && elementList.Count()==1 )
    	Flag=1;
    else
    	Flag=0;
    /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   
    
   	if(!Flag)
    return KErrArgument;
	
    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_AddElementL_2L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement *pParent=CSenBaseElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenElement& newElement=pParent->AddElementL(_L8("nsuria"),_L8("element"));
    RPointerArray<CSenElement>& elementList=pParent->ElementsL();
    const TDesC8& localName=elementList[0]->LocalName(); 
    const TDesC8& nsUri=elementList[0]->NamespaceURI(); 
    if(localName==_L8("element") && elementList.Count()==1 && nsUri==_L8("nsuria"))
    	Flag=1;
    else
    	Flag=0;
    /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_AddElementL_3L (TTestResult& aResult)
    {
    SetupL();
	TBool Flag;
    CSenBaseElement *pParent=CSenBaseElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenElement& newElement=pParent->AddElementL(_L8("nsuria"),_L8("element"),_L8("e:element"));
    RPointerArray<CSenElement>& elementList=pParent->ElementsL();
    const TDesC8& localName=elementList[0]->LocalName(); 
    const TDesC8& nsUri=elementList[0]->NamespaceURI(); 
    if(localName==_L8("element") && elementList.Count()==1 && nsUri==_L8("nsuria"))
    	Flag=1;
    else
    	Flag=0;
     /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   
	
    CleanupStack::PopAndDestroy(pParent);    
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_RemoveElementL (TTestResult& aResult)
    {
    SetupL();
    TBool TFlag;
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenBaseElement* pElement1=CSenBaseElement::NewL(_L8("Element1"));
    CleanupStack::PushL(pElement1);
    CSenBaseElement* pElement2=CSenBaseElement::NewL(_L8("Element2"));
    CleanupStack::PushL(pElement2);
    CSenBaseElement* pElement3=CSenBaseElement::NewL(_L8("Element3"));
    CleanupStack::PushL(pElement3);
    CSenBaseElement* pElement4=CSenBaseElement::NewL(_L8("Element4"));
    CleanupStack::PushL(pElement4);
    pParent->AddElementL(*pElement1);
    pParent->AddElementL(*pElement2);
    pParent->AddElementL(*pElement3);
    pParent->AddElementL(*pElement4);
    //true case
    CSenElement* pRmdElement=pParent->RemoveElement(*pElement3);
    const TDesC8& localName=pRmdElement->LocalName();
    /*CSenElement* pParElement=pElement3->Parent();//intentional
    const TDesC8& localnmae1=pParElement->LocalName();*/
    RPointerArray<CSenElement>& elList=pParent->ElementsL();
    if(localName==_L8("Element3") && elList.Count()==3 )
    	TFlag=1;
    else
    	TFlag=0;
/*    EUNIT_ASSERT( TFlag );*/
	if(!TFlag)
    return KErrArgument;
	
    CleanupStack::Pop(4);
    CleanupStack::PopAndDestroy(pParent);
    delete pRmdElement;
    //delete pRmdElement1;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_RemoveElement_1L (TTestResult& aResult)
    {
    SetupL();
    TBool TFlag,FFlag;
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    pParent->AddElementL(_L8("element1"));
    pParent->AddElementL(_L8("element2"));
    pParent->AddElementL(_L8("element3"));
    pParent->AddElementL(_L8("element4"));
    //True case
    CSenElement* pRmdElement=pParent->RemoveElement(_L8("element1"));
    RPointerArray<CSenElement>& elList=pParent->ElementsL();   
    if(pRmdElement->LocalName()==_L8("element1") && elList.Count()==3)
    	TFlag=1;
    else
    	TFlag=0;
/*    EUNIT_ASSERT( TFlag );*/
	if(!TFlag)
    return KErrArgument;
	
    //False case
    CSenElement* pRmdElement1=pParent->RemoveElement(_L8("none"));
    elList=pParent->ElementsL();
    if(pRmdElement1==NULL && elList.Count()==3)
    	FFlag=1;
    else
    	FFlag=0;
/*    EUNIT_ASSERT(FFlag);*/
	if(!FFlag)
    return KErrArgument;

	CleanupStack::PopAndDestroy(pParent);
	delete pRmdElement;
	//delete pRmdElement1;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_RemoveElement_2L (TTestResult& aResult)
    {
    SetupL();
    TBool TFlag,FFlag;
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    pParent->AddElementL(_L8("nsuri1"),_L8("element1"));
    pParent->AddElementL(_L8("nsuri2"),_L8("element2"));
    pParent->AddElementL(_L8("nsuri2a"),_L8("element2"));
    pParent->AddElementL(_L8("nsuri4"),_L8("element4"));
    //True case
    CSenElement* pRmdElement=pParent->RemoveElement(_L8("nsuri2a"),_L8("element2"));
    RPointerArray<CSenElement>& elList=pParent->ElementsL();   
    const TDesC8& localName=pRmdElement->LocalName();
    const TDesC8& nsURI=pRmdElement->NamespaceURI();
    if(localName==_L8("element2") && nsURI==_L8("nsuri2a") && elList.Count()==3)
    	TFlag=1;
    else
    	TFlag=0;
    /*EUNIT_ASSERT( TFlag );*/
	if(!TFlag)
    return KErrArgument;

    //False case
    CSenElement* pRmdElement1=pParent->RemoveElement(_L8("element1"),_L8("nothing"));
    elList=pParent->ElementsL();
    if(pRmdElement1==NULL && elList.Count()==3)
    	FFlag=1;
    else
    	FFlag=0;
/*    EUNIT_ASSERT(FFlag);*/
	if(!FFlag)
    return KErrArgument;

    CleanupStack::PopAndDestroy(pParent);
	delete pRmdElement;
	delete pRmdElement1;
    
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_ReplaceElementLL (TTestResult& aResult)
    {
    SetupL();
    TBool FFlag,TFlag;
    _LIT8(KNewElement,"<a:Element1 xmlns:a=\"nsuria\"><subelement1/>New content</a:Element1>");
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenBaseElement* pElement1=CSenBaseElement::NewL(_L8("Element1"));
    CleanupStack::PushL(pElement1);
    pElement1->SetNamespaceL(_L8("a"),_L8("nsuria"));
    pElement1->SetContentL(_L8("old content"));
    CSenBaseElement* pElement2=CSenBaseElement::NewL(_L8("Element2"));
    CleanupStack::PushL(pElement2);
    CSenBaseElement* pElement3=CSenBaseElement::NewL(_L8("Element3"));
    CleanupStack::PushL(pElement3);
    CSenBaseElement* pElement4=CSenBaseElement::NewL(_L8("Element4"));
    CleanupStack::PushL(pElement4);
    pParent->AddElementL(*pElement1);
    pParent->AddElementL(*pElement2);
    pParent->AddElementL(*pElement3);
    pParent->AddElementL(*pElement4); 
    //true case
    CSenBaseElement* pNewElement=CSenBaseElement::NewL(_L8("Element1"));
    pNewElement->SetNamespaceL(_L8("a"),_L8("nsuria"));
    CleanupStack::PushL(pNewElement);
    pNewElement->AddElementL(_L8("subelement1"));
    pNewElement->SetContentL(_L8("New content"));
    CSenElement* pReplacedElement=pParent->ReplaceElementL(*pNewElement);
    RPointerArray<CSenElement>& elList=pParent->ElementsL();
    HBufC8* pRepBuf=elList[(elList.Count()-1)]->AsXmlL();
    if(pReplacedElement!=NULL && elList.Count()==4 && 
    	elList[(elList.Count()-1)]->LocalName()==_L8("Element1") && 
    	*pRepBuf==KNewElement && elList[(elList.Count()-1)]->Content()==_L8("New content") )
    	TFlag=1;
    else
    	TFlag=0;
/*    EUNIT_ASSERT( TFlag );*/
	if(!TFlag)
    return KErrArgument;

    //False case
    CSenBaseElement* pNewElement1=CSenBaseElement::NewL(_L8("New Element1"));
    pNewElement->SetNamespaceL(_L8("a"),_L8("nsuria"));
    CleanupStack::PushL(pNewElement1);
    CSenElement* pReplacedElement1=pParent->ReplaceElementL(*pNewElement1);
    RPointerArray<CSenElement>& elList1=pParent->ElementsL();
    if(pReplacedElement1==NULL && elList1.Count()==5)
    	FFlag=1;
    else
    	FFlag=0;
/*    EUNIT_ASSERT(FFlag);*/
	if(!FFlag)
    return KErrArgument;

    CleanupStack::Pop(pNewElement1);
    CleanupStack::Pop(pNewElement);
    CleanupStack::Pop(4);
    CleanupStack::PopAndDestroy(pParent);
    delete pReplacedElement;
    delete pRepBuf;
    delete pReplacedElement1;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_AsXmlLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    _LIT8(KString,"<parent><element1/><element2/><element3/>parent content</parent>");
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent) ;
    pParent->AddElementL(_L8("element1"));
    pParent->AddElementL(_L8("element2"));
    pParent->SetContentL(_L8("parent content"));
    pParent->AddElementL(_L8("element3"));
    HBufC8* parBuf=pParent->AsXmlL();
    if(*parBuf==KString)
     	Flag=1;
    else
    	Flag=0;
    /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   
    
    delete parBuf;
    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_AsXmlUnicodeLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    _LIT(KString,"<parent><element1/><element2/><element3/>parent content</parent>");
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent) ;
    pParent->AddElementL(_L8("element1"));
    pParent->AddElementL(_L8("element2"));
    pParent->SetContentL(_L8("parent content"));
    pParent->AddElementL(_L8("element3"));
    HBufC* parBuf=pParent->AsXmlUnicodeL();
    if(*parBuf==KString)
     	Flag=1;
    else
    	Flag=0;
    /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   
    
    delete parBuf;
    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_CopyFromLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("xml element"));
    CleanupStack::PushL(pElement);
    pElement->AddElementL(_L8("child1"));
    pElement->AddElementL(_L8("child2"));
    pElement->SetContentL(_L8("element content"));
    pElement->AddNamespaceL(_L8("a"),_L8("nsuri"));
    CSenBaseElement* pCopied=CSenBaseElement::NewL(_L8("copied element"));
    CleanupStack::PushL(pCopied);
    pCopied->SetContentL(_L8("copy content"));
    pCopied->AddElementL(_L8("child1"));
    pCopied->AddNamespaceL(_L8("a"),_L8("nsuri"));
    pCopied->CopyFromL(*pElement);
    const TDesC8& content=pCopied->Content();
    const RPointerArray<CSenElement>& copElList=pCopied->ElementsL();
    const RPointerArray<CSenNamespace>& copNSList=pCopied->NamespacesL();
    if(copElList.Count()==3 && copNSList.Count()==2)
    	Flag=1;
    else
    	Flag=0;
    /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   
    
    CleanupStack::PopAndDestroy(pCopied);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_DetachLL (TTestResult& aResult)
    {
    SetupL();
    CSenBaseElement* pParent=CSenBaseElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenBaseElement* pElement1=CSenBaseElement::NewL(_L8("Element1"));
    CleanupStack::PushL(pElement1);
    pElement1->SetNamespaceL(_L8("a"),_L8("nsuria"));
    pElement1->SetContentL(_L8("old content"));
    CSenBaseElement* pElement2=CSenBaseElement::NewL(_L8("Element2"));
    CleanupStack::PushL(pElement2);
    CSenBaseElement* pElement3=CSenBaseElement::NewL(_L8("Element3"));
    CleanupStack::PushL(pElement3);
    CSenBaseElement* pElement4=CSenBaseElement::NewL(_L8("Element4"));
    CleanupStack::PushL(pElement4);
    pParent->AddElementL(*pElement1);
    pParent->AddElementL(*pElement2);
    pParent->AddElementL(*pElement3);
    pParent->AddElementL(*pElement4); 
    CSenElement* dElement=pElement1->DetachL();
    const RPointerArray<CSenElement>& elList=pParent->ElementsL();
    /*EUNIT_ASSERT( elList.Count()==3 );*/
	if(!(elList.Count()==3))
    return KErrArgument;

    CleanupStack::Pop(4);
    CleanupStack::PopAndDestroy(pParent);
    delete dElement;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_ChildL (TTestResult& aResult)
    {
    SetupL();
    TBool FFlag,TFlag;
    CSenBaseElement*pParent=CSenBaseElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    pParent->AddElementL(_L8("element1"));
    pParent->AddElementL(_L8("element2"));
    pParent->AddElementL(_L8("element3"));
    CSenElement* pElement=pParent->Child(0);
    if(pElement->LocalName()==_L8("element1"))
    	TFlag=1;
    else
    	TFlag=0;
    /*EUNIT_ASSERT( TFlag );*/
	if(!TFlag)
    return KErrArgument;
    
    CSenElement* pElement1=pParent->Child(4);
    if(pElement1==NULL)
    	FFlag=1;
    else
    	FFlag=0;
    /*EUNIT_ASSERT( FFlag );*/
    if(!FFlag)
    return KErrArgument;    
    
    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseElement_SetL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("nokia"));
    CleanupStack::PushL(pElement);
    pElement->Set(_L8("nsuri"),_L8("element"),_L8("e:e:lement"));
    if(pElement->LocalName()==_L8("element") && pElement->NamespaceURI()==_L8("nsuri") 
    		&& pElement->NsPrefix()==_L8("e"))
    		Flag=1;
    else
    		Flag=0;
    
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    if(!Flag)
    return KErrArgument;    
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }    
  
TInt CSenXmlBCTest::CSenBaseElement_AddAttributesLL (TTestResult& aResult)
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
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
      
    CSenBaseElement *pElement = CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->AddAttributesL(attrArray);
    HBufC8* pElementBuf = pElement->AsXmlL();
    /*EUNIT_ASSERT( *pElementBuf == _L8("<Nokia pr:LocalName=\"Value\"/>" ));*/
    if(!(*pElementBuf == _L8("<Nokia pr:LocalName=\"Value\"/>" )))
    return KErrArgument;    

	CleanupStack::PopAndDestroy(pElement);
	CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
	delete pElementBuf;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseElement_AsElementL(TTestResult& aResult)
	{
    SetupL();
	CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
	CleanupStack::PushL(pElement);
	pElement->AddElementL(_L8("webservices"));
	MSenElement* pAsElement=pElement->AsElement();
	HBufC8* pElementBuf=pElement->AsXmlL();
/*	EUNIT_ASSERT_EQUALS(*pElementBuf,_L8("<Nokia><webservices/></Nokia>"));*/
	TL(*pElementBuf == _L8("<Nokia><webservices/></Nokia>"));
	CleanupStack::PopAndDestroy(pElement);
	delete pElementBuf;
	Teardown();
    return KErrNone;
    }
	
TInt CSenXmlBCTest::CSenBaseElement_ContentWriteStreamLL(TTestResult& aResult)
	{
    SetupL();
	TBuf8<50> StreamBuf;
	CSenBaseElement* pElement=CSenBaseElement::NewL(_L8("Nokia"));
	CleanupStack::PushL(pElement);
	pElement->AddElementL(_L8("webservices"));
	RWriteStream& ElemntStream=pElement->ContentWriteStreamL();
	TRAPD(res,ElemntStream.WriteL(StreamBuf));
	CleanupStack::PopAndDestroy(pElement);
	//EUNIT_ASSERT(FALSE);
	Teardown();
    return KErrNone;
    }
	
TInt CSenXmlBCTest::CSenBaseElement_RemoveAttributeLL(TTestResult& aResult)
	{
    SetupL();/*
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
      
    CSenBaseElement *pElement = CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->AddAttributesL(attrArray);
    CSenBaseAttribute* RmdElement=pElement->RemoveAttributeL(_L8("pr:LocalName"));
    HBufC8* pElementBuf = pElement->AsXmlL();
    RPointerArray<CSenBaseAttribute>& AttrList=pElement->AttributesL();
    //EUNIT_ASSERT( *pElementBuf == _L8("<Nokia/>" ));
    if(!(*pElementBuf == _L8("<Nokia/>")))
    return KErrArgument;   

    //EUNIT_ASSERT_EQUALS(AttrList.Count(),0);
    TL(AttrList.Count() == 0);
   	CleanupStack::PopAndDestroy(pElement);
	CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
	delete pElementBuf;
	delete RmdElement;
	//EUNIT_ASSERT(FALSE);
	Teardown();
    return KErrNone;
    */
   	Teardown();
    return KErrNone;

    }
	
TInt CSenXmlBCTest::CSenXmlElement_NewLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("elment"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
/*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   
        
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_NewL_1L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("nsuria"),_L8("element"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
    /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_NewL_2L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("nsuria"),_L8("element"),_L8("a:element"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
   /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_SetAttributesLL (TTestResult& aResult)
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
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
      
    CSenXmlElement *pElement = CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetAttributesL(attrArray);
    HBufC8* pElementBuf = pElement->AsXmlL();
/*    EUNIT_ASSERT( *pElementBuf == _L8("<Nokia pr:LocalName=\"Value\"/>" ));*/
    if(!( *pElementBuf == _L8("<Nokia pr:LocalName=\"Value\"/>" )))
    return KErrArgument;   

	CleanupStack::PopAndDestroy(pElement);
	CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
	delete pElementBuf;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_LocalNameL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("nsuria"),_L8("element"),_L8("a:element"));
    CleanupStack::PushL(pElement);
    const TDesC8& lName=pElement->LocalName();
    if(lName==_L8("element"))
    	Flag=1;
    else
    	Flag=0;
   /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   
   
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_NamespaceURIL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("nsuria"),_L8("element"),_L8("a:element"));
    CleanupStack::PushL(pElement);
    const TDesC8& nsURI=pElement->NamespaceURI();
    if(nsURI==_L8("nsuria"))
    	Flag=1;
    else
    	Flag=0;
   /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_NsPrefixL (TTestResult& aResult)
    {
    SetupL(); 
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("nsuria"),_L8("element"),_L8("a:element"));
    CleanupStack::PushL(pElement);
    const TDesC8& nsPrefix=pElement->NsPrefix();
    if(nsPrefix==_L8("a"))
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_SetPrefixLL (TTestResult& aResult)
    {
    SetupL();//some thing
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("nsuria"),_L8("element"));
    CleanupStack::PushL(pElement);
    pElement->SetPrefixL(_L8("pre"));
    const TDesC8& lName=pElement->LocalName();
    const TDesC8& nsPrefix=pElement->NsPrefix();
    if(nsPrefix==_L8("pre") && lName==_L8("element"))
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
  
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_HasContentL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("element"));
    CleanupStack::PushL(pElement);
    //False case
    Flag=pElement->HasContent();
/*    EUNIT_ASSERT( !Flag );*/
  	if(Flag)
    return KErrArgument;  

    //True case
    pElement->SetContentL(_L8("CONTENT"));
    Flag=pElement->HasContent();
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_ContentL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("element"));
    CleanupStack::PushL(pElement);
    pElement->SetContentL(_L8("CONTENT"));
    const TDesC8& content=pElement->Content();
    if(content==_L8("CONTENT"))
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlElement_ContentWriteStreamLL (TTestResult& aResult)
    {
    SetupL();
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("element"));
    CleanupStack::PushL(pElement);
	RWriteStream stream = pElement->ContentWriteStreamL();
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    

TInt CSenXmlBCTest::CSenXmlElement_ContentUnicodeLL (TTestResult& aResult)
    {
    SetupL();    
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("nsuria"),_L8("Nokia"),_L8("N:Nokia"));
    CleanupStack::PushL(pElement);
	pElement->SetContentL(_L8("Nokia content"));
	HBufC* ElementBuf=pElement->ContentUnicodeL();
/*	EUNIT_ASSERT_EQUALS(*ElementBuf,_L("Nokia content"));*/
	TL(*ElementBuf == _L("Nokia content"));
	CleanupStack::PopAndDestroy(pElement);
	delete ElementBuf;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_SetContentLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("element"));
    CleanupStack::PushL(pElement);
    pElement->SetContentL(_L8("CONTENT"));
    TBool ContentFlag=pElement->HasContent();
    const TDesC8& content=pElement->Content();
    if(content==_L8("CONTENT") && ContentFlag)
   		Flag=1;
    else 
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    pElement->SetContentL(_L8("NEW CONTENT"));
    const TDesC8& newContent=pElement->Content();
    if(newContent==_L8("NEW CONTENT") )
   		Flag=1;
    else 
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_ConsistsOfLL (TTestResult& aResult)
    {
    SetupL();
    CSenXmlElement* pElementA=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElementA);
    CSenXmlElement* pChild1A=CSenXmlElement::NewL(_L8("Asp"));
    CleanupStack::PushL(pChild1A);
    CSenXmlElement* pChild2A=CSenXmlElement::NewL(_L8("WebServices"));
    CleanupStack::PushL(pChild2A);
    pElementA->SetContentL(_L8("Nokia organisation structure"));
    pElementA->AddElementL(*pChild1A);
    pElementA->AddElementL(*pChild2A);
    
    CSenXmlElement* pElementB=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElementB);
    CSenXmlElement* pChild1B=CSenXmlElement::NewL(_L8("Asp"));
    CleanupStack::PushL(pChild1A);
    CSenXmlElement* pChild2B=CSenXmlElement::NewL(_L8("WebServices"));
    CleanupStack::PushL(pChild2B);
    pElementB->SetContentL(_L8("Nokia organisation structure"));
    pElementB->AddElementL(*pChild1B);
    pElementB->AddElementL(*pChild2B);
    
    TBool ConsistOfFlag=pElementB->ConsistsOfL(*pElementA);
/*    EUNIT_ASSERT( ConsistOfFlag );*/
    if(!ConsistOfFlag)
    return KErrArgument;   
    
    CleanupStack::Pop(2);
    CleanupStack::PopAndDestroy(pElementB);
    CleanupStack::Pop(2);
    CleanupStack::PopAndDestroy(pElementA);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlElement_ConsistsOfL1L (TTestResult& aResult)
    {
    SetupL();
    CSenXmlElement* pElementA=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElementA);
    CSenXmlElement* pChild1A=CSenXmlElement::NewL(_L8("Asp"));
    CleanupStack::PushL(pChild1A);
    CSenXmlElement* pChild2A=CSenXmlElement::NewL(_L8("WebServices"));
    CleanupStack::PushL(pChild2A);
    pElementA->SetContentL(_L8("Nokia organisation structure"));
    pElementA->AddElementL(*pChild1A);
    pElementA->AddElementL(*pChild2A);
    
    CSenXmlElement* pElementB=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElementB);
    CSenXmlElement* pChild1B=CSenXmlElement::NewL(_L8("Asp"));
    CleanupStack::PushL(pChild1A);
    CSenXmlElement* pChild2B=CSenXmlElement::NewL(_L8("Xml"));
    CleanupStack::PushL(pChild2B);
    pElementB->SetContentL(_L8("Nokia organisation structure"));
    pElementB->AddElementL(*pChild1B);
    pElementB->AddElementL(*pChild2B);
    
    TBool ConsistOfFlag=pElementB->ConsistsOfL(*pElementA);
/*    EUNIT_ASSERT( !ConsistOfFlag );*/
    if(ConsistOfFlag)
    return KErrArgument;   

    CleanupStack::Pop(2);
    CleanupStack::PopAndDestroy(pElementB);
    CleanupStack::Pop(2);
    CleanupStack::PopAndDestroy(pElementA);
	Teardown();
    return KErrNone;
    }
	
TInt CSenXmlBCTest::CSenXmlElement_ConsistsOfL2L (TTestResult& aResult)
    {
    SetupL();
    CSenXmlElement* pElementA=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElementA);
    CSenXmlElement* pChild1A=CSenXmlElement::NewL(_L8("Asp"));
    CleanupStack::PushL(pChild1A);
    CSenXmlElement* pChild2A=CSenXmlElement::NewL(_L8("WebServices"));
    CleanupStack::PushL(pChild2A);
    CSenXmlElement* pChild3A=CSenXmlElement::NewL(_L8("Xml"));
    CleanupStack::PushL(pChild3A);
    pElementA->SetContentL(_L8("Nokia organisation structure"));
    pElementA->AddElementL(*pChild1A);
    pElementA->AddElementL(*pChild2A);
    pElementA->AddElementL(*pChild3A);
    
    CSenXmlElement* pElementB=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElementB);
    CSenXmlElement* pChild1B=CSenXmlElement::NewL(_L8("Asp"));
    CleanupStack::PushL(pChild1A);
    CSenXmlElement* pChild2B=CSenXmlElement::NewL(_L8("Xml"));
    CleanupStack::PushL(pChild2B);
    pElementB->SetContentL(_L8("Nokia organisation structure"));
    pElementB->AddElementL(*pChild1B);
    pElementB->AddElementL(*pChild2B);
    
    TBool ConsistOfFlag=pElementB->ConsistsOfL(*pElementA);
/*    EUNIT_ASSERT( ConsistOfFlag );*/
    if(!ConsistOfFlag)
    return KErrArgument;   

    CleanupStack::Pop(3);
    CleanupStack::PopAndDestroy(pElementB);
    CleanupStack::Pop(2);
    CleanupStack::PopAndDestroy(pElementA);
	Teardown();
    return KErrNone;
    }	

TInt CSenXmlBCTest::CSenXmlElement_SetNamespaceLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("namespaceuri"));
	const TDesC8& pNsURI=pElement->NamespaceURI();
	if(pNsURI==_L8("namespaceuri"))
		Flag=1;
	else
		Flag=0;
	CleanupStack::PopAndDestroy(pElement);
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlElement_SetNamespaceL_1L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("N"),_L8("namespaceuri"));
	const TDesC8& pNsURI=pElement->NamespaceURI();
	const TDesC8& pNsPrefix=pElement->NsPrefix();
	if(pNsURI==_L8("namespaceuri")&&pNsPrefix==_L8("N"))
		Flag=1;
	else
		Flag=0;
	CleanupStack::PopAndDestroy(pElement);
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    Teardown();
    return KErrNone;
    }
TInt CSenXmlBCTest::CSenXmlElement_SetNamespaceL_2same1L (TTestResult& aResult)
    {
    SetupL();
     TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("N"),_L8("namespaceuri"));
    pElement->SetNamespaceL(_L8("N"),_L8("namespaceuri"));
	const TDesC8& pNsURI=pElement->NamespaceURI();
	const TDesC8& pNsPrefix=pElement->NsPrefix();
	if(pNsURI==_L8("namespaceuri")&&pNsPrefix==_L8("N"))
		Flag=1;
	else
		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

	RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
/* 	EUNIT_ASSERT( pNsList.Count()==1 );*/
    if(!( pNsList.Count()==1 ))
    return KErrArgument;   

	CleanupStack::PopAndDestroy(pElement); 
    Teardown();
    return KErrNone;
    }    
 
TInt CSenXmlBCTest::CSenXmlElement_SetNamespaceL_2Diff1L (TTestResult& aResult)
    {
    SetupL();
     TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("N1"),_L8("namespaceuri1"));
    pElement->SetNamespaceL(_L8("N2"),_L8("namespaceuri2"));
	const TDesC8& pNsURI=pElement->NamespaceURI();
	const TDesC8& pNsPrefix=pElement->NsPrefix();
	if(pNsURI==_L8("namespaceuri2")&&pNsPrefix==_L8("N2"))
		Flag=1;
	else
		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

	RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
/* 	EUNIT_ASSERT( pNsList.Count()==2 );*/
    if(!( pNsList.Count()==2 ))
    return KErrArgument;   

 	CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }    

TInt CSenXmlBCTest::CSenXmlElement_AddNamespaceLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    TBool CheckParent=TRUE;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    CSenXmlElement* pParent=CSenXmlElement::NewL(_L8("ParentElement"));
    CleanupStack::PushL(pParent);
    
    pElement->SetParent(pParent);
    CSenNamespace* pNs1=CSenNamespace::NewL(_L8("N"),_L8("namespaceuri"));
    CleanupStack::PushL(pNs1);
    const CSenNamespace* pNs=pElement->AddNamespaceL(*pNs1,CheckParent);
    RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
	const TDesC8& pNsURI=pNs->URI();
	const TDesC8& pNsPrefix=pNs->Prefix();
	if(pNsURI==_L8("namespaceuri")&&pNsPrefix==_L8("N"))
		Flag=1;
	else
		Flag=0;
	
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

/*    EUNIT_ASSERT( pNsList.Count()==1);*/
    if(!( pNsList.Count()==1 ))
    return KErrArgument;   

   	CleanupStack::PopAndDestroy(pNs1);
   	CleanupStack::PopAndDestroy(pParent);	
	CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_AddNamespaceL_1L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    const CSenNamespace* pNs=pElement->AddNamespaceL(_L8("N"),_L8("namespaceuri"));
	const TDesC8& pNsURI=pNs->URI();
	const TDesC8& pNsPrefix=pNs->Prefix();
	if(pNsURI==_L8("namespaceuri")&&pNsPrefix==_L8("N"))
		Flag=1;
	else
		Flag=0;
	CleanupStack::PopAndDestroy(pElement);
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_NamespaceL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag,Flag1,Flag2;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    //NULL
    pElement->SetNamespaceL(_L8(""));
    const CSenNamespace* pNs2=pElement->Namespace();
    if(pNs2==NULL)
    	Flag2=1;
    else
    	Flag2=0;
/*    EUNIT_ASSERT(Flag2);*/
    if(!Flag2)
    return KErrArgument;   
   
   
    //only URI
    pElement->SetNamespaceL(_L8("nsuri"));
    const CSenNamespace* pNs1=pElement->Namespace();
    if(pNs1->Prefix()==_L8("")&&pNs1->URI()==_L8("nsuri"))
    	Flag1=1;
    else
    	Flag1=0;
/*    EUNIT_ASSERT(Flag1);*/
    if(!Flag1)
    return KErrArgument;   

    //URI and Prefix
    pElement->SetNamespaceL(_L8("n"),_L8("nsuri"));
    const CSenNamespace* pNs=pElement->Namespace();
    if(pNs->Prefix()==_L8("n") && pNs->URI()==_L8("nsuri"))
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);

    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_Namespace_1L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag,Flag1;
     //True case
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("n"),_L8("nsuri"));
    const CSenNamespace* pNs=pElement->Namespace(_L8("n"));
    if(pNs->Prefix()==_L8("n") && pNs->URI()==_L8("nsuri"))
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    //False case
    pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("n"),_L8("nsuri"));
    pNs=pElement->Namespace(_L8("N"));
    if(pNs==NULL)
    	Flag1=1;
    else
    	Flag1=0;
/*    EUNIT_ASSERT(Flag1);*/
    if(!Flag1)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_Namespace_2L (TTestResult& aResult)
    {
    SetupL();
	TBool Flag,Flag1;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    CSenXmlElement* pParent=CSenXmlElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParent);
    pParent->SetNamespaceL(_L8("p"),_L8("nsurip"));
    pElement->SetNamespaceL(_L8("n"),_L8("nsurin"));
    pElement->SetParent(pParent);
    //aCheckInParent=FALSE and parent prefix
    const CSenNamespace* pPNs=pElement->Namespace(_L8("p"),FALSE);
    if(pPNs==NULL)
    	Flag=1;
   	else
   		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    //aCheckInParent=TRUE and parent prefix
    pPNs=pElement->Namespace(_L8("p"),TRUE);
    if(pPNs->Prefix()==_L8("p") && pPNs->URI()==_L8("nsurip"))
    	Flag1=1; 
   	else
   		Flag1=0;
/*    EUNIT_ASSERT( Flag1 );*/
    if(!Flag1)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pParent);
    CleanupStack::PopAndDestroy(pElement);    
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_Namespace_3L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->AddNamespaceL(_L8("n"),_L8("nsurin"));
    pElement->AddNamespaceL(_L8("m"),_L8("nsurim"));
    pElement->AddNamespaceL(_L8("l"),_L8("nsuril"));
    RPointerArray<CSenNamespace>& pNsList=pElement->NamespacesL();
/*    EUNIT_ASSERT( pNsList.Count()==3 );*/
    if(!( pNsList.Count()==3 ))
    return KErrArgument;   

    const CSenNamespace* pNs=pElement->Namespace(_L8("m"),_L8("nsurim"));
    if(pNs->Prefix()==_L8("m") && pNs->URI()==_L8("nsurim"))
    	Flag=1;
    else
    	Flag=0;
    /*EUNIT_ASSERT(Flag);*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_ElementsLL (TTestResult& aResult)
    {
    SetupL();
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia")) ;
    CleanupStack::PushL(pElement);
    CSenXmlElement* pChild1=CSenXmlElement::NewL(_L8("element1")) ;
    CleanupStack::PushL(pChild1);
    CSenXmlElement* pChild2=CSenXmlElement::NewL(_L8("element2")) ;
    CleanupStack::PushL(pChild2);
    CSenXmlElement* pChild3=CSenXmlElement::NewL(_L8("element3")) ;
    CleanupStack::PushL(pChild3);
    pElement->AddElementL(*pChild1);
    pElement->AddElementL(*pChild2);
    pElement->AddElementL(*pChild3);
    RPointerArray<CSenElement>& ElList=pElement->ElementsL();
    /*EUNIT_ASSERT( ElList.Count()==3);*/
    if(!( ElList.Count()==3 ))
    return KErrArgument;  
    
    CleanupStack::Pop(3);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_AttributesLL (TTestResult& aResult)
    {
    SetupL();
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia")) ;
    pElement->AddAttrL(_L8("Attr1"),_L8("value1"));
    pElement->AddAttrL(_L8("Attr2"),_L8("value2"));
    pElement->AddAttrL(_L8("Attr3"),_L8("value3"));
    pElement->AddAttrL(_L8("Attr4"),_L8("value4"));    
    pElement->AddAttrL(_L8("Attr4"),_L8("value4"));    
    RPointerArray<CSenBaseAttribute>& pAttrList=pElement->AttributesL();
/*    EUNIT_ASSERT( pAttrList.Count()==4 );*/
    if(!( pAttrList.Count()==4 ))
    return KErrArgument;  
    
    delete pElement;
    Teardown();
    return KErrNone;
    }
TInt CSenXmlBCTest::CSenXmlElement_AddAttributeLL (TTestResult& aResult)
    {
    SetupL();
    CTestXmlElement* pElement=CTestXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->AddAttributeL(_L8("Attr1"),_L8("value1"));
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_NamespacesLL (TTestResult& aResult)
    {
    SetupL();
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->AddNamespaceL(_L8("l"),_L8("nsuril"));
    pElement->AddNamespaceL(_L8("m"),_L8("nsurim"));
    pElement->AddNamespaceL(_L8("n"),_L8("nsurin"));
    pElement->AddNamespaceL(_L8("m"),_L8("nsurim"));
    const RPointerArray<CSenNamespace>& pNs=pElement->NamespacesL();
/*    EUNIT_ASSERT( pNs.Count()==3);*/
    if(!( pNs.Count()==3 ))
    return KErrArgument;  
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_AttrValueL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag,NegFlag;
    _LIT8(KValue,"value4");
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia")) ;
    pElement->AddAttrL(_L8("Attr1"),_L8("value1"));
    pElement->AddAttrL(_L8("Attr2"),_L8("value2"));
    pElement->AddAttrL(_L8("Attr3"),_L8("value3"));
    pElement->AddAttrL(_L8("Attr4"),_L8("value4"));    
    pElement->AddAttrL(_L8("Attr4"),_L8("value4"));    
    const TDesC8* pAttrVal=pElement->AttrValue(_L8("Attr4"));
    if(pAttrVal!=NULL)
    	Flag=1;
    else
    	Flag=0;
/*    EUNIT_ASSERT(Flag);*/
    if(!(Flag))
    return KErrArgument;  
    
    const TDesC8* pNegAttrVal=pElement->AttrValue(_L8("attr1"));
    if(pNegAttrVal==NULL)
    	NegFlag=1;
    else
    	NegFlag=0;
    /*EUNIT_ASSERT(NegFlag);*/
    if(!(NegFlag))
    return KErrArgument;  
    
    delete pElement;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_AddAttrLL (TTestResult& aResult)
    {
    SetupL();
	CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->AddAttrL(_L8("Attr1"),_L8("value1"));
    RPointerArray<CSenBaseAttribute>& pAttrList=pElement->AttributesL();
/*    EUNIT_ASSERT( pAttrList.Count()==1 );*/
    if(!(pAttrList.Count()==1))
    return KErrArgument;  

    pElement->AddAttrL(_L8("Attr1"),_L8("Duplicatevalue1"));
    /*EUNIT_ASSERT( pAttrList.Count()==1 );*/
    if(!(pAttrList.Count()==1))
    return KErrArgument;  
    
    const TDesC8* pAttrVal=pElement->AttrValue(_L8("Attr1"));
    pElement->AddAttrL(_L8("Attr2"),_L8(" value2"));//improve here by comparing TDesC8* AttrValue()
    /*EUNIT_ASSERT( pAttrList.Count()==2 );*/
    if(!(pAttrList.Count()==2))
    return KErrArgument;  
    
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_ParentL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag,PFlag,PPFlag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CSenXmlElement* pParent=CSenXmlElement::NewL(_L8("Parent"));
    CSenXmlElement* pPParent=CSenXmlElement::NewL(_L8("parent of parent"));
    pElement->SetParent(pParent);
    pParent->SetParent(pPParent);
    CSenElement* pEl1= pElement->Parent();
    CSenElement* pEl2= pParent->Parent();
    CSenElement* pEl3= pPParent->Parent();
    const TDesC8& pLName1=pEl1->LocalName();
    if(pLName1==_L8("Parent"))
    	Flag=1;
   	else
   		Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    const TDesC8& pLName2=pEl2->LocalName();
    if(pLName2==_L8("parent of parent"))
    	PFlag=1;
    else
    	PFlag=0;
/*    EUNIT_ASSERT(PFlag);*/
    if(!PFlag)
    return KErrArgument;  
    
    if(pEl3==NULL)
    	PPFlag=1;
    else
    	PPFlag=0;
    /*EUNIT_ASSERT(PPFlag);*/
    if(!PPFlag)
    return KErrArgument;  
    
    delete pElement;
    delete pParent;
    delete pPParent;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_SetParentL (TTestResult& aResult)
    {
    SetupL();
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    CSenXmlElement* pParent=CSenXmlElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParent);
    pElement->SetNamespaceL(_L8("Nsuria"));
    pParent->SetNamespaceL(_L8("nsurip"));
    pElement->SetParent(pParent);
    CSenElement* pPar=pElement->Parent();
    const TDesC8& ParName=pPar->LocalName();
/*    EUNIT_ASSERT_EQUALS( ParName,_L8("Parent") ); */
    TL(ParName == _L8("Parent"));
    CleanupStack::PopAndDestroy(pParent);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_RootL (TTestResult& aResult)
    {
    SetupL();
	TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
    const MSenElement& pElRoot=pElement->Root();
    const TDesC8& pElLName=pElRoot.LocalName();
    if(pElLName==_L8("Nokia"))
    	Flag=1;
    else
    	Flag=0;
    
    CSenXmlElement* pParent1=CSenXmlElement::NewL(_L8("Parent1"));
    pElement->SetParent(pParent1);
    const MSenElement& pElRoot1=pElement->Root();
    const TDesC8& pElLName1=pElRoot1.LocalName();
    if(pElLName1==_L8("Parent1"))
    	Flag=1;
    else
    	Flag=0;
    
    CSenXmlElement* pParent2=CSenXmlElement::NewL(_L8("Parent2"));
    pParent1->SetParent(pParent2);
    const MSenElement& pElRoot2=pElement->Root();
    const TDesC8& pElLName2=pElRoot2.LocalName();
	const MSenElement& pParRoot=pParent1->Root();
    const TDesC8& pParLName=pParRoot.LocalName();
    if(pElLName2==_L8("Parent2") && pParLName==_L8("Parent2"))
    	Flag=1;
    else
    	Flag=0;
        
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    delete pElement;
    delete pParent1;
    delete pParent2;    
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_ElementL (TTestResult& aResult)
    {
    SetupL();
    CSenXmlElement* pParent=CSenXmlElement::NewL(_L8("Parent"));
    CleanupStack::PushL(pParent);
    CSenXmlElement* pElement1=CSenXmlElement::NewL(_L8("Nokia1"));
    CleanupStack::PushL(pElement1);
    CSenXmlElement* pElement2=CSenXmlElement::NewL(_L8("Nokia2"));
    CleanupStack::PushL(pElement2);
    CSenXmlElement* pElement3=CSenXmlElement::NewL(_L8("Nokia3"));
    CleanupStack::PushL(pElement3);
    pParent->AddElementL(*pElement1);
    pParent->AddElementL(*pElement2);
    pParent->AddElementL(*pElement3);
    CSenElement* element=pParent->Element(_L8("Nokia1"));
/*    EUNIT_ASSERT( element->LocalName()==_L8("Nokia1") );*/
    if(!(element->LocalName()==_L8("Nokia1")))
    return KErrArgument;  

    CleanupStack::Pop(3);
    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_CreateElementLL (TTestResult& aResult)
    {
    SetupL();
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("element"));
    CleanupStack::PushL(pElement);
    pElement->SetNamespaceL(_L8("N"),_L8("nsuri"));
    CSenElement* pNewElement=pElement->CreateElementL(_L8("N"),_L8("Nokia"));
    CleanupStack::PushL(pNewElement);
    HBufC8* ElementBuf=pNewElement->AsXmlL();
/*    EUNIT_ASSERT_EQUALS(*ElementBuf,_L8("<N:Nokia xmlns:N=\"nsuri\"/>"));*/
    TL(*ElementBuf == _L8("<N:Nokia xmlns:N=\"nsuri\"/>"));
    CleanupStack::PopAndDestroy(pNewElement);
    CleanupStack::PopAndDestroy(pElement);
    delete ElementBuf;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_InsertElementLL (TTestResult& aResult)
    {
    SetupL();
    
     TBool Flag;
     CSenXmlElement* pParent=CSenXmlElement::NewL(_L8("Parent"));
     CleanupStack::PushL(pParent);
     CSenXmlElement* pElement1=CSenXmlElement::NewL(_L8("Nokia1"));
     CleanupStack::PushL(pElement1);
     CSenXmlElement* pElement2=CSenXmlElement::NewL(_L8("Nokia2"));
     CleanupStack::PushL(pElement2);
     CSenXmlElement* pElement3=CSenXmlElement::NewL(_L8("Nokia3"));
     CleanupStack::PushL(pElement3);
     CSenXmlElement* pElement5=CSenXmlElement::NewL(_L8("Nokia5"));
     CleanupStack::PushL(pElement5);
     pParent->AddElementL(*pElement1);
     pParent->AddElementL(*pElement2);
     pParent->AddElementL(*pElement3);
     pParent->AddElementL(*pElement5);
     CSenXmlElement* pElement4=CSenXmlElement::NewL(_L8("Nokia4"));
     CleanupStack::PushL(pElement4);
     pParent->InsertElementL(*pElement4,*pElement5);
     RPointerArray<CSenElement>& elementList=pParent->ElementsL();
     const TDesC8& localName=elementList[3]->LocalName();
     if(localName==_L8("Nokia4"))
     	Flag=1;
     else
     	Flag=0;
         /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

     CleanupStack::Pop(pElement4);
	 CleanupStack::Pop(pElement5);
     CleanupStack::Pop(pElement3);
     CleanupStack::Pop(pElement2);
     CleanupStack::Pop(pElement1);
     CleanupStack::PopAndDestroy(pParent); 
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_AddElementLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement *pParent=CSenXmlElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("nokia"));
    CleanupStack::PushL(pElement);
    CSenElement& newElement=pParent->AddElementL(*pElement);
    RPointerArray<CSenElement>& elementList=pParent->ElementsL();
    const TDesC8& localName=elementList[0]->LocalName();
    const TDesC8& localName1=newElement.LocalName();    
    if(localName==_L8("nokia") && elementList.Count()==1 )//&& newElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::Pop(pElement);
    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_AddElementL_1L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement *pParent=CSenXmlElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenElement& newElement=pParent->AddElementL(_L8("element"));
    RPointerArray<CSenElement>& elementList=pParent->ElementsL();
    const TDesC8& localName=elementList[0]->LocalName(); 
    if(localName==_L8("element") && elementList.Count()==1 )
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_AddElementL_2L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement *pParent=CSenXmlElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenElement& newElement=pParent->AddElementL(_L8("nsuria"),_L8("element"));
    RPointerArray<CSenElement>& elementList=pParent->ElementsL();
    const TDesC8& localName=elementList[0]->LocalName(); 
    const TDesC8& nsUri=elementList[0]->NamespaceURI(); 
    if(localName==_L8("element") && elementList.Count()==1 && nsUri==_L8("nsuria"))
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_AddElementL_3L (TTestResult& aResult)
    {
    SetupL();
	TBool Flag;
    CSenXmlElement *pParent=CSenXmlElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenElement& newElement=pParent->AddElementL(_L8("nsuria"),_L8("element"),_L8("e:element"));
    RPointerArray<CSenElement>& elementList=pParent->ElementsL();
    const TDesC8& localName=elementList[0]->LocalName(); 
    const TDesC8& nsUri=elementList[0]->NamespaceURI(); 
    if(localName==_L8("element") && elementList.Count()==1 && nsUri==_L8("nsuria"))
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pParent);    
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_RemoveElementL (TTestResult& aResult)
    {
    SetupL();
    TBool TFlag;
    CSenXmlElement* pParent=CSenXmlElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenXmlElement* pElement1=CSenXmlElement::NewL(_L8("Element1"));
    CleanupStack::PushL(pElement1);
    CSenXmlElement* pElement2=CSenXmlElement::NewL(_L8("Element2"));
    CleanupStack::PushL(pElement2);
    CSenXmlElement* pElement3=CSenXmlElement::NewL(_L8("Element3"));
    CleanupStack::PushL(pElement3);
    CSenXmlElement* pElement4=CSenXmlElement::NewL(_L8("Element4"));
    CleanupStack::PushL(pElement4);
    pParent->AddElementL(*pElement1);
    pParent->AddElementL(*pElement2);
    pParent->AddElementL(*pElement3);
    pParent->AddElementL(*pElement4);
    //true case
    CSenElement* pRmdElement=pParent->RemoveElement(*pElement3);
    const TDesC8& localName=pRmdElement->LocalName();
    /*CSenElement* pParElement=pElement3->Parent();//intentional
    const TDesC8& localnmae1=pParElement->LocalName();*/
    RPointerArray<CSenElement>& elList=pParent->ElementsL();
    if(localName==_L8("Element3") && elList.Count()==3 )
    	TFlag=1;
    else
    	TFlag=0;
/*    EUNIT_ASSERT( TFlag );*/
    if(!TFlag)
    return KErrArgument;  
    
    CleanupStack::Pop(4);
    CleanupStack::PopAndDestroy(pParent);
    delete pRmdElement;
    //delete pRmdElement1;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_RemoveElement_1L (TTestResult& aResult)
    {
    SetupL();
    TBool TFlag,FFlag;
    CSenXmlElement* pParent=CSenXmlElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    pParent->AddElementL(_L8("element1"));
    pParent->AddElementL(_L8("element2"));
    pParent->AddElementL(_L8("element3"));
    pParent->AddElementL(_L8("element4"));
    //True case
    CSenElement* pRmdElement=pParent->RemoveElement(_L8("element1"));
    RPointerArray<CSenElement>& elList=pParent->ElementsL();   
    if(pRmdElement->LocalName()==_L8("element1") && elList.Count()==3)
    	TFlag=1;
    else
    	TFlag=0;
/*    EUNIT_ASSERT( TFlag );*/
    if(!TFlag)
    return KErrArgument;  

    //False case
    CSenElement* pRmdElement1=pParent->RemoveElement(_L8("none"));
    elList=pParent->ElementsL();
    if(pRmdElement1==NULL && elList.Count()==3)
    	FFlag=1;
    else
    	FFlag=0;
/*    EUNIT_ASSERT(FFlag);*/
    if(!FFlag)
    return KErrArgument;  

	CleanupStack::PopAndDestroy(pParent);
	delete pRmdElement;
	//delete pRmdElement1;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_RemoveElement_2L (TTestResult& aResult)
    {
    SetupL();
    TBool TFlag,FFlag;
    CSenXmlElement* pParent=CSenXmlElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    pParent->AddElementL(_L8("nsuri1"),_L8("element1"));
    pParent->AddElementL(_L8("nsuri2"),_L8("element2"));
    pParent->AddElementL(_L8("nsuri2a"),_L8("element2"));
    pParent->AddElementL(_L8("nsuri4"),_L8("element4"));
    //True case
    CSenElement* pRmdElement=pParent->RemoveElement(_L8("nsuri2a"),_L8("element2"));
    RPointerArray<CSenElement>& elList=pParent->ElementsL();   
    const TDesC8& localName=pRmdElement->LocalName();
    const TDesC8& nsURI=pRmdElement->NamespaceURI();
    if(localName==_L8("element2") && nsURI==_L8("nsuri2a") && elList.Count()==3)
    	TFlag=1;
    else
    	TFlag=0;
/*    EUNIT_ASSERT( TFlag );*/
    if(!TFlag)
    return KErrArgument;  

    //False case
    CSenElement* pRmdElement1=pParent->RemoveElement(_L8("element1"),_L8("nothing"));
    elList=pParent->ElementsL();
    if(pRmdElement1==NULL && elList.Count()==3)
    	FFlag=1;
    else
    	FFlag=0;
/*    EUNIT_ASSERT(FFlag);*/
    if(!FFlag)
    return KErrArgument;  

    CleanupStack::PopAndDestroy(pParent);
	delete pRmdElement;
	delete pRmdElement1;
    
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_ReplaceElementLL (TTestResult& aResult)
    {
    SetupL();
    TBool FFlag,TFlag;
    _LIT8(KNewElement,"<a:Element1 xmlns:a=\"nsuria\"><subelement1/>New content</a:Element1>");
    CSenXmlElement* pParent=CSenXmlElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenXmlElement* pElement1=CSenXmlElement::NewL(_L8("Element1"));
    CleanupStack::PushL(pElement1);
    pElement1->SetNamespaceL(_L8("a"),_L8("nsuria"));
    pElement1->SetContentL(_L8("old content"));
    CSenXmlElement* pElement2=CSenXmlElement::NewL(_L8("Element2"));
    CleanupStack::PushL(pElement2);
    CSenXmlElement* pElement3=CSenXmlElement::NewL(_L8("Element3"));
    CleanupStack::PushL(pElement3);
    CSenXmlElement* pElement4=CSenXmlElement::NewL(_L8("Element4"));
    CleanupStack::PushL(pElement4);
    pParent->AddElementL(*pElement1);
    pParent->AddElementL(*pElement2);
    pParent->AddElementL(*pElement3);
    pParent->AddElementL(*pElement4); 
    //true case
    CSenXmlElement* pNewElement=CSenXmlElement::NewL(_L8("Element1"));
    pNewElement->SetNamespaceL(_L8("a"),_L8("nsuria"));
    CleanupStack::PushL(pNewElement);
    pNewElement->AddElementL(_L8("subelement1"));
    pNewElement->SetContentL(_L8("New content"));
    CSenElement* pReplacedElement=pParent->ReplaceElementL(*pNewElement);
    RPointerArray<CSenElement>& elList=pParent->ElementsL();
    HBufC8* pRepBuf=elList[(elList.Count()-1)]->AsXmlL();
    if(pReplacedElement!=NULL && elList.Count()==4 && 
    	elList[(elList.Count()-1)]->LocalName()==_L8("Element1") && 
    	*pRepBuf==KNewElement && elList[(elList.Count()-1)]->Content()==_L8("New content") )
    	TFlag=1;
    else
    	TFlag=0;
/*    EUNIT_ASSERT( TFlag );*/
    if(!TFlag)
    return KErrArgument;  

    //False case
    CSenXmlElement* pNewElement1=CSenXmlElement::NewL(_L8("New Element1"));
    pNewElement->SetNamespaceL(_L8("a"),_L8("nsuria"));
    CleanupStack::PushL(pNewElement1);
    CSenElement* pReplacedElement1=pParent->ReplaceElementL(*pNewElement1);
    RPointerArray<CSenElement>& elList1=pParent->ElementsL();
    if(pReplacedElement1==NULL && elList1.Count()==5)
    	FFlag=1;
    else
    	FFlag=0;
/*    EUNIT_ASSERT(FFlag);*/
    if(!FFlag)
    return KErrArgument;  

    CleanupStack::Pop(pNewElement1);
    CleanupStack::Pop(pNewElement);
    CleanupStack::Pop(4);
    CleanupStack::PopAndDestroy(pParent);
    delete pReplacedElement;
    delete pRepBuf;
    delete pReplacedElement1;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_AsXmlLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    _LIT8(KString,"<parent><element1/><element2/><element3/>parent content</parent>");
    CSenXmlElement* pParent=CSenXmlElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent) ;
    pParent->AddElementL(_L8("element1"));
    pParent->AddElementL(_L8("element2"));
    pParent->SetContentL(_L8("parent content"));
    pParent->AddElementL(_L8("element3"));
    HBufC8* parBuf=pParent->AsXmlL();
    if(*parBuf==KString)
     	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    delete parBuf;
    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_AsXmlUnicodeLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    _LIT(KString,"<parent><element1/><element2/><element3/>parent content</parent>");
    CSenXmlElement* pParent=CSenXmlElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent) ;
    pParent->AddElementL(_L8("element1"));
    pParent->AddElementL(_L8("element2"));
    pParent->SetContentL(_L8("parent content"));
    pParent->AddElementL(_L8("element3"));
    HBufC* parBuf=pParent->AsXmlUnicodeL();
    if(*parBuf==KString)
     	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    delete parBuf;
    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_CopyFromLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("xml element"));
    CleanupStack::PushL(pElement);
    pElement->AddElementL(_L8("child1"));
    pElement->AddElementL(_L8("child2"));
    pElement->SetContentL(_L8("element content"));
    pElement->AddNamespaceL(_L8("a"),_L8("nsuri"));
    CSenXmlElement* pCopied=CSenXmlElement::NewL(_L8("copied element"));
    CleanupStack::PushL(pCopied);
    pCopied->SetContentL(_L8("copy content"));
    pCopied->AddElementL(_L8("child1"));
    pCopied->AddNamespaceL(_L8("a"),_L8("nsuri"));
    pCopied->CopyFromL(*pElement);
    const TDesC8& content=pCopied->Content();
    const RPointerArray<CSenElement>& copElList=pCopied->ElementsL();
    const RPointerArray<CSenNamespace>& copNSList=pCopied->NamespacesL();
    if(copElList.Count()==3 && copNSList.Count()==2)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pCopied);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_DetachLL (TTestResult& aResult)
    {
    SetupL();
    CSenXmlElement* pParent=CSenXmlElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    CSenXmlElement* pElement1=CSenXmlElement::NewL(_L8("Element1"));
    CleanupStack::PushL(pElement1);
    pElement1->SetNamespaceL(_L8("a"),_L8("nsuria"));
    pElement1->SetContentL(_L8("old content"));
    CSenXmlElement* pElement2=CSenXmlElement::NewL(_L8("Element2"));
    CleanupStack::PushL(pElement2);
    CSenXmlElement* pElement3=CSenXmlElement::NewL(_L8("Element3"));
    CleanupStack::PushL(pElement3);
    CSenXmlElement* pElement4=CSenXmlElement::NewL(_L8("Element4"));
    CleanupStack::PushL(pElement4);
    pParent->AddElementL(*pElement1);
    pParent->AddElementL(*pElement2);
    pParent->AddElementL(*pElement3);
    pParent->AddElementL(*pElement4); 
    CSenElement* dElement=pElement1->DetachL();
    const RPointerArray<CSenElement>& elList=pParent->ElementsL();
/*    EUNIT_ASSERT( elList.Count()==3 );*/
    if(!( elList.Count()==3 ))
    return KErrArgument;  

    CleanupStack::Pop(4);
    CleanupStack::PopAndDestroy(pParent);
    delete dElement;
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_ChildL (TTestResult& aResult)
    {
    SetupL();
    TBool FFlag,TFlag;
    CSenXmlElement*pParent=CSenXmlElement::NewL(_L8("parent"));
    CleanupStack::PushL(pParent);
    pParent->AddElementL(_L8("element1"));
    pParent->AddElementL(_L8("element2"));
    pParent->AddElementL(_L8("element3"));
    CSenElement* pElement=pParent->Child(0);
    if(pElement->LocalName()==_L8("element1"))
    	TFlag=1;
    else
    	TFlag=0;
/*    EUNIT_ASSERT( TFlag );*/
    if(!TFlag)
    return KErrArgument;  

    CSenElement* pElement1=pParent->Child(4);
    if(pElement1==NULL)
    	FFlag=1;
    else
    	FFlag=0;
/*    EUNIT_ASSERT( FFlag );*/
    if(!FFlag)
    return KErrArgument;  
    
    
    CleanupStack::PopAndDestroy(pParent);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_SetL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("nokia"));
    CleanupStack::PushL(pElement);
    pElement->Set(_L8("nsuri"),_L8("element"),_L8("e:e:lement"));
    if(pElement->LocalName()==_L8("element") && pElement->NamespaceURI()==_L8("nsuri") 
    		&& pElement->NsPrefix()==_L8("e"))
    		Flag=1;
    else
    		Flag=0;
    
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlElement_AddAttributesLL (TTestResult& aResult)
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
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
      
    CSenXmlElement *pElement = CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->AddAttributesL(attrArray);
    HBufC8* pElementBuf = pElement->AsXmlL();
/*    EUNIT_ASSERT( *pElementBuf == _L8("<Nokia pr:LocalName=\"Value\"/>" ));*/
    if(!( *pElementBuf == _L8("<Nokia pr:LocalName=\"Value\"/>" )))
    return KErrArgument;  

	CleanupStack::PopAndDestroy(pElement);
	CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
	delete pElementBuf;
    Teardown();
    return KErrNone;
    }
/*    
TInt CSenXmlBCTest::CSenXmlElement_RemoveAttributeLL (TTestResult& aResult)
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
    CleanupStack::Pop(4); 
    CleanupClosePushL(attribute);

    RAttributeArray attrArray;
    attrArray.AppendL(attribute);
    CleanupClosePushL(attrArray); 
      
    CSenXmlElement *pElement = CSenXmlElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    pElement->AddAttributesL(attrArray);
    CSenBaseAttribute* RmdElement=pElement->RemoveAttributeL(_L8("pr:LocalName"));
    HBufC8* pElementBuf = pElement->AsXmlL();
    RPointerArray<CSenBaseAttribute>& AttrList=pElement->AttributesL();
    EUNIT_ASSERT( *pElementBuf == _L8("<Nokia/>" ));
    EUNIT_ASSERT_EQUALS(AttrList.Count(),0);
    TL(AttrList.Count() == 0);
   	CleanupStack::PopAndDestroy(pElement);
	CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
	delete pElementBuf;
	delete RmdElement;
    Teardown();
    return KErrNone;
    }
*/
TInt CSenXmlBCTest::CSenXmlElement_AsElementL(TTestResult& aResult)
	{
    SetupL();
	CSenXmlElement* pElement=CSenXmlElement::NewL(_L8("Nokia"));
	CleanupStack::PushL(pElement);
	pElement->AddElementL(_L8("webservices"));
	MSenElement* pAsElement=pElement->AsElement();
	HBufC8* pElementBuf=pElement->AsXmlL();
/*	EUNIT_ASSERT_EQUALS(*pElementBuf,_L8("<Nokia><webservices/></Nokia>"));*/
	TL(*pElementBuf == _L8("<Nokia><webservices/></Nokia>"));
	CleanupStack::PopAndDestroy(pElement);
	delete pElementBuf;
	Teardown();
    return KErrNone;
    }
	
TInt CSenXmlBCTest::CSenBaseFragment_NewL_L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* ele = CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(ele);
    CSenElement* parent = ele->Parent();
    CSenBaseFragment* pElement=CSenBaseFragment::NewL(*ele);
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    CleanupStack::PopAndDestroy(ele);
    Teardown();
    return KErrNone;
    }
	
TInt CSenXmlBCTest::CSenBaseFragment_NewL_1L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseFragment* pElement=CSenBaseFragment::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseFragment_NewL_2L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseFragment* pElement=CSenBaseFragment::NewL(_L8("nsuri"),_L8("Nokia"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenBaseFragment_NewL_3L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseFragment* pElement=CSenBaseFragment::NewL(_L8("nsuri"),_L8("Nokia"),_L8("n:Nokia"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseFragment_NewL_4L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    RAttributeArray aAttrs;
    CSenBaseFragment* pElement=CSenBaseFragment::NewL(_L8("nsuri"),_L8("Nokia"),_L8("n:Nokia"), aAttrs);
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseFragment_NewL_5L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    RAttributeArray aAttrs;
    CSenBaseElement* ele = CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(ele);
    CSenElement* parent = ele->Parent();
    CSenBaseFragment* pElement=CSenBaseFragment::NewL(_L8("nsuri"),_L8("Nokia"),_L8("n:Nokia"), aAttrs, *parent);
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    CleanupStack::PopAndDestroy(ele);
    Teardown();
    return KErrNone;
    }
	
TInt CSenXmlBCTest::CSenBaseFragment_AsXmlUnicodeLL(TTestResult& aResult)
	{
    SetupL();
	CSenBaseFragment* pFragment=CSenBaseFragment::NewL(_L8("Fragment"));
	CleanupStack::PushL(pFragment);
	HBufC* pFragBuf=pFragment->AsXmlUnicodeL();
/*	EUNIT_ASSERT_EQUALS(*pFragBuf,_L("<Fragment/>"));*/
	TL(*pFragBuf == _L("<Fragment/>"));
	CleanupStack::PopAndDestroy(pFragment);
	delete pFragBuf;
	Teardown();
    return KErrNone;
    }
	
TInt CSenXmlBCTest::CSenBaseFragment_WriteAsXMLToLL(TTestResult& aResult)
    {
    SetupL();
    // Test serialization of Dom tree which has two child elements
    CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("Test"));
    CleanupStack::PushL(pFragment);
    CSenElement& element = pFragment->AsElement();
    CSenElement& element2 = element.AddElementL(_L8("Child"));
    CBufFlat* pBuf = CBufFlat::NewL(200);
    CleanupStack::PushL(pBuf);
    RBufWriteStream bufWs(*pBuf);
    CleanupClosePushL(bufWs);
    pFragment->WriteAsXMLToL(bufWs);
/*    EUNIT_ASSERT( pBuf->Ptr(0) == _L8("<Test><Child/></Test>"));*/
    if(!( pBuf->Ptr(0) == _L8("<Test><Child/></Test>")))
    return KErrArgument;  

    CleanupStack::PopAndDestroy(&bufWs);
    CleanupStack::PopAndDestroy(pBuf);
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenBaseFragment_ContentL( TTestResult& aResult )
	{
	TBool Flag;
	CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("Test"));
	CleanupStack::PushL(pFragment);
	CSenElement& element = pFragment->AsElement();
    element.AddElementL(_L8("Child"));
    element.SetContentL(_L8("Test Content"));
    TPtrC8 frag_Content = pFragment->Content();
    if(frag_Content == _L8("Test Content"))
    	Flag =1;
    else
    	Flag =0;
    CleanupStack::PopAndDestroy(pFragment);
    if (Flag) return KErrNone;
    else return KErrArgument;
	}
TInt CSenXmlBCTest::CSenBaseFragment_NamespaceL( TTestResult& aResult )
	{
	TBool Flag;
	CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("nsuria"),_L8("Test"),_L8("a:Test"));
	CleanupStack::PushL(pFragment);
	CSenElement& element = pFragment->AsElement();
    element.AddElementL(_L8("Child"));
    element.SetContentL(_L8("Test Content"));
    TPtrC8 frag_NameSpace = pFragment->NsUri();
    TPtrC8 frag_prefix = pFragment->NsPrefix();
    if(frag_NameSpace == _L8("nsuria") && frag_prefix == _L8("a"))
    	Flag = 1;
    else
    	Flag = 0;
    CleanupStack::PopAndDestroy(pFragment);
    if (Flag) return KErrNone;
    else return KErrArgument;
	}
TInt CSenXmlBCTest::CSenBaseFragment_EnsureNamespaceL( TTestResult& aResult )
	{
	TBool Flag;
	CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("nsuria"),_L8("Test"),_L8("a:Test"));
	CleanupStack::PushL(pFragment);
	CSenElement& element = pFragment->AsElement();
    element.AddElementL(_L8("Child"));
    element.SetContentL(_L8("Test Content"));
    pFragment->EnsureNamespace(_L8("a"));
    CSenNamespace* frag_NameSpace;
    frag_NameSpace = pFragment->Namespace(_L8("a"));
    if(frag_NameSpace)
    	Flag = 1;
    else
    	Flag = 0;
    CleanupStack::PopAndDestroy(pFragment);
    if (Flag) return KErrNone;
    else return KErrArgument;
	}
TInt CSenXmlBCTest::CSenBaseFragment_DetachLL( TTestResult& aResult )
	{
	TBool Flag;
	_LIT8(KFragment,"<Test xmlns=\"nsuri\"><Child/>Test Content</Test>");
	CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("nsuri"),_L8("Test"),_L8("Pre"));
	CleanupStack::PushL(pFragment);
	CSenElement& element = pFragment->AsElement();
    element.AddElementL(_L8("Child"));
    element.SetContentL(_L8("Test Content"));
    CSenBaseElement *pParent=CSenBaseElement::NewL(_L8("parent"));
    element.SetParent(pParent);
    TRAPD(retval,pFragment->DetachL());
    CleanupStack::PopAndDestroy(pFragment);
    return retval;
	}	
TInt CSenXmlBCTest::CSenBaseFragment_SaveNamespacesLL( TTestResult& aResult )
	{
    TBool Flag;
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("xmlns"));
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
      
    CSenBaseFragment *pFragment = CSenBaseFragment::NewL(_L8("Nokia"));
    CleanupStack::PushL(pFragment);
    pFragment->SaveNamespacesL(attrArray,TRUE);
    TPtrC8 frag_NameSpace = pFragment->NsUri();
    if(frag_NameSpace == _L8("Value"))
    	Flag = 1;
    else
    	Flag = 0;

	CleanupStack::PopAndDestroy(pFragment);
	CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    if (Flag)	return KErrNone;
    else return KErrArgument;
    }
TInt CSenXmlBCTest::CSenBaseFragment_ResetContentLL( TTestResult& aResult )
	{
	TBool Flag;
	CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("Test"));
	CleanupStack::PushL(pFragment);
	CSenElement& element = pFragment->AsElement();
    element.AddElementL(_L8("Child"));
    element.SetContentL(_L8("Test Content"));
    TPtrC8 frag_Content = pFragment->Content();
    if(frag_Content == _L8("Test Content"))
    	Flag =1;
    else
    	Flag =0;
    pFragment->ResetContentL();
    //frag_Content = pFragment->Content();
	if(pFragment->Content() == KNullDesC8)
    	Flag =1;
    else
    	Flag =0;
    CleanupStack::PopAndDestroy(pFragment);
    if (Flag) return KErrNone;
    else return KErrArgument;
	}
TInt CSenXmlBCTest::CSenBaseFragment_LocalNameL( TTestResult& aResult )
	{
	TBool Flag;
	CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("Test"));
	CleanupStack::PushL(pFragment);
	CSenElement& element = pFragment->AsElement();
    element.AddElementL(_L8("Child"));
    element.SetContentL(_L8("Test Content"));
    TPtrC8 frag_LocalName = pFragment->LocalName();
    if(frag_LocalName == _L8("Test"))
    	Flag =1;
    else
    	Flag =0;
    CleanupStack::PopAndDestroy(pFragment);
    if (Flag) return KErrNone;
    else return KErrArgument;
	}
TInt CSenXmlBCTest::CSenBaseFragment_NsUriL( TTestResult& aResult )
	{
	TBool Flag;
	CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("nsuri"),_L8("Test"),_L8("Pre"));
	CleanupStack::PushL(pFragment);
	CSenElement& element = pFragment->AsElement();
    element.AddElementL(_L8("Child"));
    element.SetContentL(_L8("Test Content"));
    TPtrC8 frag_NsUri= pFragment->NsUri();
    if(frag_NsUri == _L8("nsuri"))
    	Flag =1;
    else
    	Flag =0;
    CleanupStack::PopAndDestroy(pFragment);
    if (Flag) return KErrNone;
    else return KErrArgument;
	}
TInt CSenXmlBCTest::CSenBaseFragment_NsPrefixL( TTestResult& aResult )
	{
	TBool Flag;
	CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("nsuri"),_L8("Test"),_L8("Pre"));
	CleanupStack::PushL(pFragment);
	CSenElement& element = pFragment->AsElement();
    element.AddElementL(_L8("Child"));
    element.SetContentL(_L8("Test Content"));
    TPtrC8 frag_NsUri= pFragment->NsUri();
    if(frag_NsUri == _L8("nsuri"))
    	Flag =1;
    else
    	Flag =0;
    CleanupStack::PopAndDestroy(pFragment);
    if (Flag) return KErrNone;
    else return KErrArgument;
	}
TInt CSenXmlBCTest::CSenBaseFragment_ExtractElementL( TTestResult& aResult )
	{
	TBool Flag;
	_LIT8(KFragment,"<Test xmlns=\"nsuri\"><Child/>Test Content</Test>");
	CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("nsuri"),_L8("Test"),_L8("Pre"));
	CleanupStack::PushL(pFragment);
	CSenElement& element = pFragment->AsElement();
    element.AddElementL(_L8("Child"));
    element.SetContentL(_L8("Test Content"));
    CSenElement* Extr_Element = pFragment->ExtractElement();
    CleanupStack::PushL(Extr_Element);
    HBufC8* Ext_ElementBuf = Extr_Element->AsXmlL();
    if(!(KFragment().Compare(*Ext_ElementBuf )))
    	Flag =1;
    else
    	Flag =0;
    
    CleanupStack::PopAndDestroy(Extr_Element);
    CleanupStack::PopAndDestroy(pFragment);
    delete Ext_ElementBuf ;

    if (Flag) return KErrNone;
    else return KErrArgument;
	}
TInt CSenXmlBCTest::CSenBaseFragment_SetAttributesLL( TTestResult& aResult )
	{
    TBool Flag;   
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);
    
    RString nsUriRString        = stringPool.OpenStringL(_L8("nsuri"));
    CleanupClosePushL(nsUriRString);
    RString nsPrefixRString     = stringPool.OpenStringL(_L8("pr"));
    CleanupClosePushL(nsPrefixRString);
    RString localNameRString    = stringPool.OpenStringL(_L8("xmlns"));
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
      
    CSenBaseFragment *pFragment = CSenBaseFragment::NewL(_L8("Nokia"));
    CleanupStack::PushL(pFragment);
    pFragment->SetAttributesL(attrArray);
    TPtrC8 frag_NameSpace = pFragment->NsUri();
    if(frag_NameSpace == _L8("Value"))
    	Flag = 1;
    else
    	Flag = 0;

	CleanupStack::PopAndDestroy(pFragment);
	CleanupStack::PopAndDestroy(&attrArray);
    CleanupStack::PopAndDestroy(&attribute);
    CleanupStack::PopAndDestroy(&stringPool);
    if (Flag)	return KErrNone;
    else return KErrArgument;
    }
TInt CSenXmlBCTest::CSenBaseFragment_ConsistsOfLL( TTestResult& aResult )
	{
	TBool Flag;
	CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("Test"));
	CleanupStack::PushL(pFragment);
	CSenElement& element = pFragment->AsElement();
    element.AddElementL(_L8("Child"));
    element.SetContentL(_L8("Test Content"));
    
    CSenBaseFragment* pFragment2 = CSenBaseFragment::NewL(_L8("Test"));
	CleanupStack::PushL(pFragment2);
	CSenElement& element2 = pFragment2->AsElement();
    element2.AddElementL(_L8("Child"));
    element2.SetContentL(_L8("Test Content"));
    
    Flag = pFragment->ConsistsOfL(*pFragment2);
    
    CleanupStack::PopAndDestroy(pFragment2);
    CleanupStack::PopAndDestroy(pFragment);
    if (Flag) return KErrNone;
    else return KErrArgument;
	}
TInt CSenXmlBCTest::CSenBaseFragment_ParseLL( TTestResult& aResult )	
	{
	SetupL();
	_LIT8(KInputString, "<?xml version=\"1.0\" encoding=\"utf-8\" ?> <a:Test character=\"&quot;\" xmlns=\"nsuri\" xmlns:a=\"nasuria\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value3\">Content1</Element1>\
<Element2 z=\"value4\">Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");

	CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("Test"));
	CleanupStack::PushL(pFragment);
	
	pFragment->SetReader(*iXmlReader);
	TRAPD(retVal,pFragment->ParseL(KInputString));
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    return retVal;
	}
	
TInt CSenXmlBCTest::CSenBaseFragment_ParseL1L( TTestResult& aResult )	
	{
	SetupL();
	_LIT8(KInputString, "<?xml version=\"1.0\" encoding=\"utf-8\" ?> <a:Test character=\"&quot;&#34;assa\" xmlns=\"nsuri\" xmlns:a=\"nasuria\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value3\">Content1</Element1>\
<Element2 z=\"value4\">Content2<Element2>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");

	CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("Test"));
	CleanupStack::PushL(pFragment);
	
	pFragment->SetReader(*iXmlReader);
	TRAPD(retVal,pFragment->ParseL(KInputString));
	CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    if (retVal == -993) return KErrNone;
    else return retVal;
	}

TInt CSenXmlBCTest::CSenBaseFragment_SetContentOfLL( TTestResult& aResult )
{
	CTestBaseFragment *pTestFragment = CTestBaseFragment::NewL(_L8("Test Fragment"));
	CleanupStack::PushL(pTestFragment);
	CSenElement& element = pTestFragment->AsElement();
	element.AddElementL(_L8("Test Child"));
	MSenElement& element1 = pTestFragment->SetContentOfL(_L8("Test Child"),_L8("Child Content"));
	TInt retVal = element1.Content().Compare(_L8("Child Content"));
	CleanupStack::PopAndDestroy(pTestFragment);
	return retVal;
}
TInt CSenXmlBCTest::CSenBaseFragment_ContentOfL( TTestResult& aResult )
{
	CTestBaseFragment *pTestFragment = CTestBaseFragment::NewL(_L8("Test Fragment"));
	CleanupStack::PushL(pTestFragment);
	pTestFragment->SetContentOfL(_L8("Test Child"),_L8("Child Content"));
	TInt retVal = pTestFragment->ContentOf(_L8("Test Child")).Compare(_L8("Child Content"));
	CleanupStack::PopAndDestroy(pTestFragment);
	return retVal;
}

TInt CSenXmlBCTest::CSenXMLReader_ParserFeatureL( TTestResult& aResult )
	{
	TBool Flag;
	SetupL();
	_LIT8(KInputString, "<?xml version=\"1.0\" encoding=\"utf-8\" ?> <a:Test character=\"&quot;\" xmlns=\"nsuri\" xmlns:a=\"nasuria\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value3\">Content1</Element1>\
<Element2 z=\"value4\">&lt;Content2&quot;&gt;</Element2>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");

	CSenBaseFragment* pFragment = CSenBaseFragment::NewL(_L8("Test"));
	CleanupStack::PushL(pFragment);
	pFragment->SetReader(*iXmlReader);
	TInt feature = iXmlReader->ParserFeature();
	TRAPD(retVal,pFragment->ParseL(KInputString));
    CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    if (retVal == KErrNone && feature  == 64) return KErrNone;
    else return KErrArgument;
	}
	
//	EXPORT_C void CSenXmlReader::ParseL(RFs &aRFs, const TDesC& aFileToParse)

TInt CSenXmlBCTest::CSenXMLReader_ParseLL( TTestResult& aResult )
	{
	SetupL();
	CSenDomFragment* pFragment = CSenDomFragment::NewL();
	CleanupStack::PushL(pFragment);
	pFragment->SetReader(*iXmlReader);
	iXmlReader->SetContentHandler(*pFragment);
	_LIT(KFile,"c:\\wf_constraint_elem.xml");
	RFs fs;
	CleanupClosePushL(fs);
	TRAPD(rFile, fs.Connect());
	TRAPD(retVal,iXmlReader->ParseL(fs,KFile));
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy(pFragment);
	Teardown();
	return retVal;
	}
TInt CSenXmlBCTest::CSenDomFragment_NewL_L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenDomFragment* pElement=CSenDomFragment::NewL();
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenDomFragment_NewL_1L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenBaseElement* ele = CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(ele);
    CSenElement* parent = ele->Parent();
    CSenDomFragment* pElement=CSenDomFragment::NewL(*ele);
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    CleanupStack::PopAndDestroy(ele);
    Teardown();
    return KErrNone;
    }
	
TInt CSenXmlBCTest::CSenDomFragment_NewL_2L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenDomFragment* pElement=CSenDomFragment::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenDomFragment_NewL_3L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenDomFragment* pElement=CSenDomFragment::NewL(_L8("nsuri"),_L8("Nokia"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenDomFragment_NewL_4L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenDomFragment* pElement=CSenDomFragment::NewL(_L8("nsuri"),_L8("Nokia"),_L8("n:Nokia"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenDomFragment_NewL_5L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    RAttributeArray aAttrs;
    CSenDomFragment* pElement=CSenDomFragment::NewL(_L8("nsuri"),_L8("Nokia"),_L8("n:Nokia"), aAttrs);
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenDomFragment_NewL_6L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    RAttributeArray aAttrs;
    CSenBaseElement* ele = CSenBaseElement::NewL(_L8("Nokia"));
    CleanupStack::PushL(ele);
    CSenElement* parent = ele->Parent();
    CSenDomFragment* pElement = CSenDomFragment::NewL(_L8("nsuri"),_L8("Nokia"),_L8("n:Nokia"), aAttrs, *parent);
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    CleanupStack::PopAndDestroy(ele);
    Teardown();
    return KErrNone;
    }
/*	
TInt CSenXmlBCTest::CSenDomFragment_ExpandLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    RAttributeArray aAttrs;
    CSenDomFragment* pElement=CSenDomFragment::NewL();
    
    CleanupStack::PushL(pElement);
	pElement->ExpandL(_L8("nsuri"),_L8("Nokia"),_L8("n:Nokia"), aAttrs);
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenDomFragment_ResumeParsingFromLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    RAttributeArray aAttrs;
    CSenDomFragment* pElement=CSenDomFragment::NewL();
    
    CleanupStack::PushL(pElement);
	pElement->ResumeParsingFromL(_L8("nsuri"),_L8("Nokia"),_L8("n:Nokia"));
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
*/    
    TInt CSenXmlBCTest::CSenDomFragment_ParseWithLL (TTestResult& aResult)
    {
    SetupL();
    CSenXmlReader* aReader = CSenXmlReader::NewL();
    CleanupStack::PushL(aReader);
    
    CSenDomFragment* pElement=CSenDomFragment::NewL();
    CleanupStack::PushL(pElement);
    pElement->ParseWithL(*aReader);
    CleanupStack::PopAndDestroy(pElement);
    CleanupStack::PopAndDestroy(aReader);
    
    Teardown();
    return KErrNone;
    }

    TInt CSenXmlBCTest::CSenDomFragment_SetAttributesLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    RAttributeArray aAttrs;
    CSenDomFragment* pElement=CSenDomFragment::NewL();
    pElement->SetAttributesL(aAttrs);
    CleanupStack::PushL(pElement);

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
TInt CSenXmlBCTest::CSenDomFragment_ParseL1L( TTestResult& aResult )	
	{
	SetupL();
	_LIT8(KInputString, "<?xml version=\"1.0\" encoding=\"utf-8\" ?> <a:Test character=\"&quot;&#34;assa\" xmlns=\"nsuri\" xmlns:a=\"nasuria\" \
xmlns:b=\"nasurib\" a:x=\"value1\" b:z=\"value2\">\
<Element1 xmlns:c=\"nsuric\" c:x=\"value3\">Content1</Element1>\
<Element2 z=\"value4\">Content2<Element2>\
<Element3>Content3</Element3>\
<Element4/></a:Test>");

	CSenDomFragment* pFragment = CSenDomFragment::NewL(_L8("Test"));
	CleanupStack::PushL(pFragment);
	
	pFragment->SetReader(*iXmlReader);
	TRAPD(retVal,pFragment->ParseL(KInputString));
	CleanupStack::PopAndDestroy(pFragment);
    Teardown();
    if (retVal == -993) return KErrNone;
    else return retVal;
	}    
/*    
TInt CSenXmlBCTest::CSenDomFragment_ConsistsOfLL()
	{
    SetupL();
    CSenDomFragment* pFragmentA=CSenDomFragment::NewL(_L8("Nokia"));
    CleanupStack::PushL(pFragmentA);
    CSenElement pElement=pFragmentA->AsElement();
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    CSenDomFragment* pChildFragment1A=CSenDomFragment::NewL(_L8("Asp"));
    CleanupStack::PushL(pChild1A);
    CSenDomFragment* pChild2A=CSenDomFragment::NewL(_L8("WebServices"));
    CleanupStack::PushL(pChild2A);
    pElementA->SetContentL(_L8("Nokia organisation structure"));
    pElementA->AsElement()
    pElementA->AddElementL(*pChild1A);
    pElementA->AddElementL(*pChild2A);
    
    CSenDomFragment* pElementB=CSenDomFragment::NewL(_L8("Nokia"));
    CleanupStack::PushL(pElementB);
    CSenDomFragment* pChild1B=CSenDomFragment::NewL(_L8("Asp"));
    CleanupStack::PushL(pChild1A);
    CSenDomFragment* pChild2B=CSenDomFragment::NewL(_L8("WebServices"));
    CleanupStack::PushL(pChild2B);
    pElementB->SetContentL(_L8("Nokia organisation structure"));
    pElementB->AddElementL(*pChild1B);
    pElementB->AddElementL(*pChild2B);
    
    TBool ConsistOfFlag=pElementB->ConsistsOfL(*pElementA);
    EUNIT_ASSERT( ConsistOfFlag );
    CleanupStack::Pop(2);
    CleanupStack::PopAndDestroy(pElementB);
    CleanupStack::Pop(2);
    CleanupStack::PopAndDestroy(pElementA);
    Teardown();
    return KErrNone;
    }
*/

TInt CSenXmlBCTest::CSenNamespace_NewL_L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenNamespace* pElement=CSenNamespace::NewL(_L8("n:"),_L8("nsuri"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenNamespace_NewLC_L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenNamespace* pElement=CSenNamespace::NewLC(_L8("n:"),_L8("nsuri"));
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
 TInt CSenXmlBCTest::CSenNamespace_NewL_1L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenNamespace* pElement=CSenNamespace::NewL(_L8("nsuri"));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
 TInt CSenXmlBCTest::CSenNamespace_NewLC_1L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenNamespace* pElement=CSenNamespace::NewLC(_L8("nsuri"));
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenNamespace_MatchesL_L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenNamespace* pElement = CSenNamespace::NewL(_L8("nsuri"));
    CSenNamespace* pEle = CSenNamespace::NewL(_L8("nsuri"));

    CleanupStack::PushL(pElement);
    CleanupStack::PushL(pEle);
    
    
    if(pElement!=NULL && pEle!=NULL)
    	Flag = pEle->MatchesL(*pElement);
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pEle);
    CleanupStack::PopAndDestroy(pElement);
    
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenNamespace_InsertLL (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    const TInt KFlatBufSize = 64;
	CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
	CleanupStack::PushL(pBuf);
    RBufWriteStream bufWs(*pBuf);
    CleanupClosePushL(bufWs); 
      
    CSenNamespace* pElement = CSenNamespace::NewL(_L8("nsuri"));
    CleanupStack::PushL(pElement);
	
    pElement->InsertL(bufWs);
    CleanupStack::PopAndDestroy(pElement);
	CleanupStack::PopAndDestroy(); // bufWs.Close();
    CleanupStack::PopAndDestroy(); // pBuf;    
    Teardown();
    return KErrNone;
    }
    
    
    
    
TInt CSenXmlBCTest::CSenXmlReader_NewL_L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlReader* pElement=CSenXmlReader::NewL();
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    delete pElement;
    pElement = NULL;
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlReader_NewLC_L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlReader* pElement=CSenXmlReader::NewLC();
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlReader_NewL_1L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    TInt param = 1;
    CSenXmlReader* pElement=CSenXmlReader::NewL(param);
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlReader_NewLC_1L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    TInt param = 1;
    CSenXmlReader* pElement=CSenXmlReader::NewLC(param);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlReader_NewL_2L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlReader* pElement=CSenXmlReader::NewL(_L8(""));
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlReader_NewLC_2L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    CSenXmlReader* pElement=CSenXmlReader::NewLC(_L8(""));
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlReader_NewL_3L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    TInt param = 1;
    CSenXmlReader* pElement=CSenXmlReader::NewL(_L8(""), param);
    CleanupStack::PushL(pElement);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlReader_NewLC_3L (TTestResult& aResult)
    {
    SetupL();
    TBool Flag;
    TInt param = 1;
    CSenXmlReader* pElement=CSenXmlReader::NewLC(_L8(""), param);
    if(pElement!=NULL)
    	Flag=1;
    else
    	Flag=0;
        /*EUNIT_ASSERT( Flag );*/
    if(!Flag)
    return KErrArgument;   

    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlReader_EnabledParserFeature_L (TTestResult& aResult)
    {
    SetupL();
    TInt param = 1;
    CSenXmlReader* pElement=CSenXmlReader::NewL(param);
    CleanupStack::PushL(pElement);
/*    EUNIT_ASSERT_EQUALS(param,pElement->EnabledParserFeature());*/
    TL(param == pElement->EnabledParserFeature());
    CleanupStack::PopAndDestroy(pElement);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlUtils_ToUtf8LCL (TTestResult& aResult)
    {
    SetupL();
    HBufC8* buf; 
    
    _LIT16(KText, "text");
    _LIT8(KText1, "text");
    buf = SenXmlUtils::ToUtf8LC(KText);
    TPtr8 ptr = buf->Des();
    TL(ptr == KText1);
    CleanupStack::PopAndDestroy();
    Teardown();
    return KErrNone;
    }
 
TInt CSenXmlBCTest::CSenXmlUtils_StartsWithL (TTestResult& aResult)
    {
    SetupL();
    HBufC8* buf; 
    TBool retVal;
    _LIT8(KText, "text");
    _LIT8(KText1, "text1");
    retVal = SenXmlUtils::StartsWith(KText, KText1);
    TL(retVal == KErrNone);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlUtils_EndsWithL (TTestResult& aResult)
    {
    SetupL();
    TBool retVal;
    _LIT8(KText, "text");
    _LIT8(KText1, "text1");
    retVal = SenXmlUtils::EndsWith(KText, KText1);
    TL(retVal == KErrNone);

    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlUtils_AttrValueL (TTestResult& aResult)
    {
    SetupL();
	RAttributeArray  array;   
    _LIT8(KText, "text");
    TPtrC8 ptr = SenXmlUtils::AttrValue(array, KText);
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlUtils_AllocAttrValueLL(TTestResult& aResult)
    {
    SetupL();
	RAttributeArray  array;   
    _LIT8(KText, "text");
    HBufC8* ptr = SenXmlUtils::AllocAttrValueL(array, KText);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlUtils_DecodeHttpCharactersLL(TTestResult& aResult)
    {
    SetupL();
    TBool retVal;
    HBufC8* ptr = NULL;
    _LIT8(KText, "text");
    retVal = SenXmlUtils::DecodeHttpCharactersL(KText, ptr);
    TL(retVal == KErrNone);

    Teardown();
    return KErrNone;
    }
    
    
TInt CSenXmlBCTest::CSenXmlUtils_DecodeHttpCharactersLCL(TTestResult& aResult)
    {
    SetupL();
    TBool retVal;
    _LIT8(KText, "text");
    HBufC8* buf = SenXmlUtils::DecodeHttpCharactersLC(KText);
    TPtr8 ptr = buf->Des();
    CleanupStack::PopAndDestroy();
    
    Teardown();
    return KErrNone;
    }
    

TInt CSenXmlBCTest::CSenXmlUtils_NsPrefixL(TTestResult& aResult)
    {
    SetupL();
    _LIT8(KText, "m:");
    _LIT8(KText1, "m");
    
    TPtrC8 ptr = SenXmlUtils::NsPrefix(KText);
    //TL(ptr == KText1);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlUtils_LocalNameL(TTestResult& aResult)
    {
    SetupL();
    _LIT8(KText, "http:\\");
    _LIT8(KText1, "\\");
    TPtrC8 ptr = SenXmlUtils::LocalName(KText);
    //TL(ptr == KText1);
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlUtils_RemoveAttributeLL(TTestResult& aResult)
    {
    SetupL();
    _LIT8(KText, "text");
    _LIT8(KText1, "text1");
	CSenBaseElement* element = CSenBaseElement::NewL(KText);
	CSenBaseAttribute* attr = CSenBaseAttribute::NewL(KText, KText1);
    attr = SenXmlUtils::RemoveAttributeL(*element, KText);
    
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlUtils_RemoveAttributeL1L(TTestResult& aResult)
    {
    SetupL();
    _LIT8(KText, "text");
    _LIT8(KText1, "text1");
	CSenBaseElement* element = CSenBaseElement::NewL(KText);
	CSenBaseAttribute* attr = CSenBaseAttribute::NewL(KText, KText1);
	CSenBaseAttribute* attr1 = CSenBaseAttribute::NewL(KText, KText1);
	
	
    attr = SenXmlUtils::RemoveAttributeL(*element, attr1);
    
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlUtils_AddAttributeLL(TTestResult& aResult)
    {
    SetupL();
    _LIT8(KText, "text");
	CSenBaseElement* element = CSenBaseElement::NewL(KText);
	
    TDesC8 desc = SenXmlUtils::AddAttributeL(*element, KText,  KText , KText );
    
    Teardown();
    return KErrNone;
    }
    
TInt CSenXmlBCTest::CSenXmlUtils_AddAttributeL1L(TTestResult& aResult)
    {
    SetupL();
    _LIT8(KText, "text");
    _LIT8(KText1, "text1");
	CSenBaseElement* element = CSenBaseElement::NewL(KText);
    TDesC8 desc = SenXmlUtils::AddAttributeL(*element, KText,  KText );
    
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenXmlUtils_AddAttributeL2L(TTestResult& aResult)
    {
    SetupL();
    _LIT8(KText, "text");
    _LIT8(KText1, "text1");
	CSenBaseElement* element = CSenBaseElement::NewL(KText);
	CSenBaseAttribute* attr = CSenBaseAttribute::NewL(KText, KText1);
	
    TDesC8 desc = SenXmlUtils::AddAttributeL(*element, attr);
    
    Teardown();
    return KErrNone;
    }
TInt CSenXmlBCTest::CSenXmlUtils_EncodeHttpCharactersLCL(TTestResult& aResult)
    {
    SetupL();
    TBool retVal;
    _LIT8(KText, "text");
    HBufC8* buf = SenXmlUtils::EncodeHttpCharactersLC(KText);
    //TPtr8 ptr = buf->Des();
    CleanupStack::PopAndDestroy();
    
    Teardown();
    return KErrNone;
    }

TInt CSenXmlBCTest::CSenDomFragment_BaseConstructL( TTestResult& aResult )
{
	    SetupL();

	CTestDomFragment *pTestFragment = CTestDomFragment::NewL(_L8("Test Fragment"));
	TInt retVal = 0;
    CSenXmlReader* aReader = CSenXmlReader::NewL();
    CleanupStack::PushL(aReader);

	CleanupStack::PushL(pTestFragment);
	pTestFragment->BaseConstructL(*aReader);
	CleanupStack::PopAndDestroy(pTestFragment);
    CleanupStack::PopAndDestroy(aReader);
	
	    Teardown();

	return retVal;
}

TInt CSenXmlBCTest::CSenBaseFragment_BaseConstructL( TTestResult& aResult )
{
	    SetupL();

	CTestBaseFragment *pTestFragment = CTestBaseFragment::NewL(_L8("Test Fragment"));
	TInt retVal = 0;
    CSenXmlReader* aReader = CSenXmlReader::NewL();
    CleanupStack::PushL(aReader);

	CleanupStack::PushL(pTestFragment);
	pTestFragment->BaseConstructL(*aReader);
	CleanupStack::PopAndDestroy(pTestFragment);
    CleanupStack::PopAndDestroy(aReader);
	
	    Teardown();

	return retVal;
}
//  END OF FILE
