TEMPLATE = lib
TARGET = smfcommon
QT += core \
    gui \
    network
CONFIG += mobility
MOBILITY += contacts \
    location
DEFINES += SMFCOMMON_LIB_EXPORT
PUBLIC_HEADERS += \
	smfactions.h \
	smfalbum.h \
	smfartists.h \
	smfcomment.h \
	smfcontact.h \
	smfevent.h \
	smfgroup.h \
	smflocation.h \
	smflyrics.h \
	smfmusicfingerprint.h \
	smfmusicprofile.h \
	smfmusicrating.h \
	smfpicture.h \
	smfpicturealbum.h \
	smfplaylist.h \
	smfpluginutil.h \
	smfpost.h \
	smfprovider.h \
	smfsubtitle.h \
	smftrackinfo.h \
	smfurl.h \
	smfclientglobal.h \
	smfglobal.h \
	smfobserver.h
PRIVATE_HEADERS += \
	smfactions_p.h \
	smfalbum_p.h \
	smfartists_p.h \
	smfcomment_p.h \
	smfcontact_p.h \
	smfevent_p.h \
	smfgroup_p.h \
	smflocation_p.h \
	smflyrics_p.h \
	smfmusicfingerprint_p.h \
	smfmusicprofile_p.h \
	smfmusicrating_p.h \
	smfpicture_p.h \
	smfpicturealbum_p.h \
	smfplaylist_p.h \
	smfpost_p.h \
	smfsubtitle_p.h \
	smftrackinfo_p.h \
	smfurl_p.h \
	smfobserver.h
SOURCES += \
	smfactions.cpp \
	smfalbum.cpp \
	smfartists.cpp \
	smfcomment.cpp \
	smfcontact.cpp \
	smfevent.cpp \
	smfgroup.cpp \
	smflocation.cpp \
	smflyrics.cpp \
	smfmusicfingerprint.cpp \
	smfmusicprofile.cpp \
	smfmusicrating.cpp \
	smfpicture.cpp \
	smfpicturealbum.cpp \
	smfplaylist.cpp \
	smfpluginutil.cpp \
	smfpost.cpp \
	smfprovider.cpp \
	smfsubtitle.cpp \
	smftrackinfo.cpp \
	smfurl.cpp    
HEADERS += $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS
symbian: { 
    TARGET.UID3 = 0xEE9F4502
    TARGET.CAPABILITY = NetworkServices \
    	ReadUserData \
        WriteUserData \
        LocalServices \
        UserEnvironment \
        ReadDeviceData \
        WriteDeviceData
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += -lqjson.dll -lsmfcredmgrclient
}

symbian: {
  #export public header to \epocroot\epoc32\include to be able to use them
  headers.files = $$PUBLIC_HEADERS
  headers.path = $$PWD
  for(header, headers.files) {
    {BLD_INF_RULES.prj_exports += "$$header"}
  }
}
