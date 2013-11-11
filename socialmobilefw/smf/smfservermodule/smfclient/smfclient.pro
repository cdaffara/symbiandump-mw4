TEMPLATE = lib
TARGET = smfclient
QT += core \
    gui \
    network
CONFIG += mobility
MOBILITY += contacts \
    location
DEFINES += SMFCLIENT_LIB_EXPORT
PUBLIC_HEADERS += smfclient.h \
    smfcontactfetcher.h \
    smfpostprovider.h \
    smfgallery.h \
    smfrelationmgr.h \
    smfmusic.h \
    smfactivityfetcher.h \
    smfeventsfetcher.h
PRIVATE_HEADERS += smfclient_p.h \
    smfcontactfetcher_p.h \
    smfpostprovider_p.h \
    smfgallery_p.h \
    smfrelationmgr_p.h \
    smfmusic_p.h \
    smfactivityfetcher_p.h \
    smfeventsfetcher_p.h
SOURCES +=  \
    smfrelationmgr_p.cpp \
    smfrelationmgr.cpp \
    smfgallery.cpp \
    smfgallery_p.cpp \
    smfpostprovider.cpp \
    smfpostprovider_p.cpp \
    smfmusic.cpp \
    smfclient.cpp \
    smfclient_p.cpp \
    smfcontactfetcher.cpp \
    smfcontactfetcher_p.cpp \
    smfmusic_p.cpp \
    smfactivityfetcher.cpp \
    smfactivityfetcher_p.cpp \
    smfeventsfetcher.cpp \
    smfeventsfetcher_p.cpp
HEADERS += $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS
!symbian { 
    include(../../desktop.pri)
    LIBS += -lqjson
}
symbian: { 
    TARGET.UID3 = 0xE08059D4
    TARGET.CAPABILITY = NetworkServices \
        ReadUserData \
        WriteUserData \
        LocalServices \
        UserEnvironment \
        ReadDeviceData \
        WriteDeviceData
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += -lsmfcommon


}
symbian: { 
	PRIVATE_HEADERS += smfclientsymbian.h \
        smfclientsymbiansession.h
    SOURCES += smfclientsymbian.cpp
}
else { 
    PRIVATE_HEADERS += smfclientqt.h
    SOURCES += smfclientqt.cpp
}

symbian: {
  #export public header to \epocroot\epoc32\include to be able to use them
  headers.files = $$PUBLIC_HEADERS
  headers.path = $$PWD
  for(header, headers.files) {
    {BLD_INF_RULES.prj_exports += "$$header"}
  }
}

