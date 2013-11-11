#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# 
# Description: 
#	Stubs for Wlan wizard testing
#

DEFINES += BUILD_WLANQTUTILITIES_DLL

HEADERS += \
    stubs/eapqtcertificateinfo.h \
    stubs/eapqtconfig.h \
    stubs/eapqtconfiginterface.h \
    stubs/eapqtconfiginterface_context.h \
    stubs/eapqtpacstoreconfig.h \
    stubs/eapqtvalidator.h \
    stubs/eapqtvalidator_stub.h \
    stubs/wlanmgmtclient.h \
    stubs/wlanmgmtclient_context.h \
    stubs/wlanqtutils.h \
    stubs/wlanqtutils_context.h \
    stubs/wlanqtutilsap.h \
    stubs/wlanqtutilscommon.h
    
SOURCES += \
    stubs/eapqtcertificateinfo_stub.cpp \
    stubs/eapqtconfig_stub.cpp \
    stubs/eapqtconfiginterface_stub.cpp \
    stubs/eapqtconfiginterface_context.cpp \
    stubs/eapqtpacstoreconfig_stub.cpp \
    stubs/eapqtvalidator_stub.cpp \
    stubs/wlanmgmtclient_stub.cpp \
    stubs/wlanmgmtclient_context.cpp \
    stubs/wlanqtutils_context.cpp \
    stubs/wlanqtutils_stub.cpp \
    stubs/wlanqtutilsap_stub.cpp 
