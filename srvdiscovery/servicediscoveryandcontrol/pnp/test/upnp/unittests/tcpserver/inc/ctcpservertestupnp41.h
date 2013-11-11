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


#ifndef CTCPSERVERTESTUPNP41_H_
#define CTCPSERVERTESTUPNP41_H_

#include "atestsocket.h"

NONSHARABLE_CLASS(CTcpServerTestUPnP41) : public CTcpServerTestCase,
										  public ATestSocket
																			
	{
public:
	static CTcpServerTestUPnP41* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP41 ();

	void RecvComplete ( RMBufChain& aData );
	void NewData(TUint aCount);
	void SendComplete ( TInt /*aLength*/ );
	void ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage );
private:
		
	CTcpServerTestUPnP41 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
	CSubConnectionFlowBase* 	iFlow;
	MSessionData* 				iSSPData;
	MFlowBinderControl* 		iFlowBinder;
	MSessionControl* 			iSSP;
	TBool						iSentHeaders;
	TBool						iIsWriteDone;
	TUint						iResponseLength;
	};


NONSHARABLE_CLASS(CTcpServerTestUPnP42) : public CTcpServerTestCase,
										  public ATestSocket
																			
	{
public:
	static CTcpServerTestUPnP42* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP42 ();

	void RecvComplete ( RMBufChain& aData );
	void NewData(TUint aCount);
	void SendComplete ( TInt /*aLength*/ );
	void ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage );
private:
		
	CTcpServerTestUPnP42 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();
	
	CSubConnectionFlowBase* 	iFlow;
	MSessionData* 				iSSPData;
	MFlowBinderControl* 		iFlowBinder;
	MSessionControl* 			iSSP;
	TBool						iSentHeaders;
	TBool						iIsWriteDone;
	TUint						iResponseLength;
	};




#endif /*CTCPSERVERTESTUPNP41_H_*/
