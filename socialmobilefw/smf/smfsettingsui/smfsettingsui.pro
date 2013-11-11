TEMPLATE = app
TARGET = SMFSettings
QT += core \
    gui
HEADERS += SettingsConstants.h \
    PlatfromIndependentSettings.h \
    SettingsRepository.h \
    SMFSettings.h
SOURCES += PlatfromIndependentSettings.cpp \
    SettingsRepository.cpp \
    SMFSettings_reg.rss \
    main.cpp \
    SMFSettings.cpp
FORMS += SMFSettings.ui
TRANSLATIONS += SMFSettingsClass_la.ts
RESOURCES += 
symbian:
 { 
    TARGET.UID3 = 0xE6313AF5
    BLD_INF_RULES.prj_exports += SettingsConstants.h
    LIBS += -lcentralrepository \
        -leuser
    TARGET.CAPABILITY = "WriteUserData CommDD WriteDeviceData ReadDeviceData ReadUserData NetworkServices"
}

