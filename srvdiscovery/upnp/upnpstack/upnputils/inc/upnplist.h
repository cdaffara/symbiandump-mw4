/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares RList type
*
*/



#ifndef C_CUPNPHEADERLIST_H
#define C_CUPNPHEADERLIST_H
// INCLUDES

#include "upnphttpheader.h"
#include <e32base.h>

class CUpnpHttpHeaderList: public CBase
{
public:
	/**
      * C++ default constructor.
    */
	CUpnpHttpHeaderList();
       
public:
	/**
      * C++ default destructor.
    */
	~CUpnpHttpHeaderList();
    
public:
	/**
      * Method deletes all objects from the list.
    */
	void DeleteObjects();
	
	/**
      * Adds element to the list.
    */
	void AddL(const CUpnpHttpHeader* aHeader);
	
	/**
      * Removes element from the list.
    */
	IMPORT_C void Remove(const CUpnpHttpHeader* aHeader);
	
	/**
      * Returns the frist element of the list.
    */ 
	IMPORT_C CUpnpHttpHeader* First();
	
	/**
      * Returns the number of elements in list.
    */
	TInt Count() const;
	
	/**
    * Goes to the next element on the list.
    */
	IMPORT_C CUpnpHttpHeader* Next(const CUpnpHttpHeader* aHeader);

	
	private:
	/**
	Header list
	*/
	RPointerArray<CUpnpHttpHeader> iHeaderList;
	
};
    
#endif // C_CUPNPHEADERLIST_H

//end of file
