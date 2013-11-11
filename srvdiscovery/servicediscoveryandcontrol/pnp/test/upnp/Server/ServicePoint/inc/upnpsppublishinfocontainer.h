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


#ifndef __UPNPSPPUBLISHINFOCONTAINER_H_
#define __UPNPSPPUBLISHINFOCONTAINER_H_

#include "cpnpdeviceparam.h"
#include "cstringpoolmanager.h"


class CUPnPRootDeviceInfoContainer : public CBase
	{
public:		
	static CUPnPRootDeviceInfoContainer* NewL ( );
	~CUPnPRootDeviceInfoContainer ( );
	
	void AppendServiceInfoL ( const TDesC8& aParentUuid, CUPnPServiceInfo* aServiceInfo );
	void AppendEmbeddedDeviceL ( const TDesC8& aParentUuid, CUPnPDevice* aDevice );
	
	void DeleteServiceInfoL ( const TDesC8& aScpdUrl );
	void DeleteDeviceInfoL ( const TDesC8& aUuid );
	
	CUPnPDeviceDescription& GetRootDeviceDesciption ( ) const;
	const TDesC8& GetRootdeviceUid() const;
	CStringPoolManager& StringPoolManager ( ) const;
	CUPnPDevice* GetDeviceByUuid ( const TDesC8& aDeviceUuid );
	
	TInt PerformValidation ( const TDesC8& aDeviceUid, const TDesC8& aServiceType );
	
private:
	CUPnPRootDeviceInfoContainer ( );
    void ConstructL ( );
    TBool AppendServiceL ( const TDesC8& aParentUuid, const CUPnPServiceInfo* aServiceInfo, CUPnPDevice* aDevice );
	TBool DeleteServiceL ( const TDesC8& aScpdUrl, CUPnPDevice* aDevice );
	TBool DeleteDeviceL ( const TDesC8& aUuid, CUPnPDevice* aDevice );
	const CUPnPDevice* SearchDevice ( const CUPnPDevice* aDevice, const TDesC8& aDeviceUuid );

private:
	CUPnPDeviceDescription*		iRootDeviceDescription;
    CStringPoolManager* 		iStringPoolMgr;
    };


	
#endif /*UPNPSPPUBLISHINFOCONTAINER_H_*/
