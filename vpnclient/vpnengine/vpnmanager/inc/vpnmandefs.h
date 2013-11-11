/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This module defines the classes used for transferring data.
*
*/



#ifndef __VPNCONNMANDEFS_H__
#define __VPNCONNMANDEFS_H__

#include <e32base.h>
#include <e32std.h>
#include <in_sock.h>
#include <ipsecpolapi.h>

/**
* Real interface interface IAPId/NetworkId package definition 
* for C/S message parameters. VPN Nif name is added because this
* information is passed to kmdserver too.
*/
class TRealConfig
{
    public:
        inline TRealConfig()
        :iVpnIapId(0), iVpnNetId(0), iRealIapId(0), iRealNetId(0), iSnapId(0)
        {iVPNIfName.Zero(); }
        
    public:
        TUint32     iVpnIapId;
        TUint32     iVpnNetId;
        TUint32     iRealIapId;
        TUint32     iRealNetId;
        TUint32     iSnapId;
        TBuf<64>    iVPNIfName;
};
typedef TPckg<TRealConfig> TRealConfigPckg;

/**
* Address information package definition for passing addresses
* in Client/Server message. 
*/
class TVPNAddress
{
    public:
        inline TVPNAddress()
        :iVPNIfAddr(), iVPNIfDNS1(), iVPNIfDNS2()
        {iVPNIfAddr.Zero(); iVPNIfDNS1.Zero(); iVPNIfDNS2.Zero(); }
    
    public:
        TInetAddr       iVPNIfAddr;
        TInetAddr       iVPNIfDNS1;
        TInetAddr       iVPNIfDNS2;
};
typedef TPckg<TVPNAddress> TVPNAddressPckg;


#endif  // __VPNCONNMANDEFS_H__