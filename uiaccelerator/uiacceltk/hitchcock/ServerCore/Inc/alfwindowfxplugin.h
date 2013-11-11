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

#ifndef __ALFWINDOWFXPLUGIN_H__
#define __ALFWINDOWFXPLUGIN_H__

#include <e32base.h>

class CAlfWindowManager;
struct TWindowIdentifier;

class CAlfWindowFxPlugin: public CBase
    {
    public:
    /** 
    * 2nd phase constructor. Called by framework to complete construction of a plugin. 
    * Must have base call from deriving class to complete construction
    */
    IMPORT_C virtual void ConstructL(const CAlfWindowManager& aMgr, TInt aAlfBufferFormat);

    IMPORT_C ~CAlfWindowFxPlugin();
    // not mandatory to implement
	IMPORT_C virtual void DestroyWindow(const TWindowIdentifier& aId);
    IMPORT_C virtual void NewWindow(const TWindowIdentifier& aId);
    /** Synch message from client */    
    IMPORT_C virtual void HandleMessageL(const TDesC8& aMessage, TPtr8& aResponse);

    /** Asynch message from client */    
    IMPORT_C virtual void HandleMessageL(const TDesC8& aMessage, const RMessage2& aClientMessage);

    /** Cancel asynch message(s) from client */    
    IMPORT_C virtual void CancelMessage(TInt aMessageId);

    /** Client about to exist, discard message(s) from client */    
    IMPORT_C virtual void ClientAboutToExit(TThreadId aClientId);

    /**
    * Pure Virtuals
    */
    virtual void PrepareFrameL(TUint aEstimatedFrameInterval) = 0;
    virtual void WindowUpdated(TInt aWindowId, TInt aEventType) = 0;
    
    // utils
    IMPORT_C CAlfWindowManager* WindowManager();

    // For FW use
    TUint Priority(); 

    TUid* Identifier(); 

    private:
        class TAlfFxPluginData;
        TAlfFxPluginData* iData;
    
    };


#endif