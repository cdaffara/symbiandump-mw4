/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ALF window server client extension implementation.
*
*/

#include "alfcrppluginclient.h"

// --------------------------------------------------------------------------
// CAlfCrpPluginClient::CAlfCrpPluginClient
// --------------------------------------------------------------------------
//
CAlfCrpPluginClient::CAlfCrpPluginClient()
    {
    }

// --------------------------------------------------------------------------
// CAlfCrpPluginClient::~CAlfCrpPluginClient
// --------------------------------------------------------------------------
//
CAlfCrpPluginClient::~CAlfCrpPluginClient()
    {
    }

// --------------------------------------------------------------------------
// CAlfCrpPluginClient::ConstructL
// --------------------------------------------------------------------------
//
void CAlfCrpPluginClient::ConstructL()
    {
    const TUid id = { 0x2002C358 };
    BaseConstructL( id, KNullDesC8 );
    }    

// --------------------------------------------------------------------------
// CAlfCrpPluginClient::Synchronize
// --------------------------------------------------------------------------
//    
void CAlfCrpPluginClient::Synchronize()
    {
    TPckgC<TInt> buf(0);
    SendMessage(buf);
    Flush();
    }

// --------------------------------------------------------------------------
// CAlfCrpPluginClient::HandleMessage
// --------------------------------------------------------------------------
//    
void CAlfCrpPluginClient::HandleMessage(const TDesC8& /*aData*/)
    {
    }
    
// --------------------------------------------------------------------------
// CAlfCrpPluginClient::OnReplace
// --------------------------------------------------------------------------
//    
void CAlfCrpPluginClient::OnReplace()
    {
    }

// End of file

