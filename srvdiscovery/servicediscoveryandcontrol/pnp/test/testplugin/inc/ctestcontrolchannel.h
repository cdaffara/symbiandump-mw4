// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __CTESTCONTROLCHANNEL_H__
#define __CTESTCONTROLCHANNEL_H__

#include <e32base.h>
#include <pnp/rcontrolchannel.h>
#include <pnp/ccontrolchannelbase.h>

class CPnPRControlChannelAO;

class MControlChannelAOObsever
	{
public:
	virtual void TimeOut(CPnPRControlChannelAO& aControlChannelAO) = 0;	
	};

class CTestControlChannel : public CControlChannelBase,
							public MControlChannelAOObsever
{
public:
	static CTestControlChannel* NewL();
	virtual ~CTestControlChannel();
	void Recv ( TControlMessage& aMessage, TRequestStatus& aStatus );
	void Send ( TControlMessage& aMessage, TRequestStatus& aStatus );
	void CancelRecv ();
	void CancelSend ();
	
	void TimeOut(CPnPRControlChannelAO& aControlChannelAO);
		
private:
	CTestControlChannel();
	void ConstructL();	
	
private:	
    CPnPRControlChannelAO *rControlAO;
};



class CPnPRControlChannelAO:public CActive
    {
public:
     static CPnPRControlChannelAO* NewL(MControlChannelAOObsever& aObserver);   
     virtual ~ CPnPRControlChannelAO();      
  	 void Recvdata();
	 void Senddata();
	 void RunL();
	 void DoCancel();   
	 void After(TTimeIntervalMicroSeconds32 anInterval);
	 
private:
    void ConstructL();	
    CPnPRControlChannelAO(MControlChannelAOObsever& aObserver);	
   
private:	
     RTimer			iTimer;       
	 MControlChannelAOObsever& iObserver;
    };

#endif  //__CTESTCONTROLCHANNEL_H__