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

#include "cupnpsubscribe.h"
#include "upnpconstants.h"
#include "pnputils.h"
#include <e32const.h>

CUPnPSubscribe::CUPnPSubscribe(RSubConnection& aSubConnection , RHostResolver& aResolver):iSubConnection(aSubConnection)
	,iResolver ( aResolver )
	{
	}

CUPnPSubscribe* CUPnPSubscribe::NewL(RSubConnection& aSubConnection , RHostResolver& aResolver)
	{
	CUPnPSubscribe* subscribe=new ( ELeave ) CUPnPSubscribe(aSubConnection , aResolver);
	CleanupStack::PushL( subscribe );
	subscribe->ConstructL();
	CleanupStack::Pop();
	return subscribe;
	}

void CUPnPSubscribe::ConstructL()
	{
	iDeltaTimer=CDeltaTimer::NewL(EPriorityLow);
	}

CUPnPSubscribe:: ~CUPnPSubscribe()
	{
	iDeltaTimer->Cancel();	
	delete iDeltaTimer;
	delete iElementArray;
	iTimerMappedArray.ResetAndDestroy();
	iCallbackArray.ResetAndDestroy();
	iMappedUri=NULL;
	}

/* Sends a subscribe request to the service point.It resolves all the uris passed
   in the parameter bundle to ip addresses if they're passed as domain names and
   appends them to element array

   @param aServiceInfo The parameter bundle which contains the uris to be sent
   describe requests to
 */
void CUPnPSubscribe::SubmitRequestL(const RPnPParameterBundle& aServiceInfo)
	{
	TInt paramIndex=0;
	
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).FindFamily( EUPnPSubscribeRequestParamSet );
	
	// Iteration 1 : Mainly used for resubscribing request. All the entries
	// corresponding to the uri which've been subscribed for before and are
	// present in this parameter set are removed
	
	CUPnPSubscribeRequestParamSet* paramSet  = NULL;
	TUint count = family.CountParameterSets ( RParameterFamily::ERequested ) ;
	
	for ( paramIndex = count - 1; paramIndex >= 0; paramIndex-- )
		{
		paramSet = static_cast<CUPnPSubscribeRequestParamSet*> ( family.GetParameterSetAtIndex ( paramIndex, RParameterFamily::ERequested ) );
		
		const TDesC8& uri = paramSet->Uri();
		__ASSERT_DEBUG( paramSet->Uri() != KNullDesC8, User::Panic( KUPnPInvalidUserInput, KUPnPErrInvalidInput ) );
		__ASSERT_DEBUG( paramSet->Duration() > 0, User::Panic( KUPnPInvalidUserInput, KUPnPErrInvalidInput ) );

		for(TInt i=0;i<iTimerMappedArray.Count();i++)
			{
			if(iTimerMappedArray[i]->UriExists(uri))
				{
				CTimerMappedUri* timerUri = iTimerMappedArray[i];
				iDeltaTimer->Remove(timerUri->GetTimerEntry());
				iTimerMappedArray.Remove(i);
				delete timerUri;
				break;
				}
			}
		for( TInt i=0;i<iCallbackArray.Count();i++)
			{
			if(uri.CompareF(iCallbackArray[i]->GetUri()) == NULL)
				{
				CCallbackArgument* callArgument = iCallbackArray[i];
				iCallbackArray.Remove(i);
				delete callArgument;
				break;
				}
			}
		
		if( iElementArray && iElementArray->MatchElement(uri))
			iElementArray->RemoveElement(uri);
		
		}
	// Initialize the iterator . The uris will be checked for duplication,resolved
	// and appended
	TInetAddr address;
	
	for ( paramIndex = count - 1; paramIndex >= 0; paramIndex-- )
		{
			paramSet = static_cast<CUPnPSubscribeRequestParamSet*> ( family.GetParameterSetAtIndex ( paramIndex, RParameterFamily::ERequested ) );
			const TDesC8& uri = paramSet->Uri();
			
		// Creates a new Element array if one is not already present and
		// appends the uri to it. If the uri is duplicate then the corresponding param
		// set is removed
		if(!iElementArray)
			{
			iElementArray = new CUPnPElementArray;
			User::LeaveIfNull(iElementArray);
			iElementArray->InsertElementL(aServiceInfo.PnPObserver(), uri);
			}
		else
			{
			if(!iElementArray->MatchElement(uri))
				{
				iElementArray->InsertElementL(aServiceInfo.PnPObserver(),uri);
				}
			else
				{
				reinterpret_cast<RUPnPParameterFamily*>(&family)->DeleteParamSetL(paramIndex);
				continue;
				}
			}
			
		CUPnPUtils::ResolveHostAddressL ( iResolver, paramSet->Uri(), address );
		paramSet->SetRemoteHost ( address );
		
		}
	/* Finally if all other processing is done successfully do the
	 * set params
	 */
			
	if ( family.CountParameterSets(RParameterFamily::ERequested)>=1)
		User::LeaveIfError(iSubConnection.SetParameters(aServiceInfo));
	
	
	}

/* On expiry of the CDeltaTimer associated with the requested uri
 * it removes the corresponding from the element array and notifies the client
 */
TInt CUPnPSubscribe::OnTimerExpiry(TAny* aPtr)
	{
	TInt error = KErrNone;
	CCallbackArgument* pointerData= static_cast<CCallbackArgument*> (aPtr);
	CUPnPSubscribe* thisObject= static_cast<CUPnPSubscribe*> (pointerData->GetThisPointer());
	CUPnPElementArray* thisElementArray=thisObject->ElementArrayHandle();
	RPointerArray<CTimerMappedUri>& timerArray=thisObject->GetTimerArray();
	RPointerArray<CCallbackArgument>& argArray = thisObject->GetArgumentArray();

	const TDesC8& uri = pointerData->GetUri();

	TInt index;
	for( index=0; index<timerArray.Count(); index++ )
		{
		if(timerArray[index]->UriExists(uri))
			{
			CTimerMappedUri* timerUri = timerArray[index];
			timerArray.Remove(index);
			delete timerUri;
			index = -1;
			break;
			}
		}
	__ASSERT_DEBUG( index == -1 ,User::Panic(KUPnPTimerUriNotFound,KUPnPErrTimerError) );
	for( index=0; index<argArray.Count(); index++ )
		{
		const TDesC8& testUri = argArray[index]->GetUri();
		if(uri.CompareF(argArray[index]->GetUri())==NULL)
			{
			argArray.Remove(index);
			break;
			}
		}
	MPnPObserver* observer = thisElementArray->MatchElement(uri);
	__ASSERT_DEBUG( observer , User::Invariant());
	// Removes the uri from the element Array 
	HBufC8* notifyUri = pointerData->GetUri().Alloc();
	__ASSERT_DEBUG( notifyUri , User::Invariant());
	
	// Deletes the CCallbackargument data
	delete pointerData;
	
	// Notifies the client that timer has expired
	TRAP(error,thisObject->MakeBundleAndNotifyL(observer, notifyUri));
	return KErrNone;
	}

/* Passes back the response bundle to the client. A response bundle
 will contain only one parameter set consequently with a single observer
 */
void CUPnPSubscribe::NotifyResultsL(TUint32 aFamilyId, RPnPParameterBundleBase& aBundle)
	{
	MPnPObserver* observer = NULL;
	User::LeaveIfNull(iElementArray);
	
	CUPnPSubscribeResponseParamSet* subscribeParamSet  = NULL;
	CUPnPNotifyEventParamSet* notifyParamSet  = NULL;	
	RParameterFamily family ;
	switch(aFamilyId)
		{
		case EUPnPSubscribeResponseParamSet:
			{
			// Response to subscription request
			family = aBundle.FindFamily(EUPnPSubscribeResponseParamSet);			
			__ASSERT_DEBUG(!family.IsNull (), User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));	
			__ASSERT_DEBUG(family.CountParameterSets(RParameterFamily::ERequested) > 0, User::Panic(KUPnPNoParamSet,KUPnPErrNoParamSet));				
			TUint count = family.CountParameterSets ( RParameterFamily::ERequested ) ;
			for ( TInt paramIndex = count - 1; paramIndex >= 0; paramIndex-- )
				{
				 subscribeParamSet = static_cast<CUPnPSubscribeResponseParamSet*> (family.GetParameterSetAtIndex(paramIndex,RParameterFamily::ERequested));
				 switch(( subscribeParamSet->UPnPEvent() ).iStatus)
					{
					case TUPnPEvent::ESuccess:
						{
						/* Extract the uri and the expiry time which has been supplied by
						 * the server and create a deltatimer entry using them
						 */ 
						TInt subscribeExpiryTime = subscribeParamSet->GetTimerExpiry();
						const TDesC8& uri = subscribeParamSet->Uri();
						observer = iElementArray->MatchElement(uri);
						__ASSERT_DEBUG( observer != NULL, User::Panic(KUPnPResponseUriNotFound,KUPnPErrResponseUriNotFound));

						CreateTimerEntryL(uri,subscribeExpiryTime);
						}
					break;

					case TUPnPEvent::EFail:
						{
						const TDesC8& uri = subscribeParamSet->Uri();
						observer=iElementArray->MatchElement(uri);
						__ASSERT_DEBUG( observer != NULL, User::Panic(KUPnPResponseUriNotFound,KUPnPErrResponseUriNotFound));
						iElementArray->RemoveElement(uri);
						}
					break;

					default:
						__ASSERT_DEBUG(0, User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));	
					}
				}
			}
		break;

		case EUPnPNotifyEventParamSet:
			{
			
			// Eventing parameter set
			family = aBundle.FindFamily( EUPnPNotifyEventParamSet );
			TUint count = family.CountParameterSets ( RParameterFamily::ERequested ) ;			
			notifyParamSet = static_cast<CUPnPNotifyEventParamSet*> ( family.GetParameterSetAtIndex ( count - 1, RParameterFamily::ERequested ) );	
			observer = iElementArray->MatchElement(notifyParamSet->Uri());
			__ASSERT_DEBUG( observer != NULL, User::Panic(KUPnPResponseUriNotFound,KUPnPErrResponseUriNotFound));
			}
			break;
		default:
			__ASSERT_DEBUG(0, User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));	
		}
	
	observer->OnPnPEventL(aBundle) ;
	}

/* Makes a bundle on expiry of the timer and sends it the client application
 * saying "Subscription has timed out"
 */
void CUPnPSubscribe::MakeBundleAndNotifyL(MPnPObserver* aObserver, HBufC8* aUri)
	{
	RPnPParameterBundleBase pnpBundle ;
	pnpBundle.Open();		
	CleanupClosePushL( pnpBundle );

	RParameterFamily family = pnpBundle.CreateFamilyL(EUPnPSubscribeResponseParamSet);
	// Create a new Param set
    CUPnPSubscribeResponseParamSet* subscribeParamSet = CUPnPSubscribeResponseParamSet::NewL(family );
    
    // Set the values for the event
    TUPnPEvent event;
    event.iStatus=TUPnPEvent::ESubscriptionTimeout;
    event.iErrorCode = KErrTimedOut;

   // Set the values in the parameter set
    subscribeParamSet->SetUPnPEvent(event);
    subscribeParamSet->SetUriL ( *aUri );
    subscribeParamSet->SetTimerExpiry(NULL);

    CleanupStack::Pop( &pnpBundle);
    delete aUri;
    aObserver->OnPnPEventL( pnpBundle ) ;
	}

/* CancelDescribe is used to send cancel subscribe requests to the network
 @param aServiceInfo The bundle which contains the list of uris whose describe
 		request should be cancelled
 */
void CUPnPSubscribe::CancelSubscribeL( const RPnPParameterBundle& aServiceInfo )
	{
	User::LeaveIfNull(iElementArray);	
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).FindFamily( EUPnPCancelSubscribeParamSet );
		
	CUPnPCancelSubscribeParamSet* paramSet = NULL;
	TUint count = family.CountParameterSets ( RParameterFamily::ERequested ) ;
	
	for ( TInt paramIndex = count - 1; paramIndex >= 0; paramIndex-- )
	{
		paramSet = static_cast<CUPnPCancelSubscribeParamSet*> ( family.GetParameterSetAtIndex ( paramIndex, RParameterFamily::ERequested) );
		//Extract the uri
		const TDesC8& uri = paramSet->Uri();
		__ASSERT_DEBUG( paramSet->Uri() != KNullDesC8, User::Panic( KUPnPInvalidUserInput, KUPnPErrInvalidInput ) );

		// Removes the uri entry from the array if exists
		for(TInt i=0;i<iTimerMappedArray.Count();i++)
			{
			if(iTimerMappedArray[i]->UriExists(uri) )
				{
				CTimerMappedUri* timerUri = iTimerMappedArray[i];
				iDeltaTimer->Remove(timerUri->GetTimerEntry());
				iTimerMappedArray.Remove(i);
				delete timerUri;
				break;
				}
			}
		for( TInt i=0;i<iCallbackArray.Count();i++)
			{
			if(uri.CompareF(iCallbackArray[i]->GetUri()) == NULL)
				{
				CCallbackArgument* callArgument = iCallbackArray[i];
				iCallbackArray.Remove(i);
				delete callArgument;
				break;
				}
			}

		// Remove the entry from element array
		MPnPObserver* observer = iElementArray->MatchElement( uri );
		if ( !observer )
			{		
			reinterpret_cast<RUPnPParameterFamily*>(&family)->DeleteParamSetL ( paramIndex );
			continue;
			}
		iElementArray->RemoveElement( uri );
		}
	if ( family.CountParameterSets(RParameterFamily::ERequested) >= 1)
		User::LeaveIfError(iSubConnection.SetParameters( aServiceInfo ));
	
	}

/* The timer delay is adjusted to 90% of the subscription time.Also it is adjusted
   for microseconds and IPC lag
 */
TInt CUPnPSubscribe::AdjustedDelay( TInt aTime )
	{
	TUint64 divisor = 10;
	TUint64 temp; // just a placeholder
	TUint64 q = Math::UDivMod64(aTime,divisor,temp);
	temp = q*(divisor-1);
	aTime = static_cast<TInt>(temp);

	const TInt KIntTime = 1000000 ;
	const TInt KInternalIPCLag = 1;
	aTime = ( aTime + KInternalIPCLag ) * KIntTime ;
	return aTime;
	}

/* CreateTimerEntry : creates the deltatimer entry and queues it up
 */
void CUPnPSubscribe::CreateTimerEntryL(const TDesC8& aUri ,TInt aExpiryTime)

	{
	CCallbackArgument* callArgument = CCallbackArgument::NewL(this,aUri);
	CleanupStack::PushL(callArgument);
	iCallbackArray.AppendL(callArgument);
	CleanupStack::Pop();
	
	TCallBack callBack(OnTimerExpiry,callArgument);
	iTimerEntry = new(ELeave) TDeltaTimerEntry( callBack );
	iMappedUri = CTimerMappedUri::NewL(aUri, iTimerEntry,aExpiryTime);
	iTimerMappedArray.AppendL(iMappedUri);
	
	// The time is set to 90% of the given time to allow the user
	// to resubscribe before the expiry time
	TInt time = AdjustedDelay(aExpiryTime);
	TTimeIntervalMicroSeconds32 delay(time);
	iDeltaTimer->Queue(delay,*iTimerEntry);
	}


