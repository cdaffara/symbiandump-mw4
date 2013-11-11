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


#ifndef __CUPNPSUBSCRIBE_H_
#define __CUPNPSUBSCRIBE_H_

#include <e32base.h>
#include <e32math.h>
#include <pnpparameterbundle.h>

#include "cupnpelementarray.h"
#include "ccallbackargument.h"
#include "ctimermappeduri.h"

/* This class is used to send subscribe requests to the service point.Also the
   responses are conveyed back to the client code.
   @internalComponent
 */
class CUPnPSubscribe: public CBase
	{
public:
	static CUPnPSubscribe* NewL( RSubConnection& aSubConnection , RHostResolver& aResolver);
	void SubmitRequestL( const RPnPParameterBundle& aServiceInfo );
	void NotifyResultsL(TUint32 aFamilyId, RPnPParameterBundleBase& aBundle);
	void CancelSubscribeL( const RPnPParameterBundle& aServiceInfo);
	~CUPnPSubscribe();

private:
	void ConstructL();
	CUPnPSubscribe ( RSubConnection& aSubConnection , RHostResolver& aResolver);
	static TInt OnTimerExpiry( TAny* aPtr );
	void CreateTimerEntryL(const TDesC8& aUri ,TInt aExpiryTime);
	TInt AdjustedDelay(TInt aTime);
	void MakeBundleAndNotifyL(MPnPObserver* aObserver, HBufC8* aUri);

	inline CUPnPElementArray* ElementArrayHandle();
	inline RPointerArray<CTimerMappedUri>& GetTimerArray();
	inline RPointerArray<CCallbackArgument>& GetArgumentArray();

private:
	RSubConnection& iSubConnection;
	CUPnPElementArray* iElementArray;
	RPointerArray<CTimerMappedUri> 		iTimerMappedArray;
	RPointerArray<CCallbackArgument> 	iCallbackArray;
	
	
	TCallBack iCallBack;
	TDeltaTimerEntry* iTimerEntry;
	CDeltaTimer* iDeltaTimer;
	RHostResolver&  iResolver;	
	CTimerMappedUri* iMappedUri;
	TBool iDuplicateStatus;
	
	};

CUPnPElementArray* CUPnPSubscribe::ElementArrayHandle()
	{
	return iElementArray;
	}

/* inline method to return the RPointerArray */
RPointerArray<CTimerMappedUri>& CUPnPSubscribe::GetTimerArray()
	{
	return iTimerMappedArray;
	}
RPointerArray<CCallbackArgument>& CUPnPSubscribe::GetArgumentArray()
	{
	return iCallbackArray;
	}

#endif /*CUPNPSUBSCRIPTION_H_*/
