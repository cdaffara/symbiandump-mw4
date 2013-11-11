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
* Description:  Handles the viewing of a single frame. If the page is not frame
*               enabled, this class is used as the single view.  If frame
*               enabled, there is one instance of this class for each frame.
*
*/

#ifndef WEBSPRITE_H_
#define WEBSPRITE_H_

#include <e32base.h>
#include <w32std.h>
#include <coecntrl.h>

class CFbsBitmap;


NONSHARABLE_CLASS(CWebSprite) : public CCoeControl
{
    public:
        static CWebSprite* NewL(TPoint& aPos, CFbsBitmap* aBitmap, CFbsBitmap* aMask, 
                                TBool aInvertMask);
        static CWebSprite* NewL(CCoeControl* aParent, TPoint& aPos, CFbsBitmap* aBitmap, 
                                CFbsBitmap* aMask, TBool aInvertMask);
        CWebSprite();
        virtual ~CWebSprite();
        
        void Update(const TPoint& aPos, CFbsBitmap* aBitmap, CFbsBitmap* aMask);
        void Update(CFbsBitmap* aBitmap, CFbsBitmap* aMask);
        void Update(CFbsBitmap* aBitmap, CFbsBitmap* aMask, TBool aInvertMask);
        void SetBitmap(CFbsBitmap* aBitmap, CFbsBitmap* aMask, TBool aInvertMask);
        void Hide();
        void Show();
        void Draw(const TRect& aRect) const;
        void SetPos(const TPoint& aPos);
        TPoint GetPos() { return Position(); }
        TBool IsShown();
        
    protected:
        void BaseConstructL(TPoint& aPos, CFbsBitmap* aBitmap, CFbsBitmap* aMask, 
                            TBool aInvertMask);
        void BaseConstructL(CCoeControl* aParent, TPoint& aPos, CFbsBitmap* aBitmap, 
                            CFbsBitmap* aMask, TBool aInvertMask);
    private:
        CWebSprite(CCoeControl* aParent, CFbsBitmap* aBitmap, CFbsBitmap* aMask, TBool aInvertMask);
        void ConstructL(TPoint& aPos);
        void ConstructL(CCoeControl* aParent, TPoint& aPos);                      
        void init(TPoint& aPos, TSize aSize);
        void CreateSprite(TPoint& aPos);
        void UpdateSprite();
        
    private:
        CFbsBitmap*  iBitmap;
        CFbsBitmap*  iMask;
        TBool        iInvertMask;

        RWsSprite    iSprite;
        RWindow*     iWin;
        CCoeControl* iParent;
};

#endif /* WEBSPRITE_H_ */
