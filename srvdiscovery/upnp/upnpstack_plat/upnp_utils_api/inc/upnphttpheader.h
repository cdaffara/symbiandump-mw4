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
* Description:  Declares HttpHeader class.
*
*/


#ifndef C_CUPPNHTTPHEADER_H
#define C_CUPPNHTTPHEADER_H

// INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
	\brief A HTTPMessage header, which consists of Name-Value-pair.

	This is HTTPMessage header name-value pair. HTTPMessage contains a list of these header rows.
	*/
class CUpnpHttpHeader : public CBase
	{

public:
	/**
	Constructor function of HTTPHeader object.
	@param aName This is the name part of HTTP Header. 
	@param aValue This is the value part of HTTP Header. 
	@return A new CUpnpHttpHeader instance.
	**/
	static CUpnpHttpHeader* NewL(const TDesC8& aName, const TDesC8& aValue);
	
	/**
	Destructor function.
	**/
	virtual ~CUpnpHttpHeader();

	/**
	Returns value of HTTPHeader.
	@return A value of HTTPHeader.
	**/
	IMPORT_C TDesC8& Value();

	/**
	Returns name of HTTPHeader.
	@return A name of HTTPHeader.
	**/
	IMPORT_C TDesC8& Name();
	
	/**
	Set name of HTTPHeader.
	@param aName New name for HTTPHeader. 
	@return None.
	**/
	IMPORT_C void SetNameL(const TDesC8& aName);

	/**
	Set value of HTTPHeader.
	@param aValue New value for HTTPHeader.
	@return None. 
	**/
	IMPORT_C void SetValueL(const TDesC8& aValue);
	
private:

	/**
	First phase constructor.
	**/
	CUpnpHttpHeader();

	/**
	Second phase constructor.
	**/
	void ConstructL(const TDesC8& aName, const TDesC8& aValue);

	/**
	Name of header.
	**/
	HBufC8* iName;

	/**
	Value of header.
	**/
	HBufC8* iValue;
	};

#endif // C_CUPPNHTTPHEADER_H