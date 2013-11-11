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


#ifndef CEP0WRITER_H
#define CEP0WRITER_H

#include "cusbdevcon.h"

class CStateMachine;    // gets notifications on events happened on EP0

/**
 *  EP0 Writer
 *
 *  @lib usbdevcon.lib
 *  @since S60 v.5.0 
 */
 
class CEP0Writer : public CActive 
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
    static CEP0Writer* NewL(CStateMachine& aObserver, RDevUsbcClient& aLdd);
    
    /**
    * Destructor.
    *
    * @since S60 v.5.0
    */
    virtual ~CEP0Writer();
    
    /**
     * Set request to write EP0 data
     *
     * @since S60 v.5.0
     * @param aBuffer Data to write
     * @param aDataLength Max length of data to write
     */
    void WriteL(const RBuf8& aBuffer, TUint aDataLength);
    
private:
    
    /**
     * Default construction
     *
     * @since S60 v.5.0
     * @param aObserver Gets notification when Read request is completed
     * @param aLdd Link to services for writing EP0
     */
    CEP0Writer(CStateMachine& aObserver, RDevUsbcClient& aLdd);
    
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
     * Gets notifications
     * Not own.  
     */
    CStateMachine& iObserver;
    
    /**
     * LDD
     * Not own.  
     */
    RDevUsbcClient& iLdd;
        
    /**
     * Write data
     * 
     */
    RBuf8 iBuffer;
    };

#endif // CEP0WRITER_H
