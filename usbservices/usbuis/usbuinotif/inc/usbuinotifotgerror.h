/*
* Copyright (c) 2007, 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares USB UI OTG Errors notifier.
 *
*/


#ifndef USBUINOTIFOTGERROR_H
#define USBUINOTIFOTGERROR_H

// INCLUDES
#include <hb/hbwidgets/hbdevicemessageboxsymbian.h>
#include "usbnotifier.h"      // Base class


// CLASS DECLARATION

/**
 *  This class is used to show general USB OTG errors
 *  Asynchronous call is required.
 *
 *  @lib
 */
NONSHARABLE_CLASS(CUsbUiNotifOtgError) : public CUSBUINotifierBase, 
                                        public MHbDeviceMessageBoxObserver
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CUsbUiNotifOtgError* NewL();

    /**
     * Destructor.
     */
    virtual ~CUsbUiNotifOtgError();
   
    /**
     * Call back function to observe device message box closing.
     * @param aMessageBox Pointer to the closing message box instance.
     * @param aButton Button that was pressed.
     */
    void MessageBoxClosed(const CHbDeviceMessageBoxSymbian* aMessageBox,
            CHbDeviceMessageBoxSymbian::TButtonId aButton);
protected:

    /**
     * Second phase contructor
     */
    void ConstructL();

private:
    // Functions from base class

    /**
     * From CUSBUINotifierBase Called when a notifier is first loaded.        
     * @param None.
     * @return A structure containing priority and channel info.
     */
    TNotifierInfo RegisterL();

    /**
     * From CUSBUINotifierBase The notifier has been deactivated 
     * so resources can be freed and outstanding messages completed.
     */
    void Cancel();

    /**
     * From CUSBUINotifierBase Used in asynchronous notifier launch to 
     * store received parameters into members variables and 
     * make needed initializations.
     * @param aBuffer A buffer containing received parameters
     * @param aReplySlot A reply slot.
     * @param aMessage Should be completed when the notifier is deactivated.
     */
    void StartDialogL(const TDesC8& aBuffer, TInt aReplySlot,
            const RMessagePtr2& aMessage);

private:

    /**
     *  C++ default constructor.
     */
    CUsbUiNotifOtgError();

private:
    // Data
    /**
     *  Query
     *  Owned
     */
    CHbDeviceMessageBoxSymbian* iQuery; 
    CDesCArrayFlat* iStringIds;
    };
#endif // USBUINOTIFOTGERROR_H
