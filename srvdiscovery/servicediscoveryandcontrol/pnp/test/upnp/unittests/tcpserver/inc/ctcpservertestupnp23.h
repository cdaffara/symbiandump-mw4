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


#ifndef CTCPSERVERTESTUPNP23_H_
#define CTCPSERVERTESTUPNP23_H_

//----------------------------------------------------------//
/////////////////// CTcpServerTestUPnP23 /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//


NONSHARABLE_CLASS(CTcpServerTestUPnP23) : public CTcpServerTestCase
																			
	{
public:
	static CTcpServerTestUPnP23* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP23 ();

	void RecvComplete ( RMBufChain& aData );
	
private:
		
	void ReceivedL ( const TRuntimeCtxId& /*aSender*/, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage );
	CTcpServerTestUPnP23 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();

	};


#endif /*CTCPSERVERTESTUPNP23_H_*/
