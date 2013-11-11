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

#include "cupnpheaderreader.h"


_LIT8(KSemiSpaceSep," ;");
const TUint KOpenAngleBracket		= '<';
const TUint KCloseAngleBracket		= '>';
const TUint KCommaChar		= ',';
const TUint KSemiColonChar	= ';';
const TUint KEqualsChar		= '=';


CUPnPHeaderReader* CUPnPHeaderReader::NewL(RStringPool aStringPool)
	{
	return new (ELeave) CUPnPHeaderReader(aStringPool);
	}

CUPnPHeaderReader::~CUPnPHeaderReader()
	{
	// do nothing.
	}

void CUPnPHeaderReader::DecodeHeaderL(RHeaderField& aHeader)
	{
	RStringF fieldStr = iStringPool.StringF(aHeader.Name());
	switch( fieldStr.Index(TUPnPTable::Table()) )
		{
		case UPnP::ENTS:
		case UPnP::ENT:
		case UPnP::EST:
		case UPnP::EUSN:
		case UPnP::ESoapAction:
		case UPnP::ESID:
			DecodeGenericUpnpHeadersL(aHeader);	
			break;
		case UPnP::ETimeout:
			DecodeTimeoutHeaderL(aHeader);
			break;
		case UPnP::EExt:
			DecodeExtL(aHeader);
			break;
		case UPnP::EMX:
		case UPnP::ESEQ:
			DecodeGenericNumberL(aHeader);
			break;
		case UPnP::EMAN:
			DecodeManL(aHeader);
			break;
		case UPnP::ECallback:
			DecodeCallbackL(aHeader);
			break;
		default:
			User::Leave(KErrNotSupported);
		}	
	}

CUPnPHeaderReader::CUPnPHeaderReader(RStringPool aStringPool)
:iStringPool(aStringPool)
	{
	// do nothing.
	}

CHeaderFieldPart* CUPnPHeaderReader::SetNewPartL(RHeaderField& aHeader, TInt aPartIndex, THTTPHdrVal& aPartVal) const
	{
	CHeaderFieldPart* part = CHeaderFieldPart::NewL(aPartVal);
	CleanupStack::PushL(part);
	aHeader.SetPartL(part, aPartIndex);
	CleanupStack::Pop(part);
	return part;
	}

CHeaderFieldPart* CUPnPHeaderReader::SetNewFStringPartL(RHeaderField& aHeader, TInt aPartIndex, TPtrC8 aValue) const
	{
	RStringF partStr = iStringPool.OpenFStringL(aValue);
	CleanupClosePushL(partStr);
	THTTPHdrVal partVal(partStr);
	CHeaderFieldPart* part = SetNewPartL(aHeader, aPartIndex, partVal);
	CleanupStack::PopAndDestroy(&partStr);
	return part;
	}

CHeaderFieldPart* CUPnPHeaderReader::SetNewIntegerPartL(RHeaderField& aHeader, TInt aPartIndex, TInt aValue) const
	{
	THTTPHdrVal partVal(aValue);
	CHeaderFieldPart* part = SetNewPartL(aHeader, aPartIndex, partVal);
	return part;
	}

void CUPnPHeaderReader::DecodeGenericNumberL(RHeaderField& aHeader) const
	{
	TPtrC8 buffer;
	aHeader.RawDataL(buffer);
	TInt number = KErrNotFound;
	
	TInt decimalPos = buffer.Locate('.');
	if(decimalPos == 0)
		{
		// first character is decimal. So, set the value as zero.
		SetNewIntegerPartL(aHeader, 0, 0);
		}
	else
	   	{
		// Search for '\n' separator. In the case when a duplicate header has been received,
		// only use the fist instance of the valid data.
		TInt newLinePos = buffer.Locate('\n');
		if (newLinePos != KErrNotFound)
			{
			buffer.Set(buffer.Left(newLinePos));
			}
		
		TInt value = KErrNotFound;
		TInt ret = InetProtTextUtils::ConvertDescriptorToInt(buffer, value);
		if ( ret > KErrNone ) 
			{
			// Extract an integer.  Do not permit terminators other than WS or EOL.
			InetProtTextUtils::ExtractIntegerValueL(buffer, number, EFalse);	
			}
		SetNewIntegerPartL(aHeader, 0, number); // part 0, i.e. the first (and only) part
	   	}
	}

CHeaderFieldParam* CUPnPHeaderReader::SetNewFStringParamL(CHeaderFieldPart& aHeaderPart, TPtrC8 aParamName, TPtrC8 aParamValue) const
	{
	RStringF paramNameStr = iStringPool.OpenFStringL(aParamName);
	CleanupClosePushL(paramNameStr);
	RStringF paramValStr = iStringPool.OpenFStringL(aParamValue);
	CleanupClosePushL(paramValStr);
	THTTPHdrVal paramVal(paramValStr);
	CHeaderFieldParam* param = CHeaderFieldParam::NewL(paramNameStr, paramVal);
	CleanupStack::PushL(param);
	aHeaderPart.AddParamL(param);
	CleanupStack::Pop(param);
	CleanupStack::PopAndDestroy(2, &paramNameStr); // and paramValStr
	return param;
	}

void CUPnPHeaderReader::DecodeExtL(RHeaderField& aHeader) const
	{
	TPtrC8 nullPtr(KNullDesC8);
	SetNewFStringPartL(aHeader, 0, nullPtr);	
	}

void CUPnPHeaderReader::DecodeGenericUpnpHeadersL(RHeaderField& aHeader) const
	{
	TPtrC8 buffer;
	aHeader.RawDataL(buffer);
	
	// Search for '\n' separator. In the case when a duplicate header has been received,
	// only use the fist instance of the valid data.
	TInt newLinePos = buffer.Locate('\n');
	if (newLinePos != KErrNotFound)
		{
		buffer.Set(buffer.Left(newLinePos));
		}

	TPtrC8 token;
	InetProtTextUtils::ExtractNextTokenFromList(buffer, token, KSemiSpaceSep);
	SetNewFStringPartL(aHeader, 0, token); // part 0, i.e. the first (and only) part
	}

void CUPnPHeaderReader::DecodeTimeoutHeaderL(RHeaderField& aHeader) const
	{
	TPtrC8 buffer;
	aHeader.RawDataL(buffer);
	
	// Search for '\n' separator. In the case when a duplicate header has been received,
	// only use the fist instance of the valid data.
	TInt newLinePos = buffer.Locate('\n');
	if (newLinePos != KErrNotFound)
		{
		buffer.Set(buffer.Left(newLinePos));
		}
		
	RStringF infinite = iStringPool.StringF(UPnP::EInfinite, TUPnPTable::Table());
	if(buffer.Compare(infinite.DesC()) == 0)
		{
		SetNewIntegerPartL(aHeader, 0, -(KMaxTInt));	
		}
	
	else
		{
		TPtrC8 token;
		InetProtTextUtils::ExtractNextTokenFromList(buffer, token, KSemiSpaceSep);
		TInt consumed = token.Locate('-');
		token.Set(token.Mid(consumed+1));
		TInt intVal;
		InetProtTextUtils::ConvertDescriptorToInt(token, intVal);
		SetNewIntegerPartL(aHeader, 0, intVal); // part 0, i.e. the first (and only) part
		}
	}

void CUPnPHeaderReader::DecodeCallbackL(RHeaderField& aHeader) const
	{
	//Callback: <token1><token2><token3>...<tokenN>
	
	TPtrC8 rawData;
	aHeader.RawDataL(rawData);
	TInt remaining = rawData.Length();
	TPtrC8 token;
	TInt tokensFound = 0;
	TInt consumed;
	while (remaining)
		{
		// Locate and remove the '<' character from the token
		consumed = rawData.Locate(KOpenAngleBracket);
		if(consumed == KErrNotFound)
			{
			// No more tokens.
			break;	
			}
		// '<' character has now been removed.
		rawData.Set(rawData.Mid(consumed+1));
		// Now extract the value before '>' character. 
		// This will be the actual value of the token.
		remaining -= InetProtTextUtils::ExtractNextTokenFromList(rawData, token, KCloseAngleBracket);

		// No parameters. Just store the field value
		InetProtTextUtils::RemoveWhiteSpace(token, InetProtTextUtils::ERemoveBoth);
		SetNewFStringPartL(aHeader, tokensFound, token);
		++tokensFound;
		}
	}

void CUPnPHeaderReader::DecodeManL(RHeaderField& aHeader) const
	{
	TPtrC8 rawData;
	aHeader.RawDataL(rawData);
	TInt remaining = rawData.Length();
	TPtrC8 token;
	TInt tokensFound = 0;
	while (remaining > 0)
		{
		remaining -= InetProtTextUtils::ExtractNextTokenFromList(rawData, token, KCommaChar);

		TInt pos = token.Locate(KSemiColonChar);
		if (pos < 0)
			{
			// No parameters. Just store the field value
			InetProtTextUtils::RemoveWhiteSpace(token, InetProtTextUtils::ERemoveBoth);
			SetNewFStringPartL(aHeader, tokensFound, token);
			}
		else if (pos==0)
			{
			// No valid ns-value. Just store the parameter.
			User::Leave(KErrUPnPDecodeMAN);
			}
		else
			{
			// parameter value(s) exist.

			if (pos==token.Length())
				// if no field value exists. i.e. an invalid header
				User::Leave(KErrUPnPDecodeMAN);

			// store the field
			TPtrC8 fieldValue(token.Left(pos));
			TPtrC8 parameters(token.Mid(pos+1));
			InetProtTextUtils::RemoveWhiteSpace(fieldValue, InetProtTextUtils::ERemoveBoth);

			CHeaderFieldPart* part = SetNewFStringPartL(aHeader, tokensFound, fieldValue);

			TPtrC8 thisParam;
			do {
				// check if there is another parameter
				pos = parameters.Locate(KSemiColonChar);
				if (pos > 0)
					{
					if (pos==token.Length())
						// if no field value exists. i.e. an invalid header
						User::Leave(KErrUPnPDecodeMAN);

					thisParam.Set(parameters.Left(pos));
					parameters.Set(parameters.Mid(pos+1));
					}
				else
					thisParam.Set(parameters);

				
				TInt pPos = thisParam.Locate(KEqualsChar);
				if (pPos <= 0 || pPos==thisParam.Length())
					// Invalid parameter, missing '=' char, or missing field value.
					User::Leave(KErrUPnPDecodeMAN);

				TPtrC8 paramField(thisParam.Left(pPos));
 				TPtrC8 paramData(thisParam.Mid(pPos + 1));

				SetNewFStringParamL(*part, paramField, paramData);

				} while (pos > 0);
			}
		++tokensFound;
		}
	}
