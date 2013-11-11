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

#ifndef CUPNPHEADERREADER_H_
#define CUPNPHEADERREADER_H_

#include <stringpool.h>
#include <http/framework/cheadercodec.h>

#include "CHeaderField.h"


class CUPnPHeaderReader: public CHeaderReader
	{
public:
	static CUPnPHeaderReader* NewL(RStringPool aStringPool);
	virtual ~CUPnPHeaderReader();
	
private:
	void DecodeHeaderL(RHeaderField& aHeader);
	
private:
	CUPnPHeaderReader(RStringPool aStringPool);
	CHeaderFieldPart* SetNewPartL(RHeaderField& aHeader, TInt aPartIndex, THTTPHdrVal& aPartVal) const;
	CHeaderFieldPart* SetNewFStringPartL(RHeaderField& aHeader, TInt aPartIndex, TPtrC8 aValue) const;
	CHeaderFieldPart* SetNewIntegerPartL(RHeaderField& aHeader, TInt aPartIndex, TInt aValue) const;
	void DecodeGenericNumberL(RHeaderField& aHeader) const;
	CHeaderFieldParam* SetNewFStringParamL(CHeaderFieldPart& aHeaderPart, TPtrC8 aParamName, TPtrC8 aParamValue) const;
	void DecodeExtL(RHeaderField& aHeader) const;
	void DecodeGenericUpnpHeadersL(RHeaderField& aHeader) const;
	void DecodeTimeoutHeaderL(RHeaderField& aHeader) const;
	void DecodeCallbackL(RHeaderField& aHeader) const;
	void DecodeManL(RHeaderField& aHeader) const;
	
private:
	RStringPool iStringPool;
	};

#endif /*CUPNPHEADERREADER_H_*/
