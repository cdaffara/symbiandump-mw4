/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of classes CIkev2EapIf.
*
*/

#include <eap_vpn_if.h>
#include "ikedebug.h"
#include "ikev2EapInterface.h"
#include "ikev2payloads.h"
#include "ikev2const.h"
#include "ikepolparser.h"
#include "ikev2plugin.h"

CIkev2EapIf* CIkev2EapIf::NewL(MIkev2EapIfObserver& aEapIfObserver, TUint8 aEapType, CIkeData* aIkeData, MIkeDebug& aDebug)
    {
    CIkev2EapIf* eapIf = new (ELeave)CIkev2EapIf(aEapIfObserver, aEapType, aDebug);
    CleanupStack::PushL(eapIf); 
    eapIf->ConstructL(aIkeData);
    CleanupStack::Pop(eapIf);    
    return eapIf;
    }

CIkev2EapIf::CIkev2EapIf(MIkev2EapIfObserver& aEapIfObserver, TUint8 aEapType, MIkeDebug& aDebug)
:iEapIfObserver(aEapIfObserver), iEapType(aEapType), iDebug(aDebug)
      {
      }

void CIkev2EapIf::ConstructL(CIkeData* aIkeData)
    {
    //
    // Construct EAP ECOM plug-in library.
    // If ECOM plug-in loading fails return an error instead of leaving
    //
    DEBUG_LOG(_L("Constructing CEappluginInterface"));   
    iEapPlugin = CEapVpnInterface::NewL(this, ETrue);
    DEBUG_LOG(_L("Starting CEappluginInterface"));    
    iEapPlugin->StartL(iEapType);

    HBufC8* RealmPrefixBfr    = NULL;
    HBufC8* ManualRealmBfr    = NULL;
    HBufC8* ManualUserNameBfr = NULL;   
    TPtrC8 RealmPrefix(NULL,0);
    TPtrC8 ManualRealm(NULL,0);
    TPtrC8 ManualUserName(NULL,0);
    TBool HideIdentity(EFalse);
    if ( aIkeData )
        {
        HideIdentity = aIkeData->iEAPHideIdentity;
        if ( aIkeData->iEAPRealmPrefix )
            {    
            RealmPrefixBfr = aIkeData->iEAPRealmPrefix->GetAsciiDataL();
            if ( RealmPrefixBfr )
                RealmPrefix.Set(RealmPrefixBfr->Des()); 
            }
        if ( aIkeData->iEAPManualRealm )
            {    
            ManualRealmBfr = aIkeData->iEAPManualRealm->GetAsciiDataL();
            if ( ManualRealmBfr )
                ManualRealm.Set(ManualRealmBfr->Des()); 
            }   
        if ( aIkeData->iEAPManualUserName )
            {    
            ManualUserNameBfr = aIkeData->iEAPManualUserName->GetAsciiDataL();
            if ( ManualUserNameBfr )
                ManualUserName.Set(ManualUserNameBfr->Des()); 
            }   
        } 

    DEBUG_LOG(_L("Calling CEappluginInterface::EapConfigure"));       
    iErrorStatus = iEapPlugin->EapConfigure(
      ManualUserName, ManualRealm, RealmPrefix, HideIdentity);

    delete RealmPrefixBfr;
    delete ManualRealmBfr;
    delete ManualUserNameBfr;   
    }

CIkev2EapIf::~CIkev2EapIf()
    {
    delete iEapPlugin;
    delete iIdentity;
    delete iMSK;
    }

void CIkev2EapIf::EapDataInbound(TPayloadIkev2* aEapPayload)
    {
    ASSERT(aEapPayload);
    //
    // Pass EAP payload data to EAP plug-in as inbound data
    //
    TInt Lth = (TInt)aEapPayload->PlDataLen();
    TPtrC8 EapData(aEapPayload->PayloadData(), Lth);

    DEBUG_LOG(_L("Calling CEappluginInterface::EapInbound"));         
    iEapPlugin->EapInbound(EapData);
    }

void CIkev2EapIf::QueryIdentity()
    {
    //
    // Query identity information from EAP plugin for IKE Id
    //
    DEBUG_LOG(_L("Calling CEappluginInterface::QueryIdentity"));        
    iEapPlugin->QueryIdentity();
    }

void CIkev2EapIf::EapOutboundL(HBufC8* aResponse)
    {
    //
    // Pass outgoing EAP data to IKEv2 plug-in 
    //
    DEBUG_LOG(_L("Data received from CEappluginInterface"));          
    iEapIfObserver.SendEapDataL(aResponse);
    }

void CIkev2EapIf::EapIdentityResponseL(HBufC8* aResponse)
    {
    //
    // Pass Identity data to IKEv2 plug-in 
    //
    DEBUG_LOG(_L("Identity received from CEappluginInterface"));
    
    delete iIdentity; 
    iIdentity = aResponse;
    
    iEapIfObserver.EapEventL(KEapEventGetIdentity);
    }

void CIkev2EapIf::EapSharedKeyL(HBufC8* aResponse)
    {
    //
    // Pass pre-shared key material to IKEv2 plug-in 
    //
    DEBUG_LOG(_L("MSK received from CEappluginInterface"));  
    
    delete iMSK; 
    iMSK = aResponse;
  
    iEapIfObserver.EapEventL(KEapEventGetPSK);
    }

void CIkev2EapIf::EapIndication(TNotification aNotification) 
    {
    //
    // Pass EAP notification to IKEv2 plug-in 
    //
    DEBUG_LOG1(_L("EAP indication received from CEappluginInterface: %d"), aNotification);
    
    TRAP_IGNORE( aNotification == ESuccess ? 
        iEapIfObserver.EapEventL(KEapEventSuccess) :
        iEapIfObserver.EapEventL(KEapEventFailed) );
    }

