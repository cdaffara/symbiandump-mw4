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

#ifndef __CUPNPPUBLISHER_H_
#define __CUPNPPUBLISHER_H_
#include <e32math.h>
#include <upnpparamset.h>
#include <pnpparameterbundle.h>
#include <mpnpobserver.h>

const TInt KMicroTime = 1000000;
/* Used to send publish advertisements to the network.Also periodic refreshment
   of these advertisements is done
   @internalComponent
 */
class CUPnPPublisher : public CBase
	{
public:
	static CUPnPPublisher* NewL ( RSubConnection& aSubConnection );
	~CUPnPPublisher();
	void SubmitRequestL( const RPnPParameterBundle& aServiceInfo );
	void NotifyResultsL(RPnPParameterBundleBase& aBundle);
	
public:
	inline TBool IsService();

private:
	CUPnPPublisher(RSubConnection& aSubConnection );
	static TInt AdvertisementRefresh(TAny* aPtr);
	inline TUint64 CalculatePeriod (TInt aTime);
	inline RSubConnection& SubConnection();
	void ConstructL();
	void ValidateOtherFieldsL(const  CUPnPDeviceRegisterParamSet& aDeviceParam);
	void ValidateIconList(const  CUPnPDeviceRegisterParamSet& aDeviceParam);
	void CopyWithIconInfoL ( const CUPnPDeviceRegisterParamSet& aDeviceParam, RPnPParameterBundleBase& aBundle );
	void StoreIconListL ( const CUPnPIconList& aIconList, RBuf8& aBuffer );
	
	MPnPObserver* iObserver;
	RSubConnection& iSubConnection;
	CPeriodic* iPeriodTimer;
	TBool iService;
	TInt iPeriod;
	TInt iTime;
	};

/* Used to calculate the period of refeshing the advertisement given the cache time
 For convenience, the period is kept at 1/3rd of the cache time
 @param  aTime	The cache control time
 @return The period time
 */
TUint64 CUPnPPublisher::CalculatePeriod(TInt aTime)
	{
	const TUint64 divisor = 3;
	TUint64 temp; // Placeholder
	TUint64 q = Math::UDivMod64(aTime,divisor,temp);
	return q;
	}

RSubConnection& CUPnPPublisher::SubConnection()
	{
	return iSubConnection;
	}

TBool CUPnPPublisher::IsService()
	{
	return iService;
	}

#endif /*CUPNPPUBLISHER_H_*/
