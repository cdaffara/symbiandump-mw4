/** @file
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CUpnpHttpHeaderList
*
*/


// INCLUDES
#include "upnplist.h"

// -----------------------------------------------------------------------------
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CUpnpHttpHeaderList::CUpnpHttpHeaderList() : iHeaderList( 4 )
	{
	}

// -----------------------------------------------------------------------------
// C++ default destructor.
// -----------------------------------------------------------------------------
//
CUpnpHttpHeaderList::~CUpnpHttpHeaderList()
	{
	iHeaderList.Close();
	}

// -----------------------------------------------------------------------------
// Method deletes all objects from the list.
// -----------------------------------------------------------------------------
//

void CUpnpHttpHeaderList::DeleteObjects()
    {
	TInt size = Count();
	for ( int i = 0; i < size; i++ )
	    {
		if ( iHeaderList[i] != NULL )
	    	{
			CUpnpHttpHeader* t = iHeaderList[i];
			delete t;
			t = NULL;
		    }
	    }
    }

// -----------------------------------------------------------------------------
// Returns the number of elements in list.
// -----------------------------------------------------------------------------
//
void CUpnpHttpHeaderList::AddL( const CUpnpHttpHeader* aHeader )
    {
	iHeaderList.AppendL( aHeader );
    }

// -----------------------------------------------------------------------------
// Returns the number of elements in list.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpHeaderList::Remove( const CUpnpHttpHeader* aHeader )
    {
	TInt index = iHeaderList.Find( aHeader );
	if (index >= 0 )
        {
	    iHeaderList.Remove( index );
	    iHeaderList.Compress();
        }
    }
	 
// -----------------------------------------------------------------------------
// Returns the number of elements in list.
// -----------------------------------------------------------------------------
//	 
EXPORT_C CUpnpHttpHeader* CUpnpHttpHeaderList::First()
    {
	if ( Count() > 0 )
	    {
		return iHeaderList[0];
	    }
	return NULL;	
    }

// -----------------------------------------------------------------------------
// Returns the number of elements in list.
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpHeaderList::Count() const
	{
	return iHeaderList.Count();	
	}

// -----------------------------------------------------------------------------
// Goes to the next element on the list.
// -----------------------------------------------------------------------------
//	
EXPORT_C CUpnpHttpHeader* CUpnpHttpHeaderList::Next( const CUpnpHttpHeader* aHeader )
	{
	TInt index = iHeaderList.Find( aHeader);
	index++;
	if ( Count() > index )
        {
		return iHeaderList[index];
        }
	else
        {
		return NULL;
		}
	}


//end of file
