/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef ALFWINDOWDATA_H_
#define ALFWINDOWDATA_H_

// ---------------------------------------------------------------------------
// TAlfWindowData declaration
// ---------------------------------------------------------------------------
//
class TAlfWindowData
    {
    
    public:
        enum TFlags
        {
        EShouldInactivate = 0x01,
        EShouldDestroy = 0x20
        };


    CAlfWindowManager* iWindowManager;
    TPoint iPosition;
    TSize iSize;
    TReal32 iOpacity;
    TReal32 iRotation;
    TRect iSurfaceExtent;
    TBool iLayerUsesAlphaFlagEnabled;
    TUint iIdentifier;
    TUint iIndex;
    CAlfWindowBuffer* iInBuffer;
    CAlfWindowBuffer* iOutBuffer;
    TAlfWServInfo iOriginalInfo;
    TBool iHasPlugin; // plugin that has reserved the window for painting
    CFbsBitmapDevice* iBufferDev;
    CFbsBitGc* iBufferGc;
    TBool iIsActive;
    TBool iIsInScene;
    // was not set inactive because was reserved,
    // should be set inactive as soon as is reserved.
    TInt iFlags;

    RRegion iWindowArea;
    TPoint iWindowAreaOrigin;
    
    TBool iTransparencyAlphaChannel;
    TBool iIncludeToVisibilityCalculation;
    };


#endif /* ALFWINDOWDATA_H_ */
