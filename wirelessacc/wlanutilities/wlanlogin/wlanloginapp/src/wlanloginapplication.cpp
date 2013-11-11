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
* Wlan Login application class implementation.
* 
*/


// System includes
#include <QObject>
#include <QStringList>
#include <QString>
#include <QUrl>
#include <HbTranslator>

//User includes
#include "wlanloginapplication.h"
#include "wlanloginservice.h"
#include "wlanloginmainwindow.h"
#include "wlanloginengine.h"
#include "wlanloginview.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlanloginapplicationTraces.h"
#endif

/*!
    \class WlanLoginApplication
    \brief WLAN Login application class implementation.
*/

// External function prototypes

// Local constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param [in] argc Parameter count.
    @param [in] argv Parameters. 
 */
WlanLoginApplication::WlanLoginApplication(int argc, char* argv[]) :
    HbApplication(argc, argv, Hb::NoSplash),
    mService(new WlanLoginService(this)),
    mTranslator(new HbTranslator()),
    mEngine(new WlanLoginEngine(this)),
    mMainWindow(new WlanLoginMainWindow(this))
{
    OstTraceFunctionEntry0(WLANLOGINAPPLICATION_WLANLOGINAPPLICATION_ENTRY);
    
    bool connectStatus = connect(
        mService,
        SIGNAL(startLogin(int,int,QUrl)),
        mEngine, 
        SLOT(handleStartLogin(int,int,QUrl)));    
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        mService,
        SIGNAL(stopLogin()),
        mEngine, 
        SLOT(handleStopLogin()));    
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        mEngine,
        SIGNAL(completeServiceRequest(int)),
        mService,
        SLOT(completeServiceRequest(int)));    
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        mEngine,
        SIGNAL(connectionReady(QUrl)),
        mMainWindow->loginView(), 
        SLOT(handleConnectionReady(QUrl)));    
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        mMainWindow->loginView(),
        SIGNAL(startIcts()),
        mEngine, 
        SLOT(handleStartIcts()));    
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        mEngine,
        SIGNAL(ictsOk()),
        mMainWindow->loginView(),
        SLOT(handleIctsOk()));
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        mMainWindow->loginView(),
        SIGNAL(cancelTriggered()),
        mEngine, 
        SLOT(handleCancelTriggered()));
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        mMainWindow->loginView(),
        SIGNAL(nextTriggered()),
        mEngine, 
        SLOT(handleNextTriggered()));    
    Q_ASSERT(connectStatus == true);
    
    connectStatus = connect(
        mMainWindow.data(),
        SIGNAL(orientationChanged(Qt::Orientation)),
        mMainWindow->loginView(),
        SLOT(handleOrientationChanged(Qt::Orientation)));
    Q_ASSERT(connectStatus == true);
    
    OstTraceFunctionExit0(WLANLOGINAPPLICATION_WLANLOGINAPPLICATION_EXIT);
}


/*!
    Destructor.
 */
WlanLoginApplication::~WlanLoginApplication()
{
    OstTraceFunctionEntry0(WLANLOGINAPPLICATION_DESTRUCTOR_ENTRY);

    OstTraceFunctionExit0(WLANLOGINAPPLICATION_DESTRUCTOR_EXIT);
}


/*!
    Function that returns pointer to Qt Highway service instance.
    
    @return Pointer to XQServiceProvider
 */
WlanLoginService* WlanLoginApplication::service() const
{
    OstTraceFunctionEntry0(WLANLOGINAPPLICATION_SERVICE_ENTRY);
    OstTraceFunctionExit0(WLANLOGINAPPLICATION_SERVICE_EXIT);
    
    return mService;
}


/*!
    Function that returns pointer to WLAN login engine
    
    @return Pointer to engine
 */
WlanLoginEngine* WlanLoginApplication::engine() const
{
    OstTraceFunctionEntry0(WLANLOGINAPPLICATION_ENGINE_ENTRY);
    OstTraceFunctionExit0(WLANLOGINAPPLICATION_ENGINE_EXIT);
    
    return mEngine;
}


/*!
    Function that returns pointer to WLAN login application main window
    
    @return Pointer to application main window
 */
QSharedPointer<WlanLoginMainWindow> WlanLoginApplication::mainWindow()
{
    OstTraceFunctionEntry0(WLANLOGINAPPLICATION_MAINWINDOW_ENTRY);
    OstTraceFunctionExit0(WLANLOGINAPPLICATION_MAINWINDOW_EXIT);
    
    return mMainWindow;
}
