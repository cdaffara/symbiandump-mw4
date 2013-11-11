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


#ifndef CTCPSERVERTESTUPNP30_H_
#define CTCPSERVERTESTUPNP30_H_

//----------------------------------------------------------//
/////////////////// CTcpServerTestUPnP30 /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//


NONSHARABLE_CLASS(CTcpServerTestUPnP30) : public CTcpServerTestCase
																			
	{
public:
	static CTcpServerTestUPnP30* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP30 ();

	void RecvComplete ( RMBufChain& aData );
	
private:
		
	CTcpServerTestUPnP30 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();

	};

NONSHARABLE_CLASS(CTcpServerTestUPnP38) : public CTcpServerTestCase
																			
	{
public:
	static CTcpServerTestUPnP38* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP38 ();

	void RecvComplete ( RMBufChain& aData );
	void SendComplete ( TInt /*aLength*/ );
	
private:
		
	CTcpServerTestUPnP38 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
	TBool				iSentOnce;
	
	};

NONSHARABLE_CLASS(CTcpServerTestUPnP39) : public CTcpServerTestCase
																			
	{
public:
	static CTcpServerTestUPnP39* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP39 ();

	void RecvComplete ( RMBufChain& aData );
	void ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage );
private:
		
	CTcpServerTestUPnP39 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();

	};

NONSHARABLE_CLASS(CTcpServerTestUPnP40) : public CTcpServerTestCase
																			
	{
public:
	static CTcpServerTestUPnP40* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP40 ();

	void RecvComplete ( RMBufChain& aData );
	void ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage );
private:
		
	CTcpServerTestUPnP40 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();

	};



#endif /*CTCPSERVERTESTUPNP30_H_*/
