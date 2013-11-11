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



#ifndef HUIFXPARAMETER_H_
#define HUIFXPARAMETER_H_

#include <e32std.h>
#include <gdi.h>

enum THuiFxParameterType
    {
    EParameterTypeScalar,
    EParameterTypeColor,
    };

enum THuiFxReferencePoint
    {
    EReferencePointIdentity,
    EReferencePointUndefined,
    EReferencePointVisualWidth,
    EReferencePointVisualHeight,
    EReferencePointVisualTop,
    EReferencePointVisualBottom,
    EReferencePointVisualLeft,
    EReferencePointVisualRight,
    EReferencePointDisplayWidth,
    EReferencePointDisplayHeight,
    EReferencePointDisplayTop,
    EReferencePointDisplayBottom,
    EReferencePointDisplayLeft,
    EReferencePointDisplayRight,
    EReferencePointExtRectWidth,
    EReferencePointExtRectHeight,
    EReferencePointExtRectTop,
    EReferencePointExtRectBottom,
    EReferencePointExtRectLeft,
    EReferencePointExtRectRight,
    EReferencePointDisplayHeightMinusVisualTop
    };

class MHuiFxParameter
    {
public:
    virtual ~MHuiFxParameter();
    virtual void AdvanceTime(TReal32 aElapsedTime) = 0;
    virtual THuiFxParameterType Type() const = 0;
    virtual TBool IsAnimated() const = 0;
    virtual void Update() = 0;
    
    virtual void SetReferencePoint(THuiFxReferencePoint aRef) = 0;
    virtual THuiFxReferencePoint ReferencePoint() const = 0;
    virtual void SetReferenceValue(TReal32 aValue) = 0;
    virtual TReal32 ReferenceValue() const = 0;
    
    virtual void SetStartReference( THuiFxReferencePoint aRef ) = 0; 
    virtual THuiFxReferencePoint StartReference() const = 0;
    virtual void SetStartValue( TReal32 aValue ) = 0;
    virtual TReal32 StartValue() const = 0;
    virtual void SetStartMultiplier( TReal32 aValue ) = 0;
    
    virtual void SetEndReference( THuiFxReferencePoint aRef ) = 0; 
    virtual THuiFxReferencePoint EndReference() const = 0;
    virtual void SetEndValue( TReal32 aValue ) = 0;
    virtual TReal32 EndValue() const = 0;
    virtual void SetEndMultiplier( TReal32 aValue ) = 0;

    virtual MHuiFxParameter *CloneL() const=0;
    virtual void ReRef(TReal32 *aValue)=0;
    virtual void ReRef(TRgb *aValue)=0;
    };

template <typename TYPE, typename TIMELINE_TYPE, THuiFxParameterType TYPE_CODE>
// This class is not derived from CBase because it is a template class + a mixin class.
// If the class is derived from CBase, the static_cast to actual class points to a wrong place
// All members are zeroed or set to default values in the constructor, so deriving from
// CBase does not really provide any additional functionality.
class CHuiFxGenericParameter: /*public CBase,*/ public MHuiFxParameter
    {
public:
    typedef TYPE ValueType;
    
    CHuiFxGenericParameter(TYPE* aValue);
    ~CHuiFxGenericParameter();
    void SetTimeLine(TIMELINE_TYPE* aTimeLine); // takes ownership
    void AdvanceTime(TReal32 aElapsedTime);
    TBool IsAnimated() const;
    THuiFxParameterType Type() const;
    void SetValue(TYPE aValue);
    void Update();

    void SetReferencePoint(THuiFxReferencePoint aRef);
    THuiFxReferencePoint ReferencePoint() const;
    void SetReferenceValue(TReal32 aValue);
    TReal32 ReferenceValue() const;
    
    void SetStartReference( THuiFxReferencePoint aRef ); 
    THuiFxReferencePoint StartReference() const;
    void SetStartValue( TReal32 aValue );
    TReal32 StartValue() const;
    void SetStartMultiplier( TReal32 aValue );
    void SetEndReference( THuiFxReferencePoint aRef ); 
    THuiFxReferencePoint EndReference() const;
    void SetEndValue( TReal32 aValue );
    TReal32 EndValue() const;
    void SetEndMultiplier( TReal32 aValue );
    CHuiFxGenericParameter<TYPE, TIMELINE_TYPE, TYPE_CODE> *CloneL() const;
    void ReRef(TReal32 *aValue);
    void ReRef(TRgb *aValue);

private:
    // Not implemented, not supported
    CHuiFxGenericParameter(const CHuiFxGenericParameter&);
    CHuiFxGenericParameter& operator=(const CHuiFxGenericParameter&);

private:
    TIMELINE_TYPE*              iTimeLine;
    TYPE                        iRelValue;
    TYPE*                       iValue;
    TReal32                     iTime;
    TBool                       iAnimationFinished;
    TBool                       iLastFrameDrawn;
    THuiFxReferencePoint        iRefPoint;
    TReal32                     iRefValue;
    THuiFxReferencePoint        iStartRef;
    TReal32                     iStartValue;
    TReal32                     iStartMultiplier;
    THuiFxReferencePoint        iEndRef;
    TReal32                     iEndValue;
    TReal32                     iEndMultiplier;
    };

#include "HuiFxParameter.inl"

#endif /*HUIFXPARAMETER_H_*/
