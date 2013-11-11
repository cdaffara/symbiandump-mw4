/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Waits for request to complete
*
*/


#ifndef WAITER_H
#define WAITER_H

#include <e32std.h>
#include <e32base.h>

NONSHARABLE_CLASS(CWaiter) : public CActive
	{
public://Instantition and Destructor

	/**
	 * Instantiation
	 */
	static CWaiter* NewL();

	/**
	 * Destructor.
	 */
	virtual ~CWaiter();

protected://Construction and destruction

	/**
	 * Default constructor.
	 */
	CWaiter();

	/**
	 * 2nd phase constructor
	 */
	void ConstructL();

public:

	/**
	 * A sync function which will start waiting. 
	 * This function inturn calls CActiveSchedulerWait::Wait
	 *
	 * @param void
	 * @return void
	 */
	TInt Wait();

public://From CActive

	/**
	 * RunL function from CActive
	 * 
	 * @param void
	 * @return void
	 */
	void RunL();

	/**
	 * Cancel API from CActive
	 *
	 * @param void
	 * @return void
	 */
	void DoCancel();

	/**
	 * Completes Request
	 * 
	 * @param aStatus This value will be used to complete active object request
	 * @return ETrue is Request is completed otherwise EFalse
	 */
	TBool RequestComplete(TInt aStatus);

private://Data

	CActiveSchedulerWait iSyncWait;
	TBool iIsWaiting;
	
	};
#endif /*WAITER_H*/
