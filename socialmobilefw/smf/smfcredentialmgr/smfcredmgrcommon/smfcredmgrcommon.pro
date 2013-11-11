######################################################################
#/*
#========================================================================
# Name        : smfcredmgrcommon.pro
# Author      : Pritam Roy Biswas
# Copyright   : Copyright (c) 2009-2010, Sasken Communication Technologies Ltd.
# Description : 
#========================================================================
# */
######################################################################

TEMPLATE = subdirs
symbian:
    {
    TARGET.CAPABILITY = NetworkServices \
    	ReadUserData \
    	WriteUserData \
    	LocalServices \
    	UserEnvironment \
    	ReadDeviceData \
    	WriteDeviceData

	BLD_INF_RULES.prj_exports += inc/smfcredmgrclientdatastruct.h
	BLD_INF_RULES.prj_exports += inc/smfcredmgrclientglobal.h
	BLD_INF_RULES.prj_exports += inc/smfutils.h
	BLD_INF_RULES.prj_exports += inc/smfcredmgrcommon.h
	BLD_INF_RULES.prj_mmpfiles = "group/smfcredmgrcommon.mmp"
    }

