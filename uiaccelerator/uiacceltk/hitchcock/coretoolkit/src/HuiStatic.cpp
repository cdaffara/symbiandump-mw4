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
* Description:   Implements CHuiStatic, static data access and utility 
*                functions such as timing.
*
*/



#include "uiacceltk/HuiStatic.h"  // Class definition
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiEnv.h"
#include "HuiRenderPlugin.h"
#include "huistatictlsdata.h"
#include "uiacceltk/HuiProbe.h"
#include <e32math.h>
#include <eikenv.h>
#include <flogger.h>
#include <AknsUtils.h>
#include <AknUtils.h>
#include <AknBidiTextUtils.h>
#include <bidivisual.h>
#include "alfappfwproxyops.h"

#ifdef HUI_DEBUG_TRACK_DRAWING
#include "alfloggingconfiguration.h"
#include "alfcommanddebug.h"
#endif

// Global writeable data, used in HW instead of TLS which relatively slow
#ifndef __WINSCW__
TTlsData globalWriteableData;
#endif

void CleanupWg(TAny* aWg)
    {   
    RWindowGroup* wg = static_cast<RWindowGroup*>(aWg);
    delete wg;
    }

/** Dummy implementation for HuiProbe, to be used when external probe is not set */
NONSHARABLE_CLASS(TFakeProbe) : public MHuiProbe
    {
public:
    virtual void ReportObjectLifeCycleEvent(const MHuiSessionObject&, TLifecycleEvent) {}
    virtual void ReportProgramFlowEvent(TProgramFlowPoint, TProgramFlowEvent) {}
    virtual void ReportProgramFlowEvent(const MHuiSessionObject&, TProgramFlowPoint, TProgramFlowEvent) {}
    void ReportFrameRate(TInt) const {}
    void AssociateWithCurrentSession(MHuiSessionObject&) {}
    };


NONSHARABLE_CLASS(CAppFwProxy): public CBase
    {
public:
    class CAlfGuardedSynchCall: public CActive
        {
        public:
        CAlfGuardedSynchCall(const TAlfCommandParams& aParams):CActive(CActive::EPriorityStandard), iPckg(aParams), iPtr(0,0)
            {
            CActiveScheduler::Add(this);
            iStatus = KErrNotFound;
            }

        static TInt DoGuardedOpErrL(const TAlfCommandParams& aInParams, 
                                  TDes8& aOutParams,
                                  RNotifier& aNotifier,
                                  const TDesC8* aInParams2 = 0 // ugly extension, really
                                  )
            {
            CAlfGuardedSynchCall* me = new (ELeave) CAlfGuardedSynchCall(aInParams);

            CleanupStack::PushL(me);
            User::LeaveIfError(me->iTimer.CreateLocal());
            me->iRetBuf = aOutParams.AllocL();
            me->iPtr.Set((TUint8*)me->iRetBuf->Ptr(),0,aOutParams.MaxSize());
            if (aInParams2)
                {
                me->iAltInBuf = aInParams2->AllocL();
                }
            CleanupStack::Pop(); // me
             
            if (aInParams2)
                {
                aNotifier.StartNotifierAndGetResponse(me->iStatus, 
                                                      TUid::Uid(KAlfAppFwProxyUid), 
                                                      *me->iAltInBuf, me->iPtr);
                }
            else
                {
                aNotifier.StartNotifierAndGetResponse(me->iStatus, 
                                                      TUid::Uid(KAlfAppFwProxyUid), 
                                                      me->iPckg, me->iPtr);
                }                        
            
            me->iTimer.After(me->iTimerStatus, 100000); // 100ms
            User::WaitForRequest(me->iStatus, me->iTimerStatus);
            TBool timedOut = (me->iStatus == KRequestPending);
            
            me->iTimer.Cancel(); // Cancel timer anyway
                        
            if (!timedOut)
                {
                TInt err = me->iStatus.Int();  
                User::WaitForRequest(me->iTimerStatus);
                aOutParams.Copy(*me->iRetBuf);    
                delete me;
                return err;
                }
            else 
                {
                me->SetActive(); 
                return KErrTimedOut;
                }
            }

        static void DoGuardedOpL(const TAlfCommandParams& aInParams, 
                                  TDes8& aOutParams,
                                  RNotifier& aNotifier,
                                  const TDesC8* aInParams2 = 0 // ugly extension, really
                                  )
            {
            TInt err = DoGuardedOpErrL(aInParams, aOutParams, aNotifier, aInParams2);
            User::LeaveIfError(err);
            }

        static TInt DoGuardedOp(const TAlfCommandParams& aInParams, 
                                  TDes8& aOutParams,
                                  RNotifier& aNotifier)
            {
            TInt ret = KErrNone;    
            TInt err = KErrNone;
            TRAP(ret, err = DoGuardedOpErrL(aInParams, aOutParams, aNotifier);)
            if (ret == KErrNone)
                {
                ret = err;
                }
            return ret;
            }    

        ~CAlfGuardedSynchCall()
            {
            Cancel();
            delete iRetBuf;
            delete iAltInBuf;
            iTimer.Close(); 
            }
            
        void RunL()
            {           
            delete this;
            }
            
        void DoCancel()
            {
            // Cancel will call User::WaitForRequest for iStatus (if this ao is active)
            }
                
        TPckgC<TAlfCommandParams> iPckg;
        HBufC8* iRetBuf;
        HBufC8* iAltInBuf;
        TPtr8 iPtr;
        RTimer iTimer;
        TRequestStatus iTimerStatus;
        };

    class CAlfLayoutListener: public CActive
        {
        public:
        CAlfLayoutListener(CAppFwProxy& aNotif):CActive(CActive::EPriorityStandard), iNotif(aNotif)
            {
            CActiveScheduler::Add(this);
            iStatus = KErrNotFound;    
            }

        ~CAlfLayoutListener()
            {
            Cancel();    
            }
            
        void RunL()
            {           
            iNotif.iLayoutMirrored = (iStatus.Int() > 0);         
            SetActive();
            TAlfCommandParams params={EAlfIsMirrorred,0,0,0};
            iParams() = params;
            iNotif.iNotif.StartNotifierAndGetResponse(iStatus,TUid::Uid(KAlfAppFwProxyUid), iParams, iAwkwardApiDummy);
            }
    
        void DoCancel()
            {
            // just wait for notifier completion    
            User::WaitForRequest(iStatus);    
            }    

        private:
            CAppFwProxy& iNotif;

            TBuf8<1> iAwkwardApiDummy;
            TPckgBuf<TAlfCommandParams> iParams;
            };    

    CAppFwProxy():iLayoutMirrored(KErrNotFound)
        {}
        
    ~CAppFwProxy()
        {
        if (iConnected)
            {
            iNotif.Close();
            delete iLayoutUpdateAo;
			}
		}
    
    TBool Connect()
        {
        if (!iConnected)
            {
            // perhaps should check also whether eikon server / notifier server exists..
            // This is currently unsafe (synch) call to a process that may be blocked by window server, keep fingers crossed
            iConnected = (iNotif.Connect() == KErrNone);
            }
        return iConnected;
        }
    
    TBool LayoutMirrored()
        {                
        if ( Connect() && iLayoutMirrored == KErrNotFound)
            {
            TRequestStatus status;
            TAlfCommandParams inParams={EAlfIsMirrorred,0,0,0};
            TBuf8<1> outParams;
            TInt ret = CAlfGuardedSynchCall::DoGuardedOp( inParams, outParams , iNotif );

            // order updates
            iLayoutUpdateAo = new CAlfLayoutListener(*this);
            if (iLayoutUpdateAo)
                {
                TRAP_IGNORE(iLayoutUpdateAo->RunL());
                }
            
            iLayoutMirrored = ret > 0;
            }
     
        return iLayoutMirrored==KErrNotFound?EFalse:iLayoutMirrored;    
        }

    TInt SoftKeyOrientation()
        {
        TInt ret = 0;
        if ( Connect() )
            {
            TAlfCommandParams inParams={EAlfCbaLocation,0,0,0};
            TBuf8<1> outParams;
            ret = CAlfGuardedSynchCall::DoGuardedOp( inParams, outParams , iNotif );
            }
        
        return ret;    
        }

    TRect LayoutMetricsRect(TInt aType)
        {
        TRect ret=TRect(0,0,0,0);
        if ( Connect() )
            {
            TAlfCommandParams inParams={EAlfLayoutMetricsRect,aType,0,0};
            TPckg<TRect> outParams(ret);
            CAlfGuardedSynchCall::DoGuardedOp( inParams, outParams , iNotif );
            }
        
        return ret;    
        }
  
    TInt GetCachedColor(TRgb& aRgb, const TAknsItemID& aID,
            const TInt aIndex )
        {
        if ( Connect() )
            {
            TAlfCommandParams inParams={EAlfGetCachedColor,aID.iMajor,aID.iMinor,aIndex};
            TPckg<TRgb> outParams(aRgb);
            return CAlfGuardedSynchCall::DoGuardedOp( inParams, outParams , iNotif);
            }
    
        return KErrCouldNotConnect;    
        }
    
    void GetSkinBitmapL(CFbsBitmap* aBitmap, CFbsBitmap* aMask, const TAknsItemID& aID, const TDesC& aFileName, TInt aBitmapId, 
                        TInt aMaskId, const TSize& aSize, TScaleMode aScaleMode)
        {
        if ( Connect() )
            {
            TAlfCommandParams fakeParams={0,0,0,0};    
            TAlfCommandParams2 inParams={EAlfGetSkinBitmap,aID.iMajor,aID.iMinor,aBitmapId,aMaskId,aFileName, aSize, aScaleMode};
            TPckgC<TAlfCommandParams2> inBuf(inParams);
            TInt2 handles = {0,0};
            TPckg<TInt2> outParams(handles);
            CAlfGuardedSynchCall::DoGuardedOpL( fakeParams, outParams , iNotif, &inBuf );
            if (!handles.iInt1)
                {
                User::Leave(KErrNotFound); 
                }   
            User::LeaveIfError(aBitmap->Duplicate(handles.iInt1));
            if ( aMask && handles.iInt2 )
                {
                User::LeaveIfError(aMask->Duplicate(handles.iInt2));                    
                }
            }
        }

    void GetSkinBackgroundBitmapL(CFbsBitmap* aBitmap, const TAknsItemID& aID )
        {
        if ( Connect() )
            {
            TAlfCommandParams inParams={EAlfGetSkinBackgroundBitmap,aID.iMajor,aID.iMinor,0};
            TInt handle = 0;
            TPckg<TInt> outParams(handle);
            CAlfGuardedSynchCall::DoGuardedOpL( inParams, outParams , iNotif );
            User::LeaveIfError(aBitmap->Duplicate(handle));
            }
        }

    void GetCachedSkinItemDataArrayL(const TAknsItemID& aID, TInt aType, TAlfCachedSkinItemArray& aArray)  
        {
        if ( Connect() )
            {
            TAlfCommandParams inParams={EGetCachedSkinItemData,aID.iMajor,aID.iMinor,aType};
            TPckg<TAlfCachedSkinItemArray> outParams(aArray);
            CAlfGuardedSynchCall::DoGuardedOpL( inParams, outParams , iNotif );
            }
        }
        
    public: 
        TBool iLayoutMirrored;
        RNotifier iNotif;
        TBool iConnected;
        CAlfLayoutListener* iLayoutUpdateAo;
    };

CHuiStatic* CHuiStatic::NewLC(CHuiEnv* aPrimaryEnv)
    {
    CHuiStatic* self = new (ELeave) CHuiStatic();
    CleanupStack::PushL(self);
    self->ConstructL(aPrimaryEnv);
    return self;
    }


CHuiStatic* CHuiStatic::NewL(CHuiEnv* aPrimaryEnv)
    {
    CHuiStatic* self = NewLC(aPrimaryEnv);
    CleanupStack::Pop(self);
    return self;
    }


CHuiStatic::CHuiStatic()
    {
    }


void CHuiStatic::ConstructL(CHuiEnv* aPrimaryEnv)
    {
    iData = new (ELeave) TTlsData();
    
    iData->iPrimaryEnv = aPrimaryEnv;
    iData->iIdCounter = -1;
    iData->iTimeFactor = 1.0;
    iData->iIsFirstUpdateTime = ETrue;
    iData->iFirstUpdateTime.UniversalTime();
    iData->iUniversalTime.UniversalTime();
    iData->iRealUniversalTime.UniversalTime();
    iData->iLastFpsTime.UniversalTime();
    iData->DoInitL(); // make sure that we have infra even CCoeEnv was not there

    // Re-construct the virtual table for iFakeProbe
    iData->iFakeProbe = new (ELeave) TFakeProbe;
    
    iData->iAppFwProxy = new (ELeave) CAppFwProxy;
    
    //Register for environment change ( here system time change ) event notification
    iChangeNotifier = CEnvironmentChangeNotifier::NewL(0, TCallBack(CHuiStatic::SettingChangedCallBack, this));
    iChangeNotifier->Start();
    
#ifdef HUI_DEBUG_TRACK_DRAWING
    iData->iCommandDebugger = CAlfCommandDebug::NewL();
#endif
    
#ifndef __WINSCW__
    globalWriteableData = *iData;
    #ifdef USE_MODULE_TEST_HOOKS_FOR_ALF
        // For testing purposes, test data needs to be included into TLS
        // because test cases require information stored in TLS.
        if ( !Dll::Tls() )
            {
            Dll::SetTls( iData );    
            }
    #endif    
#else
    Dll::SetTls(iData);
#endif

#ifndef WIN32
#ifdef HUI_DEBUG_WITH_PRINTF
    EnableLogging();
#endif
#endif
    }


CHuiStatic::~CHuiStatic()
    {
#ifdef HUI_DEBUG_TRACK_DRAWING
    delete iData->iCommandDebugger;
    iData->iCommandDebugger = NULL;
#endif

    if(iData)
        {
        if(iData->iFakeProbe)
            {
            delete iData->iFakeProbe;
            iData->iFakeProbe = NULL;
            }
        iData->iLogFile.Close();
        delete iData->iAppFwProxy;
        iData->Disconnect();
        }
    iChangeNotifier->Cancel();
    delete iChangeNotifier;
#if defined( __WINSCW__ ) || defined( USE_MODULE_TEST_HOOKS_FOR_ALF )
    Dll::FreeTls();
#endif
    delete iData;
    }


EXPORT_C TTlsData* CHuiStatic::Data()
    {
#ifndef __WINSCW__
    return &globalWriteableData;
#else
    return static_cast<TTlsData*>(Dll::Tls());
#endif
    }


void CHuiStatic::UpdateTime(TTlsData* aData)
    {
    ASSERT( aData );
    // Updates the toolkit's time counters. This includes the toolkit's
    // realtime clock, the internal absolute clock (which is affected by the
    // time factor), the amount of elapsed time since last UpdateTime()
    // invocation, and the amount of elapsed time since the first UpdateTime()
    // invocation (which was done in the beginning of the first refresh).

    if(aData->iIsFirstUpdateTime)
        {
        aData->iIsFirstUpdateTime = EFalse;
        aData->iFirstUpdateTime.UniversalTime();
        aData->iUniversalTime = aData->iFirstUpdateTime;
        aData->iRealUniversalTime = aData->iUniversalTime;
        return;
        }

    TTime now;
    now.UniversalTime();

    // Advance the toolkit's internal clock, applying the time factor.
    if(!aData->iTimePaused)
        {
        aData->iInternalElapsed = now.MicroSecondsFrom(aData->iRealUniversalTime).Int64();
        aData->iInternalElapsed = (TInt64)((TReal32)aData->iInternalElapsed * aData->iTimeFactor);
        aData->iUniversalTime += TTimeIntervalMicroSeconds(aData->iInternalElapsed);
        }
    else
        {
        aData->iInternalElapsed = aData->iInternalElapsedBeforePausing;
        aData->iInternalElapsedBeforePausing = 0;        
        }

    aData->iRealUniversalTime = now;      
    }


TUint32 CHuiStatic::MilliSecondsSinceUpdateTime()
    {
    TTlsData* data = Data();
    
    TTime now;
    now.UniversalTime();
    
    return now.MicroSecondsFrom(data->iRealUniversalTime).Int64() / 1000;
    }


EXPORT_C CHuiEnv& CHuiStatic::Env()
    {
    if(!Data())
        {
        THuiPanic::Panic(THuiPanic::EStaticDataNotCreated);
        }
    return *Data()->iPrimaryEnv;
    }


EXPORT_C CHuiRenderPlugin& CHuiStatic::Renderer()
    {
    if(!Data() || !Data()->iRenderer)
        {
           HUI_DEBUG(_L("CHuiStatic::Renderer() - Static data or renderer not specified."));
        THuiPanic::Panic(THuiPanic::EStaticInvalidRenderer);
        }
    return *Data()->iRenderer;
    }


void CHuiStatic::SetRenderer(CHuiRenderPlugin& aRenderer)
    {
    if(!Data())
        {
        THuiPanic::Panic(THuiPanic::EStaticDataNotCreated);
        }
    Data()->iRenderer = &aRenderer;
    }


// @TODO SHOULD THIS BE MODIFIED SOMEHOW, OR IS THIS EXPORT OK???
EXPORT_C CHuiBitgdiRenderPlugin& CHuiStatic::BitgdiRenderer()
    {
    CHuiRenderPlugin* renderer = &Renderer();
    if(renderer->Id() != EHuiRenderPluginBitgdi)
        {
        HUI_DEBUG(_L("CHuiStatic::BitgdiRenderer() - Renderer cannot be used as Bitgdi renderer."));
        THuiPanic::Panic(THuiPanic::EStaticInvalidRenderer);
        }
    return *reinterpret_cast< CHuiBitgdiRenderPlugin* >(renderer);
    }


EXPORT_C CHuiVg10RenderPlugin& CHuiStatic::Vg10Renderer()
    {
    CHuiRenderPlugin* renderer = &Renderer();
    if(renderer->Id() != EHuiRenderPluginVg10)
        {
        HUI_DEBUG(_L("CHuiStatic::Vg10Renderer() - Renderer cannot be used as Vg10 renderer."));
        THuiPanic::Panic(THuiPanic::EStaticInvalidRenderer);
        }
    return *reinterpret_cast< CHuiVg10RenderPlugin* >(renderer);
    }


EXPORT_C CHuiGles10RenderPlugin& CHuiStatic::Gles10Renderer()
    {
    CHuiRenderPlugin* renderer = &Renderer();
    if(renderer->Id() != EHuiRenderPluginGles10 &&
       renderer->Id() != EHuiRenderPluginGles11) // compatible
        {
        HUI_DEBUG(_L("CHuiStatic::Gles10Renderer() - Renderer cannot be used as Gles10 renderer."));
        THuiPanic::Panic(THuiPanic::EStaticInvalidRenderer);
        }
    return *reinterpret_cast< CHuiGles10RenderPlugin* >(renderer);
    }


EXPORT_C CHuiGles11RenderPlugin& CHuiStatic::Gles11Renderer()
    {
    CHuiRenderPlugin* renderer = &Renderer();
    if(renderer->Id() != EHuiRenderPluginGles11)
        {
        HUI_DEBUG(_L("CHuiStatic::Gles11Renderer() - Renderer cannot be used as Gles11 renderer."));
        THuiPanic::Panic(THuiPanic::EStaticInvalidRenderer);
        }
    return *reinterpret_cast< CHuiGles11RenderPlugin* >(renderer);
    }


EXPORT_C CHuiGles20RenderPlugin& CHuiStatic::Gles20Renderer()
    {
    CHuiRenderPlugin* renderer = &Renderer();
    if(renderer->Id() != EHuiRenderPluginGles20)
        {
        HUI_DEBUG(_L("CHuiStatic::Gles20Renderer() - Renderer cannot be used as Gles20 renderer."));
        THuiPanic::Panic(THuiPanic::EStaticInvalidRenderer);
        }
    return *reinterpret_cast< CHuiGles20RenderPlugin* >(renderer);
    }


EXPORT_C MHuiRenderSurface* CHuiStatic::CurrentRenderSurface()
    {
    if(!Data())
        {
        HUI_DEBUG(_L("CHuiStatic::CurrentRenderSurface() - Static data or current render surface not specified. Panicking."));
        THuiPanic::Panic(THuiPanic::EStaticCurrentRenderSurfaceNotSpecified);
        }
    return Data()->iCurrentRenderSurface;
    }


EXPORT_C void CHuiStatic::SetCurrentRenderSurface(MHuiRenderSurface* aSurface)
    {
    Data()->iCurrentRenderSurface = aSurface;
    }


EXPORT_C void CHuiStatic::EnableLogging(TBool aEnable)
    {
    TTlsData* data = Data();

    if(aEnable && !data->iLogEnabled)
        {
        data->iLogEnabled = ETrue;

		// debug code only 
        TFileName filename;
        filename.Append('e');
        filename.Append(':');
        filename.Append('\\');
        filename.Append(_L("HuiMessages.log"));
                
        if(data->iLogFile.Replace(*data->iFs, filename, EFileWrite) != KErrNone)
            {
            RDebug::Print(_L("CHuiStatic::EnableLogging() - Could not open log file e:HuiMessages.log for writing. Trying c: .."));
            _LIT(KFileName2, "c:\\HuiMessages.log");
            if(data->iLogFile.Replace(*data->iFs, KFileName2, EFileWrite) != KErrNone)
                {
                RDebug::Print(_L("CHuiStatic::EnableLogging() - Could not open log file c:HuiMessages.log for writing. Logging disabled."));
                data->iLogEnabled = EFalse;
                }
            else
                {
                RDebug::Print(_L("CHuiStatic::EnableLogging() - Enabled Logging to c \"HuiMessages.log\"."));
                Printf(_L("CHuiStatic::EnableLogging() - Enabled Logging to \"HuiMessages.log\"."));
                }
            }
        else
            {
            RDebug::Print(_L("CHuiStatic::EnableLogging() - Enabled Logging to mmc \"HuiMessages.log\"."));
            Printf(_L("CHuiStatic::EnableLogging() - Enabled Logging to mmc \"HuiMessages.log\"."));
            }
        }
    else if(!aEnable && data->iLogEnabled)
        {
        data->iLogFile.Close();
        data->iLogEnabled = EFalse;
        }
    else
        {
        // for PC lint
        }
    }


EXPORT_C TBool CHuiStatic::Logging()
    {
    return Data()->iLogEnabled;
    }


EXPORT_C void CHuiStatic::ContinueRefresh()
    {
    TTlsData* data = Data();
    if(data && data->iPrimaryEnv)
        {
        data->iPrimaryEnv->ContinueRefresh();
        }
    }


void CHuiStatic::UpdateTime()
    {
    // Updates the toolkit's time counters. This includes the toolkit's
    // realtime clock, the internal absolute clock (which is affected by the
    // time factor), the amount of elapsed time since last UpdateTime()
    // invocation, and the amount of elapsed time since the first UpdateTime()
    // invocation (which was done in the beginning of the first refresh).

    TTlsData* data = Data();
    
    UpdateTime(data);
    }


EXPORT_C const TTime& CHuiStatic::Time()
    {
    return Data()->iUniversalTime;
    }


EXPORT_C void CHuiStatic::SetTimeFactor(TReal32 aTimeFactor) __SOFTFP
    {
    TTlsData* data = Data();

    if(data)
        {
        if(aTimeFactor < 0)
            {
            aTimeFactor = 0;
            }
            
        if(data->iTimeFactor == 0 && aTimeFactor != 0)
            {
            // Continuing from a paused state
            UpdateTime(data);
            }
            
        data->iTimeFactor = aTimeFactor;
        }
    }


EXPORT_C TReal32 CHuiStatic::TimeFactor() __SOFTFP
    {
    TTlsData* data = Data();

    if(data)
        {
        return data->iTimeFactor;
        }
    return 1.0;
    }


EXPORT_C void CHuiStatic::PauseTime()
    {
    TTlsData* data = Data();

    if(data)
        {
        // Update internal elapsed
        UpdateTime(data);       
        // Cache internal elapsed in internal time passed before pausing
        data->iInternalElapsedBeforePausing = data->iInternalElapsed;
        // Pause timing
        data->iTimePaused = ETrue;
        }
    }


EXPORT_C void CHuiStatic::ContinueTime()
    {
    TTlsData* data = Data();

    if(data && data->iTimePaused)
        {
        UpdateTime(data);
        data->iTimePaused = EFalse;
        }
    }


EXPORT_C TBool CHuiStatic::TimePaused()
    {
    TTlsData* data = Data();

    if(data)
        {
        return data->iTimePaused;
        }
    return EFalse;
    }


EXPORT_C TReal32 CHuiStatic::ElapsedSeconds() __SOFTFP
    {
    TTlsData* data = Data();

    if(data)
        {
#ifdef EKA2
        return data->iInternalElapsed / 1.0e6;
#else
        return data->iInternalElapsed.GetTReal() / 1.0e6;
#endif
        }
    return 0;
    }


EXPORT_C TReal32 CHuiStatic::SecondsSinceStart() __SOFTFP
    {
    TTlsData* data = Data();
    if(!data)
        {
        // Time has not begun yet.
        return 0;
        }

    // Calculate difference between internal absolute clock and the first update time.
    TInt64 delta = data->iUniversalTime.MicroSecondsFrom(data->iFirstUpdateTime).Int64();
#ifdef EKA2
    return delta / 1.0e6;
#else
    return delta.GetTReal() / 1.0e6;
#endif
    }


EXPORT_C TUint32 CHuiStatic::MilliSecondsSinceStart()
    {
    // This will wrap around in approx. 50 days.
    TTlsData* data = Data();
    if(data)
        {
        // Calculate difference between internal absolute clock and the first update time.
        TInt64 delta = data->iUniversalTime.MicroSecondsFrom(data->iFirstUpdateTime).Int64();
        return delta / 1000;
        }
    return 0;

    }


TInt CHuiStatic::GenerateId()
    {
    TTlsData* data = Data();

    // The ID counter counts backwards.
    TInt id = data->iIdCounter;

    if(data->iIdCounter == KMinTInt)
        {
        // Wrap around to stay negative.
        data->iIdCounter = -1;
        }
    else
        {
        --data->iIdCounter;
        }

    return id;
    }


void CHuiStatic::ReportNewFrame()
    {
    TTlsData* data = Data();
    data->iFrameCounter++;
    data->iCurrentFrameCounter++;
    }


EXPORT_C TUint CHuiStatic::FrameCount()
    {
    TTlsData* data = Data();
    return data->iFrameCounter;
    }


EXPORT_C TReal32 CHuiStatic::AverageFrameRate() __SOFTFP
    {
    return 0;
    }


EXPORT_C TReal32 CHuiStatic::FrameRate() __SOFTFP
    {

    TTlsData* data = Data();

    TTime now;
    now.UniversalTime();

    // If very little time has passed, don't calculate a new value yet.
    TTimeIntervalMicroSeconds msFromLast = 0;
    msFromLast = now.MicroSecondsFrom(data->iLastFpsTime);
    if(msFromLast < TTimeIntervalMicroSeconds(TInt64(1e6)))
        {
        // Do not recalculate yet.
#ifdef SYMBIAN_BUILD_GCE
        return 0; // to ignore the value if time is not elapsed
#else
        return data->iLastFps;
#endif
        }

    if(data->iCurrentFrameCounter == 0)
        {
        data->iLastFpsTime = now;
        return 0;
        }

    TInt64 delta = now.MicroSecondsFrom(data->iLastFpsTime).Int64();

    TReal32 fps = 0;
#ifdef EKA2
    TReal32 elapsed = delta / 1.0e6;
#else
    TReal32 elapsed = delta.GetTReal() / 1.0e6;
#endif

    if(elapsed > 0)
        {
        fps = (TReal32)data->iCurrentFrameCounter / elapsed;
        }

    data->iLastFps = fps;
    data->iCurrentFrameCounter = 0;
    data->iLastFpsTime = now;

    return fps;
    }


EXPORT_C void CHuiStatic::Printf(TRefByValue<const TDesC8> aFormat, ...)
    {
    TTlsData* data = Data();
    // Bail out early if logging is disabled.
    if(!data || !data->iLogEnabled)
        {
#ifdef _DEBUG
        // redirect to RDebug output if we are debugging
        TUint total = 0;
        TUint free = HuiUtil::FreeMemory(&total);
        TBuf8<256> buf;
        VA_LIST list;
        VA_START(list, aFormat);
        buf.FormatList(aFormat, list);
        VA_END(list);
        RDebug::Print(_L("(%i/%i KB) %s"), free/1024, total/1024, &buf);
#endif
        // Already gone?
        return;
        }
    TTime now;
    now.UniversalTime();
    TInt32 elapsed = (TInt32)(now.MicroSecondsFrom(data->iFirstUpdateTime).Int64() / 1000);

    TBuf8<100> memData;
    TUint total = 0;
    TUint free = HuiUtil::FreeMemory(&total);
    memData.Format(_L8("% 2d,%03d (%i/%i KB) "), elapsed / 1000,
                                             elapsed % 1000,
                                             free/1024, total/1024);

    TBuf8<512> buf;
    VA_LIST list;
    VA_START(list, aFormat);
    buf.FormatList(aFormat, list);
    VA_END(list);
    buf.Insert(0, memData);
    buf.Append(_L8("\n"));

    data->iLogFile.Write(buf);
    data->iLogFile.Flush();
    }


EXPORT_C void CHuiStatic::Printf(TRefByValue<const TDesC16> aFormat, ...)
    {
    TTlsData* data = Data();
    // Bail out early if logging is disabled.
    if(!data || !data->iLogEnabled)
        {
#ifdef _DEBUG
        // redirect to RDebug output if we are debugging
        TBuf16<512> buf;
        TUint total; TUint free = HuiUtil::FreeMemory(&total);
        VA_LIST list;
        VA_START(list, aFormat);
        buf.FormatList(aFormat, list);
        VA_END(list);
        RDebug::Print(_L("(%i/%i KB) %S"), free/1024, total/1024, &buf);
#endif
        // Already gone?
        return;
        }
    TTime now;
    now.UniversalTime();
    TInt32 elapsed = (TInt32)(now.MicroSecondsFrom(data->iFirstUpdateTime).Int64() / 1000);

    TBuf16<100> memData;
    TUint total = 0;
    TUint free = HuiUtil::FreeMemory(&total);
    memData.Format(_L16("% 2d,%03d (%i/%i KB) "), elapsed / 1000,
                                              elapsed % 1000,
                                              free/1024, total/1024);

    TBuf16<512> buf;
    VA_LIST list;
    VA_START(list, aFormat);
    buf.FormatList(aFormat, list);
    VA_END(list);
    buf.Insert(0, memData);
    buf.Append(_L16("\n"));

    TBuf8<512> buf8;
    buf8.SetLength(buf.Length());
    for(TInt i = 0; i < buf.Length(); ++i)
        {
        buf8[i] = TUint8(buf[i]);
        }

    data->iLogFile.Write(buf8);
    data->iLogFile.Flush();
    }


EXPORT_C void CHuiStatic::Tic(TInt aClock)
    {
    if(aClock < 0 || aClock >= KMaxClocks)
        {
        THuiPanic::Panic(THuiPanic::EStaticInvalidClock);
        }

    TTlsData* data = Data();
    data->iClocks[aClock].UniversalTime();
    }


EXPORT_C TReal32 CHuiStatic::Toc(TInt aClock) __SOFTFP
    {
    if(aClock < 0 || aClock >= KMaxClocks)
        {
        THuiPanic::Panic(THuiPanic::EStaticInvalidClock);
        }

    TTlsData* data = Data();

    TTime now;
    now.UniversalTime();

    TInt64 delta = now.MicroSecondsFrom(data->iClocks[aClock]).Int64();
#ifdef EKA2
    return delta / 1.0e6;
#else
    return delta.GetTReal() / 1.0e6;
#endif
    }


EXPORT_C RFs& CHuiStatic::FsSession()
    {
    TTlsData* data = Data();
    __ASSERT_ALWAYS(data, User::Panic(_L("HUI"),0));
    return *data->iFs;
    }

EXPORT_C RWsSession& CHuiStatic::WsSession()
    {
    TTlsData* data = Data();
    __ASSERT_ALWAYS(data, User::Panic(_L("HUI"),0));
    return *data->iWsSession;
    }

EXPORT_C CWsScreenDevice* CHuiStatic::ScreenDevice( TInt aScreenNumber )
    {
    TTlsData* data = Data();
    __ASSERT_ALWAYS(data, User::Panic(_L("HUI"),0));
    return data->WsScreenDevice(aScreenNumber);
    }

EXPORT_C RWindowGroup* CHuiStatic::RootWin( TInt aScreenNumber )
    {
    TTlsData* data = Data();
    __ASSERT_ALWAYS(data, User::Panic(_L("HUI"),0));
    return data->RootWin(aScreenNumber);
    }

EXPORT_C void CHuiStatic::SetLayoutTransitionTime( TInt aTime )
    {
    TTlsData* data = Data();
    if(data)
        {
        data->iLayoutTransitionTime = aTime;
        }
    }


EXPORT_C TInt CHuiStatic::LayoutTransitionTime()
    {
    TTlsData* data = Data();
    if(data)
        {
        return data->iLayoutTransitionTime;
        }
    return 0;
    }

EXPORT_C void CHuiStatic::SetProbe(MHuiProbe* aProbe)
    {
    TTlsData* data = Data();
    if(data)
        {
        data->iProbe = aProbe;
        }
    }

MHuiProbe& CHuiStatic::Probe()
    {
    TTlsData* data = Data();
    
    if ( !data )
        {
        THuiPanic::Panic(THuiPanic::EStaticDataNotCreated);
        }
    
    if(data->iProbe)
        {
        return *(data->iProbe);
        }
    return *(data->iFakeProbe);
    }

EXPORT_C MAknsSkinInstance* CHuiStatic::SkinInstance()
    {
    if (CCoeEnv::Static())
        {
        return AknsUtils::SkinInstance();
        }
    return 0;
    }

TBool CHuiStatic::LayoutMirrored()
    {
    if (CCoeEnv::Static())
        {
        return AknLayoutUtils::LayoutMirrored();
        }
    else
        {
        TTlsData* data = Data();
        return data->iAppFwProxy->LayoutMirrored(); 
        }
    }

TInt CHuiStatic::CbaLocation()
    {
    if (CCoeEnv::Static())
        {
        return AknLayoutUtils::CbaLocation();
        }
    else
        {
        TTlsData* data = Data();
        return data->iAppFwProxy->SoftKeyOrientation(); 
        }
    }

void CHuiStatic::LayoutMetricsRect(TInt aType, TRect& aRect)
    {
    if (CCoeEnv::Static())
        {
        (void)AknLayoutUtils::LayoutMetricsRect((AknLayoutUtils::TAknLayoutMetrics)aType, aRect);
        }
    else
        {
        TTlsData* data = Data();
        aRect = data->iAppFwProxy->LayoutMetricsRect(aType); 
        }
    }

TInt CHuiStatic::GetCachedColor(TRgb& aRgb, const TAknsItemID& aID,
          const TInt aIndex )
    {
    MAknsSkinInstance* skin = SkinInstance();
    if (skin)
        {
        return AknsUtils::GetCachedColor(skin, aRgb, aID, aIndex );
        }
    else
        {
        TTlsData* data = Data();
        return data->iAppFwProxy->GetCachedColor(aRgb, aID, aIndex );        
        }
    }

TBool CHuiStatic::ConvertToVisualAndClipL(
    TDes& aLogicalText,
    const CFont& aFont,
    TInt aMaxWidthInPixels,
    TInt aMaxClippedWidthInPixels )
    {
    if (CCoeEnv::Static())
       {
       return AknBidiTextUtils::ConvertToVisualAndClipL(
            aLogicalText,
            aFont,
            aMaxWidthInPixels,
            aMaxClippedWidthInPixels );
       }
    else
        {
        if ( !aLogicalText.Length() )
            {
            aLogicalText = KNullDesC; // null text 
            return EFalse;
            }

        HBufC* visualBuffer = HBufC::NewLC(
                aLogicalText.Length() + KAknBidiExtraSpacePerLine );
        
        TPtr ptr = visualBuffer->Des();
        
        TInt chars = aLogicalText.Length();

        TBool clipped = EFalse;

        TChar clipChar = 0xffff;

        // TextCount() converts text to visual form and then checks it
        if ( aFont.TextCount( aLogicalText, aMaxWidthInPixels ) < chars )
            {
            clipped = ETrue;

            TInt clipCharWidth = 0;
            // Not enough space even for clip char alone - return empty descriptor.
            if ( aMaxClippedWidthInPixels < clipCharWidth )
                {
                aLogicalText = KNullDesC; // null text 
                CleanupStack::PopAndDestroy(); // visual buf
                return ETrue;
                }

            // Check how many characters fit in given space with truncation char.
            // We need to give this information to TBidiLogicalToVisual.
            chars = aFont.TextCount( 
                    aLogicalText, aMaxClippedWidthInPixels - clipCharWidth );

            // This is "avkon rule": should not insert ellipsis right after a space.
            if ( chars > 1 && 
                    aLogicalText[chars - 1] == ' ' &&
                    aLogicalText[chars - 2] != ' ' )
                {
                chars--;
                }
            }
        
        TBidirectionalState::TRunInfo* array =
                new TBidirectionalState::TRunInfo[chars];
        
        TBidiLogicalToVisual converter(
                aLogicalText,
                array,
                chars ) ;
 
        TInt count = converter.Reorder();

        if (count > chars)
            {
            delete array;
            array = 0;
            array = new TBidirectionalState::TRunInfo[chars];
            }
     
        if (array)
            {
            TBidiLogicalToVisual converter = TBidiLogicalToVisual( aLogicalText, array, chars );
            converter.Reorder();
            TPtr ptr = visualBuffer->Des();
            converter.GetVisualLine( ptr, 0, chars, clipChar );
            }
     
        aLogicalText = *visualBuffer;
   
        CleanupStack::PopAndDestroy();
        return clipped;
        }
    }

HBufC* CHuiStatic::ConvertToVisualAndWrapToArrayL( 
    const TDesC& aLogicalText,
    TInt aLineWidth,
    const CFont& aFont,
    CArrayFix<TPtrC>& aWrappedArray )
    {
    if (CCoeEnv::Static())
        {
        return AknBidiTextUtils::ConvertToVisualAndWrapToArrayL( 
            aLogicalText,
            aLineWidth,
            aFont,
            aWrappedArray);
        }
    else
        {
        // will not be supported ever
        HBufC* ret = aLogicalText.AllocLC();
        aWrappedArray.AppendL(ret->Des());
        CleanupStack::Pop();
        return ret;
        }
    }

CFbsBitmap* CHuiStatic::GetBgBitmapLC(const TAknsItemID& aID)
    {
    CFbsBitmap * bitmap = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(bitmap);
 
    TTlsData* data = Data();
    data->iAppFwProxy->GetSkinBackgroundBitmapL(bitmap, aID);        
    
    return bitmap;
    }

void CHuiStatic::GetMaskedBitmapL(const TAknsItemID& aID, CFbsBitmap*& aBitmap, CFbsBitmap*& aBitmapMask,
                                 const TDesC& aFileName, TInt aBitmapId, TInt aMaskId,  const TSize& aSize, TScaleMode aScaleMode)
    {
    CFbsBitmap * bitmap = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(bitmap);

    CFbsBitmap * mask = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(mask);

    TTlsData* data = Data();
    data->iAppFwProxy->GetSkinBitmapL(bitmap, mask, aID, aFileName, aBitmapId, aMaskId, aSize, aScaleMode );        
    
    // Transfer the ownership
    if (mask->Handle())
        {
        aBitmapMask = mask;
        CleanupStack::Pop(); //mask
        }
    else 
        {
        CleanupStack::PopAndDestroy(); //mask
        }        
    aBitmap = bitmap;
    CleanupStack::Pop(); //bitmap
    }
        
CAknsItemData* CHuiStatic::GetCachedSkinItemDataL(const TAknsItemID& aID, TInt aDataType)   
    {
    CAknsImageTableItemData* skindata = 0;    
    if (aDataType == EAknsITImageTable) // only image table for framebrush supported
        {
        TTlsData* data = Data();
        TAlfCachedSkinItemArray array;
        data->iAppFwProxy->GetCachedSkinItemDataArrayL(aID, aDataType, array);         
        if (array.iCount)
            {
            skindata = CAknsImageTableItemData::NewL();
            CleanupStack::PushL(skindata);
            skindata->SetImagesL(array.iCount, array.iImages);
            CleanupStack::Pop();
            }        
        }
    return skindata;
    }

TInt CHuiStatic::SettingChangedCallBack(TAny* aInstance)
    {
    return ((CHuiStatic *)aInstance)->DoChange();
    }

TInt CHuiStatic::DoChange()
    {
    TInt change = iChangeNotifier->Change();
    TTlsData* data = Data();
    if(!data)
        {
        // Time has not begun yet.
         return 0;
         }
      if (change & EChangesLocale)
          {
          data->iIsFirstUpdateTime = ETrue;
          UpdateTime();
          }
     
      if (change & EChangesSystemTime)
          {
          data->iIsFirstUpdateTime = ETrue;
          UpdateTime();
          }
      
      return 1;
    }
