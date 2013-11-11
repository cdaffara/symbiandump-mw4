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
# Project file for WLAN Sniffer application.
#

TEMPLATE = app
TARGET = wlansniffer
DEPENDPATH += .
# Own headers
INCLUDEPATH += \
    inc \
    traces
# wlanutilities Private API
INCLUDEPATH += ../../inc
CONFIG += hb no_icon service

# Temporary solution to fix tracecompiler
# When tracecompiler is fixed, this can be removed
symbian: {
    MMP_RULES += "USERINCLUDE traces"
}

#Store generated files to their own directory
MOC_DIR = build
RCC_DIR = build

TRANSLATIONS = wlansniffer.ts

# Input
SOURCES += \
    src/main.cpp \
    src/wlansniffer.cpp \
    src/wlansnifferengine.cpp \
    src/wlansnifferlistitem.cpp \
    src/wlansnifferlistview.cpp \
    src/wlansnifferlistwidget.cpp \
    src/wlansniffermainwindow.cpp \
    src/wlansnifferservice.cpp

HEADERS += \
    inc/wlansniffer.h \
    inc/wlansnifferengine.h \
    inc/wlansnifferlistitem.h \
    inc/wlansnifferlistview.h \
    inc/wlansnifferlistwidget.h \
    inc/wlansniffermainwindow.h \
    inc/wlansnifferservice.h \
    traces/OstTraceDefinitions.h

RESOURCES = res/wlansniffer.qrc

DOCML += res/wlansnifferlistview.docml

SERVICE.FILE = res/service_conf.xml
SERVICE.OPTIONS = embeddable

# Common libraries
LIBS += -lwlanqtutilities -lxqservice -lxqserviceutil -lqtsysteminfo -lwlanwizard -lxqsettingsmanager

symbian*: {
    TARGET.CAPABILITY = CAP_APPLICATION NetworkControl
    TARGET.UID3 = 0x10281CAA
    
    BLD_INF_RULES.prj_exports += "rom/wlansniffer.iby CORE_MW_LAYER_IBY_EXPORT_PATH(wlansniffer.iby)"
    BLD_INF_RULES.prj_exports += "rom/wlansniffer_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(wlansniffer_resources.iby)"
    
    # Enable for QtHighway tracing
    # DEFINES += WLANSNIFFER_SERVICETRACES
}
