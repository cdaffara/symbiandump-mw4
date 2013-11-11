/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Adheres to the UsbMan USB Class API and manages the class.
*
*/


#ifndef CUSBOBEXCLASSCONTROLLER_H
#define CUSBOBEXCLASSCONTROLLER_H

#include <e32std.h>
#include <cusbclasscontrollerplugin.h>
#include <usbobex.h>
#include "obexsmwatcher.h"


const TInt KObexClassPriority = 2;
const TInt KObexNumInterfaces = 2;

_LIT(KUsbObexLddName, "eusbc");
_LIT(KUsbObexIfc, "OBEX");

/**
 *  This class implements OBEX class controller
 *
 *  @since S60 V3.1
 */
NONSHARABLE_CLASS(CUsbObexClassController) : public CUsbClassControllerPlugIn, public MObexSMObserver
    {
public:
    /**
     * Constructs a CUsbObexClassController object.
     *
     * @since S60 V3.1
     * @param aOwner USB Device that owns and manages the class.
     * @return A new CUsbObexClassController object
     */
    static CUsbObexClassController* NewL(MUsbClassControllerNotify& aOwner);

// from base class CActive.

    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError(TInt aError);

// from MObexSMObserver    
    void MosmError(TInt aError);

// from base class CUsbClassControllerBase
    
    /**
     * From CUsbClassControllerBase.
     * Destructor
     *
     */ 
    virtual ~CUsbObexClassController();

    /**
     * From CUsbClassControllerBase.
     * Usbman starts class controller with this method.
     *
     * @param aStatus Will be completed either success or failure.
     */ 
    virtual void Start(TRequestStatus& aStatus);

    /**
     * From CUsbClassControllerBase.
     * Usbman stops class controller with this method.
     *
     * @param aStatus Will be completed either success or failure.
     */ 
    virtual void Stop(TRequestStatus& aStatus);

    /**
     * From CUsbClassControllerBase.
     * Returns information about the interfaces supported by this class.
     *
     * @param aDescriptorInfo Will be filled in with interface information.
     */ 
    virtual void GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const;


protected:
    CUsbObexClassController(MUsbClassControllerNotify& aOwner);
    void DoGetDescriptorInfoL(TUsbDescriptor& aDescriptorInfo) const;
    void ConstructL();

private: // data
    TRequestStatus* iRequestStatus;
    CObexUSB* iObexSM; 
    CObexSMWatcher *iObexSMWatcher;
    };

#endif // CUSBOBEXCLASSCONTROLLER_H
