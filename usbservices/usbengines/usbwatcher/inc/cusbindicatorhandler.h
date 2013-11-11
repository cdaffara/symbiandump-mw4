/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for class CUsbIndicatorHandler
*
*/


#ifndef CUSBINDICATORHANDLER_H
#define CUSBINDICATORHANDLER_H

#include <bldvariant.hrh>
#include <usbstates.h>
#include <e32std.h>
#include <e32base.h>

class CHbIndicatorSymbian;
class CHbSymbianVariant;

/**
*  USB indicator handler class for device side.
*
*  This class shows USB indicator when device state changes to Configured, 
*  and hides USB indicator when device leaves Configured state. Suspended 
 * state does not change the indicator.
*
*  @since TB9.2
*/
NONSHARABLE_CLASS( CUsbIndicatorHandler ) : public CBase
    {
    friend class CtTUsbIndicatorHandler;

public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CUsbIndicatorHandler();

    /**
     * Two-phased constructor.
     */
    static CUsbIndicatorHandler* NewL();

    /**
     * Two-phased constructor.
     */
    static CUsbIndicatorHandler* NewLC();

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CUsbIndicatorHandler();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();


public:
        
	/**
     * Set USB indicator (On/Off, maybe more in future) according to 
     * the change of USB device state
     *
     * @param aStateOld previous USB device state
     * @param aStateNew current USB device state
     */
    void HandleDeviceStateChangeL( TUsbDeviceState aStateOld, 
            TUsbDeviceState aStateNew, TInt aPersonality );
            
private:
	/**
     * Show USB connected indicator (On/Off, maybe more in future)
     *
     * @since TB9.2
     * @param aActivate activate or deactivate the indicator
     * @param aPersonality the personality id
     */
    void ShowUsbConnectedIndicatorL( TBool aActivate, TInt aPersonality );

	/**
     * Show USB addressed state indicator (On/Off, maybe more in future)
     *
     * @since TB9.2
     * @param aActivate activate or deactivate the indicator
     */    
    void ShowUsbAddressedIndicator( TBool aActivate );
	
private:
    /**
     * The device state before suspend.
     * Used to filter out suspend from others;
     */
    TUsbDeviceState iDevStateB4Suspend;
    
    CHbIndicatorSymbian* iSymbianIndicator;
    
    CHbSymbianVariant* iIndicatorParameters;
    
    // remember whether indicators are active or not
    // simplifies little bit the state handling in HandleDeviceStateChange 
    TBool iAddressedIndicatorActive;
    TBool iConnectedIndicatorActive;
  
    };
    

#endif   // CUSBINDICATORHANDLER_H

// End of File
