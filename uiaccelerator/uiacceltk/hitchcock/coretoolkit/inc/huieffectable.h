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



#ifndef HUIEFFECTABLE_H_
#define HUIEFFECTABLE_H_

class THuiRealRect;
class CHuiFxEffect;
class CHuiGc;

class MHuiEffectable
    {
public:
    virtual ~MHuiEffectable() { }
    virtual void EffectSetEffect(CHuiFxEffect* aEffect)=0;
    virtual TReal32 EffectOpacityTarget() const=0;
    virtual void EffectSetOpacity(TReal32 aOpacity)=0;
    virtual void EffectDrawSelf( CHuiGc &aGc, const TRect & aDisplayRect) const=0;
    virtual THuiRealRect EffectDisplayRect() const __SOFTFP=0;
    virtual void SetLoadingEffect(TBool aLoading)=0;
    virtual void EffectSetSource( TBool aIsInput1 )=0;
    virtual TBool EffectGetSource() const=0;
    };
    
#endif    
