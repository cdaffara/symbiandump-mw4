TEMPLATE = lib
TARGET = usbuinotif
CONFIG += HB
HEADERS += inc/usbnotifier.h \
    inc/usbuincableconnectednotifier.h \
    inc/usbuinotifdebug.h \
    inc/usbuinotifmsmmerror.h \
    inc/usbuinotifotgerror.h \
    inc/usbuinotifotgwarning.h \
    inc/usbuinqueriesnotifiermdrv.h
SOURCES += src/usbuincableconnectednotifier.cpp \
    src/usbuinqueriesnotifiermdrv.cpp \
    src/usbnotifier.cpp \
    src/usbuinotifotgwarning.cpp \
    src/usbuinotifotgerror.cpp \
    src/usbuinotifmsmmerror.cpp \
    src/usbuinmain.cpp
TRANSLATIONS = usbdialogs.ts

defBlock = \      
    "$${LITERAL_HASH}if defined(EABI)" \
    "DEFFILE  .\eabi/usbavkonnotif.def" \
    "$${LITERAL_HASH}else" \
    "DEFFILE  .\BWINS/usbavkonnotif.def" \
    "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock


symbian*: { 
    TARGET.UID2 = 0x10009D8D 
    TARGET.UID3 = 0x102068DC
    TARGET.CAPABILITY = CAP_ECOM_PLUGIN 

     ecomResourceBlock = \
      "$${LITERAL_HASH}include <data_caging_paths.hrh>" \
      "START RESOURCE   ../data/102068DC.rss" \
      "TARGET           usbuinotif.rsc" \
      "END"   

   MMP_RULES += ecomResourceBlock
   MMP_RULES += "TARGETTYPE plugin"  

    BLD_INF_RULES.prj_exports += \
      "loc/usbuinotif.loc MW_LAYER_LOC_EXPORT_PATH(usbuinotif.loc)" \
      "rom/usbuinotif.iby CORE_MW_LAYER_IBY_EXPORT_PATH(usbuinotif.iby)" \
      "rom/usbuinotifresources.iby LANGUAGE_MW_LAYER_IBY_EXPORT_PATH(usbuinotifresources.iby)" 

   LIBS += -lecom \
   -lbafl \
   -lcommonengine \
   -lcentralrepository \
   -lusbman \
   -lflogger \
   -lcone \
   -leiksrv
}

