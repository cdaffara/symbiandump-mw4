# /*
# * Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
# * All rights reserved.
# * This component and the accompanying materials are made available
# * under the terms of the License "Eclipse Public License v1.0"
# * which accompanies this distribution, and is available
# * at the URL "http://www.eclipse.org/legal/epl-v10.html".
# *
# * Initial Contributors:
# * Nokia Corporation - initial contribution.
# *
# * Contributors:
# *
# * Description:  
# *
# */

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=\epoc32\data\z
endif

# ----------------------------------------------------------------------------
# TODO: Configure these
# ----------------------------------------------------------------------------

TARGETDIR=$(ZDIR)\Resource\APPS
HEADERDIR=\epoc32\include

ifneq (,$(findstring 1,$(BROWSER_SDK)))
ICONTARGETFILENAME=$(TARGETDIR)\WebKitIcons_sdk.MIF
HEADERFILENAME=$(HEADERDIR)\WebKitIcons_sdk.MBG
else
ICONTARGETFILENAME=$(TARGETDIR)\webkiticons.mif
HEADERFILENAME=$(HEADERDIR)\webkiticons.mbg
endif

do_nothing :

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
# TODO: Configure these.
#
# NOTE 1: DO NOT DEFINE MASK FILE NAMES! They are included automatically by
# MifConv if the mask detph is defined.
#
# NOTE 2: Usually, source paths should not be included in the bitmap
# definitions. MifConv searches for the icons in all icon directories in a
# predefined order, which is currently \s60\icons, \s60\bitmaps2.
# The directory \s60\icons is included in the search only if the feature flag
# __SCALABLE_ICONS is defined.
# ----------------------------------------------------------------------------

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
/c8,1 qgn_indi_wml_image_not_shown.svg\
/c8,1 qgn_prop_wml_selectarrow.svg\
/c8,1 qgn_prop_wml_object.svg\
/c8,1 qgn_prop_wml_radiobutt_off.svg\
/c8,1 qgn_prop_wml_radiobutt_on.svg\
/c8,1 qgn_prop_wml_checkbox_off.svg\
/c8,1 qgn_prop_wml_checkbox_on.svg\
/c8,1 qgn_prop_wml_selectfile.svg\
/c8,8 qgn_indi_viewer_pointer.svg\
/c8,8 qgn_indi_viewer_pointer_hand.svg\
/c8,8 qgn_indi_viewer_pointer_wait.svg\
/c8,8 qgn_prop_nrtyp_phone.svg\
/c8,8 qgn_indi_ai_nt_message.svg\
/c8,8 qgn_indi_browser_url_call.svg\
/c8,8 qgn_indi_browser_url_email.svg\
/c8,8 qgn_indi_browser_url_mm.svg\
/c8,8 qgn_indi_browser_url_mms.svg\
/c8,8 qgn_indi_browser_url_page.svg\
/c8,8 qgn_indi_browser_url_video.svg\
/c8,8 qgn_indi_viewer_pointer_text.svg\
/c8,8 qgn_indi_button_exit_fs.svg


FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
