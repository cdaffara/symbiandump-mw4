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

#include "cupnpelementarray.h"

CUPnPElementArray::CUPnPElementArray()
	{
	}

CUPnPElementArray::~CUPnPElementArray()
	{
	iElements.ResetAndDestroy();
	}

/* The callbacks alongwith the uris which're being received for each request by a control
 * point are retrieved and stored as a UPnPElement
 */
void CUPnPElementArray::InsertElementL( MPnPObserver* aObserver,const TDesC8& aUri )
	{	
	for(TInt i=0;i<iElements.Count();i++)
		{
		CUPnPElement* element= iElements[i];
		const MPnPObserver* observer=element->CallBack();
		
		/* If the MPnPObserver is already in the Element Array then
		 * append to it and return*/
		if(observer==aObserver)
			{
			element->AppendValueL(aUri);
			return ;
			}
		}
	//Create a new element and append the uri to it	
	CUPnPElement* element = new CUPnPElement(aObserver);
	element->AppendValueL(aUri);
	
	//Now Append the element to the element array
	iElements.AppendL(element);

	}

/* Tries to retrieve the callback given the uri and NULL if not present
 */
  MPnPObserver* CUPnPElementArray::MatchElement(const TDesC8& aUri)
	{
	TInt subIndex = KErrNotFound;
	for(TInt i=0;i<iElements.Count();i++)
		{
		CUPnPElement* element= iElements[i];
		subIndex=element->MatchValue(aUri);
		if(subIndex != KErrNotFound)
			return element->CallBack();
		}
	return NULL;
	}
/* Removes the uri given and the corresponding element as well if it is the last uri
 */
 
TInt CUPnPElementArray::RemoveElement(const TDesC8& aUri)
	{
	for(TInt i=0;i<iElements.Count();i++)
		{
		CUPnPElement* element= iElements[i];
		TInt subIndex=element->RemoveValue(aUri);
		if(subIndex != KErrNotFound)
			{
			if(element->UriExists()==EFalse)
				{
				iElements.Remove(i);
				delete element;
				}
			return subIndex;
			}
		}	
	// If the uri does not give any match in the element array
	return KErrNotFound;
	}

/* If any element contains "ssdp:all" then all other elements in the array are
   removed and only one element containing "ssdp:all" remains in the array.Used
   for RegisterNotify use case when a control point registers for all advertisements
   using "ssdp:all". No other uri is required to be registered thereon
   
   
 */
void CUPnPElementArray::SsdpAllModificationL( MPnPObserver* aObserver )
	// Returns the count of elements in the element array
	{
	_LIT8(KSsdpAll,"ssdp:all");
	//Create a new element and append the uri to it	
	CUPnPElement* element = new CUPnPElement(aObserver);
	
	//If unable to append this uri to the element then return directly
	element->AppendValueL( KSsdpAll);
		
	iElements.ResetAndDestroy();
	
	//Now Append the element to the element array
	iElements.AppendL(element);
	
	}
