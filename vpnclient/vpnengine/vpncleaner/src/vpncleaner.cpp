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
* Description:   Cleans Vpn settings
*
*/



#include <e32base.h>
#include <swi/swispubsubdefs.h>

#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <cmconnectionmethodext.h>
#include <cmpluginvpndef.h>

#include "log_vpncleaner.h"
#include "vpncleaner.h"


TVpnCleaner::TVpnCleaner()
{
}


TVpnCleaner::~TVpnCleaner()
{
}


void TVpnCleaner::Clean()
{
  LOG( Log::Printf( _L( "-> TVpnCleaner::Clean()\n" ) ) );
  
  TInt value;
  
  TInt ret =
    RProperty::Get( KUidSystemCategory, Swi::KUidSoftwareInstallKey, value );

  if( KErrNone != ret ){
    // No need for error handling
    return;
  }

  switch( value & Swi::KSwisOperationMask ){
    case Swi::ESwisInstall:
      // SW update ongoing, no clean needed
      return;
    case Swi::ESwisUninstall:
      TRAPD( err, CleanAPsL() );
      LOG( Log::Printf( _L( "TVpnCleaner::Clean(), err: %d\n" ), err) );
      
      // Prevent compiler warning Re: unused variable
      if( KErrNone == err ){
      }  
      break;
    default:
      return; 
  }
}


void TVpnCleaner::CleanAPsL()
{
  using namespace CMManager;
  RCmManagerExt cmManager;
  cmManager.OpenL();
  CleanupClosePushL( cmManager );

  DelAPsL( cmManager );

  CleanupStack::PopAndDestroy();  // cmManager
}


void TVpnCleaner::DelAPsL( RCmManagerExt& aCm )
{
  using namespace CMManager;

  RArray<TUint32> aps;

  TBool supportedBearersOnly = ETrue;
  TBool legacyCmsOnly        = EFalse;

  aCm.ConnectionMethodL( aps, supportedBearersOnly, legacyCmsOnly );
  CleanupClosePushL( aps );

  for( TInt i = 0; i < aps.Count(); ++i ){
    RCmConnectionMethodExt ap = aCm.ConnectionMethodL( aps[i] );
    CleanupClosePushL( ap );
    
    if( KPluginVPNBearerTypeUid == ap.GetIntAttributeL( ECmBearerType ) ){
      ap.DeleteL();
    }

    CleanupStack::PopAndDestroy();  // ap
  }    
  
  CleanupStack::PopAndDestroy();  // aps
}


/***/
