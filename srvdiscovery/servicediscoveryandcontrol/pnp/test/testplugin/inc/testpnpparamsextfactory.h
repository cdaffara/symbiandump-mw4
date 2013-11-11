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


#ifndef __TESTPNPPARAMSEXTFACTORY_H__
#define __TESTPNPPARAMSEXTFACTORY_H__

#include <e32base.h>

class CTestPnPParamSetBase;

class CTestPnPParamsExtFactory : public CBase
{
	public:
	static CTestPnPParamSetBase* NewL ( TAny* aConstructionParams );
};

#endif // __TESTPNPPARAMSEXTFACTORY_H__
