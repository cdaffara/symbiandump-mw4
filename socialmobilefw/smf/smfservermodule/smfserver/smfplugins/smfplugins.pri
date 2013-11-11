INCLUDEPATH += smfplugins

PUBLIC_HEADERS += \
	smfplugins/smfpluginbase.h \
	smfplugins/smfproviderbase.h \
	smfplugins/smfactivityfetcherplugin.h \
	smfplugins/smfcontactfetcherplugin.h \
	smfplugins/smfeventsfetcherplugin.h \
	smfplugins/smfpostproviderplugin.h \
	smfplugins/smflyricsserviceplugin.h \
	smfplugins/smfmusicsearchplugin.h \
	smfplugins/smfmusicserviceplugin.h \
	smfplugins/smfplaylistserviceplugin.h \
	smfplugins/smfgalleryplugin.h


symbian: {
    BLD_INF_RULES.prj_exports += smfplugins/smfpluginbase.h
    BLD_INF_RULES.prj_exports += smfplugins/smfproviderbase.h
    BLD_INF_RULES.prj_exports += smfplugins/smfactivityfetcherplugin.h
    BLD_INF_RULES.prj_exports += smfplugins/smfcontactfetcherplugin.h
    BLD_INF_RULES.prj_exports += smfplugins/smfeventsfetcherplugin.h
    BLD_INF_RULES.prj_exports += smfplugins/smfpostproviderplugin.h
    BLD_INF_RULES.prj_exports += smfplugins/smflyricsserviceplugin.h
    BLD_INF_RULES.prj_exports += smfplugins/smfmusicsearchplugin.h
    BLD_INF_RULES.prj_exports += smfplugins/smfmusicserviceplugin.h
    BLD_INF_RULES.prj_exports += smfplugins/smfplaylistserviceplugin.h
    BLD_INF_RULES.prj_exports += smfplugins/smfgalleryplugin.h
}
