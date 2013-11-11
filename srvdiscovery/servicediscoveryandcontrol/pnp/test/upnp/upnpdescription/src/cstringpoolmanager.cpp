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

#include <e32base.h>
#include "cstringpoolmanager.h"
#include <upnpdescriptionschema.h>

CStringPoolManager::CStringPoolManager()
	{
	}

EXPORT_C CStringPoolManager* CStringPoolManager::NewL(const TStringTable& aStringTable)
	{
	CStringPoolManager* mgr = new (ELeave) CStringPoolManager();
	CleanupStack::PushL(mgr);
	mgr->ConstructL(aStringTable);
	CleanupStack::Pop(mgr);
	return mgr;
	}
EXPORT_C CStringPoolManager* CStringPoolManager::NewL()
	{
	CStringPoolManager* mgr = new (ELeave) CStringPoolManager();
	CleanupStack::PushL(mgr);
	mgr->ConstructL(UPNPDESCRIPTIONXMLTAGS::Table );
	CleanupStack::Pop(mgr);
	return mgr;
	}
EXPORT_C void CStringPoolManager::Release()
	{
	delete this;
	}

EXPORT_C RStringPool& CStringPoolManager::StringPool()
	{
	return iPool;
	}

EXPORT_C void CStringPoolManager::SetStringTableL( const TStringTable& aStringTable )
	{
	iStringTableArray.AppendL( aStringTable );
	iPool.OpenL( aStringTable );
	}
EXPORT_C const TStringTable& CStringPoolManager::GetUPnPTable()
	{
	return UPNPDESCRIPTIONXMLTAGS::Table;
	}
CStringPoolManager::~CStringPoolManager()
	{
	iPool.Close();
	iStringTableArray.Close();
	}

void CStringPoolManager::ConstructL(const TStringTable& aStringTable)
	{
	iStringTableArray.AppendL( aStringTable );
	iPool.OpenL( aStringTable );
	}



