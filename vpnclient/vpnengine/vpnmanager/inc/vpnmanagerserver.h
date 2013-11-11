/*
* Copyright (c) 2000-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
* Server component for VPN (Ext) Manager, used by VPN (Ext) API. 
* VPN Ext API uses the definition, but provides its own implementation.
*
*/



#ifndef __VPNMANAGERSERVER_H__
#define __VPNMANAGERSERVER_H__

#include <e32base.h>
#include <f32file.h>
#include "vpnmanagerserverdefs.h"

class CVpnApiServant;

/** 
 * The main server class
 */
class CVpnManagerServer : public CPolicyServer
    {
public:

    /**
     * Create a CVpnManagerServer object using two phase construction,
     * and return a pointer to the created object
     * 
     * @result pointer to created CVpnManagerServer object
     */
    static CVpnManagerServer* NewL();

    /**
     * Create a CVpnManagerServer object using two phase construction,
     * and return a pointer to the created object, leaving a pointer
     * to the object on the cleanup stack.
     * 
     * Pointer to created CVpnManagerServer object
     */
    static CVpnManagerServer* NewLC();

    /**
     * Destroy the object and release all memory objects
     */
    ~CVpnManagerServer();

    /**
     * Increments the count of the active sessions for this server
     */
	void IncrementSessions();

    /**
     * Decrements the count of the active sessions for this server.
     * If no more sessions are in use the server terminates.
     */
	void DecrementSessions();

protected: // From CActive
    /**
     * Process any errors
     * 
     * @param aError the leave code reported
     */
	TInt RunError(TInt aError);

private:

    /**
     * Perform the first phase of two phase construction
     * 
     * @param Priority for this thread
     */
    CVpnManagerServer(TInt aPriority);

    /**
     * Perform the second phase construction of a CVpnManagerServer object
     */
    void ConstructL() ;

private: // From CServer

    /**
     * Create a time server session, and return a pointer to the
     * created object
     * 
     * @param Version
     * 
     * @result Pointer to the new session
     */ 
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;

private:
    /** The number of client-server sessions */
    TInt iSessionCount;

    /** The handler of client requests */
    CVpnApiServant* iVpnApiServant;
    
    /** The common file server session used throughout the VPN Manager */ 
    RFs iFs;
    
	static const TUint iVpnManRangeCount;
	static const TInt iVpnManRanges[];
	static const TUint8 iVpnManElementIndex[];

    static const CPolicyServer::TPolicyElement iVpnManElements[];
    static const CPolicyServer::TPolicy iVpnManPolicy;
    };


#endif // __VPNMANAGERSERVER_H__

