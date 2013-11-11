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
// Description: Implements a Symbian OS server that exposes the RUsbMassStorage API
// 
// 

#ifndef MSCFILESERVER_H
#define MSCFILESERVER_H

_LIT(KUsbMsSvrPncCat, "CUsbMsServer");

enum TUsbPanicServer
	{
	EMsClientInvalidSessCount,
	EMsControlInterfaceBadState,
	EMsControlInterfaceStillActive,
	EMsBulkOnlyStillActive,
	EMsWrongEndpoint,
	EMsWrongLength
	};

//
// Forward declarations
//
class CMscFileController;

/**
 The CMscFileServer class
 Implements a Symbian OS server that exposes the RUsbMassStorage API
 */
class CMscFileServer : public CPolicyServer
	{
public:
	static CMscFileServer* NewLC();
	virtual ~CMscFileServer();

private:
    CMscFileServer();
    void ConstructL();

public:
	virtual CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	void Error(TInt aError);

	inline CMscFileController& Controller() const;

	void IncrementSessionCount();
	void DecrementSessionCount();
	inline TInt SessionCount() const;

    static TInt ThreadFunction( TAny* aStarted );
    
private:
    static void ThreadFunctionL();
    
private:
	CMscFileController* iController;
	TInt iSessionCount;
	};

#include "mscfileserver.inl"

#endif // MSCFILESERVER_H


