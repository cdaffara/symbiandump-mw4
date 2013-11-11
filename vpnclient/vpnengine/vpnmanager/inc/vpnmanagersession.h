/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Handles client requests.
*
*/



#ifndef __VPNMANAGERSESSION_H__
#define __VPNMANAGERSESSION_H__

#include <e32base.h>

class CVpnManagerServer;
class CVpnApiServant;   

/** 
 * A handler for the service requests received from clients.
 * An instance of class CVpnManagerSession is created for each client.
 */
class CVpnManagerSession : public CSession2
    {
public:
    /**
     * Creates a CVpnManagerSession object using two phase construction,
     * and returns a pointer to the created object
     *
     * @param Client's thread
     * @param The server
     *
     * @result Pointer to the new session
     */
    static CVpnManagerSession* NewL(CVpnManagerServer& aServer,
                                    CVpnApiServant& aVpnApiServant);
    /**
     * Creates a CVpnManagerSession object using two phase construction,
     * and returns a pointer to the created object
     * (leaving a pointer to the object on the cleanup stack)
     *
     * @param Client's thread
     * @param The server
     *
     * @result Pointer to the new session
     */
    static CVpnManagerSession* NewLC(CVpnManagerServer& aServer,
                                     CVpnApiServant& aVpnApiServant);

	/**
     * Destroy the object and release all memory objects
     */
    ~CVpnManagerSession();

public: // from CSession
    /**
     * Service request from client
     * 
     * @param Message from client (containing requested operation and any data)
     */
    void ServiceL(const RMessage2& aMessage);

private: // New methods

    /**
     * The first phase of two phase construction
     * 
     * @param Client's thread
     * @param The server
     */
    CVpnManagerSession(CVpnManagerServer& aServer,
                       CVpnApiServant& aVpnApiServant);

    /**
     * The second phase of two phase construction
     */
    void ConstructL() ;

private:
    RMessage2               iMessage;
    CVpnManagerServer&      iServer;
    CVpnApiServant&         iVpnApiServant;    
    };

#endif // __VPNMANAGERSESSION_H__
