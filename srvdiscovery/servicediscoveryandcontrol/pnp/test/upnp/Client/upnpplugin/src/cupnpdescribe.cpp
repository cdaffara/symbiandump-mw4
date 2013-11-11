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

#include "cupnpdescribe.h"
#include <upnp/upnpconstants.h>
#include "pnputils.h"
#include "cupnpelement.h"

CUPnPDescribe::CUPnPDescribe(RSubConnection& aSubConnection , RHostResolver& aResolver) : iSubConnection( aSubConnection )
	,iResolver ( aResolver )
	{

	}

CUPnPDescribe::~CUPnPDescribe()
	{
	delete iElementArray;
	}

/* Sends a describe request to the service point.It resolves all the uris passed
   in the parameter bundle to ip addresses if they're passed as domain names and
   appends them to element array

   @param aServiceInfo The parameter bundle which contains the uris to be sent
   describe requests to
 */
void CUPnPDescribe::SubmitRequestL( const RPnPParameterBundle& aServiceInfo )
	{	
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).FindFamily( EUPnPDescribeRequestParamSet );

	// Iterate through all the param sets in the passed bundle
	CUPnPDescribeRequestParamSet* paramSet  = NULL;
	TUint count = family.CountParameterSets ( RParameterFamily::ERequested ) ;	

	TInetAddr address;
	// Initialize the iterator to the first family again. In this iteration all
	// the uris will be appended
	for ( TInt paramIndex = count - 1; paramIndex >= 0; paramIndex-- )
	{
		paramSet = static_cast<CUPnPDescribeRequestParamSet*> ( family.GetParameterSetAtIndex ( paramIndex, RParameterFamily::ERequested ) );

		__ASSERT_DEBUG( paramSet->Uri() != KNullDesC8, User::Panic( KUPnPInvalidUserInput, KUPnPErrInvalidInput ) );
		const TDesC8& uri = paramSet->Uri();

		// Creates a new Element array if one is not already present and
		// appends the uri to it
		if(!iElementArray)
			{
			iElementArray = new CUPnPElementArray;
			User::LeaveIfNull( iElementArray );
			}
			
		MPnPObserver* observer = iElementArray->MatchElement( uri );
		if ( observer )
			{			
			reinterpret_cast<RUPnPParameterFamily*>(&family)->DeleteParamSetL ( paramIndex );
			continue;
			}
		iElementArray->InsertElementL(aServiceInfo.PnPObserver(),uri);

		CUPnPUtils::ResolveHostAddressL ( iResolver, paramSet->Uri(), address ) ;
		paramSet->SetRemoteHost ( address );
			
		}
	User::LeaveIfError(iSubConnection.SetParameters(aServiceInfo));

	}

/* Used to send back the description data to the client through the OnPnPEventL()
 method
 */
void CUPnPDescribe::NotifyResultsL(RPnPParameterBundleBase& aBundle)
	{
	User::LeaveIfNull(iElementArray);
	
	TUint32 param = EUPnPDescribeResponseParamSet;
	RParameterFamily family = aBundle.FindFamily( param );	
	__ASSERT_DEBUG(!family.IsNull (), User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));
	__ASSERT_DEBUG( family.CountParameterSets(RParameterFamily::ERequested) > 0, User::Panic(KUPnPNoParamSet,KUPnPErrNoParamSet));	
	CUPnPDescribeResponseParamSet* paramSet  = NULL;
	MPnPObserver* observer = NULL;
	TUint count = family.CountParameterSets(RParameterFamily::ERequested);
		
	for ( TInt paramIndex = count - 1; paramIndex >= 0; paramIndex-- )
	{			
		paramSet = static_cast<CUPnPDescribeResponseParamSet*> ( family.GetParameterSetAtIndex ( paramIndex, RParameterFamily::ERequested) );
		const TDesC8& uri = paramSet->Uri();

		/* If the element array doesn't contain an element with matching
		 * uri then ignore it.
		 */

		observer = iElementArray->MatchElement( uri );
		__ASSERT_DEBUG( observer != NULL, User::Panic(KUPnPResponseUriNotFound,KUPnPErrResponseUriNotFound));
		iElementArray->RemoveElement( uri );
		
	}

	observer->OnPnPEventL(aBundle);
	}

/* CancelDescribe is used to send cancel describe requests to the network
 @param aServiceInfo The bundle which contains the list of uris whose describe
 		request should be cancelled
 */

void CUPnPDescribe::CancelDescribeL( const RPnPParameterBundle& aServiceInfo )
	{
	User::LeaveIfNull(iElementArray);
	
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).FindFamily( EUPnPCancelDescribeParamSet );
	
	TUint count = family.CountParameterSets ( RParameterFamily::ERequested ) ;
	// Iterate through the parameter sets and check to see whether the uris
	// are present.
	for ( TInt paramIndex = count - 1; paramIndex >= 0; paramIndex-- )
	{
		CUPnPCancelDescribeParamSet* paramSet  = NULL;
		paramSet = static_cast<CUPnPCancelDescribeParamSet*> ( family.GetParameterSetAtIndex ( paramIndex, RParameterFamily::ERequested) );
		//Extract the uri
		const TDesC8& uri = paramSet->Uri();

		/* If the element array doesn't contain an element with matching
		 * uri then ignore it
		 */
		MPnPObserver* observer = iElementArray->MatchElement( uri );
		if ( !observer )
			{			
			reinterpret_cast<RUPnPParameterFamily*>(&family)->DeleteParamSetL ( paramIndex );
			continue;
			}

		iElementArray->RemoveElement( uri );
	}

	// Do the set params only if the uri is to found in the element array
	if ( family.CountParameterSets(RParameterFamily::ERequested)>= 1)
		User::LeaveIfError(iSubConnection.SetParameters(aServiceInfo));	
	}
