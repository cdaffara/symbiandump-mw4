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

#ifndef STUBBER_H
#define STUBBER_H

#include <e32base.h>
#include <apibehavior.h>
#include <stubsrvconst.h>

class RStubClient;

class MApiInvoker 
{
public:
    virtual void InvokeApiComplete(const TApiBehavior& aBeh) = 0;
};

NONSHARABLE_CLASS(CStubber) : public CActive
    {
public: 

    IMPORT_C static CStubber* NewL();
    
    IMPORT_C ~CStubber();

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
    IMPORT_C void EnqueBehavior(const TApiBehavior& aBeh, TInt aExeDuration = 0);

    /**
    * Deletes all queued behaviors of the specified API.
    *
    * @param aLib the identifier of a stub
    * @param aApi the identifier of the API
    */
    IMPORT_C void DeleteBehaviors(TUint aLib, TUint aApi);

    /**
    * Invokes an API.
    *
    * If there is a queued behavior for this API, Stub server will perform according to the execution setting; 
    * otherwise stub server will act according to the specified mode.
    *
    * @param aBeh contains the lib and API identifiers when passed in;
    *                 will contain the behavior at complete.
    * @param aMode the mode to be used if no queued behavior is found for the specified API;
    *              In case of ECompleteIfNoBehaviorSet mode, stub server will complete this function 
    *              immediately and return a default TApiBehavior(TUint, TUint); 
    *              stub server will keep this function call pending until a behavior is queued if the mode
    *              is ESuspendIfBehaviorSet.
    */
    IMPORT_C void InvokeApi(TApiBehavior& aBeh, TApiInvokeMode aMode = ECompleteIfNoBehaviorSet);
	
    /**
    * asynchronous version of InvokeAPI(), MApiInvoker::InvokeApiComplete will be called when
    * this opeation is completed by stub server
    *
    * Only one InvokeApiL is allowed at a time.
    *
    * Leaves if the completion code in the behavior to be extracted is not KErrNone.
    *
    * @param aLib the identifier of a stub
    * @param aApi the identifier of the API
    * @param aMode the mode to be used if no queued behavior is found for the specified API;
    *              In case of ECompleteIfNoBehaviorSet mode, stub server will complete this function 
    *              immediately and return a default TApiBehavior(TUint, TUint); 
    *              stub server will keep this function call pending until a behavior is queued if the mode
    *              is ESuspendIfBehaviorSet.
    */
	IMPORT_C void InvokeApiL(MApiInvoker& aInvoker, TUint aLib, TUint aApi, TApiInvokeMode aMode = ECompleteIfNoBehaviorSet);

    /**
    * Cancels InvokeApi()
    */
	IMPORT_C void InvokeApiCancel();
    
protected:
    void RunL();
    void DoCancel();
    TInt RunError(TInt aReason);
    
protected:
    CStubber();
    
    void ConstructL();

protected:
    MApiInvoker* iInvoker;
    
    TApiBehavior iBeh;
    TApiBehaviorPckg iBehPckg;
    
    RStubClient* iClient;
    };
    
#endif // STUBBER_H

