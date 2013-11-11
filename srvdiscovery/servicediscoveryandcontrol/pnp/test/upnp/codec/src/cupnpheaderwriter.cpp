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

#include <upnp/tupnptable.h>
#include <inetprottextutils.h>
#include <upnp/upnpconstants.h>

#include "cupnpheaderwriter.h"
#include "CHeaderField.h"


const TInt KMaxNumDigits = 10;
_LIT8(KTimeoutSecond, "Second-");
_LIT8(KSemiSpaceSep,"; ");
_LIT8(KQuotes, "\"");
_LIT8(KOpenAngleBracket, "<");
_LIT8(KCloseAngleBracket, ">");


CUPnPHeaderWriter* CUPnPHeaderWriter::NewL(RStringPool aStringPool)
	{
	return new (ELeave) CUPnPHeaderWriter(aStringPool);
	}

CUPnPHeaderWriter::~CUPnPHeaderWriter()
	{
	// do nothing.
	}

void CUPnPHeaderWriter::EncodeHeaderL(RHeaderField& aHeader)
	{
	RStringF fieldStr = iStringPool.StringF(aHeader.Name());
	switch( fieldStr.Index(TUPnPTable::Table()) )
		{
		case UPnP::EST:
			EncodeGenericUpnpHeaderL(aHeader, KErrUPnPEncodeST);
			break;
		case UPnP::EUSN:
			EncodeGenericUpnpHeaderL(aHeader, KErrUPnPEncodeUSN);
			break;
		case UPnP::ETimeout:
			EncodeTimeoutHeaderL(aHeader, KErrUPnPEncodeTimeout);
			break;
		case UPnP::EExt:
			EncodeExtL(aHeader);
			break;
		case UPnP::ESID:
			EncodeGenericUpnpHeaderL(aHeader, KErrUPnPEncodeSID);
			break;
		case UPnP::ENTS:
			EncodeGenericUpnpHeaderL(aHeader, KErrUPnPEncodeNTS);	
			break;
		case UPnP::ENT:
			EncodeGenericUpnpHeaderL(aHeader, KErrUPnPEncodeNT);
			break;
		case UPnP::EMX:
			EncodeGenericNumberHeaderL(aHeader, KErrUPnPEncodeMX);	
			break;
		case UPnP::ESEQ:
			EncodeGenericNumberHeaderL(aHeader, KErrUPnPEncodeSEQ);	
			break;
		case UPnP::EMAN:
			EncodeMANL(aHeader, KErrUPnPEncodeMAN);
			break;
		case UPnP::ESoapAction:
			EncodeSoapActionL(aHeader);	
			break;
		case UPnP::ECallback:
			EncodeCallbackL(aHeader);	
			break;
		default:
			User::Leave(KErrNotSupported);
		}	
	}

CUPnPHeaderWriter::CUPnPHeaderWriter(RStringPool aStringPool)
:iStringPool(aStringPool)
	{
	// do nothing.
	}

void CUPnPHeaderWriter::EncodeGenericUpnpHeaderL(RHeaderField& aHeader, TInt aErrorCode) const
	{
	THeaderFieldPartIter iter = aHeader.PartsL();
	iter.First();
	if (iter.AtEnd())
		User::Leave(aErrorCode);

	aHeader.BeginRawDataL();
	
	const CHeaderFieldPart* part = iter();
	if (part == NULL)
		User::Leave(aErrorCode);
	
	THTTPHdrVal ptVal = part->Value();
	if (ptVal.Type() != THTTPHdrVal::KStrFVal)
		User::Leave(aErrorCode);
	aHeader.WriteRawDataL(ptVal.StrF().DesC());
	
	aHeader.CommitRawData();	
	}

void CUPnPHeaderWriter::EncodeGenericNumberHeaderL(RHeaderField& aHeader, TInt aErrorCodeToUseOnFailure) const
	{
	// header = <header> ":" 1*DIGIT
	THeaderFieldPartIter iter1 = aHeader.PartsL();
	iter1.First();
	if (!iter1.AtEnd())
		{
		const CHeaderFieldPart* part = iter1();
		if (part)
			{
			THTTPHdrVal pt1val = part->Value();
			if (pt1val.Type() == THTTPHdrVal::KTIntVal)
				{
				TBuf8<KMaxNumDigits> lenBuf;
				lenBuf.Num(pt1val.Int());
				aHeader.BeginRawDataL();
				aHeader.WriteRawDataL(lenBuf);
				aHeader.CommitRawData();
				return;
				}
			}
		}
	User::Leave(aErrorCodeToUseOnFailure);
	}

void CUPnPHeaderWriter::EncodeTimeoutHeaderL(RHeaderField& aHeader, TInt aErrorCode) const
	{
	THeaderFieldPartIter iter = aHeader.PartsL();
	iter.First();
	if (iter.AtEnd())
		User::Leave(aErrorCode);
	
	aHeader.BeginRawDataL();
	
	const CHeaderFieldPart* part = iter();
	if (part == NULL)
		User::Leave(aErrorCode);
	
	THTTPHdrVal ptVal = part->Value();
	if (ptVal.Type() != THTTPHdrVal::KTIntVal)
		User::Leave(aErrorCode);
	
	if(ptVal.Int() == -KMaxTInt)
		{
		RStringF infiniteStrF = iStringPool.StringF(UPnP::EInfinite, TUPnPTable::Table());
		aHeader.WriteRawDataL(infiniteStrF.DesC());
		}
	else
		{
		HBufC8* ptValDesC;
		InetProtTextUtils::ConvertIntToDescriptorL(ptVal.Int(), ptValDesC);
		CleanupStack::PushL(ptValDesC);
		RBuf8 buf;
		buf.CreateL(KTimeoutSecond());
		CleanupClosePushL(buf);
		buf.ReAllocL(buf.Length() + ptValDesC->Length());
		buf.Append(ptValDesC->Des());
		aHeader.WriteRawDataL(buf);
		CleanupStack::PopAndDestroy(&buf);
		CleanupStack::PopAndDestroy(ptValDesC);
		}
	
	aHeader.CommitRawData();	
	}

void CUPnPHeaderWriter::EncodeMANL(RHeaderField& aHeader, TInt aErrorCode) const
	{
	THeaderFieldPartIter iter = aHeader.PartsL();
	iter.First();
	if (iter.AtEnd())
		User::Leave(aErrorCode);

	aHeader.BeginRawDataL();
	const CHeaderFieldPart* part = WriteRawStrFMANPartL(aHeader, iter, aErrorCode);

	// Check for parameters
	THeaderFieldParamIter iter2 = part->Parameters();
	iter2.First();
	
	if(!iter2.AtEnd())
		{
		aHeader.WriteRawDataL(KSemiSpaceSep);
		const CHeaderFieldParam* param = iter2();
		if (!param)
			User::Leave(aErrorCode);

		RStringF nsStr = iStringPool.StringF(UPnP::ENS,TUPnPTable::Table());
		THTTPHdrVal paramVal = param->Value();
		if (param->Name() == nsStr)
			{
			// Get the ns value
			if (paramVal.Type() != THTTPHdrVal::KTIntVal)
				User::Leave(aErrorCode);
			HBufC8* nsValBuf;
			InetProtTextUtils::ConvertIntToDescriptorL(paramVal.Int(), nsValBuf);
			CleanupStack::PushL(nsValBuf);
			aHeader.WriteRawDataL(nsStr.DesC());
			aHeader.WriteRawDataL('=');
			aHeader.WriteRawDataL(*nsValBuf);
			CleanupStack::PopAndDestroy(nsValBuf);				
			}
		else
			{
			// Extension parameter - handle strings only
			if (paramVal.Type() != THTTPHdrVal::KStrFVal)
				User::Leave(aErrorCode);
			RStringF paramName = iStringPool.StringF(param->Name());
			aHeader.WriteRawDataL(paramName.DesC());
			aHeader.WriteRawDataL('=');
			aHeader.WriteRawDataL(paramVal.StrF().DesC());
			}
		}

	// Done
	aHeader.CommitRawData();	
	}

const CHeaderFieldPart* CUPnPHeaderWriter::WriteRawStrFMANPartL(RHeaderField& aHeader, THeaderFieldPartIter& aIter, TInt aErrorCode) const
	{
	const CHeaderFieldPart* part = aIter();
	if (part == NULL)
		User::Leave(aErrorCode);
	THTTPHdrVal ptVal = part->Value();
	if (ptVal.Type() != THTTPHdrVal::KStrFVal)
		User::Leave(aErrorCode);
	
	aHeader.WriteRawDataL(KQuotes());
	aHeader.WriteRawDataL(ptVal.StrF().DesC());
	aHeader.WriteRawDataL(KQuotes());
	
	return part;
	}

void CUPnPHeaderWriter::EncodeSoapActionL(RHeaderField& aHeader) const
	{
	THeaderFieldPartIter iter = aHeader.PartsL();
	iter.First();
	if (iter.AtEnd())
		User::Leave(KErrUPnPEncodeSoapAction);
	
	aHeader.BeginRawDataL();

	const CHeaderFieldPart* part = iter();
	if (part == NULL)
		User::Leave(KErrUPnPEncodeSoapAction);
	THTTPHdrVal ptVal = part->Value();
	if (ptVal.Type() != THTTPHdrVal::KStrFVal)
		User::Leave(KErrUPnPEncodeSoapAction);
	
	aHeader.WriteRawDataL(KQuotes());
	aHeader.WriteRawDataL(ptVal.StrF().DesC());
	aHeader.WriteRawDataL(KQuotes());
	aHeader.CommitRawData();
	}

void CUPnPHeaderWriter::EncodeCallbackL(RHeaderField& aHeader) const
	{
	THeaderFieldPartIter iter = aHeader.PartsL();
	iter.First();
	if (iter.AtEnd())
		User::Leave(KErrUPnPEncodeCallback);
	
	aHeader.BeginRawDataL();

	while (!iter.AtEnd())
		{
		const CHeaderFieldPart* part = iter();
		if (part == NULL)
			User::Leave(KErrUPnPEncodeCallback);
		THTTPHdrVal ptVal = part->Value();
		if (ptVal.Type() != THTTPHdrVal::KStrFVal)
			User::Leave(KErrUPnPEncodeCallback);
		
		aHeader.WriteRawDataL(KOpenAngleBracket());
		aHeader.WriteRawDataL(ptVal.StrF().DesC());
		aHeader.WriteRawDataL(KCloseAngleBracket());
		++iter;
		}
	
	aHeader.CommitRawData();
	}

void CUPnPHeaderWriter::EncodeExtL(RHeaderField& aHeader)
	{
	aHeader.BeginRawDataL();
	aHeader.WriteRawDataL(KNullDesC8);
	aHeader.CommitRawData();	
	}
