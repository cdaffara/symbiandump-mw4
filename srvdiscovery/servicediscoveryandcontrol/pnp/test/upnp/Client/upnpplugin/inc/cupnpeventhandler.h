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


#ifndef __CUPNPEVENTHANDLER_H_
#define __CUPNPEVENTHANDLER_H_
#include <pnpparameterbundle.h>
class CUPnPEventHandler : public CBase
	{
public:
	CUPnPEventHandler( RSubConnection& aSubConnection );
	void SubmitRequestL( const RPnPParameterBundle& aBundle );
	~CUPnPEventHandler();

private:
	RSubConnection& iSubConnection;
	};
#endif /*CUPNPEVENTHANDLER_H_*/
