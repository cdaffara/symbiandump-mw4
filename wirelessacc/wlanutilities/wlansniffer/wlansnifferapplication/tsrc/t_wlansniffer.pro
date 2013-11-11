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
# WLAN Sniffer UT project file.
#

CONFIG += qtestlib hb

TEMPLATE = app
TARGET = t_wlansniffer

TARGET.CAPABILITY = ALL -TCB

DEPENDPATH += .

#Store generated files to their own directory
MOC_DIR = build
RCC_DIR = build

INCLUDEPATH += \
    . \
    ../inc \
    ../traces \
    ../../../inc

# Input
HEADERS += \
    t_wlansniffer.h \
    ../inc/wlansnifferlistitem.h \
    ../inc/wlansnifferlistwidget.h \
    ../traces/OstTraceDefinitions.h

SOURCES += \
    t_wlansniffer.cpp \
    ../src/wlansnifferlistitem.cpp \
    ../src/wlansnifferlistwidget.cpp
    
symbian*::LIBS += -lwlanqtutilities
