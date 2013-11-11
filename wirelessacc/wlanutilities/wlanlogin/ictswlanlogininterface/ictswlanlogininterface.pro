#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# Project file of the ictswlanlogininterface.
#

TEMPLATE = lib
TARGET = ictswlanlogininterface

# BUILD_DLL macro is used to define import/export macro
DEFINES += BUILD_ICTSWLANLOGININTERFACE_DLL
DEPENDPATH += .
CONFIG += hb
LIBS += -lxqservice -lictsqtwrapper -lxqserviceutil

#Store generated files to their own directory
MOC_DIR = build

INCLUDEPATH += \
    inc \
    traces\
    ../inc \
    ../../inc \
    ../wlanloginapp/inc

HEADERS += \
    inc/ictswlanlogininterface.h \
    traces/OstTraceDefinitions.h

SOURCES += \
    src/ictswlanlogininterface.cpp

defFilePath = .

symbian: { 
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x2002EA66
    MMP_RULES += "USERINCLUDE traces"
    BLD_INF_RULES.prj_exports += "rom/ictswlanlogininterface.iby CORE_MW_LAYER_IBY_EXPORT_PATH(ictswlanlogininterface.iby)"
    BLD_INF_RULES.prj_exports += "inc/ictswlanlogininterface.h |../../inc/ictswlanlogininterface.h"
    LIBS += -lictsclientinterface -lflogger
}
