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

// System includes
#include <QWebPage>
#include <QWebFrame>
#include <QNetworkRequest>

// User includes
#include "wlanloginwebpage.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanloginwebpageTraces.h"
#endif

/*!
    \class WlanLoginWebPage
    \brief WLAN Login web page class implementation.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========


/*!
    Constructor.
 */
WlanLoginWebPage::WlanLoginWebPage(QObject *parent): QWebPage(parent)
{
    OstTraceFunctionEntry0( WLANLOGINWEBPAGE_WLANLOGINWEBPAGE_ENTRY );

    //Hide scrollbars
    mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff );
    mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff );

    setContentEditable(false);
    
    OstTraceFunctionExit0( WLANLOGINWEBPAGE_WLANLOGINWEBPAGE_EXIT );
}


WlanLoginWebPage::~WlanLoginWebPage()
{
    OstTraceFunctionEntry0( WLANLOGINWEBPAGE_DESTRUCTOR_ENTRY );

    OstTraceFunctionExit0( WLANLOGINWEBPAGE_DESTRUCTOR_EXIT );
}


/*!
    This function checks navigation events. This was rewritten as we needed
    indication to login view when user has submitted the password and user name. That 
    corresponds to value QWebPage::NavigationTypeFormSubmitted. This triggers starting of
    internet connectivity test.
    
    @param [in] frame
    @param [in] request
    @param [in] type
 */
bool WlanLoginWebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, QWebPage::NavigationType type)
{
    OstTraceFunctionEntry0( WLANLOGINWEBPAGE_ACCEPTNAVIGATIONREQUEST_ENTRY );
    
    if (type== QWebPage::NavigationTypeFormSubmitted) {
    
    OstTrace0(
        TRACE_BORDER,
        WLANLOGINWEBPAGE_ACCEPTNAVIGATIONREQUEST_EMITTING_TRACE,
        "WlanLoginWebPage::acceptNavigationRequest emitting formSubmitted() signal");
    
        emit formSubmitted();
    }
   
    OstTraceFunctionExit0( WLANLOGINWEBPAGE_ACCEPTNAVIGATIONREQUEST_EXIT );
    
    return QWebPage::acceptNavigationRequest(frame,request,type);
}
