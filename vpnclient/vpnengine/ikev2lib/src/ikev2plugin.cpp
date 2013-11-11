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
* Description:   IKEv2 protocol plugin
*
*/

#include <random.h>
#include <in_sock.h>

#include "ikev2plugin.h"
#include "ikedebug.h"
#include "ikev2pluginsession.h"
#include "ipsecpolicyutil.h"


EXPORT_C MIkePluginIf* Ikev2PlugInL( MKmdEventLoggerIf& aEventLogger,
                                     MIkeDebug& aDebug )
    {
    return CIkev2PlugIn::NewL(aEventLogger, aDebug);
    }

CIkev2PlugIn* CIkev2PlugIn::NewL( MKmdEventLoggerIf& aEventLogger,
                                  MIkeDebug& aDebug )
    {
    CIkev2PlugIn* self = new (ELeave) CIkev2PlugIn(aEventLogger, aDebug);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    

CIkev2PlugIn::CIkev2PlugIn( MKmdEventLoggerIf& aEventLogger,
                            MIkeDebug& aDebug )
: iEventLogger(aEventLogger), iDebug(aDebug)
    {
    }


void CIkev2PlugIn::ConstructL()
    {    
    iPfKeySocketIf = CPFKeySocketIf::NewL(this, iDebug);    
    iIpsecPolicyUtil = CIpsecPolicyUtil::NewL();
    }
    
    
CIkev2PlugIn::~CIkev2PlugIn()
    {
    delete iIpsecPolicyUtil;
    delete iPfKeySocketIf;
    
    __ASSERT_DEBUG( iPluginSessions.Count() == 0,
                    User::Invariant() );       
    iPluginSessions.Close();    	  
    }


MIkePluginSessionIf* CIkev2PlugIn::CreateSessionL( TUint32 aVpnIapId,
                                                   TUint32 aVpnNetId,
                                                   TUint32 aVpnInterfaceIndex,
                                                   MIkeDataInterface& aIkeDataInterface )
    {
    CIkev2PluginSession* pluginSession = CIkev2PluginSession::NewL( aVpnIapId,
                                                                    aVpnNetId,
                                                                    aVpnInterfaceIndex,
                                                                    aIkeDataInterface,
                                                                    *this,
                                                                    *iPfKeySocketIf,
                                                                    *iIpsecPolicyUtil,
                                                                    iEventLogger,
                                                                    iDebug ); 
    TInt err = iPluginSessions.Append( pluginSession );
    
    if ( err != KErrNone )
        {
        delete pluginSession;
        pluginSession = NULL;
        User::Leave( err );
        }
    
    return pluginSession;    
    }

void CIkev2PlugIn::PluginSessionDeleted(const MIkePluginSessionIf* aDeletedSession)
    {
    for (TInt i = 0; i < iPluginSessions.Count(); ++i)
        {
            if (iPluginSessions[i] == aDeletedSession)
                {
                iPluginSessions.Remove(i);
                }
        }
    }

void CIkev2PlugIn::PfkeyMessageReceived( const TPfkeyMessage& aPfkeyMessage )
    {    
    switch ( aPfkeyMessage.iBase.iMsg->sadb_msg_type )
        {
        case SADB_ADD: // Fall through
        case SADB_ACQUIRE:
            {
            for ( TInt i=0; i< iPluginSessions.Count(); i++ )
                {
                if ( iPluginSessions[i]->MatchDestinationAddress( aPfkeyMessage.iDstAddr.Address() ) )                
                    {
                    iPluginSessions[i]->PfkeyMessageReceived( aPfkeyMessage );
                    break;
                    }
                }
            break;
            }
            
        case SADB_EXPIRE:
            {
            for ( TInt i=0; i< iPluginSessions.Count(); i++ )
                {
                if ( iPluginSessions[i]->MatchDestinationAddress( aPfkeyMessage.iSrcAddr.Address() ) )                
                    {
                    iPluginSessions[i]->PfkeyMessageReceived( aPfkeyMessage );
                    break;
                    }
                }
            break;                        
            }
        default:
            break;
        }        
    }
