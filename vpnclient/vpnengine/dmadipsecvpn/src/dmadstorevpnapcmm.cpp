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
* Description:   VPN access point manipulation.
*
*/



#include <cmpluginvpndef.h>
#include <cmdestinationext.h>

#include "dmadstorevpnap.h"
#include "vpnlogger.h"

using namespace CMManager;
// ======== LOCAL FUNCTIONS ========

CVpnAp* CVpnAp::NewL(void)
    {
    TRACE("CVpnAp::NewL");
    CVpnAp* self = new (ELeave) CVpnAp;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    return self;
    }


CVpnAp::CVpnAp()
    {
    TRACE("CVpnAp::CVpnAp");
    }


void CVpnAp::ConstructL()
    {
    TRACE("CVpnAp::ConstructL");
    
    iCmManagerExt.OpenL();
    }


CVpnAp::~CVpnAp()
    {
    TRACE("CVpnAp::~CVpnAp");
    
    iCmManagerExt.Close();
    }


TBool CVpnAp::FindVpnApL(TVpnApCommsId aId)
    {
    TRACE("CVpnAp::FindVpnApL");
    
    TBool connectionFound = EFalse;
    
    RCmConnectionMethodExt vpnConnectionMethod;    
    TRAPD(err, vpnConnectionMethod = iCmManagerExt.ConnectionMethodL( aId ));
    CleanupClosePushL(vpnConnectionMethod); 

    switch(err)    
        {
        case KErrNone:
            if ( IsVpnConnectionMethodL(vpnConnectionMethod) )
                {
                connectionFound = ETrue;
                }
            break;
        case KErrNotFound:
            //do nothing
            break;
        default:
            User::Leave(err);
            break;
        }
    
    CleanupStack::PopAndDestroy(); //vpnConnectionMethod    
    return connectionFound;
    }


void CVpnAp::DeleteVpnApL(TVpnApCommsId aId)
    {
    TRACE("CVpnAp::DeleteVpnApL");
    
    RCmConnectionMethodExt vpnConnectionMethod = iCmManagerExt.ConnectionMethodL( aId );
    CleanupClosePushL(vpnConnectionMethod);
    
    //Check that the type of the connection is correct.
    if ( !IsVpnConnectionMethodL(vpnConnectionMethod) )
        {
        User::Leave(KErrNotFound);
        }

    if ( !vpnConnectionMethod.DeleteL() )
        {
        User::Leave(KErrGeneral);
        }    
        
    CleanupStack::PopAndDestroy(); //vpnConnectionMethod
    }


void CVpnAp::ListVpnApsL(RArray<TVpnApCommsId>& aIdArray)
    {
    TRACE("CVpnAp::ListVpnApsL");
            
    aIdArray.Reset();            
            
    //First collect all VPN connection methods from destinations
    RArray<TUint32> destinationArray;    
    iCmManagerExt.AllDestinationsL( destinationArray );
    CleanupClosePushL(destinationArray);    
    
    for (TInt i = 0; i < destinationArray.Count(); ++i)
        {
        RCmDestinationExt destination = iCmManagerExt.DestinationL( destinationArray[i] );
        CleanupClosePushL(destination);
        
        TInt connectionMethodCount = destination.ConnectionMethodCount();
        for (TInt j = 0; j < connectionMethodCount; ++j)
            {
            RCmConnectionMethodExt connectionMethod = destination.ConnectionMethodL( j );  
            CleanupClosePushL(connectionMethod);
            if ( IsVpnConnectionMethodL(connectionMethod) )
                {
                TUint32 apId = connectionMethod.GetIntAttributeL( ECmId );
                User::LeaveIfError(aIdArray.Append(apId));
                }
            CleanupStack::PopAndDestroy(); //connectionMethod       
            }
        
        CleanupStack::PopAndDestroy(); //destination
        }
    CleanupStack::PopAndDestroy(); //destinationArray    
    
    //Second collect VPN connection methods, which are not inside a destination.    
    RArray<TUint32> connectionMethodArray;    
    iCmManagerExt.ConnectionMethodL( connectionMethodArray );
    CleanupClosePushL(connectionMethodArray);
    
    for ( TInt i = 0; i < connectionMethodArray.Count(); ++i)
        {
        RCmConnectionMethodExt connectionMethod = 
                iCmManagerExt.ConnectionMethodL( connectionMethodArray[i] );
        CleanupClosePushL(connectionMethod);
        if ( IsVpnConnectionMethodL(connectionMethod) )
            {
            User::LeaveIfError(aIdArray.Append(connectionMethodArray[i]));
            }
        CleanupStack::PopAndDestroy(); //connectionMethod               
        }    
    CleanupStack::PopAndDestroy(); //connectionMethodArray
    }


void CVpnAp::GetVpnApL(TVpnApCommsId aId, TVpnApParms& aVpnApParms)
    {
    TRACE("CVpnAp::GetVpnApL");
    
    RCmConnectionMethodExt vpnConnectionMethod = iCmManagerExt.ConnectionMethodL( aId );
    CleanupClosePushL(vpnConnectionMethod);
    
    //Check that the type of the connection is correct.
    if ( !IsVpnConnectionMethodL(vpnConnectionMethod) )
        {
        User::Leave(KErrNotFound);
        }
    
    HBufC* string = vpnConnectionMethod.GetStringAttributeL( ECmName );
    aVpnApParms.iName = *string;
    delete string;
    string = NULL;
    
    string = vpnConnectionMethod.GetStringAttributeL( EVpnServicePolicy );  
    aVpnApParms.iPolicyId = *string;
    delete string;
    string = NULL;
        
    // real IAP or SNAP. ECmNextLayerIapId == EVpnIapId
    TUint32 realConn = vpnConnectionMethod.GetIntAttributeL(EVpnIapId); 
    if (realConn != 0)
        {
        DEBUG_LOG1(_L("Real connection is IAP %d"), realConn);         
        aVpnApParms.iRealConnRefType = TVpnApParms::EIapRealConnRef;
        aVpnApParms.iRealConnRef = realConn;        
        }
    else
        {
        realConn = vpnConnectionMethod.GetIntAttributeL(ECmNextLayerSNAPId);
        DEBUG_LOG1(_L("Real connection is snap %d"), realConn);         
        aVpnApParms.iRealConnRefType = TVpnApParms::ESnapRealConnRef;
        aVpnApParms.iRealConnRef = realConn;        
        }                        

    CleanupStack::PopAndDestroy(); //vpnConnectionMethod    
    }


TVpnApCommsId CVpnAp::AddVpnApL(const TVpnApParms& aVpnApParms)
    {
    TRACE("CVpnAp::AddVpnApL");
    
    RCmConnectionMethodExt vpnConnectionMethod = 
        iCmManagerExt.CreateConnectionMethodL( KPluginVPNBearerTypeUid );

    CleanupClosePushL( vpnConnectionMethod );

    UpdateVpnApL(vpnConnectionMethod, aVpnApParms);
    
    // save changes
    vpnConnectionMethod.UpdateL();

    TUint32 apId = vpnConnectionMethod.GetIntAttributeL( ECmId );

    CleanupStack::PopAndDestroy(); // vpnConnectionMethod
    
    return apId;
    }


void CVpnAp::UpdateVpnApL(TVpnApCommsId aId, const TVpnApParms& aVpnApParms)
    {
    TRACE("CVpnAp::UpdateVpnApL");

    RCmConnectionMethodExt vpnConnectionMethod = iCmManagerExt.ConnectionMethodL( aId );
    CleanupClosePushL(vpnConnectionMethod);
    
    //Check that the type of the connection is correct.
    if ( !IsVpnConnectionMethodL(vpnConnectionMethod) )
        {
        User::Leave(KErrNotFound);
        }

    UpdateVpnApL(vpnConnectionMethod, aVpnApParms);    
    vpnConnectionMethod.UpdateL();    
    CleanupStack::PopAndDestroy(); //vpnConnectionMethod    
    }
    

void CVpnAp::UpdateVpnApL(RCmConnectionMethodExt& aConnectionMethod, 
                          const TVpnApParms& aVpnApParms)
    {
    
    TRACE("CVpnAp::UpdateVpnApL");
    
    __ASSERT_DEBUG( aConnectionMethod.GetBoolAttributeL(ECmVirtual), User::Invariant() );
    
    aConnectionMethod.SetIntAttributeL( ECmNamingMethod, ENamingNothing );
    aConnectionMethod.SetStringAttributeL( ECmName, aVpnApParms.iName );
    aConnectionMethod.SetStringAttributeL( EVpnServicePolicy, aVpnApParms.iPolicyId );  
    
    switch(aVpnApParms.iRealConnRefType)
        {        
        case TVpnApParms::EIapRealConnRef:
            aConnectionMethod.SetIntAttributeL( EVpnIapId, aVpnApParms.iRealConnRef );            
            DEBUG_LOG1(_L("VPN iap uses real iap %d"), aVpnApParms.iRealConnRef);
            DEBUG_LOG1(_L("ECmNextLayerSNAPId is %d"), aConnectionMethod.GetIntAttributeL(ECmNextLayerSNAPId));            
            break;
        case TVpnApParms::ESnapRealConnRef:            
            aConnectionMethod.SetIntAttributeL( ECmNextLayerSNAPId, aVpnApParms.iRealConnRef );
            DEBUG_LOG1(_L("VPN iap uses real SNAP %d"), aVpnApParms.iRealConnRef);
            DEBUG_LOG1(_L("EVpnIapId is %d"), aConnectionMethod.GetIntAttributeL(EVpnIapId));            
            break;
        default:
            User::Invariant();
            break;                    
        }                
    }


TBool CVpnAp::IsVpnConnectionMethodL(RCmConnectionMethodExt& aConnectionMethod) const
    {
    TBool isVPNConnectionMethod = EFalse;
    
    if ( aConnectionMethod.GetBoolAttributeL(ECmVirtual) &&
         aConnectionMethod.GetIntAttributeL( ECmBearerType ) == KPluginVPNBearerTypeUid)
        {
        isVPNConnectionMethod = ETrue;        
        }
    
    return isVPNConnectionMethod;
    }
