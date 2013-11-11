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
 * Description:  Base classes for Usb notifier wrapper  
 *
 */

#ifndef C_CUSBNOTENOTIFIER_H
#define C_CUSBNOTENOTIFIER_H

#include <e32base.h>
#include "cusbnotifier.h"

class CUsbNotifManager;
class RNotifier;

/**
 * Base class of wrapper for showing RNotifier note.
 * Class implements showing note notifier by RNotifier API in asynchronized way 
 * which doesn't block the caller. When user gives some feedback on the note, 
 * the event will be forwarded to CUsbNotifManager and it does some handling.
 */
NONSHARABLE_CLASS( CUsbNoteNotifier) : public CUsbNotifier
    {
    /**
     * Active object of asynchronous showing notifier 
     */
    class CNotifierActive : public CActive
        {
    public:
        /**
         * Default constructor
         * @param aNotifier RNotifier API
         * @param aUsbNoteNotifier the note notifier container
         */
        CNotifierActive(RNotifier& aNotifier, CUsbNoteNotifier& aUsbNoteNotifier);

        /**
         * Destructor
         */
        virtual ~CNotifierActive();

        /**
         * Start to show the notifier
         */
        void StartL();

    protected:
        // From base class CActive
        /**
         * Caled when outstanding request completed
         */
        void RunL();

        /**
         * Called when outstanding request cancelled
         */
        void DoCancel();

        /**
         * Called when RunL leaves   
         * @param aError errorcode
         * @return errorcode 
         */
        TInt RunError(TInt aError);

    private:
        // data

        /**
         * The note notifier container 
         * Not Own 
         */
        CUsbNoteNotifier& iUsbNoteNotifier;

        /**
         * RNotifier API
         * Not Own 
         */
        RNotifier& iNotifier;

            /**
             * Response from notifier 
             */
            TPckg<TInt> iRes;
            
            /**
             * Notifier id pckg buffer
             */
            TPckgBuf<TInt> iNotifIdPckg;
        };
public:

    /**
     * Destructor
     */
    virtual ~CUsbNoteNotifier();

    //From base class CUsbNotifier
    /**
     * Start to show notifier
     */
    virtual void ShowL();

    /**
     * Stop showing notifier
     */
    virtual void Close();

protected:

    /**
     * Default constructor
     * @param aNotifier RNotifier API
     * @param aNotifManager Manager
     * @param acat Category
     * @param aNotidId id 
     */
    CUsbNoteNotifier(RNotifier& aNotifier, CUsbNotifManager& aNotifManager,
            TUid aCat, TUint aNotifId);

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

private:
    // data

    /**
     * Active object of handling RNotifier instance
     * Own 
     */
    CNotifierActive* iNotifierActive;

    /**
     * RNotifier API
     * Not Own 
     */
    RNotifier& iNotifier;
    };

#endif // C_CUSBNOTENOTIFIER_H
