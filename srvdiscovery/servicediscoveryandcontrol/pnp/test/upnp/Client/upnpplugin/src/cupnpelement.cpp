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

#include "cupnpelement.h"

CUPnPElement::CUPnPElement( MPnPObserver* aObserver ):iObserver( aObserver )
	{
	}

/* Destroy the contained RPointerArray but not the MPnPObserver
 * because its ownership is not with CUPnPElement
 */ 
CUPnPElement::~CUPnPElement()
	{
	iValueArray.ResetAndDestroy();
	}

 /* Append the uri into the array and returns KErrNone if it succeeds or 
 * returns a system wide error code if it fails
 * @param aUri  The uri to be appended
 */
void CUPnPElement::AppendValueL(const TDesC8& aUri)
	{
	HBufC8* uri=aUri.AllocL();
	CleanupStack::PushL(uri);
	iValueArray.AppendL(uri);
	CleanupStack::Pop(uri);
	}

/* Matches the passed uri argument against the RArrayPointer and returns
 * the index value if present and KErrNotFound if not .
 * @param aUriArg  The uri to be matched against
 */ 
TInt CUPnPElement::MatchValue(const TDesC8& aUriArg)
	{
	HBufC8* hbufPtr = NULL;
	for (TInt i=0 ; i<iValueArray.Count() ; i++)
		{
		hbufPtr = iValueArray[i];
		if( (*hbufPtr).CompareF(aUriArg) == NULL)
			return i;	
		}
	return KErrNotFound;
	}
/* Removes the url passed
 */ 

TInt CUPnPElement::RemoveValue(const TDesC8& aUriArg)
	{
	TInt index=MatchValue(aUriArg);
	if(index != KErrNotFound)
		{
		HBufC8* data=iValueArray[index];
		iValueArray.Remove(index);
		delete data;
		}
	return index;
	}


