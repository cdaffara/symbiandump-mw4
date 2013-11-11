/*
 * Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Declares USB UI Queries notifier.
 *
 */

#ifndef USBUINQUERIESNOTIFIER_H
#define USBUINQUERIESNOTIFIER_H

// INCLUDES
#include <hb/hbwidgets/hbdevicemessageboxsymbian.h>
#include "usbnotifier.h"      // Base class


// CLASS DECLARATION

/**
 *  This class is used to show USB query dialogs.
 *
 *  @lib 
 *  @since S60 3.0
 */
NONSHARABLE_CLASS(CUSBUIQueriesNotifier) : public CUSBUINotifierBase,
                                           public MHbDeviceMessageBoxObserver
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CUSBUIQueriesNotifier* NewL();

    /**
     * Destructor.
     */
    virtual ~CUSBUIQueriesNotifier();
    /**
     * Call back function to observe device message box closing.
     * @param aMessageBox Pointer to the closing message box instance.
     * @param aButton Button that was pressed.
     */
    void MessageBoxClosed(const CHbDeviceMessageBoxSymbian* aMessageBox,
            CHbDeviceMessageBoxSymbian::TButtonId aButton);


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
     * @param None.
     * @return None.
     */
    void Cancel();

    /**
     * From CUSBUINotifierBase Used in asynchronous notifier launch to 
     * store received parameters into members variables and 
     * make needed initializations.
     * @param aBuffer A buffer containing received parameters
     * @param aReplySlot A reply slot.
     * @param aMessage Should be completed when the notifier is deactivated.
     * @return None.
     */
    void StartDialogL(const TDesC8& aBuffer, TInt aReplySlot,
            const RMessagePtr2& aMessage);

private:
    
    /**
     *  C++ default constructor.
     */
    CUSBUIQueriesNotifier();


private:
    // Data
    
    /**
    * owned
    */
    CHbDeviceMessageBoxSymbian*  iUSBQueryDlg;
    TUSBUIQueries iQueryType; // To store the type of the query

    };
#endif // USBUINQUERIESNOTIFIER_H
