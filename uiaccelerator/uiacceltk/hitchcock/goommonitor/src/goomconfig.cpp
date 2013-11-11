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


#include <e32hashtab.h>

#include "goomconfig.h"
#include "goomconstants.hrh"
#include "goomapplicationconfig.h"
#include "goomrunpluginconfig.h"
#include "goomcloseappconfig.h"
#include "goomtraces.h"

CGOomConfig* CGOomConfig::NewL()
    {
    FUNC_LOG;

    CGOomConfig* self = new (ELeave) CGOomConfig;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// Add the configuration for an application closure.
// This class takes ownership of this action.
void CGOomConfig::SetAppCloseConfigL(CGOomCloseAppConfig* aActionConfig)
    {
    FUNC_LOG;

    // Find the right application config (if there is one) for the app
    // The map actually contains pointers for values, so we get pointers to pointers here...
    CGOomApplicationConfig** applicationConfigPointer = iApplicationToConfigMapping.Find(aActionConfig->iId);
    
    // Used to de-reference the pointer-to-pointer, hopefully making the code more readable
    CGOomApplicationConfig* applicationConfig = NULL;
    
    // Create a new CGOomApplicationConfig if there isn't one for this app
    if (!applicationConfigPointer)
        {
        applicationConfig = CGOomApplicationConfig::NewL(aActionConfig->iId);
        iApplicationToConfigMapping.InsertL(aActionConfig->iId, applicationConfig);
        }
    else
        {
        applicationConfig = *applicationConfigPointer;
        }
    
    // Append the action config to the appropriate list (the list for the relevant application)
    applicationConfig->SetAppCloseConfig(aActionConfig);    
    }

// Add the configuration for a plugin action.
// This class takes ownership of the configuration object.
void CGOomConfig::AddPluginConfigL(CGOomRunPluginConfig* aPluginConfig)
    {
    FUNC_LOG;

    // Check if the plugin has already been added, if so then this is an error in configuration (something is trying to add the same plugin twice)
    CGOomRunPluginConfig** runPluginConfig = iPluginToConfigMapping.Find(aPluginConfig->Id());
    if (runPluginConfig)
        {
        //GOomMonitorPanic(KPluginConfigAddedTwice);
        while((*runPluginConfig)->iNextConfig != NULL)
            *runPluginConfig = (*runPluginConfig)->iNextConfig;
        
        (*runPluginConfig)->iNextConfig = aPluginConfig;
        return;
        }
    
    iPluginToConfigMapping.InsertL(aPluginConfig->Id(), aPluginConfig);
    
    }

// Add a rule
// This class takes ownership of the given rule
// This rule applies to the specified application (and not a plugin associated with this application)
// The rule would usually apply to an "application close" event
void CGOomConfig::AddApplicationRuleL(TUint aTargetAppId, MGOomRuleConfig* aRule)
    {
    FUNC_LOG;

    CGOomApplicationConfig** applicationConfig = iApplicationToConfigMapping.Find(aTargetAppId);
    
    if (applicationConfig)
        {
        (*applicationConfig)->AddRuleL(aRule);
        }
    else
        {
        GOomMonitorPanic(KRuleConfiguredBeforeApplication);
        }
    }

// Add a rule for a plugin
// This class takes ownership of the given rule
// This rule is applied to the plugin with the specified ID
void CGOomConfig::AddPluginRuleL(TUint aPluginId, MGOomRuleConfig* aRule)
    {
    FUNC_LOG;

    CGOomRunPluginConfig** runPluginConfig = iPluginToConfigMapping.Find(aPluginId);
    
    if (runPluginConfig)
        {
        (*runPluginConfig)->AddRuleL(aRule);
        }
    else
        {
        GOomMonitorPanic(KRuleConfiguredBeforePlugin);
        }   
    }

// Add this application config - this class takes ownership of it
// Application config includes settings for a particular application, e.g. whether or not it can be closed
void CGOomConfig::AddApplicationConfigL(CGOomApplicationConfig* aApplicationConfig)
    {
    FUNC_LOG;

    // Check if the application has already been added, if so then this is an error in configuration (something is trying to add the same app twice)
    CGOomApplicationConfig** applicationConfig = iApplicationToConfigMapping.Find(aApplicationConfig->Id());
    if (applicationConfig)
        {
        GOomMonitorPanic(KAppConfigAddedTwice);
        }
    
    iApplicationToConfigMapping.InsertL(aApplicationConfig->Id(), aApplicationConfig);
    }

// Get the list of configured actions for the given app id
// If no specific actions have been configured for this application then the default action is returned
CGOomApplicationConfig& CGOomConfig::GetApplicationConfig(TInt32 aAppId)
    {
    FUNC_LOG;

    CGOomApplicationConfig** applicationConfig = iApplicationToConfigMapping.Find(aAppId);
    
    if (!applicationConfig)
        applicationConfig = iApplicationToConfigMapping.Find(KGOomDefaultAppId);
    
    // The default app configuration should always exist
    __ASSERT_ALWAYS(applicationConfig, GOomMonitorPanic(KGOomDefaultAppNotConfigured));
    
    return *(*applicationConfig);
    }

// Get the plugin configuration for the given plugin id
// If no specific actions have been configured for this plugin then the default plugin configuration is returned
CGOomRunPluginConfig& CGOomConfig::GetPluginConfig(TInt32 aPluginId)
    {
    FUNC_LOG;

    CGOomRunPluginConfig** runPluginConfig = iPluginToConfigMapping.Find(aPluginId);
    
    if (!runPluginConfig)
        runPluginConfig = iPluginToConfigMapping.Find(KGOomDefaultPluginId);
    
    // The default app configuration should always exist
    __ASSERT_ALWAYS(runPluginConfig, GOomMonitorPanic(KGOomDefaultPluginNotConfigured));
    
    return *(*runPluginConfig);
    }

CGOomConfig::~CGOomConfig()
    {
    FUNC_LOG;

    // Iterate through the hash map deleting all of the items
    RHashMap<TInt32, CGOomApplicationConfig*>::TIter iterator(iApplicationToConfigMapping);
    while (iterator.NextValue())
        delete iterator.CurrentValue();
    
    // Iterate through the plugiun hash map deleting all of the items
    RHashMap<TInt32, CGOomRunPluginConfig*>::TIter pluginIterator(iPluginToConfigMapping);
    while (pluginIterator.NextValue())
        delete pluginIterator.CurrentValue();
    
    iApplicationToConfigMapping.Close();
    }

void CGOomConfig::ConstructL()
    {
    FUNC_LOG;
    }
