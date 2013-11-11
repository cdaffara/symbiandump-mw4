/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of MHuiEventHandler, an interface enabling 
*                handling of scheduled events.
*
*/


 
#ifndef __HUIEVENTHANDLER_H__
#define __HUIEVENTHANDLER_H__

#include <e32base.h>

/* Forward declarations. */
class THuiEvent;

/**
 * MHuiEventHandler is an interface that can be derived by any object that want's to handle
 * Hitchcock UI Toolkit events scheduled by HuiScheduler.
 * 
 * If an object wishes to process events from the toolkit, it should implement this interface. 
 * 
 * @note   The destructor will NOT cancel any pending commands enqueued to this object,
 *         so you must call eg. <code>CHuiEnv::CancelCommands(this);</code> to cancel
 *         still pending commands to prevent calling destroyed object.
 */
class MHuiEventHandler
    {
public:

    /** 
     * Destructor. 
     * 
     * @note   Will NOT cancel any pending commands scheduled to this object,
     *         so you must call eg. <code>CHuiEnv::CancelCommands(this);</code> to cancel
     *         still pending commands to prevent calling destroyed object.
     */    
    IMPORT_C virtual ~MHuiEventHandler();

    /**
     * Called when an input event is being offered to this object.
     * 
     * The implementation must ensure that the function returns EFalse if 
     * it does not do anything in response to the event, otherwise, other objects
     * may be prevented from receiving the event. If it is able to process the 
     * event it should return ETrue.
     *
     * @param aEvent  Event to be handled.
     *
     * @return  <code>ETrue</code>, if the event was handled.
     *          Otherwise <code>EFalse</code>.
     */
    virtual TBool OfferEventL(const THuiEvent& aEvent) = 0;

    };  

#endif  // __HUIEVENTHANDLER_H__
