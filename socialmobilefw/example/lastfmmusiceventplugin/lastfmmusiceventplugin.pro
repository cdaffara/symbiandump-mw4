TEMPLATE = lib
CONFIG += plugin \
    mobility
MOBILITY += location \
			contacts
QT += core \
    network
HEADERS = lastfmmusiceventplugin.h
SOURCES = lastfmmusiceventplugin.cpp
TARGET = $$qtLibraryTarget(lastfmmusiceventplugin)
symbian: { 
    # Load predefined include paths (e.g. QT_PLUGINS_BASE_DIR) to be used in the pro-files
    load(data_caging_paths)
    
    # EPOCALLOWDLLDATA have to set true because Qt macros has initialised global data
    TARGET.EPOCALLOWDLLDATA = 1
    
    # Defines plugin files into Symbian .pkg package
    pluginDep.sources = lastfmmusiceventplugin.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR/smf/plugin/events.fetcher
    DEPLOYMENT += pluginDep
    TARGET.CAPABILITY = ReadUserData \
        WriteUserData \
        LocalServices \
        NetworkServices \
        UserEnvironment \
        ReadDeviceData \
        WriteDeviceData
    LIBS +=  -lsmfcommon.dll 
     LIBS +=  -lqjson.dll
    
}
target.path += $$[QT_INSTALL_PLUGINS]/smf/plugin/events.fetcher
INSTALLS += target



