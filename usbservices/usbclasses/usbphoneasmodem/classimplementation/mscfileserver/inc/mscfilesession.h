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
// Description: Implements a Session of a Symbian OS server for the RUsbMassStorage API
// 
// 

#ifndef MSCFILESESSION_H
#define MSCFILESESSION_H

#include <e32std.h>
#include <e32base.h>
#include "usbmscfileshared.h"

_LIT(KUsbMsCliPncCat, "UsbMs-Client");

enum TUsbMsPanicClient
    {
    EUsbMsPanicIllegalIPC
    };
//
// Forward declarations
//
class CMscFileServer;

/**
 The CMscFileSession class
 Implements a Session of a Symbian OS server for the RUsbMassStorage API
 */
class CMscFileSession : public CSession2
	{
public:
	static CMscFileSession* NewL( CMscFileServer& aServer );
	virtual ~CMscFileSession();

private:
    CMscFileSession( CMscFileServer& aServer );
    void ConstructL();

protected:
	virtual void ServiceL( const RMessage2& aMessage );
	
private:
    void DispatchMessageL( const RMessage2& aMessage );
	void SetupLogicalUnitL( const RMessage2& aMessage );
	void StartL( const RMessage2& aMessage );
	TInt Stop();
	TInt Shutdown();
 	void GetMsConfigL(const RMessage2& aMessage, TMassStorageConfig& aMsStorage);
	
private:
	CMscFileServer& iMscFileServer;
	};

#endif //MSCFILESESSION_H

