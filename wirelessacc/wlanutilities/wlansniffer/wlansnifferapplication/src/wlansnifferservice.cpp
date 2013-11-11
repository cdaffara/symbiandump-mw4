/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* WLAN Sniffer QtHighway Service.
*/

// System includes

// User includes

#include "wlansnifferservice.h"
#include <xqaiwdecl.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlansnifferserviceTraces.h"
#endif

/*!
    \class WlanSnifferService
    \brief WLAN Sniffer QtHighway service implementation.

    This class implements a QtHighway service that can be used to launch
    a WLAN Sniffer application. 
*/


// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param[in] parent Object parent.
 */

WlanSnifferService::WlanSnifferService(QObject* parent) :
    XQServiceProvider(
        QLatin1String("wlansniffer.com.nokia.symbian.IWlanSniffer"),
        parent)
{
    OstTraceFunctionEntry0(WLANSNIFFERSERVICE_WLANSNIFFERSERVICE_ENTRY);
    
    publishAll();

    OstTraceFunctionExit0(WLANSNIFFERSERVICE_WLANSNIFFERSERVICE_EXIT);
}

/*!
    Destructor.
 */

WlanSnifferService::~WlanSnifferService()
{
    OstTraceFunctionEntry0(WLANSNIFFERSERVICE_WLANSNIFFERSERVICEDESTR_ENTRY);
    OstTraceFunctionExit0(WLANSNIFFERSERVICE_WLANSNIFFERSERVICEDESTR_EXIT);
}

/*!
    Completes the current service.
 */

void WlanSnifferService::complete()
{
    OstTraceFunctionEntry0(WLANSNIFFERSERVICE_COMPLETE_ENTRY);
    
    QVariant returnValue(true);
    bool status = completeRequest(mAsyncRequestIndex, returnValue);
    Q_ASSERT(status);

    OstTraceFunctionExit0(WLANSNIFFERSERVICE_COMPLETE_EXIT);
}

/*!
    List view service function. Launches the WLAN Sniffer List View.
 */

void WlanSnifferService::listView()
{
    OstTraceFunctionEntry0(WLANSNIFFERSERVICE_LISTVIEW_ENTRY);
    
    mAsyncRequestIndex = setCurrentRequestAsync();
    emit toListView(title());

    OstTraceFunctionExit0(WLANSNIFFERSERVICE_LISTVIEW_EXIT);
}

/*
    Extracts and returns the client application's title (if available).
    
    @return Client application's title string or empty if not available.
 */

QString WlanSnifferService::title() const
{
    OstTraceFunctionEntry0(WLANSNIFFERSERVICE_TITLE_ENTRY);
    
    // Get service request info
    XQRequestInfo info = requestInfo();
    
    // There must be a valid request info object
    Q_ASSERT(info.isValid());

    // Extract the title
    QVariant titleInfo = info.info(XQINFO_KEY_WINDOW_TITLE);
    QString title = titleInfo.toString();
    
    OstTraceFunctionExit0(WLANSNIFFERSERVICE_TITLE_EXIT);
    return title;
}
