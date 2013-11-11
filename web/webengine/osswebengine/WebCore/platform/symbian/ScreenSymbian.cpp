/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include <../bidi.h>
#include "config.h"
#include "Screen.h"

#include "IntRect.h"
#include "FloatRect.h"
#include "Widget.h"
#include <e32std.h>
#include <e32base.h>
#include <w32std.h>
#include <coemain.h>

namespace WebCore {

FloatRect screenRect(Widget* page)
{
    // Taken from KGlobalSettings::desktopGeometry
    TSize s( CCoeEnv::Static()->SystemGc().Device()->SizeInPixels() );
    return FloatRect( 0, 0, s.iWidth, s.iHeight );
}

int screenDepth(Widget* page)
{
    TDisplayMode dm = CCoeEnv::Static()->SystemGc().Device()->DisplayMode();
    switch( dm )
        {
        case ENone:     return 0;
        case EGray2:    return 1;
        case EGray4:    return 2;
        case EGray16:   return 4;
        case EGray256:  return 8;
        case EColor16:  return 4;
        case EColor256: return 8;
        case EColor64K: return 16;
        case EColor16M: return 24;
        case EColor4K:  return 12;
        default:        return 16;
        }

}


FloatRect screenAvailableRect(Widget* page)
{    
    // FIXME
    return screenRect(page);
}

float scaleFactor(Widget* page)
{
    return 1.0f;
}

bool screenIsMonochrome(Widget*)
{
    return false;
}

int screenDepthPerComponent(Widget*)
{
    return 8;
}

}
