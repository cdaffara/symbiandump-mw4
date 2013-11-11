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

#ifndef CUPNPHEADERWRITER_H_
#define CUPNPHEADERWRITER_H_

#include <stringpool.h>
#include <http/framework/cheadercodec.h>


class CUPnPHeaderWriter: public CHeaderWriter
	{
public:
	static CUPnPHeaderWriter* NewL(RStringPool aStringPool);
	virtual ~CUPnPHeaderWriter();
	
private:
	void EncodeHeaderL(RHeaderField& aHeader);
	
private:
	CUPnPHeaderWriter(RStringPool aStringPool);
	void EncodeGenericUpnpHeaderL(RHeaderField& aHeader, TInt aErrorCode) const;
	void EncodeGenericNumberHeaderL(RHeaderField& aHeader, TInt aErrorCodeToUseOnFailure) const;
	void EncodeTimeoutHeaderL(RHeaderField& aHeader, TInt aErrorCode) const;
	void EncodeMANL(RHeaderField& aHeader, TInt aErrorCode) const;
	const CHeaderFieldPart* WriteRawStrFMANPartL(RHeaderField& aHeader, THeaderFieldPartIter& aIter, TInt aErrorCode) const;
	void EncodeSoapActionL(RHeaderField& aHeader) const;
	void EncodeCallbackL(RHeaderField& aHeader) const;
	void EncodeExtL(RHeaderField& aHeader);
	
private:
	RStringPool iStringPool;
	};

#endif /*CUPNPHEADERWRITER_H_*/
