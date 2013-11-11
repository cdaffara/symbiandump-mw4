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

#ifndef __UPNPSPSUBSCRIBEINFOCONTAINER_INL_
#define __UPNPSPSUBSCRIBEINFOCONTAINER_INL_

#include "upnpspsubscribeinfocontainer.h"	

CUPnPSubscribeInfoContainer* CUPnPSubscribeInfoContainer::NewL ( )
	{
	return  new ( ELeave ) CUPnPSubscribeInfoContainer ( );
	}

CUPnPSubscribeInfoContainer::~CUPnPSubscribeInfoContainer ( )
	{	
	iEventData.Close ( );
	iSubscribeInfoElementArr.ResetAndDestroy ( );
	}

void CUPnPSubscribeInfoContainer::AppendSubscribeInfoElementL ( CUPnPSubscribeInfoElement* aElement ) const
	{
	iSubscribeInfoElementArr.AppendL ( aElement );
	}

void CUPnPSubscribeInfoContainer::SetEventDataL ( const TDesC8& aEventData) const
	{
	iEventData.Close ( );
	iEventData.CreateL ( aEventData );
	}

const TDesC8&CUPnPSubscribeInfoContainer::EventData ( ) const 
	{
	return iEventData;
	}

inline void CUPnPSubscribeInfoContainer::DeleteSubscribeInfoElement ( CUPnPSubscribeInfoElement* aElement )
	{
	TInt index = iSubscribeInfoElementArr.Find(aElement);
	__ASSERT_DEBUG(index!=KErrNotFound,User::Invariant());
	delete aElement;
	aElement = NULL;
	iSubscribeInfoElementArr.Remove(index);
	}

TInt CUPnPSubscribeInfoContainer::CountOfSubscribeInfoElementArray ( ) const
	{
	return iSubscribeInfoElementArr.Count ( );
	}

CUPnPSubscribeInfoElement* CUPnPSubscribeInfoContainer::AtSubscribeInfoElementArray ( TInt aIndex ) const
	{	
	__ASSERT_DEBUG ( aIndex >= 0 && aIndex < iSubscribeInfoElementArr.Count ( ), User::Invariant ( ) );

	return iSubscribeInfoElementArr[aIndex];
	}

CUPnPSubscribeInfoElement* CUPnPSubscribeInfoContainer::GetSubscribeInfoBySid ( const TDesC8& aSid ) const
	{
	TUint count = iSubscribeInfoElementArr.Count ( );
	for ( TInt i = 0; i < count; i++  )
		{
		CUPnPSubscribeInfoElement* element = iSubscribeInfoElementArr[i];
		if ( aSid.CompareF( element->SubscriptionIdentifier ( ) ) == 0 )
			{
			return element;
			}
		}		
	return NULL;
	}

#endif /*UPNPSPSUBSCRIBEINFOCONTAINER_INL_*/
