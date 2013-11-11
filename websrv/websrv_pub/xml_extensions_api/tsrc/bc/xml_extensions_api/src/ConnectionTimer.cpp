/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include "ConnectionTimer.h"

CConnectionTimer* CConnectionTimer::NewL(){
       CConnectionTimer* self = new(ELeave) CConnectionTimer;
       CleanupStack::PushL(self);
       self->ConstructL();
       return self;
}
CConnectionTimer::CConnectionTimer():CActive(0){
}
CConnectionTimer::~CConnectionTimer(){
    Cancel();
    iTimer.Close();
}
void CConnectionTimer::ConstructL(){
    User::LeaveIfError(iTimer.CreateLocal());
    CActiveScheduler::Add(this);
}
void CConnectionTimer::SetConnectionTimer(TTimeIntervalMicroSeconds32 aTimeOut){
    iTimer.After(iStatus, aTimeOut);
    SetActive();
}

void CConnectionTimer::RunL(){
    CActiveScheduler::Stop();
}
void CConnectionTimer::DoCancel(){
    iTimer.Cancel();
}
//  End of File
