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
* Description: 
*
*/

#include "uiacceltk/huiStatic.h"  // Class definition
#include "uiacceltk/huiUtil.h"
#include "uiacceltk/huiEnv.h"
#include "huirenderplugin.h"
#include "huistatictlsdata.h"
#include "uiacceltk/huiProbe.h"
#include <e32math.h>
#include <eikenv.h>
#include <flogger.h>



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



void CHuiStatic::SetRenderer(CHuiRenderPlugin& aRenderer)
    {
    if(!Data())
        {
        THuiPanic::Panic(THuiPanic::EStaticDataNotCreated);
        }
    Data()->iRenderer = &aRenderer;
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

