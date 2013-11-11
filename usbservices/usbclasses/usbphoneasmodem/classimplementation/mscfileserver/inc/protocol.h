// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <e32base.h>			// C Class Definitions, Cleanup Stack
#include <e32def.h>				// T Type  Definitions
#include <e32std.h>    			// ELeave definition


class MProtocolBase;

class MTransportBase
	{
	public:
	virtual void SetupReadData(TPtr8& aData) = 0;
	virtual void SetupWriteData(TPtrC8& aData) = 0;
	virtual TInt Start() = 0;
	virtual TInt Stop() = 0;
	virtual void RegisterProtocol(MProtocolBase& aProtocol) = 0;
	virtual TInt BytesAvailable() = 0;
	virtual ~MTransportBase() {};
	};


class MProtocolBase
	{
	public:
	virtual void RegisterTransport(MTransportBase* aTransport) = 0;
	virtual TBool DecodePacket(TPtrC8& aData, TUint aLun) = 0;
	virtual TInt ReadComplete(TInt aError) = 0;
	virtual TInt Cancel() = 0;
	virtual void ReportHighSpeedDevice() {};
	virtual ~MProtocolBase() {};
	};


#endif // PROTOCOL_H

