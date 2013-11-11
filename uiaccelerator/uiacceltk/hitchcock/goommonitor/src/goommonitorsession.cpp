/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client/server interface for GOOM Monitor.
*
*/


#include <coemain.h>
#include <goommonitorsession.h>
#include "goommonitorclientserver.h"
#include "goompanic.h"
#include "goomtraces.h"

enum TGoomClientInternalFlags{
    EGoonCFlagsNone,
    EGoonCFlagsConnected = 1
    };

#define ASYNCH_NOTIFICATION_ONLY

EXPORT_C TInt RGOomMonitorSession::Connect()
    {
    FUNC_LOG;
    iFlags = EGoonCFlagsNone; // initialization
    if (CreateSession(KGraphicsMemoryMonitorServerName, TVersion(0,0,0)) == KErrNone)
        {
        iFlags = EGoonCFlagsConnected;
        }    
            
    // always return KErrNone for now        
    return KErrNone;
    }

EXPORT_C TBool RGOomMonitorSession::IsConnected()
    {
    FUNC_LOG;
    return iFlags&EGoonCFlagsConnected;
    }

EXPORT_C TInt RGOomMonitorSession::RequestFreeMemory(TInt aBytesRequested)
    {
    FUNC_LOG;
    if (IsConnected())
        {
        TIpcArgs p(aBytesRequested,0);
        return SendReceive(EGOomMonitorRequestFreeMemory, p);
        }
    return KErrCouldNotConnect;    
    }

EXPORT_C void RGOomMonitorSession::MemoryAllocationsComplete()
    {
    FUNC_LOG;
    if (IsConnected())
        {
        SendReceive(EGOomMonitorMemoryAllocationsComplete);
        }
    }

EXPORT_C TInt RGOomMonitorSession::RequestOptionalRam(TInt aBytesRequested, TInt aMinimumBytesNeeded, TInt aPluginId, TInt& aBytesAvailable)
    {
    FUNC_LOG;
    if (IsConnected())
        {
        TIpcArgs p(aBytesRequested, aMinimumBytesNeeded, aPluginId, aBytesAvailable);
        TInt ret = SendReceive(EGOomMonitorRequestOptionalRam, p);
        if (ret >= 0)
            {
            aBytesAvailable = ret;
            ret = KErrNone;
            }

        return ret;
        }
    return KErrCouldNotConnect;
    }    

EXPORT_C void RGOomMonitorSession::RequestOptionalRam(TInt aBytesRequested, TInt aMinimumBytesNeeded, TInt aPluginId, TRequestStatus& aStatus)
    {
    FUNC_LOG;

    if (IsConnected())
        {
        TIpcArgs p(aBytesRequested, aMinimumBytesNeeded, aPluginId);
        SendReceive(EGOomMonitorRequestOptionalRam, p, aStatus);
        }
    else
        {
        TRequestStatus* sptr = &aStatus;
        User::RequestComplete(sptr, KErrNone);
        }    
    }

EXPORT_C void RGOomMonitorSession::RequestFreeMemory(TInt aBytesRequested, TRequestStatus& aStatus)
    {
    FUNC_LOG;

    if (IsConnected())
        {
        TIpcArgs p(aBytesRequested,0);
        SendReceive(EGOomMonitorRequestFreeMemory, p, aStatus);
        }
    else
        {
        TRequestStatus* sptr = &aStatus;
        User::RequestComplete(sptr, KErrNone);
        }    
    }

EXPORT_C void RGOomMonitorSession::CancelRequestFreeMemory()
    {
    FUNC_LOG;
    if (IsConnected())
        {
        SendReceive(EGOomMonitorCancelRequestFreeMemory, TIpcArgs());
        }
    }

EXPORT_C void RGOomMonitorSession::ThisAppIsNotExiting(TInt aWgId)
    {
    FUNC_LOG;
    if (IsConnected())
        {
        TIpcArgs p(aWgId);
        SendReceive(EGOomMonitorThisAppIsNotExiting, p);
        }
    }

//TODO should this be asynchronous - can be called from a goom plugin, which is called from goom - deadlock ?
EXPORT_C void RGOomMonitorSession::SetGOomPriority(TGOomPriority aPriority)
    {
    FUNC_LOG;
    if (!IsConnected())
        {
        return;
        }

    CCoeEnv* coeEnv = CCoeEnv::Static();

    __ASSERT_DEBUG(coeEnv, GOomMonitorPanic(KNoCoeEnvFound));

    if (coeEnv)
        {
        TInt wgId = coeEnv->RootWin().Identifier();
        TIpcArgs p(wgId);
        switch (aPriority)
            {
            case EGOomPriorityNormal:
                SendReceive(EGOomMonitorSetPriorityNormal, p);
                break;
            case EGOomPriorityHigh:
                SendReceive(EGOomMonitorSetPriorityHigh, p);
                break;
            case EGOomPriorityBusy:
                SendReceive(EGOomMonitorSetPriorityBusy, p);
                break;
            default:
                GOomMonitorPanic(KGOomInvalidPriority);
                break;
            }
        }
    }

EXPORT_C void RGOomMonitorSession::ApplicationAboutToStart(const TUid& aAppUid)
    {
    FUNC_LOG;
    if (IsConnected())
        {
#ifdef ASYNCH_NOTIFICATION_ONLY            
        SendReceive(EGoomMonitorAppAboutToStart, TIpcArgs(aAppUid.iUid));
#else
        SendReceive(EGOomMonitorRequestFreeMemory, TIpcArgs(0,aAppUid.iUid));
#endif
        }
    }
    
    
EXPORT_C void RGOomMonitorSession::UsesAbsoluteMemTargets(TBool aUseAbsoluteAmount)
    {
    FUNC_LOG;    
    if (IsConnected())
        {
        SendReceive(EGoomMonitorAppUsesAbsoluteMemTargets, TIpcArgs(aUseAbsoluteAmount));
        }    
    }    

EXPORT_C TInt RGOomMonitorSession::Connect(TRequestStatus& aStatus)
    {
    FUNC_LOG;
    iFlags = EGoonCFlagsNone; // initialization
    
    TInt err = CreateSession( KGraphicsMemoryMonitorServerName, 
        TVersion( 0, 0, 0 ), 1, EIpcSession_Unsharable, 0, &aStatus ); 
        
    if ( err == KErrNone )    
        {
        iFlags = EGoonCFlagsConnected;
        }    
            
    return err;
    }

EXPORT_C void RGOomMonitorSession::AppAboutToStart(TRequestStatus& aStatus, const TUid& aAppUid)
    {
    FUNC_LOG;
    if (IsConnected())
        {
#ifdef ASYNCH_NOTIFICATION_ONLY            
        SendReceive(EGoomMonitorAppAboutToStart, TIpcArgs(aAppUid.iUid), aStatus);
#else
        SendReceive(EGOomMonitorRequestFreeMemory, TIpcArgs(0,aAppUid.iUid), aStatus);
#endif
        }
    }

EXPORT_C TInt RGOomMonitorSession::RequestHWRendering()
    {
    FUNC_LOG;
    if (IsConnected())
        {        
        RDebug::Printf("RAN_DEBUG : GOOM RGOomMonitorSession::RequestHWRendering");
        return SendReceive(EGOomMonitorRequestHWRendering);
        }
    return KErrCouldNotConnect;    
    }
