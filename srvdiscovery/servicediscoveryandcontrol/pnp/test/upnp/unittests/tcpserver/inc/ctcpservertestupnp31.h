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


#ifndef CTCPSERVERTESTUPNP31_H_
#define CTCPSERVERTESTUPNP31_H_

//----------------------------------------------------------//
/////////////////// CTcpServerTestUPnP31 /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//


NONSHARABLE_CLASS(CTcpServerTestUPnP31) : public CTcpServerTestCase
																			
	{
public:
	static CTcpServerTestUPnP31* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP31 ();

	void RecvComplete ( RMBufChain& aData );
	
private:
		
	CTcpServerTestUPnP31 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();

	};


#endif /*CTCPSERVERTESTUPNP31_H_*/
