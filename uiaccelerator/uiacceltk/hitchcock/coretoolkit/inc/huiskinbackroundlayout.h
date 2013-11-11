/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Defines CHuiDisplay, a drawing buffer into which a the toolkit 
*                contents (visuals) are drawn.
*
*/


#ifndef __HUISKINBACKGROUNDLAYOUT_H__
#define __HUISKINBACKGROUNDLAYOUT_H__

#include <AknsConstants.h>
#include <aknlayout.cdl.h>
#include <skinlayout.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include "layoutmetadata.cdl.h"



TAknsItemID GetParent( const TAknsItemID& aIID );
TBool GetItemLayout( const TAknsItemID& aIID, const TRect& aParentRect, TRect& aRect  );
IMPORT_C void GetRectForItem(const TAknsItemID& aID, TRect& aParentRect, TRect& aOutRect);

#endif

