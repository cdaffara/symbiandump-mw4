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


#include "config.h"
#include "SystemTime.h"

#include <e32std.h>

namespace WebCore {

double currentTime()
    {
    TTime current;
    current.HomeTime();
    // second resolution instead of microsecond
    return I64REAL( current.Int64() ) / 1000000;
    }

}
