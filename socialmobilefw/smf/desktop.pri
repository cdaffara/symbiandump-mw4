# Set up destination and link variables 

!symbian {
    DESTDIR = $$PWD/../bin

    INCLUDEPATH += $$PWD/smfservermodule/util/qjson/src
    LIBS += -L$$PWD/../bin -Wl,-rpath,$$PWD/../bin
}
