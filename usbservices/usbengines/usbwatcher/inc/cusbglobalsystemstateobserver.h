/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header of CUsbGlobalSystemStateObserver
*
*/


#ifndef CUSBGLOBALSYSTEMSTATEOBSERVER_H
#define CUSBGLOBALSYSTEMSTATEOBSERVER_H

#include <e32std.h>
#include <e32base.h>
#include <e32property.h>

class CUsbWatcher;

NONSHARABLE_CLASS( CUsbGlobalSystemStateObserver ) : public CActive
{
public:
    static CUsbGlobalSystemStateObserver* NewL( CUsbWatcher& aUsbWatcher );
    static CUsbGlobalSystemStateObserver* NewLC( CUsbWatcher& aUsbWatcher );
    void Subscribe();
    virtual ~CUsbGlobalSystemStateObserver();

    /**
     * Read and return global system state
     * 
     * @since Series 60 3.2
     * @return systemstate
     */
    static TInt GlobalSystemState();

private: // From base class
    void RunL();
    TInt RunError( TInt aError );
    void DoCancel();
        
private:
    CUsbGlobalSystemStateObserver( CUsbWatcher& aUsbWatcher );
    void ConstructL();

private: // data
    RProperty iProperty;
    CUsbWatcher& iUsbWatcher;
};

#endif // CUSBGLOBALSYSTEMSTATEOBSERVER_H

// End of file
