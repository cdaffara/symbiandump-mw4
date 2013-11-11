TEMPLATE = lib
CONFIG += plugin \
    mobility
MOBILITY += location 
QT += core \
    xml \
    network
HEADERS = musiclyricsplugin.h
SOURCES = musiclyricsplugin.cpp
TARGET = $$qtLibraryTarget(musiclyricsplugin)
symbian: { 
    # Load predefined include paths (e.g. QT_PLUGINS_BASE_DIR) to be used in the pro-files
    load(data_caging_paths)
    
    # EPOCALLOWDLLDATA have to set true because Qt macros has initialised global data
    TARGET.EPOCALLOWDLLDATA = 1
    
    # Defines plugin files into Symbian .pkg package
    pluginDep.sources = musiclyricsplugin.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR/smf/plugin/music.lyrics
    DEPLOYMENT += pluginDep
    TARGET.CAPABILITY = ReadUserData \
        WriteUserData \
        LocalServices \
        NetworkServices \
        UserEnvironment \
        ReadDeviceData \
        WriteDeviceData 
    LIBS += -lsmfclient.dll
    LIBS +=  -lsmfcommon
    LIBS += -lqjson.dll
}
target.path += $$[QT_INSTALL_PLUGINS]/smf/plugin/music.lyrics
INSTALLS += target



