/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Wlan Login document loader implementation
*
*/

// System includes
#include <HbMainWindow>

//User includes
#include "wlanloginwebview.h"
#include "wlanlogindocumentloader.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanlogindocumentloaderTraces.h"
#endif

/*!
    \class WlanLoginDocumentLoader
    \brief WLAN Login document loader implementation.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor
*/
WlanLoginDocumentLoader::WlanLoginDocumentLoader(const HbMainWindow *window) : HbDocumentLoader(window)
{
    OstTraceFunctionEntry0(WLANLOGINDOCUMENTLOADER_WLANLOGINDOCUMENTLOADER_ENTRY);
    OstTraceFunctionExit0(WLANLOGINDOCUMENTLOADER_WLANLOGINDOCUMENTLOADER_EXIT);   
}

/*!
    Destructor
*/
WlanLoginDocumentLoader::~WlanLoginDocumentLoader()
{
    OstTraceFunctionEntry0(WLANLOGINDOCUMENTLOADER_DESTRUCTOR_ENTRY);
    OstTraceFunctionExit0(WLANLOGINDOCUMENTLOADER_DESTRUCTOR_EXIT);
}

/*!
    This function returns WLAN Login UI specific objects from DocML
*/
QObject* WlanLoginDocumentLoader::createObject(const QString& type, const QString &name)
{
    OstTraceFunctionEntry0(WLANLOGINDOCUMENTLOADER_CREATEOBJECT_ENTRY);
    
    QObject *res(NULL);
    if( type == WlanLoginWebView::staticMetaObject.className() ) {
        res = new WlanLoginWebView();
        res->setObjectName(name);
    }

    if (!res) {
        res = HbDocumentLoader::createObject(type, name);
    }
    OstTraceFunctionEntry0(WLANLOGINDOCUMENTLOADER_CREATEOBJECT_EXIT);
    return res;
}
