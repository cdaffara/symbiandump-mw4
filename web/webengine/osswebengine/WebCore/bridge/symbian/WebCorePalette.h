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

#ifndef WEBCOREPALLET_H
#define WEBCOREPALLET_H
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

class TWebCorePalette
    {
    public:
    TWebCorePalette( const TRgb& aBackGroundColor,
        const TRgb& aForeGroundColor ):
        iBackGroundColor( aBackGroundColor ),
        iForeGroundColor( aForeGroundColor )
                    {}


    const TRgb& BackGround() const { return iBackGroundColor; }
    const TRgb& ForeGround() const { return iForeGroundColor; }

    private:
        // member variables
        TRgb iBackGroundColor;
        TRgb iForeGroundColor;
    };

#endif //WEBCOREPALLET