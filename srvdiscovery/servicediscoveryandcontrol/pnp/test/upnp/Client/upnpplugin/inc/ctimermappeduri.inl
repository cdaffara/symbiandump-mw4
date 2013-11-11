#ifndef CTIMERMAPPEDURI_INL_
#define CTIMERMAPPEDURI_INL_
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
// Returns a reference to the timer entry for the purpose of deleting it from
// the deltatimer entry contained in the calling object
// 
//

TDeltaTimerEntry& CTimerMappedUri::GetTimerEntry() const
	{
	return *iEntry;
	}

const TDesC8& CTimerMappedUri::GetUri() const
	{
	return iUri;
	}


/* Tells if the uri passed is the same as contained in the object
 */
TBool CTimerMappedUri::UriExists(const TDesC8& aUri) const
	{
	if(aUri.CompareF(iUri) == NULL)
		return ETrue;
	else
		return EFalse;
	}

/* Returns the delay time according to which the timer has to be set */
TInt CTimerMappedUri::Delay() const
	{
	return iTime;
	}

CTimerMappedUri* CTimerMappedUri::NewL(const TDesC8& aUri,TDeltaTimerEntry* aEntry,TInt aTime)
	{
	CTimerMappedUri* timerUri = new(ELeave) CTimerMappedUri(aEntry,aTime);
	CleanupStack::PushL(timerUri);
	timerUri->ConstructL(aUri);
	CleanupStack::Pop(1);
	return timerUri;
	}

CTimerMappedUri::CTimerMappedUri(TDeltaTimerEntry* aEntry,TInt aTime):iEntry(aEntry),iTime(aTime)
	{
	}

void CTimerMappedUri::ConstructL(const TDesC8& aUri)
	{
	iUri.Close();
	iUri.CreateL(aUri);
	}

CTimerMappedUri::~CTimerMappedUri()
	{
	iUri.Close();
	delete iEntry;
	}
TBool CTimerMappedUri::MatchTimerEntry( const TDeltaTimerEntry* aEntry ) const
	{
	if ( aEntry == iEntry )
		return ETrue;
	else
		return EFalse;
	}

#endif /*CTIMERMAPPEDURI_INL_*/
