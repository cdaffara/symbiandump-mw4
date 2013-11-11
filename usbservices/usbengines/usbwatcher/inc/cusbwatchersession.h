/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Session class for USB Watcher
*
*/


#ifndef CUSBWATCHERSESSION_H
#define CUSBWATCHERSESSION_H

#include <e32std.h>
#include <e32base.h>
#include "musbwatchernotify.h"
//
// Forward declarations
//
class CUsbWatcherServer;
class CUsbWatcher;

NONSHARABLE_CLASS( CUsbWatcherSession ) : public CSession2,
    public MUsbWatcherNotify
    {
public:
    static CUsbWatcherSession* NewL(CUsbWatcherServer* aServer);
    virtual ~CUsbWatcherSession();

    // CSession
    virtual void ServiceL(const RMessage2& aMessage);
        
    virtual void Notify(TInt iPersonalityId, TInt aStatus); 

protected:
    CUsbWatcherSession(CUsbWatcherServer* aServer);
    void ConstructL();

    void DispatchMessageL(const RMessage2& aMessage);

private:

    TInt SetPersonality(const RMessage2& aMessage, TBool& aComplete);
    TInt CancelSetPersonality(const RMessage2& aMessage, 
            TBool& aComplete);
    TInt SetPreviousPersonality(const RMessage2& aMessage,
            TBool& aComplete);
    TInt SetPreviousPersonalitySync(const RMessage2& aMessage, 
            TBool& aComplete);    
    TInt CancelSetPreviousPersonality(const RMessage2& aMessage, 
            TBool& aComplete);
    TInt SetPreviousPersonalityOnDisconnect(const RMessage2& aMessage, 
            TBool& aComplete);

    CUsbWatcherServer* iUsbWatcherServer;
    RMessage2 iSetPersonalityMessage;
    RMessage2 iCancelSetPersonalityMessage;
    RMessage2 iSetPreviousPersonalityMessage;
    RMessage2 iCancelSetPreviousPersonalityMessage;
    TBool iSetPersonalityOutstanding;
    TBool iCancelSetPersonalityOutstanding;

    TBool iSetPreviousPersonalityOutstanding;
    TBool iCancelSetPreviousPersonalityOutstanding;
    };

#endif

// End of file
