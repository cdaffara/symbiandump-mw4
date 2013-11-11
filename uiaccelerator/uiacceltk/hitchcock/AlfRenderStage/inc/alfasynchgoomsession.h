/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   alfasynchgoomsession provides asynchronous 
*                connection to graphics out of memory monitor.
*
*/



#ifndef __ALFASYNCHGOOMSESSION_H__
#define __ALFASYNCHGOOMSESSION_H__

#include <e32base.h>
#include <goommonitorsession.h>

/**
 * Active object to handle asynchronous connection to GOOM thread.
 */
NONSHARABLE_CLASS( CAlfAsynchGoomSession ) : public CActive
    {
public:
    /**
     * Constructor.
     */
    CAlfAsynchGoomSession();

    /**
     * Destructor.
     */
    ~CAlfAsynchGoomSession();
    
    /**
     * Informs that application is about to start.
     * This is the method clients should use.
     * @param aAppUid application UID.
     */
    void AppAboutToStart( const TUid& aAppUid );

    /**
     * Informs that application is considered to be closed.
     * @param aAppUid application UID.
     */
    void AppClosed( const TUid& aAppUid );

public:
    /**
     * Starts connection creation to goom.
     */
    void Start();

    /**
     * Handles completion of request.
     */
    void RunL();
    
    /**
     * Cancels pending request.
     */
    void DoCancel();

private:
    // Goom session. Owned.
    RGOomMonitorSession iGoomSession;
    
    // ETrue if connecting, EFalse otherwise.
    TBool iConnecting;
    
    // ETrue if connected. 
    TBool iConnected;
    
    // Pending UIDs of started applications, but not yet
    // delivered to GOOM. Owned.
    RArray<TUid> iPendingUids;
    
    // Running application list. Owned.
    RArray<TInt> iRunningAppsList;
    };

#endif //__ALFASYNCHGOOMSESSION_H__
