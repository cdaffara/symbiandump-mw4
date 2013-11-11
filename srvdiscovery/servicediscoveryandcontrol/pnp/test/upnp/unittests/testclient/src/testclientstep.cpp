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
// testupnpdescriptionservicestep.cpp
// Contains implementation of testupnpdescriptionservicestep class
// @internalAll
// 
//

// System Include
// for StartC32()
#include <c32comm.h>
#include <e32base.h>
#include "pnputils.h"

// User Include
#include "testclientstep.h"
/**
Constructor: Sets the test step name.
@internalTechnology
@test
*/
CTestClientServiceTestStep::CTestClientServiceTestStep()
	{

	SetTestStepName(KTestClientServiceStep);
	}


/**
Destructor: Closes the iFileServ.
@internalTechnology
@test
*/
CTestClientServiceTestStep::~CTestClientServiceTestStep()
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
TVerdict CTestClientServiceTestStep::doTestStepPreambleL()
	{
    return TestStepResult();
	}	// doTestPreambleL


/**
Base class pure virtual doTestStepL():
@internalTechnology
@test
@param		None
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestClientServiceTestStep::doTestStepL()
	{
	_LIT(KUriValidity, "uriValidity");
	_LIT(KHostResolver, "hostResolver");

	TInt result = 0;
	TInt testUriValidity;
	TInt testHostResolver;
	
	if (!GetIntFromConfig(ConfigSection(),KUriValidity,testUriValidity)||
	!GetIntFromConfig(ConfigSection(),KHostResolver,testHostResolver))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	while( testUriValidity )
		{
		switch ( testUriValidity )
		{
			
		case 1 : 
			{
			_LIT8(KDes,"des");
			result = CUPnPUtils::GenericUriValidity ( KDes ,CUPnPUtils::EDevice);
			if ( result == KErrCorrupt)
				testUriValidity = 2;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;
		case 2:
			{
			_LIT8(KUid,"uuidhelloworld");
			result = CUPnPUtils::GenericUriValidity ( KUid ,CUPnPUtils::EDevice);
			if ( result == KErrCorrupt)
				testUriValidity = 3;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			
			break;
		case 3 : 
			{
			_LIT8(KHello,"helloworld");
			result = CUPnPUtils::GenericUriValidity ( KHello ,CUPnPUtils::EDevice);
			if ( result == KErrCorrupt)
				testUriValidity = 4;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;		
		case 4 : 
			{
			_LIT8(KUrn,"urn:");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EDevice);
			if ( result == KErrCorrupt)
				testUriValidity = 5;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;	
		case 5 : 
			{
			_LIT8(KUrn,"urn:able");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EDevice);
			if ( result == KErrCorrupt)
				testUriValidity = 6;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;	
		case 6 : 
			{
			_LIT8(KUrn,"urn:able:");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EDevice);
			if ( result == KErrCorrupt)
				testUriValidity = 7;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;		
		case 7 : 
			{
			_LIT8(KUrn,"urn:able:");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EDevice);
			if ( result == KErrCorrupt)
				testUriValidity = 8;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;	
		case 8 : 
			{
			_LIT8(KUrn,"urn:able:device");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EDevice);
			if ( result == KErrCorrupt)
				testUriValidity = 9;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;	
		case 9 : 
			{
			_LIT8(KUrn,"urn:able:service:1");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EDevice);
			if ( result == KErrCorrupt)
				testUriValidity = 10;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;	
		case 10 : 
			{
			_LIT8(KUrn,"urn:able:device:1");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EService);
			if ( result == KErrCorrupt)
				testUriValidity = 11;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;	
		case 11 : 
			{
			_LIT8(KUrn,"urn:able:helllo:1");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EAny);
			if ( result == KErrCorrupt)
				testUriValidity = 12;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;	
		case 12 : 
			{
			_LIT8(KUrn,"urn:able:helllo:1");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EDevice);
			if ( result == KErrCorrupt)
				testUriValidity = 13;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;	
		case 13 : 
			{
			_LIT8(KUrn,"urn:able:service:");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EService);
			if ( result == KErrCorrupt)
				testUriValidity = 14;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;	
		case 14 : 
			{
			_LIT8(KUrn,"urn:able:service:serviceType");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EService);
			if ( result == KErrCorrupt)
				testUriValidity = 15;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;	
		case 15 : 
			{
			_LIT8(KUrn,"urn:able:service:serviceType");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EService);
			if ( result == KErrCorrupt)
				testUriValidity = 16;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;	
		case 16 : 
			{
			_LIT8(KUrn,"urn:able:service:abcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghij:1");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EService);
			if ( result == KErrCorrupt)
				testUriValidity = 17;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;		
		case 17 : 
			{
			_LIT8(KUrn,"urn:able:service:serviceType:+");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EService);
			if ( result == KErrCorrupt)
				testUriValidity = 18;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;
			
		case 18 : 
			{
			_LIT8(KUrn,"urn:able:service:serviceType:a");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EService);
			if ( result == KErrCorrupt)
				testUriValidity = 19;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;
		case 19 :
			{
			_LIT8(KUrn,"uuid:1348481083930030030");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EDevice);
			if ( result == KErrNone)
				testUriValidity = 20;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;
		case 20 :
			{
			_LIT8(KUrn,"upnp:rootdevice");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EDevice);
			if ( result == KErrNone)
				testUriValidity = 21;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;
		case 21 :
			{
			_LIT8(KUrn,"urn:schemas-upnp-org:service:ContentDirectory:1");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EService);
			if ( result == KErrNone)
				testUriValidity = 22;
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
			break;
		case 22 :
			{
			_LIT8(KUrn,"urn:schemas-upnp-org:device:ContentDirectory:1");
			result = CUPnPUtils::GenericUriValidity ( KUrn ,CUPnPUtils::EDevice);
			if ( result == KErrNone)
				{
				testUriValidity = 0;
				SetTestStepResult(EPass);
				return TestStepResult();
				}
			else
				{
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}			
			
		
		default:
			{
			SetTestStepResult(EFail);
			return TestStepResult();		
			}			
		}
		}
	while ( testHostResolver )
		{
		RSocketServ ss;
		TInt err = ss.Connect();
		RHostResolver resolver;
		err = resolver.Open( ss , KAfInet , KProtocolInetTcp);// , iConnection);
		TInetAddr address;
		switch ( testHostResolver )
			{
			case 1:
				{
				_LIT8(KUri,"http://10.192.197.2:2869/");
				TRAP(err, CUPnPUtils::ResolveHostAddressL ( resolver,KUri, address ));
				if ( err == KErrNone )
					testHostResolver = 2;
				else
					{
					SetTestStepResult(EFail);
					return TestStepResult();
					}	
				}	
			case 2:
				{
				_LIT8(KUri,"junk://notneeded.com/");
				TRAP(err, CUPnPUtils::ResolveHostAddressL ( resolver,KUri, address ));
				if ( err == KErrCorrupt )
					testHostResolver = 3;
				else
					{
					SetTestStepResult(EFail);
					return TestStepResult();
					}	
				}	
			case 3:
				{
				_LIT8(KUri,"http://www.google.com/");
				TRAP(err, CUPnPUtils::ResolveHostAddressL ( resolver,KUri, address ));
				if ( err == KErrNone )
					{
					SetTestStepResult(EPass);
					return TestStepResult();
					}
				else
					{
					SetTestStepResult(EFail);
					return TestStepResult();
					}
				}
				
			}
		}
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

