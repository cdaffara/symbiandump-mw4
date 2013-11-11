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
* Description:  Header file for connectionmanager
*
*/


#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <e32base.h>

class CDpsEngine; 
class CNotes;
class CImagePrintUiAppUi;

/**
* CConnectionManager is an active object that sends an asynchronous request 
* to know about the connection status.
*/
class CConnectionManager : public CActive 
    {
        
public:

    /**
     * Factory method NewL
     * @param aAppUi the pointer to the AppUi instance 
     * @param aEngine    Instance to DpsEngine
     * @param aNotes     Instance to Notes class
     * @return           The new object. 
     */
    static CConnectionManager* NewL(CImagePrintUiAppUi* aAppUi, 
    						        CDpsEngine* aEngine, CNotes* aNotes);

   /**
    * Destructor.
    */
   virtual ~CConnectionManager();

   /**
    * Starts search printer
    */
   void StartSearchPrinter();

private: 

//from CActive

    /**
     *	Implements CActive
     *	@param none
     *	@return none
     */
    void RunL();
    
    /**
     *  Implements CActive
     *  @param aError the error returned
     *  @return error
     */
    TInt RunError(TInt aError);

    /**
     *	Implements CActive
     *	@param none
     *	@return none
     */
    void DoCancel();

private: 

    /**
     * Starts listening for notifications. If already listening, nothing 
     * happens.
     * @param none
     * @return none
     */
    void StartListening();

    /**
     * C++ default constructor
     * @param aAppUi the pointer to the AppUi instance  
     * @param aEngine   Instance of DpsEngine
     * @param aNotes     Instance to Notes class
     */
    CConnectionManager( CImagePrintUiAppUi* aAppUi, 
                        CDpsEngine* aEngine, CNotes* aNotes);
     
private: //data


    CImagePrintUiAppUi* iAppUi;

    /**
     * Instance to DpsEngine to get connection status
     * Not own
     */
    CDpsEngine* iDpsEngine; 

    TBool iActiveNote;
    TBool iSetPrintCall;
    CNotes* iNotes;
    TInt iPreviousStatus;
    };


#endif      // CONNECTIONMANAGER_H

// End of File
