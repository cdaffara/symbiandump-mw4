#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
# WLAN Qt Utilities project file.
#

TEMPLATE = lib
TARGET = wlanqtutilities

#BUILD_DLL macro is used to define export macro
DEFINES += BUILD_WLANQTUTILITIES_DLL
DEPENDPATH += .

# Store generated files to their own directory
MOC_DIR = build

# Temporary solution to fix tracecompiler
# When tracecompiler is fixed, this can be removed
symbian: {
    MMP_RULES += "USERINCLUDE traces"
}

INCLUDEPATH += \
    ../inc \
    base/inc \
    wrapper/inc \
    traces

HEADERS += \
    base/inc/wlanqtutils.h \
    base/inc/wlanqtutils_p.h \
    base/inc/wlanqtutilsap.h \
    base/inc/wlanqtutilsconnection.h \
    base/inc/wlanqtutilsiapsettings.h \
    wrapper/inc/wlanqtutilsconmonwrapper.h \
    wrapper/inc/wlanqtutilsesockwrapper.h \
    wrapper/inc/wlanqtutilsscan.h \
    traces/OstTraceDefinitions.h

SOURCES += \
    base/src/wlanqtutils.cpp \ 
    base/src/wlanqtutils_p.cpp \
    base/src/wlanqtutilsap.cpp \
    base/src/wlanqtutilsconnection.cpp \
    base/src/wlanqtutilsiapsettings.cpp \
    wrapper/src/wlanqtutilsconmonwrapper.cpp \
    wrapper/src/wlanqtutilsesockwrapper.cpp \
    wrapper/src/wlanqtutilsscan.cpp

# Common libraries
LIBS += -lconnection_settings_shim

symbian: { 
    HEADERS += \
        wrapper/inc/wlanqtutilsconmonwrapperdisconnect_symbian.h \
        wrapper/inc/wlanqtutilsconmonwrapperinfo_symbian.h \
        wrapper/inc/wlanqtutilsesockwrapper_symbian.h \
        wrapper/inc/wlanqtutilsscanap_symbian.h \
        wrapper/inc/wlanqtutilsscaniap_symbian.h
    SOURCES += \
        wrapper/src/wlanqtutilsconmonwrapperdisconnect_symbian.cpp \
        wrapper/src/wlanqtutilsconmonwrapperinfo_symbian.cpp \
        wrapper/src/wlanqtutilsesockwrapper_symbian.cpp \
        wrapper/src/wlanqtutilsscanap_symbian.cpp \
        wrapper/src/wlanqtutilsscaniap_symbian.cpp

    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x20029F52
    defFilePath = .

    BLD_INF_RULES.prj_exports += "rom/wlanqtutilities.iby CORE_MW_LAYER_IBY_EXPORT_PATH(wlanqtutilities.iby)"
    BLD_INF_RULES.prj_exports += \
        "base/inc/wlanqtutils.h |../inc/wlanqtutils.h" \
        "base/inc/wlanqtutilsap.h |../inc/wlanqtutilsap.h"

    # S60 libraries                                 
    LIBS += -lconnmon -lecom -lesock -lextendedconnpref -lnetmeta -lictswlanlogininterface -lcharconv
}

# Stubs for emulator
include(stubs/stubs.pri)
