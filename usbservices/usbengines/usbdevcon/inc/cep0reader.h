/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  State, while waiting for requests
*
*/


#ifndef CEP0READER_H
#define CEP0READER_H

#include "cusbdevcon.h"

class CStateMachine;    // gets notifications on events happened on EP0

/**
 *  EP0 Reader
 *
 *  @lib usbdevcon.lib
 *  @since S60 v.5.0 
 */
 
class CEP0Reader : public CActive 
    {

public:

    /**
     * Two-phased constructor.
     *
     * @since S60 v.5.0
     * @param aObserver Gets notification when Read request is completed
     * @param aLdd Link to services for reading EP0
     * @return Constructed instance
     */
    static CEP0Reader* NewL(CStateMachine& aObserver, RDevUsbcClient& aLdd);
    
    /**
    * Destructor.
    *
    * @since S60 v.5.0
    */
    virtual ~CEP0Reader();
    
    /**
     * Set request to read setup packet
     *
     * @since S60 v.5.0
     */
    void ReadSetupPacketL();
    
    /**
     * Set request to read EP0 data
     *
     * @since S60 v.5.0
     * @param aDataLength Max length of data to read
     */
    void ReadL(TUint aDataLength);
    
private:
    
    /**
     * Default construction
     *
     * @since S60 v.5.0
     * @param aObserver Gets notification when Read request is completed
     * @param aLdd Link to services for reading EP0
     */
    CEP0Reader(CStateMachine& aObserver, RDevUsbcClient& aLdd);
    
    /**
     * Two-phased construction.
     *
     */
    void ConstructL();
    
    // from CActive
    
    /**
     * From CActive.
     *
     */
    void RunL();
    
    /**
     * From CActive.
     *
     */ 
    void DoCancel();
    
    /**
     * From CActive.
     *
     */ 
     TInt RunError( TInt /*aError*/ );

private: // data
    
    /**
     * LDD Logical Device Driver
     * Not own.  
     */
    RDevUsbcClient& iLdd;
    
    /**
     * State machine gets read data
     * Not own.  
     */
    CStateMachine& iObserver;
    
    /**
     * Read buffer
     */
    RBuf8 iBuffer;
        
    };

#endif // CEP0READER_H
