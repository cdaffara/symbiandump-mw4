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

TEMPLATE = app
TARGET = wlansettingstest
QT += core \
    gui
HEADERS += wlansettingstest.loc \
    ../inc/wlansettings.h \
    ../inc/wlansettings_s60_p.h \
    ../traces/OstTraceDefinitions.h \
				wlansettingstest.h
	
SOURCES +=  wlansettingstest.cpp \
				wlansettingstest.rss \
			 wlansettingstest_reg.rss \
				main.cpp \
			../src/wlansettings.cpp \
			../src/wlansettings_s60.cpp
			
FORMS += 
RESOURCES += 
symbian:TARGET.UID3 = 0xE05A6280

TARGET.CAPABILITY = All -TCB

CONFIG += qtestlib
LIBS += -lCOMMSDAT \
    -lcommdb \
    -lcpframework \
    -lCOMMSDAT \
    -lcmmanagerdatabase \
    -lcmmanager \
    -lecom \
    -lxqsettingsmanager
