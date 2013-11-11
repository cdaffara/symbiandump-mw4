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
#include "config.h"
#include "WebDocumentLoader.h"
#include "SubstituteData.h"
#include "WebView.h"
#include "BrCtl.h"
#include "WebCharsetData.h"
#include "SettingsContainer.h"
#include "WebFrame.h"
#include "WebView.h"
#include "WebFrameView.h"

using namespace WebCore;

WebDocumentLoader::WebDocumentLoader(const ResourceRequest& request, const SubstituteData& substituteData)
    : DocumentLoader(request, substituteData)
    , m_hasEverBeenDetached(false) , m_userWasAskedToLoadNonSecureItem(false), m_userAgreedToLoadNonSecureItem (false)
{
}

/*
void WebDocumentLoader::setDataSource(WebDataSource *dataSource)
{

    ASSERT(!m_dataSource);
    HardRetain(dataSource);
    m_dataSource = dataSource;
    
    m_resourceLoadDelegate = [webView resourceLoadDelegate];
    m_downloadDelegate = [webView downloadDelegate];

    m_dataSource = dataSource;
}
*/
void WebDocumentLoader::attachToFrame()
{
    DocumentLoader::attachToFrame();
    ASSERT(m_loadingResources.isEmpty());
/*

    if (m_hasEverBeenDetached)
        HardRetain(m_dataSource);
*/
    if (control(frame())->settings() && charsetForUid(control(frame())->settings()->brctlSetting(TBrCtlDefs::ESettingsCharacterset))){
        setOverrideEncoding(TPtrC(charsetForUid(control(frame())->settings()->brctlSetting(TBrCtlDefs::ESettingsCharacterset))));
    }

}

void WebDocumentLoader::detachFromFrame()
{
    DocumentLoader::detachFromFrame();
/*
  
    m_hasEverBeenDetached = true;
    HardRelease(m_dataSource);
*/
}

void WebDocumentLoader::increaseLoadCount(unsigned long identifier)
{
    if (m_loadingResources.contains(identifier))
        return;
/*

    if (m_loadingResources.isEmpty())
        HardRetain(m_dataSource);
*/
    m_loadingResources.add(identifier);
}

void WebDocumentLoader::decreaseLoadCount(unsigned long identifier)
{
    m_loadingResources.remove(identifier);
/*
    
    if (m_loadingResources.isEmpty()) {
        m_resourceLoadDelegate = 0;
        m_downloadDelegate = 0;
        HardRelease(m_dataSource);
    }
*/
}

void WebDocumentLoader::setUserAgreedToLoadNonSecureItem(bool userAgreed)
{
    m_userAgreedToLoadNonSecureItem = userAgreed;
    m_userWasAskedToLoadNonSecureItem = true;
}


