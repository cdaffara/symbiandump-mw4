/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for capabilitymanager
*
*/


#ifndef CAPABILITYMANAGER_H
#define CAPABILITYMANAGER_H

#include <e32base.h>
#include <pictbridge.h>
#include <dpsoperation.h>
#include <dpsparam.h>
#include <badesca.h>


const TInt KPhoneCapabilityVersion = 0;
const TInt KRetryAsk = 2 * 100 * 1000; // 200 ms
const TInt KDefaultArray = 5;
_LIT(KPhoneCapability, "private\\1020e470\\phonecapability.rsc");

class CImagePrintUiAppUi;

/**
* CCapabilityManager is an active object that sends an asynchronous request 
* to retrieve capabilities
*/
class CCapabilityManager : public CActive
    {
        
public:

    /**
    * Factory method NewL
    * @param aOwner  The observer to be notified on the change. 
    * @param aEngine the pointer to the PictBridge engine
    * @return instance of capability manager        
    */
    static CCapabilityManager* NewL(CImagePrintUiAppUi* aAppUi, CDpsEngine* aEngine);


    /**
    * Destructor.
    */
    virtual ~CCapabilityManager();
        
       
    /**
     * Gives current qualities for doDpsRequest 
     * 
     */   
     TUint Quality();

	
	/**
     * Gives current paper size for doDpsRequest
     * 
     */  
     TUint PaperSize();
	
	/**
     * Gives current  paper layout for doDpsRequest
     * 
     */  
     TUint Layout();
    
	/**
     * Gives current print setting to UI
     * 
     */  
	 TUint CurrentPrintSettings(TInt aDpsArg);
	
	/**
     * All qualities asked by UI engine
     * @param aReason the answer for request complete to UI capability query 
     */  
	void QualitiesAsked(TInt aReason);


    /**
     * Return certain type list of Capabilities
     * @param aCapability the Capabilities type requested
     */
    RArray<TUint>& GetCapabilitiesL(TInt aCapability);
    
    /**
    * Stores value choosed by user
    */
    void SetValue(TInt aCapability, TUint aValue);
    
    /**
    * Ask capabilities when needed
    *
    */
    void AskCapabilitiesL(TBool aChanged);
    
    
     /**
    * Ask layout when user has selected certain papersize
    *
    */
    void AskLayoutForPaperSizeL(TUint aPaperSize);
    
    
    
    /**
    * Check if capabilities differ when user start print
    *
    */ 
    void CheckIfNewCapabilityDiffer();
   

private: //from CActive
    /**
    *	Implements CActive
    *   If this is not a one-shot CR watcher, the listening is restarted.
    *	@param none
    *	@return none
    */
    void RunL();
    
    /**
    *	Implements CActive
    *	@param aError the error returned
    *	@return error
    */
    TInt RunError(TInt aError);

    /**
    *	Implements CActive
    *	@param none
    *	@return none
    */
    void DoCancel();
    
    
    /**
     * Stores current qualities 
     * 
     */   
    void StoreQualitiesL(RArray<TUint>& aCurrQualities);
    
    /**
     * Stores current paper size 
     * 
     */   
    void StorePaperSizeL(RArray<TUint>& aSuppPaperSize);
    
    /**
     * Stores current first paper layout 
     * 
     */    	
     void StoreLayouts(RArray<TUint>& aSuppLayouts);
    
    /**
     * Ask quality from printer
     */  
    void AskQualityL();
    
    /**
     * Ask paper size from printer 
     */  
    void AskPaperSizeL();
    
    /**
     *  Ask layout from printer
     */  
    void AskLayoutL();
    

private: //own methods

    enum TCapabilityType
    	{
		EQualities,
		EPaperSize,
		ELayouts
  		};    

   /**
    *   Second phase constructor. Operations which might leave should 
    *   be called here
    */ 
    void ConstructL();

       
    /**
    * Handles capabilities after RunL
    *
    */
    void HandleCapabilitiesL(TCapabilityType aType);
    
    /**
    * Read phone supported capabilities
    *
    */
    void GetPhoneConfigL();

    /**
    * C++ default constructor
    * @param aAppUi the pointer to the AppUi instance 
    * @param aEngine the pointer to the PictBridge engine
    */
    CCapabilityManager( CImagePrintUiAppUi* aAppUi, 
                        CDpsEngine* aEngine);
    
    
    
    /**
    * Handles result of asking layout for certain papersize
    *
    */ 
    void HandleLayoutForPaperSize();
      
     
private: //data
    
    CImagePrintUiAppUi* iAppUi;
    
    /**
     * Instance to PictBridge Engine to get capabilities
     * Not own
     */
    CDpsEngine* iDpsEngine; 
    
    TBool iCapabilityAskFailed;
    TBool iCapabilityNotChanged;
    TBool iIsCapabilityEmpty;
    TBool iLayoutForPaperSize;
    
    TCapabilityType iCapabilityType;
    
    // types for DoDpsRequest
    TDpsGetCapability      iCap;
        
    
    //Arrays for storing and maintaining capabilities data after 
    //comparing printer and phone supported capabilities
    RArray<TUint> iCurrentQualities; 
    RArray<TUint> iCurrentPaperSizes;
    RArray<TUint> iCurrentLayouts;
    
        
    TUint iCurrentQuality;
    TUint iCurrentPaperSize;
    TUint iCurrentLayout;
    
    TUint iUserSelectedQuality;
    TUint iUserSelectedPaperSize;
    TUint iUserSelectedLayout;
    
    //phone supported capabilities
    RArray<TUint>  iPhoneSuppLayout;   
    RArray<TUint>  iPhoneSuppPaperSize;   
    RArray<TUint>  iPhoneSuppQuality;  
    
    RArray<TUint> iReturnArray;  
     
    };


#endif      // CAPABILITYMANAGER_H

// End of File
