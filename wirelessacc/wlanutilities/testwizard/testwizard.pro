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
#    Test application for wizards
#


TEMPLATE = app
TARGET = testwizard
CONFIG += hb

MOC_DIR = build
RCC_DIR = build

DEPENDPATH += .
INCLUDEPATH += . \
	../inc

HEADERS = inc/MyApplication.h \
    inc/MyAppMainWindow.h \
    inc/FirstView.h \
    inc/MyAppEngine.h
    
SOURCES += src/MyAppEngine.cpp \
    src/MyApplication.cpp \
    src/MyAppMainWindow.cpp \
    src/FirstView.cpp \
    src/main.cpp
    
RESOURCES = resources/resource.qrc

DOCML += resources/mainview.docml

symbian {
    # TODO: gain UID
    TARGET.UID3 = 0xE5C15256
    TARGET.CAPABILITY = ALL -TCB
}

LIBS += -lwlanwizard

myDependencies.sources = wlanwizard.dll eapwizard.dll wpswizard.dll wlanqtutilities.dll  
DEPLOYMENT += myDependencies

BLD_INF_RULES.prj_exports += \ 
    "rom/testwizard.iby CORE_MW_LAYER_IBY_EXPORT_PATH(testwizard.iby)"

