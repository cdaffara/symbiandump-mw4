/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Application UI definition
*
*/



#ifndef M_ALFMETRICSINTERFACE_H
#define M_ALFMETRICSINTERFACE_H

#include <e32base.h>
#include <e32cmn.h>

class MAlfTraceInterface;

class MAlfMetricsInterface
    {
public:
    /**
     * New message from client has arrived. If the message shouldn't be processed futhther by session,
     * Message must be completed in the implementation of method
     *
     * @param aSessionId identifies the session which received the message.
     * @param aMessage refence to arrived message
     *
     */
    virtual void EnterClientMessageL(TInt aSessionId, const RMessage2& aMessage) = 0;
    
    /**
     * Handling of message has been completed in server, server is about to enter back to active status 
     *
     * @param aSessionId identifies the session which received the message.
     * @param aMessage refence to arrived message
     *
     */
    virtual void ExitClientMessage(TInt aSessionId, const RMessage2& aMessage) = 0;
    
    /**
     * Called when new session is created on the server.
     *
     * @param aSessionId Session id for new session
     *
     */
    virtual void SessionCreated(TInt aSessionId) = 0;
    
    /**
     * Called when session is closing down on the server.
     *
     * @param aSessionId Session id for the session
     *
     */
    virtual void SessionTerminated(TInt aSessionId) = 0;
    
    /**
     * Get trace object for session id.
     *
     * @since S60 v3.2
     * @param aSessionId Session id
     * @return Trace object, or NULL in case of unknown session id
     */
    virtual MAlfTraceInterface* GetTraceForSession(TInt aSessionId) const = 0;

    /**
     * Make new session be the default session
     *
     * @since S60 v3.2
     * @param aSessionId Session id
     *
     */
    virtual void PushDefaultSession(TInt aSessionId) = 0;

    /**
     * Restore to previous default session.
     *
     * @since S60 v3.2
     *
     */
    virtual void PopDefaultSession() = 0;
    
    /**
     * Get trace object for default session. If there's no default session defined, the
     * auxiliary trace will be returned.
     *
     * @since S60 v3.2
     * @return Trace object for default session
     */
    virtual MAlfTraceInterface& GetDefaultSessionTrace() const = 0;

    /**
     * Get the current default session id. If there's no default session defined,
     * the id will be 0.
     *
     * @since S60 v3.2
     * @return The default session id
     */
    virtual TInt GetDefaultSessionId() const = 0;

    /**
     * Get auxiliary trace object for tracing without exact session id.
     *
     * @since S60 v3.2
     * @param aSessionId Session id
     * @return Trace object, or NULL in case of unknown session id
     */
    virtual MAlfTraceInterface& GetAuxiliaryTrace() const = 0;
    };

#endif // M_ALFMETRICSINTERFACE_H
