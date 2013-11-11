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
# Description: Project file for ictsqtwrapper.dll
#

TEMPLATE = lib
TARGET = ictsqtwrapper
TARGET.CAPABILITY = CAP_GENERAL_DLL

#BUILD_DLL macro is used to define export macro
DEFINES += BUILD_ICTSQTWRAPPER_DLL
DEPENDPATH += .

#Store generated .moc files to their own directory
MOC_DIR = build

INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE \
               inc \
               traces

HEADERS += inc/ictsqtwrapper.h  \
           traces/OstTraceDefinitions.h

SOURCES += src/ictsqtwrapper.cpp

defFilePath = .

symbian: {
    LIBS += -lictsclientinterface 
    HEADERS += inc/ictsqtwrapper_symbian.h
    SOURCES += src/ictsqtwrapper_symbian.cpp
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x2002E6F7
    MMP_RULES += "USERINCLUDE traces"
    BLD_INF_RULES.prj_exports += "rom/ictsqtwrapper.iby CORE_MW_LAYER_IBY_EXPORT_PATH(ictsqtwrapper.iby)"
    BLD_INF_RULES.prj_exports += "inc/ictsqtwrapper.h |../../inc/ictsqtwrapper.h"
}