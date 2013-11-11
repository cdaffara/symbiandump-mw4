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

#ifndef __CCALLBACKARGUMENT_H_
#define __CCALLBACKARGUMENT_H_
#include <e32base.h>
/*
  This class holds the *This pointer of the object calling it so that the static
  methods being called on expiry of CDeltaTimer can operate on the attributes of 
  class
 
  @internalComponent
 */
class CCallbackArgument: public CBase
	{
public:
	static CCallbackArgument* NewL(TAny* aThis ,const TDesC8& aUri );
	~CCallbackArgument();
	inline TAny* GetThisPointer() const;
	inline const TDesC8& GetUri() const;
private:
	CCallbackArgument (TAny* aThis );
	void ConstructL(const TDesC8& aUri);
	
	TAny* iThisPointer;
	RBuf8 iUri;
	};

/* Returns the pointer to the object it contains. */
TAny* CCallbackArgument::GetThisPointer() const
	{
	return iThisPointer;
	}

/* 	Returns a reference to the uri for which the timer has been triggered.This
 	enables the calling function to look up its table of uris for the matching uri
 	and take the appropriate action
 	*/ 
const TDesC8& CCallbackArgument::GetUri() const
	{
	return iUri;
	}

#endif /*CCALLBACKARGUMENT_H_*/
