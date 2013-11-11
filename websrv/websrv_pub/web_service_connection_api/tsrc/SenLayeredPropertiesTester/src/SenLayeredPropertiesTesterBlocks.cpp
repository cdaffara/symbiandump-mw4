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
//  INTERNAL INCLUDES
#include <e32debug.h>
#include <e32base.h>
#include <StifTestInterface.h>
#include "senlayeredhttptransportproperties.h"
#include "senlayeredtransportproperties.h"
#include "senlayeredxmlproperties.h"
#include "SenLayeredPropertiesTester.h"

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
// CSenLayeredPropertiesTester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CSenLayeredPropertiesTester::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CSenLayeredPropertiesTester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CSenLayeredPropertiesTester::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
		ENTRY("HTTPTPP-CloneL-test_CSenLayeredXmlProperties_CloneL", CSenLayeredPropertiesTester::UT_CSenLayeredHttpTransportProperties_CloneLL),
		ENTRY("TPP-NewL-test_CSenLayeredHttpTransportProperties_NewL", CSenLayeredPropertiesTester::UT_CSenLayeredHttpTransportProperties_NewLL),
		ENTRY("TPP-NewL-test_CSenLayeredTransportProperties_NewL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_NewLL),
		ENTRY("TPP-NewLC-test_CSenLayeredTransportProperties_NewLC", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_NewLCL),
		ENTRY("TPP-NewL-test_CSenLayeredTransportProperties_NewL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_NewL_1L),
		ENTRY("TPP-NewLC-test_CSenLayeredTransportProperties_NewLC", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_NewLC_1L),
		ENTRY("TPP-NewL-test_CSenLayeredTransportProperties_NewL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_NewL_2L),
		ENTRY("TPP-NewLC-test_CSenLayeredTransportProperties_NewLC", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_NewLC_2L),
		ENTRY("TPP-SetReader-test_CSenLayeredTransportProperties_SetReader", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetReaderL),
		ENTRY("TPP-PropertiesClassType-test_CSenLayeredTransportProperties_PropertiesClassType", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertiesClassTypeL),
		ENTRY("TPP-WriteToL-test_CSenLayeredTransportProperties_WriteToL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_WriteToLL),
		ENTRY("TPP-ReadFromL-test_CSenLayeredTransportProperties_ReadFromL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_ReadFromLL),
		ENTRY("TPP-AsUtf8L-test_CSenLayeredTransportProperties_AsUtf8L", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_AsUtf8LL),
		ENTRY("TPP-AsUtf8LC-test_CSenLayeredTransportProperties_AsUtf8LC", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_AsUtf8LCL),
		ENTRY("TPP-SetPropertyL-test_CSenLayeredTransportProperties_SetPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetPropertyLL),
		ENTRY("TPP-PropertyL-test_CSenLayeredTransportProperties_PropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertyLL),
		ENTRY("TPP-SetIntPropertyL-test_CSenLayeredTransportProperties_SetIntPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetIntPropertyLL),
		ENTRY("TPP-IntPropertyL-test_CSenLayeredTransportProperties_IntPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_IntPropertyLL),
		ENTRY("TPP-SetBoolPropertyL-test_CSenLayeredTransportProperties_SetBoolPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetBoolPropertyLL),
		ENTRY("TPP-BoolPropertyL-test_CSenLayeredTransportProperties_BoolPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_BoolPropertyLL),
		ENTRY("TPP-WriteToL-test_CSenLayeredTransportProperties_WriteToL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_WriteToL_1L),
		ENTRY("TPP-ReadFromL-test_CSenLayeredTransportProperties_ReadFromL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_ReadFromL_1L),
		ENTRY("TPP-AsUtf8L-test_CSenLayeredTransportProperties_AsUtf8L", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_AsUtf8L_1L),
		ENTRY("TPP-AsUtf8LC-test_CSenLayeredTransportProperties_AsUtf8LC", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_AsUtf8LC_1L),
		ENTRY("TPP-ShrinkTo-test_CSenLayeredTransportProperties_ShrinkTo", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_ShrinkToL),
		ENTRY("TPP-ExpandToL-test_CSenLayeredTransportProperties_ExpandToL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_ExpandToLL),
		ENTRY("TPP-TopLayer-test_CSenLayeredTransportProperties_TopLayer", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_TopLayerL),
		ENTRY("TPP-SetPropertyL-test_CSenLayeredTransportProperties_SetPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetPropertyL_1L),
		ENTRY("TPP-PropertyL-test_CSenLayeredTransportProperties_PropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertyL_1L),
		ENTRY("TPP-PropertyL-test_CSenLayeredTransportProperties_PropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertyL_2L),
		ENTRY("TPP-PropertyL-test_CSenLayeredTransportProperties_PropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertyL_3L),
		ENTRY("TPP-PropertiesByTypeL-test_CSenLayeredTransportProperties_PropertiesByTypeL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertiesByTypeLL),
		ENTRY("TPP-SetIntPropertyL-test_CSenLayeredTransportProperties_SetIntPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetIntPropertyL_1L),
		ENTRY("TPP-IntPropertyL-test_CSenLayeredTransportProperties_IntPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_IntPropertyL_1L),
		ENTRY("TPP-SetBoolPropertyL-test_CSenLayeredTransportProperties_SetBoolPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetBoolPropertyL_1L),
		ENTRY("TPP-BoolPropertyL-test_CSenLayeredTransportProperties_BoolPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_BoolPropertyL_1L),
		ENTRY("TPP-SetPropertyL-test_CSenLayeredTransportProperties_SetPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetPropertyL_2L),
		ENTRY("TPP-PropertyL-test_CSenLayeredTransportProperties_PropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertyL_4L),
		ENTRY("TPP-CloneL-test_CSenLayeredXmlProperties_CloneL", CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_CloneLL),
		ENTRY("XmlP-NewL-test_CSenLayeredXmlProperties_NewL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_NewLL),
		ENTRY("XmlP-NewLC-test_CSenLayeredXmlProperties_NewLC", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_NewLCL),
		ENTRY("XmlP-NewL1-test_CSenLayeredXmlProperties_NewL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_NewL_1L),
		ENTRY("XmlP-NewLC1-test_CSenLayeredXmlProperties_NewLC", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_NewLC_1L),
		ENTRY("XmlP-NewL2-test_CSenLayeredXmlProperties_NewL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_NewL_2L),
		ENTRY("XmlP-NewLC2-test_CSenLayeredXmlProperties_NewLC", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_NewLC_2L),
		ENTRY("XmlP-SetReader-test_CSenLayeredXmlProperties_SetReader", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetReaderL),
		ENTRY("XmlP-PropertiesClassType-test_CSenLayeredXmlProperties_PropertiesClassType", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertiesClassTypeL),
		ENTRY("XmlP-WriteToL-test_CSenLayeredXmlProperties_WriteToL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_WriteToLL),
		ENTRY("XmlP-ReadFromL-test_CSenLayeredXmlProperties_ReadFromL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_ReadFromLL),
		ENTRY("XmlP-AsUtf8L-test_CSenLayeredXmlProperties_AsUtf8L", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_AsUtf8LL),
		ENTRY("XmlP-AsUtf8LC-test_CSenLayeredXmlProperties_AsUtf8LC", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_AsUtf8LCL),
		ENTRY("XmlP-SetPropertyL-test_CSenLayeredXmlProperties_SetPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetPropertyLL),
		ENTRY("XmlP-PropertyL-test_CSenLayeredXmlProperties_PropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertyLL),
		ENTRY("XmlP-SetIntPropertyL-test_CSenLayeredXmlProperties_SetIntPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetIntPropertyLL),
		ENTRY("XmlP-IntPropertyL-test_CSenLayeredXmlProperties_IntPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_IntPropertyLL),
		ENTRY("XmlP-SetBoolPropertyL-test_CSenLayeredXmlProperties_SetBoolPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetBoolPropertyLL),
		ENTRY("XmlP-BoolPropertyL-test_CSenLayeredXmlProperties_BoolPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_BoolPropertyLL),
		ENTRY("XmlP-WriteToL-test_CSenLayeredXmlProperties_WriteToL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_WriteToL_1L),
		ENTRY("XmlP-ReadFromL-test_CSenLayeredXmlProperties_ReadFromL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_ReadFromL_1L),
		ENTRY("XmlP-AsUtf8L-test_CSenLayeredXmlProperties_AsUtf8L", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_AsUtf8L_1L),
		ENTRY("XmlP-AsUtf8LC-test_CSenLayeredXmlProperties_AsUtf8LC", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_AsUtf8LC_1L),
		ENTRY("XmlP-ShrinkTo-test_CSenLayeredXmlProperties_ShrinkTo", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_ShrinkToL),
		ENTRY("XmlP-ExpandToL-test_CSenLayeredXmlProperties_ExpandToL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_ExpandToLL),
		ENTRY("XmlP-TopLayer-test_CSenLayeredXmlProperties_TopLayer", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_TopLayerL),
		ENTRY("XmlP-SetPropertyL-test_CSenLayeredXmlProperties_SetPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetPropertyL_1L),
		ENTRY("XmlP-PropertyL-test_CSenLayeredXmlProperties_PropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertyL_1L),
		ENTRY("XmlP-PropertyL-test_CSenLayeredXmlProperties_PropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertyL_2L),
		ENTRY("XmlP-PropertyL-test_CSenLayeredXmlProperties_PropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertyL_3L),
		ENTRY("XmlP-SetPropertyL-test_CSenLayeredXmlProperties_SetPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetPropertyL_2L),
		ENTRY("XmlP-PropertyL-test_CSenLayeredXmlProperties_PropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertyL_4L),
		ENTRY("XmlP-PropertiesByTypeL-test_CSenLayeredXmlProperties_PropertiesByTypeL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertiesByTypeLL),
		ENTRY("XmlP-SetIntPropertyL-test_CSenLayeredXmlProperties_SetIntPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetIntPropertyL_1L),
		ENTRY("XmlP-IntPropertyL-test_CSenLayeredXmlProperties_IntPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_IntPropertyL_1L),
		ENTRY("XmlP-CloneL-test_CSenLayeredXmlProperties_CloneL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_CloneLL),
		ENTRY("XmlP-SetBoolPropertyL-test_CSenLayeredXmlProperties_SetBoolPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetBoolPropertyL_1L),
		ENTRY("XmlP-BoolPropertyL-test_CSenLayeredXmlProperties_BoolPropertyL", CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_BoolPropertyL_1L),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

//  METHODS

    
TInt CSenLayeredPropertiesTester::UT_CSenLayeredHttpTransportProperties_CloneLL(CStifItemParser& aItem)
    {
    _LIT8(KOutputString, "<Properties><PropertyName6>PropertyValue6</PropertyName6>\
<PropertyName5>PropertyValue5</PropertyName5>\
<PropertyName4>PropertyValue4</PropertyName4>\
<PropertyName3>PropertyValue3</PropertyName3>\
<PropertyName2>PropertyValue2</PropertyName2>\
<PropertyName1>PropertyValue1</PropertyName1></Properties>");
    
    CSenLayeredHttpTransportProperties* pProperties = CSenLayeredHttpTransportProperties::NewLC();
    
    // Create properties for all 6 leayers
    pProperties->SetPropertyL(_L8("PropertyName1"), _L8("PropertyValue1"),
                              MSenLayeredProperties::ESenTransportLayer);
    pProperties->SetPropertyL(_L8("PropertyName2"), _L8("PropertyValue2"),
                              MSenLayeredProperties::ESenSereneLayer);
    pProperties->SetPropertyL(_L8("PropertyName3"), _L8("PropertyValue3"),
                              MSenLayeredProperties::ESenFrameworkLayer);
    pProperties->SetPropertyL(_L8("PropertyName4"), _L8("PropertyValue4"),
                              MSenLayeredProperties::ESenProviderSessionLayer);
    pProperties->SetPropertyL(_L8("PropertyName5"), _L8("PropertyValue5"),
                              MSenLayeredProperties::ESenConsumerSessionLayer);
    pProperties->SetPropertyL(_L8("PropertyName6"), _L8("PropertyValue6"),
                              MSenLayeredProperties::ESenMessageLayer);
                              
    HBufC8* pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CSenLayeredHttpTransportProperties* pProperties2 = (CSenLayeredHttpTransportProperties*)pProperties->CloneL();
    // Destroy cloned properties immediately after cloning
    CleanupStack::PopAndDestroy(pProperties);
    CleanupStack::PushL(pProperties2);

    pAsXml = pProperties2->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);
    
    CleanupStack::PopAndDestroy(pProperties2);
    
    // Test Layered Properties and StringPool
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);

    pProperties = CSenLayeredHttpTransportProperties::NewLC(stringPool);
    
    // Create properties for all 6 leayers
    pProperties->SetPropertyL(_L8("PropertyName1"), _L8("PropertyValue1"),
                              MSenLayeredProperties::ESenTransportLayer);
    pProperties->SetPropertyL(_L8("PropertyName2"), _L8("PropertyValue2"),
                              MSenLayeredProperties::ESenSereneLayer);
    pProperties->SetPropertyL(_L8("PropertyName3"), _L8("PropertyValue3"),
                              MSenLayeredProperties::ESenFrameworkLayer);
    pProperties->SetPropertyL(_L8("PropertyName4"), _L8("PropertyValue4"),
                              MSenLayeredProperties::ESenProviderSessionLayer);
    pProperties->SetPropertyL(_L8("PropertyName5"), _L8("PropertyValue5"),
                              MSenLayeredProperties::ESenConsumerSessionLayer);
    pProperties->SetPropertyL(_L8("PropertyName6"), _L8("PropertyValue6"),
                              MSenLayeredProperties::ESenMessageLayer);

    TPtrC8 propertyValue;                              
    pProperties->PropertyL(_L8("PropertyName1"), propertyValue);
                              
    pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    TInt error;
    pProperties2 = (CSenLayeredHttpTransportProperties*)pProperties->Clone(error);
    LOCAL_ASSERT( error == KErrNone );
    // Destroy cloned properties immediately after cloning
    CleanupStack::PopAndDestroy(pProperties);
    CleanupStack::PushL(pProperties2);

    pAsXml = pProperties2->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pProperties2);
    
    CleanupStack::PopAndDestroy(&stringPool);
    
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredHttpTransportProperties_NewLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_NewLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_NewLCL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_NewL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_NewLC_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_NewL_2L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_NewLC_2L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetReaderL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertiesClassTypeL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_WriteToLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_ReadFromLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_AsUtf8LL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_AsUtf8LCL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetPropertyLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertyLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetIntPropertyLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_IntPropertyLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetBoolPropertyLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_BoolPropertyLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_WriteToL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_ReadFromL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_AsUtf8L_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_AsUtf8LC_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_ShrinkToL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_ExpandToLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_TopLayerL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetPropertyL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertyL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertyL_2L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertyL_3L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertiesByTypeLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetIntPropertyL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_IntPropertyL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetBoolPropertyL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_BoolPropertyL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_SetPropertyL_2L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_PropertyL_4L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }
    
TInt CSenLayeredPropertiesTester::UT_CSenLayeredTransportProperties_CloneLL(CStifItemParser& aItem)
    {
    _LIT8(KOutputString, "<Properties><PropertyName6>PropertyValue6</PropertyName6>\
<PropertyName5>PropertyValue5</PropertyName5>\
<PropertyName4>PropertyValue4</PropertyName4>\
<PropertyName3>PropertyValue3</PropertyName3>\
<PropertyName2>PropertyValue2</PropertyName2>\
<PropertyName1>PropertyValue1</PropertyName1></Properties>"); 
    
    CSenLayeredTransportProperties* pProperties = CSenLayeredTransportProperties::NewLC();
    
    // Create properties for all 6 leayers
    pProperties->SetPropertyL(_L8("PropertyName1"), _L8("PropertyValue1"),
                              MSenLayeredProperties::ESenTransportLayer);
    pProperties->SetPropertyL(_L8("PropertyName2"), _L8("PropertyValue2"),
                              MSenLayeredProperties::ESenSereneLayer);
    pProperties->SetPropertyL(_L8("PropertyName3"), _L8("PropertyValue3"),
                              MSenLayeredProperties::ESenFrameworkLayer);
    pProperties->SetPropertyL(_L8("PropertyName4"), _L8("PropertyValue4"),
                              MSenLayeredProperties::ESenProviderSessionLayer);
    pProperties->SetPropertyL(_L8("PropertyName5"), _L8("PropertyValue5"),
                              MSenLayeredProperties::ESenConsumerSessionLayer);
    pProperties->SetPropertyL(_L8("PropertyName6"), _L8("PropertyValue6"),
                              MSenLayeredProperties::ESenMessageLayer);
                              
    HBufC8* pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CSenLayeredTransportProperties* pProperties2 = (CSenLayeredTransportProperties*)pProperties->CloneL();
    // Destroy cloned properties immediately after cloning
    CleanupStack::PopAndDestroy(pProperties);
    CleanupStack::PushL(pProperties2);

    pAsXml = pProperties2->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);
    
    CleanupStack::PopAndDestroy(pProperties2);
    
    // Test Layered Properties and StringPool
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);

    pProperties = CSenLayeredTransportProperties::NewLC(stringPool);
    
    // Create properties for all 6 leayers
    pProperties->SetPropertyL(_L8("PropertyName1"), _L8("PropertyValue1"),
                              MSenLayeredProperties::ESenTransportLayer);
    pProperties->SetPropertyL(_L8("PropertyName2"), _L8("PropertyValue2"),
                              MSenLayeredProperties::ESenSereneLayer);
    pProperties->SetPropertyL(_L8("PropertyName3"), _L8("PropertyValue3"),
                              MSenLayeredProperties::ESenFrameworkLayer);
    pProperties->SetPropertyL(_L8("PropertyName4"), _L8("PropertyValue4"),
                              MSenLayeredProperties::ESenProviderSessionLayer);
    pProperties->SetPropertyL(_L8("PropertyName5"), _L8("PropertyValue5"),
                              MSenLayeredProperties::ESenConsumerSessionLayer);
    pProperties->SetPropertyL(_L8("PropertyName6"), _L8("PropertyValue6"),
                              MSenLayeredProperties::ESenMessageLayer);

    TPtrC8 propertyValue;                              
    pProperties->PropertyL(_L8("PropertyName1"), propertyValue);
                              
    pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    pProperties2 = (CSenLayeredTransportProperties*)pProperties->CloneL();
    // Destroy cloned properties immediately after cloning
    CleanupStack::PopAndDestroy(pProperties);
    CleanupStack::PushL(pProperties2);

    pAsXml = pProperties2->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pProperties2);
    
    CleanupStack::PopAndDestroy(&stringPool);
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_NewLL(CStifItemParser& aItem)
    {
    _LIT8(KInputString, "<Properties>\
<Element1>öKissa käveli öisellä kadulla</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></Properties>");

    _LIT8(KOutputString, "<Properties>\
<Element1>öKissa käveli öisellä kadulla</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></Properties>");

    _LIT8(KInputString1, "<Properties>\
<Element1>äveli öisellä kadulla</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></Properties>");

    _LIT8(KOutputString1, "<Properties>\
<Element1>äveli öisellä kadulla</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></Properties>");

	CSenLayeredXmlProperties* pProperties = CSenLayeredXmlProperties::NewL();
	CleanupStack::PushL(pProperties);
	
	pProperties->ReadFromL(KInputString);
	
	HBufC8* pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );

    
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pProperties);

    // Test CSenLayeredXmlProperties when RStringPool is used
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);

	pProperties = CSenLayeredXmlProperties::NewL(stringPool);
	CleanupStack::PushL(pProperties);
	
	pProperties->ReadFromL(KInputString);
	
	pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pProperties);

    CleanupStack::PopAndDestroy(&stringPool);
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_NewLCL(CStifItemParser& aItem)
    {
    _LIT8(KInputString, "<Properties>\
<Element1>öKissa käveli öisellä kadulla</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></Properties>");

    _LIT8(KOutputString, "<Properties>\
<Element1>öKissa käveli öisellä kadulla</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></Properties>");

	CSenLayeredXmlProperties* pProperties = CSenLayeredXmlProperties::NewLC();
	
	pProperties->ReadFromL(KInputString);
	
	HBufC8* pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pProperties);


    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);

	pProperties = CSenLayeredXmlProperties::NewLC(stringPool);
	
	pProperties->ReadFromL(KInputString);
	
	pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pProperties);

    CleanupStack::PopAndDestroy(&stringPool);
    
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_NewL_1L(CStifItemParser& aItem)
    {
    _LIT8(KInputString, "<Properties>\
<Element1>Kissa käveli öisellä kadulla</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></Properties>");

    _LIT8(KOutputString, "<Properties>\
<Element1>Kissa käveli öisellä kadulla</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></Properties>");

    CSenXmlReader* pReader = CSenXmlReader::NewLC();

	CSenLayeredXmlProperties* pProperties = CSenLayeredXmlProperties::NewL(KInputString, *pReader);
	CleanupStack::PushL(pProperties);
	
	HBufC8* pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pProperties);

    // Test CSenLayeredXmlProperties when RStringPool is used
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);

	pProperties = CSenLayeredXmlProperties::NewL(KInputString, *pReader, stringPool);
	CleanupStack::PushL(pProperties);
	
	pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pProperties);
    CleanupStack::PopAndDestroy(&stringPool);
    CleanupStack::PopAndDestroy(pReader);
    
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_NewLC_1L(CStifItemParser& aItem)
    {
    _LIT8(KInputString, "<Properties>\
<Element1>Kissa käveli öisellä kadulla</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></Properties>");

    _LIT8(KOutputString, "<Properties>\
<Element1>Kissa käveli öisellä kadulla</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></Properties>");

    CSenXmlReader* pReader = CSenXmlReader::NewLC();

	CSenLayeredXmlProperties* pProperties = CSenLayeredXmlProperties::NewLC(KInputString, *pReader);
	
	HBufC8* pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pProperties);

    // Test CSenLayeredXmlProperties when RStringPool is used
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);

	pProperties = CSenLayeredXmlProperties::NewLC(KInputString, *pReader, stringPool);
	
	pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pProperties);
    CleanupStack::PopAndDestroy(&stringPool);
    CleanupStack::PopAndDestroy(pReader);
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_NewL_2L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_NewLC_2L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetReaderL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertiesClassTypeL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_WriteToLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_ReadFromLL(CStifItemParser& aItem)
    {
    _LIT8(KInputString, "<Properties>\
<Element1>öKissa käveli öisellä kadulla</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></Properties>");

    _LIT8(KOutputString, "<Properties>\
<Element1>öKissa käveli öisellä kadulla</Element1>\
<Element2>Content2</Element2>\
<Element3>Content3</Element3>\
<Element4/></Properties>");

	CSenLayeredXmlProperties* pProperties = CSenLayeredXmlProperties::NewLC();
	
	pProperties->ReadFromL(KInputString);
	
	HBufC8* pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pProperties);


    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);

	pProperties = CSenLayeredXmlProperties::NewLC(stringPool);
	
	pProperties->ReadFromL(KInputString);
	
	pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pProperties);

    CleanupStack::PopAndDestroy(&stringPool);
    
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_AsUtf8LL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_AsUtf8LCL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetPropertyLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertyLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetIntPropertyLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_IntPropertyLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetBoolPropertyLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_BoolPropertyLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_WriteToL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_ReadFromL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_AsUtf8L_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_AsUtf8LC_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_ShrinkToL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_ExpandToLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_TopLayerL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetPropertyL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertyL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertyL_2L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertyL_3L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetPropertyL_2L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertyL_4L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_PropertiesByTypeLL(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetIntPropertyL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_IntPropertyL_1L(CStifItemParser& aItem)
    {
    LOCAL_ASSERT( ETrue );
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_CloneLL(CStifItemParser& aItem)
    {
    _LIT8(KOutputString, "<Properties><PropertyName6>PropertyValue6</PropertyName6>\
<PropertyName5>PropertyValue5</PropertyName5>\
<PropertyName4>PropertyValue4</PropertyName4>\
<PropertyName3>PropertyValue3</PropertyName3>\
<PropertyName2>PropertyValue2</PropertyName2>\
<PropertyName1>PropertyValue1</PropertyName1></Properties>"); 
    
    CSenLayeredXmlProperties* pProperties = CSenLayeredXmlProperties::NewLC();
    
    // Create properties for all 6 leayers
    pProperties->SetPropertyL(_L8("PropertyName1"), _L8("PropertyValue1"),
                              MSenLayeredProperties::ESenTransportLayer);
    pProperties->SetPropertyL(_L8("PropertyName2"), _L8("PropertyValue2"),
                              MSenLayeredProperties::ESenSereneLayer);
    pProperties->SetPropertyL(_L8("PropertyName3"), _L8("PropertyValue3"),
                              MSenLayeredProperties::ESenFrameworkLayer);
    pProperties->SetPropertyL(_L8("PropertyName4"), _L8("PropertyValue4"),
                              MSenLayeredProperties::ESenProviderSessionLayer);
    pProperties->SetPropertyL(_L8("PropertyName5"), _L8("PropertyValue5"),
                              MSenLayeredProperties::ESenConsumerSessionLayer);
    pProperties->SetPropertyL(_L8("PropertyName6"), _L8("PropertyValue6"),
                              MSenLayeredProperties::ESenMessageLayer);
                              
    HBufC8* pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CSenLayeredXmlProperties* pProperties2 = (CSenLayeredXmlProperties*)pProperties->CloneL();
    // Destroy cloned properties immediately after cloning
    CleanupStack::PopAndDestroy(pProperties);
    CleanupStack::PushL(pProperties2);

    pAsXml = pProperties2->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);
    
    CleanupStack::PopAndDestroy(pProperties2);
    
    // Test Layered Properties and StringPool
    RStringPool stringPool;
    stringPool.OpenL();
    CleanupClosePushL(stringPool);

    pProperties = CSenLayeredXmlProperties::NewLC(stringPool);
    
    // Create properties for all 6 leayers
    pProperties->SetPropertyL(_L8("PropertyName1"), _L8("PropertyValue1"),
                              MSenLayeredProperties::ESenTransportLayer);
    pProperties->SetPropertyL(_L8("PropertyName2"), _L8("PropertyValue2"),
                              MSenLayeredProperties::ESenSereneLayer);
    pProperties->SetPropertyL(_L8("PropertyName3"), _L8("PropertyValue3"),
                              MSenLayeredProperties::ESenFrameworkLayer);
    pProperties->SetPropertyL(_L8("PropertyName4"), _L8("PropertyValue4"),
                              MSenLayeredProperties::ESenProviderSessionLayer);
    pProperties->SetPropertyL(_L8("PropertyName5"), _L8("PropertyValue5"),
                              MSenLayeredProperties::ESenConsumerSessionLayer);
    pProperties->SetPropertyL(_L8("PropertyName6"), _L8("PropertyValue6"),
                              MSenLayeredProperties::ESenMessageLayer);

    TPtrC8 propertyValue;                              
    pProperties->PropertyL(_L8("PropertyName1"), propertyValue);
                              
    pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    pProperties2 = (CSenLayeredXmlProperties*)pProperties->CloneL();
    // Destroy cloned properties immediately after cloning
    CleanupStack::PopAndDestroy(pProperties);
    CleanupStack::PushL(pProperties2);

    pAsXml = pProperties2->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == KOutputString );
    CleanupStack::PopAndDestroy(pAsXml);

    CleanupStack::PopAndDestroy(pProperties2);
    
    CleanupStack::PopAndDestroy(&stringPool);
    return KErrNone;
    }

TInt CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetBoolPropertyL_1L(CStifItemParser& aItem)
    {
    TBool value;
    CSenLayeredXmlProperties* pProperties = CSenLayeredXmlProperties::NewLC();
    
    pProperties->SetBoolPropertyL(_L8("PropertyName"), EFalse);
    pProperties->BoolPropertyL(_L8("PropertyName"), value);
    LOCAL_ASSERT( value == EFalse );

    HBufC8* pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == _L8("<Properties><PropertyName>false</PropertyName></Properties>") );
    CleanupStack::PopAndDestroy(pAsXml);

    pProperties->SetBoolPropertyL(_L8("PropertyName"), ETrue);
    pProperties->BoolPropertyL(_L8("PropertyName"), value);
    LOCAL_ASSERT( value != EFalse );
    
    pAsXml = pProperties->AsUtf8LC();
    LOCAL_ASSERT( *pAsXml == _L8("<Properties><PropertyName>true</PropertyName></Properties>") );
    CleanupStack::PopAndDestroy(pAsXml);
    
    CleanupStack::PopAndDestroy(pProperties);
    
    return KErrNone;
    }

void CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_BoolPropertyL_1L(CStifItemParser& aItem)
    {
    CSenLayeredPropertiesTester::UT_CSenLayeredXmlProperties_SetBoolPropertyL_1L(aItem);    
    }

// -----------------------------------------------------------------------------
// CSenLayeredPropertiesTester::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CSenLayeredPropertiesTester::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
