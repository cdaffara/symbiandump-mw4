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
* Description:   Header for timer class used by IKEv1
*
*/

#ifndef C_IKEV1TIMEOUT_H
#define C_IKEV1TIMEOUT_H

#include <e32base.h>

class CIkev1Negotiation;

NONSHARABLE_CLASS(CIkev1Timeout) : public CTimer
    {
public:
    static CIkev1Timeout* NewL(CIkev1Negotiation& aNegotiation);
    
    //destructor
    ~CIkev1Timeout();
            
    void IssueRequest(TTimeIntervalMicroSeconds32 anInterval);
protected:

    //Cancel Packet Sending
    void RunL();            

private:
    CIkev1Timeout(CIkev1Negotiation& aNegotiation);    
        
    CIkev1Negotiation& iNegotiation;
    };


#endif // C_IKEV1TIMEOUT_H
