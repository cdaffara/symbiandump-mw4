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


#ifndef CTCPSERVERTESTUPNP32_H_
#define CTCPSERVERTESTUPNP32_H_

//----------------------------------------------------------//
/////////////////// CTcpServerTestUPnP32 /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//


NONSHARABLE_CLASS(CTcpServerTestUPnP32) : public CTcpServerTestCase
																			
	{
public:
	static CTcpServerTestUPnP32* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP32 ();

	void RecvComplete ( RMBufChain& aData );
	
private:
		
	CTcpServerTestUPnP32 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();

	};



#endif /*CTCPSERVERTESTUPNP32_H_*/
