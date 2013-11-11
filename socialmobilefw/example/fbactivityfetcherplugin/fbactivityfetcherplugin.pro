TEMPLATE = lib

CONFIG += plugin \
	mobility
	
MOBILITY += contacts
	
QT += core \
	xml \
	network
	
HEADERS = fbactivityfetcherplugin.h

SOURCES = fbactivityfetcherplugin.cpp

TARGET = $$qtLibraryTarget(fbactivityfetcherplugin)

symbian: { 
    # Load predefined include paths (e.g. QT_PLUGINS_BASE_DIR) to be used in the pro-files
    load(data_caging_paths)
    
    # EPOCALLOWDLLDATA have to set true because Qt macros has initialised global data
    TARGET.EPOCALLOWDLLDATA = 1
    
    # Defines plugin files into Symbian .pkg package
    pluginDep.sources = fbactivityfetcherplugin.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR/smf/plugin/activity.fetcher
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

target.path += $$[QT_INSTALL_PLUGINS]/smf/plugin/activity.fetcher

INSTALLS += target