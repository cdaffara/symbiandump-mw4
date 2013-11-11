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
# For template extension makefiles, must include the following lines when using the EPOCROOT
# To ensure that EPOCROOT always ends with a forward slash. 
TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

do_nothing :
	@echo do_nothing

MAKMAKE : do_nothing

BLD :
	@$(EPOCROOT)EPOC32/TOOLS/series60color.exe

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :

FINAL : do_nothing

