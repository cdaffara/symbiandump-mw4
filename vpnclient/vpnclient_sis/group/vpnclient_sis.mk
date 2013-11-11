#
# Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  Creates either localised or nonlocalised mVPN Client SIS files 
#

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD :
	perl create_vpnclient_sis.pl $(CFG)
	
CLEAN : 
	if exist mVPN_RnD*.sis erase mVPN_RnD*.sis
	if exist temp_* erase temp_*
	
LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing		
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES : do_nothing

FINAL : do_nothing
