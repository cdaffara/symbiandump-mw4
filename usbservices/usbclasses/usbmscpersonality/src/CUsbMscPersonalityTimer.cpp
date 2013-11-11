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
* Description:  Common timer class for UsbMscPersonality
*
*/


#include <e32base.h>
#include "CUsbMscPersonalityTimer.h"
#include "CUsbActiveMscHandlerMdrv.h"
#include "debug.h"

CUsbMscPersonalityTimer::CUsbMscPersonalityTimer(TCallBack aCallBack,
                                                 TTimeIntervalMicroSeconds32 aTime)
    : CActive( EPriorityStandard ),
      iCallBack(aCallBack),
      iTime(aTime)
    {
    FLOG(_L("[USBWATCHER]\tCUsbMscPersonalityTimer::CUsbMscPersonalityTimer"));        
    iTimer.CreateLocal();
    CActiveScheduler::Add(this);
    }
    
CUsbMscPersonalityTimer::~CUsbMscPersonalityTimer()
    {
    FLOG(_L("[USBWATCHER]\tCUsbMscPersonalityTimer::~CUsbMscPersonalityTimer"));        
    Cancel();
    iTimer.Close();
    }
    
void CUsbMscPersonalityTimer::Start()
    {
    FLOG(_L("[USBWATCHER]\tCUsbMscPersonalityTimer::Start"));        
    iTimer.After(iStatus, iTime);
    SetActive();
    }
    
void CUsbMscPersonalityTimer::RunL()
    {
    FLOG(_L("[USBWATCHER]\tCUsbMscPersonalityTimer::RunL"));        
    
    if (iStatus == KErrNone)
        {
        iCallBack.CallBack();
        }
    }
    
void CUsbMscPersonalityTimer::DoCancel()
    {
    FLOG(_L("[USBWATCHER]\tCUsbMscPersonalityTimer::DoCancel"));        
    iTimer.Cancel();
    }
    
TInt CUsbMscPersonalityTimer::RunError(TInt /*aError*/)
    {
    FLOG(_L("[USBWATCHER]\tCUsbMscPersonalityTimer::RunError"));                
    return KErrNone;
    }
    
// End of file
