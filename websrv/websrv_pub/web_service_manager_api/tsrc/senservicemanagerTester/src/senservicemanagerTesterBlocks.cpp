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
#include "senservicemanagerTester.h"

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

#define LOCAL_ASSERT(expression)	{if(!(expression)){return KErrArgument;}}
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CsenservicemanagerTester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CsenservicemanagerTester::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CsenservicemanagerTester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CsenservicemanagerTester::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
		ENTRY("NewL-test", CsenservicemanagerTester::UT_CSenServiceManager_NewLL),
		ENTRY("NewLC-test", CsenservicemanagerTester::UT_CSenServiceManager_NewLCL),
		ENTRY("NewL-test1-CSenServiceManager", CsenservicemanagerTester::UT_CSenServiceManager_NewL_1L),
		ENTRY("NewLC-test1C-CSenServiceManager", UT_CSenServiceManager_NewL_1CL),
		ENTRY("normal-ServiceDescriptionsL", CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsLL_normalL),
		ENTRY("notFound-ServiceDescriptionsL", CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsLL_notFoundL),
		ENTRY("badDescriptor-ServiceDescriptionsL", CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsLL_badDescriptorL),
		ENTRY("NULL-ServiceDescriptionsL", CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsLL_nullL),
		ENTRY("randomPtr-ServiceDescriptionsL", CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsLL_randomPtrL),
		ENTRY("normal(URI)-ServiceDescriptionsL", CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsL_1L_normalL),
		ENTRY("notFound(URI)-ServiceDescriptionsL", CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsL_1L_notFoundL),
		ENTRY("badDescriptor(URI)-ServiceDescriptionsL", CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsL_1L_badDescriptorL),
		ENTRY("XMLPattern(URI)-ServiceDescriptionsL", CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsL_1L_XMLpatternL),
		ENTRY("normal-RegisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_normalL),
		ENTRY("badDescriptor-RegisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_badDescriptorL),
		ENTRY("no_Endpoint-RegisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_noEndpointL),
		ENTRY("no_Contract-RegisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_noContractL),
		ENTRY("bad_Framework-RegisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_badFrameworkL),
		ENTRY("random_Ptr-RegisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_randomPtrL),
		ENTRY("NULL-RegisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_nullL),
		ENTRY("normal-UnregisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_normalL),
		ENTRY("badDescriptor-UnregisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_badDescriptorL),
		ENTRY("noEndpoint-UnregisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_noEndpointL),
		ENTRY("noContract-UnregisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_noContractL),
		ENTRY("badFramework-UnregisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_badFrameworkL),
		ENTRY("randomPtr-UnregisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_randomPtrL),
		ENTRY("NULL-UnregisterServiceDescriptionL", CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_nullL),
		ENTRY("normal-RegisterIdentityProviderL", CsenservicemanagerTester::UT_CSenServiceManager_RegisterIdentityProviderLL_normalL),
		ENTRY("badDescriptor-RegisterIdentityProviderL", CsenservicemanagerTester::UT_CSenServiceManager_RegisterIdentityProviderLL_badDescriptorL),
		ENTRY("providerIDInUse-RegisterIdentityProviderL", CsenservicemanagerTester::UT_CSenServiceManager_RegisterIdentityProviderLL_providerIDInUseL),
		ENTRY("NULL-RegisterIdentityProviderL", CsenservicemanagerTester::UT_CSenServiceManager_RegisterIdentityProviderLL_nullL),
		ENTRY("randomPtr-RegisterIdentityProviderL", CsenservicemanagerTester::UT_CSenServiceManager_RegisterIdentityProviderLL_randomPtrL),
		ENTRY("normal-UnregisterIdentityProviderL", CsenservicemanagerTester::UT_CSenServiceManager_UnregisterIdentityProviderLL_normalL),
		ENTRY("badDescriptor-UnregisterIdentityProviderL", CsenservicemanagerTester::UT_CSenServiceManager_UnregisterIdentityProviderLL_badDescriptorL),
		ENTRY("notFound-UnregisterIdentityProviderL", CsenservicemanagerTester::UT_CSenServiceManager_UnregisterIdentityProviderLL_notFoundL),
		ENTRY("NULL-UnregisterIdentityProviderL", CsenservicemanagerTester::UT_CSenServiceManager_UnregisterIdentityProviderLL_nullL),
		ENTRY("randomPtr-UnregisterIdentityProviderL", CsenservicemanagerTester::UT_CSenServiceManager_UnregisterIdentityProviderLL_randomPtrL),
		ENTRY("normal-AssociateServiceL", CsenservicemanagerTester::UT_CSenServiceManager_AssociateServiceLL_normalL),
		ENTRY("argument-AssociateServiceL", CsenservicemanagerTester::UT_CSenServiceManager_AssociateServiceLL_argumentL),
		ENTRY("notReady-AssociateServiceL", CsenservicemanagerTester::UT_CSenServiceManager_AssociateServiceLL_notReadyL),
		ENTRY("notFound-AssociateServiceL", CsenservicemanagerTester::UT_CSenServiceManager_AssociateServiceLL_notFoundL),
		ENTRY("normal-DissociateServiceL", CsenservicemanagerTester::UT_CSenServiceManager_DissociateServiceLL_normalL),
		ENTRY("argument-DissociateServiceL", CsenservicemanagerTester::UT_CSenServiceManager_DissociateServiceLL_argumentL),
		ENTRY("notReady-DissociateServiceL", CsenservicemanagerTester::UT_CSenServiceManager_DissociateServiceLL_notReadyL),
		ENTRY("notFound-DissociateServiceL", CsenservicemanagerTester::UT_CSenServiceManager_DissociateServiceLL_notFoundL),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


void CsenservicemanagerTester::Empty(  )
{
}
void CsenservicemanagerTester::DeleteDBL(){
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

void CsenservicemanagerTester::SetupL(  )
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

void CsenservicemanagerTester::Teardown(  )
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

TInt CsenservicemanagerTester::UT_CSenServiceManager_NewLL( CStifItemParser& aItem )
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

TInt CsenservicemanagerTester::UT_CSenServiceManager_NewLCL( CStifItemParser& aItem )
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
    
TInt CsenservicemanagerTester::UT_CSenServiceManager_NewL_1L( CStifItemParser& aItem )
    {
    Empty();
    CSenServiceManager*     serviceManager = NULL;
    MSenAuthenticationProvider aProvider;
    serviceManager = CSenServiceManager::NewL(aProvider);
    TL(serviceManager != (CSenServiceManager*)NULL);
    if (serviceManager != NULL){
        __ASSERT_ALWAYS_NO_LEAVE(delete serviceManager );
        serviceManager = NULL;
    }
    Empty();
	return KErrNone;
    
    }
    
TInt CsenservicemanagerTester::UT_CSenServiceManager_NewL_1CL( CStifItemParser& aItem )
    {
    Empty();
    CSenServiceManager*     serviceManager = NULL;
    MSenAuthenticationProvider aProvider;
    serviceManager = CSenServiceManager::NewLC(aProvider);
    TL(serviceManager != (CSenServiceManager*)NULL);
    CleanupStack::PopAndDestroy();
    serviceManager = NULL;
    Empty();
	return KErrNone;
    
    }

//-------------------------------------------
//              SEARCHING
//-------------------------------------------
TInt CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsLL_normalL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsLL_notFoundL(CStifItemParser& aItem)
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

TInt CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsLL_badDescriptorL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsLL_nullL(CStifItemParser& aItem)
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


TInt CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsLL_randomPtrL(CStifItemParser& aItem)
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


TInt CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsL_1L_normalL(CStifItemParser& aItem)
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
    LOCAL_ASSERT((iServiceManager->UnregisterServiceDescriptionL( *iSenXmlServiceDescription)) == KErrNone);
    
    arr.ResetAndDestroy();

	//Added Vaibhav
	//iServiceManager->UnregisterServiceDescriptionL( *iSenXmlServiceDescription);

    Teardown();
    return KErrNone;
    }
TInt CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsL_1L_notFoundL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsL_1L_badDescriptorL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_ServiceDescriptionsL_1L_XMLpatternL(CStifItemParser& aItem)
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

TInt CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_normalL(CStifItemParser& aItem){
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_badDescriptorL(CStifItemParser& aItem){
	SetupL(); 

    Teardown();
    return KErrNone;
}
TInt CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_noEndpointL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_noContractL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_badFrameworkL(CStifItemParser& aItem)
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

TInt CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_randomPtrL(CStifItemParser& aItem){
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_RegisterServiceDescriptionLL_nullL(CStifItemParser& aItem){
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

TInt CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_normalL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_badDescriptorL(CStifItemParser& aItem){
	SetupL(); 

    Teardown();
    return KErrNone;
}
TInt CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_noEndpointL(CStifItemParser& aItem)
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

TInt CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_noContractL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_badFrameworkL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_randomPtrL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_UnregisterServiceDescriptionLL_nullL(CStifItemParser& aItem)
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

TInt CsenservicemanagerTester::UT_CSenServiceManager_RegisterIdentityProviderLL_normalL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_RegisterIdentityProviderLL_badDescriptorL(CStifItemParser& aItem)
    {
	SetupL(); 

    Teardown();
    return KErrNone;
    }
TInt CsenservicemanagerTester::UT_CSenServiceManager_RegisterIdentityProviderLL_providerIDInUseL(CStifItemParser& aItem)
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

TInt CsenservicemanagerTester::UT_CSenServiceManager_RegisterIdentityProviderLL_nullL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_RegisterIdentityProviderLL_randomPtrL(CStifItemParser& aItem)
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

TInt CsenservicemanagerTester::UT_CSenServiceManager_UnregisterIdentityProviderLL_normalL(CStifItemParser& aItem)
    {
	SetupL(); 

    iProvider->SetProviderID(KText);
    iProvider->SetUserInfoL(KText, KText, KText);
    LOCAL_ASSERT((iServiceManager->RegisterIdentityProviderL(*iProvider)) == KErrNone); 
    LOCAL_ASSERT((iServiceManager->UnregisterIdentityProviderL( *iProvider)) == KErrNone);

    Teardown();
    return KErrNone;
    }
TInt CsenservicemanagerTester::UT_CSenServiceManager_UnregisterIdentityProviderLL_badDescriptorL(CStifItemParser& aItem)
    {
	SetupL(); 

    Teardown();
    return KErrNone;
    }

TInt CsenservicemanagerTester::UT_CSenServiceManager_UnregisterIdentityProviderLL_notFoundL(CStifItemParser& aItem)
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

TInt CsenservicemanagerTester::UT_CSenServiceManager_UnregisterIdentityProviderLL_nullL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_UnregisterIdentityProviderLL_randomPtrL(CStifItemParser& aItem)
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

TInt CsenservicemanagerTester::UT_CSenServiceManager_AssociateServiceLL_normalL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_AssociateServiceLL_argumentL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_AssociateServiceLL_notReadyL(CStifItemParser& aItem)
 {
	SetupL(); 

    Teardown();
    return KErrNone;
 }
TInt CsenservicemanagerTester::UT_CSenServiceManager_AssociateServiceLL_notFoundL(CStifItemParser& aItem)
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

TInt CsenservicemanagerTester::UT_CSenServiceManager_DissociateServiceLL_normalL(CStifItemParser& aItem)
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
TInt CsenservicemanagerTester::UT_CSenServiceManager_DissociateServiceLL_argumentL(CStifItemParser& aItem)
    {
	SetupL(); 

    LOCAL_ASSERT((iServiceManager->DissociateServiceL(KText, KNullDesC8)) == KErrArgument);
    LOCAL_ASSERT((iServiceManager->DissociateServiceL(KNullDesC8, KText)) == KErrArgument);
    LOCAL_ASSERT((iServiceManager->DissociateServiceL(KNullDesC8, KNullDesC8)) == KErrArgument);

    Teardown();
    return KErrNone;
    }
TInt CsenservicemanagerTester::UT_CSenServiceManager_DissociateServiceLL_notReadyL(CStifItemParser& aItem)
    {
	SetupL(); 

    Teardown();
    return KErrNone;
    }
TInt CsenservicemanagerTester::UT_CSenServiceManager_DissociateServiceLL_notFoundL(CStifItemParser& aItem)
    {
    TRAPD(err, SetupL())
    if(err != KErrNone)return err;

    LOCAL_ASSERT((iServiceManager->DissociateServiceL(KText, KText)) == KErrNotFound);
    _LIT8(KPattern, "&<>");

    Teardown();
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CsenservicemanagerTester::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CsenservicemanagerTester::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
