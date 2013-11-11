/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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





#include "sendebug.h"
#include "senlogger.h"
#include "wsstarcredentialobserver.h"
const TInt64 KHalfHour = 1000000 *60*30; //2;//30;  2 minutes instead 30
CWSStarCredentialObserver::CWSStarCredentialObserver(CWSStarServiceSession* aSession, RFileLogger* aLogger) : CTimer( EPriorityStandard )// Standard priority
{
    iSession = aSession;
    iLogger = aLogger; 
}

CWSStarCredentialObserver* CWSStarCredentialObserver::NewLC(CWSStarServiceSession* aSession, RFileLogger* aLogger)
{
	CWSStarCredentialObserver* self = new ( ELeave ) CWSStarCredentialObserver(aSession, aLogger);
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
}

CWSStarCredentialObserver* CWSStarCredentialObserver::NewL(CWSStarServiceSession* aSession, RFileLogger* aLogger)
{
	CWSStarCredentialObserver* self = CWSStarCredentialObserver::NewLC(aSession, aLogger);
	CleanupStack::Pop(); // self;
	return self;
}

void CWSStarCredentialObserver::ConstructL()
{
	CTimer::ConstructL() ;	// Initialize timer
	CActiveScheduler::Add( this );				// Add to scheduler
          iRunned = EFalse;
}

CWSStarCredentialObserver::~CWSStarCredentialObserver()
{
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarCredentialObserver::~ - ");   
	Cancel(); // Cancel any request, if outstanding
	TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"                        - cancelled");   
	
}

//void CWSStarCredentialObserver::Start(const TTime &aTime)
void CWSStarCredentialObserver::Start(const TTimeIntervalMicroSeconds& aDiff) 
{
    if (iRunned)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarCredentialObserver::Start - cancelled");   
        Cancel();
        }
        
    iRestTime = aDiff.Int64();
    StartCount();
    iRunned = ETrue;
}
void CWSStarCredentialObserver::StartCount() 
{
    if (iRestTime > KHalfHour) 
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CWSStarCredentialObserver::start - START only half hour  = %Ld microsec"), KHalfHour));
        After((TInt)KHalfHour);
        iRestTime = iRestTime - KHalfHour;
        }
    else
        {
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KNormalLogLevel, _L8("CWSStarCredentialObserver::start - START rest = %Ld microsec"), iRestTime));
        After((TInt)iRestTime);
        iRestTime = 0;
        }
}


void CWSStarCredentialObserver::RunL()
{
    if (!iRestTime )
        {
        iRunned = EFalse;
        HBufC8* error(NULL);
        iSession->SessionContext()->Update(WSStarContextKeys::KOnlySharing, EFalse); 
        iSession->SessionContext()->Update(WSStarContextKeys::KReAuthNeededFromCredObserv, ETrue);
        TInt retVal = iSession->RevalidateMobileTicketIfExpiredL( error );
        iSession->SessionContext()->Update(WSStarContextKeys::KReAuthNeededFromCredObserv, EFalse);
        delete error;
        error = NULL;    
        }
    else
        {
        StartCount();
        }
}

RFileLogger* CWSStarCredentialObserver::Log() const
    {
    return iLogger;
    }
