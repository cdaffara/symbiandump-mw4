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
#

TEMPLATE = lib
TARGET = cpwlansettingsplugin
CONFIG += hb plugin

INCLUDEPATH += traces

HEADERS += inc/cpwlansettingsplugin.h \
		inc/cpwlansettingsgroupitemdata.h \
		inc/wlansettings.h \
		inc/wlansettings_s60_p.h \
		traces/OstTraceDefinitions.h
		
SOURCES += src/cpwlansettingsplugin.cpp \
		src/cpwlansettingsgroupitemdata.cpp \
		src/wlansettings.cpp \
		src/wlansettings_s60.cpp

TRANSLATIONS = cpwlansettingsplugin.ts

LIBS += -lcpframework \
		-lcentralrepository \
		-lCOMMSDAT \
		-lcmmanagerdatabase \
		-lcmmanager \
		-lecom \
		-lxqsettingsmanager
				
# Temporary solution to fix tracecompiler
# When tracecompiler is fixed, this can be removed
symbian: {
    MMP_RULES += "USERINCLUDE traces"
}

symbian: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x2002DC79
    
    BLD_INF_RULES.prj_exports += "rom/cpwlansettingsplugin.iby CORE_MW_LAYER_IBY_EXPORT_PATH(cpwlansettingsplugin.iby)"
    BLD_INF_RULES.prj_exports += "rom/cpwlansettingsplugin_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(cpwlansettingsplugin_resources.iby)"
}
symbian { 
    deploy.path = C:
    qtplugins.path = /resource/qt/plugins/controlpanel
    qtplugins.sources += qmakepluginstubs/cpwlansettingsplugin.qtplugin
    
    # This is for new exporting system coming in garden
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin $$deploy.path$$qtplugins.path/$$basename(qtplugin)"
}
TARGET.CAPABILITY = CAP_GENERAL_DLL
plugin.sources = cpwlansettingsplugin.dll
plugin.path = /resource/qt/plugins/controlpanel
DEPLOYMENT += plugin
