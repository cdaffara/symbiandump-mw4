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

#include "wlanloginapplication.h"
#include <qsysteminfo.h>

int main(int argc, char *argv[])
{
    // Check WLAN dynamic configuration & exit if WLAN is not enabled
    QtMobility::QSystemInfo sysinfo;
    if (!sysinfo.hasFeatureSupported(QtMobility::QSystemInfo::WlanFeature)) {
        return -1;
    }
    
    WlanLoginApplication app(argc, argv);
    return app.exec();
}
