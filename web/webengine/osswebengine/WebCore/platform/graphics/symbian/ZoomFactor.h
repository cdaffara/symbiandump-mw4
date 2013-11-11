/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

#ifndef __ZOOMFACTOR_H__
#define __ZOOMFACTOR_H__

#include <e32base.h>

namespace WebCore {

static int fontZoomFactor();

TInt xForm(TInt x);

TPoint xForm(const TPoint& pt);

TSize xForm(const TSize& sz);

TRect xForm(const TRect& rect);

}

#endif //!__ZOOMFACTOR_H__
