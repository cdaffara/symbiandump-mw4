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

#include "ccallbackargument.h"

CCallbackArgument* CCallbackArgument::NewL( TAny* aThis,const TDesC8& aUri )
	{
	CCallbackArgument* callArgument = new (ELeave )CCallbackArgument(aThis);
	CleanupStack::PushL(callArgument);
	callArgument->ConstructL(aUri);
	CleanupStack::Pop(1);
	return callArgument;
	}
CCallbackArgument::CCallbackArgument( TAny* aThis):iThisPointer(aThis)
	{
	}

CCallbackArgument::~CCallbackArgument()
	{
	iUri.Close();
	}

void CCallbackArgument::ConstructL(const TDesC8& aUri )
	{
	iUri.Close();
	iUri.CreateL(aUri);
	}
