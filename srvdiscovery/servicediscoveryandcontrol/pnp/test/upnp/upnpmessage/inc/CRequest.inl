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

#ifndef __CREQUEST_INL__
#define __CREQUEST_INL__

inline CRequest::CRequest(RStringPool& aStringPool)
		:iStringPool(aStringPool)
	{
	}

inline const RStringF& CRequest::Method() const
  	{
  	return iMethod;
  	}

inline RStringF& CRequest::Method()
	{
	return iMethod;
	}

inline CUri8*& CRequest::URI()
	{
	return iURI;
	}

inline RRequest CRequest::Handle()
	{
	RRequest r;
	r.iImplementation = this;
	return r;
	}

inline RStringPool& CRequest::StringPool()
	{
	return iStringPool;
	}

inline const RStringPool& CRequest::StringPool() const
  	{
  	return iStringPool;
  	}
  
inline TBool CRequest::IsHTTP10() const
	{
	return (((iVersion.iMajor == 1)&&(iVersion.iMinor == 0))?(ETrue):(EFalse));
	}

inline TVersion& CRequest::Version()
   	{
  	return iVersion;
  	}

inline void CRequest::Reset()
	{
	Handle().GetHeaderCollection().RemoveAllFields();
	}

#endif // __CREQUEST_INL__
