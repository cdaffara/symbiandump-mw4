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

DESTDIR = \epoc32\include\oem
SRCDIR = sdkinc

ifeq ($(S60WEBKIT_SDK),1)
MAKMAKE : export_headers
CLEAN:
	@echo Removing WebCore exported headers from $(DESTDIR)
	@perl -le "chomp (@f=`ls -1 $(SRCDIR)`); chdir '$(DESTDIR)'; unlink @f"
else
MAKMAKE : noop
CLEAN : noop
endif

noop  :

export_headers :
	@echo Exporting headers to $(DESTDIR) via copy sdkinc\* $(DESTDIR)
	@copy /y sdkinc\* $(DESTDIR)

CLEAN : noop

BLD : noop

LIB : noop

CLEANLIB : noop

RESOURCE : noop

FREEZE : noop

SAVESPACE : noop

RELEASABLES : noop

FINAL : noop
