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
// Declaration of CHeaderField, CHeaderFieldPart, THeaderFieldPartIter, CHeaderFieldParam,
// and THeaderFieldParamIter classes.
// 
//

#ifndef __CHEADERFIELD_H__
#define __CHEADERFIELD_H__

#include <e32base.h>
#include <http/thttphdrval.h>
#include "rhttpheaders.h"



// The default chunk size for Raw data should be set to the average length of a header field when encoded in
// plaintext
const TInt KHttpDefaultRawChunkSize= 32;


class CHeaders;
class CHeaderFieldPart;
class THeaderFieldPartIter;
class CHeaderFieldParam;
class THeaderFieldParamIter;
class CHeaderCodec;


/** Generalised internal representation of a single HTTP header field.  The field's
	value may be contained as raw data as received by HTTP, or
	in a object-oriented parsed form, which is accessible to the client via
	the class API.

	Each field has a type and a (potentially complex) value.  The type must
	be chosen at construction time, and remains the same during the
	header's lifetime.

	Simple headers are represented using a single 'part'.  Several factory
	methods are provided for each type the part can take; e.g.
	
	@verbatim
	Max-Forwards: 3							-- a single integer value
	Location: http://ayg.com/				-- a single string token value
	Expires: Mon, 15 Jan 2001 10:30:25 GMT	-- a single date/time value
	@endverbatim
	
	More complex headers are possible (see the RFC2116 for many more examples)
	which are built up from several parts; e.g.
	
	@verbatim
	Accept: text/wml, application/vnd.wap.wmlc	-- a header with two string token parts
	Allow: GET, POST, HEAD						-- a header with three string token parts
	@endverbatim
	
	Multiple parts are supported by the CHeaderFieldPart class.

	Furthermore, some headers allow parameters to be supplied in the form of attribute-value
	pairs.   These may be present on any part of the header, according to the syntax defined
	in the RFC; e.g.
	
	@verbatim
	Accept-Charset: utf-8; q=0.8, us-ascii; q=0.2
	Cache-Control: maxage=6000
	@endverbatim
	
	Parameters are supported by the CHeaderFieldParam class.	

    The classes CHeaderField, CHeaderFieldPart and CHeaderFieldParam are internal header representations
	and will only be manipulated by protocol handlers and filters.  Only the RHTTPHeaders and THTTPHdrVal
	classes are intended for use by the Client.
	@see RHTTPHeaders
	@see THTTPHdrVal
	@see CHeaderFieldPart
	@see CHeaderFieldParam
	*/

class CHeaderField : public CBase
	{
friend class THeaderFieldPartIter;
friend class RHeaderField;
public:
	//
	// Getters
	//

	/** Obtain the name of the header field, e.g. Accept
		@return The header field name as a string token
	*/
	RStringF Name() const;

	/** Access the header value parts via an iterator
		@return An iterator constructed to point at the first part
		@leave KErrNoMemory - insufficient free memory for object creation when decoding parts
		@leave KErrNotSupported - if decoding when counting parts fails
	*/
	THeaderFieldPartIter PartsL();

	/** Get the Raw header representation.  This will flip the internal state of the header,
		if it's not already in Raw form. Requires a Codec to be set.
		@param The 8-bit data buffer representing the header in Raw form
		@leave KErrNoMemory - insufficient free memory for object creation when decoding parts
		@leave KErrNotSupported - if decoding when counting parts fails
	*/
	IMPORT_C
	void RawDataL(TPtrC8& aRawData);

	//
	// Setters
	//

	/** Set a part in the header. It replaces any existing parts at the specified index.
		Note this is for use only by the header codec, when converting Raw data into parsed header parts from parsed Raw data.
		This function may leave with any of the system wide error codes, if the attempt to append this part to the internal list
		fails.
		@param aPart The part to be added.  The header takes ownership of the new part.
		@param aIndex The index of the part you are replacing
	*/
	//void SetPartL(CHeaderFieldPart* aPart, TInt aIndex);

	/** Start setting Raw data in the header.  Leaves if initial Raw buffer allocation fails. The
		optional parameter specifies the size to be used for Raw data chunk allocation.
		Note this is for use only by the header codec. 
		@leave KErrNoMemory - insufficient free memory buffer allocation
	*/
	void BeginRawDataL(TInt aChunkSize = KHttpDefaultRawChunkSize);

	/** Add more Raw data, supplied as a descriptor.  The buffer is extended as necessary.
		Leaves if buffer allocation fails.
		Note this is for use only by the header codec. 
		@leave KErrNoMemory - insufficient free memory buffer allocation
	*/
	void WriteRawDataL(const TDesC8& aData);

	/** Add more Raw data, supplied as a single character.  The buffer is extended as necessary.
		Leaves if buffer allocation fails.
		Note this is for use only by the header codec. 
		@leave KErrNoMemory - insufficient free memory buffer allocation
	*/
	//void WriteRawDataL(TChar aData);

	/** Commit the Raw data.  The parsed header parts and parameters are removed.
		Note this is for use only by the header codec.
	*/
	void CommitRawData();



	/** Obtain the number of parts in this header
		@return The number, as an integer
		@leave KErrNoMemory - insufficient free memory for object creation when decoding parts
		@leave KErrNotSupported - if decoding when counting parts fails
	*/
	TInt NumPartsL();


	/** Create a new header field that has no value yet.
		@param aHeaderFieldName The header field type.
		@param aOwner The header fields collection that this field belongs to
		@leave KErrNoMemory - insufficient free memory to create an instance of the class.
	*/
	static CHeaderField* NewL(RStringF aHeaderFieldName, CHeaders& aOwner);

	/** Create a new header field with a single part, whose HTTP value is specified.
		@param aHeaderFieldName The header field type.
		@param aOwner The header fields collection that this field belongs to
		@param aVal The header field value. A copy is taken when storing.
		@leave KErrNoMemory - insufficient free memory to create an instance of the class, or any classes needed to create subclasses for parsing
		@leave KErrNotSupported - if decoding for this header field can't be made
	*/
	static CHeaderField* NewL(RStringF aHeaderFieldName, CHeaders& aOwner, THTTPHdrVal aVal);

	/** Create a new header field from the supplied Raw data using the specified transaction codec.
		@param aHeaderFieldName The header field type.
		@param aOwner The header fields collection that this field belongs to
		@param aRawData The raw data buffer, as a descriptor
		@leave KErrNoMemory - insufficient free memory to create an instance of the class, or any classes needed to create subclasses for parsing
		@leave KErrNotSupported - if decoding for this header field can't be made
	*/
	static CHeaderField* NewL(RStringF aHeaderFieldName, CHeaders& aOwner, const TDesC8& aRawData);

	// Class destructor
	virtual ~CHeaderField();
	/** Access the header value parts by index
		@param aIndex The index of the desired part. 0 references the first one.
		@return The indexed part, or NULL if the index was invalid.
		@leave KErrNoMemory - insufficient free memory for object creation when decoding parts
		@leave KErrNotSupported - if decoding when counting parts fails
	*/
	CHeaderFieldPart* PartL(TInt aIndex);


	/** Add a part to the header. It is appended after any existing parts. Simple headers
		have just one part;  they could be constructed using the 'single part' NewL() methods.
		@param aPart The part to be added.  The header takes ownership of the new part.
		@leave KErrNoMemory - insufficient free memory to create an instance of the class, or any classes needed to create subclasses for parsing
		@leave KErrNotSupported - if decoding for this header field can't be made
	*/
	void AddPartL(CHeaderFieldPart* aPart);

	/** Insert a part into the header. It is inserted at the specified index - i.e. to insert
		a new part at the start of the header, aIndex should be set to 0.
		@param aPart The part to be inserted.  The header takes ownership of the new part.
		@param aIndex The index at which the part is to be inserted.
		@leave KErrNoMemory - insufficient free memory to create an instance of the class, or any classes needed to create subclasses for parsing
		@leave KErrNotSupported - if decoding for this header field can't be made
	*/
	//void InsertPartL(CHeaderFieldPart* aPart, TInt aIndex);

	
	/** Remove the specified part from the header.
		@param aPart The part to be removed.  The header deletes the removed part.
		@return An error code; KErrNone if the removal succeeds or KErrNotFound if it didn't.
		@leave KErrNoMemory - insufficient free memory to create an instance of the class, or any classes needed to create subclasses for parsing
		@leave KErrNotSupported - if decoding for this header field can't be made
	*/
	//TInt RemovePartL(CHeaderFieldPart* aPart);


	/** Remove the indexed part from the header
		@param aIndex An index to the part to be removed.  The header deletes the removed part.
		@return An error code; KErrNone if the removal succeeds or KErrNotFound if it didn't.
		@leave KErrNoMemory - insufficient free memory to create an instance of the class, or any classes needed to create subclasses for parsing
		@leave KErrNotSupported - if decoding for this header field can't be made
	*/
	TInt RemovePartL(TInt aIndex);
private: // methods

	//
	// Internal format conversion
	//

	/** Create an Raw encoding of the header's field name and value. This is done
		using the Codec, which must be supplied prior to this call.	The part data is removed
		after conversion to the Raw format.
		@leave KErrNotSupported if a codec that supports encoding this header cannot be found
	*/
	void ConvertToRawFormatL();

	/** Create the part objects that represent the header field's value, by parsing the 
		Raw data received by HTTP.  The Raw data is removed after conversion completes
		successfully.
		@leave KErrNotSupported if a codec that supports encoding this header cannot be found
	*/
	void ConvertToParsedFormatL();	

	/** Standard constructor.  Used when client constructs headers.
		@param aHeaderFieldName A string table reference to the header field's name.
		@param aOwner The header fields collection that this field belongs to
		@return The constructed instance of CHeaderField
	*/
	CHeaderField(RStringF aHeaderFieldName, CHeaders& aOwner);

	/** Phase Two construction. This variant copies-to-keep the supplied Raw data buffer.
		@param aRawData An 8-bit buffer containing raw header data received by HTTP.
	*/
	void ConstructL(const TDesC8& aRawData);

	// Empty and destroy contents of the 'parts' array.
	void ClearParsedData();

	// Empty and destroy any Raw data held
	void ClearRawData();

private: // types and enums

	/** Enumeration of the states in which this header's value data can exist.
	*/
	enum TDataState
		{
		ENoDataSupplied	= 0x00, /**< the header object is not populated with data after construction*/
		EDataInRawForm	= 0x01, /**< the data is in it's raw form, i.e. not accessible by part*/
		EDataParsedOK	= 0x02, /**< the data is in it's client representation, i.e. parsed into parts.	*/
		EDataParseError	= 0x03  /**< the data could not be parsed so remains in Raw form*/
		};

private: // attributes

	/**  Name of this header (a reference into the string table) 
	*/
	RStringF iName;

	/** Has this header been parsed yet? did it parse correctly?
	*/
	TDataState iDataParseState;

	/**  Headers collection that owns this field
	*/
	CHeaders* iOwner;

	/** Header data - in Raw format.
	*/
	HBufC8* iRawData;

	/** Header data - in parsed format.
	*/
	RPointerArray<CHeaderFieldPart> iParts;
	
	/** Codec used to convert this header
	*/
	const CHeaderCodec& iCodec;

	/** Size of data chunk used when allocating and extending the Raw data buffer
	*/
	TInt iRawChunkSize;
	};




/** Generalised representation of a single HTTP header value part.  The part takes a
	value of one of the following three types:

	- integer, signed (i.e. can be -ve or +ve)
	- string token, which can represent free text (i.e. text with no special meaning; ETags; MD5 hashes;
	  MIME types, etc.), enumerations (words with special meanings such as 'private', 'close', ') or URLs
	- Date/Time

	In addition the part may have one or more parameters.  These take the form of
	attribute-value pairs.  They are implemented by the CHeaderFieldParam class.
*/
class CHeaderFieldPart : public CBase
	{
friend class THeaderFieldParamIter;

public:
	/** Create a new part, for the supplied HTTP value 
		@param aVal The HTTP value for this part of the header field. A copy is taken when storing.
		@return The constructed instance of CHeaderFieldPart.
	*/
	IMPORT_C
	static CHeaderFieldPart* NewL(THTTPHdrVal aVal);

	// Class destructor
	virtual ~CHeaderFieldPart();

	//
	// Getters
	//

	/** Obtain the value held in this field part.  The caller must call Copy() on the returned THTTPHdrVal
		if it is going to be stored.  This is needed in order to correctly maintain string references.
		@return The field part's HTTP value
	*/
	IMPORT_C
	THTTPHdrVal Value() const;

	/** Obtain the number of parameters in this header
		@return The number, as an integer
	*/ 
	IMPORT_C
	TInt NumParameters() const;

	/** Access the parameters in this header part via an iterator
		@return An iterator constructed to point at the first parameter
	*/
	IMPORT_C
	THeaderFieldParamIter Parameters() const;

	/** Access the header parameters by index
		@param aIndex The index of the desired parameter. 0 references the first one.
		@return The indexed parameter, or NULL if the index was invalid.
	*/
	//CHeaderFieldParam* Parameter(TInt aIndex) const;

	/** Access the header parameters by parameter name
		@param aParamName The name of the desired part.
		@return The named parameter, or NULL if it doesn't exist.
	*/
	CHeaderFieldParam* Parameter(RStringF aParamName) const;

	//
	// Setters
	//

	/** Change the value held in this field part
		@param  The HTTP value. It is copied in order to store it.
	*/
	//void SetValue(THTTPHdrVal aVal);

	/** Add a parameter to the header part. It is appended after any existing
		parameters.
		@param aParam The parameter to be added.  The header takes ownership of the
					  new parameter.
	*/
	IMPORT_C
	void AddParamL(CHeaderFieldParam* aParam);

	/** Remove the specified parameter from the header part.
		@param aParam The parameter to be removed.  The header deletes the removed
					  parameter.
		@return An error code; KErrNone if the removal succeeds or KErrNotFound if
				it didn't.
	*/
	//TInt RemoveParam(CHeaderFieldParam* aParam);

private: // methods

	// Default constructor. 
	CHeaderFieldPart(THTTPHdrVal aVal);

private: // attributes

	// The value of this part of the header field
	THTTPHdrVal iValue;

	// Parameters for this part.
	RPointerArray<CHeaderFieldParam> iParams;
	};


/** Generalised representation of a single HTTP header parameter.  The parameter has
	a name and takes a value using one of the three types supported in CHeaderFieldPart.

	Special case:
	- an unnamed parameter with an assigned value

	Examples of the use for this special case are:

	@verbatim
	Cache-Control: max-stale [=delta-seconds]
	Expect: expectation1=value1; expectation2=value2
	Content-Range: bytes 0-1023/4096
	@endverbatim

	Cache-Control's max-stale directive can take an optional value; hence the client
	should create a one-part header where the part is an enumeration value 'max-stale',
	and assign to that part an un-named parameter for the delta-seconds value if it is
	desired.

	Expect's sets of expectation expressions should each be added by the client as a
	separate part, each of which is given an unnamed parameter that takes the
	expectation	value.

	Content-Range should have one part added containing the enumeration 'bytes'.  Three
	parameters should be defined for that part: 'first', 'last' and 'total' which take
	the values in the byte-range.  This is, of course, application-specific and similar
	guidelines might be adopted by the parser to represent other headers.
	*/

class CHeaderFieldParam : public CBase
	{
public: // methods

	/** Create a new header parameter that has a descriptor value. 
		@param aParamName The name of the parameter, as a string table
						  reference. Can be an empty string.
		@param aVal The HTTP value of the parameter. A copy is taken when storing.
		@return The constructed instance of CHeaderFieldParam
	*/
	IMPORT_C
	static CHeaderFieldParam* NewL(RStringF aParamName, THTTPHdrVal aVal);

	// Class destructor
	virtual ~CHeaderFieldParam();

	//
	// Getters
	//

	/** Obtain the name of the parameter, e.g. q
	    @return String table reference to the name
	*/
	IMPORT_C
	RStringF Name() const;

	/** Obtain the value of the parameter.  The caller must call Copy() on the returned THTTPHdrVal
		if it is going to be stored.  This is needed in order to correctly maintain string references.
		@return The parameter's HTTP value
	*/
	IMPORT_C
	THTTPHdrVal Value() const;

	//
	// Setters
	//

	/** Change the value held in this field parameter
		@param  The HTTP value
	*/
	//void SetValue(THTTPHdrVal aVal);

private: // methods

	/** Default constructor. 
	*/
	CHeaderFieldParam(RStringF aName, THTTPHdrVal aVal);

private: // attributes

	// A string table reference for the name of this parameter
	RStringF iName;

	// The value of this part of the header field
	THTTPHdrVal iValue;
	};

/** Iterator class to iterate the parts contained within a header field. Requires friendship with
	CHeaderField and as such is tightly bound to that class.
*/
class THeaderFieldPartIter
	{
public:
	/** Construct an iterator for the parts of the supplied header.
		@param aHeader The header whose parts we want to iterate
	*/
	THeaderFieldPartIter(const CHeaderField* aHeader);
	/** Class destructor
	*/

	/** Reset iterator to point at the first part of the header
	*/
	IMPORT_C
	void First();
	/** Check if the iterator is at the end of the list. If so, further calls to
	    operator() will return NULL.
		@return True if the iterator has hit the end of the parts list
	*/
	IMPORT_C
	TBool AtEnd();

	/** Advance the iterator.
	*/
	IMPORT_C
	void operator++();
	/** Obtain the header part currently pointed at by the iterator.
		@return The header part;  NULL if the iterator has gone off the end of the list
	*/
	IMPORT_C
	const CHeaderFieldPart* operator()();

private:

	/** Check the iterator state for invalidity following deletions in the list
	*/
	void CheckInvalidation();

	/** The header whose parts we are iterating.
	*/
	const CHeaderField* iHeader;

	/** The index of the part in the header that is currently pointed at by the iterator
	*/
	TInt iPosIdx;
	};

/** Iterator class to iterate the parameters contained within a header part. Requires friendship with
	CHeaderFieldPart and as such is tightly bound to that class.
*/
class THeaderFieldParamIter
	{
public:
	/** Construct an iterator for the parameterss of the supplied header part.
		@param aHeaderPart The header part whose parameters we want to iterate
	*/
	THeaderFieldParamIter(const CHeaderFieldPart* aHeaderPart);
	/** Class destructor
	*/
	IMPORT_C
	~THeaderFieldParamIter();

	/** Reset iterator to point at the first part of the header
	*/
	IMPORT_C
	void First();
	/** Check if the iterator is at the end of the list. If so, further calls to
	    operator() will return NULL.
	*/
	IMPORT_C
	TBool AtEnd();

	/** Advance the iterator.
		@return True if the iterator still points at a valid parameter after advancing.
	*/
	IMPORT_C
	void operator++();
	/** Obtain the header parameter currently pointed at by the iterator.
		@return The header parameter;  NULL if the iterator has gone off the end of the list
	*/
	IMPORT_C
	const CHeaderFieldParam* operator()();

private:

	/** Check the iterator state for invalidity following deletions in the list
	*/
	void CheckInvalidation();

	/** The header part whose parameters we are iterating.
	*/
	const CHeaderFieldPart* iHeaderPart;
	/** The index of the parameter in the header part that is currently pointed at by the iterator
	*/
	TInt iPosIdx;
	};

#endif // __CHEADERFIELD_H__
