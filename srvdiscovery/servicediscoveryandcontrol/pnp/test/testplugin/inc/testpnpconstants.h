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
//


#ifndef __TESTPNPCONSTANTS_H_
#define __TESTPNPCONSTANTS_H_

/**
This is the tier id value which must be specified whenever the user wants to open a TestPnP Service Point or Control Point. 
This is the parameter which must be specified in the RPnPServicePublisher::Open or RPnPServiceDiscovery::Open APIs
*/

const TUint KTestPnPInterface  = 0x2025D054;


//implementation uid for discovery 
const TUint KTestPnPDiscoveryUid = 0x2025DC69;

//implementation uid for publisher
const TUint KTestPnPPublisherUid = 0x2025DC6A;

//implementation uid for params factory
const TInt KTestPnPParamsFactoryUid = 0x2001D05E;


//TestPnP panic codes
enum TTestPnPPanic
	{
	KTestPnPErrNotOpen = 1 , //Set the starting error code.
	KTestPnPErrNoObserver,
	KTestPnPErrInvalidFamilyCount,
	KTestPnPErrInvalidFamily,
	KTestPnPErrNoParamSet,
	KTestPnPErrDiscoveryInProgress,
	KTestPnPErrPublish,
	KTestPnPErrResponseUriNotFound,
	};


#endif //__TESTPNPCONSTANTS_H_


