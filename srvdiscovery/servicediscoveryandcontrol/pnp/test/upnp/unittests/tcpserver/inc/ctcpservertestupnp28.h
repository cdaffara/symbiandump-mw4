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


#ifndef CTCPSERVERTESTUPNP28_H_
#define CTCPSERVERTESTUPNP28_H_

//----------------------------------------------------------//
/////////////////// CTcpServerTestUPnP28 /////////////////////////
//////Testcase: Send the test data from the test client/////////
//----------------------------------------------------------//


NONSHARABLE_CLASS(CTcpServerTestUPnP28) : public CTcpServerTestCase
																			
	{
public:
	static CTcpServerTestUPnP28* NewL ( CProxyProvd* aProvd );
	~CTcpServerTestUPnP28 ();

	void RecvComplete ( RMBufChain& aData );
	
private:
		
	CTcpServerTestUPnP28 ( CProxyProvd* aProvd );
	void ConstructL ();
	virtual TVerdict RunTestL ();

	};


#endif /*CTCPSERVERTESTUPNP28_H_*/
