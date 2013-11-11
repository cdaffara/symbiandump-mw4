/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Declares a interface for metrics recorder class
*
*/



#ifndef __HUIPROBE_H__
#define __HUIPROBE_H__

#include <e32base.h>

class MHuiSessionObject;

/**
 * Interface for Hitchcock Toolkit Probe object.
 */
class MHuiProbe
    {
public:

    /**
     * Event enums for object lifecycle changes.
     */
    enum TLifecycleEvent
        {
        ELifecycleEventConstructed,
        ELifecycleEventDestructed,
        ELifecycleEventReleased,
        ELifecycleEventRestored,
        };

    /**
     * Location enums for points of interest in program flow.
     */
    enum TProgramFlowPoint
        {   
        EProgramFlowPointRefreshPrepareDraw,
        EProgramFlowPointRefreshDraw,
        EProgramFlowPointRefresh,
        };

    /**
     * Event enums for entering to or exiting from the point of interest in program flow.
     */
    enum TProgramFlowEvent
        {   
        EProgramFlowEventEnter,
        EProgramFlowEventExit,
        };

    /**
     * Report change in object life cycle
     */
    virtual void ReportObjectLifeCycleEvent(
        const MHuiSessionObject& aObject,
        TLifecycleEvent aLifecycleEvent) = 0;

    /**
     * Report enter or exit to/from "point of interest" in program flow
     */
    virtual void ReportProgramFlowEvent(
        TProgramFlowPoint aProgramFlowPoint,
        TProgramFlowEvent aProgramFlowEvent) = 0;

    /**
     * Report enter or exit to/from "point of interest" in program flow, which
     * resides inside session-aware object
     */
    virtual void ReportProgramFlowEvent(
        const MHuiSessionObject& aSessionObject,
        TProgramFlowPoint aProgramFlowPoint,
        TProgramFlowEvent aProgramFlowEvent) = 0;

    /**
     * Explocitly report the frame rate
     */
    virtual void ReportFrameRate(TInt aFrameRate) const = 0;

    /**
     * Associate the given object with current session (e.g. session
     * which is currently handling a message).
     */
    virtual void AssociateWithCurrentSession(MHuiSessionObject& aObject) = 0;

    };

#endif // __HUIPROBE_H__

