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
# 
# Description: 
#	EAP Settings Wizard plugin for WLAN Wizard
#

TEMPLATE = lib
TARGET = eapwizard

DEFINES += BUILD_EAP_WIZARD_DLL

VERSION = 0.0.1

CONFIG += hb

MOC_DIR = build
RCC_DIR = build

RESOURCES += resources/eapwizard_resource.qrc

DOCML += \
    resources/occ_eap_wizard_01_02_04.docml \
    resources/occ_eap_wizard_03.docml \
    resources/occ_eap_wizard_05_07.docml \
    resources/occ_eap_wizard_06.docml \
    resources/occ_eap_wizard_08.docml

TRANSLATIONS = eapwizard.ts

HEADERS   += \
	inc/eapwizard.h \
	inc/eapwizard_p.h \
	inc/eapwizardpage.h \
	inc/eapwizardpagecertca.h \
	inc/eapwizardpagecertuser.h \             
	inc/eapwizardpageidentity.h \
	inc/eapwizardpageinnertype.h \
	inc/eapwizardpagenewpacstore.h \
	inc/eapwizardpageoutertype.h \
	inc/eapwizardpagepacstorepasswordconfirm.h \
	inc/eapwizardpageusernamepassword.h
	
SOURCES   += \
	src/eapwizard.cpp \
	src/eapwizard_p.cpp \ 
	src/eapwizardpage.cpp \
	src/eapwizardpagecertca.cpp \
	src/eapwizardpagecertuser.cpp \
	src/eapwizardpageidentity.cpp \
	src/eapwizardpageinnertype.cpp \	
	src/eapwizardpagenewpacstore.cpp \
	src/eapwizardpageoutertype.cpp \
	src/eapwizardpagepacstorepasswordconfirm.cpp \
	src/eapwizardpageusernamepassword.cpp
    
LIBS  += -leapqtconfiginterface -leapqtplugininfo
    
INCLUDEPATH += \
	inc \
	../inc    
    
symbian {
	TARGET.UID3 = 0x2002C2FE
	TARGET.CAPABILITY = CAP_GENERAL_DLL
	TARGET.EPOCALLOWDLLDATA = 1
       
	# Location of directories for def files
    defFilePath = .
  	
    # OST trace system requires traces directory to be in USERINCLUDES
  	MMP_RULES += "USERINCLUDE traces"
  	
  	BLD_INF_RULES.prj_exports += \ 
  		"rom/eapwizard.iby CORE_MW_LAYER_IBY_EXPORT_PATH(eapwizard.iby)"
  	  	  		
    BLD_INF_RULES.prj_exports += \
        "rom/eapwizard_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(eapwizard_resources.iby)"
  	
  	# interface of eapwizard
  	BLD_INF_RULES.prj_exports += "inc/eapwizard.h |../inc/eapwizard.h"   	
}
