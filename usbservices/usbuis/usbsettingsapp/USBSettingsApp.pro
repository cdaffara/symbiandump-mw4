TEMPLATE = app
TARGET = USBSettingsApp
QT += core \
    gui
CONFIG += no_icon    
HEADERS += inc/usbsettingsapp.h \
    inc/usbuimodelactive.h \
    inc/usbuisettingmodel.h
SOURCES += src/usbuimodelactive.cpp \
    src/usbuisettingmodel.cpp \
    src/main.cpp \
    src/usbsettingsapp.cpp
LIBS += -lusbwatcher
LIBS += -lxqsettingsmanager
LIBS += -lusbman
FORMS += src/USBSettingsApp.ui
CONFIG += hb
TRANSLATIONS = usbview.ts
RESOURCES += src/usbsettinglayout.qrc \
    src/usbsettingsapp.qrc
symbian: { 
    TARGET.UID3 = 0x2002E70C
    TARGET.CAPABILITY = LocalServices WriteDeviceData
}

BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
        "rom/usbsettingsapp.iby   CORE_MW_LAYER_IBY_EXPORT_PATH(usbsettingsapp.iby)" \
        "rom/usbsettingsappresources.iby   LANGUAGE_MW_LAYER_IBY_EXPORT_PATH(usbsettingsappresources.iby)"