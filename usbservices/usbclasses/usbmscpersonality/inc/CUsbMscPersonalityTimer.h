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
* Description:  MSC Personality timer class
*
*/


#ifndef C_CUSBMSCPERSONALITYTIMER_H
#define C_CUSBMSCPERSONALITYTIMER_H

class CUsbMscPersonalityTimer : public CActive
    {
public:
    CUsbMscPersonalityTimer(TCallBack aCallBack,
                      TTimeIntervalMicroSeconds32 aTime);
    ~CUsbMscPersonalityTimer();
        
    void Start();
    
private:        
    void DoCancel();
    void RunL();
    TInt RunError( TInt /*aError*/ );
        
    RTimer iTimer;
    TCallBack iCallBack;
    TTimeIntervalMicroSeconds32 iTime;
    };

#endif
