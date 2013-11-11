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
#include "testpnpparamsextfactory.h"
#include "testpnpparamset.h"

START_ATTRIBUTE_TABLE(CTestPnPRequestParamSetBase, KTestPnPParamsFactoryUid, ETestPnPRequestParamSet)	
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE(CTestPnPResponseParamSetBase, KTestPnPParamsFactoryUid, ETestPnPResponseParamSet)
END_ATTRIBUTE_TABLE_BASE(CTestPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CTestPnPRegisterAnnouncementParamSet, KTestPnPParamsFactoryUid, ETestPnPRegisterAnnouncementParamSet)
END_ATTRIBUTE_TABLE_BASE(CTestPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CTestPnPAbsenceAnnouncementParamSet, KTestPnPParamsFactoryUid, ETestPnPAbsenceAnnouncementParamSet)
END_ATTRIBUTE_TABLE_BASE(CTestPnPResponseParamSetBase, 0)

START_ATTRIBUTE_TABLE(CTestPnPPresenceAnnouncementParamSet, KTestPnPParamsFactoryUid, ETestPnPPresenceAnnouncementParamSet)
END_ATTRIBUTE_TABLE_BASE(CTestPnPAbsenceAnnouncementParamSet, 0)


START_ATTRIBUTE_TABLE(CTestPnPDiscoverRequestParamSet, KTestPnPParamsFactoryUid, ETestPnPDiscoverRequestParamSet)
END_ATTRIBUTE_TABLE_BASE(CTestPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CTestPnPDiscoverResponseParamSet, KTestPnPParamsFactoryUid, ETestPnPDiscoverResponseParamSet)
END_ATTRIBUTE_TABLE_BASE(CTestPnPResponseParamSetBase, 0)

START_ATTRIBUTE_TABLE(CTestPnPDescribeRequestParamSet, KTestPnPParamsFactoryUid, ETestPnPDescribeRequestParamSet)
END_ATTRIBUTE_TABLE_BASE(CTestPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CTestPnPDescribeResponseParamSet, KTestPnPParamsFactoryUid, ETestPnPDescribeResponseParamSet)
END_ATTRIBUTE_TABLE_BASE(CTestPnPResponseParamSetBase, ETestPnPResponseParamSet)

START_ATTRIBUTE_TABLE(CTestPnPSubscribeRequestParamSet, KTestPnPParamsFactoryUid, ETestPnPSubscribeRequestParamSet)
END_ATTRIBUTE_TABLE_BASE(CTestPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CTestPnPSubscribeResponseParamSet, KTestPnPParamsFactoryUid, ETestPnPSubscribeResponseParamSet)
END_ATTRIBUTE_TABLE_BASE(CTestPnPResponseParamSetBase, 0)

START_ATTRIBUTE_TABLE(CTestPnPNotifyEventParamSet, KTestPnPParamsFactoryUid, ETestPnPNotifyEventParamSet)		
END_ATTRIBUTE_TABLE_BASE(CTestPnPResponseParamSetBase, 0)

START_ATTRIBUTE_TABLE(CTestPnPCancelDiscoverParamSet, KTestPnPParamsFactoryUid, ETestPnPCancelDiscoverParamSet)		
END_ATTRIBUTE_TABLE_BASE(CTestPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CTestPnPCancelDescribeParamSet, KTestPnPParamsFactoryUid, ETestPnPCancelDescribeParamSet)		
END_ATTRIBUTE_TABLE_BASE(CTestPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CTestPnPCancelSubscribeParamSet, KTestPnPParamsFactoryUid, ETestPnPCancelSubscribeParamSet)		
END_ATTRIBUTE_TABLE_BASE(CTestPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CTestPnPCancelRegisterNotifyParamSet, KTestPnPParamsFactoryUid, ETestPnPCancelRegisterNotifyParamSet)		
END_ATTRIBUTE_TABLE_BASE(CTestPnPRequestParamSetBase, 0)

START_ATTRIBUTE_TABLE(CTestPnPServiceRegisterParamSet, KTestPnPParamsFactoryUid, ETestPnPServiceRegisterParamSet)
END_ATTRIBUTE_TABLE_BASE(CTestPnPRequestParamSetBase, 0)


START_ATTRIBUTE_TABLE(CTestPnPPublishResponseParamSet, KTestPnPParamsFactoryUid, ETestPnPPublishResponseParamSet)		
END_ATTRIBUTE_TABLE_BASE(CTestPnPResponseParamSetBase, 0)


CTestPnPParamSetBase* CTestPnPParamsExtFactory::NewL ( TAny* aConstructionParams )
	{
	const TInt32 familyType = reinterpret_cast<TInt32>(aConstructionParams);
	CTestPnPParamSetBase* myTestPnPParamBase = NULL;
	switch ( familyType )
		{
		case ETestPnPRegisterAnnouncementParamSet:		
			myTestPnPParamBase = new (ELeave) CTestPnPRegisterAnnouncementParamSet;
		break;

		case ETestPnPPresenceAnnouncementParamSet:
			myTestPnPParamBase = new (ELeave) CTestPnPPresenceAnnouncementParamSet;
		break;

		case ETestPnPDiscoverRequestParamSet:
			myTestPnPParamBase = new (ELeave) CTestPnPDiscoverRequestParamSet;
		break;

		case ETestPnPDiscoverResponseParamSet:
			myTestPnPParamBase = new (ELeave) CTestPnPDiscoverResponseParamSet;
		break;

		case ETestPnPDescribeRequestParamSet:
			myTestPnPParamBase = new (ELeave) CTestPnPDescribeRequestParamSet;
		break;

		case ETestPnPDescribeResponseParamSet:
			myTestPnPParamBase = new (ELeave) CTestPnPDescribeResponseParamSet;
		break;
				
		case ETestPnPSubscribeRequestParamSet:
			myTestPnPParamBase = new (ELeave) CTestPnPSubscribeRequestParamSet;
		break;

		case ETestPnPSubscribeResponseParamSet:
			myTestPnPParamBase = new (ELeave) CTestPnPSubscribeResponseParamSet;
		break;

		case ETestPnPNotifyEventParamSet:
			myTestPnPParamBase = new (ELeave) CTestPnPNotifyEventParamSet;
		break;


		case ETestPnPCancelDiscoverParamSet:
			myTestPnPParamBase = new (ELeave) CTestPnPCancelDiscoverParamSet;
		break;
		
		case ETestPnPCancelDescribeParamSet:
			myTestPnPParamBase = new (ELeave) CTestPnPCancelDescribeParamSet;
		break;
		
		case ETestPnPCancelSubscribeParamSet:
			myTestPnPParamBase = new (ELeave) CTestPnPCancelSubscribeParamSet;
		break;
		
		case ETestPnPCancelRegisterNotifyParamSet:
			myTestPnPParamBase = new (ELeave) CTestPnPCancelRegisterNotifyParamSet;
		break;
		
		case ETestPnPServiceRegisterParamSet:
			myTestPnPParamBase = new (ELeave) CTestPnPServiceRegisterParamSet;
		break;
				
		case ETestPnPPublishResponseParamSet:
			myTestPnPParamBase	= new (ELeave) CTestPnPPublishResponseParamSet;
		break;

		default:
			ASSERT(0);
		break;
		}
	return myTestPnPParamBase;
	}

