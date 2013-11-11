/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Monitors Obex ServiceMan errors.
*
*/

#ifndef COBEXSMWATCHER_H
#define COBEXSMWATCHER_H

#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <e32property.h> //Publish & Subscribe header
#include "mobexsmwatcher.h"

/**
 * CObexSMWatcher class
 * Subscribes to ObexServiceMan property changes to monitor initialization errors
 */
class CObexSMWatcher : public CActive
    {
public:
    static CObexSMWatcher* NewL(MObexSMObserver& aObserver);
    virtual ~CObexSMWatcher();

    // From CActive
    virtual void RunL();
    virtual void DoCancel();
    
protected:
    CObexSMWatcher(MObexSMObserver& aObserver);
    void ConstructL();
    
private:
    // An observer object for obexseviceman P&S keys
    MObexSMObserver& iObserver;
    // A property object for obexseviceman P&S keys
    RProperty iObexSMPostInit;
    };

#endif
