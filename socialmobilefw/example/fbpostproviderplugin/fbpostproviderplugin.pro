TEMPLATE = lib

CONFIG += plugin \
	mobility
	
MOBILITY += contacts \ 
	location
	
QT += core \
	xml \
	network

HEADERS = fbpostproviderplugin.h

SOURCES = fbpostproviderplugin.cpp

TARGET = $$qtLibraryTarget(fbpostproviderplugin)

symbian: { 
    # Load predefined include paths (e.g. QT_PLUGINS_BASE_DIR) to be used in the pro-files
    load(data_caging_paths)
    
    # EPOCALLOWDLLDATA have to set true because Qt macros has initialised global data
    TARGET.EPOCALLOWDLLDATA = 1
    
    # Defines plugin files into Symbian .pkg package
    pluginDep.sources = fbpostproviderplugin.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR/smf/plugin/contact.posts
    DEPLOYMENT += pluginDep
    
    TARGET.CAPABILITY = NetworkServices \
    	ReadUserData \
    	WriteUserData \
    	LocalServices \
    	UserEnvironment \
    	ReadDeviceData \
    	WriteDeviceData
    
   LIBS += -lsmfcommon
}

target.path += $$[QT_INSTALL_PLUGINS]/smf/plugin/contact.posts

INSTALLS += target