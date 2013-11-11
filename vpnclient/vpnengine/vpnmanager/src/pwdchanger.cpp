/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Handles the calling of PKI Service Server module’s password change function.
*
*/



#include "pwdchanger.h"
#include "vpnapiservant.h"

CPwdChanger* CPwdChanger::NewL(const RMessage2& aMessage, CVpnApiServant& aVpnApiServant)
    {
    CPwdChanger* self = new (ELeave) CPwdChanger(aMessage, aVpnApiServant);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

CPwdChanger::CPwdChanger(const RMessage2& aMessage, CVpnApiServant& aVpnApiServant) :
    CActive(0), iMessage(aMessage), iVpnApiServant(aVpnApiServant)
    {
    }

void CPwdChanger::ConstructL()
    {
    CActiveScheduler::Add(this);
    User::LeaveIfError(iPkiService.Connect());
    }

CPwdChanger::~CPwdChanger()
    {
    Cancel();
    iPkiService.Close();
    }

void CPwdChanger::ChangePassword()
    {
    iPkiService.ChangePassword(iStatus);
    SetActive();
    }

void CPwdChanger::DoCancel()
    {
    iPkiService.CancelPendingOperation();
    iMessage.Complete(KErrCancel);
    }

void CPwdChanger::RunL()
    {
    iMessage.Complete(iStatus.Int());
    iVpnApiServant.PasswordChangeComplete();
    }
    
