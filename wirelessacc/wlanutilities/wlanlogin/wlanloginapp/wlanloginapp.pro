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
# Project file for WLAN Login application.
#

TEMPLATE = app
TARGET = wlanlogin
DEPENDPATH += .
INCLUDEPATH += \
    . \
    /epoc32/include/mw \
    ../inc \
    ../../inc
                 
LIBS += -lxqservice -lxqserviceutil -lictsqtwrapper -lqtsysteminfo 

CONFIG += hb service mobility no_icon
MOBILITY += bearer

QT += \
    core \
    network \
    webkit
      
#Store generated .moc files to their own directory
MOC_DIR = build
      
# Input
SOURCES += \
    src/main.cpp \
    src/wlanloginapplication.cpp \
    src/wlanloginengine.cpp \
    src/wlanlogindocumentloader.cpp \
    src/wlanloginmainwindow.cpp \
    src/wlanloginnetworkaccessmanager.cpp \
    src/wlanloginservice.cpp \
    src/wlanloginview.cpp \
    src/wlanloginwebpage.cpp \
    src/wlanloginwebview.cpp
               
HEADERS += \
    inc/wlanloginapplication.h \
    inc/wlanloginengine.h \
    inc/wlanlogindocumentloader.h \
    inc/wlanloginmainwindow.h \
    inc/wlanloginnetworkaccessmanager.h \
    inc/wlanloginservice.h \
    inc/wlanloginview.h \
    inc/wlanloginwebpage.h \
    inc/wlanloginwebview.h \
    traces/OstTraceDefinitions.h

DOCML += res/wlanloginview.docml

RESOURCES = res/wlanlogin.qrc
          
TRANSLATIONS = wlanlogin.ts

SERVICE.FILE = res/service_conf.xml

# Temporary solution to fix tracecompiler
# When tracecompiler is fixed, this can be removed
MMP_RULES += "USERINCLUDE traces"

symbian*: {
    LIBS += -lictsclientinterface -lapgrfx -lws32
    TARGET.UID3 = 0x2002E6D0
    TARGET.CAPABILITY = CAP_APPLICATION NetworkControl
    BLD_INF_RULES.prj_exports += "rom/wlanlogin.iby CORE_MW_LAYER_IBY_EXPORT_PATH(wlanlogin.iby)"
    BLD_INF_RULES.prj_exports += "rom/wlanlogin_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(wlanlogin_resources.iby)"
}
