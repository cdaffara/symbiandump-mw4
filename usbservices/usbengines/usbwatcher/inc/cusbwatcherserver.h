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
* Description:  Header file for CUsbWatcherServer class
*
*/


#ifndef CUSBWATCHERSERVER_H
#define CUSBWATCHERSERVER_H

//
// Forward declarations
//
class CUsbWatcher;

/**
 * The CUsbWatcherServer class
 *
 * Implements a Symbian OS server that exposes the RUsbWatcher API
 */
NONSHARABLE_CLASS( CUsbWatcherServer ) : public CPolicyServer
    {
public:
    static CUsbWatcherServer* NewLC();
    virtual ~CUsbWatcherServer();

    virtual CSession2* NewSessionL( const TVersion &aVersion,
            const RMessage2& aMessage ) const;
    void Error( TInt aError );

    CUsbWatcher& Watcher() const;
        
protected:
    CUsbWatcherServer();
    void ConstructL();
        
private:
    CUsbWatcher* iUsbWatcher;
    };

#include "cusbwatcherserver.inl"

#endif

// End of file

