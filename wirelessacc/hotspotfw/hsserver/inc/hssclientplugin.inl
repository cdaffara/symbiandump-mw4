/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Inline functions of CHssClientPlugin class.
*
*/



#ifndef HSSCLIENTPLUGIN_INL
#define HSSCLIENTPLUGIN_INL

// ---------------------------------------------------------
// CHssClientPlugin::NewL
// ---------------------------------------------------------
//
inline CHssClientPlugin* CHssClientPlugin::NewL( const TUid aUid, TDesC8& aUidText )
    {
    TAny* interface( NULL );
    if ( aUidText != KNullDesC8 )
        {
        TEComResolverParams resolverParams;
        
        TBuf8<KExtensionAPILength> buf;
        buf.Append( KUidPrefix );
        buf.Append( aUidText );
        buf.Append( KExtensionAPI );
        resolverParams.SetDataType( buf );
        resolverParams.SetWildcardMatch( ETrue );
        
        interface = REComSession::CreateImplementationL(
        K3rdPartyEcomInterfaceUid,  _FOFF( CHssClientPlugin,
        iInstanceIdentifier ),
        resolverParams );
        }
    else
        {
        interface = REComSession::CreateImplementationL(
               aUid,  _FOFF( CHssClientPlugin,
               iInstanceIdentifier ) );
        }
    return reinterpret_cast<CHssClientPlugin*>( interface );
    }
    
// ---------------------------------------------------------
//CHssClientPlugin::~CHssClientPlugin
// ---------------------------------------------------------
//
inline CHssClientPlugin::~CHssClientPlugin()
    {
    REComSession::DestroyedImplementation( iInstanceIdentifier );
    }

#endif // CHssClientPlugin_INL
            
// End of File
