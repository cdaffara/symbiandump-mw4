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



#ifndef HUIFXPARAMETERMANAGER_H_
#define HUIFXPARAMETERMANAGER_H_

#include "HuiFxParameter.h"
#include "HuiFxParameterCollection.h"
#include <e32std.h>
#include <e32hashtab.h>

class CHuiFxEngine;
class TRgb;

/**
 *  A hash map containing named layer and filter parameters
 */
typedef RPtrHashMap<TDesC, MHuiFxParameter> RHuiFxParameterMap;

class CHuiFxParameterManager: public CBase, public MHuiFxParameterCollection
    {
public:
    static CHuiFxParameterManager* NewL();
    ~CHuiFxParameterManager();

    MHuiFxParameter* Parameter(const TDesC& aName);

    // does not take ownership
    void RegisterParameterL(const TDesC& aName, TRgb* aColor);
    void RegisterParameterL(const TDesC& aName, TReal32* aScalar);
    
    TBool HasAnimatedParameters() const;
    void AdvanceTime(TReal32 aElapsedTime);
    TBool Update(CHuiFxEngine& aEngine, const TRect& aVisualRect);
public: // effect cache
    CHuiFxParameterManager* CloneL() const;
    void CopyParameterL(const TDesC &aName, TReal32 *aValue, const CHuiFxParameterManager *aOldValues);
    void CopyParameterL(const TDesC &aName, TRgb *aValue, const CHuiFxParameterManager *aOldValues);
private:
    CHuiFxParameterManager();
    void ConstructL();
    void ResolveReference( TReal32& aDefaultValue, THuiFxReferencePoint aRef, CHuiFxEngine& aEngine, const TRect& aVisualRec );

    RHuiFxParameterMap      iParameters;
    };

#endif /* HUIFXPARAMETERMANAGER_H_ */
