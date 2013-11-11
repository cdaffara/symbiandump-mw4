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

#ifndef __CUPNPSEARCH_H_
#define __CUPNPSEARCH_H_

#include <e32base.h>
#include <mpnpobserver.h>
#include <upnp/upnpconstants.h>

#include "pnputils.h"
#include "ccallbackargument.h"
#include "ctimermappeduri.h"

/* Used to send discover requests for service points in the network
 @internalComponent
 */
class CUPnPSearch: public CBase
{
public:
	static CUPnPSearch* NewL( RSubConnection& aSubConnection );
	void SubmitRequestL( const RPnPParameterBundle& aServiceInfo );
	void NotifyResultsL( RPnPParameterBundleBase& aBundle );
	void CancelSearchL( const RPnPParameterBundle& aServiceInfo );
	inline TBool DiscoverInProgress();
	~CUPnPSearch();

private:
	CUPnPSearch( RSubConnection& aSubConnection );
	void ConstructL();
	void CancelSearchL(const TDesC8& aUri);
	void CreateTimerEntryL( const TDesC8& aUri );
	void MakeBundleAndNotifyL(CCallbackArgument* aCallArgument);
	static TInt StopSearch( TAny* aPtr );

	inline TInt AdjustedDelay(TInt aTime);
	inline RPointerArray<CTimerMappedUri>& GetTimerArray();
	inline MPnPObserver* Observer();
	inline RPointerArray<CCallbackArgument>& GetArgumentArray();
	inline void SetDiscoverProgress( TBool aValue);
	CTimerMappedUri* iMappedUri;
	RSubConnection& iSubConnection;
	CDeltaTimer* iDeltaTimer;
	MPnPObserver* iObserver;
	RPointerArray<CTimerMappedUri> 		iTimerMappedArray;
	RPointerArray<CCallbackArgument> 	iCallbackArray;
	TDeltaTimerEntry* iEntry;
	TBool iIsSearching;
};

/* Used to calculate the time in microseconds to set the timer entry.Also a little
 of time is used extra to account for Intra process communication
 @param aTime	The time for which the timer has to be set in seconds
 @return The actual time in microseconds
 */
TInt CUPnPSearch::AdjustedDelay( TInt aTime )
	{
	const TUint KIntTime = 1000000 ;
	const TUint KInternalIPCLag = 1;
	aTime = ( aTime + KInternalIPCLag ) * KIntTime ;
	return aTime;
	}

MPnPObserver* CUPnPSearch::Observer()
	{
	return iObserver;
	}

/* inline method to return the RPointerArray */
RPointerArray<CTimerMappedUri>& CUPnPSearch::GetTimerArray()
	{
	return iTimerMappedArray;
	}
RPointerArray<CCallbackArgument>& CUPnPSearch::GetArgumentArray()
	{
	return iCallbackArray;
	}

TBool CUPnPSearch::DiscoverInProgress()
	{
	return iIsSearching;
	}
void CUPnPSearch::SetDiscoverProgress( TBool aValue)
	{
	iIsSearching = aValue;
	}
#endif /*CUPNPSEARCH_H_*/
