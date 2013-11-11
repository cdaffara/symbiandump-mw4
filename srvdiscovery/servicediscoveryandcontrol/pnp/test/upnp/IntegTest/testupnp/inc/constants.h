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
// Contains constants refered in test code
// 
//

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

// Constant literals
_LIT(KTestUPnPManager, "TestUPnPManager");
_LIT(KTestRControlChannel, "TestRControlChannel");


const TInt KMaxLogSize 		= 1024;
const TInt KMaxUriSize 		= 1024;
const TUint KTierId 		= 0x2000D054;

//Constants
_LIT(KDiscovery, 			"discovery");
_LIT(KDescribe, 			"describe");
_LIT(KSubscribe, 			"subscribe");
_LIT(KRegisterAnnouncement, "registernotify");
_LIT(KUnsubscribe, 			"unsubscribe");
_LIT(KPublishService, 		"publishservice");
_LIT(KPublishDevice, 		"publishdevice");
_LIT(KNotify, 				"notify");
_LIT(KCancelDiscover, 		"canceldiscovery");
_LIT(KCancelDescribe, 		"canceldescribe");
_LIT(KCancelNotify,			"cancelnotify");
_LIT(KUnKnown, 				"unknown");
_LIT(KStartScheduler,		"startscheduler");
_LIT(KStopScheduler,		"stopscheduler");
_LIT(KPrintResults,			"printresults");
_LIT(KPrintAnnouncements,	"printannouncements");
_LIT(KStartTimer,			"starttimer");
_LIT(KReadIni,				"readini");
_LIT(KInitiateControl,		"initiateaction");
_LIT(KDescribeService,		"describeservice");
_LIT(KOpenDiscoverer,		"opendiscoverer");
_LIT(KOpenPublisher,		"openpublisher");
_LIT(KDefaultFileFormat,	"%c:\\describeoutput_");
_LIT(KDefaultExtension, 	".txt");
_LIT(KCancelAll,			"cancelall");
_LIT(KCancelAllService,		"cancelallservicedescription");
_LIT(KExpctedDescriptions,	"expecteddescriptions");
_LIT(KPositiveCase,			"positivecase");
_LIT(KServiceMaxDuration, 	"serviceduration");
_LIT(KDescribeIcon, 		"describeicon");

//Constant literals -- Discovery response 
_LIT(KLocation,				"location");
_LIT(KSearchTarget,			"searchtarget");
_LIT(KServiceType,			"servicetype");
_LIT(KCacheControl,			"cachecontrol");
_LIT(KExpiryTime, 			"expirytime");
_LIT(KDate,					"date");
_LIT(KExtResponse,			"extresponse");
_LIT(KServer,				"server");
_LIT(KUsn,					"usn");
_LIT(KOkResponse,			"okresponse");
_LIT(KResponseTime, 		"responsetime");
_LIT(KUSN, 					"usn");
_LIT(KDescription, 			"description");
_LIT(KServiceDescription, 	"servicedescription");
_LIT(KDescriptionPath, 		"descriptionpath");
_LIT(KTimeOut, 				"timeout");
_LIT(KSid, 					"sid");
_LIT(KNotification,			"notification");
_LIT(KUri, 					"uri");
_LIT(KSubscriptionUri, 		"subscriptionuri");
_LIT(KDuration, 			"duration");
_LIT8(KSsdpall,				"ssdp:all");
_LIT(KOperationType,		"operationtype");
_LIT8(KHeaderValue,			"%S:%d");
_LIT(KIconRefer,			"iconRefer");
_LIT(KMimeType,			"mimetype");
_LIT(KWidth,				"width");
_LIT(KHeight,				"height");
_LIT(KDepth,				"depth");
_LIT(KUrl,					"url");

const TInt KMaxBufLength = 256;

// constants for Device Register params

_LIT(KBaseURL, "BaseURL");
_LIT(KParentDeviceUid, "ParentDeviceUid");
_LIT(KDeviceType, "DeviceType");
_LIT(KFriendlyName, "FriendlyName");
_LIT(KManufacturer, "Manufacturer");
_LIT(KManufacturerURL, "ManufacturerURL");
_LIT(KModelDescription, "ModelDescription");
_LIT(KModelName, "ModelName");
_LIT(KModelNumber, "ModelNumber");
_LIT(KModelURL, "ModelURL");
_LIT(KPresentationURL, "PresentationURL");
_LIT(KSerialNumber, "SerialNumber");
_LIT(KUDN, "UDN");
_LIT(KUPC, "UPC");
_LIT(KDescription_Path,"Description_Path %d");

//Cntrl Url
_LIT8(KStartCntrlUrl, "<controlURL>");
_LIT8(KEndCntrlUrl, "</controlURL>");
const TInt cntrlLength = 12;// 12 is the length of the <controlURL>

_LIT8(KStartScpdUrl, "<SCPDURL>");
_LIT8(KEndScpdUrl, "</SCPDURL>");
const TInt scpdLength = 9;// 9 is the length of the <SCPDURL>

//Event Url
_LIT8(KStartEventUrl, "<eventSubURL>");
_LIT8(KEndEventUrl, "</eventSubURL>");
const TInt eventLength = 13;// 9 is the length of the <eventSubURL>

_LIT8(KSlash, "/");
_LIT8(KColonSlashSlash, "://");
_LIT8(KColon, ":");


//Panics
_LIT(KDescribeTestPanicCategory,"Describe");
_LIT(KDescoveryTestPanicCategory, "Discovery");

_LIT(KExpectedURL, "expectedurl");
_LIT(KExpectedCount, "expectedcount");
	
enum TestUPnPError
	{
	//No Description path found
	KErrNoDescriptionPath 			= -1,
	KErrNODiscoveryResponse 		= -2,
	KErrNoServiceUris 				= -3,
	KErrMissingURiOrResponceTime 	= -4
	};

enum TPublisherControl
	{
	KDevice,
	KService,
	KErrNoControl = -1
	};

#endif //__CONSTANTS_H__
