/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Utility methods for handling connection settings
*
*/

#include "cmmanagerutils.h"

#include <cmconnectionmethoddef.h> // default ccm definitions
#include <cmpluginvpndef.h> // vpn specific ccm definitions
#include <cmdestinationext.h>
#include <cmmanagerext.h>

#include "eventlogger.h"
#include "log_r6.h"


static const TUint KMaxDestinationNameLength = 32;


void CmManagerUtils::CreateVPNConnectionMethodToIntranetL(const TVpnPolicyInfo& aVpnPolicyInfo,
                                                          CEventLogger& aEventLogger)
    {
    LOG_("CmManagerUtils::CreateVPNConnectionMethodToIntranetL \n");
    
    using namespace CMManager;
    
    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();        
    CleanupClosePushL( cmManagerExt );     

    //Makes sure that Internet Destination Exists
    RArray<TUint32> destinationArray;    
    cmManagerExt.AllDestinationsL( destinationArray );
    CleanupClosePushL(destinationArray);    
        
    TUint32 internetDestinationId = 0;
    for (TInt i = 0; i < destinationArray.Count(); ++i)
        {
        RCmDestinationExt destination = cmManagerExt.DestinationL( destinationArray[i] );
        CleanupClosePushL(destination);
        
        TUint32 purposeMetaData = destination.MetadataL( ESnapMetadataPurpose );
        if ( ESnapPurposeInternet ==  purposeMetaData )
            {
            internetDestinationId = destinationArray[i];
            CleanupStack::PopAndDestroy(); //destination
            break;
            }                
        CleanupStack::PopAndDestroy(); //destination
        }
    
    
    if ( internetDestinationId != 0)
        {                        
        //Generate name for the connection method
        HBufC* name = CreateConnectionMethodNameLC(aVpnPolicyInfo.iName);
        
        //Create VPN connection method        
        RCmConnectionMethodExt vpnConnectionMethod =
            CreateNewConnectionMethodToIntranetL(cmManagerExt,
                                                 *name,
                                                 aVpnPolicyInfo.iId,
                                                 internetDestinationId);                        
            
        CleanupClosePushL( vpnConnectionMethod );
        TUint32 connectionMethodId = vpnConnectionMethod.GetIntAttributeL( ECmId );
        aEventLogger.LogEvent(R_VPN_MSG_CREATED_VPN_ACCESS_POINT_WITH_SNAP,
                              &(aVpnPolicyInfo.iId), NULL,
                              connectionMethodId, internetDestinationId);
        CleanupStack::PopAndDestroy(&vpnConnectionMethod);       
        CleanupStack::PopAndDestroy(name);
        }
    else
        {
        LOG_("No Internet destination: VPN connection method not created. \n");
        }
    
    CleanupStack::PopAndDestroy(); //destinationArray  
    CleanupStack::PopAndDestroy(); //cmManagerExt
    
    LOG_("CmManagerUtils::CreateVPNConnectionMethodToIntranetL - end \n");
    }

void CmManagerUtils::SetVpnConnectionMethodAttributesL(RCmConnectionMethodExt& aConnectionMethod,
                                                       const TDesC& aConnectionMethodName,                                                       
                                                       const TVpnPolicyId aPolicyId,
                                                       const TUint32 aInternetDestinationId)
    {
    using namespace CMManager;
    
    LOG_("CmManagerUtils::SetVpnConnectionMethodAttributesL \n");                                                   
    
    aConnectionMethod.SetIntAttributeL( ECmNamingMethod, ENamingNothing );
    aConnectionMethod.SetStringAttributeL( ECmName, aConnectionMethodName );
    aConnectionMethod.SetStringAttributeL( EVpnServicePolicy, aPolicyId );      
    aConnectionMethod.SetIntAttributeL( ECmNextLayerSNAPId, aInternetDestinationId );
         
    LOG_("CmManagerUtils::SetVpnConnectionMethodAttributesL - end \n");
    }

HBufC* CmManagerUtils::CreateConnectionMethodNameLC(const TDesC& aPolicyName)
    {
        
    LOG_("CmManagerUtils::CreateConnectionMethodNameLC \n");
    
    HBufC* name = HBufC::NewLC(KMaxDestinationNameLength);
    TPtr namePtr = name->Des();
    
    namePtr.Copy(aPolicyName.Left(KMaxDestinationNameLength));   
    
    LOG_("CmManagerUtils::CreateConnectionMethodNameLC - end \n");
    return name;
    }


HBufC* CmManagerUtils::CreateIntranetDestinationNameLC(RCmManagerExt& aCmManagerExt)
    {
    _LIT(KDestinationNameBase, "Intranet");
    _LIT(KDestinationNumberFormat, "(%d)");
    
    HBufC* destinationName = HBufC::NewLC(KMaxDestinationNameLength);
    TPtr destinationNamePtr = destinationName->Des();
    
    RArray<TUint32> destArray;
    aCmManagerExt.AllDestinationsL(destArray);
    CleanupClosePushL(destArray);
   
    TUint counter = 1;    
    do
        {
        destinationNamePtr = KDestinationNameBase;
        if (counter > 1)
            {        
            TBuf<10> numberValue;
            numberValue.Format(KDestinationNumberFormat, counter);
            destinationNamePtr.Append(numberValue);
            }
    
        for (TInt i = 0; i < destArray.Count(); ++i)
            {
            RCmDestinationExt destination = aCmManagerExt.DestinationL(destArray[i]);
            CleanupClosePushL(destination);
            HBufC* name = destination.NameLC();
            
            if (name->Compare(*destinationName) == 0)
                {
                destinationNamePtr.Zero();
                CleanupStack::PopAndDestroy(name);
                CleanupStack::PopAndDestroy(); //destination
                break;
                }
            
            CleanupStack::PopAndDestroy(name);
            CleanupStack::PopAndDestroy(); //destination
            }
        counter++;
        }while(destinationName->Length() == 0);
    
    CleanupStack::PopAndDestroy(); //destArray
    return destinationName;
    }


RCmConnectionMethodExt CmManagerUtils::CreateNewConnectionMethodToIntranetL(RCmManagerExt& aCmManagerExt,
                                                                            const TDesC& aConnectionMethodName,
                                                                            const TVpnPolicyId aPolicyId,
                                                                            const TUint32 aInternetDestinationId)
    {
    LOG_("CmManagerUtils::CreateNewConnectionMethodToIntranetL \n");
    using namespace CMManager;

    RCmDestinationExt intranetDestination;
    RCmConnectionMethodExt vpnConnectionMethod;
    TBool intranetSnapFound = EFalse;
    
    RArray<TUint32> destinationArray;    
    aCmManagerExt.AllDestinationsL( destinationArray );
    CleanupClosePushL(destinationArray);    
    
    //Try to find Intranet Destination using metadata.        
    for (TInt i = 0; i < destinationArray.Count(); ++i)
        {
        intranetDestination = aCmManagerExt.DestinationL( destinationArray[i] );
        CleanupClosePushL(intranetDestination);
        
        TUint32 purposeMetaData = intranetDestination.MetadataL( ESnapMetadataPurpose );
        if ( purposeMetaData ==  ESnapPurposeIntranet)
            {
            //Jumps out and leaves the destination 
            //in the clenaup stack.     
            intranetSnapFound = ETrue;
            // create new vpn record with default values to intranet
            vpnConnectionMethod = 
                intranetDestination.CreateConnectionMethodL( KPluginVPNBearerTypeUid );
            SetVpnConnectionMethodAttributesL( vpnConnectionMethod,
                                               aConnectionMethodName,
                                               aPolicyId,
                                               aInternetDestinationId );
            intranetDestination.UpdateL();
            break;
            }                
        CleanupStack::PopAndDestroy(); //intranetDestination
        }            
                                                
    if (!intranetSnapFound)
        {                      
        //If not found Create the Intranet Destination.
        //If the Intranet destination is not supported this leaves.
        HBufC* intranetSnapName = CreateIntranetDestinationNameLC(aCmManagerExt);            
        intranetDestination = 
            aCmManagerExt.CreateDestinationL( *intranetSnapName );
        CleanupStack::PopAndDestroy(intranetSnapName);

        CleanupClosePushL(intranetDestination);
        TRAPD(err, intranetDestination.SetMetadataL( ESnapMetadataPurpose, 
                                                     ESnapPurposeIntranet ));

        //Intranet metadata is not supported in all platforms.
        if (err == KErrNone)
            {
             // create new vpn record with default values  to intranet
            vpnConnectionMethod = 
                intranetDestination.CreateConnectionMethodL( KPluginVPNBearerTypeUid );
            SetVpnConnectionMethodAttributesL( vpnConnectionMethod,
                                               aConnectionMethodName,
                                               aPolicyId,
                                               aInternetDestinationId );
            intranetDestination.UpdateL();
             }
        else
            {
 
            // create new vpn record with default values   
            vpnConnectionMethod = 
                aCmManagerExt.CreateConnectionMethodL( KPluginVPNBearerTypeUid );
            SetVpnConnectionMethodAttributesL( vpnConnectionMethod,
                                               aConnectionMethodName,
                                               aPolicyId,
                                               aInternetDestinationId );
            vpnConnectionMethod.UpdateL();
            }
        }

    CleanupStack::PopAndDestroy(&intranetDestination); 
    CleanupStack::PopAndDestroy(&destinationArray); 
    
    LOG_("CmManagerUtils::CreateNewConnectionMethodToIntranetL - end \n");
    return vpnConnectionMethod;
    }
