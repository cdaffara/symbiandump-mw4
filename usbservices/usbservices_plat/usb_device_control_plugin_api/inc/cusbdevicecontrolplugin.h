/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  USB Device Control Plug-In API
*
*/


#ifndef     CUSBDEVICECONTROLPLUGIN_H__
#define    CUSBDEVICECONTROLPLUGIN_H__

#include <e32base.h>
#include <ecom/ecom.h>

const TUid KUsbCMHandlerInterface =  {0x10283306}; // this interface UID

class RUsb;
class RDevUsbcClient;
class RUsbWatcher;

/**
 *  All USB control message handlers plugins must implement this interface.
 *  ECOM framework is used here.
 *  Class is inherited from CBase, to have an access to virtual destructor
 *  NewL is not provided, due to class is not intended for instantiation 
 *
 *  @since S60 5.0
 */
class CUsbCMHandler: public CBase
    {
public: 
    
    /**
     * Destruction
     * 
     */
     virtual ~CUsbCMHandler();
    
     /**
     * Handler
     *
     * @since S60 v.5.0
     * @param aSetupPacket Request to be handled
     * @param aData Either data for request, or result of request handling
     * @return Errorcode
     */
    virtual TInt Handle(const RBuf8& aSetupPacket, RBuf8& aData) = 0;
    
    /**
     * Provides handlers with links to USB services 
     *
     * @since S60 v.5.0
     * @param aUsbcClient Link to LDD services
     * @param aUsbWatcher Link to USB Watcher services
     * @param aUsbManager Link to USB Manager services 
     */
     virtual void Initialize(RDevUsbcClient& aUsbcClient, RUsbWatcher& aUsbWatcher, RUsb& aUsbManager) = 0;
     
    /**
     * UID given to us by ECOM when the instance is created. Used when the 
     * instance is destroyed.
     * The class member is public one, due to when instantiate the plugin, there is need access to this member by _FOFF macro
     * This member is not intended for initialization, modification, or any else type of use
     */
    TUid iPrivateEComUID;
    };
    
    #include <cusbdevicecontrolplugin.inl>

#endif // CUSBDEVICECONTROLPLUGIN_H__
