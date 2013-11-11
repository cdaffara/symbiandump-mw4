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

#ifndef __CTIMERMAPPEDURI_H_
#define __CTIMERMAPPEDURI_H_
#include <e32base.h>
/* This class stores a mapping of TDeltaTimerEntry corresponding to the uris
 for which search request has been  issued. It allows the entry to be removed in 
 case a new search/subscribe request with the same uri is issued or when cancelling
 the search/subscribe request 
 
 @internalComponent
 */
class CTimerMappedUri : public CBase
	{
public:
	inline static CTimerMappedUri* NewL(const TDesC8& aUri,TDeltaTimerEntry* aEntry,TInt aTime);
	inline ~CTimerMappedUri();
	inline TDeltaTimerEntry& GetTimerEntry() const;
	inline TInt Delay() const;
	inline TBool UriExists(const TDesC8& aUri) const;
	inline const TDesC8& GetUri() const;
	inline TBool MatchTimerEntry( const TDeltaTimerEntry* aEntry ) const ;
private:
	inline void ConstructL(const TDesC8& aUri);
	inline CTimerMappedUri(TDeltaTimerEntry* aEntry, TInt aTime );
	
	RBuf8 iUri;
	TDeltaTimerEntry* iEntry;
	TInt iTime;
	};

#include "ctimermappeduri.inl"
#endif /*CTIMERMAPPEDURI_H_*/
