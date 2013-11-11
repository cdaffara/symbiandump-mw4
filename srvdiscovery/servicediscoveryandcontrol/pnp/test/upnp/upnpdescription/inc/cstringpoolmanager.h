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

#ifndef __CSTRINGPOOLMANAGER_H__
#define __CSTRINGPOOLMANAGER_H__

#include <e32base.h>
#include <stringpool.h>

/** CStringPoolManager manages all the upnp device and service description tags in a single
 string pool.
 @publishedPartner
 @prototype
 */ 
class CStringPoolManager : public CBase
	{
public:
	IMPORT_C static CStringPoolManager* NewL( const TStringTable& aStringTable);
	IMPORT_C static CStringPoolManager* NewL();
	IMPORT_C void Release();
	IMPORT_C RStringPool& StringPool();
	IMPORT_C const TStringTable& GetUPnPTable();
	IMPORT_C void SetStringTableL( const TStringTable& aStringTable );
private:
	CStringPoolManager();
	void ConstructL(const TStringTable& stringTable);
	~CStringPoolManager();
	
private:
	RStringPool iPool;
	RArray<TStringTable> iStringTableArray;
};

#endif // __CSTRINGPOOLMANAGER_H__

