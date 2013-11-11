/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main classes for Graphics Out of Memory Monitor
*
*/


#ifndef GOOMWSERVEVENTRECEIVER_H
#define GOOMWSERVEVENTRECEIVER_H

#include <w32std.h>
class CMemoryMonitor;

NONSHARABLE_CLASS(CWservEventReceiver) : public CActive
    {
public:
    CWservEventReceiver(CMemoryMonitor& aMonitor, RWsSession& aWs);
    ~CWservEventReceiver();
    void ConstructL();
private:
    void Queue();
    void DoCancel();
    void RunL();
private:
    CMemoryMonitor& iMonitor;
    RWsSession& iWs;
    RWindowGroup iWg;
    };

#endif /*GOOMWSERVEVENTRECEIVER_H*/
