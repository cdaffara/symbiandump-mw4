/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Source for timer class used by IKEv1
*
*/

#include "ikev1timeout.h"
#include "ikev1negotiation.h"


CIkev1Timeout* CIkev1Timeout::NewL(CIkev1Negotiation& aNegotiation)
    {
    CIkev1Timeout* self = new (ELeave) CIkev1Timeout(aNegotiation);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    return self;
    }


CIkev1Timeout::CIkev1Timeout(CIkev1Negotiation& aNegotiation)
 : CTimer(EPriorityStandard),
   iNegotiation(aNegotiation)
    {
    CActiveScheduler::Add(this);    //Adds itself to the scheduler only the first time
    }


CIkev1Timeout::~CIkev1Timeout()
    {
    if (IsActive())
        Cancel();
    }


//Issues next RunL execution
void CIkev1Timeout::IssueRequest(TTimeIntervalMicroSeconds32 anInterval)
    {
    After(anInterval);  //Also sets the object as Active
    }


// CPacketTimeout
// will send all the packets. One packet each Time
void CIkev1Timeout::RunL()
    {
    iNegotiation.ReSendL();
    }




