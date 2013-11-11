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
* Description:  Handles DpeEventNotify call with AO
*
*/



#include <e32base.h>
#include <dpsdefs.h>
#include <dpsparam.h>
#include <s32file.h>
#include <f32file.h>
#include <e32std.h>
#include <coemain.h>
#include <d32usbc.h>

#include "eventmanager.h"
#include "imageprintuidebug.h"
#include "pictbridge.h"
#include "imageprintuiappui.h"
 
 
// ---------------------------------------------------------------------------
//  constructor
// ---------------------------------------------------------------------------
//
CEventManager::CEventManager(CImagePrintUiAppUi* aAppUi, CDpsEngine* aDpsEngine)
    : CActive(CActive::EPriorityStandard), iAppUi(aAppUi),iDpsEngine(aDpsEngine),
      iErrorState(EFalse)
    {
    CActiveScheduler::Add(this);
    StartListening();
    }



// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CEventManager* CEventManager::NewL(CImagePrintUiAppUi* aAppUi, CDpsEngine* aDpsEngine)
    {
    CEventManager* self = new( ELeave ) CEventManager(aAppUi, aDpsEngine);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CEventManager::~CEventManager()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Start listening events
// 
// ---------------------------------------------------------------------------
//
void CEventManager::StartListening()
	{
	FLOG(_L("[IMAGEPRINTUI]\t CEventManager::Start listening"));
	iDpsEngine->DpsEventNotify(iEventRequest, iStatus);
	SetActive();
	}
 	
// ---------------------------------------------------------------------------
// Handles situation depends on result after calling DpsEventNotify 
// 
// ---------------------------------------------------------------------------
//
void CEventManager::HandleReturnStatusL()
	{
    FLOG(_L("[IMAGEPRINTUI]>>> CEventManager; HandleReturnStatus"));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager event value is %d"), iEventRequest.iEvent ));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager iPrintStatus is %x"), iEventRequest.iPrinterEvent.iRepParam.iPrintStatus ));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager iJobStatus is %x"), iEventRequest.iPrinterEvent.iRepParam.iJobStatus ));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager iErrorStatus hex is %x"), iEventRequest.iPrinterEvent.iRepParam.iErrorStatus ));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager iJobEndReason is %x"), iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iMajor ));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager iPaperMinor is %x"), iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iPaperMinor ));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager iInkMinor is %x"), iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iInkMinor ));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager iHardMinor is %x"), iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iHardMinor ));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager iFileMinor is %x"), iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iFileMinor ));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager iDisconnectEnable is %d"), iEventRequest.iPrinterEvent.iRepParam.iDisconnectEnable ));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager iCapabilityChange is %d"), iEventRequest.iPrinterEvent.iRepParam.iCapabilityChange ));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager iNewJobOk is is %d"), iEventRequest.iPrinterEvent.iRepParam.iNewJobOk ));
    
    
	if(iEventRequest.iEvent == EDpsEvtNotifyJobStatus)	
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CEventManager; NotifyPrintProgress"));
		FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager  iImagesPrinted is %d"), iEventRequest.iJobEvent.iRepParam.iImagesPrinted ));
		FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager  iProgress is %d"), iEventRequest.iJobEvent.iRepParam.iProgress ));
	    iAppUi->NotifyPrintProgress(iEventRequest.iJobEvent.iRepParam.iImagesPrinted, 
	                                 iEventRequest.iJobEvent.iRepParam.iProgress);
		}
		
	else if(iEventRequest.iEvent == EDpsEvtNotifyDeviceStatus)
		    {		    		    
		    FLOG(_L("[IMAGEPRINTUI]<<< CEventManager; HandleReturnStatus, EDpsEvtNotifyDeviceStatus"));
		    iAppUi->NotifyPrintStatus(iEventRequest.iPrinterEvent.iRepParam.iPrintStatus);
		    
		    HandleJobStatusL(iEventRequest.iPrinterEvent.iRepParam.iJobStatus);
		    
		    HandleErrorStatusL(iEventRequest.iPrinterEvent.iRepParam.iErrorStatus);
		    
		    if(iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iMajor == EDpsJobErrorPaper)
				{
				FLOG(_L("[IMAGEPRINTUI] CEventManager::EDpsJobErrorPaper"));
				HandlePaperErrorL(iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iPaperMinor);
				}
			else if(iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iMajor == EDpsJobErrorInk)
				{
				FLOG(_L("[IMAGEPRINTUI] CEventManager::EDpsJobErrorInk"));
				HandleInkErrorL(iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iInkMinor);
				}
			else if(iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iMajor == EDpsJobErrorFile)
				{
				FLOG(_L("[IMAGEPRINTUI] CEventManager::EDpsJobErrorFile"));
				HandleFileErrorL(iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iFileMinor);
				}
			else if(iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iMajor == EDpsJobErrorHardware)
				{
				FLOG(_L("[IMAGEPRINTUI] CEventManager::EDpsJobErrorHardware"));
				HandleHardwareErrorL(iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iHardMinor);
				}			
		    
		    if(iEventRequest.iPrinterEvent.iRepParam.iNewJobOk)
				{
				FLOG(_L("[IMAGEPRINTUI] CEventManager::HandleDeviceStatusL, NewJob is OK"));
				iAppUi->NotifyEventL(CEventManager::ENewJobOK );
		    	}
	        //capability change	
			else if(iEventRequest.iPrinterEvent.iRepParam.iCapabilityChange)
				{
				FLOG(_L("[IMAGEPRINTUI] CEventManager::capability change"));
				iAppUi->NotifyEventL(CEventManager::ECapabilityChange);
				}
		    }
    FLOG(_L("[IMAGEPRINTUI]<<< CEventManager; HandleReturnStatus, next call StartListening"));		    
	StartListening();
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEventManager::HandleErrorStatusL(TInt aStatus)
	{
	FLOG(_L("[IMAGEPRINTUI] CEventManager::HandleErrorStatusL"));
	if(aStatus  == EDpsErrorStatusOk)
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager::HandleDeviceStatusL, EDpsErrorStatusOk"));
	    iAppUi->NotifyEventL(CEventManager::ENotErrorState);
		}	
	else if(aStatus == EDpsErrorStatusFatal)
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager::HandleDeviceStatusL, Fatal error"));
		iAppUi->NotifyEventL(CEventManager::EErrorState);
		
		if(iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iInkMinor == EDpsInkEmpty ||
		   iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iInkMinor == EDpsInkLow ||
		   iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iPaperMinor == EDpsPaperJam )
			{
			FLOG(_L("[IMAGEPRINTUI] CEventManager Fatal error, don't show note"));
			}
		else if(iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iMajor == EDpsJobErrorPaper ||
    	   iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iMajor == EDpsJobErrorInk ||
    	   iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iMajor == EDpsJobErrorHardware ||
    	   iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iMajor == EDpsJobErrorFile )
			{
			FLOG(_L("[IMAGEPRINTUI] CEventManager error major, don't show note"));
			}
		 	
		else 
			{
			FLOG(_L("[IMAGEPRINTUI] CEventManager Fatal error, show note"));
		    iAppUi->NotifyEventL(EDpsErrorStatusFatal);	
			}		
		}
	else if(aStatus == EDpsErrorStatusWarning)
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager::HandleDeviceStatusL, Warning"));
		iAppUi->NotifyEventL(CEventManager::EErrorState);
				
	    if(iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iMajor == EDpsJobErrorPaper ||
    	   iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iMajor == EDpsJobErrorInk ||
    	   iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iMajor == EDpsJobErrorHardware ||
    	   iEventRequest.iPrinterEvent.iRepParam.iJobEndReason.iMajor == EDpsJobErrorFile )
			{
			FLOG(_L("[IMAGEPRINTUI] CEventManager warning major, don't show note"));
			}
		else
			{
			FLOG(_L("[IMAGEPRINTUI] CEventManager warning,  show note"));
			iAppUi->NotifyEventL(CEventManager::EWarning);	
			}
		}	
	}
	
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEventManager::HandleJobStatusL(TInt aStatus)
	{
	FLOG(_L("[IMAGEPRINTUI] CEventManager::HandleJobStatusL"));
	if( aStatus == EDpsJobStatusEndedOther)
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager::HandleDeviceStatusL, User cancel from printer"));
		iAppUi->NotifyEventL(EDpsJobStatusEndedOther);
		}
		
	else if( aStatus  == EDpsJobStatusEndedOk)
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager::HandleDeviceStatusL, Job ended normally"));
		iAppUi->NotifyEventL(EDpsJobStatusEndedOk );
		}	
	else if( aStatus  == EDpsJobStatusEndedAbortImmediately)
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager::HandleDeviceStatusL, User has abort job"));
		iAppUi->NotifyEventL(EDpsJobStatusEndedAbortImmediately);
		}	
	
	}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEventManager::HandleHardwareErrorL(TInt aError)
	{
	FLOG(_L("[IMAGEPRINTUI] CEventManager::HandleHardwareErrorL"));
	 if( aError ==  EDpsHardFatal || aError ==  EDpsHardServiceCall ||
         aError ==  EDpsHardNotAvailable || aError ==  EDpsHardBusy || 
         aError ==  EDpsHardLever || aError ==  EDpsHardCoverOpen ||
         aError ==  EDpsHardNoMarkingHead || aError ==  EDpsHardInkCoverOpen ||
         aError ==  EDpsHardNoInkCartridge  )
		{
		FLOG(_L("[IMAGEPRINTUI]\t CEventManager::harware  minor"));
		iAppUi->NotifyEventL(CEventManager::EWarning);
		}	
	else 
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager::Hardware major"));
		iAppUi->NotifyEventL(CEventManager::EShowError);
		} 		
	
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEventManager::HandleInkErrorL(TInt aError)
	{
	FLOG(_L("[IMAGEPRINTUI] CEventManager::HandleInkErrorL"));
	if(aError == EDpsInkEmpty)
	    {
		FLOG(_L("[IMAGEPRINTUI] CEventManager::ink empty"));
		iAppUi->NotifyEventL(CEventManager::EInkEmpty); 
		}
	else if(aError == EDpsInkLow )
	    {
	    FLOG(_L("[IMAGEPRINTUI] CEventManager::ink low"));
	    iAppUi->NotifyEventL(EDpsInkLow);	
	    }
	else if( aError== EDpsInkWaste)	
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager::ink general"));
		iAppUi->NotifyEventL(CEventManager::EWarning);
		}
	else 
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager ink major,  show note"));
	    iAppUi->NotifyEventL(CEventManager::EShowError);	
		}
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEventManager::HandlePaperErrorL(TInt aError)
	{
	FLOG(_L("[IMAGEPRINTUI] CEventManager::HandlePaperErrorL"));
	
	if( aError == EDpsPaperLoad  || aError == EDpsPaperEject ||
	    aError == EDpsPaperMedia || aError == EDpsPaperNearlyEmpty ||
	    aError == EDpsPaperTypeSizeNoMatch ) 
		{
		FLOG(_L("[IMAGEPRINTUI]\t CEventManager::paperMinor, show warning"));
		iAppUi->NotifyEventL(CEventManager::EWarning);
		}
	else if( aError == EDpsPaperJam)
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager::Paper jam"));
		iAppUi->NotifyEventL(EDpsPaperJam);
		}
	else if( aError == EDpsPaperEmpty)
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager::Paper empty"));
		iAppUi->NotifyEventL(EDpsPaperEmpty);
		}	
    else
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager paper major, show note"));
	    iAppUi->NotifyEventL(CEventManager::EShowError);	
		}		
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEventManager::HandleFileErrorL(TInt aError)
	{
	FLOG(_L("[IMAGEPRINTUI] CEventManager::HandleFileErrorL"));
	
	if(aError ==  EDpsFilePrintInfo ||
       aError ==  EDpsFileDecode  )
	    {
		FLOG(_L("[IMAGEPRINTUI] CEventManager::file minor"));
		iAppUi->NotifyEventL(CEventManager::EWarning);
		}	
	else
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager::file major"));
		iAppUi->NotifyEventL(CEventManager::EShowError);
		}
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CEventManager::RunError()
    {
    FLOG(_L("[IMAGEPRINTUI] CEventManager::RunError"));
    
    StartListening();
    if(iStatus.Int() == KErrTimedOut)
    	{
    	FLOG(_L("[IMAGEPRINTUI]<<< CEventManager; RunError, timed out"));	
    	}
    else if( iStatus.Int() == KErrUsbInterfaceNotReady)	
		{
		FLOG(_L("[IMAGEPRINTUI] CEventManager::KErrUsbInterfaceNotReady"));
		// iAppUi->NotifyEventL(KErrUsbInterfaceNotReady);
		}		
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager::RunError iStatus is is %d"), iStatus.Int() ));
    // iAppUi->NotifyEventL(CEventManager::ESeriousError);
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CEventManager::RunL()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CEventManager::RunL"));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CEventManager::RunL iStatus   is %d"), iStatus.Int() ));
    if (iStatus.Int() == KErrNone)
		{
		FLOG(_L("[IMAGEPRINTUI]\t CEventManager::RunL no error"));
	    HandleReturnStatusL();
		}
	else
		{
		FLOG(_L("[IMAGEPRINTUI]\t CEventManager::RunL there is error in iStatus"));
		User::LeaveIfError(iStatus.Int());
		}
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CEventManager::DoCancel()
    {
    FLOG(_L("[IMAGEPRINTUI]>>> CEventManager  DoCancel"));
    iDpsEngine->CancelDpsEventNotify();
    FLOG(_L("[IMAGEPRINTUI]>>> CEventManager  DoCancel complete"));
    }
