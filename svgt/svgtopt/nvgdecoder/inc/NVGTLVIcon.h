/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  NVG Decoder header file
 *
*/


#ifndef NVGTLVICON_H_
#define NVGTLVICON_H_

#include <e32base.h>
#include <openvg.h>
#include "NVGIcon.h"

class CNVGIconData;
class MVGImageBinder;
class COpenVGHandleStore;

class CNVGTLVIcon :  public CBase, public MNVGIcon
    {
private:
    CNVGTLVIcon();
    void ConstructL();

public:
    enum TNVGTLVIconCommands
        {
        EPath = 0x50,
        ENone
        };

public:
    static CNVGTLVIcon * NewL();
	
    static CNVGTLVIcon * NewLC();
	
    virtual ~CNVGTLVIcon();
    
    TInt SetPreserveAspectRatio(TInt aPreserveAspectSetting,
            TInt aSmilFitSetting);

    TInt Rotate(TReal32 aAngle, TReal32 aX, TReal32 aY) __SOFTFP;

    virtual TInt Draw(const TSize aSize, CNvgEngine * aNVGEngine);
    
    void AddDrawPathCommandL(VGPath aPath, VGPaintMode aPaintMode);
    
    void AddCommandL(const TUint8 * aCommandBuffer, TInt aCommandBufferLength);
    
    void AddCommandL(TInt8 aCommandType, const TUint8 * aCommandBuffer, TInt aCommandBufferLength);
    
    TInt DirectDrawL(const TDesC8& aBuf, const TSize& aTargetSize);
    
    TInt CreateL(const TDesC8& aBuf, const TSize& aTargetSize);
    
    void SetVGImageBinder(MVGImageBinder *aImageBinder)
        {
        iVGImageBinder = aImageBinder;
        }

private:
    TInt DoDrawL(const TSize aSize);
    void UpdateClientMatrices();
    void RestoreClientMatrices();
    
    VGint               iMatrixMode;
    TReal32             iImageMatrix[9];
    TReal32             iPathMatrix[9];
    CNvgEngine *        iNVGEngine;
    CNVGIconData *      iNVGIconData;    
    MVGImageBinder *    iVGImageBinder;
    COpenVGHandleStore* iOpenVGHandles;
    };

#endif
//--------------------------------EndOfFile------------------------------------
