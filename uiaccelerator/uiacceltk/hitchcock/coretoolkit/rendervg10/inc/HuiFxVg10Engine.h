/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef HUIFXV10ENGINE_H_
#define HUIFXV10ENGINE_H_

#include "HuiFxEngine.h"
#include "HuiVg10RenderPlugin.h"
#include "HuiVg10PBufferSurface.h"
#include "HuiFxVg10.h"

//FORWARD DECLARATIONS
class CHuiFxVg10OnscreenRenderbuffer;

class CHuiFxVg10Engine: public CHuiFxEngine
    {
public:
    static CHuiFxVg10Engine* NewL(CHuiVg10RenderPlugin& aPlugin);
    ~CHuiFxVg10Engine();

    void Release();
    void RestoreL();

    CHuiFxRenderbuffer* DefaultRenderbuffer();
    CHuiFxFilter* CreateFilterL(THuiFxFilterType aType);
    void Composite(CHuiFxRenderbuffer& aTarget, CHuiFxRenderbuffer& aSource, 
                   const TRect& aTargetRect, const TRect& aSourceRect,
                   THuiFxBlendingMode aMode, TInt aAlpha);
    void Composite(CHuiGc& aGc, CHuiFxRenderbuffer& aSource, const TPoint& aTargetPoint, TBool aOpaque, TInt aAlpha);
    
protected:
    void ConstructL(CHuiVg10RenderPlugin& aPlugin);
    CHuiFxRenderbuffer* AcquireNativeRenderbuffer(const TSize& aDesiredSize);
    void ReleaseNativeRenderbuffer(CHuiFxRenderbuffer* aBuffer);
private:
    CHuiVg10RenderPlugin*           iPlugin;
    CHuiFxVg10OnscreenRenderbuffer* iDefaultBuffer;
    VGPaint                         iCompPaint;
    };

#endif /*HUIFXV10ENGINE_H_*/
