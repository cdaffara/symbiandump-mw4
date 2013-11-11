/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VPN AP storer
*
*/



#ifndef __DMADSTOREVPNAP_H__
#define __DMADSTOREVPNAP_H__

#include <e32base.h>
#include <cmmanagerext.h>

typedef TInt32 TVpnApCommsId; 
const TInt KVpnApSvrMaxFieldLength = 50;

class TVpnApParms
    {
    public:
        enum TRealConnRefType
            {
            EIapRealConnRef,
            ESnapRealConnRef
            };
    
        TBuf<KVpnApSvrMaxFieldLength>   iName;
        TBuf<KVpnApSvrMaxFieldLength>   iPolicyId;
        
        TRealConnRefType                iRealConnRefType;
        TVpnApCommsId                   iRealConnRef;        
    };


class RCmConnectionMethodExt;

/**
 *  Store VPN AP
 *
 *  Store VPN accesspoints data to commsdb.
 *
 *  @lib (internal) dmadipsecvpn.lib
 *  @since S60 3.0
 */
class CVpnAp : public CBase
    {
public:
    static CVpnAp* NewL(void);
    ~CVpnAp();

    TBool FindVpnApL(TVpnApCommsId aId);
    void DeleteVpnApL(TVpnApCommsId aId);    
    void ListVpnApsL(RArray<TVpnApCommsId>& aIdArray);
    void GetVpnApL(TVpnApCommsId aId, TVpnApParms& aVpnApParms);
    TVpnApCommsId AddVpnApL(const TVpnApParms& aVpnApParms);
    void UpdateVpnApL(TVpnApCommsId aId, const TVpnApParms& aVpnApParms);

private: // implementation
    void ConstructL(void);
    CVpnAp();
    
    
    void UpdateVpnApL(RCmConnectionMethodExt& aConnectionMethod, 
                      const TVpnApParms& aVpnApParms);
    
    TBool IsVpnConnectionMethodL(RCmConnectionMethodExt& aConnectionMethod) const;
     
private: // data
    
    RCmManagerExt   iCmManagerExt;
    
    };

#endif // __DMADSTOREVPNAP_H__


