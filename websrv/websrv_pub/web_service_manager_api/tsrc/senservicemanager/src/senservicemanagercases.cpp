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
* Description:        ?Description
*
*/









// INCLUDE FILES
#include <e32math.h>
#include "senservicemanagerbctest.h"

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


//Macro for Porting to STIF
#define LOCAL_ASSERT(expression)	{if(!(expression)){return KErrArgument;}}


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSenServiceManagerBCTest::Case
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
const TCaseInfo CSenServiceManagerBCTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    SenServiceManager_stif.cpp file and to SenServiceManager_stif.h 
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
        // To add new test cases, add new items to this array
        
        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g. 
        // CSenServiceManagerBCTest::PrintTest. Otherwise the compiler
        // gives errors.
        
		ENTRY("NewL - test ", CSenServiceManagerBCTest::UT_CSenServiceManager_NewLL),
		ENTRY("NewLC - test ", CSenServiceManagerBCTest::UT_CSenServiceManager_NewLCL),
		ENTRY("normal - ServiceDescriptionsL", CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsLL_normalL),
		ENTRY("notFound - ServiceDescriptionsL", CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsLL_notFoundL),
		ENTRY("badDescriptor - ServiceDescriptionsL", CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsLL_badDescriptorL),
		ENTRY("NULL - ServiceDescriptionsL", CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsLL_nullL),
		ENTRY("randomPtr - ServiceDescriptionsL", CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsLL_randomPtrL),
		ENTRY("normal(URI) - ServiceDescriptionsL", CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsL_1L_normalL),
		ENTRY("notFound(URI) - ServiceDescriptionsL", CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsL_1L_notFoundL),
		ENTRY("badDescriptor(URI) - ServiceDescriptionsL", CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsL_1L_badDescriptorL),
		ENTRY("XMLPattern(URI) - ServiceDescriptionsL", CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsL_1L_XMLpatternL),
		ENTRY("normal - RegisterServiceDescriptionL", CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_normalL),
		ENTRY("badDescriptor - RegisterServiceDescriptionL", CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_badDescriptorL),
		ENTRY("no Endpoint - RegisterServiceDescriptionL ", CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_noEndpointL),
		ENTRY("no Contract - RegisterServiceDescriptionL", CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_noContractL),
		ENTRY("bad Framework - RegisterServiceDescriptionL", CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_badFrameworkL),
		ENTRY("random Ptr - RegisterServiceDescriptionL", CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_randomPtrL),
		ENTRY("NULL - RegisterServiceDescriptionL", CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_nullL),
		ENTRY("normal - UnregisterServiceDescriptionL", CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_normalL),
		ENTRY("badDescriptor - UnregisterServiceDescriptionL", CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_badDescriptorL),
		ENTRY("noEndpoint - UnregisterServiceDescriptionL", CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_noEndpointL),
		ENTRY("noContract - UnregisterServiceDescriptionL", CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_noContractL),
		ENTRY("badFramework - UnregisterServiceDescriptionL", CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_badFrameworkL),
		ENTRY("randomPtr - UnregisterServiceDescriptionL", CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_randomPtrL),
		ENTRY("NULL - UnregisterServiceDescriptionL", CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_nullL),
		ENTRY("normal - RegisterIdentityProviderL", CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterIdentityProviderLL_normalL),
		ENTRY("badDescriptor - RegisterIdentityProviderL", CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterIdentityProviderLL_badDescriptorL),
		ENTRY("providerIDInUse - RegisterIdentityProviderL", CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterIdentityProviderLL_providerIDInUseL),
		ENTRY("NULL - RegisterIdentityProviderL", CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterIdentityProviderLL_nullL),
		ENTRY("randomPtr - RegisterIdentityProviderL", CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterIdentityProviderLL_randomPtrL),
		ENTRY("normal - UnregisterIdentityProviderL", CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterIdentityProviderLL_normalL),
		ENTRY("badDescriptor - UnregisterIdentityProviderL", CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterIdentityProviderLL_badDescriptorL),
		ENTRY("notFound - UnregisterIdentityProviderL", CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterIdentityProviderLL_notFoundL),
		ENTRY("NULL - UnregisterIdentityProviderL", CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterIdentityProviderLL_nullL),
		ENTRY("randomPtr - UnregisterIdentityProviderL", CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterIdentityProviderLL_randomPtrL),
		ENTRY("normal - AssociateServiceL", CSenServiceManagerBCTest::UT_CSenServiceManager_AssociateServiceLL_normalL),
		ENTRY("argument - AssociateServiceL", CSenServiceManagerBCTest::UT_CSenServiceManager_AssociateServiceLL_argumentL),
		ENTRY("notReady - AssociateServiceL", CSenServiceManagerBCTest::UT_CSenServiceManager_AssociateServiceLL_notReadyL),
		ENTRY("notFound - AssociateServiceL", CSenServiceManagerBCTest::UT_CSenServiceManager_AssociateServiceLL_notFoundL),
		ENTRY("normal - DissociateServiceL", CSenServiceManagerBCTest::UT_CSenServiceManager_DissociateServiceLL_normalL),
		ENTRY("argument - DissociateServiceL", CSenServiceManagerBCTest::UT_CSenServiceManager_DissociateServiceLL_argumentL),
		ENTRY("notReady - DissociateServiceL", CSenServiceManagerBCTest::UT_CSenServiceManager_DissociateServiceLL_notReadyL),
		ENTRY("notFound - DissociateServiceL", CSenServiceManagerBCTest::UT_CSenServiceManager_DissociateServiceLL_notFoundL),

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

void CSenServiceManagerBCTest::Empty(  )
{
}
void CSenServiceManagerBCTest::DeleteDBL(){
    TPtrC file;
    TInt fileErr = KErrNone;
    
    CFileMan* fileMan = CFileMan::NewL(iFsSession);
    CleanupStack::PushL(fileMan);
    
    fileErr = fileMan->Delete(KSessionsFile);
    RDebug::Print( KSessionsFile);
    RDebug::Print( _L( "WSModTester: DeleteFileL status %d" ), fileErr );

	if(!(fileErr == KErrNone || fileErr == KErrNotFound))
	{
		User::LeaveIfError(fileErr);
	}

    fileErr = fileMan->Delete(KIdentitiesFile);
    RDebug::Print( _L( "WSModTester: DeleteFileL status %d" ), fileErr );
	if(!(fileErr == KErrNone || fileErr == KErrNotFound))
	{
		User::LeaveIfError(fileErr);
	}

    CleanupStack::PopAndDestroy();
}

void CSenServiceManagerBCTest::SetupL(  )
    {
    User::LeaveIfError(this->iFsSession.Connect());

    DeleteDBL();
	
    if ( iServiceManager!= NULL ){
        delete iServiceManager;
	    iServiceManager = NULL;
    }
    iServiceManager = CSenServiceManager::NewL();
    User::LeaveIfNull(iServiceManager);

    if ( iSenXmlServiceDescription!= NULL ){
        delete iSenXmlServiceDescription;
	    iSenXmlServiceDescription = NULL;
    }
    iSenXmlServiceDescription = CSenXmlServiceDescription::NewL();
    User::LeaveIfNull(iSenXmlServiceDescription);
    
    if ( iProvider != NULL ){
        delete iProvider;
	    iProvider = NULL;
    }
    iProvider = CSenIdentityProvider::NewL(KText, KText);
    User::LeaveIfNull(iProvider);
    
    } 

void CSenServiceManagerBCTest::Teardown(  )
    {
    if ( iSenXmlServiceDescription!= NULL ){
        delete iSenXmlServiceDescription;
	    iSenXmlServiceDescription = NULL;
    }
    if ( iServiceManager!= NULL ){
        delete iServiceManager;
	    iServiceManager = NULL;
    }
    if ( iProvider!= NULL ){
        delete iProvider;
	    iProvider = NULL;
    }
    iFsSession.Close();
    }
    
//-------------------------------------------
//              METHODS
//-------------------------------------------

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_NewLL( TTestResult& aResult )
    {
    Empty();
    
    CSenServiceManager*     serviceManager = NULL;
    CSenServiceManager*     serviceManager2 = NULL;
    serviceManager = CSenServiceManager::NewL();
    TL(serviceManager != (CSenServiceManager*)NULL);

    __ASSERT_ALWAYS_NO_LEAVE(delete serviceManager );
    
    serviceManager = NULL;

    serviceManager = CSenServiceManager::NewL();
    TRAPD(error, serviceManager2 = CSenServiceManager::NewL());
    RDebug::Print( _L( "WSModTester: CSenServiceManager_NewL 2nd try with status %d" ), error);
    LOCAL_ASSERT(error==KErrNone);

    if (serviceManager != NULL){
        __ASSERT_ALWAYS_NO_LEAVE(delete serviceManager );
        serviceManager = NULL;
    }

    if (serviceManager2 != NULL){
        __ASSERT_ALWAYS_NO_LEAVE(delete serviceManager2 );
        serviceManager2 = NULL;
    }

    Empty();
    return KErrNone;
    }

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_NewLCL( TTestResult& aResult )
    {
    Empty();
    
    CSenServiceManager*     serviceManager = NULL;
    serviceManager = CSenServiceManager::NewLC();
    TL(serviceManager != (CSenServiceManager*)NULL);
    CleanupStack::PopAndDestroy();
    serviceManager = NULL;
    Empty();
	return KErrNone;
    }

//-------------------------------------------
//              SEARCHING
//-------------------------------------------
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsLL_normalL(TTestResult& aResult)
    {
	SetupL(); 
	   
    iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    iSenXmlServiceDescription->SetEndPointL(KText);
    iSenXmlServiceDescription->SetContractL(KText);
    LOCAL_ASSERT((iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNone);
 
    RServiceDescriptionArray arr;
    LOCAL_ASSERT(iServiceManager->ServiceDescriptionsL(*iSenXmlServiceDescription, arr) == KErrNone);
  	TInt SDs = arr.Count();
    LOCAL_ASSERT(SDs == 1);
    arr.ResetAndDestroy();
    
    //Added Vaibhav
    iServiceManager->UnregisterServiceDescriptionL( *iSenXmlServiceDescription);
    
    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsLL_notFoundL(TTestResult& aResult)
    {
	SetupL(); 

    iSenXmlServiceDescription->SetEndPointL(KText);
    RServiceDescriptionArray arr;
    LOCAL_ASSERT(iServiceManager->ServiceDescriptionsL(*iSenXmlServiceDescription, arr) == KErrNotFound);
  	TInt SDs = arr.Count();
    LOCAL_ASSERT(SDs == 0);
    arr.ResetAndDestroy();

    Teardown();
    return KErrNone;
    }

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsLL_badDescriptorL(TTestResult& aResult)
    {
	SetupL(); 

    iSenXmlServiceDescription->SetEndPointL(KNullDesC8);
    RServiceDescriptionArray arr;
  	TInt SDs = arr.Count();
    LOCAL_ASSERT(SDs == 0);
    arr.ResetAndDestroy();

    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsLL_nullL(TTestResult& aResult)
    {
	SetupL(); 

    delete iSenXmlServiceDescription;
    iSenXmlServiceDescription = NULL;
    RServiceDescriptionArray arr;
	
	#ifdef __WINS__
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcAccessViolation) ;
	#else
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcPageFault) ;
	#endif     
	iServiceManager->ServiceDescriptionsL(*iSenXmlServiceDescription, arr);
	
  	TInt SDs = arr.Count();
    LOCAL_ASSERT(SDs == 0);
    arr.ResetAndDestroy();

    Teardown();
    return KErrNone;
    }


TInt CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsLL_randomPtrL(TTestResult& aResult)
    {
	SetupL(); 

    delete iSenXmlServiceDescription;
    iSenXmlServiceDescription = (CSenXmlServiceDescription *)0xeaeaea;
    RServiceDescriptionArray arr;

	#ifdef __WINS__
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcAccessViolation) ;
	#else
		TestModuleIf().SetExitReason (TestModuleIf(). EException,EExcPageFault);
	#endif      
	iServiceManager->ServiceDescriptionsL(*iSenXmlServiceDescription, arr);
    
    TInt SDs = arr.Count();
    LOCAL_ASSERT(SDs == 0);
    arr.ResetAndDestroy();

    Teardown();
    return KErrNone;
    }


TInt CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsL_1L_normalL(TTestResult& aResult)
    {
    
	SetupL(); 

    iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    iSenXmlServiceDescription->SetEndPointL(KText);
    iSenXmlServiceDescription->SetContractL(KText);
    LOCAL_ASSERT((iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNone);
 
    RServiceDescriptionArray arr;
    LOCAL_ASSERT(iServiceManager->ServiceDescriptionsL(KText, arr) == KErrNone);
  	TInt SDs = arr.Count();
    LOCAL_ASSERT(SDs == 1);
    
    arr.ResetAndDestroy();

	//Added Vaibhav
	iServiceManager->UnregisterServiceDescriptionL( *iSenXmlServiceDescription);

    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsL_1L_notFoundL(TTestResult& aResult)
{
	SetupL(); 

    RServiceDescriptionArray arr;
    LOCAL_ASSERT(iServiceManager->ServiceDescriptionsL(KText, arr) == KErrNotFound);
  	TInt SDs = arr.Count();
    LOCAL_ASSERT(SDs == 0);
    arr.ResetAndDestroy();

    Teardown();
    return KErrNone;
}
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsL_1L_badDescriptorL(TTestResult& aResult)
{
	SetupL(); 

    RServiceDescriptionArray arr;
    //LOCAL_ASSERT(iServiceManager->ServiceDescriptionsL(KNullDesC8, arr) == KErrBadDescriptor);
  	TInt SDs = arr.Count();
    LOCAL_ASSERT(SDs == 0);
    arr.ResetAndDestroy();

    Teardown();
    return KErrNone;
}
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_ServiceDescriptionsL_1L_XMLpatternL(TTestResult& aResult)
{
	SetupL(); 

    RServiceDescriptionArray arr;
    _LIT8(KXmlPattern, "&<>");
    LOCAL_ASSERT(iServiceManager->ServiceDescriptionsL(KXmlPattern, arr) == KErrNotFound);
  	TInt SDs = arr.Count();
    LOCAL_ASSERT(SDs == 0);
    arr.ResetAndDestroy();

    Teardown();
    return KErrNone;
}
//-------------------------------------------
//              REGISTERING SERVICE
//-------------------------------------------

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_normalL(TTestResult& aResult){
	SetupL(); 

    iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    iSenXmlServiceDescription->SetEndPointL(KText);
    iSenXmlServiceDescription->SetContractL(KText);
    LOCAL_ASSERT((iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNone);
 
 	//Added Vaibhav
	iServiceManager->UnregisterServiceDescriptionL( *iSenXmlServiceDescription);
	
    Teardown();
    return KErrNone;
}
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_badDescriptorL(TTestResult& aResult){
	SetupL(); 

    Teardown();
    return KErrNone;
}
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_noEndpointL(TTestResult& aResult)
    {
	SetupL(); 

   TInt error = KErrNone;
    
    //not set
    iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    iSenXmlServiceDescription->SetContractL(KText);
    error = iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription);
    RDebug::Print( _L( "WSModTester: CSenServiceManager_RegisterServiceDescriptionLL_noEndpoint status %d" ), error);

    //null
    iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    iSenXmlServiceDescription->SetContractL(KText);
    iSenXmlServiceDescription->SetEndPointL(KNullDesC8);
    error = iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription);
    RDebug::Print( _L( "WSModTester: CSenServiceManager_RegisterServiceDescriptionLL_noEndpoint status %d" ), error);

    Teardown();
    return KErrNone;
    
}
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_noContractL(TTestResult& aResult)
    {
	SetupL(); 

    TInt error = KErrNone;
    
    iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
	error = iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription);
    RDebug::Print( _L( "WSModTester: CSenServiceManager_RegisterServiceDescriptionLL_noContract status %d" ), error);
    
    //not set
    iSenXmlServiceDescription->SetEndPointL(KText);
    error = iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription);
    RDebug::Print( _L( "WSModTester: CSenServiceManager_RegisterServiceDescriptionLL_noContract status %d" ), error);
    LOCAL_ASSERT(error == KErrSenNoContract);
    
    //null
    iSenXmlServiceDescription->SetContractL(KNullDesC8);
    error = iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription);
    RDebug::Print( _L( "WSModTester: CSenServiceManager_RegisterServiceDescriptionLL_noContract status %d" ), error);
    LOCAL_ASSERT(error == KErrSenNoContract);

    Teardown();
    return KErrNone;
    
}
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_badFrameworkL(TTestResult& aResult)
    {
	SetupL(); 

    _LIT8(KBadFramework,"bla");
    iSenXmlServiceDescription->SetFrameworkIdL(KBadFramework);
    iSenXmlServiceDescription->SetEndPointL(KText);
    iSenXmlServiceDescription->SetContractL(KText);
    LOCAL_ASSERT((iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNotFound);

    Teardown();
    return KErrNone;
}

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_randomPtrL(TTestResult& aResult){
	SetupL(); 

    delete iSenXmlServiceDescription;
    iSenXmlServiceDescription = (CSenXmlServiceDescription *)0xeaeaea;
   	#ifdef __WINS__
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcAccessViolation) ;
	#else
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcPageFault) ;
	#endif      
	iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription);

    Teardown();
    return KErrNone;
    
}
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterServiceDescriptionLL_nullL(TTestResult& aResult){
	SetupL(); 

    delete iSenXmlServiceDescription;
    iSenXmlServiceDescription = NULL;
   	#ifdef __WINS__
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcAccessViolation) ;
	#else
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcPageFault) ;
	#endif      
	iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription);

    Teardown();
    return KErrNone;
    
    }
//-------------------------------------------
//              UNREGISTERING SERVICE
//-------------------------------------------

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_normalL(TTestResult& aResult)
    {
	SetupL(); 

    iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    iSenXmlServiceDescription->SetEndPointL(KText);
    iSenXmlServiceDescription->SetContractL(KText);
    LOCAL_ASSERT((iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNone);
    LOCAL_ASSERT((iServiceManager->UnregisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNone);

    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_badDescriptorL(TTestResult& aResult){
	SetupL(); 

    Teardown();
    return KErrNone;
}
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_noEndpointL(TTestResult& aResult)
    {
	SetupL(); 

    iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    iSenXmlServiceDescription->SetEndPointL(KText);
    iSenXmlServiceDescription->SetContractL(KText);
    LOCAL_ASSERT((iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNone);
    iSenXmlServiceDescription->SetEndPointL(KNullDesC8);

    Teardown();
    return KErrNone;
    }

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_noContractL(TTestResult& aResult)
    {
	SetupL(); 

   iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    iSenXmlServiceDescription->SetEndPointL(KText);
    iSenXmlServiceDescription->SetContractL(KText);
    LOCAL_ASSERT((iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNone);
    iSenXmlServiceDescription->SetContractL(KNullDesC8);
    LOCAL_ASSERT((iServiceManager->UnregisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrSenNoContract);

    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_badFrameworkL(TTestResult& aResult)
    {
	SetupL(); 

    _LIT8(KBadFramework,"bla");
    iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    iSenXmlServiceDescription->SetEndPointL(KText);
    iSenXmlServiceDescription->SetContractL(KText);
    LOCAL_ASSERT((iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNone);
    iSenXmlServiceDescription->SetFrameworkIdL(KBadFramework);
    LOCAL_ASSERT((iServiceManager->UnregisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNotFound);

    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_randomPtrL(TTestResult& aResult)
    {
	SetupL(); 

    iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    iSenXmlServiceDescription->SetEndPointL(KText);
    iSenXmlServiceDescription->SetContractL(KText);
    LOCAL_ASSERT((iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNone);
    iSenXmlServiceDescription = (CSenXmlServiceDescription *)0xeaeaea;
   	#ifdef __WINS__
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcAccessViolation) ;
	#else
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcPageFault) ;
	#endif      
	iServiceManager->UnregisterServiceDescriptionL( *iSenXmlServiceDescription);

    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterServiceDescriptionLL_nullL(TTestResult& aResult)
    {
	SetupL(); 

    iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    iSenXmlServiceDescription->SetEndPointL(KText);
    iSenXmlServiceDescription->SetContractL(KText);
    LOCAL_ASSERT((iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNone);
    iSenXmlServiceDescription = NULL;
   	#ifdef __WINS__
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcAccessViolation) ;
	#else
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcPageFault) ;
	#endif      
	iServiceManager->UnregisterServiceDescriptionL( *iSenXmlServiceDescription);

    Teardown();
    return KErrNone;
    
    }

//-------------------------------------------
//              REGISTERING IDENTITY
//-------------------------------------------

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterIdentityProviderLL_normalL(TTestResult& aResult)
    {
	SetupL(); 

    iProvider->SetProviderID(KText);
    iProvider->SetUserInfoL(KText, KText, KText);
    LOCAL_ASSERT((iServiceManager->RegisterIdentityProviderL(*iProvider)) == KErrNone); 

	//Added Vaibhav
	iServiceManager->UnregisterIdentityProviderL(*iProvider);
	
    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterIdentityProviderLL_badDescriptorL(TTestResult& aResult)
    {
	SetupL(); 

    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterIdentityProviderLL_providerIDInUseL(TTestResult& aResult)
    {
    TRAPD(err, SetupL())
    if(err != KErrNone)return err;

    iProvider->SetProviderID(KText);
    iProvider->SetUserInfoL(KText, KText, KText);
    iServiceManager->RegisterIdentityProviderL(*iProvider);
    
    CSenIdentityProvider* provider = CSenIdentityProvider::NewL(KText2, KText2);
    provider->SetProviderID(KText);
    LOCAL_ASSERT((iServiceManager->RegisterIdentityProviderL(*provider)) == KErrSenProviderIdInUseByAnotherEndpoint); 
    delete provider;

    Teardown();
    return KErrNone;
    }

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterIdentityProviderLL_nullL(TTestResult& aResult)
    {
	SetupL(); 

    delete iProvider;
    iProvider = NULL;
   	#ifdef __WINS__
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcAccessViolation) ;
	#else
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcPageFault) ;
	#endif      
	iServiceManager->RegisterIdentityProviderL(*iProvider);
    
    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_RegisterIdentityProviderLL_randomPtrL(TTestResult& aResult)
    {
	SetupL(); 

    delete iProvider;
    iProvider = (CSenIdentityProvider *)0xeaeaea;
   	#ifdef __WINS__
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcAccessViolation) ;
	#else
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcPageFault) ;
	#endif      
	iServiceManager->RegisterIdentityProviderL(*iProvider);

    Teardown();
    return KErrNone;
    }

//-------------------------------------------
//              UNREGISTERING IDENTITY
//-------------------------------------------

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterIdentityProviderLL_normalL(TTestResult& aResult)
    {
	SetupL(); 

    iProvider->SetProviderID(KText);
    iProvider->SetUserInfoL(KText, KText, KText);
    LOCAL_ASSERT((iServiceManager->RegisterIdentityProviderL(*iProvider)) == KErrNone); 
    LOCAL_ASSERT((iServiceManager->UnregisterIdentityProviderL( *iProvider)) == KErrNone);

    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterIdentityProviderLL_badDescriptorL(TTestResult& aResult)
    {
	SetupL(); 

    Teardown();
    return KErrNone;
    }

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterIdentityProviderLL_notFoundL(TTestResult& aResult)
    {
	SetupL(); 

    iProvider->SetProviderID(KText);
    iProvider->SetUserInfoL(KText, KText, KText);
    LOCAL_ASSERT((iServiceManager->RegisterIdentityProviderL(*iProvider)) == KErrNone); 

    CSenIdentityProvider* provider = CSenIdentityProvider::NewL(KText2, KText2);
    provider->SetProviderID(KText2);
    LOCAL_ASSERT((iServiceManager->UnregisterIdentityProviderL( *provider)) == KErrNotFound);
    delete provider;

    Teardown();
    return KErrNone;
    }

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterIdentityProviderLL_nullL(TTestResult& aResult)
    {
	SetupL(); 

    delete iProvider;
    iProvider = NULL;
   	#ifdef __WINS__
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcAccessViolation) ;
	#else
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcPageFault) ;
	#endif      
	iServiceManager->UnregisterIdentityProviderL( *iProvider);

    Teardown();
    return KErrNone;
    
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_UnregisterIdentityProviderLL_randomPtrL(TTestResult& aResult)
    {
	SetupL(); 

    delete iProvider;
    iProvider = (CSenIdentityProvider *)0xeaeaea;
   	#ifdef __WINS__
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcAccessViolation) ;
	#else
		TestModuleIf().SetExitReason (TestModuleIf(). EException, EExcPageFault) ;
	#endif      
	iServiceManager->UnregisterIdentityProviderL( *iProvider);

    Teardown();
    return KErrNone;
    
    }

//-------------------------------------------
//              ASSOCIATE IDENTITY
//-------------------------------------------

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_AssociateServiceLL_normalL(TTestResult& aResult)
 {
	SetupL(); 

    iProvider->SetProviderID(KText2);
   	iProvider->SetUserInfoL(KText, KNullDesC8, KNullDesC8);

    iSenXmlServiceDescription->SetEndPointL(KText);
    iSenXmlServiceDescription->SetContractL(KText);
    iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    LOCAL_ASSERT((iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNone);
    LOCAL_ASSERT((iServiceManager->RegisterIdentityProviderL(*iProvider)) == KErrNone); 
    LOCAL_ASSERT((iServiceManager->AssociateServiceL(KText, KText2)) == KErrNone);
    //2nd time doesnt matter
    LOCAL_ASSERT((iServiceManager->AssociateServiceL(KText, KText2)) == KErrNone);

	//Added Vaibhav
	iServiceManager->DissociateServiceL(KText, KText2);
	iServiceManager->UnregisterIdentityProviderL(*iProvider);
	iServiceManager->UnregisterServiceDescriptionL( *iSenXmlServiceDescription);

    Teardown();
    return KErrNone;
 }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_AssociateServiceLL_argumentL(TTestResult& aResult)
 {
	SetupL(); 

    LOCAL_ASSERT((iServiceManager->AssociateServiceL(KText, KNullDesC8)) == KErrArgument);
    LOCAL_ASSERT((iServiceManager->AssociateServiceL(KNullDesC8, KText)) == KErrArgument);
    LOCAL_ASSERT((iServiceManager->AssociateServiceL(KNullDesC8, KNullDesC8)) == KErrArgument);
    _LIT8(KPattern, "&<>");
    iProvider->SetProviderID(KText);
    LOCAL_ASSERT((iServiceManager->RegisterIdentityProviderL(*iProvider)) == KErrNone); 

    Teardown();
    return KErrNone;
 }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_AssociateServiceLL_notReadyL(TTestResult& aResult)
 {
	SetupL(); 

    Teardown();
    return KErrNone;
 }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_AssociateServiceLL_notFoundL(TTestResult& aResult)
 {
    TRAPD(err, SetupL())
    if(err != KErrNone)return err;

    iProvider->SetProviderID(KText);
    LOCAL_ASSERT((iServiceManager->RegisterIdentityProviderL(*iProvider)) == KErrNone); 
    LOCAL_ASSERT((iServiceManager->AssociateServiceL(KText2, KText2)) == KErrNotFound);

    Teardown();
    return KErrNone;
 }
//-------------------------------------------
//              DISSOCIATE IDENTITY
//-------------------------------------------

TInt CSenServiceManagerBCTest::UT_CSenServiceManager_DissociateServiceLL_normalL(TTestResult& aResult)
    {
	SetupL(); 

    iProvider->SetProviderID(KText2);
   	iProvider->SetUserInfoL(KText, KNullDesC8, KNullDesC8);

    iSenXmlServiceDescription->SetEndPointL(KText);
    iSenXmlServiceDescription->SetContractL(KText);
    iSenXmlServiceDescription->SetFrameworkIdL(KDefaultIdWsfFrameworkID);
    LOCAL_ASSERT((iServiceManager->RegisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNone);
    LOCAL_ASSERT((iServiceManager->RegisterIdentityProviderL(*iProvider)) == KErrNone); 
    LOCAL_ASSERT((iServiceManager->AssociateServiceL(KText, KText2)) == KErrNone);
    TInt error = KErrNone;
    error = iServiceManager->DissociateServiceL(KText, KText2);
    RDebug::Print( _L( "WSModTester: DissociateService status %d" ), error);
    LOCAL_ASSERT(error == KErrNone);

    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_DissociateServiceLL_argumentL(TTestResult& aResult)
    {
	SetupL(); 

    LOCAL_ASSERT((iServiceManager->DissociateServiceL(KText, KNullDesC8)) == KErrArgument);
    LOCAL_ASSERT((iServiceManager->DissociateServiceL(KNullDesC8, KText)) == KErrArgument);
    LOCAL_ASSERT((iServiceManager->DissociateServiceL(KNullDesC8, KNullDesC8)) == KErrArgument);

    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_DissociateServiceLL_notReadyL(TTestResult& aResult)
    {
	SetupL(); 

    Teardown();
    return KErrNone;
    }
TInt CSenServiceManagerBCTest::UT_CSenServiceManager_DissociateServiceLL_notFoundL(TTestResult& aResult)
    {
    TRAPD(err, SetupL())
    if(err != KErrNone)return err;

    LOCAL_ASSERT((iServiceManager->DissociateServiceL(KText, KText)) == KErrNotFound);
    _LIT8(KPattern, "&<>");

    Teardown();
    return KErrNone;
    }



// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
?type ?classname::?member_function(
   ?arg_type arg,
   ?arg_type arg )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
/*
?type  ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg )  // ?description
    {

    ?code

    }
*/
//  End of File
