# ##############################################################################
# /*
# ========================================================================
# Name : smfcredmgrclient.pro
# Author : Pritam Roy Biswas
# Copyright : Copyright (c) 2009-2010, Sasken Communication Technologies Ltd.
# Description :
# ========================================================================
# */
# ###############################################################################
QT += network
TARGET = smfcredmgrclient
TEMPLATE = lib
CONFIG += precompile_header
DEFINES += SMFCREDMGRCLIENT_LIBRARY \
	Q_OS_SYMBIAN
INCLUDEPATH += . \
    
SOURCES += smfcredmgrclient.cpp \
    smfcredmgrclientutil.cpp
HEADERS += smfcredmgrclient.h \
    smfcredmgrclientutil.h
symbian { 
    TARGET.UID3 = 0xEd21cfd3

    # EPOCALLOWDLLDATA have to set true because Qt macros has initialised global data
    TARGET.EPOCALLOWDLLDATA=1
    
    SOURCES += smfcredmgrclient_p.cpp \
        smfcredmgrclientsession.cpp
        
    HEADERS += smfcredmgrclient_p.h \
        smfcredmgrclientsession.h 
     
    BLD_INF_RULES.prj_exports += smfcredmgrclient.h
    TARGET.CAPABILITY = NetworkServices \
    	ReadUserData \
    	WriteUserData \
    	LocalServices \
    	UserEnvironment \
    	ReadDeviceData \
    	WriteDeviceData
  	    	
}

symbian::LIBS += -leuser \
    -lestor \
    -lefsrv \
    -lsmfcredmgrcommon.lib
    
