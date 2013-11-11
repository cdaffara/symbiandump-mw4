// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains constants refered in test code
// 
//

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

// Constant literals
_LIT(KTestPnPManager, "TestPnPManager");


const TInt KMaxLogSize 		    = 1024;
const TInt KMaxUriSize 		    = 1024;
const TUint KTestTierId 	    = 0x2025D054;
const TUint KInvalidTestTierId 	= 0x2525D025;

//Constants
_LIT(KDiscovery, 			"discovery");
_LIT(KDescribe, 			"describe");
_LIT(KSubscribe, 			"subscribe");
_LIT(KRegisterAnnouncement, "registernotify");
_LIT(KPublishService, 		"publishservice");
_LIT(KNotify, 				"notify");
_LIT(KCancelDiscover, 		"canceldiscovery");
_LIT(KCancelDescribe, 		"canceldescribe");
_LIT(KCancelNotify,			"cancelnotify");
_LIT(KReadIni,				"readini");
_LIT(KDescribeService,		"describeservice");
_LIT(KOpenDiscoverer,		"opendiscoverer");
_LIT(KOpenPublisher,		"openpublisher");
_LIT(KCancelAll,			"cancelall");
_LIT(KPositiveCase,			"positivecase");
_LIT(KOpenDiscovererFail,	"opendiscovererfailed");
_LIT(KOpenPublisherFail,	"openpublisherfailed");

//Constant literals -- Discovery response 
_LIT(KResponseTime, 		"responsetime");
_LIT(KDescription, 			"description");
_LIT(KUri, 					"uri");
_LIT(KServiceType,			"servicetype");
_LIT(KOperationType,		"operationtype");
_LIT(KUrl,					"url");

const TInt KMaxBufLength = 256;


_LIT8(KSlash, "/");
_LIT8(KColonSlashSlash, "://");
_LIT8(KColon, ":");


//Panics
_LIT(KDescribeTestPanicCategory,"Describe");
_LIT(KDescoveryTestPanicCategory, "Discovery");

	


#endif //__CONSTANTS_H__
