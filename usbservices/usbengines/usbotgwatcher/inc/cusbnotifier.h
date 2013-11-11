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

#ifndef C_CUSBNOTIFIER_H
#define C_CUSBNOTIFIER_H

#include <e32base.h>

class CUsbNotifManager;

/**
 * Base class for usb notifiers.
 * Define primitive methods needed for all kinds of notifiers such as Avkon 
 * informative note, feedback-needed note, usb indicator etc.
 * Some notifier needs feedback input from end-user such as waiting note, some 
 * of them don't such as informative note and usb indicator. 
 */
NONSHARABLE_CLASS( CUsbNotifier) : public CBase
    {
public:
    /**
     * Destructor.
     */
    virtual ~CUsbNotifier();

    /**
     * Start to show notifier
     */
    virtual void ShowL() = 0;

    /**
     * Stop showing notifier
     */
    virtual void Close() = 0;

    /**
     * Getter for returning if the notifier needs input from end-user
     * @return ETrue-user feedback needed, EFalse-user feedback not needed. 
     */
    TBool IsFeedbackNeeded() const;

protected:

    /**
     * Default constructor
     * @param aNotifManager manager
     * @param acat Category
     * @param aNotidId id 
     */
    CUsbNotifier(CUsbNotifManager& aNotifManager, TUid aCat, TUint aNotifId);

    /**
     * Set by child-class if the concrete notifier needs feedback from end-user
     */
    void SetFeedbackNeeded();

protected:
    //data

    /**
     * The manager of all usb notifieres.
     * All notifiers are created, destroied and manupulated by the manager.  
     * Not own 
     */
    CUsbNotifManager& iNotifManager;

    /**
     * Category of the notifier. Needed for cancelling 
     */
    TUid iCat;

    /**
     * The identifier of the notifier
     */
    TUint iNotifId;

    /**
     * If the notifier needs feedback from end-user. 
     */
    TBool iIsFeedbackNeeded;

    };

#endif // C_CUSBNOTIFIER_H
