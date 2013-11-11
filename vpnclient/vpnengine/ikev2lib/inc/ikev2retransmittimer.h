/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Timer to trigger IKE request resend, if do not obtain a reply
*
*/

#ifndef C_IKEV2RETRANSMITTIMER_H
#define C_IKEV2RETRANSMITTIMER_H

#include <e32base.h>

/**
 *  IKEv2 retransmit timer callback interface.
 *  Callback interface which is used by CIkev2RetransmitTimer object to request
 *  IKE request resend.
 *  
 *  @lib internal (ikev2lib.lib)
 */
NONSHARABLE_CLASS(MIkev2RetransmitTimerCallback)
    {
public:        
    /**
     * Request IKE request resend.
     */
    virtual void RetransmitRequest() = 0;
    };

/**
 *  IKEv2 retransmit timer.
 *  Triggers IKE request retransmit after timeout.
 *  
 *  @lib internal (ikev2lib.lib)
 */
NONSHARABLE_CLASS(CIkev2RetransmitTimer) : public CTimer
{
public:
    static CIkev2RetransmitTimer* NewL( MIkev2RetransmitTimerCallback& aCallback );
	~CIkev2RetransmitTimer();
    
	/**
	 * Issues a reques to the timer.
	 * The actual time out value is calculated:
     * timeout = aSendAttempt * 1 second;
	 * 
	 * @param aSendAttempt the current send attempt count
	 */
	void IssueRequest( TUint16 aSendAttempt );
	
protected:
    void RunL();
       
private:
    CIkev2RetransmitTimer( MIkev2RetransmitTimerCallback& aCallback );
  
private: // data    
	MIkev2RetransmitTimerCallback& iCallback;	
};
#endif // C_IKEV2RETRANSMITTIMER_H
