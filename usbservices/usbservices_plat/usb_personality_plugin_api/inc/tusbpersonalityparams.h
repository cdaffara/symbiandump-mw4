/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container class for USB personalities
*
*/


#ifndef TUSBPERSONALITYPARAMS_H
#define TUSBPERSONALITYPARAMS_H

class RUsb;
class CUsbPersonalityNotifier;

/**
 *  Container class for USB personalities
 *
 *  Personality id, handle for Usb Manager and personality notifier
 *  is given with this class to USB personalities.
 *
 *  @lib euser.lib, usbman.lib
 *  @since S60 3.2
 */
class TUsbPersonalityParams
    {
public:    
    IMPORT_C TUsbPersonalityParams(RUsb& aUsbMan, CUsbPersonalityNotifier& aPersonalityNotifier);
    IMPORT_C ~TUsbPersonalityParams();
    
    /**
     * Method to get handle to USB Manager
     *
     * @since S60 3.2
     * @return Handle to USB Manager
     */
    IMPORT_C RUsb& UsbMan() const;
    
    /**
     * Method to get handle to personality notifier
     *
     * @since S60 3.2
     * @return Handle to personality notifier
     */    
    IMPORT_C CUsbPersonalityNotifier& PersonalityNotifier() const;
    
    /**
     * Set personality id.
     *
     * @since S60 3.2
     * @param Personality id
     */    
    IMPORT_C void SetPersonalityId(TInt aPersonalityId);
    
    /**
     * Method to get current personality id
     *
     * @since S60 3.2
     * @return Personality id
     */        
    IMPORT_C TInt PersonalityId() const;
    
private:
    /**
     * Handle to USB Manager
     */
    RUsb& iUsbMan;

    /**
     * Handle to personality notifier
     */    
    CUsbPersonalityNotifier& iPersonalityNotifier;

    /**
     * Current personality id
     */    
    TInt iPersonalityId;
    };

#endif
