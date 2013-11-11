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
* Description:  S60 AppUi class
*
*/


#include <avkon.hrh>
#include <avkon.rsg> 
#include <AiwGenericParam.h>
#include <GenericParamConsumer.h>
#include <aknnavide.h>
#include <imageprintui.rsg>
#include <AknWaitDialog.h>
#include <f32file.h>
#include <s32file.h>
// #include <d32usbc.h>

#include <badesca.h>
#include <bautils.h>
#include <e32std.h>
#include <pathinfo.h> 

#include "imageprintui.hrh"
#include "imageprintuiappui.h"
#include "imageprintuidocument.h"
#include "emptyview.h"
#include "settingsview.h"
#include "imageprintuidebug.h"
#include "connectionmanager.h"
#include "capabilitymanager.h"
#include "requestmanager.h"
#include "eventmanager.h"
#include "notes.h"
#include "notetimer.h"

 
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//  ConstructL()
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::ConstructL()
    {
    
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::ConstructL()"));
    
    BaseConstructL( EAknEnableSkin | EAknEnableMSK );
    iEngine = static_cast<CImagePrintUiDocument*>(Document())->Engine();
    
    iImageFiles = new (ELeave) CDesCArrayFlat(KImageDefaultArray);
    iNotes = CNotes::NewL(this);
    iNoteShowed = EFalse;
    LoadImagesL();
    
    CAknView* emptyview = CEmptyView::NewLC();
    AddViewL(emptyview);
    CleanupStack::Pop(emptyview); 

    CAknView* settingsview = CSettingsView::NewLC();
    AddViewL(settingsview);
    CleanupStack::Pop(settingsview);
    ActivateLocalViewL( emptyview->Id() ); 
 
    iCableConnected = EFalse;
    iActiveNote = EFalse;
    iNumberOfUnSuppFiles = 0;
    iCapabilityChanged = EFalse;
    iPrintActive = EFalse;
    iErrorState = EFalse;
    iStartJobOK = EFalse;
    iPaperEmpty = EFalse;
    iNoteTimeOut = EFalse;
    iCloseTimeOut = EFalse;
    iCapabilityTimeOut = EFalse;
    iRequestTimeOut = EFalse;
    iNoteTimer = CNoteTimer::NewL(this);
    ReadNumberOfUnSuppL();
   
    iConnectionManager = CConnectionManager::NewL(this, iEngine, iNotes);
    iConnectionManager->StartSearchPrinter();
      
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::ConstructL() complete"));
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CImagePrintUiAppUi::~CImagePrintUiAppUi()
    {
    delete iNaviDecorator;
    delete iCapabilityManager; 
    delete iRequestManager; 
    
    delete iEventManager;
    delete iConnectionManager; 
       
    delete iNoteTimer;    	
    delete iNotes;
    delete iImageFiles;
    }


// ---------------------------------------------------------------------------
// Handle global commands
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::HandleCommandL(TInt aCommand)
    {

    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::HandleCommandL()"));

    // Handle global events
    switch ( aCommand )
        {
        case EAknSoftkeyExit: 
        case EAknSoftkeyBack:
        case EAknCmdExit:
        case EEikCmdExit:
            {
            Exit(); 
            break;
            }
        default:
            break;
        }

    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::HandleCommandL() complete"));
    }


// ---------------------------------------------------------------------------
// Closes the application when the cable is disconnected or when some other
// mode is selected. 
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::CloseApplication(TBool aDisconnect)
	{
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::CloseApplication"));
    iNotes->SetDisconnectState(aDisconnect);
    if(iPrintActive)
    	{
    	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::CloseApplication show note"));
    	TRAP_IGNORE(iNotes->ShowErrorMsgL(R_ERROR_CONNECTION_LOST));
    	if(!iNoteTimer->IsActive())
    		iCloseTimeOut = ETrue;
    		iNoteTimer->After(KRetryClose);
    	}
    else
    	{
    	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::CloseApplication just now"));
    	TRAPD(err, ProcessCommandL(EAknCmdExit));  
    	if(err)
    		{
    		FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::error when CloseApplication"));
    		}
    	}
    
	}

// ---------------------------------------------------------------------------
// Closes the application after showing note
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::CloseAfterNoteL()
	{
	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::CloseAfterNoteL "));
	ProcessCommandL(EAknCmdExit);  
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CImagePrintUiAppUi::NotifyPrintStatus(TInt aStatus)
	{
	FLOG(_L("[IMAGEPRINTUI]>>> CImagePrintUiAppUi::NotifyPrintStatus "));
	FTRACE(FPrint(_L("[IMAGEPRINTUI] CImagePrintUiAppUi  aStatus is %x"), aStatus ));
	FTRACE(FPrint(_L("[IMAGEPRINTUI] CImagePrintUiAppUi  aStatus is %d"), aStatus ));
	if(aStatus == EDpsPrintServiceStatusPrinting || aStatus == EDpsPrintServiceStatusIdle)
		{
		FLOG(_L("[IMAGEPRINTUI]>>> CImagePrintUiAppUi::NotifyPrintStatus set flag false "));
		iPaperEmpty = EFalse;
		}
	if(aStatus == EDpsPrintServiceStatusPrinting)
		{
		FLOG(_L("[IMAGEPRINTUI]>>> CImagePrintUiAppUi::Print status active "));
	    iPrintActive = ETrue;	
		}
	if(aStatus == EDpsPrintServiceStatusIdle)
		{
		FLOG(_L("[IMAGEPRINTUI]>>> CImagePrintUiAppUi::Print status idle"));
		iNotes->PrintProgress(KPrintFinishValue);   
	    iPrintActive = EFalse;	
		}
	FLOG(_L("[IMAGEPRINTUI]>>> CImagePrintUiAppUi::NotifyPrintStatus "));
	}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::NotifyPrintProgress(TInt aImagesPrinted, TInt aProgress)
	{
	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::NotifyPrintProgress"));
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi aImagesPrinted value is %d"), aImagesPrinted ));
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi aProgress value is %d"), aProgress ));
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi iNumberOfImages value is %d"), iNumberOfImages ));
    TInt realProgress;
    TUint currentlayout;
    
    currentlayout = iCapabilityManager->Layout();
	
	if(currentlayout == EDpsLayoutDefault || currentlayout == EDpsLayout1Up)
		{
		FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::NotifyPrintProgress progress print"));
		if(iNumberOfImages == KImageNumber)
			{
			FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::NotifyPrintProgress layout case 1 image"));
			aImagesPrinted = aImagesPrinted*KPrintFinishValue;
			realProgress = aImagesPrinted/iNumberOfImages;
			realProgress = (realProgress <= KPrintFinishValue) ? realProgress : KPrintFinishValue;
		    
			if(!iErrorState)	
				{
				FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::NotifyPrintProgress no error "));
			    iNotes->PrintProgress(realProgress);	
				}
			}
		
		if(iNumberOfImages > KImageNumber)
			{
			FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::NotifyPrintProgress one print image several images"));
			if(aProgress < KPrintFinishValue)
				{
				FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::NotifyPrintProgress one print image several images update"));
			    if(!iErrorState)
			    	{
			    	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::NotifyPrintProgress no error state "));
			        iNotes->PrintProgress(aProgress); 	
			    	}
				}
		
			}
		}
	else
		{
		FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::NotifyPrintProgress NumberOfImages print"));
		aImagesPrinted = aImagesPrinted*KPrintFinishValue;
		realProgress = aImagesPrinted/iNumberOfImages;
		realProgress = (realProgress <= KPrintFinishValue) ? realProgress : KPrintFinishValue;
		if(realProgress == KPrintFinishValue)
			{
			FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::NotifyPrintProgress printing done layout big"));
			}
	    
		if(realProgress < KPrintFinishValue)
			{
			FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::NotifyPrintProgress show progress layout big"));
			if(!iErrorState)
				{
				FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::NotifyPrintProgress no error state"));
			    iNotes->PrintProgress(realProgress); 	
				}
		    else
		    	{
		    	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::NotifyPrintProgress error state"));
		    	}
			}
		
		}
	}

// ---------------------------------------------------------------------------
// To inform about info from eventmanager
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::NotifyEventL( TInt aEventNotifyType)
	{
	FLOG(_L("[IMAGEPRINTUI] CImagePrintUiAppUi NotifyEvent start"));
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi aEventNotifyType is  %d"), aEventNotifyType ));
	switch(aEventNotifyType)
		{
        case CEventManager::ENewJobOK:
            FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi >>> New job OK"));
            break;
            
        case CEventManager::ENewJobNOK: 
        	 FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi >>> New job NOK"));
             iNotes->ShowErrorMsgL(R_ERROR_PRINTER_BUSY);
             break;    
             
        case CEventManager::ECapabilityChange:
        	 FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi >>> Capability change"));
             TRAPD(err, iCapabilityManager->AskCapabilitiesL(ETrue));
             if(err)
             	{
             	iNotes->ShowErrorMsgL(R_ERROR_GENERAL);
             	}
             break; 
             
        case EDpsPaperJam: 
        	 FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi Paper Jam"));	 	
        	 iNotes->ShowErrorMsgL(R_ERROR_PAPER_JAM);
             break;  
             
        case EDpsPaperEmpty: 
        	 FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi Paper empty"));
        	 iPaperEmpty = ETrue;
        	 iNotes->ShowErrorMsgL(R_ERROR_OUT_OF_PAPER);
             break;       
             
        case CEventManager::EInkEmpty:
        	 FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi Ink empty"));
        	 iNotes->PrintFinished();	
        	 iNotes->ShowErrorMsgL(R_ERROR_OUT_OF_INK); 
             break;
             
        case EDpsInkLow: 
        	 FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi Ink low"));	
        	 iNotes->ShowErrorMsgL(R_ERROR_INK_LOW);  
             break;   
                 
		case EDpsJobStatusEndedOther: 
			 FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi >>> User cancel from printer"));	
			 if(iPrintActive )
			 	{
			 	iNotes->PrintFinished();
				FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi >>> User cancel print active"));
				iPrintActive = EFalse;
			 	}
			 break;	
        case EDpsJobStatusEndedOk: 
        	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi >>> Job eneded normally"));
        	break;
        	
		case EDpsJobStatusEndedAbortImmediately: 
			FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi  User aborted"));
			break;
			
		case EDpsErrorStatusFatal: 
		    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi  Fatal error in printer side"));
		    if(iPrintActive)
		    	{
		    	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi  Fatal error in printer side, stop progress note"));
		    	iNotes->PrintFinished();
		    	iPrintActive = EFalse;
		    	}
		    iNotes->ShowErrorMsgL(R_ERROR_CHECK_STATUS);
            break;
            
        case CEventManager::EWarning:
        	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi  Warning in printer side"));
        	if(!iPaperEmpty)
        		{
        		if(iPrintActive)
			    	{
			    	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi  Warning in printer side, stop progress note"));
			    	iNotes->PrintFinished();
			    	iPrintActive = EFalse;
			    	}
			    	iNotes->ShowErrorMsgL(R_ERROR_CHECK_STATUS);
        		}
            break;
            
        case CEventManager::ENotErrorState:
        	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi,  Not Error State"));
        	iErrorState = EFalse;
        	break;
        
        case CEventManager::EErrorState:  
        	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi,  Error State"));
        	iErrorState = ETrue;
        	break;
        
        case CEventManager::EShowError:
        	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi, EShowError"));
        	if(iPrintActive)
		    	{
		    	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi  EShowError in printer side, stop progress note"));
		    	iNotes->PrintFinished();
		    	iPrintActive = EFalse;
		    	}
        	iNotes->ShowErrorMsgL(R_ERROR_GENERAL);
        	break;
        
	// 	case KErrUsbInterfaceNotReady:
		// 	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi, KErrUsbInterfaceNotReady"));
			// break;
        
        default:
        	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi  NotifyEvent::Default branch"));  		
            break;   
		}
	}
// ---------------------------------------------------------------------------
// To inform about info from requestmanager
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::NotifyRequestL(TInt aNotifyType)
	{
	FLOG(_L("[IMAGEPRINTUI] CImagePrintUiAppUi NotifyRequest start"));
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi aNotifyType is  %d"), aNotifyType ));
	switch(aNotifyType)
		{
        case CRequestManager::EConfigurePrintServiceOK:
            FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUiAppUi NotifyRequest; EConfigurePrintServiceOK "));
            TRAPD(err, iCapabilityManager->AskCapabilitiesL(EFalse));
            if(err)
            	{
            	iNotes->ShowErrorMsgL(R_ERROR_GENERAL);
            	}
            break;
            
                 
        case CRequestManager::EConfigurePrintServiceNOK:
             //printer does not support
             FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUiAppUi NotifyRequest; EConfigurePrintServiceNOK "));
             iNotes->ShowErrorMsgL(R_ERROR_UNABLE_TO_CONNECT); 
             break;    
             
        case CRequestManager::EGetJobStatusOK:
        	 FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUiAppUi NotifyRequest; EGetJobStatusOK "));	
             break; 
             
        case CRequestManager::EGetJobStatusNOK:
             FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUiAppUi NotifyRequest; EGetJobStatusNOK "));
             iNotes->ShowErrorMsgL(R_ERROR_GENERAL);  
             break;      
             
        case CRequestManager::EGetDeviceStatusOK:
             FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUiAppUi NotifyRequest; EGetDeviceStatusOK, start print "));
             break;
             
        case CRequestManager::EGetDeviceStatusNOK:
             // printer is busy
             FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUiAppUi NotifyRequest; EGetDeviceStatusNOK "));
             iNotes->ShowErrorMsgL(R_ERROR_PRINTER_BUSY); 
             break;  
             
        case CRequestManager::EStartJobOK:	
             //printing start ok
             FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::EStartJobOK"));
             iPrintActive = ETrue;
             iStartJobOK = ETrue;
             FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi EStartJobOK  iPrintActiveis  %d"), iPrintActive ));
             break;
             
        case CRequestManager::EStartJobNOK:
             //printing start nok
             FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUiAppUi NotifyRequest; EStartJobNOK "));
             iNotes->ShowErrorMsgL(R_ERROR_GENERAL); 
             break;     
             
        case CRequestManager::EAbortJobOK:
             FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUiAppUi NotifyRequest; EAbortJobOK ")); 
             break; 
             
        case CRequestManager::EAbortJobNOK:
             iNotes->ShowErrorMsgL(R_ERROR_GENERAL); 
             CloseApplication(EFalse);
             FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUiAppUi NotifyRequest; EAbortJobNOK ")); 
             break;
             
        case CRequestManager::EPicturesNotFound:  
        	FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUiAppUi NotifyRequest; EPicturesNotFound ")); 
        	iNotes->ShowErrorMsgL(R_ERROR_FILE_NOT_FOUND); 
        	break;
        		  
        default:
            FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUiAppUi NotifyRequest; Default branch")); 
            break;   
		
		}
	
	FLOG(_L("[IMAGEPRINTUI] CImagePrintUiAppUi NotifyRequest; END "));
	}


// ---------------------------------------------------------------------------
// To inform number of images
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::NumberOfImages(TInt aNumberOfImages)
	{
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t NumberOfImages iNumberOfImages  is %d"), iNumberOfImages));
	iNumberOfImages = aNumberOfImages;
	}
	
// ---------------------------------------------------------------------------
// To handle timer timeout
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::HandleTimeOutL()
	{
	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi <<< HandleTimeOutL"));
	if(iNoteTimeOut)
		{
		FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi <<< HandleTimeOutL, note"));
		iNoteTimeOut = EFalse;
		ShowNoteL(); 		
		}
	else if(iCloseTimeOut)
		{
		FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi <<< HandleTimeOutL, close"));
		iCloseTimeOut = EFalse;
		CloseAfterNoteL();
		}
	else if(iCapabilityTimeOut)
		{
		FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi <<< HandleTimeOutL, capability"));
		iCapabilityTimeOut = EFalse;
		iCapabilityManager->AskCapabilitiesL(EFalse);
    	}
	else if(iRequestTimeOut)
		{
		FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi <<< HandleTimeOutL, request"));
		iRequestTimeOut = EFalse;
		iRequestManager->ReIssueDoDpsRequestL();
		}
	
	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi >>> HandleTimeOutL"));
	}
	
// ---------------------------------------------------------------------------
// To handle timer error
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::HandleTimeErrorL(TInt aError)
	{
	if(iNoteTimeOut)
		{
		FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi <<< HandleTimeErrorL, note"));
		if(aError == KErrTimedOut)
			{
			iNoteTimeOut = EFalse;
			ShowNoteL();
			}
		}
	else if(iCloseTimeOut)
		{
		FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi <<< HandleTimeErrorL, close"));
		if(aError == KErrTimedOut)
			{
		    iCloseTimeOut = EFalse;
			CloseAfterNoteL(); 	
			}
		}
	else if(iCapabilityTimeOut)
		{
		FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi <<< HandleTimeErrorL, capability"));
		if(aError == KErrTimedOut)
			{
			iCapabilityTimeOut = EFalse;
			iCapabilityManager->AskCapabilitiesL(EFalse);
			}
		}
	else if(iRequestTimeOut)
		{
		FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi <<< HandleTimeErrorL, request"));
		if(aError == KErrTimedOut)
    		{
			iRequestTimeOut = EFalse;
			iRequestManager->ReIssueDoDpsRequestL();
    		}
		
		}
	
	
	}
// ---------------------------------------------------------------------------
// To handle request timeout
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::HandleRequestTime(TInt aTime)
	{	
	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi <<< HandleRequestTimeL"));
	iRequestTimeOut = ETrue;
	if(!iNoteTimer->IsActive())
		{
	    iNoteTimer->After(aTime); 	
		}
	}
	
// ---------------------------------------------------------------------------
// To handle capability timeout
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::HandleCapabilityTime(TInt aTime)
	{	
	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi <<< HandleCapabilityTimeL"));
	iCapabilityTimeOut = ETrue;
	if(!iNoteTimer->IsActive())
		{
	    iNoteTimer->After(aTime);	
		}
	}
		
// ---------------------------------------------------------------------------
// To inform when the cable is connected
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::CableConnectedL()
	{
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi <<< CableConnected"));
      
    iCableConnected = ETrue;
    iEventManager = CEventManager::NewL(this, iEngine);
    iCapabilityManager = CCapabilityManager::NewL(this, iEngine);
    iRequestManager    = CRequestManager::NewL(iEngine, iCapabilityManager, this);
    
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi >>> CableConnected"));
	}


// ---------------------------------------------------------------------------
// To inform when the cable is disconnected
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::NotifyError()
	{
	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::NotifyError"));
	TRAP_IGNORE(iNotes->ShowErrorMsgL(R_ERROR_GENERAL));
	}
// ---------------------------------------------------------------------------
// To inform when the cable is disconnected
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::CableDisConnected()
	{
	iCableConnected = EFalse;
	}
// ---------------------------------------------------------------------------
// CapabilityManager informs here when capabilities are fetched from dps engine
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::CapabilitiesReady()
{
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::CapabilitiesReady, activate local view"));
    TRAPD(err,ActivateLocalViewL(KImagePrintSettingsViewId));
    if(err)
    	{
    	TRAP_IGNORE(iNotes->ShowErrorMsgL(R_ERROR_GENERAL));
    	}
    	
    TRAPD(err2, SetNavipaneTextL()); 
    if(err2)
    	{
    	FLOG(_L("[IMAGEPRINTUI] CImagePrintUiAppUi::Unable to show printer name"));
    	}
} 


// ---------------------------------------------------------------------------
// This is called when the user presses Options->Print
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::StartPrinting()
	{
    // get first device status, then if OK  dps type start job
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi >>> StartPrinting called")); 
    iCapabilityManager->CheckIfNewCapabilityDiffer();
    if(iCableConnected)
    	{
        iNotes->SetCancelState(EFalse);
    	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi >>> StartPrinting, cable connected")); 
        TRAPD(err, iRequestManager->ChooseDpsRequestTypeL(CRequestManager::EGetDeviceStatus )); 
        if(err)
        	{
        	TRAP_IGNORE(iNotes->ShowErrorMsgL(R_ERROR_GENERAL));
        	}
    	}
    else
    	{
    	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi >>> StartPrinting, cable NOT connected")); 
    	TRAP_IGNORE(iNotes->ShowNoteL(R_CONNECT_USB));
    	iActiveNote = ETrue;
    	}                                          
    
	}


// ---------------------------------------------------------------------------
// This is called when the user presses Options->Cancel Print
// ---------------------------------------------------------------------------
//

void CImagePrintUiAppUi::CancelPrinting()
	{
	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi >>> CancelPrinting BEGIN"));
	TBool cancel = ETrue;
    iNotes->SetCancelState(cancel);
	TRAPD(err, iRequestManager->ChooseDpsRequestTypeL(CRequestManager::EAbortJob ));
	if(err)
    	{
        TRAP_IGNORE(iNotes->ShowErrorMsgL(R_ERROR_GENERAL));
        }

	
	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi >>> CancelPrinting END"));
	}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CDesCArrayFlat* CImagePrintUiAppUi::ImagesToPrint()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::ImagesToPrint()"));
    FTRACE(FPrint(_L("[IMAGEPRINTUI] ImagesToPrint; iImageFiles  is %x"), iImageFiles ));
    return iImageFiles;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//

void CImagePrintUiAppUi::ReadNumberOfUnSuppL()
	{
	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::ReadNumberOfUnSuppL start()"));
    
    TFileName unSuppFile( KResource ); 
    unSuppFile = PathInfo::PhoneMemoryRootPath(); 
    TBuf<KResource> lengthUn(KUnSuppFile);
    unSuppFile.SetLength(KDriver + lengthUn.Length());
    unSuppFile.Replace(KDriver, lengthUn.Length(), KUnSuppFile);
    
    
    RFileReadStream readStream;
    User::LeaveIfError( readStream.Open(CCoeEnv::Static()->FsSession(), unSuppFile, EFileRead) );
    readStream.PushL();
    iNumberOfUnSuppFiles = readStream.ReadInt16L();	
    FTRACE(FPrint(_L("[IMAGEPRINTUI] CImagePrintUiAppUi iNumberOfUnSuppFiles  is %d"), iNumberOfUnSuppFiles ));
    CleanupStack::PopAndDestroy(&readStream); 
    
    
    if(iNumberOfUnSuppFiles < KNumberOfUnsupportedFiles)
    	{
    	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::ReadNumberOfUnSuppL none"));
    	}
    else if(iNumberOfUnSuppFiles == KNumberOfUnsupportedFiles ||
            iNumberOfUnSuppFiles > KNumberOfUnsupportedFiles)
    	{
    	FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::ReadNumberOfUnSuppL one or more"));
    	if(!iNoteTimer->IsActive())
    	    iNoteTimeOut = ETrue;
    		iNoteTimer->After(KRetryInterval);
    	} 
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::LoadImagesL()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::LoadImagesL()"));
    
    TFileName printNameFile( KResource ); 
    printNameFile = PathInfo::PhoneMemoryRootPath();  
    TBuf<KResource> length(KParamFile);
    printNameFile.SetLength(KDriver + length.Length());
    printNameFile.Replace(KDriver, length.Length(), KParamFile);
    
  
    RFileReadStream readStream;
    CleanupClosePushL(readStream); //1
    User::LeaveIfError( readStream.Open(CCoeEnv::Static()->FsSession(),
                        printNameFile, EFileRead) );
    CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC(readStream); //2

    TInt index( 0 ); 
    const TAiwGenericParam* param = paramList->FindFirst(index,
            EGenericParamFile,
            EVariantTypeDesC);
    while ( index != KErrNotFound )
		{
        TFileName filename( param->Value().AsDes() );
        FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::LoadImagesL; append picture")); 
        iImageFiles->AppendL( filename ); 	
        	
        param = paramList->FindNext(index,
            EGenericParamFile,
            EVariantTypeDesC);
        }

    CleanupStack::PopAndDestroy(paramList);
    CleanupStack::PopAndDestroy(&readStream);
  
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::LoadImagesL() complete"));
    }


// ---------------------------------------------------------------------------
// Set navi pane text
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::SetNavipaneTextL()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::SetNavipaneTextL Start"));
    CEikStatusPane* sp =
        iEikonEnv->AppUiFactory()->StatusPane();
       
    CAknNavigationControlContainer* np =
        static_cast<CAknNavigationControlContainer*>(
            sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
           
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    
    TInt blank(1);
    TBuf8<KMaxPrinterName> printername;
    TBuf8<KMaxPrinterName> productname;
    
    iRequestManager->GetPrinterName(printername, productname); 
    // convert to HBuf16 because CreateNavigationLabel needs TDesC16, not TDesC8
    HBufC* temp = HBufC::NewLC(printername.Length() + productname.Length() + blank); 
    HBufC* temp2 = HBufC::NewLC(productname.Length()); 
    FTRACE(FPrint(_L("[IMAGEPRINTUI] CImagePrintUiAppUi printername  is %S"), &printername ));
    FTRACE(FPrint(_L("[IMAGEPRINTUI] CImagePrintUiAppUi productname  is %S"), &productname ));
    temp->Des().Copy(printername); 
    temp->Des().Append(_L(" "));
    temp2->Des().Copy(productname);
    temp->Des().Append(temp2->Des());
    
    iNaviDecorator = np->CreateNavigationLabelL( *temp );
    CleanupStack::PopAndDestroy(temp2);
    CleanupStack::PopAndDestroy(temp);
    np->PushL( *iNaviDecorator );
    sp->DrawNow(); 
    FLOG(_L("[IMAGEPRINTUI]\t CImagePrintUiAppUi::SetNavipaneText END"));
    }

CCapabilityManager* CImagePrintUiAppUi::CapabilityManager()
	{
	return iCapabilityManager;
	}
	
CNotes* CImagePrintUiAppUi::Notes()
	{
	return iNotes;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CImagePrintUiAppUi::PrintActive()
	{
	return iPrintActive;
	}
	

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CImagePrintUiAppUi::ShowNoteL()
	{
    iNoteShowed = ETrue;
    if(iNumberOfUnSuppFiles == KNumberOfUnsupportedFiles)
    	{
        iNotes->ShowInfoNoteL(R_ERROR_FILE_NOT_SUPPORTED);	
    	}
    else if(iNumberOfUnSuppFiles > KNumberOfUnsupportedFiles)
    	{
    	iNotes->ShowInfoNoteL(R_ERROR_UNSUPPORTED_FILES_REMOVED);
    	}	
	}
// end of file
