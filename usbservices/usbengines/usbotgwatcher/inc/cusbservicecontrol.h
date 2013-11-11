/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Implementation 
 *
 */

#ifndef C_USBSERVICECONTROL_H
#define C_USBSERVICECONTROL_H

#include <e32base.h>

class RUsb;

/**
 *  usb service control observer interface, to get feedback on usb service state change
 *
 */
NONSHARABLE_CLASS( MUsbServiceControlObserver)
    {
public:

    /**
     * called when request for usb services is completed
     * if there were few requests in a row, during operation, 
     * only the last one's completion is notified
     * @param aError error code
     */
    virtual void UsbServiceControlReqCompletedL(TInt aError) = 0;

    };

/**
 *  This class implements starting/stopping usb services 
 *
 */
NONSHARABLE_CLASS( CUsbServiceControl ) : public CActive
    {

public:

    enum TUsbServiceRequest
        {
        ERequestUndefined,
        EStartUsbService,
        EStopUsbService
        };

    /**
     * Two-phased constructor.
     * @param aObserver Observer
     * @param aUsb RUsb API
     */
    static CUsbServiceControl* NewL(MUsbServiceControlObserver& aObserver,
            RUsb& aUsb);

    /**
     * Destructor.
     */
    virtual ~CUsbServiceControl();

    /**
     * Starts usb service. When service is started call back function 
     * MUsbServiceControlObserver::UsbServiceControlReqCompletedL will be called.
     * Client should not worry is usb service already started, or not,
     * and which one personality is started. Just request this API 
     * @param aPersonalityId personality id (required by RUsb API to start usb services) 
     * @return error code
     */
    TInt StartL(TInt aPersonalityId);

    /**
     * Stops service. When service is stopped call back function 
     * MUsbServiceControlObserver::UsbServiceControlReqCompletedL will be called
     * @return error code
     */
    TInt StopL();

public:

    // from base class CActive
    /**
     * Called when request is completed
     */
    void RunL();

    /**
     * called when RunL leaves
     * @param aError error code
     * @return error code
     */
    TInt RunError(TInt aError);

    /**
     * Called when request is cancelled
     */
    void DoCancel();

private:

    /**
     * Default constructor
     * @param aObserver receives call back when service started or stopped
     * @param aUsb usbman API
     */
    CUsbServiceControl(MUsbServiceControlObserver& aObserver, RUsb& aUsb);

    /**
     * 2nd phase construction
     */
    void ConstructL();

private:

    /**
     * Observer
     * not own
     */
    MUsbServiceControlObserver& iObserver;

    /**
     * RUsb API
     */
    RUsb& iUsb;

    /**
     * personality ID, to be started
     */
    TInt iPersonalityId;

    /**
     * Service state
     */
    TUsbServiceState iServiceState; // required as a parameter for service states notifications 

    };

#endif //  C_USBSTATEHOSTAHANDLE_H
