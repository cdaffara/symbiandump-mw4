/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
// LayoutMetaData_MSK generated by
// cdlcompiler instance ..\cdl\LayoutMetaData.cdl LayoutMetaData_MSK
// This customisation implements the interface defined in LayoutMetaData.cdl

#include "layoutmetadata_msk.h"

namespace LayoutMetaData_MSK
{

TBool IsMSKEnabled()
	{
	return ETrue;
	}


const Layout_Meta_Data::SCdlImpl KCdlImpl = 
	{
	NULL, // &IsLandscapeOrientation
	NULL, // &IsMirrored
	NULL, // &IsScrollbarEnabled
	NULL, // &IsAPAC
	NULL, // &IsPenEnabled
	NULL, // &IsListStretchingEnabled,
	&IsMSKEnabled,
	NULL, // &IsTouchPaneEnabled,
	};

} // end of namespace LayoutMetaData_MSK
