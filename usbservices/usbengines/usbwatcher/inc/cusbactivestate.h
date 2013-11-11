/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for class CUsbActiveState
*
*/


#ifndef CUSBACTIVESTATE_H
#define CUSBACTIVESTATE_H

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATION
class CUsbWatcher;

// CLASS DECLARATION

/**
*  Class to handle USB state changes.
*
*  @lib euser.lib usbman.lib
*  @since Series 60 2.?
*/
NONSHARABLE_CLASS( CUsbActiveState ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aUsbMan Reference to usb manager.
        * @param aParent Reference to UsbMediaWatcher.
        * @return Pointer to created object.
        */
        static CUsbActiveState* NewL( RUsb& aUsbMan, CUsbWatcher& aOwner );
        
        /**
        * Destructor.
        */
        virtual ~CUsbActiveState();

    public: // from base class CActive

        /**
        * From CActive.
        * This method is called when device state has changed.
        * New request is issued and device state is given to owner class.
        *
        * @since Series 60 2.?
        */
        void RunL();

        /**
        * From CActive
        * If there is outstanding request pending when Cancel() is called then
        * this method must cancel request.
        *
        * @since Series 60 2.?
        */
        void DoCancel();

    public: // new methods

        /**
        * Get the current device state
        * @return the current device state
        *
        * @since Series 60 3.2
        */
        TUsbDeviceState CurrentState();

        /**
        * Get previous device state
        * @return previous device state
        *
        * @since Series 60 5.1
        */
        TUsbDeviceState PreviousState();

    
    private:

        /**
        * Constructor is prohibited.
        * @param aUsbMan Reference to Usb manager.
        * @param aParent Reference to CUsbMediaWatcher.
        */
        CUsbActiveState( RUsb& aUsbMan, CUsbWatcher& aOwner );

        /**
        * C++ default constructor (no implementation provided).
        */
        CUsbActiveState();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        // Prohibit copy constructor when not deriving from CBase.
        CUsbActiveState( const CUsbActiveState& );
        // Prohibit assigment operator when not deriving from CBase.
        CUsbActiveState& operator=( const CUsbActiveState& );

    private: // Data
        /**
         * Handle to Usb Manager
         */
        RUsb iUsbMan;
        
        /**
         * Refernce to owner class
         */
        CUsbWatcher& iOwner;

        /**
         * Current device state
         */
        TUsbDeviceState iCurrentState;

        /**
         * Previous device state
         */
        TUsbDeviceState iPreviousState;        

    };

#endif   // CUSBACTIVESTATE_H

// End of File
