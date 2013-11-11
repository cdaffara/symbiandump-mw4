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

// System includes
#include <e32std.h>
#include <http/framework/cheadercodec.h>

// Local includes
#include "CHeaders.h"
#include "CHeaderField.h"


CHeaderField* CHeaderField::NewL(RStringF aHeaderFieldName, CHeaders& aOwner)
	{
	return new(ELeave)CHeaderField(aHeaderFieldName, aOwner);
	}

/*
CHeaderField* CHeaderField::NewL(RStringF aHeaderFieldName, CHeaders& aOwner, THTTPHdrVal aVal)
	{
	CHeaderField* me = new(ELeave)CHeaderField(aHeaderFieldName, aOwner);
	CleanupStack::PushL(me);
	CHeaderFieldPart* part = CHeaderFieldPart::NewL(aVal);
	CleanupStack::PushL(part);
	me->AddPartL(part);
	CleanupStack::Pop(2);
	return me;
	}
*/

CHeaderField* CHeaderField::NewL(RStringF aHeaderFieldName, CHeaders& aOwner, const TDesC8& aRawData)
	{
	CHeaderField* me = new(ELeave)CHeaderField(aHeaderFieldName, aOwner);
	CleanupStack::PushL(me);
	me->ConstructL(aRawData);
	CleanupStack::Pop(me);
	return me;
	}


/** D'tor
*/

CHeaderField::~CHeaderField()
	{
	iName.Close();
	ClearParsedData();
	ClearRawData();
	}


/** Getter's
*/
/*
RStringF CHeaderField::Name() const
	{
	return iName;
	}
*/

TInt CHeaderField::NumPartsL()
	{
	// Convert, if necessary, to the parsed format
	ConvertToParsedFormatL();

	// Check the array exists first
	return iParts.Count();
	}


THeaderFieldPartIter CHeaderField::PartsL()
	{
	// Convert, if necessary, to the parsed format
	ConvertToParsedFormatL();

	// create an iterator initialised for this header
	return THeaderFieldPartIter(this);
	}


CHeaderFieldPart* CHeaderField::PartL(TInt aIndex)
	{
	// Convert, if necessary, to the parsed format
	ConvertToParsedFormatL();

	// Check the part is in range, if so return it
	if (aIndex < NumPartsL())
		return iParts[aIndex];
	else
		return NULL;
	}

EXPORT_C
void CHeaderField::RawDataL(TPtrC8& aRawData)
	{
	ConvertToRawFormatL();
	aRawData.Set(*iRawData);
	}


/** Setters
*/
/*
void CHeaderField::SetPartL(CHeaderFieldPart* aPart, TInt aIndex)
	{
	if (aIndex < iParts.Count())
		{
		CHeaderFieldPart* oldPart = iParts[aIndex];
		iParts[aIndex] = aPart;
		delete oldPart;
		}
	else
		User::LeaveIfError(iParts.Append(aPart));
	}
*/
void CHeaderField::BeginRawDataL(TInt aChunkSize)
	{
	iRawChunkSize = aChunkSize;

	// clear out any existing Raw data, but don't reallocate if not needed
	if (!iRawData)
		iRawData = HBufC8::NewL(iRawChunkSize);
	else
		{
		TPtr8 rawPtr = iRawData->Des();
		rawPtr.Zero();
		}
	}


void CHeaderField::WriteRawDataL(const TDesC8& aData)
	{
    // Check to see if the buffer needs to grow
	TInt maxLength  = iRawData->Des().MaxLength();
	TInt reqdLength = iRawData->Length() + aData.Length();
	if (reqdLength > maxLength)
		{
		// Yes - allocate a new buffer of a larger size, and copy the contents
		// of the old one over. This may need more than one extra chunk. Check
		// for partial chunks - add an extra one if necessary.
		TInt numChunksReqd = (reqdLength - maxLength)/iRawChunkSize;
		 if ( (reqdLength - maxLength)%iRawChunkSize > 0)
			++numChunksReqd;
		HBufC8* largerBuffer = iRawData->ReAllocL(maxLength + iRawChunkSize*numChunksReqd);

		// Switch buffers. The old one was removed by ReAlloc.
		iRawData = largerBuffer;
		}

	// Can now append confidently
	iRawData->Des().Append(aData);
	}

/*
void CHeaderField::WriteRawDataL(TChar aData)
	{
	// Check to see if the buffer descriptor needs to grow
	TInt maxLength  = iRawData->Des().MaxLength();
	TInt reqdLength = iRawData->Length() + 1;
	if (reqdLength > maxLength)
		{
		// Yes - allocate a new buffer of a larger size, and copy the contents
		// of the old one over. Since a single character is being added, one
		// chunk will be enough.
		HBufC8* largerBuffer = iRawData->ReAllocL(maxLength + iRawChunkSize);

		// Switch buffers. The old one was removed by ReAlloc.
		iRawData = largerBuffer;
		}

	// Can now append confidently
	iRawData->Des().Append(aData);
	}
*/

void CHeaderField::CommitRawData()
	{
	// Change state and remove the parsed form of the header data
	iDataParseState = EDataInRawForm;
	ClearParsedData();
	}


void CHeaderField::AddPartL(CHeaderFieldPart* aPart)
	{
	// Convert, if necessary, to the parsed format
	ConvertToParsedFormatL();

	// Append the item
	User::LeaveIfError(iParts.Append(aPart));
	}

/*
void CHeaderField::InsertPartL(CHeaderFieldPart* aPart, TInt aIndex)
	{
	// Convert, if necessary, to the parsed format
	ConvertToParsedFormatL();

	// If the array isn't yet constructed, or is empty, then use 'AddPart'
	if (!NumPartsL()) 
		AddPartL(aPart);
	else
		User::LeaveIfError(iParts.Insert(aPart, aIndex));
	}
*/
/*
TInt CHeaderField::RemovePartL(CHeaderFieldPart* aPart)
	{
	// Convert, if necessary, to the parsed format
	ConvertToParsedFormatL();

	// Sanity check - if no parts, then the part can't be removed...
	if (iParts.Count() == 0)
		return KErrNotFound;

	// Find the part.  
	TInt idx = iParts.Find(aPart);

	// Now remove by index
	iParts.Remove(idx);
	iParts.Compress();
	delete aPart;
	return KErrNone;
	}
*/

TInt CHeaderField::RemovePartL(TInt aIndex)
	{
	// Convert, if necessary, to the parsed format
	ConvertToParsedFormatL();

	// Sanity check - if no array exists or if the index supplied is out of the bounds
	// of the array, then the part can't be removed...
	if ((aIndex < 0) || (aIndex >= NumPartsL()))
		return KErrNotFound;

	// Note that the array doesn't actually delete the part itself. Also the array must be
	// compressed to close the 'gap' left by the deleted part
	CHeaderFieldPart* part = PartL(aIndex);
	iParts.Remove(aIndex);
	iParts.Compress();
	delete part;
	return KErrNone;
	}


/** Constructors and second-phase construction
*/

void CHeaderField::ConvertToRawFormatL()
	{
	if (iDataParseState != EDataParsedOK)
		return;
	RHeaderField rf(*this);
	// State change and removal of parsed data  is done when the Raw data is committed by the codec when it is encoded
	iCodec.EncodeHeaderL(rf);
	}

void CHeaderField::ConvertToParsedFormatL()
	{
	if (iDataParseState != EDataInRawForm)
		return;
	RHeaderField rf(*this);
	iCodec.DecodeHeaderL(rf);

	// If we haven't left, then it must have succeeded. Change state and remove the Raw data
	iDataParseState = EDataParsedOK;
	ClearRawData();
	}

CHeaderField::CHeaderField(RStringF aHeaderFieldName, CHeaders& aOwner)
	: iDataParseState(ENoDataSupplied), iOwner(&aOwner), iCodec(aOwner.Codec())
	{
	// Ensure that we maintain string references correctly with the header field name
	iName = aHeaderFieldName.Copy();
	iDataParseState = EDataParsedOK;
	}

void CHeaderField::ConstructL(const TDesC8& aRawData)
	{
	// Copy the data.  Set the state.
	iRawData = aRawData.AllocL();
	iDataParseState = EDataInRawForm;
	}

void CHeaderField::ClearParsedData()
	{
	// Clear and destroy parts array and its contents
	iParts.ResetAndDestroy();
	}

void CHeaderField::ClearRawData()
	{
	// Destroy any Raw data
	delete iRawData;
	iRawData = NULL;
	}


/** Implementation of CHeaderFieldPart class *************************************************
*/

/** Factory methods
*/

EXPORT_C
CHeaderFieldPart* CHeaderFieldPart::NewL(THTTPHdrVal aVal)
	{
	return new(ELeave)CHeaderFieldPart(aVal);
	}


/** D'tor
*/

CHeaderFieldPart::~CHeaderFieldPart()
	{
	// Remove the parameter list.  Parent class clears the part value.
	iParams.ResetAndDestroy();

	// If the part's value is a string, then it must be closed.
	if (iValue.Type() == THTTPHdrVal::KStrFVal)
		iValue.StrF().Close();
	if (iValue.Type() == THTTPHdrVal::KStrVal)
		iValue.Str().Close();
	}


/** Getter's
*/

EXPORT_C
THTTPHdrVal CHeaderFieldPart::Value() const
	{
	return iValue;
	}

EXPORT_C
TInt CHeaderFieldPart::NumParameters() const
	{
	// Check the array exists first
	return iParams.Count();
	}


EXPORT_C
THeaderFieldParamIter CHeaderFieldPart::Parameters() const
	{
	// create an iterator initialised for this part
	return THeaderFieldParamIter(this);
	}

/*
CHeaderFieldParam* CHeaderFieldPart::Parameter(TInt aIndex) const
	{
	// Check the parameter is in range, if so return it
	if (aIndex < NumParameters())
		return (CHeaderFieldParam*)iParams[aIndex];
	else
		return NULL;
	}
*/

CHeaderFieldParam* CHeaderFieldPart::Parameter(RStringF aParamName) const
	{
	// Iterate the parameter list looking for a name match
	THeaderFieldParamIter it(this);
	CHeaderFieldParam* retVal = NULL;
	const CHeaderFieldParam* nextParam = NULL;
	TBool done = EFalse;
	while (!done && !it.AtEnd())
		{
		nextParam = it();
		if (nextParam->Name() == aParamName)
			{
			retVal = CONST_CAST(CHeaderFieldParam*, nextParam);
			done = ETrue;
			}
		++it;
		}
	return retVal;
	}



/** Setter's
*/

/*
void CHeaderFieldPart::SetValue(THTTPHdrVal aVal)
	{
	// We must check for an existing string value since it will have to be closed
	if (iValue.Type() == THTTPHdrVal::KStrFVal)
		iValue.StrF().Close();
	if (iValue.Type() == THTTPHdrVal::KStrVal)
		iValue.Str().Close();
	iValue = aVal.Copy();
	}
*/
EXPORT_C
void CHeaderFieldPart::AddParamL(CHeaderFieldParam* aParam)
	{
	// Append the item
	User::LeaveIfError(iParams.Append(aParam));
	}

/*
TInt CHeaderFieldPart::RemoveParam(CHeaderFieldParam* aParam)
	{
	// Sanity check - if no array exists, then the part can't be removed...
	if (iParams.Count() == 0)
		return KErrNotFound;

	// Find the param. 
	TInt idx = iParams.Find(aParam);

	// Now remove by index. Note that the array doesn't actually delete the param itself. Also
	// the array must be compressed to close the 'gap' left by the deleted param
	if (idx != KErrNotFound)
		{
		iParams.Remove(idx);
		iParams.Compress();
		delete aParam;
		idx = KErrNone;
		}
	return idx;
	}
*/

/** c'tor
*/
CHeaderFieldPart::CHeaderFieldPart(THTTPHdrVal aVal)
	: iValue(aVal)
	{
	// Ensure that we've copied the HdrVal properly - strings must be Copy()d in order to
	// correctly maintain string references
	iValue = aVal.Copy();
	}


	
/** Implementation of CHeaderFieldParam class *************************************************
*/

/** Factory methods
*/

EXPORT_C
CHeaderFieldParam* CHeaderFieldParam::NewL(RStringF aParamName, THTTPHdrVal aVal)
	{
	return new(ELeave)CHeaderFieldParam(aParamName, aVal);
	}


/** D'tor
*/

CHeaderFieldParam::~CHeaderFieldParam()
	{
	// Close the parameter name and value, if it is a string.
	iName.Close();
	if (iValue.Type() == THTTPHdrVal::KStrVal)
		iValue.Str().Close();
	if (iValue.Type() == THTTPHdrVal::KStrFVal)
		iValue.StrF().Close();
	}

/** Getter's
*/

EXPORT_C
RStringF CHeaderFieldParam::Name() const
	{
	return iName;
	}

EXPORT_C
THTTPHdrVal CHeaderFieldParam::Value() const
	{
	return iValue;
	}

/** Setter's
*/
/*
void CHeaderFieldParam::SetValue(THTTPHdrVal aVal)
	{
	// We must check for an existing string value since it will have to be closed
	if (iValue.Type() == THTTPHdrVal::KStrFVal)
		iValue.StrF().Close();
	if (iValue.Type() == THTTPHdrVal::KStrVal)
		iValue.Str().Close();
	iValue = aVal.Copy();
	}
*/
/** c'tor
*/
CHeaderFieldParam::CHeaderFieldParam(RStringF aName, THTTPHdrVal aVal)
	: iValue(aVal)
	{
	// Ensure that we've copied the arguments properly - strings must be Copy()d in order to
	// correctly maintain string references
	iName = aName.Copy();
	iValue = aVal.Copy();
	}


	
/** Implementation of THeaderFieldPartIter class **********************************************
*/


/** Normal c'tor
*/

THeaderFieldPartIter::THeaderFieldPartIter(const CHeaderField* aHeader)
	: iHeader(aHeader)
	{
	// to begin at the beginning...
	First();
	}

/** d'tor
*/


/** Reset the iterator to point at the first element
*/

EXPORT_C
void THeaderFieldPartIter::First()
	{
	// Check existence of the array in CHeaderField. If it hasn't been created yet then
	// we must start off 'at the end'.  This is indicated by the position index set to KErrNotFound
	iPosIdx = 0;
	CheckInvalidation();
	}

/** Check if the iterator is at the end of the list it traverses
*/

EXPORT_C
TBool THeaderFieldPartIter::AtEnd()
	{
	return (iPosIdx == KErrNotFound);
	}

/** Advance the iterator
*/

EXPORT_C
void THeaderFieldPartIter::operator++()
	{
	// Do nothing if we're already at the end
	if (iPosIdx >= 0)
		{
		++iPosIdx;
		CheckInvalidation();
		}
	}

/** Obtain the element at the iterator's current position
*/

EXPORT_C
const CHeaderFieldPart* THeaderFieldPartIter::operator()()
	{
	CheckInvalidation();
	if (iPosIdx > KErrNotFound)
		return iHeader->iParts.operator[](iPosIdx);
	else
		return NULL;
	}

void THeaderFieldPartIter::CheckInvalidation()
	{
	// iPosIdx may have been valid after the last use of operator() but
	// if an item was subsequently removed from the collection the iterator
	// may have gone off the end.
	if (iPosIdx >= iHeader->iParts.Count())
		iPosIdx = KErrNotFound; // Hit the end
	}



/** Implementation of THeaderFieldParamIter class *********************************************
*/


/** Normal c'tor
*/

THeaderFieldParamIter::THeaderFieldParamIter(const CHeaderFieldPart* aHeaderPart)
	: iHeaderPart(aHeaderPart)
	{
	// to begin at the beginning...
	First();
	}

/** d'tor
*/

EXPORT_C
THeaderFieldParamIter::~THeaderFieldParamIter()
	{
	// does nothing
	}

/** Reset the iterator to point at the first element
*/

EXPORT_C
void THeaderFieldParamIter::First()
	{
	// Check existence of the array in CHeaderFieldPart. If it hasn't been created yet then
	// we must start off 'at the end'.  This is indicated by the position index set to KErrNotFound
	iPosIdx = 0;
	CheckInvalidation();
	}

/** Check if the iterator is at the end of the list it traverses
*/

EXPORT_C
TBool THeaderFieldParamIter::AtEnd()
	{
	return (iPosIdx == KErrNotFound);
	}

/** Advance the iterator
*/

EXPORT_C
void THeaderFieldParamIter::operator++()
	{
	// Do nothing if we're already at the end
	if (iPosIdx >= 0)
		{
		++iPosIdx;
		CheckInvalidation();
		}
	}

/** Obtain the element at the iterator's current position
*/
EXPORT_C
const CHeaderFieldParam* THeaderFieldParamIter::operator()()
	{
	CheckInvalidation();
	if (iPosIdx > KErrNotFound)
		return iHeaderPart->iParams.operator[](iPosIdx);
	else
		return NULL;
	}

void THeaderFieldParamIter::CheckInvalidation()
	{
	// iPosIdx may have been valid after the last use of operator() but
	// if an item was subsequently removed from the collection the iterator
	// may have gone off the end.
	if (iPosIdx >= iHeaderPart->iParams.Count())
		iPosIdx = KErrNotFound; // Hit the end
	}

