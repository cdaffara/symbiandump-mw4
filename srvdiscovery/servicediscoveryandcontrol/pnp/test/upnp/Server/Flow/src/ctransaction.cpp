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
// @file
// @internalComponent
// 
//

#include "ctransaction.h"


CTransaction* CTransaction::NewL ( CHeaderCodec& aCodec, RStringPool& aStringPool )
	{
	CTransaction* self = new ( ELeave ) CTransaction ( );
    CleanupStack::PushL ( self );
    self->ConstructL ( aCodec, aStringPool );
    CleanupStack::Pop ( self );
	return self;
	}

CTransaction::CTransaction ( )
: iDataLeft ( KErrUnknown )
	{	
	}

CTransaction::~CTransaction ( )
	{
	delete iRequest;
	delete iResponse;
	}
	
void CTransaction::ConstructL ( CHeaderCodec& aCodec, RStringPool& aStringPool )
	{
	iRequest = CRequest::NewL ( aCodec, aStringPool );
	iResponse = CResponse::NewL ( aCodec, aStringPool );
	}
	
CRequest* CTransaction::Request ( ) const
	{
	return iRequest;
	}
	
CResponse* CTransaction::Response ( ) const
	{
	return iResponse;
	}
