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
* Description:  Header file for class CUsbDevConStarter
*
*/


#ifndef CUSBDEVCONSTARTER_H
#define CUSBDEVCONSTARTER_H

#include <e32base.h>

/**
 * class for starting usbdevcon process
 * This class will start usbdevcon process. It also supervises its
 * termination. 
 * @since S60 v5.0
 */
NONSHARABLE_CLASS( CUsbDevConStarter ) : public CActive
    {

public:

    //states for starting usbdevcon
    enum TUsbDevConStarterState
        {
        EIdle,
        EWaitingRendezvous,
        ERunning
        };

    static CUsbDevConStarter* NewL();

    /**
     * Destructor.
     */
    virtual ~CUsbDevConStarter();

    /**
     * Start usbdevcon process
     * The usbdevcon process is started, unless it is already running.
     *
     * @since S60 5.0
     */
    void Start();

    /**
     * Logon for usbdevcon process termination
     *
     * @since S60 5.0
     */
    void Logon();

    /**
     * StopRestarting prevents usbdevcon restart
     * If usbdevcon terminates, it is not restarted, if StopRestarting has
     * been called. 
     *
     * @since S60 5.0
     */
    void StopRestarting();

private: // From base class

    /**
     * From CActive.
     * Handles the process termination
     *
     * @since Series 60 5.0
     */
    void RunL();

    /**
     * From CActive.
     * Never called in this implementation
     *
     * @since Series 60 5.0
     * @param aError The error returned
     * @return error
     */
    TInt RunError( TInt aError );

    /**
     * From CActive
     * Cancels outstanding request.
     * Note that process may be running after Cancel. If Start is called
     * after Cancel, another process will be started. 
     *
     * @since Series 60 5.0
     */
    void DoCancel();

private:

    CUsbDevConStarter();

    void ConstructL();

private: // data

    /**
     * Handle to UsbDevCon process
     */
    RProcess iProcess;
    
    /**
     * Is set to run. 
     */
    TBool iStarted;

    /**
     * State of starting usbdevcon
     */
    TUsbDevConStarterState iState;
    };

#endif // CUSBDEVCONSTARTER_H

// End of file
