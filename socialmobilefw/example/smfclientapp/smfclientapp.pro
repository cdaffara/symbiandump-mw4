TEMPLATE = app
TARGET = smfclientapp
DEFINES += WRITE_LOG
QT += core \
    gui \
    network \
    webkit
CONFIG += mobility
MOBILITY = contacts \
    location
HEADERS += posttestui.h \
    displaywidget.h \
    testscreen.h \
    smfclientapp.h
SOURCES += posttestui.cpp \
    displaywidget.cpp \
    testscreen.cpp \
    smfclientapp_reg.rss \
    main.cpp \
    smfclientapp.cpp
FORMS += posttestui.ui \
    displaywidget.ui \
    smfclientapp.ui
RESOURCES += 
symbian:TARGET.UID3 = 0xE08059D6
symbian:LIBS += -lsmfclient -lsmfcommon
symbian:TARGET.CAPABILITY = ReadUserData \
    WriteUserData \
    LocalServices \
    NetworkServices \
    UserEnvironment \
    ReadDeviceData \
    WriteDeviceData
