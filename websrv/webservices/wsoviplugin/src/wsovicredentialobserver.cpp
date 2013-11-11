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
#include "wsovicredentialobserver.h"


const TInt64 KHalfHour = 1000000 *60*30; //2;//30;  2 minutes instead 30
const TInt64 KSecond = 1000000;
CWSOviCredentialObserver::CWSOviCredentialObserver(CWSOviServiceSession* aSession, RFileLogger* aLogger) : CTimer( EPriorityStandard )// Standard priority
	{
    iSession = aSession;
    iLogger = aLogger; 
	}

CWSOviCredentialObserver* CWSOviCredentialObserver::NewLC(CWSOviServiceSession* aSession, RFileLogger* aLogger)
	{
	CWSOviCredentialObserver* self = new ( ELeave ) CWSOviCredentialObserver(aSession, aLogger);
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

CWSOviCredentialObserver* CWSOviCredentialObserver::NewL(CWSOviServiceSession* aSession, RFileLogger* aLogger)
	{
	CWSOviCredentialObserver* self = CWSOviCredentialObserver::NewLC(aSession, aLogger);
	CleanupStack::Pop(); // self;
	return self;
	}

void CWSOviCredentialObserver::ConstructL()
{
	CTimer::ConstructL() ;	// Initialize timer
	CActiveScheduler::Add( this );				// Add to scheduler
    iRunned = EFalse;
}

CWSOviCredentialObserver::~CWSOviCredentialObserver()
{
    //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CWSStarCredentialObserver::~ - ");   
	Cancel(); // Cancel any request, if outstanding
	//TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"                        - cancelled");   
	
}

//void CWSStarCredentialObserver::Start(const TTime &aTime)
void CWSOviCredentialObserver::Start(const TTimeIntervalSeconds& aDiff) 
{
    if (iRunned)
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarCredentialObserver::Start - cancelled");
        //TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel,"CWSStarCredentialObserver::Start - cancelled");   
        Cancel();
        }
        
    iRestTime = aDiff.Int()*KSecond;
    StartCount();
    iRunned = ETrue;
}
void CWSOviCredentialObserver::StartCount() 
{
    if (iRestTime > KHalfHour) 
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarCredentialObserver::start - START only half hour");
        //TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel, _L8("CWSStarCredentialObserver::start - START only half hour  = %Ld microsec"), KHalfHour));
        After((TInt)KHalfHour);
        iRestTime = iRestTime - KHalfHour;
        }
    else
        {
        TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarCredentialObserver::start - START rest");
        //TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KSenCoreServiceManagerLogLevel, _L8("CWSStarCredentialObserver::start - START rest = %Ld microsec"), iRestTime));
        After((TInt)iRestTime);
        iRestTime = 0;
        }
}


void CWSOviCredentialObserver::RunL()
{
    TLSLOG_L(KSenCoreServiceManagerLogChannelBase  , KMinLogLevel,"CWSStarCredentialObserver::RunL");
    if (!iRestTime )
        {
        iRunned = EFalse;
        HBufC8* error(NULL);
        iSession->SessionContext()->Update(WSOviContextKeys::KOnlySharing, EFalse); 
        //iSession->SessionContext()->Update(WSStarContextKeys::KReAuthNeededFromCredObserv, ETrue);
        TInt retVal = iSession->RevalidateMobileTicketIfExpiredL( error );
        //iSession->SessionContext()->Update(WSStarContextKeys::KReAuthNeededFromCredObserv, EFalse);
        delete error;
        error = NULL;    
        }
    else
        {
        StartCount();
        }
}

RFileLogger* CWSOviCredentialObserver::Log() const
    {
    return iLogger;
    }

