/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef STUBCLIENT_H
#define STUBCLIENT_H

#include <e32std.h>
#include <stubsrvconst.h>
#include <apibehavior.h>

class RStubClient : public RSessionBase
    {

public:

   /**
    * Queues an API behavior specification to Stub server.
    *
    * This method is called by API behavior controllers, e.g. a module test, to specify the next expected 
    * behavior of an API. When InvokeApi() is called, the first queued behavior matching
    * the lib identifier and API identifier will be extracted and returned.
    *
    * @param aApi an API behavior.
    * @param aExeDuration, specifies the execution time of InvokeApi() in microseconds.
    *            value less than 0 means never completing InvokeApi(); 
    *            value 0 means completing InvokeApi() asap;
    *            value 1000000 means InvokeApi() takes 1 sec execution time;
    *        and so on. 
    */
    void EnqueBehavior(const TApiBehavior& aBeh, TInt aExeDuration = 0);

    /**
    * Deletes all queued behaviors of the specified API.
    *
    * @param aLib the identifier of a stub
    * @param aApi the identifier of the API
    */
    void DeleteBehaviors(TUint aLib, TUint aApi);

    /**
    * Invokes an API.
    *
    * If there is a queued behavior for this API, stub server will perform according to the execution setting; 
    * otherwise stub server will act according to the specified mode.
    *
    * @param aBehPckg contains the lib and API identifiers when passed in;
    *                 will contain the behavior at complete.
    * @param aMode the mode to be used if no queued behavior is found for the specified API;
    *              In case of ECompleteIfNoBehaviorSet mode, stub server will complete this function 
    *              immediately and return a default TApiBehavior(TUint, TUint); 
    *              stub server will keep this function call pending until a behavior is queued if the mode
    *              is ESuspendIfBehaviorSet.
    */
	void InvokeApi(TApiBehaviorPckg& aBehPckg, TApiInvokeMode aMode);
	
    /**
    * asynchronous version of InvokeAPI().
    *
    * Leaves if the completion code in the behavior is not KErrNone
    */
	void InvokeApiL(TApiBehaviorPckg& aBehPckg, TRequestStatus& aStatus, TApiInvokeMode aMode);

    /**
    * Cancels InvokeApi()
    * @param aLib the identifier of library which provides the API
    * @param aApi the identifier of the API
    */
	void InvokeApiCancel(TUint aLib, TUint aApi);

public: 

    /**
    * @return   Version information about the server
    */
    TVersion Version() const;

    /**
    * Establishes a session with stub server.
    * @return   error code
    */
    TInt Connect();

private:

    void CheckSession();

    };

#endif  // STUBCLIENT_H
