/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CTCPSERVERTESTUPNP34_H_
#define CTCPSERVERTESTUPNP34_H_

#include "cupnptimer.h"
//----------------------------------------------------------//
/////////////////// CTcpServerTestUPnP34 /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//


NONSHARABLE_CLASS(CTcpServerTestUPnP34) : public CTcpServerTestCase
																			
	{
public:
	static CTcpServerTestUPnP34* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP34 ();

	void RecvComplete ( RMBufChain& aData );
	
private:
		
	CTcpServerTestUPnP34 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();

	};


// ServerHandler recv timer testcases
NONSHARABLE_CLASS(CTcpServerTestUPnP35) : public CTcpServerTestCase																			
	{
public:
	static CTcpServerTestUPnP35* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP35 ();

	void SendComplete ( TInt aLength );
	void RecvComplete ( RMBufChain& aData );
	
	
private:
		
	CTcpServerTestUPnP35 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();

	};


NONSHARABLE_CLASS(CTcpServerTestUPnP36) : public CTcpServerTestCase																			
	{
public:
	static CTcpServerTestUPnP36* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP36 ();

	void RecvComplete ( RMBufChain& aData );	
	
private:		
	CTcpServerTestUPnP36 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();

	};

NONSHARABLE_CLASS(CTcpServerTestUPnP37) : public CTcpServerTestCase,
											public MUPnPTimerObserver																		
	{
public:
	static CTcpServerTestUPnP37* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP37 ();

	void RecvComplete ( RMBufChain& aData );	
	void SendResponseToServerFlow ();
		
private:		
	CTcpServerTestUPnP37 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
	virtual void TimeOut ();
	
	void ReceivedL ( const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage );
	
	RBuf8 iResponseBuf;
	CTransaction* iTransaction; // not owned, dont delete
	CUPnPTimer* iUPnPTimer;
	};





#endif /*CTCPSERVERTESTUPNP34_H_*/
