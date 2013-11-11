/*
* Copyright (c) 2006 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Control the asynchronouse request from serviceplugin to 
*                client side. 
*
*/


#ifndef USBLCDACTIVE_H
#define USBLCDACTIVE_H

#include <locodbearerpluginobserver.h>


/**
 *  Base Active Object class 
 *
 *  Its instance will be used to control Asynchronous request
 *  from ECom plugin to FaxModem server
 *
 */
NONSHARABLE_CLASS(CUsbLcdActive) : public CActive
    {

public:

    static CUsbLcdActive* NewL(MLocodBearerPluginObserver& aObserver); 

    virtual ~CUsbLcdActive();

protected:

    // from base class CActive
    /**
     * From CActive. 
     * Cancel current activity.
     */
    void DoCancel();

    /**
     * From CActive. 
     * RunL() function.
     */
    void RunL();

    /**
     * From CActive. 
     * Called if RunL() leaves. Retry listening after error.
     */
    TInt RunError( TInt aError );

private:

    CUsbLcdActive(MLocodBearerPluginObserver& aObserver);

    void ConstructL();
    void Start();
    /**
     * Handle the change of selected USB personality.
     * @param aNewPersonalityId current selected USB personality ID.
     */
    void HandleUsbPersonalityChange( TInt aNewPersonalityId );

private: // data

    MLocodBearerPluginObserver& iObserver;
    RProperty iProperty;
    };


#endif // USBLCDACTIVE_H
