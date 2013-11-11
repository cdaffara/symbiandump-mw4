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
* Contributors:
*
* Description:  Header file for class TUsbIndicatorHandler
*
*/


#ifndef TUSBINDICATORHANDLER_H
#define TUSBINDICATORHANDLER_H

#include <bldvariant.hrh>
#include <usbstates.h>


/**
*  USB indicator handler class for device side.
*
*  This class shows USB indicator when device state changes to Configured, 
*  and hides USB indicator when device leaves Configured state. Suspended 
 * state does not change the indicator.
*
*  @since TB9.2
*/
NONSHARABLE_CLASS( TUsbIndicatorHandler )
    {

public:
    /**
     * Constructor
     */
    TUsbIndicatorHandler();
    
	/**
     * Set USB indicator (On/Off, maybe more in future) according to 
     * the change of USB device state
     *
     * @param aStateOld previous USB device state
     * @param aStateNew current USB device state
     */
    void HandleDeviceStateChange( TUsbDeviceState aStateOld, 
            TUsbDeviceState aStateNew );
            
private:
	/**
     * Show USB indicator (On/Off, maybe more in future)
     *
     * @since TB9.2
     * @param aUsbIndicatorState the state of the indicator, 0: OFF, 1: ON
     */
    void ShowUsbIndicator( TInt aUsbIndicatorState );
	
private:
    /**
     * The device state before suspend.
     * Used to filter out suspend from others;
     */
    TUsbDeviceState iDevStateB4Suspend;
    
    /////////////////////////////////////////////////////////////////////////
    // Test code
    friend class CtTUsbIndicatorHandler;
    TBool iUsbIndicator; 
    // 
    /////////////////////////////////////////////////////////////////////////
     
    };
    

#endif   // TUSBINDICATORHANDLER_H

// End of File
