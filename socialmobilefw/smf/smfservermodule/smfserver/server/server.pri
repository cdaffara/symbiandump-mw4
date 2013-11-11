INCLUDEPATH += server \
    ./
PUBLIC_HEADERS += server/smfserver.h \
    server/smfsettingshandler.h
    
SOURCES += server/smfserver.cpp \
    server/smfsettingshandler.cpp
    
symbian { 
    PRIVATE_HEADERS += server/smfserversymbian_p.h \
        server/smfsettingshandler_symbian.h \
		server/smfsettingshandler_qt.h
            
    SOURCES += server/smfserversymbian.cpp \
        server/smfsettingshandler_symbian.cpp \
        server/smfsettingshandler_qt.cpp
}
else { 
    PRIVATE_HEADERS += server/smfserverqt_p.h \
        server/smfserverqtsession.h
        
    SOURCES += server/smfserverqt.cpp \
        server/smfserverqtsession.cpp
}
