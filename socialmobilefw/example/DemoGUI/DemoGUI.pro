# -------------------------------------------------
# Project created by QtCreator 2010-06-29T13:53:43
# -------------------------------------------------
TARGET = DemoGUI
TEMPLATE = app
QT += network
CONFIG += mobility
MOBILITY += contacts \
    location
SOURCES += ImageDownload.cpp \
    main.cpp \
    mainwindow.cpp \
    CustomListWidget.cpp \
    HomeView.cpp \
    GridView.cpp \
    ToolBar.cpp
HEADERS += ImageDownload.h \
    mainwindow.h \
    CustomListWidget.h \
    HomeView.h \
    ScreenSize.h \
    GridView.h \
    ToolBar.h \
    WidgetConstants.h
FORMS += mainwindow.ui
symbian: { 
    TARGET.CAPABILITY = NetworkServices \
        ReadUserData \
        WriteUserData \
        LocalServices \
        UserEnvironment \
        ReadDeviceData \
        WriteDeviceData
    LIBS += -lsmfclient.dll \
        -lcone \
        -leikcore \
        -lavkon \
	-lsmfcommon
    ICON = sasken_logo.svg
}
