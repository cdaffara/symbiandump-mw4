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
* Description:  Header file for eventmanager
*
*/



#ifndef C_EVENTMANAGER_H
#define C_EVENTMANAGER_H

#include <dpsoperation.h>
#include <dpsparam.h>
#include <eikenv.h>

class CDpsEngine;
class CImagePrintUiAppUi;

/**
*  Class for requesting DPS event
*/
class CEventManager : public CActive
    {
    
public:

    enum TEventNotifyType
    	{
    	ENewJobOK = 1,
    	ENewJobNOK,
    	EInkEmpty,
    	ECapabilityChange,
    	EWarning,
    	EErrorState,
    	ENotErrorState,
    	ESeriousError,
		EShowError
    	};     

public:

    /**
     * Two-phase constructors
     * @since S60 v3.2 
     * @param aAppUi the pointer to the AppUi instance  
     * @param aEngine   Instance of DpsEngine
     * @return  Initialiazed instance of EventManager 
     */
    static CEventManager* NewL(CImagePrintUiAppUi* aAppUi, CDpsEngine* aDpsEngine);

    /**
     *  Destructor.
     */	
    virtual ~CEventManager();
    

    
private:

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
    void RunError();


private:

    
    /**
     * C++ default constructor
     * @param aAppUi the pointer to the AppUi instance  
     * @param aEngine   Instance of DpsEngine
     */
    CEventManager(CImagePrintUiAppUi* aAppUi, CDpsEngine* aDpsEngine);


     /**
     * Handles return status  from PictBridge Engine.  
     * 
     */
	void HandleReturnStatusL();
	
	
    /**
     * Start listening events from PictBridge Engine.  
     * 
     */
	void StartListening();
	
	/**
     * Handles error status  
     * 
     */
	void HandleErrorStatusL(TInt aStatus);
	
	/**
     * Handles job status  
     * 
     */
	void HandleJobStatusL(TInt aStatus);
	
	/**
     * Handles hardware error.  
     * 
     */
    void HandleHardwareErrorL(TInt aError);
    
    
    /**
     * Handles ink error.  
     * 
     */
    void HandleInkErrorL(TInt aError);
    
    /**
     * Handles paper error.  
     * 
     */
    void HandlePaperErrorL(TInt aError);
    
    /**
     * Handles file error.  
     * 
     */
    void HandleFileErrorL(TInt aError);
    
private: 
    
    CImagePrintUiAppUi* iAppUi;
    CDpsEngine*      iDpsEngine;
    TDpsEvents       iEventRequest;
    TBool            iErrorState;
   
    };


#endif // C_EVENTMANAGER_H
