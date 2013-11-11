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



#include "HuiFxParameterManager.h"
#include "HuiFxEngine.h"
#include "HuiFxScalarParameter.h"
#include "HuiFxColorParameter.h"
#include <e32base.h>

CHuiFxParameterManager* CHuiFxParameterManager::NewL()
    {
    CHuiFxParameterManager* self = new (ELeave) CHuiFxParameterManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CHuiFxParameterManager::CHuiFxParameterManager()
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxParameterManager::CHuiFxParameterManager - 0x%x "), this);
#endif
    }

void CHuiFxParameterManager::ConstructL()
    {
    }

CHuiFxParameterManager* CHuiFxParameterManager::CloneL() const
{
  CHuiFxParameterManager *params = CHuiFxParameterManager::NewL();
  return params;
}

void CHuiFxParameterManager::CopyParameterL(const TDesC &aName, TReal32 *aValue, const CHuiFxParameterManager *aOldValues)
    {
    MHuiFxParameter *param = const_cast<CHuiFxParameterManager*>(aOldValues)->Parameter(aName);
    MHuiFxParameter *param2 = param->CloneL();
    param2->ReRef(aValue);
    iParameters.InsertL(aName.AllocL(), param2);
    }
void CHuiFxParameterManager::CopyParameterL(const TDesC &aName, TRgb *aValue, const CHuiFxParameterManager *aOldValues)
    {
    MHuiFxParameter *param = const_cast<CHuiFxParameterManager*>(aOldValues)->Parameter(aName);
    MHuiFxParameter *param2 = param->CloneL();
    param2->ReRef(aValue);
    iParameters.InsertL(aName.AllocL(), param2);
    }


CHuiFxParameterManager::~CHuiFxParameterManager()
    {
    // Free registered parameters
    /*TPtrHashMapIter<TDesC, MHuiFxParameter> i(iParameters);
    for (i.NextValue(); i.CurrentValue(); i.NextValue())
        {
        delete i.CurrentKey();
        delete i.CurrentValue();
        }
    iParameters.Close();*/
    iParameters.ResetAndDestroy();
    iParameters.Close();
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxParameterManager::~CHuiFxParameterManager - 0x%x "), this);
#endif
    }

void CHuiFxParameterManager::RegisterParameterL(const TDesC& aName, TReal32* aValue)
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxParameterManager::RegisterParameterL - 0x%x %S scal: 0x%x"), this, &aName, *aValue);
#endif
    MHuiFxParameter* param = new (ELeave) CHuiFxScalarParameter(aValue);
    CleanupStack::PushL(param);
    iParameters.InsertL(aName.AllocL(), param);
    CleanupStack::Pop(param);
    }

void CHuiFxParameterManager::RegisterParameterL(const TDesC& aName, TRgb* aValue)
    {
#ifdef HUIFX_TRACE    
    RDebug::Print(_L("CHuiFxParameterManager::RegisterParameterL - 0x%x %S rgb: 0x%x"), this, &aName, *aValue);
#endif //#ifdef HUIFX_TRACE    
    MHuiFxParameter* param = new (ELeave) CHuiFxColorParameter(aValue);
    CleanupStack::PushL(param);
    iParameters.InsertL(aName.AllocL(), param);
    CleanupStack::Pop(param);
    }

TBool CHuiFxParameterManager::HasAnimatedParameters() const
    {
    TPtrHashMapIter<TDesC, MHuiFxParameter> i(iParameters);
    for (i.NextValue(); i.CurrentValue(); i.NextValue())
        {
        if (i.CurrentValue()->IsAnimated())
            {
            return ETrue;
            }
        }
    return EFalse;
    }

void CHuiFxParameterManager::AdvanceTime(TReal32 aElapsedTime)
    {
    TPtrHashMapIter<TDesC, MHuiFxParameter> i(iParameters);
    for (i.NextValue(); i.CurrentValue(); i.NextValue())
        {
        MHuiFxParameter* p = (MHuiFxParameter*)i.CurrentValue();
        p->AdvanceTime(aElapsedTime);
        }
    }

TBool CHuiFxParameterManager::Update(CHuiFxEngine& aEngine, const TRect& aVisualRect)
    {
    // Calculate all current parameter values
    TPtrHashMapIter<TDesC, MHuiFxParameter> i(iParameters);
    
    TBool parameterChanged = EFalse;
    for (i.NextValue(); i.CurrentValue(); i.NextValue())
        {
        MHuiFxParameter* p = (MHuiFxParameter*)i.CurrentValue();
        if ( p->IsAnimated() )
            {
            parameterChanged = ETrue;
            }
        
        TReal32 newValue = p->ReferenceValue();
        ResolveReference( newValue, p->ReferencePoint(), aEngine, aVisualRect );
        if ( newValue != p->ReferenceValue() )
            {
            p->SetReferenceValue( newValue );
            parameterChanged = ETrue;
            }
            
        if ( p->StartReference() != EReferencePointUndefined )
            {
            newValue = p->StartValue();
            ResolveReference( newValue, p->StartReference(), aEngine, aVisualRect );
            if ( newValue != p->StartValue() )
                {
                p->SetStartValue( newValue );
                parameterChanged = ETrue;
                }
            }
            
        if ( p->EndReference() != EReferencePointUndefined )
            {
            newValue = p->EndValue();
            ResolveReference( newValue, p->EndReference(), aEngine, aVisualRect );
            if ( newValue != p->EndValue() )
                {
                p->SetEndValue( newValue );
                parameterChanged = ETrue;
                }
            }
            
        p->Update();
        }
    return parameterChanged;
    }
    
MHuiFxParameter* CHuiFxParameterManager::Parameter(const TDesC& aName)
    {
    return iParameters.Find(aName);
    }
    
void CHuiFxParameterManager::ResolveReference( TReal32& aDefaultValue, THuiFxReferencePoint aRef, CHuiFxEngine& aEngine, const TRect& aVisualRect )
    {
    switch ( aRef )
        {
        case EReferencePointIdentity:
        case EReferencePointUndefined:
        // If the reference point is an external value, it will not change
        // after it has been set
        case EReferencePointExtRectWidth:
        case EReferencePointExtRectHeight:
        case EReferencePointExtRectTop:
        case EReferencePointExtRectBottom:
        case EReferencePointExtRectLeft:
        case EReferencePointExtRectRight:
            break;
        case EReferencePointVisualWidth:
            aDefaultValue = aVisualRect.Size().iWidth;
            break;
        case EReferencePointVisualHeight:
            aDefaultValue = aVisualRect.Size().iHeight;
            break;
        case EReferencePointVisualTop:
            aDefaultValue = aVisualRect.iTl.iY;
            break;
        case EReferencePointVisualBottom:
            // the bottom right corner of a rect is actually outside the rect
            aDefaultValue = aVisualRect.iBr.iY;
            break;
        case EReferencePointVisualLeft:
            aDefaultValue = aVisualRect.iTl.iX;
            break;
        case EReferencePointVisualRight:
            // the bottom right corner of a rect is actually outside the rect
            aDefaultValue = aVisualRect.iBr.iX;
            break;
        case EReferencePointDisplayHeightMinusVisualTop:
            {
            TReal32 height = aEngine.GetReferenceValue( aRef ); // this will be display.height
            height -= aVisualRect.iTl.iY;
            aDefaultValue = height;
            break;
            }
        default:
            // This will return reference points that depend on display size
            aDefaultValue = aEngine.GetReferenceValue( aRef );
            break;
        }
    }
    
