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
* Description:  Header file for requestmanager  
*
*/


#ifndef C_REQUESTMANAGER_H
#define C_REQUESTMANAGER_H

#include <dpsoperation.h>
#include <dpsparam.h>
#include <badesca.h>

class CDpsEngine;
class CCapabilityManager;
class CImagePrintUiAppUi;

const TInt KDefaultSize = 64;


/**
* Class for requesting DPS request
*/
class CRequestManager : public CActive
    {

public:

     enum TRequestType
    	{
    	EConfigurePrintService = 0,
    	EGetCapability,
    	EGetJobStatus,
    	EGetDeviceStatus,
    	EStartJob,
    	EAbortJob,
    	EContinueJob,
    	ESetPrintMode
    	}; 
    	
    enum TNotifyType
    	{
    	EConfigurePrintServiceOK = 1,
    	EConfigurePrintServiceNOK,
    	EGetCapabilityOK,
    	EGetJobStatusOK,
    	EGetJobStatusNOK,
    	EGetDeviceStatusOK,
    	EGetDeviceStatusNOK,
    	EStartJobOK,
    	EStartJobNOK,
    	EAbortJobOK,
    	EAbortJobNOK,
    	EContinueJobOK,
    	EPicturesNotFound
    	}; 	

    /**
     * Two-phase constructors
     * @param aDpsEngine the pointer to the PictBridge engine
     * @param aCapabilityManager the pointer to the Capability Manager
     * @param aAppUi the pointer to the AppUi class
     * @return  Initialized instance of CRequestManager
     */
    static CRequestManager* NewL( CDpsEngine* aDpsEngine, 
                                  CCapabilityManager* aCapabilityManager, CImagePrintUiAppUi* aAppUi);
    
    /**
     * Two-phase constructors
     * @param aDpsEngine the pointer to the PictBridge engine
     * @param aCapabilityManager the pointer to the Capability Manager
     * @param aAppUi the pointer to the AppUi class
     * @return  Initialized instance of CRequestManager
     */
    static CRequestManager* NewLC( CDpsEngine* aDpsEngine, 
                                   CCapabilityManager* aCapabilityManager, CImagePrintUiAppUi* aAppUi);
    
    
    /**
     * Default destructor.
     * @since S60 v3.2
     */	
    virtual ~CRequestManager();
    
    
    /**
     * Chooses DoDpsRequest type 
     * @param aType type of request
     * @return none
     */
   	 void ChooseDpsRequestTypeL( TRequestType aType );
   	 
   	/**
     * Give printer and vendor names to UI 
     * @param aVendorName the vendor name
     * @param aProductName the product name
     * @return none
     */
   	 void GetPrinterName(TDes8& aVendorName, TDes8& aProductName);
   	 
    /**
     * Issues again DoDpsRequest if iStatus indicates timeout or PictBridge is in use 
     */
     void ReIssueDoDpsRequestL();
    
protected:
    
   /**
    *   @see CActive
    */
    void RunL();
    
   /**
    *   @see CActive
    */ 
    void DoCancel();
    
   /**
    *   @see CActive
    */
    TInt RunError(TInt aError);
    
   /**
    * Externalixe value when save those into config file
    */
    void ExternalizeL(RWriteStream& aStream) const;


private:

   /**
    *  Default constructor
    * @param aDpsEngine the pointer to the PictBridge engine
    * @param aCapabilityManager the pointer to the Capability Manager
    * @param aAppUi the pointer to the AppUi class
    */
    CRequestManager( CDpsEngine* aDpsEngine, 
                     CCapabilityManager* aCapabilityManager, CImagePrintUiAppUi* aAppUi);
    

    /** 
     * 2nd phase construction 
     */
    void ConstructL();
    	
	/**
     * Handles return situation after requesting DPS request GetDeviceStatus  
     */
    void HandleDeviceStatusL();
    
    /**
     * Handles return situation after requesting DPS request AbortJob
     */
    void HandleAbortL();
    
    
    /**
     * Handles return situation after requesting DPS request StartJob 
     */
    void HandleStartJobL();
    
    
    /**
     * Handles return situation after requesting DPS request GetJobStatus  
     */

     void HandleGetJobStatusL();
     
     /**
     * Handles return situation after requesting DPS request GetConfigure
     */ 
     void HandleConfigureL();
     
     /**
     * Saves configure info to file
     */ 
     void SaveConfigInfoL();
     
     
    /**
     * Read configure info from file
     */ 
     void ReadConfigInfoL();
     
    /**
     * Performs start job request
     */ 
     void ExecuteStartJobRequestL();
      
     
    
private: 
    
    // owned by AppUi
    CDpsEngine* iDpsEngine;
    CCapabilityManager* iCapabilityManager;
    CImagePrintUiAppUi* iAppUi;
    
    // types for DoDpsRequest
    TDpsGetCapability      iCap;
    TDpsStartJob           iStart;
    TDpsAbortJob           iAbort;
    TDpsContinueJob        iContinue;
    TDpsGetPrinterStatus   iPrinter;
    TDpsGetJobStatus       iJob;
    TDpsConfigPrintService iConfig;
        
    TRequestType   iRequestType;
    
    TInt iNumberOfImages;
    
    TBool iJobFinished;
    TBool iFileExist;
    
    CDesCArrayFlat* iImageArrayFlat;
    
    TBuf8<KDefaultSize> iVendorName;
    TBuf8<KDefaultSize> iProductName;
      
    HBufC* iCongFileName;
     
    };


#endif // C_REQUESTMANAGER_H
