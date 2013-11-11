/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Transition server repository handler.
*
*/


#include "repositoryhandler.h"
#include <alflogger.h>
#include <centralrepository.h>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CRepositoryHandler::CRepositoryHandler(TInt aPriority, const TUid& aRepository, TInt aKey) :
    CActive(aPriority),
    iRepository(aRepository),
    iKey(aKey)
    {
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CRepositoryHandler::ConstructL()
    {
    iCrep = CRepository::NewL(iRepository);
    __ALFFXLOGSTRING("CRepositoryHandler: Starting");
    CActiveScheduler::Add(this);
    RunL();
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CRepositoryHandler* CRepositoryHandler::NewL(TInt aPriority, const TUid& aRepository, TUint32 aKey)
    {
    CRepositoryHandler* self = new (ELeave) CRepositoryHandler(aPriority, aRepository, aKey);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CRepositoryHandler::~CRepositoryHandler()
    {
    Cancel();
    delete iCrep;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CRepositoryHandler::RunL()
    {
    __ALFFXLOGSTRING("CRepositoryHandler::RunL (alf)");
    iError = iCrep->NotifyRequest(iKey, iStatus);
    TInt value;
    TInt err = iCrep->Get(iKey, value);
    if( err == KErrNone )
    	{
    	iValue = value;
    	}
    
    if(iError == KErrNone)
    	{
    	SetActive();
    	iError = err;
    	}
    __ALFFXLOGSTRING2("CRepositoryHandler::RunL (alf) iValue: %d, iError %d <<", iValue, iError);
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CRepositoryHandler::GetValue(TInt& aValue)
	{
	if(iError != KErrNone) //if we had an error eariler
		{
		TInt value;
		iError = iCrep->Get(iKey, value); //try to get the value
		if(iError == KErrNone)
			{
			iValue = value;
			if (!IsActive())
			    {
    			iError = iCrep->NotifyRequest(iKey, iStatus); //try to start request
    			if(iError == KErrNone)
    				{
    				SetActive();
    				}
			    }
			}
		}
	aValue = iValue;
	return iError;
	}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CRepositoryHandler::DoCancel()
    {
    __ALFFXLOGSTRING("CRepositoryHandler: Canceled");
    iCrep->NotifyCancel(iKey);
    }
