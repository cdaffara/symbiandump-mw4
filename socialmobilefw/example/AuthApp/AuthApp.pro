TEMPLATE = app
TARGET = AuthApp
QT += core \
    gui \
    network \
    webkit \
    xml
LIBS += -lsmfcredmgrclient
HEADERS += inc/keys.h \
    progressbar.h \
    inc/requestSP.h \
    inc/xmlParser.h \
    inc/sessionSP.h \
    inc/loginDialog.h \
    inc/errorCodes.h \
    inc/baseDialog.h \
    inc/authAppConstants.h \
    AuthApp.h
SOURCES += progressbar.cpp \
    src/requestSP.cpp \
    src/xmlParser.cpp \
    src/sessionSP.cpp \
    src/loginDialog.cpp \
    src/baseDialog.cpp \
    main.cpp \
    AuthApp.cpp
FORMS += progressbar.ui \
    AuthApp.ui
RESOURCES += 
symbian: { 
    TARGET.EPOCHEAPSIZE = 0x020000 \
        0x3200000
    TARGET.UID3 = 0xEFE2FD23 # Symbian specific, unprotected uid3 from symbiansigned.com
    TARGET.CAPABILITY = NetworkServices # Symbian specific, we just need network access
}
