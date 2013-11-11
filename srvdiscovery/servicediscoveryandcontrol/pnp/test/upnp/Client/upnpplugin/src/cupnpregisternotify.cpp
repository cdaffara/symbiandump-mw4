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

#include "cupnpregisternotify.h"
#include "pnputils.h"
#include <upnp/upnpconstants.h>

_LIT8(KSsdpAll,"ssdp:all");

CUPnPRegisterNotify::CUPnPRegisterNotify( RSubConnection& aSubConnection )
	: iSubConnection( aSubConnection )
	{
	}

CUPnPRegisterNotify::~CUPnPRegisterNotify()
	{
	delete iElementArray;
	}
/* Submits a request to the network to register for announcements of a specific type.
 @param aBundle Parameter bundle containing list of uris to be registered
 */
void CUPnPRegisterNotify::SubmitRequestL( const RPnPParameterBundle& aBundle )
	{
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aBundle).FindFamily ( EUPnPRegisterAnnouncementParamSet );
	// Iterate through all the param sets in the passed bundle

	CUPnPRegisterAnnouncementParamSet* paramSet  = NULL;

	TUint count = family.CountParameterSets ( RParameterFamily::ERequested ) ;
	// Perform the first iteration in which validation of all the uris
	// contained in the bundle will be done
	
	for ( TInt paramIndex = count - 1; paramIndex >= 0; paramIndex-- )
		{
		paramSet = static_cast<CUPnPRegisterAnnouncementParamSet*> ( family.GetParameterSetAtIndex ( paramIndex, RParameterFamily::ERequested ) );
		const TDesC8& uri  = paramSet->Uri();
		if( !iElementArray )
			{
			iElementArray = new (ELeave) CUPnPElementArray ;
			if (uri.CompareF(KSsdpAll) != NULL)
				User::LeaveIfError(CUPnPUtils::GenericUriValidity( uri, CUPnPUtils::EAny ));
			iElementArray->InsertElementL( aBundle.PnPObserver() , uri );
			}
		else
			{
			if ( iElementArray->MatchElement( uri ) || iElementArray->MatchElement( KSsdpAll ))
	 			{	 			
	 			reinterpret_cast<RUPnPParameterFamily*>(&family)->DeleteParamSetL ( paramIndex );
	 			continue;
	 			}
			if (uri.CompareF( KSsdpAll) == NULL)
				{
				iElementArray->SsdpAllModificationL( aBundle.PnPObserver() );
				for(TInt i=family.CountParameterSets(RParameterFamily::ERequested)-1; i>=0 ; i--)
					{
					if( i != paramIndex )						
						reinterpret_cast<RUPnPParameterFamily*>(&family)->DeleteParamSetL ( i );
					}
				break;
				}	
			
			User::LeaveIfError( CUPnPUtils::GenericUriValidity( uri, CUPnPUtils::EAny ) );	
			iElementArray->InsertElementL( aBundle.PnPObserver() , uri );
			}
		}
	User::LeaveIfError(iSubConnection.SetParameters(aBundle));	
	}

/* Used to cancel registration request
 @param aServiceInfo Bundle containing list of uris whose registraion for
         			 advertisements have to be cancelled
  */
void CUPnPRegisterNotify::CancelNotifyAnnouncementL( const RPnPParameterBundle& aServiceInfo )
	{
	RParameterFamily family = const_cast<RPnPParameterBundle&>(aServiceInfo).FindFamily ( EUPnPCancelRegisterNotifyParamSet );
	// Iterate through all the param sets in the passed bundle
	CUPnPCancelRegisterNotifyParamSet* paramSet = NULL;
	TUint count = family.CountParameterSets ( RParameterFamily::ERequested ) ;
	
	for ( TInt paramIndex = count - 1; paramIndex >= 0; paramIndex-- )
	{
	  	paramSet = static_cast<CUPnPCancelRegisterNotifyParamSet*> ( family.GetParameterSetAtIndex ( paramIndex, RParameterFamily::ERequested) );
	  
		if ( !(iElementArray->MatchElement(paramSet->Uri())))
			{			
			reinterpret_cast<RUPnPParameterFamily*>(&family)->DeleteParamSetL ( paramIndex );
			continue;
			}
		else
			iElementArray->RemoveElement( paramSet->Uri() );
		// Removes the uri entry from the array if exists
	}

	User::LeaveIfError(iSubConnection.SetParameters( aServiceInfo ));	
	}

/* Sends the notification to the client according to the advertisement received.
 Advertisements can be for both presence (ssdp:alive) or absence (ssdp:bye-bye)

 @param aFamily Indicates whether the advertisement is presence or absence
 */
void CUPnPRegisterNotify::NotifyResultsL(TUint32 aFamily, RPnPParameterBundleBase& aBundle)
	{
	MPnPObserver* observer = NULL;
	
	CUPnPPresenceAnnouncementParamSet* presenceParamSet  = NULL;
	CUPnPAbsenceAnnouncementParamSet* absenceParamSet  = NULL;
	RParameterFamily family ;
	switch(aFamily)
		{
		case EUPnPPresenceAnnouncementParamSet:
			{
			family = aBundle.FindFamily(EUPnPPresenceAnnouncementParamSet);						
			__ASSERT_DEBUG(!family.IsNull (), User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));
			__ASSERT_DEBUG( family.CountParameterSets(RParameterFamily::ERequested) > 0, User::Panic(KUPnPNoParamSet,KUPnPErrNoParamSet));		
			TUint count = family.CountParameterSets ( RParameterFamily::ERequested ) ;			
			presenceParamSet = static_cast<CUPnPPresenceAnnouncementParamSet*> ( family.GetParameterSetAtIndex ( count - 1, RParameterFamily::ERequested ) );
			
			observer = iElementArray->MatchElement( presenceParamSet->Uri () );
			if ( !observer )
				{
				observer = iElementArray->MatchElement( KSsdpAll );
				User::LeaveIfNull(observer);
				}
			break;
			}

		case EUPnPAbsenceAnnouncementParamSet:
			{
			// Remove the uri from the element array as the service
			// point is no more in the network
			family = aBundle.FindFamily(EUPnPAbsenceAnnouncementParamSet);			
			__ASSERT_DEBUG(!family.IsNull (), User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));
			__ASSERT_DEBUG( family.CountParameterSets(RParameterFamily::ERequested) > 0, User::Panic(KUPnPNoParamSet,KUPnPErrNoParamSet));
			TUint count = family.CountParameterSets ( RParameterFamily::ERequested ) ;			
			absenceParamSet = static_cast<CUPnPAbsenceAnnouncementParamSet*> ( family.GetParameterSetAtIndex ( count - 1, RParameterFamily::ERequested ) );		 

			observer = iElementArray->MatchElement( absenceParamSet->Uri () );
			if ( !observer )
				{
				observer = iElementArray->MatchElement( KSsdpAll );
				User::LeaveIfNull(observer);
				}
			break;
			}

		default:
		__ASSERT_DEBUG(0, User::Panic(KUPnPInvalidFamily,KUPnPErrInvalidFamily));	
		}

	observer->OnPnPEventL( aBundle ) ;
	}
