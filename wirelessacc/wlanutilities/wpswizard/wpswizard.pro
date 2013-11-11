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
# Description: WPS wizard 
#


TEMPLATE = lib
TARGET = wpswizard
DEFINES += BUILD_WPS_WIZARD_DLL
VERSION = 0.0.1
CONFIG += hb
MOC_DIR = build
RCC_DIR = build
RESOURCES += resources/wpswizard_resource.qrc
TRANSLATIONS += wpswizardplugin.ts

DOCML += \
    resources/occ_wps_01_05.docml \
    resources/occ_wps_02_03.docml \
    resources/occ_wps_04.docml

INCLUDEPATH += inc \
        ../inc

HEADERS += inc/wpswizard.h \
    inc/wpswizard_p.h \
    inc/wpswizardpage.h \
    inc/wpswizardsteptwo.h \
    inc/wpswizardstepthreebutton.h \
    inc/wpswizardstepthreenumber.h \
    inc/wpswizardstepfour.h \
    inc/wpswizardstepfour_p.h \
    inc/wpswizardstepfive.h \
    inc/cwpsactiverunner.h \
    inc/mwpsactiverunnercallback.h \
    
SOURCES += src/wpswizard_p.cpp \
    src/wpswizard.cpp \
    src/wpswizardpage.cpp \
    src/wpswizardsteptwo.cpp \
    src/wpswizardstepthreebutton.cpp \
    src/wpswizardstepthreenumber.cpp \
    src/wpswizardstepfour.cpp \
    src/wpswizardstepfour_p.cpp \
    src/wpswizardstepfive.cpp \
    src/cwpsactiverunner.cpp
    

symbian { 

    LIBS += -lecom    
        
    TARGET.UID3 = 0x2003D2FE
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    
    TARGET.EPOCALLOWDLLDATA = 1
        
    MMP_RULES += "USERINCLUDE traces"
    
    # location of directories for def files
    defFilePath = .
    
    BLD_INF_RULES.prj_exports += "rom/wpswizard.iby CORE_MW_LAYER_IBY_EXPORT_PATH(wpswizard.iby)"
    BLD_INF_RULES.prj_exports += "rom/wpswizard_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(wpswizard_resources.iby)"
    BLD_INF_RULES.prj_exports += "inc/wpswizard.h |../inc/wpswizard.h"       
}

include (stubs/stubs.pri)

