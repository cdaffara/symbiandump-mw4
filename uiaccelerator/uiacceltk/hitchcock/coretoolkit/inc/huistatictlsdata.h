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
* Description:   Declares the private data stored to the thread local storage
*                by CHuiStatic.
*
*/



#ifndef __HUISTATICTLSDATA_H__
#define __HUISTATICTLSDATA_H__


// For testing purposes, insert additional test data into TLS.
// Notice, that TTlsData object is inserted into TLS. So, test
// object needs to be added into TTLSData and not inserted directly
// into TLS.
#define AMT_CONTROL() static_cast<TTlsData*>(Dll::Tls())->iAlfModuleTestDataControl
#include "alfmoduletest.h"


LOCAL_D const TInt KMaxClocks = 10;


class TFakeProbe;
class CAppFwProxy;
#ifdef HUI_DEBUG_TRACK_DRAWING
class CAlfCommandDebug;   
 #ifndef __WINSCW__
extern TTlsData globalWriteableData;
 #endif
#endif
/**
 * Thread local storage space. Writable static data is not supported in
 * Symbian, so static data is stored in this struct.
 */

extern void CleanupWg(TAny* aWg);

NONSHARABLE_STRUCT(TTlsData)
    {
    CHuiRenderPlugin* iRenderer;

    /** The render surface that has been made current. */
    MHuiRenderSurface* iCurrentRenderSurface;

    /** Primary environment. */
    CHuiEnv* iPrimaryEnv;

    /** Realtime clock. */
    TTime iRealUniversalTime;

    /** Internal absolute clock. */
    TTime iUniversalTime;

    /** Flags that tells if this the first time update is done. */
    TBool iIsFirstUpdateTime;

    /** Time of the first invocation of UpdateTime. */
    TTime iFirstUpdateTime;

    /** Microseconds elapsed in the internal absolute clock since the last
        UpdateTime(). */
    TInt64 iInternalElapsed;
    
    /** Microseconds elapsed from previous update time until pause was called. */
    TInt64 iInternalElapsedBeforePausing;

    /** Time pause flag. */
    TBool iTimePaused;

    /** Time factor. */
    TReal32 iTimeFactor;

    /** Counter for unique identifier numbers. */
    TInt iIdCounter;

    /** Frame counter since application started. */
    TUint iFrameCounter;
    
    /** Frames counted since FPS was last calculated. */
    TUint iCurrentFrameCounter;

    /** Time when the first frame was shown. */
    //TTime iFirstFrameTime;

    //TTime iPreviousFrameTime;

    /** Time when the current frame rate was last calculated. */
    TTime iLastFpsTime;

    /** The last recorded FPS. */
    TReal32 iLastFps;

    /** Message logger. */
    RFile iLogFile;
    TBool iLogEnabled;

    /** A list of clocks used for timing events.*/
    TTime iClocks[KMaxClocks];

    /** How long it takes, in milliseconds, for a layout transition to complete. */
    TInt iLayoutTransitionTime;
    
    MHuiProbe* iProbe;
    TFakeProbe* iFakeProbe;

    // Shared 
    RWsSession* iWsSession;
    RPointerArray<RWindowGroup> iWindowGroups;
    RPointerArray<CWsScreenDevice> iScreenDevices;    
    RFs* iFs;
    
    CAppFwProxy* iAppFwProxy;

#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
    // For testing purposes, test data needs to be included into TLS object.
    CAlfModuleTestDataControl* iAlfModuleTestDataControl;
#endif

#ifdef HUI_DEBUG_TRACK_DRAWING
    CAlfCommandDebug* iCommandDebugger;   
#endif

    CWsScreenDevice* WsScreenDevice(TInt aScreenNumber)
        {
        if (aScreenNumber < iScreenDevices.Count())
            {
            return iScreenDevices[aScreenNumber];
            }
        return 0;
        }       
    
    RWindowGroup* RootWin(TInt aScreenNumber)
        {
        if (aScreenNumber < iWindowGroups.Count())
            {
            return iWindowGroups[aScreenNumber];
            }
        return 0;
        }       
      
    TTlsData():
        iRenderer(0),iCurrentRenderSurface(0),iPrimaryEnv(0),iIsFirstUpdateTime(0),
        iInternalElapsed(0),iInternalElapsedBeforePausing(0),iTimePaused(0),iTimeFactor(0),
        iIdCounter(0),iFrameCounter(0),iCurrentFrameCounter(0),iLastFps(0),iLogEnabled(0),
        iLayoutTransitionTime(0),iProbe(0),iFakeProbe(0),iWsSession(0),iFs(0),iAppFwProxy(0)
        {  
        }
                
     void DoInitL()
        {
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
    // For testing purposes, test data needs to be included into TTlsData object because
    // TTlsData object itself is inserted into TLS.
    iAlfModuleTestDataControl = new(ELeave) CAlfModuleTestDataControl();
    User::LeaveIfError( iAlfModuleTestDataControl->OpenGlobalObjects() );
#endif

        CCoeEnv* coe = CCoeEnv::Static();
        if (coe)
            {
            iWsSession = &coe->WsSession();
            iFs = &coe->FsSession();
            iScreenDevices.Append(coe->ScreenDevice()); // Todo: Multiple display support    
            iWindowGroups.Append(&coe->RootWin()); // Todo: Multiple display support    
            return;
            }
            
        if(!RFbsSession::GetSession())  
            {
            User::LeaveIfError(RFbsSession::Connect());
            }

        iFs = new (ELeave) RFs;
        User::LeaveIfError(iFs->Connect());    
        
        iWsSession = new (ELeave) RWsSession;
        User::LeaveIfError(iWsSession->Connect());    
        
        CWsScreenDevice* screenDevice =new(ELeave) CWsScreenDevice(*iWsSession);
        CleanupStack::PushL(screenDevice); 
        User::LeaveIfError(screenDevice->Construct(0)); // For main display only
        User::LeaveIfError(iScreenDevices.Append(screenDevice));
        CleanupStack::Pop();
        
        RWindowGroup* windowGroup = new(ELeave) RWindowGroup(*iWsSession);
        CleanupStack::PushL(TCleanupItem(CleanupWg, (TAny*)windowGroup)); 
        User::LeaveIfError(windowGroup->Construct((TUint32)screenDevice, EFalse,screenDevice));                   
        CleanupClosePushL(*windowGroup);
        User::LeaveIfError(iWindowGroups.Append(windowGroup));
        CleanupStack::Pop(2);
        }

    void Disconnect()
        {
        if (CCoeEnv::Static())
            {
            iWindowGroups.Close();
            iScreenDevices.Close();
            return;
            }
        
        for(;iWindowGroups.Count();)
            {    
            iWindowGroups[0]->Close();
            delete iWindowGroups[0];
            iWindowGroups.Remove(0);
            }
            
        for(;iScreenDevices.Count();)
            {
            delete iScreenDevices[0];
            iScreenDevices.Remove(0);
            }
        
        if (iWsSession)
            iWsSession->Close();        
        delete iWsSession;
        iWsSession = 0;
        
        if (iFs)
            iFs->Close();        
        delete iFs;
        iFs = 0;

        RFbsSession::Disconnect();
        
#ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
        delete iAlfModuleTestDataControl;
        iAlfModuleTestDataControl = NULL;
#endif        
        }

#ifdef HUI_DEBUG_TRACK_DRAWING
    static CAlfCommandDebug* CommandDebugger() 
        {
        #ifndef __WINSCW__
        TTlsData* data = &globalWriteableData;
        #else
        TTlsData* data = static_cast<TTlsData*>(Dll::Tls());
        #endif

        return data->iCommandDebugger;
        }   
#endif
    
    };


#endif
