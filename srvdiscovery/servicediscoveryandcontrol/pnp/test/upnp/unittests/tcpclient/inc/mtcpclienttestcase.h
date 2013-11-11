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


#ifndef __MTCPCLIENTTESTCASE_H__
#define __MTCPCLIENTTESTCASE_H__

class MTcpClientTestCase
	{
public:
	/*
	This method should return a descriptor that contains the display name of the test case.
	*/
	virtual const TDesC& TestCaseName() const = 0;

	/*
	This method should return the total number of transactions that are opened and involved
	in the test case. This number can be dynamic during the running of the test, an example
	of this would be if transactions are cancelled and therefore those should removed from
	the count. The test HTTP client uses this value so it knows when all the transactions
	have completed.
	*/
	virtual TInt TotalTransactionCount () const = 0;

	/*
	This method should return the number of transactions on a giving connection index. For
	tests that open a number of connections on a server, this method should return the
	number of transactions on a connection identified by its index (ie starting from 0).
	Tests that don't use multiple conenctions, this should simply return the same value as
	TotalTransactionCount(0).
	*/
	virtual TInt TransactionCount() const = 0;
	
	/*
	This method should return the raw request data that is expected to be reeived by the
	server for a giving connection and transaction on that connection. The connection
	is indentified by its index and then the transaction index on that connection. For
	instance, for the 3rd transaction on the 2nd connection will result in aConnectionIndex = 1
	and aTransIndex = 2.
	*/
	virtual const TDesC8& GetRawRequest(TInt aTransIndex) = 0;

	/*
	This method should return the raw response data that the server should return for a
	given transaction. This is similar to GetRawRequest() with the parameter values.
	*/	
	virtual const TDesC8& GetRawResponse(TInt aTransIndex) = 0;
	};
	
#endif // __MTcpClientTestCase_H__
