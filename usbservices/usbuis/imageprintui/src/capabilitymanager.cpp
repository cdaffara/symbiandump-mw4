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
* Description:  Retrieve printing capabilites from printer 
*
*/


#include "capabilitymanager.h"
#include "imageprintuidebug.h"
#include "imageprintuiappui.h"
#include <dpsdefs.h>
#include <dpsparam.h>
#include <barsc.h>
#include <barsread.h>
#include <bautils.h>
#include <f32file.h>
#include <pathinfo.h> 
#include <phonecapability.rsg>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCapabilityManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCapabilityManager* CCapabilityManager::NewL(CImagePrintUiAppUi* aAppUi, CDpsEngine* aEngine)
    {
    FLOG(_L("[IMAGEPRINTUI]\tCCapabilityManager:NewL"));
    CCapabilityManager* self 
        = new (ELeave) CCapabilityManager(aAppUi, aEngine);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); 
    return self;
    }


// -----------------------------------------------------------------------------
// CCapabilityManager::~CCapabilityManager
// C++ default destructor
// -----------------------------------------------------------------------------
//    
CCapabilityManager::~CCapabilityManager()
    {
    FLOG(_L("[IMAGEPRINTUI]\tCCapabilityManager: desctructor"));
    Cancel(); // cancels any existing active object orders for iStatus
    
    iReturnArray.Close();
    iCurrentLayouts.Close();
    iCurrentPaperSizes.Close();
    iCurrentQualities.Close();
       
    iPhoneSuppQuality.Close();
    iPhoneSuppPaperSize.Close();   
    iPhoneSuppLayout.Close();
    FLOG(_L("[IMAGEPRINTUI]\tCCapabilityManager: desctructor complete"));
    }


// -----------------------------------------------------------------------------
// CCapabilityManager::CCapabilityManager
// C++ default constructor 
// -----------------------------------------------------------------------------
//
CCapabilityManager::CCapabilityManager(CImagePrintUiAppUi* aAppUi, CDpsEngine* aEngine)
    : CActive(EPriorityStandard), 
    iAppUi(aAppUi), iDpsEngine(aEngine),
    iUserSelectedQuality(EDpsPrintQualityDefault),
    iUserSelectedPaperSize(EDpsPaperSizeDefault),
    iUserSelectedLayout(EDpsLayoutDefault)
    {
    CActiveScheduler::Add(this);
    }


// -----------------------------------------------------------------------------
// CCapabilityManager::RunError
// Standard active object error function.
// -----------------------------------------------------------------------------
//

TInt CCapabilityManager::RunError(TInt aError)
    {
    FTRACE(FPrint(_L("[IMAGEPRINTUI]CCapabilityManager RunError is %d ."), aError));
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CCapabilityManager::RunL
// This function will be called upon a change in the watched key.
// -----------------------------------------------------------------------------
//
void CCapabilityManager::RunL()
    {
    FLOG(_L("[IMAGEPRINTUI]\tCCapabilityManager::RunL"));
    if(iStatus.Int() == KErrNone && iLayoutForPaperSize)
    	{
    	FLOG(_L("[IMAGEPRINTUI]\tCCapabilityManager::RunL layout for papersize asked"));
    	HandleLayoutForPaperSize();
    	return;
    	}
    
    if ( iStatus.Int() == KErrNone )
    	{
    	FLOG(_L("[IMAGEPRINTUI]\tCCapabilityManager::RunL, no error in iStatus"));
        HandleCapabilitiesL(iCapabilityType); 
    	}
    
    else 
    	{
    	FLOG(_L("[IMAGEPRINTUI]\tCCapabilityManager::RunL, there is error in iStatus"));
    	FTRACE(FPrint(_L("[IMAGEPRINTUI]\tCCapabilityManager::RunL: ERROR is %d  ."), iStatus.Int()));
    	iAppUi->HandleCapabilityTime(KRetryAsk);
    	FLOG(_L("[IMAGEPRINTUI]\tCCapabilityManager::Timer called"));
    	}
    FLOG(_L("[IMAGEPRINTUI]\tCCapabilityManager::RunL complete."));
    }


// -----------------------------------------------------------------------------
// CCapabilityManager::ConstructL
// Construct member variable iPersonalityRepository that is to access the
// repository. Then start listening of the changes in the repository.
// -----------------------------------------------------------------------------
//
void CCapabilityManager::ConstructL()
    {
    FLOG(_L("[IMAGEPRINTUI]<<<CCapabilityManager: ConstructL"));
    iCapabilityNotChanged = EFalse;
    iIsCapabilityEmpty = EFalse;
    iLayoutForPaperSize = EFalse;
    
    iPhoneSuppLayout = RArray<TUint>(KDefaultArray);  
    iPhoneSuppPaperSize = RArray<TUint>(KDefaultArray);
    iPhoneSuppQuality = RArray<TUint>(KDefaultArray);  
    iCurrentQualities = RArray<TUint>(KDefaultArray);
    iCurrentPaperSizes = RArray<TUint>(KDefaultArray);
    iCurrentLayouts = RArray<TUint>(KDefaultArray);
    
    iReturnArray = RArray<TUint>(KDefaultArray);
    
    GetPhoneConfigL();
    FLOG(_L("[IMAGEPRINTUI]>>>CCapabilityManager: ConstructL complete"));        
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCapabilityManager::AskCapabilitiesL(TBool aChanged)
	{
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager::AskCapabilitiesL iStatus   is %d"), iStatus.Int() ));  
    
	if(aChanged)
		{
		if(iStatus.Int() == KErrNotReady) // this is collision situation, have asked earlier and event capability
			{                             // change requires asking again when AO is already active
		    FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager::AskCapabilitiesL not ready when capability change")); 
		    return;	
			}
		else
			{
		    FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager::AskCapabilities cancel pending"));
			Cancel();
			}
		
		}
	if(!IsActive())
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager::start ask paper size "));
	    AskPaperSizeL();	
		}
	else
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager::cannot ask quality, already active "));
		}
	
	FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager::AskCapabilities"));
    
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCapabilityManager::AskQualityL()
	{
	iCap.iRepParam.Reset();
	FLOG(_L("[IMAGEPRINTUI]>>> CCapabilityManager::AskQualityL "));
	iCap.iReqParam.iCap = EDpsArgQualities;
    iCapabilityType = EQualities;
    iDpsEngine->DoDpsRequestL(&iCap, iStatus);	
	SetActive();
	FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager::AskQualityL "));
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CCapabilityManager::AskPaperSizeL()
	{
	iCap.iRepParam.Reset();
	FLOG(_L("[IMAGEPRINTUI]>>> CCapabilityManager::AskPaperSizeL "));
	iCap.iReqParam.iCap = EDpsArgPaperSizes;
    iCapabilityType = EPaperSize;
    iDpsEngine->DoDpsRequestL(&iCap, iStatus);	
	SetActive();
	FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager::AskPaperSizeL "));
	}	

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CCapabilityManager::AskLayoutL()
	{
	iCap.iRepParam.Reset();
	FLOG(_L("[IMAGEPRINTUI]>>> CCapabilityManager::AskLayoutL "));
    
    iCap.iReqParam.iCap = EDpsArgLayouts;
    iCap.iReqParam.iAttribute = EDpsPaperSizeDefault;
    iCapabilityType = ELayouts;
    iDpsEngine->DoDpsRequestL(&iCap, iStatus);	
	SetActive();
	FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager::AskLayoutL "));
	}	

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCapabilityManager::AskLayoutForPaperSizeL(TUint aPaperSize)
	{
	FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager::AskLayoutForPaperSizeL "));
	iCap.iRepParam.Reset();
	iCap.iReqParam.iCap = EDpsArgLayouts;
    iCap.iReqParam.iAttribute = aPaperSize;
   	iDpsEngine->DoDpsRequestL(&iCap, iStatus);
   	
	if(!IsActive())
		{
		iLayoutForPaperSize = ETrue;
	    SetActive(); 	
		}
	}


// -----------------------------------------------------------------------------
// CCapabilityManager::DoCancel
// Standard active object cancellation function.
// -----------------------------------------------------------------------------
//
void CCapabilityManager::DoCancel()
    {
    FLOG(_L("[IMAGEPRINTUI]\tCCapabilityManager::DoCancel"));
    iDpsEngine->CancelDpsRequest();
    FLOG(_L("[IMAGEPRINTUI]\tCCapabilityManager::DoCancel complete"));
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//	
RArray<TUint>& CCapabilityManager::GetCapabilitiesL(TInt aCapabilities)
    {
    FLOG(_L("[IMAGEPRINTUI]<<<CCapabilityManager.GetCapabilities"));
    iReturnArray.Reset();
    
    switch ( aCapabilities )
	    {        
	    case EDpsArgQualities:
	    	for(int i = 0; i < iCurrentQualities.Count(); i++ )
	    		{
	    		iReturnArray.AppendL(iCurrentQualities.operator[](i));
	    		}
	        break;
	  
	    case EDpsArgPaperSizes:
	        for(int i = 0; i < iCurrentPaperSizes.Count(); i++ )
	    		{
	    		iReturnArray.AppendL(iCurrentPaperSizes.operator[](i));
	    		}	
	        break;
	        
	    case EDpsArgLayouts:
	    	for(int i = 0; i < iCurrentLayouts.Count(); i++ )
	    		{
	    		iReturnArray.AppendL(iCurrentLayouts.operator[](i));
	    		}
			break;		
	    default:
	        FLOG(_L("[IMAGEPRINTUI]>>>CCapabilityManager.GetCapabilities default brach"));
	        break;  
	    }     
    FLOG(_L("[IMAGEPRINTUI]>>>CCapabilityManager.GetCapabilities")); 
    return iReturnArray;
    }  

// ---------------------------------------------------------------------------
// User has choosed value for saving
// ---------------------------------------------------------------------------
//
void CCapabilityManager::SetValue(TInt aCapability, TUint aValue)
    { 
    FLOG(_L("[IMAGEPRINTUI]<<<CCapabilityManager.SetValue"));
    switch ( aCapability )
	    {        
	    case EDpsArgQualities:
	        FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager::SetValue in quality is %x"), aValue ));
	        iUserSelectedQuality = aValue;	
	        break;
	            
	    case EDpsArgPaperSizes:
	        FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager::SetValue in papersize is %x"), aValue ));
	        iUserSelectedPaperSize = aValue;
	        // now user has select papersize, retrive layout for certain papersize
	        TRAPD(err,AskLayoutForPaperSizeL(aValue));
	        if(err)
	        	{
	        	FLOG(_L("[IMAGEPRINTUI]<<<CCapabilityManager.AskLayoutForPaperSizeL was unsuccessfull"));
	        	}
	        break;
	            
	    case EDpsArgLayouts:
	    	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager::SetValue in layouts is %x"), aValue ));
	        iUserSelectedLayout = aValue; 	
	        break;
	    default:
	     	break;  
	    } 		
	FLOG(_L("[IMAGEPRINTUI]>>>CCapabilityManager.SetValue"));    
    }     

// ---------------------------------------------------------------------------
//  UI requires current value
// ---------------------------------------------------------------------------
//	
TUint CCapabilityManager::CurrentPrintSettings(TInt aDpsArg)
	{
	FLOG(_L("[IMAGEPRINTUI]<<<CCapabilityManager::CurrentPrintSettings"));
	TUint retValue;
	if(aDpsArg == EDpsArgQualities)
		{
        retValue = iUserSelectedQuality;		
		}
    else if(aDpsArg == EDpsArgPaperSizes)
    	{
    	retValue = iUserSelectedPaperSize;
    	}
    else if(aDpsArg == EDpsArgLayouts)
    	{
    	retValue = iUserSelectedLayout;
    	}
    else if(aDpsArg != EDpsArgQualities &&  
            aDpsArg != EDpsArgPaperSizes && aDpsArg != EDpsArgLayouts)
    	{
    	FLOG(_L("[IMAGEPRINTUI]\t CCapabilityManager: Wrong value asked"));
    	retValue = (TUint)KErrArgument;
    	}
    FLOG(_L("[IMAGEPRINTUI]>>>CCapabilityManager CurrentPrintSettings"));
    return retValue;		
	}   
	
// ---------------------------------------------------------------------------
// Gives paper size value for DoDpsRequest
// ---------------------------------------------------------------------------
//	
TUint CCapabilityManager::PaperSize()
	{
	return iUserSelectedPaperSize;
	}   
	
// ---------------------------------------------------------------------------
// Gives layout value for DoDpsRequest
// ---------------------------------------------------------------------------
//
TUint CCapabilityManager::Layout()
	{
	return iUserSelectedLayout;
	}   
	
// ---------------------------------------------------------------------------
// Gives quality value for DoDpsRequest
// ---------------------------------------------------------------------------
//
TUint CCapabilityManager::Quality()
	{
	return iUserSelectedQuality;
	}   
	
// ---------------------------------------------------------------------------
//  Goes through phone supported quality values and compare with printer supported,
//  if both supported save value 
// ---------------------------------------------------------------------------

void CCapabilityManager::StoreQualitiesL(RArray<TUint>& aPrinterQualities)
	{
	FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: Store qualities"));
	TInt countPrinter, countPhone;
	TUint compareValue;
	countPrinter = aPrinterQualities.Count();
	countPhone = iPhoneSuppQuality.Count();
	iIsCapabilityEmpty = EFalse;
	iCurrentQualities.Reset();
	
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager aPrinterQualities count is %d"), aPrinterQualities.Count() ));
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager iPhoneSuppQuality count is %d"), iPhoneSuppQuality.Count() ));
  
	
	for(int r=0; r < countPrinter; r++)
		{
		compareValue = aPrinterQualities[r];
		for(int d=0; d < countPhone; d++)
			{
		    if(compareValue == iPhoneSuppQuality[d])
		    	{
		    	iIsCapabilityEmpty = ETrue;
		    	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager compareValue save quality is %x"), compareValue ));
		    	iCurrentQualities.AppendL(compareValue);
		    	}
			}
		}
	if(!iIsCapabilityEmpty)	
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: qualities add default value"));
		iCurrentQualities.AppendL(EDpsPrintQualityDefault);
		}
	FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: qualities stored, ask layout"));	
    iCurrentQualities.Sort();
	AskLayoutL();	
	}
	
		
// ---------------------------------------------------------------------------
//  Goes through phone supported paper size values and compare with printer supported,
//  if both supported save value 
// ---------------------------------------------------------------------------
void CCapabilityManager::StorePaperSizeL(RArray<TUint>& aPrinterPaperSize)
    {
    FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: Store paper size"));
    TInt countPrinter, countPhone;
    TUint compareValue;
	countPrinter = aPrinterPaperSize.Count();
	countPhone = iPhoneSuppPaperSize.Count();
	iIsCapabilityEmpty = EFalse;
	iCurrentPaperSizes.Reset();
	
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager aPrinterPaperSize count is %d"), aPrinterPaperSize.Count() ));
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager iPhoneSuppPaperSize count is %d"), iPhoneSuppPaperSize.Count() ));
  
	
	for(int r=0; r < countPrinter; r++)
		{
		compareValue = aPrinterPaperSize[r];
		for(int f=0; f < countPhone; f++)
			{
		    if(compareValue == iPhoneSuppPaperSize[f])
		    	{
		    	iIsCapabilityEmpty = ETrue;
		    	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager compareValue save paper size is %x"), compareValue ));
		    	iCurrentPaperSizes.AppendL(compareValue);
		    	}
			}	
		}
	if(!iIsCapabilityEmpty)	
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: paper size add default value"));
		iCurrentPaperSizes.AppendL(EDpsPaperSizeDefault);
		}
	FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: paper sizes stored, ask quality"));
	iCurrentPaperSizes.Sort();
	AskQualityL();	
    }
      
// ---------------------------------------------------------------------------
//  Goes through phone supported layout values and compare with printer supported,
//  if both supported save value 
// ---------------------------------------------------------------------------
void CCapabilityManager::StoreLayouts(RArray<TUint>& aPrinterLayouts)
	{
	FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: Store layouts"));
	TInt countPrinter, countPhone;
	TInt ret = KErrNone;
	TUint compareValue;
	countPrinter = aPrinterLayouts.Count();
	countPhone = iPhoneSuppLayout.Count();	
	iIsCapabilityEmpty = EFalse;
	iCurrentLayouts.Reset();
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager aPrinterLayouts count is %d"), aPrinterLayouts.Count() ));
	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager iPhoneSuppLayout count is %d"), iPhoneSuppLayout.Count() ));
  
	for(int r=0; r < countPrinter; r++)
		{
		compareValue = aPrinterLayouts[r];
		for(int e=0; e < countPhone; e++)
			{
		    if(compareValue == iPhoneSuppLayout[e])
		    	{
		    	iIsCapabilityEmpty = ETrue;
		    	FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager compareValue save layout is %x"), compareValue ));
		    	if ( (ret = iCurrentLayouts.Append(compareValue)) != KErrNone)
		    	    {
		    	    FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: Store layouts *** append error"));
		    	    }
		    	}
			}	
		}
		
	if(!iIsCapabilityEmpty)	
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: layout add default value"));
		ret = iCurrentLayouts.Append(EDpsLayoutDefault);
		if (ret != KErrNone) 
		    {
            FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: Store layouts *** append error"));
		    }
		}
	
	iCurrentLayouts.Sort();	
	if(iCapabilityAskFailed)
		{
        // all capabilities has asked
        // asking one of the tree capabilities was unsuccessfull
	    FLOG(_L("[IMAGEPRINTUI]<<< :CCapabilityManager, Capabilities NOT ready "));	
	    iAppUi->CapabilitiesReady(); 
	 	}
	else
		{
		FLOG(_L("[IMAGEPRINTUI]<<< :CCapabilityManager, Capabilities are ready "));
		iAppUi->CapabilitiesReady(); 
		}		
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCapabilityManager::HandleLayoutForPaperSize()
	{
	FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager::HandleLayoutForPaperSize"));
	StoreLayouts(iCap.iRepParam.iContent); 
	}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCapabilityManager::HandleCapabilitiesL(TCapabilityType aType)
	{
		
	if(aType == EQualities)
		{
		if(iCap.iResult.iMajorCode == EDpsResultOk)
			{					
			FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: Dps request qualities capability OK"));
			FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager HandleCapabilitiesL qualities amount is %d"), iCap.iRepParam.iContent.Count() ));	
			for(int r=0; r < iCap.iRepParam.iContent.Count(); r++)
				{
				FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager HandleCapabilitiesL qualities is %x"), iCap.iRepParam.iContent[r] ));	
				}
	        StoreQualitiesL(iCap.iRepParam.iContent);
			}
		else
			{
			FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: Dps request qualities capability NOK"));
			iCapabilityAskFailed = ETrue;
			// if fails add default value
			iAppUi->NotifyError();
			iCurrentQualities.Reset();
			iCurrentQualities.AppendL(EDpsPrintQualityDefault);
			AskLayoutL();
			}
		
		}
 	else if(aType == EPaperSize)
 		{
 		if(iCap.iResult.iMajorCode == EDpsResultOk)
 			{
 		    FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: Dps request paper size capability OK"));
 		    FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager HandleCapabilitiesL papersize amount is %d"), iCap.iRepParam.iContent.Count() ));
			for(int r=0; r < iCap.iRepParam.iContent.Count(); r++)
				{
				FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager HandleCapabilitiesL paper size is %x"), iCap.iRepParam.iContent[r] ));
				}
	        
 		    StorePaperSizeL(iCap.iRepParam.iContent);
 			}
 		else
 			{
 			FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: Dps request paper size capability NOK"));
 			iCapabilityAskFailed = ETrue;
 			// if fails add default value;
 			iAppUi->NotifyError();
 			iCurrentPaperSizes.Reset();
 			iCurrentPaperSizes.AppendL(EDpsPaperSizeDefault);
 			AskQualityL();
 			}
 		
 		}
 	else if(aType == ELayouts)
 		{
 		if(iCap.iResult.iMajorCode == EDpsResultOk)
 			{
 			FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: Dps request layout capability OK"));
 			FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager HandleCapabilitiesL layout amount is %d"), iCap.iRepParam.iContent.Count() ));	
			for(int r=0; r < iCap.iRepParam.iContent.Count(); r++)
				{
				FTRACE(FPrint(_L("[IMAGEPRINTUI]\t CCapabilityManager HandleCapabilitiesL layout is %x"), iCap.iRepParam.iContent[r] ));	
				}
 		    StoreLayouts(iCap.iRepParam.iContent); 	
 			}
 		else
 			{
 			FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager: Dps request layout capability NOK"));
 			iCapabilityAskFailed = ETrue;
 			//if fails add default value
 			iAppUi->NotifyError();
 			iCurrentLayouts.Reset();
 			iCurrentLayouts.AppendL(EDpsLayoutDefault);
 			// complete even if not succesfull, UI must activate
 			iAppUi->CapabilitiesReady();
 			}
 		
 		}
    
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCapabilityManager::GetPhoneConfigL()
    {
    FLOG(_L("[IMAGEPRINTUI]>>> :CCapabilityManager, GetPhoneConfigL BEGIN"));
    
        
    TBuf<KResource> resourceFile(PathInfo::RomRootPath());
    TBuf<KResource> length(KPhoneCapability);    
    resourceFile.SetLength(KDriver + length.Length());
    resourceFile.Replace(KDriver, length.Length(), KPhoneCapability);
    
    RResourceFile resource;
    resource.OpenL(CCoeEnv::Static()->FsSession(),  resourceFile);
    FLOG(_L("[IMAGEPRINTUI]>>> CImagePrintUi:CCapabilityManager, Resource open"));
    
    CleanupClosePushL(resource);
    resource.ConfirmSignatureL(KPhoneCapabilityVersion);
    
    HBufC8* id = resource.AllocReadLC(PHONECAPABILITY_CONFIG);
    
    TResourceReader reader;
    reader.SetBuffer(id);   
    
    TInt qualityCount = reader.ReadUint8();
    TInt papersizeCount = reader.ReadUint8();
    TInt layoutCount  = reader.ReadUint8();
      
       
    for (TInt i = 0; i < qualityCount; i++)
    	{
    	iPhoneSuppQuality.AppendL(reader.ReadUint16());
    	}
    FLOG(_L("[IMAGEPRINTUI]>>> CImagePrintUi:CCapabilityManager, quality readed"));
    	
    for (TInt i = 0; i < papersizeCount; i++)
    	{
       	iPhoneSuppPaperSize.AppendL(reader.ReadUint16());
    	}		
    
    FLOG(_L("[IMAGEPRINTUI]>>> CImagePrintUi:CCapabilityManager, papersize readed"));
    for (TInt i = 0; i < layoutCount; i++)
    	{
    	iPhoneSuppLayout.AppendL(reader.ReadUint16());
    	}
    FLOG(_L("[IMAGEPRINTUI]>>> CImagePrintUi:CCapabilityManager, layout readed"));
    	
    CleanupStack::PopAndDestroy(id);
    CleanupStack::PopAndDestroy(&resource);
    FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUi:CCapabilityManager GetPhoneConfigL END "));
    }
   
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCapabilityManager::CheckIfNewCapabilityDiffer()
	{
	FLOG(_L("[IMAGEPRINTUI]>>> CCapabilityManager:: CheckIfNewCapabilityDiffer"));
	TBool compareValue = EFalse;
	
	
	for(TInt i = 0; i  < iCurrentLayouts.Count(); i++)
		{
		if(iCurrentLayouts.operator[](i) == iUserSelectedLayout )
			{
			compareValue = ETrue;
			}
		
		}
		
	if(!compareValue)
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUi:: user selected layout not supported"));
		iUserSelectedLayout = EDpsLayoutDefault;
		}
		
    compareValue = EFalse;		
	
	for(TInt i = 0; i  < iCurrentQualities.Count(); i++)
		{
		if(iCurrentQualities.operator[](i) == iUserSelectedQuality )
			{
			compareValue = ETrue;
			}
			 
		}
		
	if(!compareValue)
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUi:: user selected quality not supported"));
		iUserSelectedQuality = EDpsPrintQualityDefault;
		}
    	
	compareValue = EFalse;

    for(TInt i = 0; i  < iCurrentPaperSizes.Count(); i++)
		{
		if(iCurrentPaperSizes.operator[](i) == iUserSelectedPaperSize )
			{
			compareValue = ETrue;
			}
		} 

	if(!compareValue)
		{
		FLOG(_L("[IMAGEPRINTUI]<<< CImagePrintUi:: user selected paper size not supported"));
		iUserSelectedPaperSize = EDpsPaperSizeDefault;
		}
	FLOG(_L("[IMAGEPRINTUI]<<< CCapabilityManager:: CheckIfNewCapabilityDiffer"));	
	}

// End of file
