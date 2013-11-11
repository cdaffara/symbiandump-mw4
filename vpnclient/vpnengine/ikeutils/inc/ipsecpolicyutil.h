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
* Description:  Utility class for using IPSec policy server 
*
*/


#ifndef C_IPSECPOLICYUTIL_H
#define C_IPSECPOLICYUTIL_H

#include <e32base.h>
#include <ipsecpolapi.h>

// FORWARD DECLARATIONS
class CIpsecSaSpecList;
class TInetAddr;

class CIpsecPolicyUtil : public CBase
    {
public:    

    IMPORT_C static CIpsecPolicyUtil* NewL();
    ~CIpsecPolicyUtil() ;

    
    /**
     * Gets acceptable IPsec policies for specified selectors.
     * 
     * @param aLocalAddr IP address, including possible port, of the local end selector
     * @param aLocalMask Local end selector mask
     * @param aRemoteAddr IP address, including possible port, of the remote end selector
     * @param aRemoteMask Remote end selector mask
     * @param aProtocol Protocol id
     * @param aVpnNetId VPN net id
     */
    IMPORT_C CIpsecSaSpecList* GetIpseSaSpecListLC( TInetAddr aLocalAddr, TInetAddr aLocalMask, 
                                                    TInetAddr aRemoteAddr, TInetAddr aRemoteMask,
                                                    TInt aProtocol, TUint32 aVpnNetId );

private:
    CIpsecPolicyUtil();
    void ConstructL();
    
private: // data
    
    /**
     * IPSec policy server.
     * Own.
     */
    RIpsecPolicyServ    iIpsecPolicyServ;
    
    };
    

#endif // C_IPSECPOLICYUTIL_H
