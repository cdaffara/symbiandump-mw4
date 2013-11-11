/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for CUsbActiveBasicHandler
*
*/


#ifndef C_CUSBACTIVEBASICHANDLER_H
#define C_CUSBACTIVEBASICHANDLER_H

#include "cusbpersonalityplugin.h"

class RUsb;

/**
*  Class to handle basic personalities.
*
*  @lib euser.lib
*  @since Series 60 3.0
*/
class CUsbActiveBasicHandler : public CUsbPersonalityPlugin
    {
public:  // Constructors and destructor
        
    /**
     * Two-phased constructor.
     * @param aPersonalityParams Reference to the container class.
     * @return pointer to created object
     */
    static CUsbActiveBasicHandler* NewL(TUsbPersonalityParams& aPersonalityParams);
        
    /**
     * Destructor.
     */
    virtual ~CUsbActiveBasicHandler();

public: // from base class CUsbPersonality        
        
    /**
     * From CUsbPersonality
     * Called when personality will be changed. In this personality 
     * no actions is taken.
     *
     * @since Series 60 3.0
     * @param aStatus Status of the ended operation.
     */        
    virtual void ConfirmPersonalityUnload(TRequestStatus& aStatus);

    /**
     * From CUsbPersonality
     * Called by personality handler when personality start needs to be 
     * prepared. Nothing is done in this personality.
     *
     * @since Series 60 3.0
     * @param aStatus Status of the ended operation.
     */
    virtual void PreparePersonalityStart(TRequestStatus& aStatus);

    /**
     * From CUsbPersonality
     * Called by personality handler when personality start needs to be 
     * finished. For this personality: Show infonote to user.
     *
     * @since Series 60 3.0
     * @param aStatus Status of the ended operation.
     */
    virtual void FinishPersonalityStart(TRequestStatus& aStatus);
                
    /**
     * From CUsbPersonality
     * Called by personality handler when personality stop needs to be 
     * prepared. For this personality: Do nothing.
     *
     * @since Series 60 3.0
     * @param aStatus Status of the ended operation.
     */
    virtual void PreparePersonalityStop(TRequestStatus& aStatus);

    /**
     * From CUsbPersonality
     * Called by personality handler when personality stop needs to be 
     * finished. For this personality: Do nothing.
     *
     * @since Series 60 3.0
     * @param aStatus Status of the ended operation.
     */
    virtual void FinishPersonalityStop(TRequestStatus& aStatus);
        
    /**
     * From CUsbPersonality
     * State change notify
     *
     * @since Series 60 3.0
     * @param aState state of the device
     */
    virtual void StateChangeNotify( TUsbDeviceState aState );

public: // from base class CActive

    /**
     * From CActive.
     * Left empty in this implementation.
     *
     * @since Series 60 3.0
     */
    void RunL();

    /**
     * From CActive.
     * Left empty in this implementation.
     *
     * @since Series 60 3.0
     * @param aError the error returned
     * @return error
     */
    TInt RunError( TInt /*aError*/ );

    /**
     * From CActive
     * Left empty in this implementation.
     *
     * @since Series 60 3.0        
     */
    void DoCancel();

protected:

    /**
     * C++ default constructor (no implementation provided).
     */
    CUsbActiveBasicHandler(TUsbPersonalityParams& aPersonalityParams);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    // Prohibit copy constructor when not deriving from CBase.
    CUsbActiveBasicHandler( const CUsbActiveBasicHandler& );
    // Prohibit assigment operator when not deriving from CBase.
    CUsbActiveBasicHandler& operator=( const CUsbActiveBasicHandler& );

private:    // Data
    /**
     * Request to completed later
     */
    TRequestStatus* iRequestStatus;
    };

#endif   // USBACTIVECDCHANDLER_H

// End of File
