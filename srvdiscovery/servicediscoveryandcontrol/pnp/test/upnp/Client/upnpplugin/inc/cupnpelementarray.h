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

#ifndef __CUPNPELEMENTARRAY_H_
#define __CUPNPELEMENTARRAY_H_
#include "cupnpelement.h"

/* Container providing access and other operations on CUPnPElement object
 @internalComponent
 */
class CUPnPElementArray:public CBase
	{
public:
 	void  InsertElementL( MPnPObserver* aObserver,const TDesC8& aUri);
	MPnPObserver* MatchElement(const TDesC8& aUri );
	TInt RemoveElement( const TDesC8& aUri );
	void SsdpAllModificationL( MPnPObserver* aObserver );
	CUPnPElementArray();
	~CUPnPElementArray();

private:
	RPointerArray<CUPnPElement>			iElements;
		
};
#endif /*CUPNPELEMENTARRAY_H_*/
