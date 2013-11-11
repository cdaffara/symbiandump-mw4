TEMPLATE = lib

CONFIG += plugin \
	mobility
	
MOBILITY += contacts \ 
	location
	
QT += core \
	xml \
	network
	
#DEFINES += SMF_XMLPARSING
	
HEADERS = flickrcontactfetcherplugin.h

SOURCES = flickrcontactfetcherplugin.cpp

TARGET = $$qtLibraryTarget(flickrcontactfetcherplugin)

symbian: { 
    # Load predefined include paths (e.g. QT_PLUGINS_BASE_DIR) to be used in the pro-files
    load(data_caging_paths)
    
    # EPOCALLOWDLLDATA have to set true because Qt macros has initialised global data
    TARGET.EPOCALLOWDLLDATA = 1
    
    # Defines plugin files into Symbian .pkg package
    pluginDep.sources = flickrcontactfetcherplugin.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR/smf/plugin/contact.fetcher
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

target.path += $$[QT_INSTALL_PLUGINS]/smf/plugin/contact.fetcher

INSTALLS += target