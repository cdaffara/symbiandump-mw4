/*
* Copyright (c) 2006, 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Informs about USB connection 
*
*/


#include <pictbridge.h>
#include <imageprintui.rsg>

#include "connectionmanager.h"
#include "imageprintuidebug.h"
#include "settingsview.h"
#include "notes.h"
#include "imageprintuiappui.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CConnectionManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CConnectionManager* CConnectionManager::NewL( CImagePrintUiAppUi* aAppUi, 
											  CDpsEngine* aEngine, CNotes* aNotes)
    {
    FLOG(_L("[IMAGEPRINTUI]\tCConnectionManager:NewL"));
    CConnectionManager* self 
        = new (ELeave) CConnectionManager(aAppUi, aEngine, aNotes);
    return self;
    }


// -----------------------------------------------------------------------------
// CConnectionManager::~CConnectionManager
// C++ default destructor
// -----------------------------------------------------------------------------
//    
CConnectionManager::~CConnectionManager()
    {
    FLOG(_L("[IMAGEPRINTUI]\tCConnectionManager: desctructor"));
    Cancel(); // cancels any existing active object orders for iStatus
    
    FLOG(_L("[IMAGEPRINTUI]\tCConnectionManager: desctructor complete"));
    }


// -----------------------------------------------------------------------------
// CConnectionManager::CConnectionManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CConnectionManager::CConnectionManager( CImagePrintUiAppUi* aAppUi, 
									    CDpsEngine* aEngine, CNotes* aNotes)
    : CActive(EPriorityStandard), iAppUi(aAppUi), 
      iDpsEngine(aEngine), iActiveNote(EFalse),iSetPrintCall(EFalse),
      iNotes(aNotes), iPreviousStatus(CDpsEngine::ENotConnected)
    {
    CActiveScheduler::Add(this);
    }



// -----------------------------------------------------------------------------
// CConnectionManager::RunError
// Standard active object error function.
// -----------------------------------------------------------------------------
//

TInt CConnectionManager::RunError(TInt aError)
    {
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\tCConnectionManager::RunError: ERROR %d from RunL."), aError));
    if ( aError == KErrTimedOut)
    	{
    	FLOG(_L("[IMAGEPRINTUI] CConnectionManager: Time out, start listen again"));
    	StartListening();
    	}
    else
    	{
    	FLOG(_L("[IMAGEPRINTUI] CConnectionManager RunError close application"));
        iAppUi->CloseApplication(ETrue);	
    	}
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// Starts search printer
// -----------------------------------------------------------------------------
//
void CConnectionManager::StartSearchPrinter()
	{
	FLOG(_L("[IMAGEPRINTUI]\tCConnectionManager: StartSerchPrinter"));
    iDpsEngine->SetPrintMode(iStatus);
    iSetPrintCall = ETrue;
    SetActive(); 
    FLOG(_L("[IMAGEPRINTUI]\tCConnectionManager: StartSerchPrinter complete")); 
	}
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CConnectionManager::RunL()
    {
    FLOG(_L("[IMAGEPRINTUI]>>>CConnectionManager::RunL"));
    FTRACE(FPrint(_L("[IMAGEPRINTUI] CConnectionManager iStatus  is %d"), iStatus.Int() ));
    FTRACE(FPrint(_L("[IMAGEPRINTUI] CConnectionManager iPreviousStatus  is %d"), iPreviousStatus ))
    
    if ( iStatus.Int() == CDpsEngine::ENotConnected  )
        {
        FLOG(_L("[IMAGEPRINTUI]>>>CConnectionManager::ENotConnected "));
        if(iPreviousStatus == CDpsEngine::EOtherConnected)
        	{
        	FLOG(_L("[IMAGEPRINTUI]\tCConnectionManager:: Previous was EOtherConnected, close earlier note")); 
        	iNotes->StopShowNote(R_NOT_CONNECT_PRINTER);
        	}
        iNotes->ShowNoteL(R_CONNECT_USB);
    	iActiveNote = ETrue; 
    	iPreviousStatus = CDpsEngine::ENotConnected;
    	StartListening();	
    	
        }
    else if ( iStatus.Int() == CDpsEngine::EOtherConnected )
        {
        FLOG(_L("[IMAGEPRINTUI]>>>CConnectionManager::EOtherConnected "));
        if(iPreviousStatus == CDpsEngine::ENotConnected)
        	{
        	FLOG(_L("[IMAGEPRINTUI]\tCConnectionManager:: Previous was ENotConnected, close earlier note")); 
        	iNotes->StopShowNote(R_CONNECT_USB);
        	}
        iNotes->ShowNoteL(R_NOT_CONNECT_PRINTER);
        iActiveNote = ETrue; 
        iPreviousStatus = CDpsEngine::EOtherConnected;
        StartListening();
        
        }
    else if (iStatus.Int() == CDpsEngine::EPrinterConnected)
        {
        FLOG(_L("[IMAGEPRINTUI]>>>CConnectionManager::EPrinterConnected"));
        iSetPrintCall = EFalse;
        if(iActiveNote)
        	{
        	FLOG(_L("[IMAGEPRINTUI]>>>CConnectionManager::Stop show note"));
            iNotes->StopShowNote(R_CONNECT_USB);
            iActiveNote = EFalse; 	
       		}	
        iPreviousStatus = CDpsEngine::EPrinterConnected;	
        StartListening(); 
        TRAPD(err, iAppUi->CableConnectedL());
        if(err)
        	{
        	// creation of some manager instance was unsuccessfull, close application
        	iNotes->ShowErrorMsgL(R_ERROR_GENERAL);
        	iAppUi->CloseApplication(EFalse); 
        	}
        }
    else if ( iStatus.Int() == CDpsEngine::EPrinterDisconnected || 
              iStatus.Int() == CDpsEngine::EWrongPrintModeConnected )
        {
        FLOG(_L("[IMAGEPRINTUI]>>>CConnectionManager::EPrinterDisconnected or EWrongPrintModeConnected"));
        iAppUi->CloseApplication(ETrue); 
        }

    FLOG(_L("[IMAGEPRINTUI]<<<CConnectionManager::RunL complete."));
    }
	


// -----------------------------------------------------------------------------
// CConnectionManager::StartListening
// Orders notifications and sets the object active.
// -----------------------------------------------------------------------------
//
void CConnectionManager::StartListening()
    {
    FLOG(_L("[IMAGEPRINTUI]\tCConnectionManager: StartListeningL"));
    if(IsActive())
        {
        return;   
        }
    iDpsEngine->ConnectStateNotify( iStatus );
    SetActive();
    FLOG(_L("[IMAGEPRINTUI]\tCConnectionManager: StartListeningL complete"));
    }


// -----------------------------------------------------------------------------
// CConnectionManager::DoCancel
// Standard active object cancellation function.
// -----------------------------------------------------------------------------
//
void CConnectionManager::DoCancel()
    {
    FLOG(_L("[IMAGEPRINTUI]\tCConnectionManager::DoCancel"));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t tCConnectionManager iDpsEngine  is %x"), iDpsEngine ));
    if(!iDpsEngine)
    	{
    	FLOG(_L("[IMAGEPRINTUI]\tCConnectionManager::DpsEngine allredy destroyed, panic"));
    	}
	else
		{
    iDpsEngine->CancelPrintMode();
		}
    FLOG(_L("[IMAGEPRINTUI]\tCConnectionManager::DoCancel complete"));
    }



// End of file
