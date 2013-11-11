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



#ifndef HUIFXTIMELINE_H_
#define HUIFXTIMELINE_H_

enum THuiFxLoopingMode
    {
    ELoopingModeNone,
    ELoopingModeRepeat,
    };

enum THuiFxInterpolationMode
    {
    EInterpolationModeHold,
    EInterpolationModeLinear,
    EInterpolationModeQuadraticBezier,
    EInterpolationModeCubicBezier,
    EInterpolationModeInQuad,
    EInterpolationModeOutQuad,
    EInterpolationModeInOutQuad,
    EInterpolationModeOutInQuad,
    EInterpolationModeInBack,
    EInterpolationModeOutBack,
    EInterpolationModeInOutBack,
    EInterpolationModeOutInBack,
/*    
    EInterpolationModeDecelerate,
    EInterpolationModeAccelerate,
    EInterpolationModeImpulse,
    EInterpolationModeSineWave
*/    
    };

template <typename TYPE, typename FPTYPE>
class RHuiFxTimeLine
    {
public:
    RHuiFxTimeLine();
    ~RHuiFxTimeLine();

    // TODO: tangent control, ease in/out
    void AppendKeyFrameL(TReal32 aTime, TYPE aValue);
    void AppendKeyFrameL(TReal32 aTime, TYPE aValue, TYPE aAuxValue1);
    void AppendKeyFrameL(TReal32 aTime, TYPE aValue, TYPE aAuxValue1, TYPE aAuxValue2);
     void SetLoopStart(TReal32 aTime);
    TReal32 LoopStart() const;
    void SetLoopEnd(TReal32 aTime);
    TReal32 LoopEnd() const;
    TReal32 Duration() const;
    void SetDuration(TReal32 aDuration);
    void SetInterpolationMode(THuiFxInterpolationMode aMode);
    THuiFxInterpolationMode InterpolationMode() const;
    void SetLoopingMode(THuiFxLoopingMode aMode);
    THuiFxLoopingMode LoopingMode() const;
    TYPE ValueAt(TReal32 aTime);
    
    RHuiFxTimeLine* CloneL();
protected:
    FPTYPE ToFixedPoint(const TYPE& aValue);
    TYPE FromFixedPoint(const FPTYPE& aValue);
    FPTYPE Blend(const FPTYPE& aValue1, const FPTYPE& aValue2, TInt32 aAlpha);
    FPTYPE WeightedSum4(const FPTYPE& aValue1, const FPTYPE& aValue2, const FPTYPE& aValue3, const FPTYPE& aValue4,
                        TInt32 aWeight1, TInt32 aWeight2, TInt32 aWeight3, TInt32 aWeight4);
    TReal32 LocalTime(TReal32 aTime) const;

private:
    // Not implemented
    RHuiFxTimeLine(const RHuiFxTimeLine& aTimeLine);
    RHuiFxTimeLine& operator=(const RHuiFxTimeLine& aTimeLine);
    
private:
    class TKeyFrame
        {
    public:
        /** Time */
        TReal32             iTime;
        
        /** Fixed point time */
        TInt32              iFpTime;
        
        /** Value */
        TYPE                iValue;

        /** Fixed point value */
        FPTYPE              iFpValue;
        
        /** Auxiliary values for bezier interpolation */
        TYPE                iAuxValue[2];
        FPTYPE              iFpAuxValue[2];
        };
    
    typedef RArray<TKeyFrame> TKeyFrames;

    TReal32                 iTime;
    TReal32                 iDuration;
    TKeyFrames              iKeyFrames;
    TInt                    iLastIndex;
    TReal32                 iLoopStart;
    TReal32                 iLoopEnd;
    THuiFxLoopingMode       iLoopingMode;
    THuiFxInterpolationMode iInterpolationMode;
    };

#include "HuiFxTimeLine.inl"

#endif /*HUIFXTIMELINE_H_*/
