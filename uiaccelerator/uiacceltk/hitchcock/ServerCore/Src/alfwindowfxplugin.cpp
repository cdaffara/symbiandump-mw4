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
* Description: 
*
*/

#include "alfwindowfxplugin.h"

#include <e32base.h>

#include "alfwindowmanager.h"
#include "alfwindowstructs.h"

class CAlfWindowFxPlugin::TAlfFxPluginData
    {
    public:
        TUint iPriority;
        TUint iBufferFormat;
        CAlfWindowManager* iWindowMgr;
        TUid iIdentifier;
    };

EXPORT_C void CAlfWindowFxPlugin::ConstructL(const CAlfWindowManager& aMgr, TInt aAlfBufferFormat)
    {
    if (!iData)
        {
        iData = new (ELeave) TAlfFxPluginData;
        }
    iData->iBufferFormat = aAlfBufferFormat;
    iData->iWindowMgr = (CAlfWindowManager*)&aMgr;
    }

EXPORT_C CAlfWindowFxPlugin::~CAlfWindowFxPlugin()
    {
    if (iData)
        {
        delete iData;
        }
    }


EXPORT_C void CAlfWindowFxPlugin::NewWindow(const TWindowIdentifier& /*aId*/)
    {
    }

EXPORT_C void CAlfWindowFxPlugin::DestroyWindow(const TWindowIdentifier& /*aId*/)
    {
    }
    
EXPORT_C CAlfWindowManager* CAlfWindowFxPlugin::WindowManager()
    {
    if (!iData)
        {
        return NULL;
        }
    return iData->iWindowMgr;
    }

EXPORT_C void CAlfWindowFxPlugin::HandleMessageL(const TDesC8& /*inBuf*/, TPtr8& /*outBuf*/ )
    {
    // synchronous messages are completed by server 
    }

EXPORT_C void CAlfWindowFxPlugin::HandleMessageL(const TDesC8& /*inBuf*/, const RMessage2& aClientMessage )
    {
    // make sure that there are no pending aynch calls left here
    aClientMessage.Complete(KErrNotSupported);
    }


EXPORT_C void CAlfWindowFxPlugin::CancelMessage(TInt /*aMessageId*/)
    {
    // default implementation does not have pending messages
    }
    
TUint CAlfWindowFxPlugin::Priority()
    {
    if (iData)
        {
        return iData->iPriority;
        }
    return 0;
    }

TUid* CAlfWindowFxPlugin::Identifier()
    {
    if (iData)
        {
        return &iData->iIdentifier;
        }
    return 0;
    }


EXPORT_C void CAlfWindowFxPlugin::ClientAboutToExit(TThreadId)
    {
    }


