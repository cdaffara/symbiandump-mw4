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
* Description:   Inline functions of CHssMgmtClient class.
*
*/



#ifndef HSSMGMTCLIENT_INL
#define HSSMGMTCLIENT_INL

// ---------------------------------------------------------
// CHssScanRequest::NewL
// ---------------------------------------------------------
//
inline CHssMgmtClient* CHssMgmtClient::NewL()
    {
    const TUid KCHssMgmtClientUid = { 0x10282ece };

    TAny* interface = REComSession::CreateImplementationL(
        KCHssMgmtClientUid,
        _FOFF( CHssMgmtClient,
        iInstanceIdentifier ) );
    return reinterpret_cast<CHssMgmtClient*>( interface );
    }
    
// ---------------------------------------------------------
// CHssScanRequest::~CHssMgmtClient
// ---------------------------------------------------------
//
inline CHssMgmtClient::~CHssMgmtClient()
    {
    REComSession::DestroyedImplementation( iInstanceIdentifier );
    }

#endif // HSSMGMTCLIENT_INL
            
// End of File
