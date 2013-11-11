/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  active object, that monitors IKE plugin session error
*
*/


#ifndef C_ERROROBSERVER_H
#define C_ERROROBSERVER_H

#include <e32base.h>

// FORWARD DECLARATIONS
class MIkeDebug;
class MIkePluginSessionIf;
class MIkePluginSessionHandlerCallback;

/**
 *  Error observer.
 *  Active object that provides functionality for observing IKE plugin session
 *  error.
 *
 *  @lib internal (kmdserver.exe)
  */
class CErrorObserver : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aIkePluginSession IKE plugin session
     * @param aCallback Callback interface
     * @param aDebug Debug trace interface
     */
    static CErrorObserver* NewL( MIkePluginSessionIf& aIkePluginSession,
                                 MIkePluginSessionHandlerCallback& aCallback,
                                 MIkeDebug& aDebug );

    /**
     * Destructor.
     */
    ~CErrorObserver();

    /**
     * Starts observing internal address change.
     */
    void StartObserving();
    
private:
    
    CErrorObserver( MIkePluginSessionIf& aIkePluginSession,
                    MIkePluginSessionHandlerCallback& aCallback,
                    MIkeDebug& aDebug );
    
    void ConstructL();

// from base class CActive
    
    /**
     * From CActive
     * Handles completion of asynchronous notification request.
     */    
    void RunL();

    /**
     * From CActive
     * Handles cancellation of asynchronous notification request.
     */    
    void DoCancel();
        
private: // data
    
    /**
     * IKE plugin session.
     * Not own.
     */
    MIkePluginSessionIf&        iIkePluginSession;
    
    /**
     * Callback interface.
     * Not own.
     */
    MIkePluginSessionHandlerCallback&     iCallback;
    
    /**
     * Debug trace interface.
     * Not own.
     */
    MIkeDebug&                  iDebug;            
    };


#endif // C_ERROROBSERVER_H
