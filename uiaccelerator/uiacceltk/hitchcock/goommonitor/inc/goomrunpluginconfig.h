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
* Description:  Configuration representation classes for Graphics Out of Memory Monitor
*
*/


#ifndef GOOMRUNPLUGINCONFIG_
#define GOOMRUNPLUGINCONFIG_

#include "goomactionconfig.h"

enum TGOomPluginType
    {
    EGOomAppPlugin,
    EGOomSystemPlugin
    };

/*
 * The GOOM action of running an GOOM plug-in to free up memory.
 */
NONSHARABLE_CLASS(CGOomRunPluginConfig) : public CGOomActionConfig
    {
public:
    static CGOomRunPluginConfig* NewL(TUint aPluginId, TGOomPluginType aPluginType);
    
    TUint CalculatePluginPriority(const CGOomWindowGroupList& aWindowGroupList);
    
    inline TUint Id();
    
    inline void SetTargetApp(TUint aTargetAppId);
    
    inline TUint TargetApp() const;
    
    ~CGOomRunPluginConfig();
    
    // Returns ETrue if a wait period has been explicitly configured for this plugin
    inline TBool WaitAfterPluginDefined();
    
    // Returns the configured wait after the plugin has been called
    inline TInt WaitAfterPlugin();
    
    // Set the time to wait 
    inline void SetWaitAfterPlugin(TInt aMillisecondsToWait);
    
    inline TGOomPluginType PluginType();
    
    inline void SetSwRend(TBool aSwRend);
    
    inline TBool IsSwRendSupported();
    
    CGOomRunPluginConfig * iNextConfig;
    
private:
    CGOomRunPluginConfig(TUint aPluginId, TGOomPluginType aPluginType);
    
    TUint iPluginId;
    
    TUint iTargetAppId;
        
    TBool iWaitAfterPluginDefined;
    
    TInt iWaitAfterPlugin; // The period to wait after a plugin has been called
    
    TGOomPluginType iPluginType;
    
    TBool iSwRenderingSupported;
    };

#include "goomrunpluginconfig.inl"

#endif /*GOOMRUNPLUGINCONFIG_*/
