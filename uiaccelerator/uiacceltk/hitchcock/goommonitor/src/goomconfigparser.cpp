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
* Description:  The parser for the Graphics OOM configuration file
*
*/


#include <e32base.h>
#include <xml/parser.h>

#include "goomconfigparser.h"
#include "goompanic.h"
#include "goomtraces.h"
#include "goomidletimerule.h"
#include "goomforegroundrule.h"
#include "goomconstants.hrh"
#include "goomapplicationconfig.h"
#include "goomcloseappconfig.h"
#include "goomconfig.h"
#include "goomrunpluginconfig.h"

enum
{
KGOomErrMoreThanOneGOomConfig = 0,
KGOomErrGlobalSettingsMustComeAfterRoot,
KGOomErrAppSettingsMustComeAfterGlobalSettings,
KGOomErrCloseAppSettingsMustComeAfterAppSettings,
KGOomErrAppIdleSettingsMustComeAfterAppCloseSettings,
KGOomErrLowRamErrorInGlobalSettings,
KGOomErrGoodRamErrorInGlobalSettings,
KGOomErrMaxCloseErrorInGlobalSettings,
KGOomErrDefaultPriorityErrorInGlobalSettings,
KGOomErrMissingUidFromAppCloseConfig,
KGOomErrMissingPriorityFromAppCloseConfig,
KGOomErrMissingSyncModeFromAppCloseConfig,
KGOomErrMissingEstimateFromAppCloseConfig,
KGOomErrInvalidSyncMode,
KGOomErrMissingSyncModeInAppCloseConfig,
KGOomErrBadOrMissingPriorityInAppIdleRule,
KGOomErrBadOrMissingIdleTimeInAppIdleRule,
KGOomErrBadOrMissingUidInAppIdleRule,
KGOomErrBadNeverCloseValue,
KGOomErrBadOrMissingUidInAppConfig,
KGOomErrBadOrMissingPriorityInAppCloseConfig,
KGOomErrBadLowThresholdValueForAppConfig,
KGOomErrBadGoodThresholdValueForAppConfig,
KGOomErrBadTargetFreeValueForAppConfig,
KGOomErrBadSkipPluginValueForAppConfig,
KGOomErrBadUseSwRenderingValueForAppConfig,
KGOomErrSystemPluginSettingsMustComeAfterAppCloseSettings,
KGOomErrAppPluginSettingsMustComeAfterSystemPluginSettings,
KGOomErrAppPluginIdleTimeRulesMustComeAfterAppPluginSettings,
KGOomErrBadOrMissingUidInAppCloseConfig,
KGOomErrBadOrMissingUidInSystemPluginConfig,
KGOomErrBadOrMissingPriorityInSystemPluginConfig,
KGOomErrBadOrMissingTargetAppIdInAppPluginConfig,
KGOomErrBadOrMissingUidInAppPluginConfig,
KGOomErrBadOrMissingPriorityInAppPluginConfig,
KGOomErrBadOrMissingPriorityInPluginIdleRule,
KGOomErrBadOrMissingIdleTimeInPluginIdleRule,
KGOomErrBadOrMissingUidInPluginIdleRule,
KGOomErrBadOrMissingUidInForegroundAppRule,
KGOomErrBadOrMissingPriorityInForegroundAppRule,
KGOomErrBadOrMissingTargetAppIdInForegroundAppRule,
KGOomErrDefaultWaitAfterPluginInGlobalSettings,
KGOomErrBadOrMissingPriorityInForceCheck,
KGOomErrGOomRulesMustComeLast,
KGOomErrBadPluginWaitTime,
KGOomErrBadXml,
KGOomErrAppCloseIdleRuleOutsideAppCloseElement,
KGOomErrForegroundAppRuleOutsideAppCloseElement,
KGOomErrPluginIdleRuleOutsideAppPluginElement,
KGOomErrPluginForegroundRuleOutsidePluginElement,
KGOomErrInvalidSwRendConfig
};


const TInt KGOomXmlFileBufferSize = 1024;

using namespace Xml;

// Mime type of the parsed document
_LIT8(KXmlMimeType, "text/xml");

_LIT(KGOomConfigFilePath, ":\\private\\10207218\\goomconfig.xml");
_LIT(KRomDrive, "z");

// Element strings
// Root
_LIT8(KGOomConfigGOomConfig, "goom_config");

// Global settings
_LIT8(KGOomConfigGlobalSettings, "global_settings");
_LIT8(KGOomConfigForceCheckAtPriority, "force_check");

// App settings
_LIT8(KGOomConfigAppSettings, "app_specific_thresholds");
_LIT8(KGOomConfigApp, "app");

// App close settings
_LIT8(KGOomConfigAppCloseSettings, "app_close_settings");
_LIT8(KGOomConfigCloseApp, "close_app");

_LIT8(KGOomAttributeAppCloseTimeout, "close_timeout");
_LIT8(KGOomAttributeAppWaitAfterClose, "wait_after_close");

// App close idle time
_LIT8(KGOomConfigAppCloseIdlePriority, "app_close_idle_priority");

_LIT8(KGOomConfigForegroundAppPriority, "foreground_app_priority");

// Global settings attribute names
_LIT8(KGOomAttributeLowRamThreshold, "low_ram_threshold");
_LIT8(KGOomAttributeGoodRamThreshold, "good_ram_threshold");
_LIT8(KGOomAttributeMaxAppCloseBatch, "max_app_close_batch");
_LIT8(KGOomAttributeDefaultWaitAfterPlugin, "default_wait_after_plugin");

//App specific
_LIT8(KGOomAttributeTargetFreeOnStartup, "target_free_on_startup");
_LIT8(KGOomAttributeSkipPlugin, "skip_plugin");
_LIT8(KGOomAttributeUseSwRendering, "use_sw_rend");
_LIT8(KGOomConfigUseSwRend, "Yes");
_LIT8(KGOomConfigNotUseSwRend, "No");

// System plugins 

_LIT8(KGOomAttributeSystemPluginSettings, "system_plugin_settings");
_LIT8(KGOomAttributeSystemPlugin, "system_plugin");

// Application plugins

_LIT8(KGOomAttributeAppPluginSettings, "app_plugin_settings");
_LIT8(KGOomAttributeAppPlugin, "app_plugin");

// Plugin idle time rules

_LIT8(KGOomAttributePluginIdlePriority, "plugin_idle_priority");

// Plugin foreground app rules
_LIT8(KGOomAttributePluginForegroundAppPriority, "plugin_foreground_app_priority");

// Atribute names
_LIT8(KGOomAttibuteUid, "uid");
_LIT8(KGOomAttibuteSyncMode, "sync_mode");
_LIT8(KGOomAttibutePriority, "priority");
_LIT8(KGOomAttibuteRamEstimate, "ram_estimate");

_LIT8(KGOomConfigSyncModeContinue, "continue");
_LIT8(KGOomConfigSyncModeCheck, "check");
_LIT8(KGOomConfigSyncModeEstimate, "estimate");

_LIT8(KGOomAttibuteIdleTime, "idle_time");
_LIT8(KGOomAttibuteIdlePriority, "priority");

_LIT8(KGOomAttibuteNeverClose, "NEVER_CLOSE");

_LIT8(KGOomAttributeTargetAppId, "target_app_id");

_LIT8(KGOomAttributeWait, "wait");

_LIT8(KGOomAttributeIfForegroundAppId, "if_foreground_app_id");

_LIT8(KGOomConfigDefaultAppUid, "DEFAULT_APP");
_LIT8(KGOomConfigDefaultPluginUid, "DEFAULT_PLUGIN");
_LIT8(KGOomConfigTargetAppValue, "TARGET_APP");

_LIT8(KGOomConfigBusyAppUid, "BUSY_APP");
_LIT8(KGOomConfigHighPriorityAppUid, "HIGH_PRIORITY_APP");

_LIT8(KGOomAttibuteSwRend, "sw_rend");
_LIT8(KGOomConfigSwRendSupported, "supported");
_LIT8(KGOomConfigSwRendNotSupported, "not_supported");

CGOomConfigParser::CGOomConfigParser(CGOomConfig& aConfig, RFs& aFs) : iConfig(aConfig), iFs(aFs), iState(EGOomParsingStateNone)
    {
    }

void CGOomConfigParser::ParseL()
    {
    FUNC_LOG;

    TRACES("CGOomConfigParser::ParseL: Parsing Config File");
    
    CParser* parser = CParser::NewLC(KXmlMimeType, *this);
    
    RFile configFile;
    TFileName configFileName;
    TChar driveChar = iFs.GetSystemDriveChar();
    configFileName.Append(driveChar);
    configFileName.Append(KGOomConfigFilePath);
    if (configFile.Open(iFs, configFileName, EFileShareExclusive) != KErrNone)
        {
        configFileName.Replace(0,1,KRomDrive); //replace 'c' with 'z'
        User::LeaveIfError(configFile.Open(iFs, configFileName, EFileShareExclusive));
        }
    CleanupClosePushL(configFile);
    
    TBuf8<KGOomXmlFileBufferSize> fileBuffer;
    TInt bytesRead;
    do
        {
        User::LeaveIfError(configFile.Read(fileBuffer));
        bytesRead = fileBuffer.Size();
        
        parser->ParseL(fileBuffer);
        
        } while (bytesRead != 0);
    
    CleanupStack::PopAndDestroy(2, parser); // config file - automatically closes it
                                            // parser
            
    TRACES("CGOomConfigParser::ParseL: Finished Parsing Config File");    
    }

void CGOomConfigParser::OnStartDocumentL(const RDocumentParameters&, TInt)
    {
    FUNC_LOG;
    }

void CGOomConfigParser::OnEndDocumentL(TInt)
    {
    FUNC_LOG;
    }


void CGOomConfigParser::OnEndElementL(const RTagInfo&, TInt)
    {
    }

void CGOomConfigParser::OnContentL(const TDesC8&, TInt)
    {
    }

void CGOomConfigParser::OnStartPrefixMappingL(const RString&, const RString&, 
                                   TInt)
    {
    }

void CGOomConfigParser::OnEndPrefixMappingL(const RString&, TInt)
    {
    }

void CGOomConfigParser::OnIgnorableWhiteSpaceL(const TDesC8&, TInt)
    {
    }

void CGOomConfigParser::OnSkippedEntityL(const RString&, TInt)
    {
    }

void CGOomConfigParser::OnProcessingInstructionL(const TDesC8&, const TDesC8&, 
                                      TInt)
    {
    }

void CGOomConfigParser::OnError(TInt)
    {
    }

TAny* CGOomConfigParser::GetExtendedInterface(const TInt32)
    {
    return 0;
    }

void CGOomConfigParser::OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, 
                             TInt aErrorCode)
    {
    if (aErrorCode != KErrNone)
        ConfigError(KGOomErrBadXml);
    
    StartElementL(aElement.LocalName().DesC(), aAttributes);
    }

TInt CGOomConfigParser::StartElementL(const TDesC8& aLocalName,
                                        const RAttributeArray& aAttributes)
    {
    // Root
    if (aLocalName == KGOomConfigGOomConfig)
        {
        if (iState != EGOomParsingStateNone)
            ConfigError(KGOomErrMoreThanOneGOomConfig);
        
        ChangeState(EGOomParsingStateRoot);
        }
    // Parse main elements
    else if (aLocalName == KGOomConfigGlobalSettings)
        {
        if (iState != EGOomParsingStateRoot)
            ConfigError(KGOomErrGlobalSettingsMustComeAfterRoot);
        
        SetGlobalSettings(aAttributes);
        
        ChangeState(EGOomParsingStateGlobalSettings);
        }
    else if (aLocalName == KGOomConfigAppSettings)
        {
        ChangeState(EGOomParsingStateAppSettings);
        }
    else if (aLocalName == KGOomConfigAppCloseSettings)
        {
        ChangeState(EGOomParsingStateAppCloseSettings);
        }
    else if (aLocalName == KGOomAttributeSystemPluginSettings)
        {
        ChangeState(EGOomParsingStateSystemPluginSettings);
        }
    else if (aLocalName == KGOomAttributeAppPluginSettings)
        {
        ChangeState(EGOomParsingStateAppPluginSettings);
        }
    // Parse actual configuration elements
    else if (aLocalName == KGOomConfigForceCheckAtPriority)
        {
        SetForceCheckConfigL(aAttributes);
        }
    else if (aLocalName == KGOomConfigApp)
        {
        SetAppConfigL(aAttributes);
        }
    else if (aLocalName == KGOomConfigCloseApp)
        {
        SetCloseAppConfigL(aAttributes);
        }
    else if (aLocalName == KGOomConfigAppCloseIdlePriority)
        {
        CheckState(EGOomParsingStateAppCloseSettings, KGOomErrAppCloseIdleRuleOutsideAppCloseElement);
        SetAppCloseIdlePriorityConfigL(aAttributes);
        }
    else if (aLocalName == KGOomConfigForegroundAppPriority)
        {
        CheckState(EGOomParsingStateAppCloseSettings, KGOomErrForegroundAppRuleOutsideAppCloseElement);
        SetForegroundAppPriorityL(aAttributes);
        }
    else if (aLocalName == KGOomAttributeSystemPlugin)
        {
        SetSystemPluginConfigL(aAttributes);
        }
    else if (aLocalName == KGOomAttributeAppPlugin)
        {
        SetAppPluginConfigL(aAttributes);
        }
    else if (aLocalName == KGOomAttributePluginIdlePriority)
        {
        CheckState(EGOomParsingStateAppPluginSettings, KGOomErrPluginIdleRuleOutsideAppPluginElement);
        SetPluginIdlePriorityL(aAttributes);
        }
    else if (aLocalName == KGOomAttributePluginForegroundAppPriority)
        {
        CheckState(EGOomParsingStateAppPluginSettings, EGOomParsingStateSystemPluginSettings, KGOomErrPluginForegroundRuleOutsidePluginElement);
        SetPluginForegroundAppPriorityL(aAttributes);
        }
    
    return KErrNone;
    }

void CGOomConfigParser::ConfigError(TInt aError)
    {
    GOomConfigParserPanic(aError);
    }

void CGOomConfigParser::SetGlobalSettings(const RAttributeArray& aAttributes)
    {
    TInt defaultLowMemoryThreshold;    
    TInt err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttributeLowRamThreshold, defaultLowMemoryThreshold);

    if (err == KErrNone)
        iConfig.SetDefaultLowRamThreshold(defaultLowMemoryThreshold * 1024);
    else
        ConfigError(KGOomErrLowRamErrorInGlobalSettings);
    
    if (err == KErrNone)
        {
        TInt defaultGoodMemoryThreshold;    
        TInt err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttributeGoodRamThreshold, defaultGoodMemoryThreshold);

        if (err == KErrNone)
            iConfig.SetDefaultGoodRamThreshold(defaultGoodMemoryThreshold * 1024);
        else
            ConfigError(KGOomErrGoodRamErrorInGlobalSettings);
        }

    if (err == KErrNone)
        {
        TInt defaultMaxCloseAppBatch;    
        TInt err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttributeMaxAppCloseBatch, defaultMaxCloseAppBatch);

        if (err == KErrNone)
            iConfig.SetMaxCloseAppBatch(defaultMaxCloseAppBatch);
        else
            ConfigError(KGOomErrMaxCloseErrorInGlobalSettings);
        }
    
    if (err == KErrNone)
        {
        TInt defaultWaitAfterPlugin;    
        TInt err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttributeDefaultWaitAfterPlugin, defaultWaitAfterPlugin);

        if (err == KErrNone)
            iConfig.SetDefaultWaitAfterPlugin(defaultWaitAfterPlugin);
        else
            ConfigError(KGOomErrDefaultWaitAfterPluginInGlobalSettings);
        }
    }

void CGOomConfigParser::SetForceCheckConfigL(const RAttributeArray& aAttributes)
    {
    TUint priority;
    TInt err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttibutePriority, priority);
    if (err == KErrNone)
        {
        iConfig.GlobalConfig().AddForceCheckPriorityL(priority);
        }
    else
        {
        ConfigError(KGOomErrBadOrMissingPriorityInForceCheck);    
        }
    }

void CGOomConfigParser::SetAppConfigL(const RAttributeArray& aAttributes)
    {
    TUint uid;
    CGOomApplicationConfig* appConfig = NULL;
    
    TInt err = GetValueFromHexAttributeList(aAttributes, KGOomAttibuteUid, uid);
    
    if (err != KErrNone)
        {
        ConfigError(KGOomErrBadOrMissingUidInAppConfig);
        }
    else
        iParentUid = uid;

    appConfig = CGOomApplicationConfig::NewL(uid);
    CleanupStack::PushL(appConfig);    
        
    // Set the app specific memory thresholds (if they exist)
    // Get the app specific low threshold
    if (err == KErrNone)
        {
        TUint lowThreshold;
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttributeLowRamThreshold, lowThreshold);
        if (err == KErrNone)
            {
            appConfig->iLowRamThreshold = lowThreshold * 1024; // The config files work in K, so we need to multiply by 1024
            }
        else if (err == KErrNotFound)
            err = KErrNone;
        
        if (err != KErrNone)
            ConfigError(KGOomErrBadLowThresholdValueForAppConfig);
        }

    // Get the app specific good threshold
    if (err == KErrNone)
        {
        TUint goodThreshold;
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttributeGoodRamThreshold, goodThreshold);
        if (err == KErrNone)
            {
            appConfig->iGoodRamThreshold = goodThreshold * 1024; // The config files work in K, so we need to multiply by 1024
            }
        else if (err == KErrNotFound)
            err = KErrNone;
        
        if (err != KErrNone)
            ConfigError(KGOomErrBadGoodThresholdValueForAppConfig);
        }
    
    // Get the app specific TargetFreeOnStartup
        if (err == KErrNone)
            {
            TUint targetFree;
            err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttributeTargetFreeOnStartup, targetFree);
            if (err == KErrNone)
                {
                appConfig->iTargetFree = targetFree * 1024; // The config files work in K, so we need to multiply by 1024
                }
            else if (err == KErrNotFound)
                err = KErrNone;
            
            if (err != KErrNone)
                ConfigError(KGOomErrBadTargetFreeValueForAppConfig);
            }
        
    // Get the app specific SkipPlugin
        if (err == KErrNone)
            {
            TUint skipPlugin;
            err = GetValueFromHexAttributeList(aAttributes, KGOomAttributeSkipPlugin, skipPlugin);
            if (err == KErrNone)
                {
                TRACES2("SKIP PLUGIN %x configured for App %x", skipPlugin, uid); 
                appConfig->iSkipPluginId = skipPlugin;
                }
            else if (err == KErrNotFound)
                err = KErrNone;
            
            if (err != KErrNone)
                ConfigError(KGOomErrBadSkipPluginValueForAppConfig);
            }
                
        // Get the software rendering config
        if (err == KErrNone)
            {
            TPtrC8 swrendString;
            TInt err = GetValueFromAttributeList(aAttributes, KGOomAttributeUseSwRendering, swrendString);
            if (err == KErrNone)
                {
                if (swrendString == KGOomConfigUseSwRend)
                    {
                    TRACES1("Sw Rend configured for App %x", uid); 
                    appConfig->iUseSwRendering = ETrue;
                    }
                else
                    {
                    appConfig->iUseSwRendering = EFalse;
                    }
                }
            else if (err == KErrNotFound)
                {
                err = KErrNone;
                appConfig->iUseSwRendering = EFalse;
                }
            
            if (err != KErrNone)
                ConfigError(KGOomErrBadUseSwRenderingValueForAppConfig);
            }
        
    // Add the applciation config to the main config
    if ((err == KErrNone) && (appConfig))
        {
        iConfig.AddApplicationConfigL(appConfig);
        }
    
    if (appConfig)
        CleanupStack::Pop(appConfig);
    }

void CGOomConfigParser::SetCloseAppConfigL(const RAttributeArray& aAttributes)
    {
    // Get and convert uid attribute to TInt
    TInt err = KErrNone;
    
    TUint uid;
    err = GetValueFromHexAttributeList(aAttributes, KGOomAttibuteUid, uid);

    if (err != KErrNone)
        {
        ConfigError(KGOomErrBadOrMissingUidInAppCloseConfig);    
        return;
        }
    else
        iParentUid = uid;
    
    CGOomCloseAppConfig* closeAppConfig = CGOomCloseAppConfig::NewL(uid); // Radio UID
    CleanupStack::PushL(closeAppConfig);

     if (err == KErrNone)
        {
        // Check that we have a priority for the added app_close event
        // Specifying a priority is mandatory
        TUint priority;
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttibutePriority, priority);
        if (err == KErrNone)
            {
            closeAppConfig->SetDefaultPriority(priority);
            }
        else
            {
            ConfigError(KGOomErrBadOrMissingPriorityInAppCloseConfig);    
            }
        }
    
    if (err == KErrNone)
        {
        TPtrC8 syncModeString;
        err = GetValueFromAttributeList(aAttributes, KGOomAttibuteSyncMode, syncModeString);
        
        if (err == KErrNone)
            {
            TGOomSyncMode syncMode = EContinue;
            
            if (syncModeString == KGOomConfigSyncModeContinue)
                syncMode = EContinue;
            else if (syncModeString == KGOomConfigSyncModeCheck)
                syncMode = ECheckRam;
            else if (syncModeString == KGOomConfigSyncModeEstimate)
                syncMode = EEstimate;
            else
                ConfigError(KGOomErrInvalidSyncMode);
            
            if (err == KErrNone)
                {
                closeAppConfig->iSyncMode = syncMode;
                }
            }
        else
            {
            ConfigError(KGOomErrMissingSyncModeInAppCloseConfig);
            }
        }
    
    
    if (err == KErrNone)
        {
        // If we have a default priority attribute then add it, otherwise use the global default priority
        TInt ramEstimate;
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttibuteRamEstimate, ramEstimate);
        if ((err == KErrNotFound) && (closeAppConfig->iSyncMode != EEstimate))
            {
            err = KErrNone;
            }
        
        if (err != KErrNone)
            ConfigError(KGOomErrMissingEstimateFromAppCloseConfig);
        else
            closeAppConfig->iRamEstimate = ramEstimate * 1024;
        }
    
    if (err == KErrNone)
        {
        TInt closeTimeout;
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttributeAppCloseTimeout, closeTimeout);
        if (err == KErrNone)
            {
            closeAppConfig->iCloseTimeout = closeTimeout;
            }
        else if (err == KErrNotFound)
            {
            closeAppConfig->iCloseTimeout=0;
            err = KErrNone;
            }
        }
     
    if (err == KErrNone)
        {
        TInt waitAfterClose;
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttributeAppWaitAfterClose, waitAfterClose);
        if (err == KErrNone)
            {
            closeAppConfig->iWaitAfterClose = waitAfterClose;
            }
        else if (err == KErrNotFound)
            {
            err = KErrNone;
            closeAppConfig->iWaitAfterClose = 0;
            }
        }

    if (err == KErrNone)
        {
         iConfig.SetAppCloseConfigL(closeAppConfig);
        }
    else
        {
        TRACES2("ERROR Creating Appcloseconfig file for %x, err %d", uid,err);
        }
    
    CleanupStack::Pop(closeAppConfig);
    }

void CGOomConfigParser::SetAppCloseIdlePriorityConfigL(const RAttributeArray& aAttributes)
    {
    TUint uid;
    TInt idleTime;
    TUint priority;

    // Use the UID from the parent scope
    uid = iParentUid;

    TInt err = KErrNone;
    
    err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttibuteIdleTime, idleTime);
    
#ifdef __WINS__
    // The tick is 5 times slower on the emulator than on the phone
    idleTime = idleTime / 5;
#endif
    
    if (err != KErrNone)
        {
        ConfigError(KGOomErrBadOrMissingIdleTimeInAppIdleRule);
        }
    
    if (err == KErrNone)
        {
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttibuteIdlePriority, priority);
        
        if (err != KErrNone)
            {
            ConfigError(KGOomErrBadOrMissingPriorityInAppIdleRule);
            }
        }
    
    if (err == KErrNone)
        {
        CGOomIdleTimeRule* idleRule = CGOomIdleTimeRule::NewL(idleTime, priority);
        CleanupStack::PushL(idleRule);
        iConfig.AddApplicationRuleL(uid, idleRule);
        CleanupStack::Pop(idleRule);
        }
    }

void CGOomConfigParser::SetForegroundAppPriorityL(const RAttributeArray& aAttributes)
    {
    TUint appUid;
    TUint targetAppId;
    TUint priority;

    TInt err = KErrNone;
    
    // Use the UID from the parent scope
    appUid = iParentUid;

    // Check that we have a priority for the added system plugin action
    // Specifying a priority is mandatory
    err = GetValueFromHexAttributeList(aAttributes, KGOomAttributeIfForegroundAppId, targetAppId);
    if (err != KErrNone)
        {
        ConfigError(KGOomErrBadOrMissingTargetAppIdInForegroundAppRule);    
        }
    
    if (err == KErrNone)
        {
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttibuteIdlePriority, priority);
        
        if (err != KErrNone)
            {
            ConfigError(KGOomErrBadOrMissingPriorityInForegroundAppRule);
            }
        }
    
    if (err == KErrNone)
        {
        CGOomForegroundRule* foregroundRule = new (ELeave) CGOomForegroundRule(targetAppId, priority);
        CleanupStack::PushL(foregroundRule);
        iConfig.AddApplicationRuleL(appUid, foregroundRule);
        CleanupStack::Pop(foregroundRule);
        }

    }

void CGOomConfigParser::SetSystemPluginConfigL(const RAttributeArray& aAttributes)
    {
    // Get and convert uid attribute to TInt
    TInt err = KErrNone;
    
    TUint uid;
    err = GetValueFromHexAttributeList(aAttributes, KGOomAttibuteUid, uid);

    if (err != KErrNone)
        {
        ConfigError(KGOomErrBadOrMissingUidInSystemPluginConfig);    
        return;
        }
    else
        iParentUid = uid;
    
    CGOomRunPluginConfig* pluginConfig = CGOomRunPluginConfig::NewL(uid, EGOomSystemPlugin);
    CleanupStack::PushL(pluginConfig);

     if (err == KErrNone)
        {
        // Check that we have a priority for the added system plugin action
        // Specifying a priority is mandatory
        TUint priority;
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttibutePriority, priority);
        if (err == KErrNone)
            {
            pluginConfig->SetDefaultPriority(priority);
            }
        else
            {
            ConfigError(KGOomErrBadOrMissingPriorityInSystemPluginConfig);    
            }
        }
     
     if (err == KErrNone)
        {
        TInt wait;
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttributeWait, wait);
        if (err == KErrNone)
            {
            pluginConfig->SetWaitAfterPlugin(wait);
            }
        else if (err == KErrNotFound)
            {
            // If this attribute isn't present then just don't set it, and clear the error
            err = KErrNone;
            }
        else
            ConfigError(KGOomErrBadPluginWaitTime);
        }
     
     if (err == KErrNone)
         {
         // Get the config for the sync mode for this plugin (if one is specified) and set it
         SetPluginSyncMode(aAttributes, *pluginConfig);
        
         TPtrC8 swrendString;
         TInt err = GetValueFromAttributeList(aAttributes, KGOomAttibuteSwRend, swrendString);
         if (err == KErrNone)
             {
             if (swrendString == KGOomConfigSwRendSupported)
                 pluginConfig->SetSwRend(ETrue);
             else if (swrendString == KGOomConfigSwRendNotSupported)
                 pluginConfig->SetSwRend(EFalse);
             else
                 ConfigError(KGOomErrInvalidSwRendConfig);
             }
         
         }
          
     iConfig.AddPluginConfigL(pluginConfig);
     
     CleanupStack::Pop(pluginConfig);
    }

void CGOomConfigParser::SetAppPluginConfigL(const RAttributeArray& aAttributes)
    {
    // Get and convert uid attribute to TInt
    TInt err = KErrNone;
    
    TUint uid;
    err = GetValueFromHexAttributeList(aAttributes, KGOomAttibuteUid, uid);

    if (err != KErrNone)
        {
        ConfigError(KGOomErrBadOrMissingUidInAppPluginConfig);    
        return;
        }
    else
        iParentUid = uid;
    
    CGOomRunPluginConfig* pluginConfig = CGOomRunPluginConfig::NewL(uid, EGOomAppPlugin);
    CleanupStack::PushL(pluginConfig);

    if (err == KErrNone)
       {
       // Check that we have a priority for the added system plugin action
       // Specifying a priority is mandatory
        TUint priority;
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttibutePriority, priority);
        if (err == KErrNone)
            {
            pluginConfig->SetDefaultPriority(priority);
            }
        else
            {
            ConfigError(KGOomErrBadOrMissingPriorityInAppPluginConfig);    
            }
       }

    if (err == KErrNone)
       {
       // Check that we have a priority for the added system plugin action
       // Specifying a priority is mandatory
        TUint targetAppId;
        err = GetValueFromHexAttributeList(aAttributes, KGOomAttributeTargetAppId, targetAppId);
        if (err == KErrNone)
            {
            pluginConfig->SetTargetApp(targetAppId);
            iParentTargetApp = targetAppId;
            }
        else
            {
            ConfigError(KGOomErrBadOrMissingTargetAppIdInAppPluginConfig);    
            }
       }
    
    if (err == KErrNone)
       {
       TInt wait;
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttributeWait, wait);
        if (err == KErrNone)
            {
            pluginConfig->SetWaitAfterPlugin(wait);
            }
        else if (err == KErrNotFound)
            {
            // If this attribute isn't present then just don't set it, and clear the error
            err = KErrNone;
            }
        else
            ConfigError(KGOomErrBadPluginWaitTime);
       }    
    
    if (err == KErrNone)
        {
        // Get the config for the sync mode for this plugin (if one is specified) and set it
        SetPluginSyncMode(aAttributes, *pluginConfig);
        }

     iConfig.AddPluginConfigL(pluginConfig);
     
     CleanupStack::Pop(pluginConfig);

    }

void CGOomConfigParser::SetPluginIdlePriorityL(const RAttributeArray& aAttributes)
    {
    TUint uid;
    TInt idleTime;
    TUint priority;

    TInt err = KErrNone;
    
    // Use the UID from the parent scope
    uid = iParentUid;

    err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttibuteIdleTime, idleTime);
    
#ifdef __WINS__
    // The tick is 5 times slower on the emulator than on the phone
    idleTime = idleTime / 5;
#endif
    
    if (err != KErrNone)
        {
        ConfigError(KGOomErrBadOrMissingIdleTimeInPluginIdleRule);
        }
    
    if (err == KErrNone)
        {
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttibuteIdlePriority, priority);
        
        if (err != KErrNone)
            {
            ConfigError(KGOomErrBadOrMissingPriorityInPluginIdleRule);
            }
        }
    
    if (err == KErrNone)
        {
        CGOomIdleTimeRule* idleRule = CGOomIdleTimeRule::NewL(idleTime, priority);
        CleanupStack::PushL(idleRule);
        iConfig.AddPluginRuleL(uid, idleRule);
        CleanupStack::Pop(idleRule);
        }
    }

void CGOomConfigParser::SetPluginForegroundAppPriorityL(const RAttributeArray& aAttributes)
    {
    TUint uid;
    TUint targetAppId;
    TUint priority;

    TInt err = KErrNone;
    
    // Use the UID from the parent scope
    uid = iParentUid;

        // Check that we have a priority for the added system plugin action
        // Specifying a priority is mandatory
        
    TPtrC8 targetAppString;
    err = GetValueFromAttributeList(aAttributes, KGOomAttributeTargetAppId, targetAppString);
    if ((err == KErrNone)
            && (targetAppString == KGOomConfigTargetAppValue)
            && (iState == EGOomParsingStateAppPluginSettings))
        // If the target app is specified as "TARGET_APP" then we use the target app from the parent entry
        {
        targetAppId = iParentTargetApp;
        }
    else
        {
        err = GetValueFromHexAttributeList(aAttributes, KGOomAttributeTargetAppId, targetAppId);
        if (err != KErrNone)
            {
            ConfigError(KGOomErrBadOrMissingTargetAppIdInForegroundAppRule);    
            }
        }
     
    if (err == KErrNone)
        {
        err = GetValueFromDecimalAttributeList(aAttributes, KGOomAttibuteIdlePriority, priority);
        
        if (err != KErrNone)
            {
            ConfigError(KGOomErrBadOrMissingPriorityInForegroundAppRule);
            }
        }
    
    if (err == KErrNone)
        {
        CGOomForegroundRule* foregroundRule = new (ELeave) CGOomForegroundRule(targetAppId, priority);
        CleanupStack::PushL(foregroundRule);
        iConfig.AddPluginRuleL(uid, foregroundRule);
        CleanupStack::Pop(foregroundRule);
        }
    }

// Finds an attribute of the given name and gets its value
// A value is only valid as long as AAtrributes is valid (and unmodified)
// Returns KErrNone if the attribute is present in the list, KErrNotFound otherwise
TInt CGOomConfigParser::GetValueFromAttributeList(const RAttributeArray& aAttributes, const TDesC8& aName, TPtrC8& aValue)
    {
    TInt index = aAttributes.Count();
    TBool attributeFound = EFalse;
    while ((index--) && (!attributeFound))
        {
        if (aAttributes[index].Attribute().LocalName().DesC() == aName)
            {
            attributeFound = ETrue;
            aValue.Set(aAttributes[index].Value().DesC());
            }
        }
    
    TInt err = KErrNone;
    
    if (!attributeFound)
        err = KErrNotFound;
    
    return err;
    }

// Finds an attribute of the given name and gets its value (coverting the string hex value to a UInt)
// Returns KErrNone if the attribute is present in the list, KErrNotFound otherwise
// Returns KErrCorrupt if the string is not a valid hex number
TInt CGOomConfigParser::GetValueFromHexAttributeList(const RAttributeArray& aAttributes, const TDesC8& aName, TUint& aValue)
    {
    TPtrC8 hexString;
    TInt err = GetValueFromAttributeList(aAttributes, aName, hexString);

    if (hexString == KGOomConfigDefaultAppUid)
        {
        // This is a special case
        // When we hit this value in a hex field then we return the default app UID
        aValue = KGOomDefaultAppId;
        }
    else if (hexString == KGOomConfigDefaultPluginUid)
        {
        // This is a special case
        // When we hit this value in a hex field then we return the default app UID
        aValue = KGOomDefaultPluginId;
        }
    else if (hexString == KGOomConfigBusyAppUid)
        {
        aValue = KGOomBusyAppId;
        }
    else if (hexString == KGOomConfigHighPriorityAppUid)
        {
        aValue = KGOomHighPriorityAppId;
        }
    else if (err == KErrNone)
        {
        TLex8 hexLex(hexString);
        err = hexLex.Val(aValue, EHex);
        if (err != KErrNone)
            err = KErrCorrupt;
        }
    
    return err;
    }

// Finds an attribute of the given name and gets its value (coverting the string decimal value to a UInt)
// Returns KErrNone if the attribute is present in the list, KErrNotFound otherwise
// Returns KErrCorrupt if the string is not a valid decimal number
TInt CGOomConfigParser::GetValueFromDecimalAttributeList(const RAttributeArray& aAttributes, const TDesC8& aName, TUint& aValue)
    {
    TPtrC8 decimalString;
    TInt err = GetValueFromAttributeList(aAttributes, aName, decimalString);

    if (err == KErrNone)
        {
        if (decimalString == KGOomAttibuteNeverClose)
            aValue = KGOomPriorityInfinate;
        else
            {
            TLex8 decimalLex(decimalString);
            err = decimalLex.Val(aValue, EDecimal);
            if (err != KErrNone)
                err = KErrCorrupt;
            }
        }
    
    return err;
    }

TInt CGOomConfigParser::GetValueFromDecimalAttributeList(const RAttributeArray& aAttributes, const TDesC8& aName, TInt& aValue)
    {
    TUint uintValue;
    TInt err = GetValueFromDecimalAttributeList(aAttributes, aName, uintValue);
    aValue = uintValue;
    return err;
    }

void CGOomConfigParser::SetPluginSyncMode(const RAttributeArray& aAttributes, CGOomRunPluginConfig& aRunPluginConfig)
    {
    TPtrC8 syncModeString;
    TInt err = GetValueFromAttributeList(aAttributes, KGOomAttibuteSyncMode, syncModeString);
    
    if (err == KErrNone)
        // If there is no specified sync mode then leave it as the default
        {
        TGOomSyncMode syncMode = EContinue;
        
        if (syncModeString == KGOomConfigSyncModeContinue)
            syncMode = EContinueIgnoreMaxBatchSize;
        else if (syncModeString == KGOomConfigSyncModeCheck)
            syncMode = ECheckRam;
        else if (syncModeString == KGOomConfigSyncModeEstimate)
            syncMode = EEstimate;
        else
            ConfigError(KGOomErrInvalidSyncMode);
        
        if (err == KErrNone)
            {
            aRunPluginConfig.iSyncMode = syncMode;
            }
        }
    }

// Check that the current state is as expected
// If not then the specified config error is generated
void CGOomConfigParser::CheckState(TGOomParsingState aExpectedState, TInt aError)
    {
    if (iState != aExpectedState)
        ConfigError(aError);
    }

// Check that the current state is as expected
// If not then the specified config error is generated
// This version checks to ensure that the current state matches either of the passed in states
void CGOomConfigParser::CheckState(TGOomParsingState aExpectedState1, TGOomParsingState aExpectedState2, TInt aError)
    {
    if ((iState != aExpectedState1)
            && (iState != aExpectedState2))
        ConfigError(aError);
    }

void CGOomConfigParser::ChangeState(TGOomParsingState aState)
    {
    iState = aState;
    }
