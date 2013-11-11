/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An active object which watches for app close actions successfully completing
*
*/


#ifndef GOOMAPPCLOSEWATCHER_H_
#define GOOMAPPCLOSEWATCHER_H_

#include <e32base.h>

class CGOomCloseApp;
class TApaTask;

/**
 *  This class implements an active object which watches for app close actions successfully completing.
 *  
 *  This watcher object logs-on to the thread of the application being closed and completes 
 *  the app close action when the thread dies.  
 *
 *  @lib oommonitor.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CGOomAppCloseWatcher) : public CActive
    {
public:
    CGOomAppCloseWatcher(CGOomCloseApp& aMonitor);
    ~CGOomAppCloseWatcher();
    void Start(const TApaTask& aTask);
private:
    void DoCancel();
    void RunL();
    
private: //data

    /**
     * A reference to the owning close app action
     */
	CGOomCloseApp& iMonitor;

    /**
     * A handle to the main thread of the application to be closed.
     */
    RThread iThread;
    
    /**
     * A backup of the orginal priority of the application's process
     */
    TProcessPriority iOriginalProcessPriority;
    };

#endif /*GOOMAPPCLOSEWATCHER_H_*/
