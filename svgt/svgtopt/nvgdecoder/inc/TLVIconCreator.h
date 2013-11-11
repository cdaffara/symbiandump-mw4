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


#ifndef TLVICONCREATER_H_
#define TLVICONCREATER_H_

#include <e32base.h>
#include <VG/openvg.h>
#include <VG/vgu.h>
#include "FloatFixPt.h"
#include "PseudoVG.h"

class CNVGTLVIcon;
class CNVGIconData;
class CTLVIconCreator : public CBase
    {
    
public:
    static CTLVIconCreator * NewL(const TDesC8& aPtr, TInt aWidth, TInt aHeight, CNVGTLVIcon * aNVGTLVIcon);
    
    static CTLVIconCreator * NewLC(const TDesC8& aPtr, TInt aWidth, TInt aHeight, CNVGTLVIcon * aNVGTLVIcon);
    
    virtual ~CTLVIconCreator();

    TInt ExecuteL();

    void SetTLVIcon(CNVGTLVIcon * aNVGTLVIcon)
        {
        iNVGTLVIcon = aNVGTLVIcon;
        }

private:
    CTLVIconCreator(TInt aWidth, TInt aHeight, CNVGTLVIcon * aNVGTLVIcon);
    void ConstructL(const TDesC8& aPtr);

    void DvgAddCommandL(TInt aLength, TInt aPos);
    void DvgAddCommandL();
    void DvgSetParameterfvL( );    
    void DvgSetPaintL( );    
    void DvgAppendPathDataL( );    
    void DvgDrawPathL( );
    void DvgClearPathL();
    void DvgCreatePaintL( );   
    void DvgSetivL( );                
    void DvgClearL();
    void DvgSetfvL();
    void DvgCreatePathL();
    void DvgCreateImageL();
    void DvgGetPixelsL();
    void DvgClearImageL();
    void DvgImageSubDataL();
    void DvgDrawImageL();
    void DvgDestroyImageL();
    void DvgDestroyPaintL();
    void DvgDestroyPathL();

    void DvguRectL();
    void DvguEllipseL();
    void DvguRoundRectL();
    void DvguLineL();

    void DvgPrepareToBindImageL();
    void DvgBindImageL();
    void DvgUnBindImageL();
    
    void DvgFlushL(); 

    TInt ExecuteL(TInt index);

#ifdef NVG_DEBUG
    //Command specific logging methods
    void LogvgSeti(VGParamType type, VGint fvalue, TInt cmdsize);
    void LogvgSetf (VGParamType type, VGfloat fvalue, TInt cmdsize);
    void LogvgSetParameteri(VGHandle handle, VGint paramType, VGint pvalue, TInt cmdsize, TInt Lpvalue);		
    void LogvgSetPaint(VGPaint paint, VGbitfield paintModes, TInt cmdsize, TInt Lpvalue);                        
    void LogvgDrawPath(VGbitfield paintModes, int cmdsize);
    void LogvgSetParameterfv(VGPaint handle, VGint paramtype, TInt count, TInt handlenum);			                        
#endif

private:

    typedef void (CTLVIconCreator::*pvgapi)( );
    
    VGPath  CreatePathL();

    pvgapi vgapi[EvgFlush + 1];
    
    CNVGIconData *  iNVGIconData;
    TInt            iDataLength;
    TSize           iResizedCanvasSize;
        
    
    TUint8          iNvgHeader[KNVGHeaderSize];
    TInt            iTargetWidth;
    TInt            iTargetHeight;
    TBool           iScaled;
    
    TReal32         iUserMatrix[9];
    TUint           iPrepareToBindImage;
    
    TPtrC8          iNVGDataPtr;

    CNVGTLVIcon *   iNVGTLVIcon;
    
    TInt8           iCurrentCommand;
    TUint8 *        iCommandBuffer;
    VGPath          iLastVGPath;
    
    friend class CNVGTLVIcon;
    };

#endif
//--------------------------------EndOfFile------------------------------------
