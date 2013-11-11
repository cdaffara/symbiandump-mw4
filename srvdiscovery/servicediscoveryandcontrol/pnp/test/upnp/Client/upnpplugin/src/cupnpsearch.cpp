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

#include "cupnpsearch.h"

CUPnPSearch::CUPnPSearch( RSubConnection& aSubConnection )
	:iSubConnection( aSubConnection )
	{
	}

CUPnPSearch* CUPnPSearch::NewL(RSubConnection& aSubConnection)
	{
	CUPnPSearch* search = new (ELeave) CUPnPSearch( aSubConnection );
	CleanupStack::PushL( search );
	search->ConstructL();
	CleanupStack::Pop( search );
	return search;
	}

void CUPnPSearch::ConstructL()
	{
	// Create a delta timer with the given priority
	iDeltaTimer=CDeltaTimer::NewL(EPriorityLow);
	}

CUPnPSearch::~CUPnPSearch()
	{
	iDeltaTimer->Cancel();	
	delete iDeltaTimer;
	iTimerMappedArray.ResetAndDestroy();
	iCallbackArray.ResetAndDestroy();
	iMappedUri=NULL;
	}

/* Used to send a discover request to the network
 @param aServiceInfo Bundle of uri's which have to be searched for
 */
void CUPnPSearch::SubmitRequestL(const RPnPParameterBundle& aServiceInfo)
	{
	const TInt KMaxMXValue = 120;
	_LIT8(KSsdpAll,"ssdp:all");	
	iObserver = aServiceInfo.PnPObserver();
	
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).FindFamily( EUPnPDiscoverRequestParamSet );
	// Iterate through all the param sets in the passed bundle
	
	CUPnPDiscoverRequestParamSet* paramSet = NULL;
	TUint count = family.CountParameterSets ( RParameterFamily::ERequested ) ;
	
	for ( TInt paramIndex = count - 1; paramIndex >= 0; paramIndex-- )
		{
		paramSet = static_cast<CUPnPDiscoverRequestParamSet*> ( family.GetParameterSetAtIndex ( paramIndex, RParameterFamily::ERequested ) );

		//Extract the MX value and the uri
		TInt delayResp=paramSet->ResponseTime();
		const TDesC8& uri = paramSet->Uri();
		__ASSERT_DEBUG( uri != KNullDesC8, User::Panic( KUPnPInvalidUserInput, KUPnPErrInvalidInput ) );
		__ASSERT_DEBUG( delayResp>=0, User::Panic( KUPnPInvalidUserInput, KUPnPErrInvalidInput ) );
		if ( delayResp > KMaxMXValue )
			{
			paramSet->SetResponseTime(KMaxMXValue);
			delayResp = KMaxMXValue;
			}
		/* If one of the uris being discovered is ssdp:all then no other uri
		 needs to be discovered and hence no more traversal through the param
		 bundle is required */
		if ( paramSet->Uri().CompareF(KSsdpAll) == NULL)
			{
			CreateTimerEntryL(uri);
			iTimerMappedArray.ResetAndDestroy();
			iMappedUri = CTimerMappedUri::NewL(uri,iEntry,delayResp);
			iTimerMappedArray.AppendL(iMappedUri);
			if ( family.CountParameterSets(RParameterFamily::ERequested) > 1 )
				{
				for ( TInt i = family.CountParameterSets(RParameterFamily::ERequested)-1; i >= 0; i-- )
					{
					if ( i != paramIndex )						
					    reinterpret_cast<RUPnPParameterFamily*>(&family)->DeleteParamSetL ( i );

					}
				}

			break;
			}
		// Check for validity of the uri
		User::LeaveIfError(CUPnPUtils::GenericUriValidity( uri, CUPnPUtils::EAny ));
		 

		// If uri has already been indexed into iTimerMappedArray that means its a
		// duplicate entry and the index of this param set is appended to
		// tempDuplicateArray
		TInt err = KErrNone;
		for (TInt i=0;i<iTimerMappedArray.Count();i++)
			{
			if(iTimerMappedArray[i]->UriExists(uri))
				{
				reinterpret_cast<RUPnPParameterFamily*>(&family)->DeleteParamSetL(paramIndex);
				err = KErrAlreadyExists;
				break;
				}
			}
		if (err == KErrAlreadyExists)
			continue;

		// CreateTimerEntry sets iEntry to the new timer entry
		CreateTimerEntryL(uri);
		iMappedUri = CTimerMappedUri::NewL(uri,iEntry,delayResp);
		iTimerMappedArray.AppendL(iMappedUri);
		
		}

	/* Transfer ownvership of all CTimerMappedUri objects from tempMappedArray to
	  iTimerMappedArray
	  */
	for(TInt i=0;i<iTimerMappedArray.Count();i++)
		{
		// queue up in deltatimer
		TInt time = AdjustedDelay(iTimerMappedArray[i]->Delay());
		TTimeIntervalMicroSeconds32 delay(time);
		iDeltaTimer->Queue(delay,iTimerMappedArray[i]->GetTimerEntry());
		}
	/* if SetParameters fails should it be communicated to client
	* so that he may take corrective measures */
	User::LeaveIfError(iSubConnection.SetParameters(aServiceInfo));
	SetDiscoverProgress(ETrue);
	}

/* Static method which is called on timer expiry and which removes the uri entry
 * from the array.
 */
TInt CUPnPSearch::StopSearch(TAny* aPtr)
	{
	CCallbackArgument* pointerData= static_cast<CCallbackArgument*> (aPtr);
	CUPnPSearch* thisObject= static_cast < CUPnPSearch*> (pointerData->GetThisPointer());
	RPointerArray<CTimerMappedUri>& rArray=thisObject->GetTimerArray();
	RPointerArray<CCallbackArgument>& argArray = thisObject->GetArgumentArray();
	const TDesC8& uri = pointerData->GetUri();
	TInt index;
	for(index=0;index<rArray.Count();index++)
		{
		if(rArray[index]->UriExists(uri))
			{
			// Removes the uri entry from the array
			CTimerMappedUri* timerUri = rArray[index];
			rArray.Remove(index);
			delete timerUri;
			break;
			}
		}
	for( index=0;index<argArray.Count();index++)
		{
		if(uri.CompareF(argArray[index]->GetUri()) == NULL)
			{
			argArray.Remove(index);
			break;
			}
		}
	// Requests the subconnection to cancel ongoing search
	TRAPD( error , thisObject->CancelSearchL( uri ));

	// Reset Discover In progress
	if(rArray.Count() <= 0)
		thisObject->SetDiscoverProgress(EFalse);
	
	// Notifies the client that timer has expired
	TRAP(error,thisObject->MakeBundleAndNotifyL(pointerData));
	delete pointerData;
	return error;
	}

/* Used to notify results back to the client application */
void CUPnPSearch::NotifyResultsL(RPnPParameterBundleBase& aBundle)
	{
	RParameterFamily family = aBundle.FindFamily(EUPnPDiscoverResponseParamSet);	
 	__ASSERT_DEBUG(!family.IsNull (), User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));	
	__ASSERT_DEBUG(family.CountParameterSets(RParameterFamily::ERequested) > 0, User::Panic(KUPnPNoParamSet,KUPnPErrNoParamSet));
	iObserver->OnPnPEventL(aBundle) ;
	}

/* Is used to notify the timeout expired result back to the control point */
void CUPnPSearch::MakeBundleAndNotifyL(CCallbackArgument* aCallArgument)
	{
	RPnPParameterBundleBase pnpBundle ;
	pnpBundle.Open();	
	CleanupClosePushL( pnpBundle );

	// Create the family on the bundle
	RParameterFamily family = pnpBundle.CreateFamilyL(EUPnPDiscoverResponseParamSet);	
	//Set the values in the parameter set
	CUPnPDiscoverResponseParamSet* searchParamSet = CUPnPDiscoverResponseParamSet::NewL(family );

	TUPnPEvent event;
	const TDesC8& uri = aCallArgument->GetUri();
	event.iStatus=TUPnPEvent::EDiscoveryTimeout;
	event.iErrorCode = KErrTimedOut;
	searchParamSet->SetUPnPEvent(event);
	searchParamSet->SetUriL ( uri );	
	CleanupStack::Pop(&pnpBundle);
	iObserver->OnPnPEventL( pnpBundle );
	}

/* Used to send a Cancel discover request to the network in case of timer expiry
 Sent automatically by CUPnPSearch::StopSearch */
void CUPnPSearch::CancelSearchL(const TDesC8& aUri)
	{	
	RPnPParameterBundleBase rBundle;
	rBundle.Open();
	CleanupClosePushL( rBundle );
	RParameterFamily family = rBundle.CreateFamilyL(EUPnPCancelDiscoverParamSet);	
	
	//Allocate the param set on the family
	CUPnPCancelDiscoverParamSet* paramSet = CUPnPCancelDiscoverParamSet::NewL(family );

	// Set the values in the parameter set
	paramSet->SetUriL( aUri );
	// Finally send cancel request to the subconnection
	User::LeaveIfError( iSubConnection.SetParameters(rBundle ));
	CleanupStack::PopAndDestroy( &rBundle );
	}

/* Used to send a Cancel discover request  to the network. This is called on client
 invocation
 @param aServiceInfo Bundle containing list of uri's whose discovery has to be cancelled
 */
void CUPnPSearch::CancelSearchL( const RPnPParameterBundle& aServiceInfo )
	{	
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).FindFamily(EUPnPCancelDiscoverParamSet);
	
	/* The uris passed in the bundle are looked up in iTimerMappedUri.If they
	 exist, then its entry is removed from DeltaTimer and iTimerMappedUri as well
	 */
	TInt err = 0;
	// Iterate through all the param sets in the passed bundle
	
	TUint count = family.CountParameterSets ( RParameterFamily::ERequested ) ;
	CUPnPCancelDiscoverParamSet* paramSet = NULL;
	for ( TInt paramIndex = count - 1; paramIndex >= 0; paramIndex-- )
		{
		paramSet = static_cast<CUPnPCancelDiscoverParamSet*> ( family.GetParameterSetAtIndex ( paramIndex, RParameterFamily::ERequested) );
		
		//Extract the uri
		const TDesC8& uri = paramSet->Uri();
		__ASSERT_DEBUG( paramSet->Uri() != KNullDesC8, User::Panic( KUPnPInvalidUserInput, KUPnPErrInvalidInput ) );
		// Removes the uri entry from the array if exists
		err=KErrNotFound;
		for(TInt i=0;i<iTimerMappedArray.Count();i++)
			{
			if(iTimerMappedArray[i]->UriExists(uri))
				{
				err = KErrNone;
				CTimerMappedUri* timerUri = iTimerMappedArray[i];
				iDeltaTimer->Remove(timerUri->GetTimerEntry());
				iTimerMappedArray.Remove(i);
				if(iTimerMappedArray.Count()<= 0)
					SetDiscoverProgress(EFalse);
				delete timerUri;
				break;
				}
			}
		if ( err != KErrNone )
			{			
			reinterpret_cast<RUPnPParameterFamily*>(&family)->DeleteParamSetL ( paramIndex );
			continue;
			}
		}
	if ( family.CountParameterSets(RParameterFamily::ERequested)>= 1)
			User::LeaveIfError(iSubConnection.SetParameters(aServiceInfo));
	}

/* CreateTimerEntry : creates the deltatimer entry and sets it to iEntry
 @param aUri  The uri whose timer entry has to be created
 */
void CUPnPSearch::CreateTimerEntryL(const TDesC8& aUri )
	{
	CCallbackArgument* callArgument =CCallbackArgument::NewL(this,aUri);
	CleanupStack::PushL(callArgument);
	iCallbackArray.AppendL(callArgument);
	TCallBack callBack(StopSearch,callArgument);
	iEntry = new (ELeave) TDeltaTimerEntry ( callBack );
	CleanupStack::Pop();
	}
