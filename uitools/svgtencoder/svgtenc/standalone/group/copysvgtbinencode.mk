#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
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
# ======================================================================
# Makefile to copy the standalone encoder binary to tools dir
# ======================================================================

SVGBETARGETDIR = \epoc32\tools

ERASE = @erase /f 2>>nul

SVGBESOURCEDIR=..\lib
PROGRAM=SVGTBinEncode.exe
XERCES_DLL=xerces-c_2_6.dll


do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing


SAVESPACE BLD : 
		perl -S emkdir.pl $(SVGBETARGETDIR) && \
		perl -S ecopyfile.pl $(SVGBESOURCEDIR)\$(PROGRAM) $(SVGBETARGETDIR)\$(PROGRAM) && \
		perl -S ecopyfile.pl $(SVGBESOURCEDIR)\$(XERCES_DLL) $(SVGBETARGETDIR)\$(XERCES_DLL)
	

CLEAN : 
	cd $(SVGBETARGETDIR) && \
	$(ERASE) "$(PROGRAM)" && \
	$(ERASE) "$(XERCES_DLL)"
	
LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing

FREEZE : do_nothing

RELEASABLES : 
	if exist $(SVGBETARGETDIR)\$(PROGRAM) echo $(SVGBETARGETDIR)\$(PROGRAM) 
	if exist $(SVGBETARGETDIR)\$(XERCES_DLL) echo $(SVGBETARGETDIR)\$(XERCES_DLL)

FINAL : do_nothing

