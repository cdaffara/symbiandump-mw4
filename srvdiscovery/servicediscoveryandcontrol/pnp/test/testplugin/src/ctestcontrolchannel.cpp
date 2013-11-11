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

#include "ctestcontrolchannel.h"
#include <e32debug.h>


CTestControlChannel* CTestControlChannel::NewL()
	{
	CTestControlChannel* self = new (ELeave) CTestControlChannel ();
	CleanupStack::PushL ( self );
	self->ConstructL ( );
	CleanupStack::Pop (); // self
	return self;
	}

CTestControlChannel::~CTestControlChannel ()
	{
	
	}

void CTestControlChannel::Recv ( TControlMessage& /*aMessage*/, TRequestStatus& /*aStatus*/ )
	{
	rControlAO = CPnPRControlChannelAO::NewL(*this);
	rControlAO->Recvdata();			
	}

void CTestControlChannel::Send ( TControlMessage& /*aMessage*/, TRequestStatus& /*aStatus*/ )
	{
	rControlAO = CPnPRControlChannelAO::NewL(*this);
	rControlAO-> Senddata();
	}


void CTestControlChannel::CancelRecv ()
	{
	
	}

void CTestControlChannel::CancelSend ()
	{
	
	}


CTestControlChannel::CTestControlChannel ( )
	{
	 //Nothing	
	}

void CTestControlChannel::ConstructL ()
	{	
	 //Nothing
	}

void CTestControlChannel::TimeOut(CPnPRControlChannelAO& aControlChannelAO)
	{
	CPnPRControlChannelAO* controlChannelAO = &aControlChannelAO;
	delete controlChannelAO;	
	}

CPnPRControlChannelAO* CPnPRControlChannelAO::NewL(MControlChannelAOObsever& aObserver)
{
    CPnPRControlChannelAO* self = new (ELeave) CPnPRControlChannelAO (aObserver);
	CleanupStack::PushL ( self );
	self->ConstructL ( );
	CleanupStack::Pop (); 
	return self;
}

CPnPRControlChannelAO::CPnPRControlChannelAO(MControlChannelAOObsever& aObserver)
:CActive(EPriorityStandard), iObserver(aObserver)
    {
	CActiveScheduler::Add(this);	
    }  

void CPnPRControlChannelAO::ConstructL()
	{
	 iTimer.CreateLocal();	
	}

void CPnPRControlChannelAO::Recvdata()
    {
    After(500000);
	}

void CPnPRControlChannelAO::Senddata()
    {
    After(500000);	
    }
CPnPRControlChannelAO::~CPnPRControlChannelAO()
    {
     Cancel();
     iTimer.Close();
    }

void CPnPRControlChannelAO::After(TTimeIntervalMicroSeconds32 anInterval)
  {
	iTimer.After(iStatus,anInterval);
	SetActive();			
  }	

void CPnPRControlChannelAO::DoCancel()
	{
	 iTimer.Cancel();
	}

void CPnPRControlChannelAO::RunL()
    {
	iObserver.TimeOut(*this);	
    }

