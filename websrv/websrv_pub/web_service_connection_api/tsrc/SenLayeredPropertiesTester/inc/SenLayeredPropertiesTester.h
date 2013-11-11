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
* Description: Header declaration
*
*/









#ifndef SENLAYEREDPROPERTIESTESTER_H
#define SENLAYEREDPROPERTIESTESTER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KSenLayeredPropertiesTesterLogPath, "\\logs\\testframework\\SenLayeredPropertiesTester\\" ); 
// Log file
_LIT( KSenLayeredPropertiesTesterLogFile, "SenLayeredPropertiesTester.txt" ); 
_LIT( KSenLayeredPropertiesTesterLogFileWithTitle, "SenLayeredPropertiesTester_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
class CSenLayeredHttpTransportProperties;
class CSenLayeredXmlProperties;
class CSenLayeredTransportProperties;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif
//class ?FORWARD_CLASSNAME;
class CSenLayeredPropertiesTester;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CSenLayeredPropertiesTester test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CSenLayeredPropertiesTester) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSenLayeredPropertiesTester* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CSenLayeredPropertiesTester();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CSenLayeredPropertiesTester( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */


         TInt UT_CSenLayeredHttpTransportProperties_CloneLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredHttpTransportProperties_NewLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_NewLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_NewLCL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_NewL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_NewLC_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_NewL_2L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_NewLC_2L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_SetReaderL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_PropertiesClassTypeL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_WriteToLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_ReadFromLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_AsUtf8LL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_AsUtf8LCL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_SetPropertyLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_PropertyLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_SetIntPropertyLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_IntPropertyLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_SetBoolPropertyLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_BoolPropertyLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_WriteToL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_ReadFromL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_AsUtf8L_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_AsUtf8LC_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_ShrinkToL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_ExpandToLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_TopLayerL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_SetPropertyL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_PropertyL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_PropertyL_2L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_PropertyL_3L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_PropertiesByTypeLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_SetIntPropertyL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_IntPropertyL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_SetBoolPropertyL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_BoolPropertyL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_SetPropertyL_2L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_PropertyL_4L(CStifItemParser& aItem);


         TInt UT_CSenLayeredTransportProperties_CloneLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_NewLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_NewLCL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_NewL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_NewLC_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_NewL_2L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_NewLC_2L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_SetReaderL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_PropertiesClassTypeL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_WriteToLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_ReadFromLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_AsUtf8LL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_AsUtf8LCL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_SetPropertyLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_PropertyLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_SetIntPropertyLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_IntPropertyLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_SetBoolPropertyLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_BoolPropertyLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_WriteToL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_ReadFromL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_AsUtf8L_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_AsUtf8LC_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_ShrinkToL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_ExpandToLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_TopLayerL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_SetPropertyL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_PropertyL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_PropertyL_2L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_PropertyL_3L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_SetPropertyL_2L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_PropertyL_4L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_PropertiesByTypeLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_SetIntPropertyL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_IntPropertyL_1L(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_CloneLL(CStifItemParser& aItem);


         TInt UT_CSenLayeredXmlProperties_SetBoolPropertyL_1L(CStifItemParser& aItem);


         void UT_CSenLayeredXmlProperties_BoolPropertyL_1L(CStifItemParser& aItem);
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    };

#endif      // SENLAYEREDPROPERTIESTESTER_H

// End of File
