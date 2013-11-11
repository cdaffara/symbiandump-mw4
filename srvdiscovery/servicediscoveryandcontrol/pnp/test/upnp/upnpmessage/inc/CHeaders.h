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

#ifndef __CHEADERS_H__
#define __CHEADERS_H__

#include <e32base.h>
#include <stringpool.h>


class CHeaderField;
class CHeaderFieldPart;
class CHeaderFieldParam;
class CHeaderCodec;
class RHTTPHeaders;
class THTTPHdrFieldIter;
class THTTPHdrVal;


/** The CHeaders class provides the internal implementation of the RHTTPHeaders interface, which is
	documented in <http.h>.  Briefly, this interface provides a unified point at which all header data
	can	be read and written for a given request or response.  The CHeaders object is related to an
	instance of a specific CHTTPTransactionCodec subclass, which is used to provide on-demand decoding
	and encoding of indivual header fields.
	@see RHTTPHeaders
*/
class CHeaders : public CBase
	{
friend class THTTPHdrFieldIter;
public:
 	
	// Factory construction, given a particular codec.  Ownership of the codec remains with the caller.
	static CHeaders* NewL(CHeaderCodec& aCodec);

 	// Destructor. Cleans up and releases resources to the system.
	~CHeaders();

	// Creates a handle that is an abstraction of the interface
	RHTTPHeaders Handle();

public: // methods proxied from RHTTPHeaders

	// Create or append a field part for the named field and assigned value
	void SetFieldL(RStringF aFieldName, THTTPHdrVal aFieldValue);

	// Create or append a field part with parameter, for the specified field, parameter and associated values
	void SetFieldL(RStringF aFieldName, THTTPHdrVal aFieldValue,
				   RStringF aParamName, THTTPHdrVal aParamValue);

	// Create a parameter in an exisitng part
	void SetParamL(RStringF aFieldName, RStringF aParamName,THTTPHdrVal aParamValue,  TInt aPartIdx);

	// Set a named field in the header to contain the supplied OTA data. Any existing header field of
	// this name will be lost.
	void SetRawFieldL(RStringF aFieldName, const TDesC8& aOtaFieldData, const TDesC8& aFieldSeparator);

	// Remove, entirely, the named header field from the header collection. All its parts and
	// associated parameters (where they exist) are also removed
	TInt RemoveField(RStringF aFieldName);

	// Remove the particular part of a field. All its parameters ,where they exist, are removed. If this is the only part
	// then the header is also removed
	TInt RemoveFieldPartL(RStringF aFieldName, TInt aIndex);


	// Remove all the fields in this header collection
	void RemoveAllFields();

	// Obtain the number of parts in the named header field's value, Simple headers are created with a single part
	// following one call to SetFieldL. Subsequent calls to SetFieldL create additional parts with the field exists
	// already
	TInt FieldPartsL(RStringF aFieldName) const;

	// Obtain the named header field's value.  Optionally, the index of a part within the field
	// may be specified. Parts are indexed from zero
	 TInt GetField(RStringF aFieldName, TInt aPartIdx, THTTPHdrVal& aHeaderValue) const;

	// Obtain an Over-The-Air representation of the named header field's value.
	TInt GetRawField(RStringF aFieldName, TPtrC8& aRawFieldData) const;


	// Obtain the value of a named parameter, associated with the named header field.  An optional index to a part
	// within the header field may be supplied, if not it is assumed that it is the first part
	THTTPHdrVal GetParam(RStringF aFieldName, RStringF aParamName, THTTPHdrVal& aValue, TInt aPartIdx) const;

	// Access the header fields via an iterator
	THTTPHdrFieldIter Fields() const;

	// Access the codec used to convert header fields between OTA and parsed format
	CHeaderCodec& Codec() const;

	static const TStringTable& Table();

private:

	// Structure which contains the headerfield and the associated name
	class THeaderFieldNamePair
		{
	public:
		static TInt CompareFieldNames(const THeaderFieldNamePair& aField1, const THeaderFieldNamePair& aFieldName2);
	public:
		RStringF iFieldName;
		CHeaderField* iField;
		};

	
	// Constructor; stores the supplied codec pointer
	CHeaders(CHeaderCodec& aCodec);

	// Utility to look up a given header name from the iFieldNames array and return its index.
	TInt LocateFieldName(RStringF aHeaderName) const;

	// Looks up the given header name and returns the corresponding field if it exists; if not, a new field
	// is created and returned
	CHeaderField& CreateOrLookupFieldL(RStringF aHeaderName);

	// Obtains the indexed part of the named field
	CHeaderFieldPart* GetFieldPartL(RStringF aFieldName, TInt aPartIdx) const;

	// Obtains the index parameter of the named field's indexed part
	CHeaderFieldParam* GetFieldParameter(RStringF aFieldName, RStringF aParamName, TInt aPartIdx) const;

	// Inserts a header in the order of the name.
	void DoInsertInNameOrderL(CHeaderField* aHeader, RStringF aHeaderName);

	// Deletes all fields
	void PurgeFields();
	
private:

	// An array of field name pairs. Each field contains several values that is associated with a name of a field
	RArray<THeaderFieldNamePair> iFields;

	// The transaction codec associated with this header collection.
	CHeaderCodec& iCodec;
	};


inline CHeaders::CHeaders(CHeaderCodec& aCodec)
	: iCodec(aCodec)
	{
	}

inline CHeaderCodec& CHeaders::Codec() const
	{
	return iCodec;
	}

#endif // CMESSAGE_H
