/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Name        : alfclientwindow.h
* Contributors:
* Description :  AlfClientWindow class, to post drawing commands to server 
*                side canvas
*
*/

#ifndef ALFCLIENTWINDOW_H_
#define ALFCLIENTWINDOW_H_
class CAlfEnv;

 /**
  * To mention the status of command buffer to be posted
  */
enum TAlfCommandBufferStatus 
   {
       EComplete,  //Clear the canvas command buffer and post the complete buffer
       EPartial,   //Partial buffer, dont clear canvas command buffer
       ELastPart   //Last part of buffer
   }; 


/**
 * CAlfClientWindow class 
 *
 * Represents the client side Alf window, corrosponding to a symbian side RWindow
 * Client can create own instance of this class and hold the responsibility of cleaning it.
 * 
 * Posting the drawing commands through this class can hardware accelerate the commands,
 * bypassing the windowserver
 * 
 * Currently functionality of this class is limited to posting the commands.
 * Usage: 
 * @code
 *
 * iAlfEnv = CAlfEnv::NewL();
 * //Create Client window
 * CAlfClientWindow* alfWindow = CAlfClientWindow::NewL(*iEnv, *rWindow);  
 * 
 * //Post drawing commands
 * alfWindow->PostCommandsL( command, EComplete );
 *
 * //destruction
 * delete alfWindow;
 * delete iAlfEnv;
 * @endcode 
 *
 */

class CAlfClientWindow: public CBase
    {
public:
    
    /**
     * Constructor
     *
     * @param   aEnv  Alf Environment
     * @param   aWindow  RWindow for which the AlfClientwindow is to
     *                   to be created
     */

    IMPORT_C static CAlfClientWindow* NewL( CAlfEnv& aEnv, RWindow& aWindow );
    
    /**
     * Constructor
     */
    static CAlfClientWindow* NewLC( CAlfEnv& aEnv, RWindow& aWindow );   

    /**
     * Destructor
     */
    IMPORT_C virtual ~CAlfClientWindow(); 
    
    /*
     * PostCommandsL
     * 
     *
     * @param   aCommandBuffer  Serialized commands for HuiCanvasVisual
     * @param   aStatus  Buffer status
     *          @see TAlfCommandBufferStatus
     */
    IMPORT_C void PostCommandsL( TDes8& aCommandBuffer, TAlfCommandBufferStatus aStatus );

    
private:  

   /**
    * 2nd phase constructor
    */
    void ConstructL( CAlfEnv& aEnv, RWindow& aWindow );
	
    CAlfClientWindow();
	
    CAlfEnv* iEnv; //not owned
    RWindow* iRWindow; //not owned
    };

#endif /* ALFCLIENTWINDOW_H_ */
