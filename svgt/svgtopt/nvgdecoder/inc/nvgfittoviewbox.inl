/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// ---------------------------------------------------------------------------
// Setter function for viewBox
// ---------------------------------------------------------------------------
inline void CNvgFitToViewBoxImpl::SetViewBox(TReal avbX, TReal avbY, TReal avbW, TReal avbH)
    {
    ivbX = avbX;
    ivbY = avbY;
    ivbW = avbW;
    ivbH = avbH;
    iViewBoxDefined = ETrue;
    }



// ---------------------------------------------------------------------------
// Setter function for Alignment
// ---------------------------------------------------------------------------
inline void CNvgFitToViewBoxImpl::SetAlign(TNvgAlignStatusType aAlignStatus)    
    {
    iAlign = aAlignStatus;
    }



// ---------------------------------------------------------------------------
// Setter function for Scaling of viewport to viewbox
// ---------------------------------------------------------------------------
inline void CNvgFitToViewBoxImpl::SetScaling(TNvgMeetOrSliceType aMeetSlice)    
    {
    iMeetSlice = aMeetSlice;
    }


// ---------------------------------------------------------------------------
// Setter function for setting current transformation matrix
// ---------------------------------------------------------------------------
inline void CNvgFitToViewBoxImpl::SetTransform(TReal aM00, TReal aM01, TReal aM02, TReal aM10, TReal aM11, TReal aM12)
    {
    iM00 = aM00;    
    iM01 = aM01;
    iM02 = aM02;
    iM10 = aM10;
    iM11 = aM11;
    iM12 = aM12;
    }



// ---------------------------------------------------------------------------
// Setter function for translation factors
// ---------------------------------------------------------------------------
inline void CNvgFitToViewBoxImpl::Translate(TReal aTx, TReal aTy)
    {
    TReal lTranslateMatrix[6] =  { 1, 0, aTx, 0, 1, aTy};
    Concatenate(lTranslateMatrix);
    }



// ---------------------------------------------------------------------------
// Setter function for Scaling factors
// ---------------------------------------------------------------------------
inline void CNvgFitToViewBoxImpl::Scale(TReal aSx, TReal aSy)
    {
    TReal lScaleMatrix[6] = { aSx, 0, 0, 0, aSy, 0};
    Concatenate(lScaleMatrix);
    }


inline void CNvgFitToViewBoxImpl::Concatenate(TReal aMatrix[6])
    {
    Concatenate(aMatrix[0], aMatrix[1], aMatrix[2], aMatrix[3], aMatrix[4], aMatrix[5]);
    }
//===========================End Of File=================================================

