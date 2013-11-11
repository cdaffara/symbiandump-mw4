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
* WLAN Sniffer application main class implementation.
*/

// System includes

#include <QProcess>
#include <QLocale>
#include <HbStyleLoader>
#include <HbTranslator>

// User includes

#include "wlansnifferengine.h"
#include "wlansniffermainwindow.h"
#include "wlansniffer.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wlansnifferTraces.h"
#endif

/*!
    \class WlanSniffer
    \brief WLAN Sniffer main class implementation.
*/

// External function prototypes

// Local constants

//! Location of WLAN Sniffer custom layout definitions
static const QString WlanSnifferLayoutPath(":/wlansnifferlayout/");

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor.
    
    @param [in] argc Parameter count.
    @param [in] argv Parameters. 
 */

WlanSniffer::WlanSniffer(int argc, char* argv[]) :
    HbApplication(argc, argv),
    mEngine(new WlanSnifferEngine(this)),
    mTranslator(),
    mMainWindow(0)
{
    OstTraceFunctionEntry0(WLANSNIFFER_WLANSNIFFER_ENTRY);

    // Start scanning immediately to get the first scan results as soon as
    // possible, since the scanning takes time.
    if (mEngine->masterWlan() && !mEngine->forceDisableWlan()) {
        mEngine->startWlanScanning();
    }
    
    // Register custom layout location
    HbStyleLoader::registerFilePath(WlanSnifferLayoutPath);

    // Install localization
    mTranslator = QSharedPointer<HbTranslator>(new HbTranslator());
    mTranslator->loadCommon();
    
    mMainWindow = QSharedPointer<WlanSnifferMainWindow>(
        new WlanSnifferMainWindow(mEngine));
    
    // Connect exit handling
    bool connectStatus = connect(
        mEngine,
        SIGNAL(exitTriggered()),
        this,
        SLOT(exitApplication()));
    Q_ASSERT(connectStatus);

    // Start the Wlan Sniffer Keepalive process, if not started yet
    // The purpose of the process is to manage Wlan links opened
    // by this application, and the process should be running always
    // after the first launch of this application.
    if (findKeepaliveProcess() == false) {
        QProcess::startDetached("wlansnifferkeepalive");
    }
    
    OstTraceFunctionExit0(WLANSNIFFER_WLANSNIFFER_EXIT);
}

/*!
    Destructor.
 */

WlanSniffer::~WlanSniffer()
{
    OstTraceFunctionEntry0(WLANSNIFFER_WLANSNIFFERDESTR_ENTRY);

    HbStyleLoader::unregisterFilePath(WlanSnifferLayoutPath);
    
    OstTraceFunctionExit0(WLANSNIFFER_WLANSNIFFERDESTR_EXIT);
}

/*!
    Function for finding possible running keepalive process.
    
    @return TRUE if a keepalive process was found running.
 */

bool WlanSniffer::findKeepaliveProcess()
{
    OstTraceFunctionEntry0(WLANSNIFFER_FINDKEEPALIVEPROCESS_ENTRY);
    
    // Try to search the keepalive process
    bool found = false;
    TFileName executableName;
    executableName.Copy(_L("wlansnifferkeepalive*"));
    TFindProcess processSearch;
    TFullName processFullName;
    while (processSearch.Next(processFullName) == KErrNone) {
        if (processFullName.Match(executableName) != KErrNotFound) { 
            found = true;
            break;
        }
    }
    
    OstTraceExt1(
        TRACE_NORMAL,
        WLANSNIFFER_FINDKEEPALIVEPROCESS_RESULT,
        "WlanSniffer::findKeepaliveProcess;found=%hhu",
        found);
    
    OstTraceFunctionExit0(WLANSNIFFER_FINDKEEPALIVEPROCESS_EXIT);
    return found;
}

/*!
    Slot for handling application exit.
 */

void WlanSniffer::exitApplication()
{
    OstTraceFunctionEntry0(WLANSNIFFER_EXITAPPLICATION_ENTRY);
    
    exit();

    OstTraceFunctionExit0(WLANSNIFFER_EXITAPPLICATION_EXIT);
}
