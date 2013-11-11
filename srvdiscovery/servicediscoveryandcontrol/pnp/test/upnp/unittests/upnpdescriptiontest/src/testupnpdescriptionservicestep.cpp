// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Contains implementation of testupnpdescriptionservicestep class
// @internalAll
// 
//

// System Include
// for StartC32()
#include <c32comm.h>
#include <e32base.h>

// User Include
#include "testupnpdescriptionservicestep.h"
// library include
#include <cupnpdescriptionparser.h>
#include <cupnpdescriptioncomposer.h>
#include "cpnpdeviceparam.h"
#include <cstringpoolmanager.h>
#include <testvendordeviceschema.h>
#include <testvendorserviceschema.h>

_LIT(KDirName, "c:\\upnp\\description\\xml\\");

/**
Constructor: Sets the test step name.
@internalTechnology
@test
*/
CTestXmlServiceTestStep::CTestXmlServiceTestStep()
	{

	SetTestStepName(KTestXmlServiceStep);
	}


/**
Destructor: Closes the iFileServ.
@internalTechnology
@test
*/
CTestXmlServiceTestStep::~CTestXmlServiceTestStep()
	{
	}


/**
Base class virtual doTestStepPreambleL():
Create and install the active scheduler and connect to iFileServ (RFs).
@internalTechnology
@test
@param		None
@return		EPass or EFail.
*/
TVerdict CTestXmlServiceTestStep::doTestStepPreambleL()
	{
    return TestStepResult();
	}	// doTestPreambleL


/**
Base class pure virtual doTestStepL():
Tests the ebo support in http.
@internalTechnology
@test
@param		None
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestXmlServiceTestStep::doTestStepL()
	{
	TPtrC	aInputPath;
	TPtrC	aOutputPath;
	TBool	aIsService;
	TBool	aIsPositive;
    TInt	err=KErrNone;
    RFs		fs;
    RFile	handle;
    _LIT(KVendorDevice,"Xml_61");
    _LIT(KVendorService,"Xml_62");
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    // create the output folder
    
    err = fs.MkDirAll(KDirName);
    if ((err != KErrNone) && (err!=KErrAlreadyExists))
    	{
    		User::LeaveIfError(err);
    	}

	if(
	!GetStringFromConfig(ConfigSection(),KIniInputPath,aInputPath)||
	!GetStringFromConfig(ConfigSection(),KIniOutputPath,aOutputPath)||
	!GetBoolFromConfig(ConfigSection(),KIniIsTestCasePositive,aIsPositive)||
	!GetBoolFromConfig(ConfigSection(),KIniIsInputFileService,aIsService))
		{
		ERR_PRINTF5(_L("Problem in reading values from ini.			\
				\nExpected fields are: \n%S\n \n%S\n \n%S\n \n%S\n" 
			  ),&KIniInputPath,&KIniOutputPath,&KIniIsTestCasePositive,&KIniIsInputFileService);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	//-----------------------
	RFile 			handle1;
	
	TRAP(err,handle1.Open(fs, aInputPath, EFileShareReadersOnly));
	// For Hardware system path is c:, so descriptionPath value present in '.ini' is referring 'c:'
	if ( err == KErrPathNotFound )
		{				
		RBuf fileName;
		TDriveName aSystemDrive;
		TDriveUnit driveunit(RFs::GetSystemDrive());
		aSystemDrive.Zero();
		aSystemDrive=driveunit.Name();				
		fileName.CreateL ( aInputPath.Length () );
		fileName.Zero();
		fileName.Append(aSystemDrive);
		fileName.Append ( aInputPath.Mid ( aSystemDrive.Length () ) );		
		
		err = handle1.Open(fs, fileName, EFileShareReadersOnly);
		}
	if (err != KErrNone)
	    {
	    User::LeaveIfError(err);
	    }
	CleanupClosePushL(handle1);
	TInt aFileSize = 0;
	handle1.Size(aFileSize);
	
	HBufC8 * Buf1 = HBufC8::NewL(aFileSize);
	TPtr8 Pointer = Buf1->Des();
	
	TInt err1 = handle1.Read(Pointer,aFileSize);
	
	CleanupStack::PopAndDestroy(&handle1);

	//-----------------------

    err = handle.Open(fs,aOutputPath,EFileWrite);
    if (err == KErrNotFound)
        {
        err=handle.Create(fs,aOutputPath,EFileWrite);
        if(err != KErrNone )
        	SetTestStepResult(EFail);
        }
    CleanupClosePushL(handle);

    RBuf8 aComposedXml;
    CleanupClosePushL(aComposedXml);
    CStringPoolManager* poolManager = CStringPoolManager::NewL();
    CleanupStack::PushL( poolManager );
    if ( ConfigSection() == KVendorDevice )
    	poolManager->SetStringTableL( UPNPVENDORDEVICETAGS::Table);
    else if ( ConfigSection() == KVendorService )
    	poolManager->SetStringTableL( UPNPVENDORSERVICETAGS::Table);
    if(aIsService)
    	{
        CUPnPServiceDescription *aServDescObj = NULL;
               	
        CUPnPDescriptionParser* app = CUPnPDescriptionParser::NewL( poolManager->StringPool() ,CUPnPDescriptionParser::EService);
	    CleanupStack::PushL(app);
	    CUPnPDescriptionComposer *comp  = CUPnPDescriptionComposer::NewL( poolManager->StringPool());
	    CleanupStack::PushL(comp);

	    TRAPD(error1,aServDescObj = static_cast<CUPnPServiceDescription*> (app->ParseDescriptionBufL(Pointer)));
	    if(error1!=KErrNone)
	    	{
	    	SetTestStepResult(EFail);
	    	SetTestStepError(error1);
	    	}
	    else
	    	{
	    	//TRAPD(error2,comp->ComposeServiceXmlL(app->GetServiceDescObj(),aComposedXml));
	    	TRAPD(error2,comp->ComposeDescriptionXmlL(aServDescObj,CUPnPDescriptionComposer::EService , aComposedXml));
		    if(error2!=KErrNone)
		    	{
		    	SetTestStepResult(EFail);
		    	SetTestStepError(error2);
		    	}
		    else
		    	{
		    	SetTestStepResult(EPass);
		    	}
	    	}
	    delete aServDescObj;

	    CleanupStack::PopAndDestroy(comp);
	    CleanupStack::PopAndDestroy(app);
    	}
    else
    	{
    	CUPnPDeviceDescription *aDeviceDescObj = NULL;
    	CUPnPDescriptionParser* app = CUPnPDescriptionParser::NewL(poolManager->StringPool(), CUPnPDescriptionParser::EDevice);
        CleanupStack::PushL(app);
        CUPnPDescriptionComposer *comp  = CUPnPDescriptionComposer::NewL( poolManager->StringPool() );
        CleanupStack::PushL(comp);
            
        TRAPD(error1,aDeviceDescObj = static_cast<CUPnPDeviceDescription*> (app->ParseDescriptionBufL(Pointer)));
        if(error1!=KErrNone)
        	{
	    	SetTestStepResult(EFail);
	    	SetTestStepError(error1);        	
	    	}
        else
        	{
        	//TRAPD(error2,comp->ComposeDeviceXmlL(app->GetDeviceDescObj(),aComposedXml));
        	TRAPD(error2,comp->ComposeDescriptionXmlL(aDeviceDescObj, CUPnPDescriptionComposer::EDevice,aComposedXml));
    	    if(error2!=KErrNone)
    	    	{
    	    	SetTestStepResult(EFail);
    	    	SetTestStepError(error2);
    	    	}
    	    else
    	    	{
    	    	SetTestStepResult(EPass);
    	    	}
    	    	
        	}
        delete aDeviceDescObj;

        CleanupStack::PopAndDestroy(comp);
        CleanupStack::PopAndDestroy(app);
    	}

    
    delete Buf1;
    handle.Write(aComposedXml);
    handle.Close();
    CleanupStack::PopAndDestroy(poolManager);
    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy(&handle);
    CleanupStack::PopAndDestroy(&fs);

	
	return TestStepResult();

	}	// doTestStepL

