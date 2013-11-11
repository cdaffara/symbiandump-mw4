#
# Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#


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

ICONTARGETFILENAME=$(TARGETDIR)\webkitutilsicons.mif
HEADERFILENAME=$(HEADERDIR)\webkitutilsicons.mbg

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
/c8,8 qgn_indi_browser_tb_back.svg\
/c8,8 qgn_indi_browser_tb_page.svg\
/c8,8 qgn_indi_browser_tb_reload.svg\
/c8,8 qgn_indi_browser_tb_find.svg\
/c8,8 qgn_indi_browser_tb_rss.svg\
/c8,8 qgn_prop_pinb_link_bm_id.svg\
/c8,8 qgn_indi_tp_toolbar.svg\
/c8,8 qgn_indi_button_preview_close.svg\
/c8,8 qgn_indi_browser_tb_help.svg\
/c8,8 qgn_indi_browser_tb_switch_win.svg\
/c8,8 qgn_indi_browser_tb_urls.svg\
/c8,8 qgn_indi_browser_tb_feeds.svg\
/c8,8 qgn_indi_browser_tb_save_page.svg\
/c8,8 qgn_indi_browser_tb_view_images.svg\
/c8,8 qgn_indi_browser_tb_view_pages.svg\
/c8,8 qgn_indi_browser_tb_home.svg\
/c8,8 qgn_indi_browser_tb_bm.svg\
/c8,8 qgn_indi_browser_tb_rotate.svg\
/c8,8 qgn_indi_browser_tb_settings.svg\
/c8,8 qgn_indi_browser_tb_zoom_out.svg\
/c8,8 qgn_indi_browser_tb_zoom_in.svg\
/c8,8 qgn_indi_browser_tb_find_kw.svg\
/c8,8 qgn_indi_browser_tb_save_bm.svg\
/c8,8 qgn_indi_browser_tb_goto.svg\
/c8,8 qgn_indi_browser_tb_tb.svg\
/c8,8 qgn_indi_browser_tb_key_map.svg\
/c8,8 qgn_indi_button_exit_fs.svg

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
