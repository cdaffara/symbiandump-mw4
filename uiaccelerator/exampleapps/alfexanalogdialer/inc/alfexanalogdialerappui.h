/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Application UI class for Analog Dialer example application, which
*                implements rotating dial pad.
*
*/



#ifndef ANALOGDIALER_APPLICATION_H
#define ANALOGDIALER_APPLICATION_H

#include <aknappui.h>
#include <alf/alfenv.h>
#include <alf/alfdisplay.h>

// Forward reference
//class CAlfExAnalogDialerControl

class CAlfDeckLayout;
/**
 *  Application UI class for Analog Dialer
 *
 *  @since S60 5.0
 */
class CAlfExAnalogDialerAppUi : public CAknAppUi
    {
public:

    
    /* Constructors and destructor. */

    /**
     * Constructor of CAlfExAnalogDialerAppUi
     * This needs to be public due to the way the framework constructs the AppUi 
     *
     * @since S60 5.0
     */
    CAlfExAnalogDialerAppUi();

    /**
     * Second phase constructor of CAlfExAnalogDialerAppUi
     * This needs to be public due to the way the framework constructs the AppUi 
     *
     * @since S60 5.0
     */
    void ConstructL();


    /**
     * Destructor of CAlfExAnalogDialerAppUi
     * Destroy the object and release all memory objects
     *
     * @since S60 5.0
     */
    virtual ~CAlfExAnalogDialerAppUi();


    /* Methods. */

// from base class CAknAppUi

    /**
     * From CAknAppUi
     * Receives command from framework
     *
     * @param aId system or application command Id 
     */
    void HandleCommandL(TInt aCommand);

    /**
     * From CAknAppUi
     * Handles a change to switch in display orientation.
     * This function calls CAknAppUi::HandleResourceChangeL().
     *
     * @param aType The type of resources that have changed.
     */
    void HandleResourceChangeL(TInt aType);
    
    /** 
     * From CAknAppUi
     * Handle system events. Release resources during FG->BG
     * transition, and restore resource during BG->FG transitions.
     * This is not mandatory, but reduces memory consumption of the 
     * application when it is in the background.
     * @param aEvent event from framework
     * @param aDestination a Symbian OS CCoeControl, for which event occurre
     */		
    void HandleWsEventL(const TWsEvent& aEvent,
	                    CCoeControl*    aDestination);


private:

    /**
     * Pointer to ALF Environment
     * Own.  
     */
    CAlfEnv* iEnv;
    
    };


#endif // ANALOGDIALER_APPUI_H

