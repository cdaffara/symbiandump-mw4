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
* Description:  Header file for imageprintuiappui
*
*/


#ifndef C_CIMAGEPRINTUIAPPUI_H
#define C_CIMAGEPRINTUIAPPUI_H

#include <aknViewAppUi.h>
#include <AknProgressDialog.h>
#include <AknWaitDialog.h>
#include <badesca.h>
#include "connectionmanager.h"
#include "capabilitymanager.h"
#include "eventmanager.h"
#include "requestmanager.h"

#include <AiwServiceIfMenu.h>
#include <GenericParamConsumer.h>
#include <eikenv.h>

#include <pictbridge.h>

class CNotes;
class CNoteTimer;
class CAknNavigationDecorator; 
class DRMCommon;
 
 
const TInt KNumberOfUnsupportedFiles = 1; 
const TInt KPrintFinishValue = 100; 
const TInt KMaxPrinterName = 64; 
const TInt KRetryInterval = 1 * 1000 * 1000; // 1 s
const TInt KRetryClose = 1 * 1000 * 1000; // 1 s
const TInt KImageDefaultArray = 2;
const TInt KRemoveValue = 5;
const TInt KImageNumber = 1;

// CONSTANTS
_LIT( KConfigInfoFile, "config.DPS");
_LIT( KParamFile, "system\\data\\T_AIW_PRINT.DAT");
_LIT( KUnSuppFile, "system\\data\\T_AIW_UNSUPP.DAT");
const TInt KResource = 40;
const TInt KDriver = 3;
const TInt KConfigLength = 16;

 
/**
 *  S60 application UI class
 */
class CImagePrintUiAppUi : public CAknViewAppUi                                           
    {

public:

   /**
    *   Destructor
    */	 
    virtual ~CImagePrintUiAppUi();


   /**
    *   Second phase constructor. Operations which might leave should 
    *   be called here
    */ 
    void ConstructL();
    
    /**
     * Returns instance of Capability Manager. 
     * @return CCapabilityManager.
     */
    CCapabilityManager* CapabilityManager();
    
    
    /**
     * Returns instance of Notes
     * @return CNotes.
     */
    CNotes* Notes();
    

public:	// Methods derived from CAknViewAppUi base class

    void HandleCommandL( TInt aCommand );

    
public: //own methods


    /**
     * Starts printing process when user select print
     * @return none
     */
    void StartPrinting();
    
    /**
     * Cancel print when user select cancel
     * @return none
     */
    void CancelPrinting();
    
    /**
     * Returns print state to other instances
     * @return print state
     */
    TBool PrintActive();
	
	/**
     * Provide array of images to be printed
     * @return CDesCArrayFlat
     */
    CDesCArrayFlat* ImagesToPrint();
    
    /**
     * Tells if cable is connected or not
     * @return TBool .
     */
    TBool IsCableConnected();
    
    /**
     * Shows note if there is unsupported files
     * @return none.
     */
    void ShowNoteL();
    
    /**
     * Closes application after showing error  note
     * @return none.
     */
    void CloseAfterNoteL();
     

    /**
     * Request Manager notifies result of DoDps Request 
     * @return none.
     */ 
    void NotifyRequestL( TInt aNotifyType);
    
    /**
     * Request Manager informs about number of images to be printed
     * @return none.
     */ 
    void NumberOfImages(TInt aNumberOfImages);
    

    /**
     * Event Manager notifies DpsEvent 
     * @return none.
     */  
    void NotifyEventL(TInt aEventNotifyType);
    
    /**
     * Event Manager notifies about Print Progress 
     * @return none.
     */ 
    void NotifyPrintProgress(TInt aNumberOfImages, TInt aProgress);
    
    /**
     * Event Manager notifies about Print Status
     * @return none.
     */ 
    void NotifyPrintStatus(TInt aStatus);


    /**
     * Connection Manager closes application if cable is disconnect
     * or personaluty changes different as PTP
     * @return none.
     */ 
    void CloseApplication(TBool aDisconnect);

    /**
     * Connection Manager informs PTP printer is found
     * @return none.
     */ 
    void CableConnectedL(); 
    
    /**
     * Connection Manager informs about cable disconnection
     * @return none.
     */ 
    void CableDisConnected();


    /**
     * Capability Manager informs about capabilities has
     * retrieved from printer
     * @return none.
     */ 
    void CapabilitiesReady(); 
    
    /**
     * Capability Manager informs about error when retrieving
     * capabilities from printer
     * @return none.
     */ 
    void NotifyError();
    
    /**
     * Handles situation after timer timeout
     * @return none.
     */ 
    void HandleTimeOutL();
    
    /**
     * Handles requesting DPS request again
     * @return none.
     */ 
    void HandleRequestTime(TInt aTime);
    
    /**
     * Handles requsting capabilities again
     * @return none.
     */ 
	void HandleCapabilityTime(TInt aTime);
	
	
	/**
     * Handles situation after RunError in timer
     * @return none.
     */ 
	void HandleTimeErrorL(TInt aError);
    

private: //own methods


    /**
     * Set printer and vendor info to navipane
     * @return none.
     */
    void SetNavipaneTextL();
    
    /**
     * Read images to be printed to array
     * @return none.
     */
    void LoadImagesL();
    
    /**
     * Reads number of unsupported files
     * @return none.
     */
    void ReadNumberOfUnSuppL();
    
public:

	/**
     * Show all notes
     * Own this pointer
     */
    CNotes*  iNotes;   
    

private: // data


    /** A singleton DPS engine object */
    CDpsEngine* iEngine; 

    /**
     * Connection manager to check connection status
     * Owns this pointer 
     */
    CConnectionManager* iConnectionManager;

    /**
     * Retrieve, restore and change capabilities if needed 
     * Own this pointer
     */
    CCapabilityManager* iCapabilityManager;
    
        
    /**
     * Needed when creating navipane text
     * Own this pointer
     */
    CAknNavigationDecorator* iNaviDecorator; 
    
    
    /**
     * Performs DPS request towards PictBridge engine
     * Own this pointer
     */
    CRequestManager* iRequestManager;
    
    
    /**
     * Notifies Device- and JobEvent coming from PictBridge engine
     * Own this pointer
     */
    CEventManager* iEventManager;
    
    
    // images to be printed
    CDesCArrayFlat* iImageFiles;
    
    // timer for unsupported file note
    CNoteTimer* iNoteTimer;
    
    HBufC* iCongFileName;
    
    TBool iPrintActive;
    TBool iCableConnected;
    TBool iActiveNote;
    TBool iCapabilityChanged;
    TBool iUnsupportedFiles;
    TBool iNoteShowed;
    TInt  iNumberOfImages;
    TInt  iNumberOfUnSuppFiles;
    TBool iErrorState;
    TBool iStartJobOK;
    TBool iPaperEmpty;
    TBool iNoteTimeOut;
    TBool iCloseTimeOut;
    TBool iCapabilityTimeOut;
    TBool iRequestTimeOut;
    
    TFileName     iLogFilePath; 

    };

#endif // C_CIMAGEPRINTUIAPPUI_H
