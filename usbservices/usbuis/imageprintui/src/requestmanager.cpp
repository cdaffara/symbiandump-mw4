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
* Description:  Handles Dps Request call with AO
*
*/


#include <e32base.h>
#include <dpsdefs.h>
#include <s32file.h>
#include <f32file.h>
#include <coeutils.h>
#include <imageprintui.rsg>

#include "requestmanager.h"
#include "imageprintuiappui.h"
#include "pictbridge.h"
#include "notes.h"
#include "capabilitymanager.h"
#include "imageprintuidebug.h"

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CRequestManager::CRequestManager(CDpsEngine* aDpsEngine,
								 CCapabilityManager* aCapabilityManager, CImagePrintUiAppUi* aAppUi)
    : CActive(CActive::EPriorityStandard),  iDpsEngine(aDpsEngine), 
      iCapabilityManager(aCapabilityManager), iAppUi(aAppUi), iNumberOfImages(0)
    {
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// 2nd phase construction
// ---------------------------------------------------------------------------
//

void CRequestManager::ConstructL()
    {
    FLOG(_L("[IMAGEPRINTUI]>>> CRequestManager  ConstructL"));
    iFileExist = EFalse;
    iCongFileName = HBufC::NewL(iDpsEngine->DpsFolder().Length() + KConfigLength);
    iCongFileName->Des().Copy(iDpsEngine->DpsFolder());
    iCongFileName->Des().Append(KConfigInfoFile);
    
    // always start with configure, this is part of the DPS protocol
    ChooseDpsRequestTypeL(EConfigurePrintService); 
    
    FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager  ConstructL"));
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CRequestManager* CRequestManager::NewL( CDpsEngine* aDpsEngine,
									    CCapabilityManager* aCapabilityManager,CImagePrintUiAppUi* aAppUi)
    {
    CRequestManager* self = CRequestManager::NewLC( aDpsEngine, aCapabilityManager, aAppUi);
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CRequestManager* CRequestManager::NewLC(CDpsEngine* aDpsEngine, 
                                        CCapabilityManager* aCapabilityManager, 
                                        CImagePrintUiAppUi* aAppUi)
    {
    CRequestManager* self = new( ELeave ) CRequestManager(aDpsEngine, 
    													  aCapabilityManager, aAppUi);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CRequestManager::~CRequestManager()
    {
    FLOG(_L("[IMAGEPRINTUI]>>> CRequestManager  Destructor called"));
    delete iCongFileName;
    Cancel();         
    FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager  Destructor END"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CRequestManager::ReIssueDoDpsRequestL()
	{
	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager  ReIssueDoDpsRequestL"));
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CRequestManager::reissue request is %d"), iRequestType ));
	ChooseDpsRequestTypeL(iRequestType);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void	 CRequestManager::RunL()
    {
    FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager  RunL"));
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CRequestManager::RunL iStatus   is %d"), iStatus.Int() ));
    if(iStatus.Int() == KErrNone)
    	{
	    switch ( iRequestType )
	        {
	        case EConfigurePrintService:
	            FLOG(_L("[IMAGEPRINTUIENGINE]\t CRequestManager: RunL, Handle configure"));
	            HandleConfigureL();
	            break;
	           
	        case EGetJobStatus:
	            FLOG(_L("[IMAGEPRINTUIENGINE]\t CRequestManager: RunL, HandleGetJobStatusL"));
	            HandleGetJobStatusL();
	            break;
	            
	        case EGetDeviceStatus:
	            FLOG(_L("[IMAGEPRINTUIENGINE]\t CRequestManager: RunL, Handle Device Status"));
	        	HandleDeviceStatusL();
	        	break;
	        	
	        case EStartJob:
	        	FLOG(_L("[IMAGEPRINTUIENGINE]\t CRequestManager: RunL, Handle Start Job"));
	        	HandleStartJobL();
	        	break;
	        	
	        case EAbortJob:
	            FLOG(_L("[IMAGEPRINTUIENGINE]\t CRequestManager: RunL, HandleAbortL")); 
	        	HandleAbortL();
	        	break;
	        	
	        case EContinueJob:
	            FLOG(_L("[IMAGEPRINTUIENGINE]\t CRequestManager: RunL, EContinueJob")); 
	        	break;
	        	
	        default:
	        	FLOG(_L("[IMAGEPRINTUIENGINE]\t CRequestManager: RunL, default branch")); 
	            break;
	        }
    	}
    else
    	{
    	FLOG(_L("[IMAGEPRINTUIENGINE]\t CRequestManager: RunL, call leave, it call RunError")); 
    	User::LeaveIfError(iStatus.Int());
    	}
	FLOG(_L("[IMAGEPRINTUI]>>> CRequestManager  RunL"));
    }


// -----------------------------------------------------------------------------
// CRequestManager::RunError
// Standard active object error function.
// -----------------------------------------------------------------------------
//

TInt CRequestManager::RunError(TInt aError)
    {
    FTRACE(FPrint(_L("[IMAGEPRINTUI] CRequestManager::RunError: err %d from RunL"), aError));
    if(aError == KErrNotFound)
		{
		FLOG(_L("[IMAGEPRINTUI] CRequestManager  Pictures not found when printing"));
		TRAPD(err,iAppUi->NotifyRequestL(EPicturesNotFound));
		if(err)
			{
			FLOG(_L("[IMAGEPRINTUI] CRequestManager  err calling notify"));
			}
		}
	else if(aError == KErrTimedOut || aError == KErrInUse)
    	{
    	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager; RunError, timed out or in use"));
    	iAppUi->HandleRequestTime(KRetryClose);
    	}		
    	
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CRequestManager::DoCancel()
    {
    FLOG(_L("[IMAGEPRINTUI]>>> CRequestManager  DoCancel"));
    iDpsEngine->CancelDpsRequest(); 	
    FLOG(_L("[IMAGEPRINTUI]>>> CRequestManager  DoCancel complete"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CRequestManager::ExecuteStartJobRequestL()
	{
	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager, ExecuteStartJobRequestL "));
	iStart.iReqParam.Reset();
	            
    TUint layout = iCapabilityManager->Layout();
	TUint quality = iCapabilityManager->Quality();
	TUint paperSize = iCapabilityManager->PaperSize();
    
    //fill request parameter by retrieved values
    TDpsArgsInt req_quality,req_papersize, req_layout;
    
    req_quality.iElement = EDpsArgQuality;
    req_quality.iContent = quality;
    iStart.iReqParam.iJobConfig.AppendL(req_quality);
    
    req_papersize.iElement = EDpsArgPaperSize;
    req_papersize.iContent = paperSize;
    iStart.iReqParam.iJobConfig.AppendL(req_papersize);

    req_layout.iElement = EDpsArgLayout;
    req_layout.iContent = layout;
    iStart.iReqParam.iJobConfig.AppendL(req_layout);
              
   
    // retrieve  images
    FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager, Get Images"));
    iImageArrayFlat = iAppUi->ImagesToPrint(); 	// not taking ownership
    
   
    iNumberOfImages = iImageArrayFlat->Count();
	TDpsPrintInfo* helpTDpsPrintInfo = new (ELeave) TDpsPrintInfo[iNumberOfImages];
	CleanupStack::PushL(helpTDpsPrintInfo);          
    // go through the list of images and add info for start job request 
    
    for(int i=0; i<iNumberOfImages; i++)
    	{
    	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager, Start job, append image"));
    	// check if file really exist, use may have delete it after choose print
    	// that will set printer unstable state
    	iFileExist = ConeUtils::FileExists(iImageArrayFlat->operator[](i));
    	if(iFileExist)
    		{
    		FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager, Start job, file exist"));
    		helpTDpsPrintInfo[i].iFile.Copy(iImageArrayFlat->operator[](i));
        	iStart.iReqParam.iPrintInfo.AppendL(helpTDpsPrintInfo[i]);
    		}
    	}
    
    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CRequestManager iNumberOfImages  is %d"), iNumberOfImages));	        
    iAppUi->NumberOfImages(iNumberOfImages); 
    if(!iFileExist)
    	{
    	FLOG(_L("[IMAGEPRINTUI]>>> CRequestManager, ExecuteStartJobRequestL, file not exist "));
        iAppUi->Notes()->ShowErrorMsgL(R_ERROR_FILE_NOT_FOUND); 
    	}
    else
    	{
    	iDpsEngine->DoDpsRequestL(&iStart, iStatus);
    	}
    
	CleanupStack::PopAndDestroy(helpTDpsPrintInfo);

	
	FLOG(_L("[IMAGEPRINTUI]>>> CRequestManager, ExecuteStartJobRequestL "));
	}
// ---------------------------------------------------------------------------
// Choose DoDpsRequest type
// ---------------------------------------------------------------------------
//
void CRequestManager::ChooseDpsRequestTypeL(const TRequestType aType)
	{
	iRequestType = aType;
	switch ( aType )	
	        {
	      		        
	        case EConfigurePrintService:
	            FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager, ChooseDpsRequestType Configure"));
	            iConfig.iReqParam.Reset();
	            // retrieve current phone settings from DpsEngine and fill req param
	            iDpsEngine->GetDpsConfigL(iConfig.iReqParam);
	            iDpsEngine->DoDpsRequestL(&iConfig, iStatus);
	            break;
	        
	        
	        case EGetJobStatus:
	        	// no need to fill parameter for this Dps request
	        	iJobFinished = EFalse;
	        	iDpsEngine->DoDpsRequestL(&iJob, iStatus);
	            break;
	            
	        case EGetDeviceStatus:
	            FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager, ChooseDpsRequestType Get Device status"));
	        	// no need to fill parameter for this Dps request
	        	iDpsEngine->DoDpsRequestL(&iPrinter, iStatus);
	        	break;
	        	
	        case EStartJob:
	         	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager, ChooseDpsRequestType Start Job"));
	            ExecuteStartJobRequestL();
	        	break;
	        
	        case EAbortJob:
	        	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager, ChooseDpsRequestType Abort Job"));
	            // fill only abort style ref. PictBridge Spec. CIPA DC-001-2003 page 68. 
	            iAbort.iReqParam.iAbortStyle = EDpsAbortStyleImmediately;
   				iDpsEngine->DoDpsRequestL(&iAbort, iStatus);
	        	break;
	        	
	        case EContinueJob:
	        FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager, ChooseDpsRequestType Continue Job"));
	            // no need to fill parameter for this Dps request
	        	iDpsEngine->DoDpsRequestL(&iContinue, iStatus);
	        	break;
	        	
	        default:
	        	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager, ChooseDpsRequestType default branch"));
	            break;
	        }
	        
	    if(!IsActive()) // check if active, user might try to print twice which is causing collision
	    	{
	    	if(!iFileExist && iRequestType == EStartJob)
	    		{
	    		FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager, ChooseDpsRequestType, file not exist"));
	    		}
	    	else	
	    		{
	    		SetActive();
	    		}
	        
	    	}
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CRequestManager::HandleDeviceStatusL()
	{
	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager HandleDeviceStatus Start"));
	
	if(!iPrinter.iRepParam.iNewJobOk)
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager,HandleDeviceStatus NOK "));
		iAppUi->NotifyRequestL(EGetDeviceStatusNOK);
		}
	else
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager,HandleDeviceStatus OK "));
		iAppUi->NotifyRequestL(EGetDeviceStatusOK);
		ChooseDpsRequestTypeL(EStartJob);
		}
	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager HandleDeviceStatus End"));
	}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CRequestManager::HandleAbortL()
	{
	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager HandleAbort Start"));
	if(iAbort.iResult.iMajorCode != EDpsResultOk)
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager HandleAbort EAbortJobNOK"));
		iAppUi->NotifyRequestL(EAbortJobNOK);
		}
	else
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager HandleAbort EAbortJobOK"));
		iAppUi->NotifyRequestL(EAbortJobOK);
		}
	
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CRequestManager::HandleStartJobL()
	{
	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager HandleStartJob Start"));
		
	if(iStart.iResult.iMajorCode == EDpsResultOk)
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager,HandleStartJob OK "));
		iAppUi->NotifyRequestL(EStartJobOK);
		}
	else
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager,HandleStartJob NOK "));
		iAppUi->NotifyRequestL(EStartJobNOK);
		}	
		
	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager HandleStartJob End"));
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CRequestManager::HandleGetJobStatusL()
	{ 
	if(iJob.iResult.iMajorCode == EDpsResultOk)
		{
		iAppUi->NotifyRequestL(EGetJobStatusOK);
		}
	else
		{
		iAppUi->NotifyRequestL(EGetJobStatusNOK);
		} 
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CRequestManager::HandleConfigureL()
	{
	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager HandleConfigure Start"));
	if(iConfig.iRepParam.iPrintAvailable == EDpsPrintServiceAvailableTrue) 
		{
		// save name for AppUi
		FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager,HandleConfigure OK "));
		iVendorName = iConfig.iRepParam.iVendorName;
		iProductName = iConfig.iRepParam.iProductName;
		SaveConfigInfoL();
		iAppUi->NotifyRequestL(EConfigurePrintServiceOK);
	    
		}
	
	else if(iConfig.iResult.iMajorCode == EDpsResultNotExecuted) 
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager,HandleConfigure OK old session"));
		ReadConfigInfoL();
		iAppUi->NotifyRequestL(EConfigurePrintServiceOK);
		}
	else if(iConfig.iRepParam.iPrintAvailable == EDpsPrintServiceAvailableFalse)
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager,HandleConfigure NOK "));
		iAppUi->NotifyRequestL(EConfigurePrintServiceNOK);
		}
		    
	iConfig.iRepParam.Reset();	
	FLOG(_L("[IMAGEPRINTUI]<<< CRequestManager HandleConfigure End"));
	}
	
void CRequestManager::GetPrinterName(TDes8& aVendorName, TDes8& aProductName)
	{
	FLOG(_L("[IMAGEPRINTUI]\t CRequestManager::GetPrinterName BEGIN"));
	aVendorName = iVendorName.Ptr();
	aProductName = iProductName.Ptr();
	FLOG(_L("[IMAGEPRINTUI]\t CRequestManager::GetPrinterName END"));
	}
	
	
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//

void CRequestManager::SaveConfigInfoL()
	{
	FLOG(_L("[IMAGEPRINTUI]<<< CEventManager;SaveConfigInfoL "));
	RFileWriteStream stream;
  	CleanupClosePushL(stream);
	if((stream.Replace(CCoeEnv::Static()->FsSession(),iCongFileName->Des() ,EFileWrite)) == KErrNone)
		{
		ExternalizeL(stream);
		}
	CleanupStack::PopAndDestroy(&stream);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CRequestManager::ExternalizeL(RWriteStream& aStream) const
	{
	FLOG(_L("[IMAGEPRINTUI]<<< CEventManager;ExternalizeL "));	
	aStream << iVendorName;
	aStream << iProductName;
	aStream.CommitL();
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CRequestManager::ReadConfigInfoL()
    {
    FLOG(_L("[IMAGEPRINTUI]\t CRequestManager::ReadConfogInfoL Sstart"));
    RFileReadStream readStream;
    User::LeaveIfError( readStream.Open(CCoeEnv::Static()->FsSession(), 
    					iCongFileName->Des(), EFileRead) );
    readStream.PushL();
    readStream >> iVendorName; 
	readStream >> iProductName;
	
    CleanupStack::PopAndDestroy(&readStream);
    
    FLOG(_L("[IMAGEPRINTUI]\t CRequestManager::ReadConfogInfoL end"));
    }

// end of file
