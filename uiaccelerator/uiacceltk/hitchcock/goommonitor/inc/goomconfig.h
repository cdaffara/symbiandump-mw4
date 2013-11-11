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


#ifndef GOOMCONFIG_
#define GOOMCONFIG_

#include <e32base.h>

#include "goomglobalconfig.h"

class CGOomCloseAppConfig;
class CGOomRunPluginConfig;
class MGOomRuleConfig;
class CGOomApplicationConfig;

/*
 * A class representing the entire configuration.
 */
NONSHARABLE_CLASS(CGOomConfig) : public CBase
    {
public:
    
    static CGOomConfig* NewL();
    
// Functions for setting configuration  
    
    // Add the configuration for an application closure.
    // This class takes ownership of this configuration object.
    void SetAppCloseConfigL(CGOomCloseAppConfig* aActionConfig);
    
    // Add the configuration for a plugin action.
    // This class takes ownership of the configuration object.
    void AddPluginConfigL(CGOomRunPluginConfig* aPluginConfig);
    
    // Add a rule for an application
    // This class takes ownership of the given rule
    // This rule applies to the specified application (and not a plugin associated with this application)
    // The rule would usually apply to an "application close" event
    void AddApplicationRuleL(TUint aTargetAppId, MGOomRuleConfig* aRule);
    
    // Add a rule for a plugin
    // This class takes ownership of the given rule
    // This rule is applied to the plugin with the specified ID
    void AddPluginRuleL(TUint aPluginId, MGOomRuleConfig* aRule);    
    
    // Add this application config - this class takes ownership of it
    // Application config includes settings for a particular application, e.g. whether or not it can be closed
    void AddApplicationConfigL(CGOomApplicationConfig* aApplicationConfig);
    
// Functions for getting configuration  
    
    // Get the application configuration for the given app id
    // If no specific actions have been configured for this application then the default application configuration is returned
    CGOomApplicationConfig& GetApplicationConfig(TInt32 aAppId);
    
    // Get the plugin configuration for the given plugin id
    // If no specific actions have been configured for this plugin then the default plugin configuration is returned
    CGOomRunPluginConfig& GetPluginConfig(TInt32 aPluginId);
        
// Functions for setting global configuration   
    
    ~CGOomConfig();
    
    inline CGOomGlobalConfig& GlobalConfig();
    
    inline void SetDefaultLowRamThreshold(TInt aLowRamThreshold);
    inline void SetDefaultGoodRamThreshold(TInt aGoodRamThreshold);
    inline void SetMaxCloseAppBatch(TUint MaxCloseAppBatch);
    inline void SetDefaultWaitAfterPlugin(TInt aMilliseconds);
    
private:
    void ConstructL();
    
    RHashMap<TInt32, CGOomApplicationConfig*> iApplicationToConfigMapping; // A hash-map of application configs, keyed on the application ID
    
    RHashMap<TInt32, CGOomRunPluginConfig*> iPluginToConfigMapping; // A hash-map of plug-in configs, keyed on the plugin ID

    CGOomGlobalConfig iGlobalConfig;
    };

#include "goomconfig.inl"

#endif /*GOOMCONFIG_*/
