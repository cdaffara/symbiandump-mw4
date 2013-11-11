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
* Description:  Implemetation of PictographSymbian
*
*/


#ifndef PICTOGRAPHSYMBIAN_H
#define PICTOGRAPHSYMBIAN_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <AknPictographInterface.h>
#include <AknPictographDrawerInterface.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class WebCoreGraphicsContext;
class GraphicsContext;
class CBitmapContext;

namespace WebCore 
{

/**PictographSymbian
*/
class PictographSymbian : public CBase, public MAknPictographAnimatorCallBack
    {
    public:     
        PictographSymbian();
        ~PictographSymbian();

    public:
        void DrawPictographsInText(WebCoreGraphicsContext* aContext,
                                    CBitmapContext& aGc, const CFont& aFont, 
                                    const TDesC& aText, const TPoint& aPosition);
        
        void DrawPictographsInText(WebCoreGraphicsContext* aContext,
                                    CBitmapContext& aGc, const CFont& aFont, 
                                    const TDesC& aText, const TRect& aBox, 
                                    TInt aBaselineOffset, 
                                    CGraphicsContext::TTextAlign aAlignment, 
                                    TInt aLeftMargin );
        
        void DrawPictographArea();
        
        
    private:
        CAknPictographInterface* iPictographInterface;        
        WebCoreGraphicsContext* iContext;

    };
    
};


#endif  // PICTOGRAPHSYMBIAN_H

// End of File


