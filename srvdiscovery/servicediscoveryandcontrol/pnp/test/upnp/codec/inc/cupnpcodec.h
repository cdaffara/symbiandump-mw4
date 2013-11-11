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
// cupnpcodec.cpp
// 
//

#ifndef CUPNPCODEC_H_
#define CUPNPCODEC_H_

#include <http/framework/cheadercodecplugin.h>
#include <stringpool.h>

class CUPnPCodec: public CHeaderCodecPlugin
	{
public:
	static CUPnPCodec* NewL(TAny* aStringPool);
	virtual ~CUPnPCodec();
	
private:
	TBool CanEncode(RStringF aHeaderField) const;
	TBool CanDecode(RStringF aHeaderField) const;
	CHeaderCodec* FindDelegateCodecL(RStringF aHeaderField) const;
	
private:
	CUPnPCodec(RStringPool aStringPool);
	void ConstructL();
	
private:
	RStringPool iStringPool;
	};

#endif /*CUPNPCODEC_H_*/
