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

#include <e32base.h>
#include <comms-infras/metabuffer.h>
#include "upnpparamsextfactory.h"
#include "upnpparamset.h"

START_ATTRIBUTE_TABLE(CUPnPRequestParamSetBase, KUPnPParamsFactoryUid, EUPnPRequestParamSet)
	REGISTER_ATTRIBUTE(CUPnPRequestParamSetBase, iUri, TMetaBuf8)	
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE(CUPnPResponseParamSetBase, KUPnPParamsFactoryUid, EUPnPResponseParamSet)
	REGISTER_ATTRIBUTE(CUPnPResponseParamSetBase, iUPnPEvent, TMetaNumber)
END_ATTRIBUTE_TABLE_BASE(CUPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPRegisterAnnouncementParamSet, KUPnPParamsFactoryUid, EUPnPRegisterAnnouncementParamSet)
END_ATTRIBUTE_TABLE_BASE(CUPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPAbsenceAnnouncementParamSet, KUPnPParamsFactoryUid, EUPnPAbsenceAnnouncementParamSet)
	REGISTER_ATTRIBUTE(CUPnPAbsenceAnnouncementParamSet, iUSN, TMetaBuf8)
END_ATTRIBUTE_TABLE_BASE(CUPnPResponseParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPPresenceAnnouncementParamSet, KUPnPParamsFactoryUid, EUPnPPresenceAnnouncementParamSet)
	REGISTER_ATTRIBUTE(CUPnPPresenceAnnouncementParamSet, iExpiration, TMetaNumber)
	REGISTER_ATTRIBUTE(CUPnPPresenceAnnouncementParamSet, iLocation, TMetaBuf8)
END_ATTRIBUTE_TABLE_BASE(CUPnPAbsenceAnnouncementParamSet, 0)

START_ATTRIBUTE_TABLE(CUPnPDiscoverRequestParamSet, KUPnPParamsFactoryUid, EUPnPDiscoverRequestParamSet)
	REGISTER_ATTRIBUTE(CUPnPDiscoverRequestParamSet, iTimeToDelayResponse, TMetaNumber)
END_ATTRIBUTE_TABLE_BASE(CUPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPDiscoverResponseParamSet, KUPnPParamsFactoryUid, EUPnPDiscoverResponseParamSet)
	REGISTER_ATTRIBUTE(CUPnPDiscoverResponseParamSet, iExpiration, TMetaNumber)	
	REGISTER_ATTRIBUTE(CUPnPDiscoverResponseParamSet, iDeviceLocation, TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPDiscoverResponseParamSet, iUSN, TMetaBuf8)
END_ATTRIBUTE_TABLE_BASE(CUPnPResponseParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPDescribeRequestParamSet, KUPnPParamsFactoryUid, EUPnPDescribeRequestParamSet)
	REGISTER_ATTRIBUTE(CUPnPDescribeRequestParamSet, iHostAddr, TMeta<TAppProtAddr>)
END_ATTRIBUTE_TABLE_BASE(CUPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPDescribeResponseParamSet, KUPnPParamsFactoryUid, EUPnPDescribeResponseParamSet)
	REGISTER_ATTRIBUTE(CUPnPDescribeResponseParamSet, iDescription, TMetaBuf8)
END_ATTRIBUTE_TABLE_BASE(CUPnPResponseParamSetBase, EUPnPResponseParamSet)

START_ATTRIBUTE_TABLE(CUPnPSubscribeRequestParamSet, KUPnPParamsFactoryUid, EUPnPSubscribeRequestParamSet)
	REGISTER_ATTRIBUTE(CUPnPSubscribeRequestParamSet, iTimeoutSeconds, TMetaNumber)
	REGISTER_ATTRIBUTE(CUPnPSubscribeRequestParamSet, iRemoteHost, TMeta<TAppProtAddr>)
END_ATTRIBUTE_TABLE_BASE(CUPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPSubscribeResponseParamSet, KUPnPParamsFactoryUid, EUPnPSubscribeResponseParamSet)
	REGISTER_ATTRIBUTE(CUPnPSubscribeResponseParamSet, iTimeoutSeconds, TMetaNumber)	
END_ATTRIBUTE_TABLE_BASE(CUPnPResponseParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPNotifyEventParamSet, KUPnPParamsFactoryUid, EUPnPNotifyEventParamSet)		
	REGISTER_ATTRIBUTE(CUPnPNotifyEventParamSet, iNotifyData, TMetaBuf8)
END_ATTRIBUTE_TABLE_BASE(CUPnPResponseParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPCancelDiscoverParamSet, KUPnPParamsFactoryUid, EUPnPCancelDiscoverParamSet)		
END_ATTRIBUTE_TABLE_BASE(CUPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPCancelDescribeParamSet, KUPnPParamsFactoryUid, EUPnPCancelDescribeParamSet)		
END_ATTRIBUTE_TABLE_BASE(CUPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPCancelSubscribeParamSet, KUPnPParamsFactoryUid, EUPnPCancelSubscribeParamSet)		
END_ATTRIBUTE_TABLE_BASE(CUPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPCancelRegisterNotifyParamSet, KUPnPParamsFactoryUid, EUPnPCancelRegisterNotifyParamSet)		
END_ATTRIBUTE_TABLE_BASE(CUPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPServiceRegisterParamSet, KUPnPParamsFactoryUid, EUPnPServiceRegisterParamSet)
	REGISTER_ATTRIBUTE(CUPnPServiceRegisterParamSet, iDeviceUid, TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPServiceRegisterParamSet, iCacheControl, TMetaNumber)
	REGISTER_ATTRIBUTE(CUPnPServiceRegisterParamSet, iServiceDescription, TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPServiceRegisterParamSet, iInitialEventMessage, TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPServiceRegisterParamSet, iTimeOut, TMetaNumber)
END_ATTRIBUTE_TABLE_BASE(CUPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPDeviceRegisterParamSetBase, KUPnPParamsFactoryUid, EUPnPDeviceRegisterParamSetBase)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetBase, iCacheControl, TMetaNumber)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetBase, iParentDeviceUid, TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetBase, iFriendlyName , TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetBase, iManufacturer , TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetBase, iManufacturerURL , TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetBase, iModelDescription , TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetBase, iModelName , TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetBase, iModelNumber , TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetBase, iModelURL , TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetBase, iPresentationURL , TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetBase, iSerialNumber , TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetBase, iUDN , TMetaBuf8)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetBase, iUPC , TMetaBuf8)	
END_ATTRIBUTE_TABLE_BASE(CUPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPDeviceRegisterParamSet, KUPnPParamsFactoryUid, EUPnPDeviceRegisterParamSet)
END_ATTRIBUTE_TABLE_BASE(CUPnPDeviceRegisterParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPDeviceRegisterParamSetExt, KUPnPParamsFactoryUid, EUPnPDeviceRegisterParamSetExt)
	REGISTER_ATTRIBUTE(CUPnPDeviceRegisterParamSetExt, iIconData, TMetaBuf8)
END_ATTRIBUTE_TABLE_BASE(CUPnPDeviceRegisterParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPServiceNotificationParamSet, KUPnPParamsFactoryUid, EUPnPServiceNotificationParamSet)	
	REGISTER_ATTRIBUTE(CUPnPServiceNotificationParamSet, iNotifyData, TMetaBuf8)	
END_ATTRIBUTE_TABLE_BASE(CUPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CUPnPPublishResponseParamSet, KUPnPParamsFactoryUid, EUPnPPublishResponseParamSet)		
END_ATTRIBUTE_TABLE_BASE(CUPnPResponseParamSetBase, 0)


CUPnPParamSetBase* CUPnPParamsExtFactory::NewL ( TAny* aConstructionParams )
	{
	//TUPnPParamSetFamily familyType = REINTERPRET_CAST ( TUPnPParamSetFamily, *aConstructionParams );	
	const TInt32 familyType = reinterpret_cast<TInt32>(aConstructionParams);
	CUPnPParamSetBase* myUPnPParamBase = NULL;
	switch ( familyType )
		{
		case EUPnPRegisterAnnouncementParamSet:		
			myUPnPParamBase = new (ELeave) CUPnPRegisterAnnouncementParamSet;
		break;

		case EUPnPAbsenceAnnouncementParamSet:
			myUPnPParamBase = new (ELeave) CUPnPAbsenceAnnouncementParamSet;
		break;

		case EUPnPPresenceAnnouncementParamSet:
			myUPnPParamBase = new (ELeave) CUPnPPresenceAnnouncementParamSet;
		break;

		case EUPnPDiscoverRequestParamSet:
			myUPnPParamBase = new (ELeave) CUPnPDiscoverRequestParamSet;
		break;

		case EUPnPDiscoverResponseParamSet:
			myUPnPParamBase = new (ELeave) CUPnPDiscoverResponseParamSet;
		break;

		case EUPnPDescribeRequestParamSet:
			myUPnPParamBase = new (ELeave) CUPnPDescribeRequestParamSet;
		break;

		case EUPnPDescribeResponseParamSet:
			myUPnPParamBase = new (ELeave) CUPnPDescribeResponseParamSet;
		break;
				
		case EUPnPSubscribeRequestParamSet:
			myUPnPParamBase = new (ELeave) CUPnPSubscribeRequestParamSet;
		break;

		case EUPnPSubscribeResponseParamSet:
			myUPnPParamBase = new (ELeave) CUPnPSubscribeResponseParamSet;
		break;

		case EUPnPNotifyEventParamSet:
			myUPnPParamBase = new (ELeave) CUPnPNotifyEventParamSet;
		break;


		case EUPnPCancelDiscoverParamSet:
			myUPnPParamBase = new (ELeave) CUPnPCancelDiscoverParamSet;
		break;
		
		case EUPnPCancelDescribeParamSet:
			myUPnPParamBase = new (ELeave) CUPnPCancelDescribeParamSet;
		break;
		
		case EUPnPCancelSubscribeParamSet:
			myUPnPParamBase = new (ELeave) CUPnPCancelSubscribeParamSet;
		break;
		
		case EUPnPCancelRegisterNotifyParamSet:
			myUPnPParamBase = new (ELeave) CUPnPCancelRegisterNotifyParamSet;
		break;
		
		case EUPnPServiceRegisterParamSet:
			myUPnPParamBase = new (ELeave) CUPnPServiceRegisterParamSet;
		break;
		
		case EUPnPDeviceRegisterParamSet:
			myUPnPParamBase = new (ELeave) CUPnPDeviceRegisterParamSet;
		break;
		
		case EUPnPServiceNotificationParamSet:
			myUPnPParamBase = new (ELeave) CUPnPServiceNotificationParamSet;
		break;
				
		case EUPnPPublishResponseParamSet:
			myUPnPParamBase	= new (ELeave) CUPnPPublishResponseParamSet;
		break;
		
		case EUPnPDeviceRegisterParamSetExt:
			myUPnPParamBase = new (ELeave) CUPnPDeviceRegisterParamSetExt;
		break;
		
		default:
			ASSERT(0);
		break;
		}
	return myUPnPParamBase;
	}

