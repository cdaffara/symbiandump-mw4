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

# ----------------------------------------------------------------------------
# TODO: Need some way to determine if we are compiling 'internally' ie within 
# S60 RnD environment, in which case deriving the JS bindings is skipped. 
# ----------------------------------------------------------------------------

DESTDIR = ../../../../DerivedSources/WebCore

noop  :

EXPORT : noop

MAKMAKE :
	@echo Generating derived sources in $(DESTDIR)
	bash -x build-generated-files.sh

CLEAN :
	@echo Removing $(DESTDIR)
	@rm -rf $(DESTDIR)

BLD : noop

LIB : noop

CLEANLIB : noop

RESOURCE : noop

FREEZE : noop

SAVESPACE : noop

RELEASABLES : noop

FINAL : noop
