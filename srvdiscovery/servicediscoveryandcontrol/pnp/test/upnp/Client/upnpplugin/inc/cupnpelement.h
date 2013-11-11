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

#ifndef __CUPNPELEMENT_H_
#define __CUPNPELEMENT_H_

#include <e32base.h>
#include <mpnpobserver.h>
/* This is the container class which stores the uri's which have been requested for
   alongwith the callback(MPnPObserver) pointer which has sent the request
   
   @internalComponent
 */
class CUPnPElement:public CBase
	{
public:
	CUPnPElement( MPnPObserver* aObserver);
	void AppendValueL( const TDesC8& aUri);
	TInt MatchValue( const TDesC8& aUri);
	TInt RemoveValue( const TDesC8& aUri);
	~CUPnPElement();
	
	inline MPnPObserver* CallBack() const;
	inline TBool UriExists() const;
	
private:
	MPnPObserver*				iObserver;
	RPointerArray<HBufC8>		iValueArray;
	};

// Returns true or false based on whether the element contains any uris or not
// so that it can be deleted if no uri exist
TBool CUPnPElement::UriExists() const 
	{
	return (iValueArray.Count()<=0)? EFalse:ETrue;
	}

MPnPObserver* CUPnPElement::CallBack() const
	{
	return iObserver;
	}

#endif /*CUPNPELEMENT_H_*/
