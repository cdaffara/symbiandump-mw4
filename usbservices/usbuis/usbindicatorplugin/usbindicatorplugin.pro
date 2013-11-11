TEMPLATE = lib
TARGET = usbindicatorplugin
CONFIG += plugin
CONFIG += hb
HEADERS += inc/usbindicatorplugin.h \
    inc/usbindicator.h \
    inc/usbaddressedindicator.h \
    inc/usbmassstorageindicator.h \
    inc/usbdisconnectingindicator.h
	
SOURCES += src/usbindicatorplugin.cpp \
    src/usbindicator.cpp \
    src/usbaddressedindicator.cpp \
    src/usbmassstorageindicator.cpp \
    src/usbdisconnectingindicator.cpp

LIBS += -lmsmmindicatorsession

SYMBIAN_PLATFORMS = ARMV5

TRANSLATIONS = usbindimenu.ts	

symbian { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x2002E70D
    pluginstub.sources = usbindicatorplugin.dll
    pluginstub.path = /resource/plugins/indicators
    DEPLOYMENT += pluginstub
}
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "qmakepluginstubs/usbindicatorplugin.qtplugin /epoc32/data/z/pluginstub/usbindicatorplugin.qtplugin" \
    "rom/usbindicatorplugin.iby CORE_APP_LAYER_IBY_EXPORT_PATH(usbindicatorplugin.iby)" \
     "rom/usbindicatorpluginresources.iby   LANGUAGE_MW_LAYER_IBY_EXPORT_PATH(usbindicatorpluginresources.iby)"
