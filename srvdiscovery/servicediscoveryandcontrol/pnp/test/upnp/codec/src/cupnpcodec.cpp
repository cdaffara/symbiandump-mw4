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
// upnpcodecmain.cpp
// 
//

#include <upnp/tupnptable.h>

#include "cupnpcodec.h"
#include "cupnpheaderreader.h"
#include "cupnpheaderwriter.h"


CUPnPCodec* CUPnPCodec::NewL(TAny* aStringPool)
	{
	RStringPool* strPool = static_cast<RStringPool*>(aStringPool);
	CUPnPCodec* me = new (ELeave) CUPnPCodec(*strPool);
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

CUPnPCodec::~CUPnPCodec()
	{
	// do nothing.
	}

TBool CUPnPCodec::CanEncode(RStringF aHeaderField) const
	{
	TBool canEncode = EFalse;

	RStringF st = iStringPool.StringF(UPnP::EST, TUPnPTable::Table());
	RStringF usn = iStringPool.StringF(UPnP::EUSN, TUPnPTable::Table());
	RStringF timeout = iStringPool.StringF(UPnP::ETimeout, TUPnPTable::Table());
	RStringF ext = iStringPool.StringF(UPnP::EExt, TUPnPTable::Table());
	RStringF sid = iStringPool.StringF(UPnP::ESID, TUPnPTable::Table());
	RStringF nts = iStringPool.StringF(UPnP::ENTS, TUPnPTable::Table());
	RStringF nt = iStringPool.StringF(UPnP::ENT, TUPnPTable::Table());
	RStringF mx = iStringPool.StringF(UPnP::EMX, TUPnPTable::Table());
	RStringF seq = iStringPool.StringF(UPnP::ESEQ, TUPnPTable::Table());
	RStringF man = iStringPool.StringF(UPnP::EMAN, TUPnPTable::Table());
	RStringF soapaction = iStringPool.StringF(UPnP::ESoapAction, TUPnPTable::Table());
	RStringF callback = iStringPool.StringF(UPnP::ECallback, TUPnPTable::Table());

	if(
	  (aHeaderField == st)||(aHeaderField == usn)||(aHeaderField == timeout)||(aHeaderField == ext)||
	  (aHeaderField == sid)||(aHeaderField == nts)||(aHeaderField == nt)||(aHeaderField == mx)||
	  (aHeaderField == seq)||(aHeaderField == man)||(aHeaderField == soapaction)||(aHeaderField == callback)
	  ) 
		{
		canEncode = ETrue; 
		}

	return canEncode;  
	}

TBool CUPnPCodec::CanDecode(RStringF aHeaderField) const
	{
	TBool canDecode = EFalse;

	RStringF st = iStringPool.StringF(UPnP::EST, TUPnPTable::Table());
	RStringF usn = iStringPool.StringF(UPnP::EUSN, TUPnPTable::Table());
	RStringF timeout = iStringPool.StringF(UPnP::ETimeout, TUPnPTable::Table());
	RStringF ext = iStringPool.StringF(UPnP::EExt, TUPnPTable::Table());
	RStringF sid = iStringPool.StringF(UPnP::ESID, TUPnPTable::Table());
	RStringF nts = iStringPool.StringF(UPnP::ENTS, TUPnPTable::Table());
	RStringF nt = iStringPool.StringF(UPnP::ENT, TUPnPTable::Table());
	RStringF mx = iStringPool.StringF(UPnP::EMX, TUPnPTable::Table());
	RStringF seq = iStringPool.StringF(UPnP::ESEQ, TUPnPTable::Table());
	RStringF man = iStringPool.StringF(UPnP::EMAN, TUPnPTable::Table());
	RStringF soapaction = iStringPool.StringF(UPnP::ESoapAction, TUPnPTable::Table());
	RStringF callback = iStringPool.StringF(UPnP::ECallback, TUPnPTable::Table());

	if(
	  (aHeaderField == st)||(aHeaderField == usn)||(aHeaderField == timeout)||(aHeaderField == ext)||
	  (aHeaderField == sid)||(aHeaderField == nts)||(aHeaderField == nt)||(aHeaderField == mx)||
	  (aHeaderField == seq)||(aHeaderField == man)||(aHeaderField == soapaction)||(aHeaderField == callback)
	  ) 
		{
		canDecode = ETrue; 
		}

	return canDecode;
	}

CHeaderCodec* CUPnPCodec::FindDelegateCodecL(RStringF /*aHeaderField*/) const
	{
	return CHeaderCodecPlugin::CreateDelegateCodecL(iStringPool);
	}

CUPnPCodec::CUPnPCodec(RStringPool aStringPool)
:CHeaderCodecPlugin(), iStringPool(aStringPool)
	{
	// do nothing.
	}

void CUPnPCodec::ConstructL()
	{
	CHeaderCodec::ConstructL();
	
	iWriter = CUPnPHeaderWriter::NewL(iStringPool);
	iReader = CUPnPHeaderReader::NewL(iStringPool);
	}
