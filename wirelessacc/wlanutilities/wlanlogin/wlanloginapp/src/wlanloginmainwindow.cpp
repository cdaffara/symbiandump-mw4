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
* Description:
* Wlan Login main window class implementation
* 
*/

// System includes

//User includes

#include "wlanloginmainwindow.h"
#include "wlanloginengine.h"
#include "wlanloginview.h"
#include "wlanloginapplication.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanloginmainwindowTraces.h"
#endif

/*!
    \class WlanLoginMainWindow
    \brief WLAN Login main window class implementation.
*/

/*!
    Constructor.
 */
WlanLoginMainWindow::WlanLoginMainWindow(WlanLoginApplication* appRef) :
    mAppRef(appRef),
    mLoginView(NULL)
{
    OstTraceFunctionEntry0(WLANLOGINMAINWINDOW_WLANLOGINMAINWINDOW_ENTRY);
    
    addLoginView();
    setCurrentView(mLoginView);
    show();
    
    OstTraceFunctionExit0(WLANLOGINMAINWINDOW_WLANLOGINMAINWINDOW_EXIT);
}


/*!
    Destructor.
 */
WlanLoginMainWindow::~WlanLoginMainWindow()
{
    OstTraceFunctionEntry0(WLANLOGINMAINWINDOW_DESTRUCTOR_ENTRY);

    OstTraceFunctionExit0(WLANLOGINMAINWINDOW_DESTRUCTOR_EXIT);
}


/*!
    This function creates login view
 */
void WlanLoginMainWindow::addLoginView()
{
    OstTraceFunctionEntry0(WLANLOGINMAINWINDOW_ADDLOGINVIEW_ENTRY);

    mLoginView = new WlanLoginView(this);
    addView(mLoginView);

    OstTraceFunctionExit0(WLANLOGINMAINWINDOW_ADDLOGINVIEW_EXIT);
}


/*!
    This function returns pointer to login view
 */
WlanLoginView* WlanLoginMainWindow::loginView() const
{
    OstTraceFunctionEntry0(WLANLOGINMAINWINDOW_LOGINVIEW_ENTRY); 
    OstTraceFunctionExit0(WLANLOGINMAINWINDOW_LOGINVIEW_EXIT);
    
    return mLoginView;
}

/*!
    This function returns pointer to application instance
 */
WlanLoginApplication* WlanLoginMainWindow::application() const
{
    OstTraceFunctionEntry0(WLANLOGINMAINWINDOW_APPLICATION_ENTRY); 
    OstTraceFunctionExit0(WLANLOGINMAINWINDOW_APPLICATION_EXIT);
    
    return mAppRef;
}

