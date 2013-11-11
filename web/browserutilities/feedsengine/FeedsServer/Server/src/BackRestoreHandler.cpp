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
* Description:  Class for making Feed Engine backup and restore aware.
*
*/


#include "BackRestoreHandler.h"
#include "FeedsServer.h"
#include <e32property.h> 			// For RProperty Object;
#include <sacls.h>  					// For KUidBackupRestoreKey global variable
#include <connect/sbdefs.h>						// for conn Namespace


// -----------------------------------------------------------------------------
// CBackRestoreHandler::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBackRestoreHandler* CBackRestoreHandler::NewL(CFeedsServer* aFeedServer)
		{
			CBackRestoreHandler *self = NewLC(aFeedServer);
			CleanupStack::Pop(self);
			return self;
		}



// -----------------------------------------------------------------------------
// CBackRestoreHandler::NewLC
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBackRestoreHandler* CBackRestoreHandler::NewLC(CFeedsServer* aFeedServer)
		{
			CBackRestoreHandler *self = new (ELeave) CBackRestoreHandler;
			CleanupStack::PushL(self);
			self->ConstructL(aFeedServer);
			return self;
		}



// -----------------------------------------------------------------------------
// CBackRestoreHandler::ConstructL
//
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBackRestoreHandler::ConstructL(CFeedsServer* aFeedServer)
		{
			CActiveScheduler::Add(this);
			iFeedServer = aFeedServer;
		}



// -----------------------------------------------------------------------------
// CBackRestoreHandler::~CBackRestoreHandler()
//
// Destructor
// -----------------------------------------------------------------------------
//
CBackRestoreHandler::~CBackRestoreHandler()
		{
			if(iIsListening)
			{
				StopListening();
			}
		}



// -----------------------------------------------------------------------------
// CBackRestoreHandler::CBackRestoreHandler
//
// C++ default constructor can NOT contain any code that
// might leave.
// -----------------------------------------------------------------------------
//
CBackRestoreHandler::CBackRestoreHandler():CActive(EPriorityStandard),iIsListening(EFalse),iIsBkpRestoreInProgress(EFalse)
		{
	
		}



// -----------------------------------------------------------------------------
// CBackRestoreHandler::RunL()
//
// To Receive event from Subscribe and publish server and
// based on the status of backup or restore operation 
// this function wakes up or makes server sleeping
// -----------------------------------------------------------------------------
//
void CBackRestoreHandler::RunL()
		{
			if(iStatus == KErrNone)
			{
				TInt backupStateValue=-1;
				iProperty.Subscribe(iStatus);
				SetActive();
				iProperty.Get(backupStateValue);
				if((backupStateValue == conn::EBURNormal | conn::ENoBackup )||backupStateValue == 0) // No backup or Restore activities
				{
					if(iIsBkpRestoreInProgress) // Check whether server was made sleeping by this class
					{
						iFeedServer->WakeupServerL();
						iIsBkpRestoreInProgress = EFalse;
					}
				}
				else // Either backup or Restore is in Progress
				{
					if(!iIsBkpRestoreInProgress)  //	Check whether Server is already sleeping
					{
						iFeedServer->SleepServer();
						iIsBkpRestoreInProgress = ETrue;
					}
				}
			}
			else if(iStatus == KErrCancel)
			{
				iIsListening = EFalse;
			}
			else
			{
					iIsListening = EFalse;
			}
		}



// -----------------------------------------------------------------------------
// CBackRestoreHandler::RunError()
//
// -----------------------------------------------------------------------------
//
		
void CBackRestoreHandler::RunError()
		{
	
		}



// -----------------------------------------------------------------------------
// CBackRestoreHandler::DoCancel()
//
// -----------------------------------------------------------------------------
//
		
void CBackRestoreHandler::DoCancel()
		{
			iProperty.Cancel();
		}
		
		
// -----------------------------------------------------------------------------
// CBackRestoreHandler::StartListening()
//
// This function subscribe to receive events on active backup or restore activities
// -----------------------------------------------------------------------------
//
void CBackRestoreHandler::StartListening()
		{
			if(!IsActive())
			{
				iProperty.Attach(KUidSystemCategory, KUidBackupRestoreKey);
				iProperty.Subscribe(iStatus);
				SetActive();
				iIsListening = ETrue;
			}
		}
		
// -----------------------------------------------------------------------------
// CBackRestoreHandler::StopListening()
//
// For cancellation of subscription
// -----------------------------------------------------------------------------
//
void CBackRestoreHandler::StopListening()
		{
			if(IsActive())
			{
				this->Cancel();
				iIsListening = EFalse;
			}
		}
