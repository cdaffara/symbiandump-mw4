/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   DocumentLoader in webkit side
*
*/


#include <../bidi.h>
#include "SettingsContainer.h"
#include "WebView.h"
#include "BrCtl.h"
#include "WebUtil.h"
#include "Page.h"
#include "Frame.h"
#include "FrameTree.h"
#include "FrameView.h"
#include "Document.h"
#include "Settings.h"
#include "HttpSessionManager.h"
#include "StaticObjectsContainer.h"
#include "ResourceLoaderDelegate.h"
#include "HistoryInterface.h"
#include "BrCtlDefs.h"
#include "WebDocumentLoader.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "WebCharsetData.h"

using namespace WebCore;

static const int defaultFontSize = 12;

SettingsContainer::SettingsContainer(WebView* view, HistoryControllerInterface* historyController)
{
    m_webView = view;
    m_historyController = historyController;
    brctlSettings[TBrCtlDefs::ESettingsSmallScreen] = 0;
    brctlSettings[TBrCtlDefs::ESettingsAutoLoadImages] = 1;
    brctlSettings[TBrCtlDefs::ESettingsFontSize] = TBrCtlDefs::EFontSizeLevelNormal;
    brctlSettings[TBrCtlDefs::ESettingsEmbedded] = 0;
    brctlSettings[TBrCtlDefs::ESettingsTextWrapEnabled] = 0;
    brctlSettings[TBrCtlDefs::ESettingsCookiesEnabled] = 1;
    brctlSettings[TBrCtlDefs::ESettingsCSSFetchEnabled] = 0;
    brctlSettings[TBrCtlDefs::ESettingsECMAScriptEnabled] = 1;
    brctlSettings[TBrCtlDefs::ESettingsIMEINotifyEnabled] = 0;
    brctlSettings[TBrCtlDefs::ESettingsCharacterset] = 0;
    brctlSettings[TBrCtlDefs::ESettingsSendRefererHeader] = 0;
    brctlSettings[TBrCtlDefs::ESettingsSecurityWarnings] = 1;
    brctlSettings[TBrCtlDefs::ESettingsApId] = 0;
    brctlSettings[TBrCtlDefs::ESettingsCurrentZoomLevelIndex] = 100;
    brctlSettings[TBrCtlDefs::ESettingsPageOverview] = view->pageScalerEnabled();
    brctlSettings[TBrCtlDefs::ESettingsNumOfDownloads] = 0;
    brctlSettings[TBrCtlDefs::ESettingsLaunchAppUid] = 0;
    brctlSettings[TBrCtlDefs::ESettingsLaunchViewId] = 0;
    brctlSettings[TBrCtlDefs::ESettingsLaunchCustomMessageId] = 0;
    brctlSettings[TBrCtlDefs::ESettingsBackList] = 1;
    brctlSettings[TBrCtlDefs::ESettingsAutoRefresh] = 1;
    brctlSettings[TBrCtlDefs::ESettingsBrowserUtf8Encoding] = 0;
    brctlSettings[TBrCtlDefs::ESettingsAutoFormFillEnabled] = 2;
    brctlSettings[TBrCtlDefs::ESettingsSavedPage] = 1;
    brctlSettings[TBrCtlDefs::ESettingsSmartLink] = 1;
    brctlSettings[TBrCtlDefs::ESettingsAutoOpenDownloads] = 1;
    brctlSettings[TBrCtlDefs::ESettingsDisableFlash] = 0;
    brctlSettings[TBrCtlDefs::ESettingsToolbarOnOff] = 1;
    brctlSettings[TBrCtlDefs::ESettingsToolbarButton1Cmd] = 7;
    brctlSettings[TBrCtlDefs::ESettingsToolbarButton2Cmd] = 8;
    brctlSettings[TBrCtlDefs::ESettingsToolbarButton3Cmd] = 9;
    brctlSettings[TBrCtlDefs::ESettingsToolbarButton4Cmd] = 10;
    brctlSettings[TBrCtlDefs::ESettingsToolbarButton5Cmd] = 11;
    brctlSettings[TBrCtlDefs::ESettingsToolbarButton6Cmd] = 12;
    brctlSettings[TBrCtlDefs::ESettingsToolbarButton7Cmd] = 13;
    brctlSettings[TBrCtlDefs::ESettingsScriptLog] = 0;
    brctlSettings[TBrCtlDefs::ESettingsCursorShowMode] = 1;
    brctlSettings[TBrCtlDefs::ESettingsEnterKeyMode] = TBrCtlDefs::EEnterKeyCanActivateLink;
    

    updatePageSetting(TBrCtlDefs::ESettingsAutoLoadImages);
    updatePageSetting(TBrCtlDefs::ESettingsFontSize);
    updatePageSetting(TBrCtlDefs::ESettingsECMAScriptEnabled);

    updateHttpSetting(TBrCtlDefs::ESettingsEmbedded);
    updateHttpSetting(TBrCtlDefs::ESettingsCookiesEnabled);
    updateHttpSetting(TBrCtlDefs::ESettingsIMEINotifyEnabled);
    updateHttpSetting(TBrCtlDefs::ESettingsCharacterset);
    updateHttpSetting(TBrCtlDefs::ESettingsSendRefererHeader);
    updateHttpSetting(TBrCtlDefs::ESettingsSecurityWarnings);
    updateHttpSetting(TBrCtlDefs::ESettingsApId);
    updateHttpSetting(TBrCtlDefs::ESettingsAutoOpenDownloads);

    updateHistorySetting(TBrCtlDefs::ESettingsBackList);

    updateDocumentLoaderSettings(TBrCtlDefs::ESettingsCharacterset);
}


void SettingsContainer::setBrctlSetting(TBrCtlDefs::TBrCtlSettings setting, unsigned int value)
{
    if (setting > TBrCtlDefs::ESettingsUnknown && setting < TBrCtlDefs::ESettingsMaxEnum) {
        brctlSettings[setting] = value;
        updatePageSetting(setting);
        updateHttpSetting(setting);
        updateHistorySetting(setting);
        updateDocumentLoaderSettings(setting);
        updateZoomSettings(setting, value);
    }
}

void SettingsContainer::updatePageSetting(TBrCtlDefs::TBrCtlSettings setting)
{
    switch (setting)
    {
        case TBrCtlDefs::ESettingsAutoLoadImages:
            m_webView->page()->settings()->setLoadsImagesAutomatically(brctlSettings[setting]);
            break;
        case TBrCtlDefs::ESettingsFontSize:
        {
            m_webView->page()->settings()->setDefaultFontSize(textMultiplier(brctlSettings[setting], defaultFontSize));
            // force layout on the mainframe
            Frame* mf = core(m_webView->mainFrame());
            FrameView* v = mf->view();
            Document* doc = mf->document();
            if (v && doc) {
                for (Frame* f= mf; f; f = f->tree()->traverseNext(mf))
                    f->document()->recalcStyle(Node::Force);
                v->setNeedsLayout();
                v->layout();
            }    
            break;
        }
        case TBrCtlDefs::ESettingsECMAScriptEnabled:
            m_webView->page()->settings()->setJavaScriptEnabled(brctlSettings[setting]);
            break;
        case TBrCtlDefs::ESettingsCurrentZoomLevelIndex:
            m_webView->setZoomLevel(brctlSettings[setting]);
            break;
    }
}

void SettingsContainer::updateHttpSetting(TBrCtlDefs::TBrCtlSettings setting)
{
    HttpSessionManager* httpSessionMgr = WebCore::StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    switch (setting)
    {
        case TBrCtlDefs::ESettingsEmbedded:
            httpSessionMgr->setEmbedded(brctlSettings[setting]);
            break;
        case TBrCtlDefs::ESettingsCookiesEnabled:
            httpSessionMgr->enableCookies(brctlSettings[setting]);
            break;
        case TBrCtlDefs::ESettingsIMEINotifyEnabled:
            httpSessionMgr->enableImei(brctlSettings[setting]);
            break;
        case TBrCtlDefs::ESettingsCharacterset:
            httpSessionMgr->setCharset(brctlSettings[setting]);
            break;
        case TBrCtlDefs::ESettingsSendRefererHeader:
            httpSessionMgr->enableReferer(brctlSettings[setting]);
            break;
        case TBrCtlDefs::ESettingsSecurityWarnings:
            httpSessionMgr->enableSecurityDialogs(brctlSettings[setting]);
            break;
        case TBrCtlDefs::ESettingsApId:
            httpSessionMgr->setAp(brctlSettings[setting]);
            break;
        case TBrCtlDefs::ESettingsAutoOpenDownloads:
            httpSessionMgr->enableAutoOpenDownloads(brctlSettings[setting]);
            break;
    }
}

void SettingsContainer::updateHistorySetting(TBrCtlDefs::TBrCtlSettings setting)
{
    switch (setting)
    {
        case TBrCtlDefs::ESettingsBackList:
            m_historyController->setBackListAllowed(brctlSettings[setting]);
            break;
    }
}

void SettingsContainer::updateDocumentLoaderSettings(TBrCtlDefs::TBrCtlSettings setting)
{
    switch (setting)
    {
        case TBrCtlDefs::ESettingsCharacterset:
        {
            const TText* charset = charsetForUid(brctlSetting(TBrCtlDefs::ESettingsCharacterset));
            TPtrC ptr(KNullDesC);
            if (charset) {
                ptr.Set(charset);
            }
            if (core(m_webView->mainFrame())->loader()->activeDocumentLoader()) {
                core(m_webView->mainFrame())->loader()->activeDocumentLoader()->setOverrideEncoding(ptr);
            }            
            break;
        }
    }
}

unsigned int SettingsContainer::brctlSetting(TBrCtlDefs::TBrCtlSettings setting)
{
    unsigned int retVal (0);
    HttpSessionManager* httpSessionMgr = WebCore::StaticObjectsContainer::instance()->resourceLoaderDelegate()->httpSessionManager();
    switch( setting )
        {
        case TBrCtlDefs::ESettingsSmallScreen:
        case TBrCtlDefs::ESettingsAutoLoadImages:
        case TBrCtlDefs::ESettingsEmbedded:
        case TBrCtlDefs::ESettingsTextWrapEnabled:
        case TBrCtlDefs::ESettingsCookiesEnabled:
        case TBrCtlDefs::ESettingsCSSFetchEnabled:
        case TBrCtlDefs::ESettingsECMAScriptEnabled:
        case TBrCtlDefs::ESettingsIMEINotifyEnabled:
        case TBrCtlDefs::ESettingsSecurityWarnings:
        case TBrCtlDefs::ESettingsSendRefererHeader:
        case TBrCtlDefs::ESettingsAutoRefresh:
        case TBrCtlDefs::ESettingsSmartLink:
        case TBrCtlDefs::ESettingsAutoOpenDownloads:
        case TBrCtlDefs::ESettingsDisableFlash:
        case TBrCtlDefs::ESettingsFontSize:
        case TBrCtlDefs::ESettingsCharacterset:
        case TBrCtlDefs::ESettingsLaunchAppUid:
        case TBrCtlDefs::ESettingsLaunchViewId:
        case TBrCtlDefs::ESettingsLaunchCustomMessageId:
        case TBrCtlDefs::ESettingsAutoFormFillEnabled:
        case TBrCtlDefs::ESettingsApId:
        case TBrCtlDefs::ESettingsCurrentZoomLevelIndex:
        case TBrCtlDefs::ESettingsPageOverview:
        case TBrCtlDefs::ESettingsBackList:
        case TBrCtlDefs::ESettingsToolbarOnOff:
        case TBrCtlDefs::ESettingsToolbarButton1Cmd:
        case TBrCtlDefs::ESettingsToolbarButton2Cmd:
        case TBrCtlDefs::ESettingsToolbarButton3Cmd:
        case TBrCtlDefs::ESettingsToolbarButton4Cmd:
        case TBrCtlDefs::ESettingsToolbarButton5Cmd:
        case TBrCtlDefs::ESettingsToolbarButton6Cmd:
        case TBrCtlDefs::ESettingsToolbarButton7Cmd:      
        case TBrCtlDefs::ESettingsScriptLog:
        case TBrCtlDefs::ESettingsZoomLevelMin:
        case TBrCtlDefs::ESettingsZoomLevelMax:
        case TBrCtlDefs::ESettingsCursorShowMode:    
        case TBrCtlDefs::ESettingsZoomLevelDefault:
        case TBrCtlDefs::ESettingsEnterKeyMode:
            {
            retVal = brctlSettings[setting];
            break;
            }
        case TBrCtlDefs::ESettingsNumOfDownloads :
            {
            retVal = httpSessionMgr->httpDownload()->numOfDownloads();
            break;
            }
        default:
            {
            break;   // should not occur
            }
        }   // end of switch
    return retVal;

}

void SettingsContainer::updateZoomSettings(TBrCtlDefs::TBrCtlSettings setting, unsigned int value )
{
    m_webView->updateZoomLevel( setting, value );
}
