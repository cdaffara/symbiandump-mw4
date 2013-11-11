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
* Description:   Keeps track of settings
*
*/


#ifndef __SETTINGSCONTAINER_H__
#define __SETTINGSCONTAINER_H__

#include <e32base.h>
#include "BrCtlDefs.h"

class WebView;
class HistoryControllerInterface;

class SettingsContainer
{
public:
    enum NavigationType { NavigationTypeCursor, NavigationTypeTabbed, NavigationTypeNone };
    SettingsContainer(WebView* view, HistoryControllerInterface* historyProvider);
    unsigned int brctlSetting(TBrCtlDefs::TBrCtlSettings setting);
    void setBrctlSetting(TBrCtlDefs::TBrCtlSettings setting, unsigned int value);
    void setNavigationType(NavigationType type) { m_navigationType = type; }
    NavigationType getNavigationType() const { return m_navigationType; }

private:
    void updatePageSetting(TBrCtlDefs::TBrCtlSettings setting);
    void updateHttpSetting(TBrCtlDefs::TBrCtlSettings setting);
    void updateHistorySetting(TBrCtlDefs::TBrCtlSettings setting);
    void updateDocumentLoaderSettings(TBrCtlDefs::TBrCtlSettings setting);
    void updateZoomSettings(TBrCtlDefs::TBrCtlSettings setting, unsigned int value );


private:
    unsigned int brctlSettings[TBrCtlDefs::ESettingsMaxEnum];
    WebView* m_webView;
    HistoryControllerInterface* m_historyController;
    NavigationType m_navigationType;
};

#endif // __WEBDOCUMENTLOADER_H__

// END OF FILE
