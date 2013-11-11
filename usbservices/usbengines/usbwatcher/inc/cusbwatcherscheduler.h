/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active scheduler for server
*
*/


#ifndef CUSBWATCHERSCHEDULER_H
#define CUSBWATCHERSCHEDULER_H

#include <e32base.h>

class CUsbWatcherServer;

/**
 * The CUsbWatcherScheduler class
 *
 * Implements an Active Scheduler for the server to use. This is necessary
 * in order to provide an Error() function which does something useful instead
 * of panicking.
 */
NONSHARABLE_CLASS( CUsbWatcherScheduler ) : public CActiveScheduler
    {
public:
    static CUsbWatcherScheduler* NewL();
    ~CUsbWatcherScheduler();

    void SetServer(CUsbWatcherServer& aServer);

private:
    inline CUsbWatcherScheduler() {};
    // from CActiveScheduler
    void Error( TInt aError ) const;

public:
    CUsbWatcherServer* iServer;
    };

#endif

// End of file
