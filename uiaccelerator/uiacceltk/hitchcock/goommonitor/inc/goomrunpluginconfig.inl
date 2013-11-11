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


#ifndef GOOMRUNPLUGINCONFIGINL_H_
#define GOOMRUNPLUGINCONFIGINL_H_


// Returns ETrue if a wait period has been explicitly configured for this plugin
inline TBool CGOomRunPluginConfig::WaitAfterPluginDefined()
    {
    return iWaitAfterPluginDefined;
    }

// Returns the configured wait after the plugin has been called
inline TInt CGOomRunPluginConfig::WaitAfterPlugin()
    {
    return iWaitAfterPlugin;
    }

// Set the time to wait 
inline void CGOomRunPluginConfig::SetWaitAfterPlugin(TInt aMillisecondsToWait)
    {
    iWaitAfterPluginDefined = ETrue;
    iWaitAfterPlugin = aMillisecondsToWait;
    }


inline void CGOomRunPluginConfig::SetTargetApp(TUint aTargetAppId)
    {
    iTargetAppId = aTargetAppId;
    }

inline TUint CGOomRunPluginConfig::Id()
    {
    return  iPluginId;
    }

inline TGOomPluginType CGOomRunPluginConfig::PluginType()
    {
    return iPluginType;
    }

inline TUint CGOomRunPluginConfig::TargetApp() const
    {
    return iTargetAppId;
    }

inline void CGOomRunPluginConfig::SetSwRend(TBool aSwRend)
    {
    iSwRenderingSupported = aSwRend;
    }

inline TBool CGOomRunPluginConfig::IsSwRendSupported()
    {
    return iSwRenderingSupported;
    }
#endif /*GOOMRUNPLUGINCONFIGINL_H_*/
