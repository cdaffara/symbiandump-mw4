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

#ifndef __CRESPONSE_INL__
#define __CRESPONSE_INL__

inline CResponse::CResponse(RStringPool& aStringPool)
	:iStringPool(aStringPool)
	{
	}

inline TInt& CResponse::Status()
	{
	return iStatus;
	}

inline RStringF& CResponse::StatusString()
	{
	return iStatusString;
	}

inline TBool CResponse::IsHTTP10() const
	{
	return (((iVersion.iMajor == 1)&&(iVersion.iMinor == 0))?(ETrue):(EFalse));
	}

inline RResponse CResponse::Handle()
	{
	RResponse r;
	r.iImplementation = this;
	return r;
	}

inline RStringPool& CResponse::StringPool()
	{
	return iStringPool;
	}

inline const RStringPool& CResponse::StringPool() const
  	{
  	return iStringPool;
  	}
  
inline TVersion& CResponse::Version()
	{
	return iVersion;
	}

inline void CResponse::Reset()
	{
	Handle().GetHeaderCollection().RemoveAllFields();
	}

#endif // __CRESPONSE_INL__
