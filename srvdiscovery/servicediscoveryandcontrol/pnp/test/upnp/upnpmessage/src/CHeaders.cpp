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
#include <http/thttptable.h>
#include <upnp/tupnptable.h>

// Local includes
#include "CHeaderField.h"
#include "CHeaders.h"
#include "rhttpheaders.h"
#include "tupnpmessagepanic.h"


CHeaders* CHeaders::NewL(CHeaderCodec& aCodec)
	{
	CHeaders* me = new(ELeave)CHeaders(aCodec);
	return me;
	}

CHeaders::~CHeaders()
	{
	PurgeFields();
	iFields.Close();
	}

RHTTPHeaders CHeaders::Handle()
	{	
	RHTTPHeaders handle;
	handle.iImplementation = this; 
	return handle;
	}


// create or append a field part for the named field and assigned value
void CHeaders::SetFieldL(RStringF aFieldName, THTTPHdrVal aFieldValue)
	{
	CHeaderField& field = CreateOrLookupFieldL(aFieldName);
	
	// Create a part for the value supplied
	CHeaderFieldPart* part = CHeaderFieldPart::NewL(aFieldValue);
	CleanupStack::PushL(part);
	field.AddPartL(part);
	CleanupStack::Pop(part);
	}

// create or append a field part with parameter, for the specified field, parameter and associated values
void CHeaders::SetFieldL(RStringF aFieldName, THTTPHdrVal aFieldValue,
						 RStringF aParamName, THTTPHdrVal aParamValue)
	{
	CHeaderField& field = CreateOrLookupFieldL(aFieldName);
	
	// Check whether a part with the supplied value already exists, if so then the API semantics are that
	// an additional parameter should be set on that part. Otherwise a new part will be created.
	THeaderFieldPartIter iter = field.PartsL();
	iter.First();
	CHeaderFieldPart* part = NULL;
	while (!iter.AtEnd())
		{
		const CHeaderFieldPart* pt = iter();
		if (pt->Value() == aFieldValue)
			part = CONST_CAST(CHeaderFieldPart*, pt);
		++iter;
		}

	// Create a part only if the match didn't occur
	if (!part)
		{
		part = CHeaderFieldPart::NewL(aFieldValue);
		CleanupStack::PushL(part);
		field.AddPartL(part);
		CleanupStack::Pop(part);
		}
	
	// Create a parameter for the value supplied
	CHeaderFieldParam* param = CHeaderFieldParam::NewL(aParamName, aParamValue);
	CleanupStack::PushL(param);
	part->AddParamL(param);
	CleanupStack::Pop(param);
	}

// Set a named field in the header to contain the supplied Raw data. Any existing header field of
// this name will be lost.
void CHeaders::SetRawFieldL(RStringF aFieldName, const TDesC8& aRawFieldData, const TDesC8& aFieldSeparator)
	{
	// See if field exists
	TInt idx = LocateFieldName(aFieldName);

	if (idx == KErrNotFound)
		{
		// create a new one using the field name
		CHeaderField* hdr = CHeaderField::NewL(aFieldName, *this, aRawFieldData);
		CleanupStack::PushL(hdr);
		DoInsertInNameOrderL(hdr, aFieldName);
		CleanupStack::Pop(hdr); 
		}
	else
		{
		CHeaderField* field = iFields[idx].iField;
		TPtrC8 originalRawData;
		field->RawDataL(originalRawData);

		// add the new raw data to the existing field using the aFieldSeparator param as a separator
		HBufC8* newRawDataBuf = HBufC8::NewLC(originalRawData.Length() + aFieldSeparator.Length() + aRawFieldData.Length());
		TPtr8 newRawDataPtr = newRawDataBuf->Des();
		newRawDataPtr.Append(originalRawData);
		newRawDataPtr.Append(aFieldSeparator);
		newRawDataPtr.Append(aRawFieldData);
		field->BeginRawDataL();
		field->WriteRawDataL(*newRawDataBuf);
		field->CommitRawData();
		CleanupStack::PopAndDestroy(newRawDataBuf);
		}	
	}

// Remove, entirely, the named header field from the header collection. All its parts and
// associated parameters (where they exist) are also removed
TInt CHeaders::RemoveField(RStringF aFieldName)
	{
	// Locate the name first - give up straight away if the field isn't found
	TInt idx = LocateFieldName(aFieldName);
	if (idx < 0)
		return idx;

	__ASSERT_DEBUG(idx < iFields.Count(), TUPnPMessagePanic::Panic(TUPnPMessagePanic::EHeaderFieldMissing));
	// destroy the header field
	CHeaderField* field = iFields[idx].iField;
	iFields.Remove(idx);
	delete field;

	return KErrNone;
	}

TInt CHeaders::RemoveFieldPartL(RStringF aFieldName, TInt aIndex)
	{
	TInt idx = LocateFieldName(aFieldName);
	if (idx == KErrNotFound)
		return idx;

	CHeaderField* field = iFields[idx].iField;
	field->RemovePartL(aIndex);
	
	if (field->NumPartsL() == 0)
		{
		iFields.Remove(idx);
		delete field;
		}
	return KErrNone;
	}

void CHeaders::RemoveAllFields()
	{
	PurgeFields();
	iFields.Reset();
	}

// Obtain the number of parts in the named header field's value, Simple headers are created with a single part
// following one call to SetFieldL. Subsequent calls to SetFieldL create additional parts with the field exists
// already
TInt CHeaders::FieldPartsL(RStringF aFieldName) const
	{
	TInt idx = LocateFieldName(aFieldName);
	if (idx == KErrNotFound)
		return 0;

	CHeaderField* field = iFields[idx].iField;
	return field->NumPartsL();
	}

// Obtain the named header field's value.  Optionally, the index of a part within the field
// may be specified. Parts are indexed from zero
TInt CHeaders::GetField(RStringF aFieldName, TInt aPartIdx, THTTPHdrVal& aHeaderValue) const
	{
	TInt error = KErrNone;
	CHeaderFieldPart* part = NULL;
	TRAPD(err, part = GetFieldPartL(aFieldName, aPartIdx));
	if (err)
		return err;

	if (part)
		aHeaderValue = part->Value();
	else
		error = KErrNotFound;
	return error;
	}

// Obtain the raw representation of the named header field's value.
TInt CHeaders::GetRawField(RStringF aFieldName, TPtrC8& aRawFieldData) const
	{
	TInt idx = LocateFieldName(aFieldName);
	if (idx < 0)
		return idx;
	CHeaderField* field = iFields[idx].iField;
	TRAPD(err,field->RawDataL(aRawFieldData));
	
	if (err == KErrNone)
		{
		TInt newLinePos = aRawFieldData.Locate('\n');
		if (newLinePos != KErrNotFound)
			{
			aRawFieldData.Set(aRawFieldData.Left(newLinePos));
			}
		}
		
	return err;
	}


// Obtain the value of a named parameter, associated with the named header field.  An optional index to a part
// within the header field may be supplied, if not it is assumed that it is the first part
THTTPHdrVal CHeaders::GetParam(RStringF aFieldName, RStringF aParamName, 
							   THTTPHdrVal& aValue, TInt aPartIdx) const
	{
	CHeaderFieldParam* param = GetFieldParameter(aFieldName, aParamName, aPartIdx);
	if (param == NULL)
		return KErrNotFound;
	aValue =  param->Value();
	return 0;
	}

// Set a param, overwriting one if it exists
void CHeaders::SetParamL(RStringF aFieldName, RStringF aParamName,THTTPHdrVal aParamValue,  TInt aPartIdx)
	{
	// Locate the name first
	TInt idx = LocateFieldName(aFieldName);
	User::LeaveIfError(idx);
	CHeaderField* field = iFields[idx].iField;
	CHeaderFieldPart* part = field->PartL(aPartIdx);
	CHeaderFieldParam* param = CHeaderFieldParam::NewL(aParamName, aParamValue);
	CleanupStack::PushL(param);
	part->AddParamL(param);
	CleanupStack::Pop(param);
	}


// create an iterator initialised for this header collection
THTTPHdrFieldIter CHeaders::Fields() const
	{
	return THTTPHdrFieldIter(this);
	}

// Obtain the index in the CHeaderField/RStringF arrays for the named header
TInt CHeaders::LocateFieldName(RStringF aHeaderName) const
	{
	// Locate the name first in the array of field names
	const TInt count = iFields.Count();
	for (TInt ii = 0; ii < count; ++ii)
		{
		if (iFields[ii].iFieldName == aHeaderName)
			return ii;
		}
	return KErrNotFound;
	}

// Check if a named header field exists. If so, return it. If not, create it.
CHeaderField& CHeaders::CreateOrLookupFieldL(RStringF aFieldName)
	{
	// Locate the name first
	TInt idx = LocateFieldName(aFieldName);

	// If the header was found, return it.  If no header matched, create
	// one for that field name
	CHeaderField* found = NULL;
	if (idx != KErrNotFound)
		{
		found = iFields[idx].iField;
		}
	else
		{
		CHeaderField* hdr = CHeaderField::NewL(aFieldName, *this);
		CleanupStack::PushL(hdr);
		DoInsertInNameOrderL(hdr, aFieldName);
		CleanupStack::Pop(hdr);
		found = hdr;
		}
	return *found;
	}

// Get the indexed part of the named field, if it exists
CHeaderFieldPart* CHeaders::GetFieldPartL(RStringF aFieldName, TInt aPartIdx) const
	{
	// check the field really exists
	TInt idx = LocateFieldName(aFieldName);
	User::LeaveIfError(idx);
	
	CHeaderField* field = iFields[idx].iField;
	CHeaderFieldPart* part = field->PartL(aPartIdx);
	return part;
	}

// Get the named parameter from the named field, if it exists
CHeaderFieldParam* CHeaders::GetFieldParameter(RStringF aFieldName, RStringF aParamName, TInt aPartIdx) const
	{
	// get the part and look up the named parameter
	CHeaderFieldPart* part = NULL;
	TRAPD(err, part = GetFieldPartL(aFieldName, aPartIdx));
	if (part && !err)
		return part->Parameter(aParamName);
	
	if (err == KErrNotFound)
		return NULL;
	else
		{
		// find out what circumstances this might occur in.  non-existent field is fine;
		// however if GetFieldPartL left then there might be a decoding error that should
		// be reported somehow.
		__DEBUGGER();
		return NULL;
		}
	}


void CHeaders::DoInsertInNameOrderL(CHeaderField* aHeader, RStringF aHeaderName)
	{
	THeaderFieldNamePair field;
	field.iFieldName = aHeaderName;
	field.iField = aHeader;
	TLinearOrder<THeaderFieldNamePair> sortKey(THeaderFieldNamePair::CompareFieldNames);
	User::LeaveIfError(iFields.InsertInOrder(field,sortKey));
	}


TInt CHeaders::THeaderFieldNamePair::CompareFieldNames(const THeaderFieldNamePair& aField1, 
													   const THeaderFieldNamePair& aField2)
	{
	TInt val1 = KErrNone;
	TInt val2 = KErrNone;
	
	RStringF header1 = aField1.iFieldName;
	RStringPool pool1 = aField1.iFieldName.Pool();
	
	RStringF nts1 = pool1.StringF(UPnP::ENTS, TUPnPTable::Table());
	RStringF nt1 = pool1.StringF(UPnP::ENT, TUPnPTable::Table());
	RStringF usn1 = pool1.StringF(UPnP::EUSN, TUPnPTable::Table());
	RStringF mx1 = pool1.StringF(UPnP::EMX, TUPnPTable::Table());
	RStringF st1 = pool1.StringF(UPnP::EST, TUPnPTable::Table());
	RStringF timeout1 = pool1.StringF(UPnP::ETimeout, TUPnPTable::Table());
	RStringF seq1 = pool1.StringF(UPnP::ESEQ, TUPnPTable::Table());
	RStringF man1 = pool1.StringF(UPnP::EMAN, TUPnPTable::Table());
	RStringF soapaction1 = pool1.StringF(UPnP::ESoapAction, TUPnPTable::Table());
	RStringF ext1 = pool1.StringF(UPnP::EExt, TUPnPTable::Table());
	RStringF sid1 = pool1.StringF(UPnP::ESID, TUPnPTable::Table());
	RStringF callback1 = pool1.StringF(UPnP::ECallback, TUPnPTable::Table());
	
	RStringF header2 = aField2.iFieldName;
	RStringPool pool2 = aField2.iFieldName.Pool();

	RStringF nts2 = pool2.StringF(UPnP::ENTS, TUPnPTable::Table());
	RStringF nt2 = pool2.StringF(UPnP::ENT, TUPnPTable::Table());
	RStringF usn2 = pool2.StringF(UPnP::EUSN, TUPnPTable::Table());
	RStringF mx2 = pool2.StringF(UPnP::EMX, TUPnPTable::Table());
	RStringF st2 = pool2.StringF(UPnP::EST, TUPnPTable::Table());
	RStringF timeout2 = pool2.StringF(UPnP::ETimeout, TUPnPTable::Table());
	RStringF seq2 = pool2.StringF(UPnP::ESEQ, TUPnPTable::Table());
	RStringF man2 = pool2.StringF(UPnP::EMAN, TUPnPTable::Table());
	RStringF soapaction2 = pool2.StringF(UPnP::ESoapAction, TUPnPTable::Table());
	RStringF ext2 = pool2.StringF(UPnP::EExt, TUPnPTable::Table());
	RStringF sid2 = pool2.StringF(UPnP::ESID, TUPnPTable::Table());
	RStringF callback2 = pool2.StringF(UPnP::ECallback, TUPnPTable::Table());

	if(
	   ((header1.DesC().Compare(nts1.DesC()) == 0)||(header1.DesC().Compare(nt1.DesC()) == 0)||
	   (header1.DesC().Compare(usn1.DesC()) == 0)||(header1.DesC().Compare(mx1.DesC()) == 0)||
	   (header1.DesC().Compare(st1.DesC()) == 0)||(header1.DesC().Compare(timeout1.DesC()) == 0)||
	   (header1.DesC().Compare(seq1.DesC()) == 0)||(header1.DesC().Compare(man1.DesC()) == 0)||
	   (header1.DesC().Compare(soapaction1.DesC()) == 0)||(header1.DesC().Compare(ext1.DesC()) == 0)||
	   (header1.DesC().Compare(sid1.DesC()) == 0)||(header1.DesC().Compare(callback1.DesC()) == 0))&&
	   ((header2.DesC().Compare(nts2.DesC()) == 0)||(header2.DesC().Compare(nt2.DesC()) == 0)||
	   (header2.DesC().Compare(usn2.DesC()) == 0)||(header2.DesC().Compare(mx2.DesC()) == 0)||
	   (header2.DesC().Compare(st2.DesC()) == 0)||(header2.DesC().Compare(timeout2.DesC()) == 0)||
	   (header2.DesC().Compare(seq2.DesC()) == 0)||(header1.DesC().Compare(man2.DesC()) == 0)||
	   (header2.DesC().Compare(soapaction2.DesC()) == 0)||(header2.DesC().Compare(ext2.DesC()) == 0)||
	   (header2.DesC().Compare(sid2.DesC()) == 0)||(header2.DesC().Compare(callback2.DesC()) == 0))
	  )
		
		{
		val1 = aField1.iFieldName.Index(TUPnPTable::Table());
		val2 = aField2.iFieldName.Index(TUPnPTable::Table());
		}
	else
		{
		val1 = aField1.iFieldName.Index(THTTPTable::Table());
		val2 = aField2.iFieldName.Index(THTTPTable::Table());
		}

	if (val1 != KErrNotFound && val2 != KErrNotFound)
		return val1 - val2;
	else if (val1 != KErrNotFound)
		return -1;
	else if (val2 != KErrNotFound)
		return 1;
	else
		return aField1.iFieldName.DesC().Ptr() - 
			aField2.iFieldName.DesC().Ptr();
	}


void CHeaders::PurgeFields()
	{
	const TInt count = iFields.Count();
	for (TInt ii = 0; ii< count; ++ii)
		{
		delete iFields[ii].iField;
		}
	}

