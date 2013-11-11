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


#ifndef OPENVGHANDLESTORE_H_
#define OPENVGHANDLESTORE_H_

#include <e32base.h>
#include <openvg.h>

class COpenVGHandleStore : public CBase
    {
public:
    struct TLVVGHandlePair
        {
        enum THandleType
            {
            EVGPath,
            EVGPaint,
            EVGImage
            };
        VGHandle    iVGHandle;
        THandleType  iHandleType;
        
        explicit TLVVGHandlePair(VGHandle aVGHandle,  THandleType aHandleType = EVGPath)
            : iVGHandle(aVGHandle),
              iHandleType(aHandleType)
                {
                }
        };

    virtual ~COpenVGHandleStore();
    
    static COpenVGHandleStore * NewL();

    static COpenVGHandleStore * NewLC();

    void AddPathDHL(VGPath aHandle)
        {
        TInt error = AddPath(aHandle);
        if (error != KErrNone)
            {
            vgDestroyPath(aHandle);
            User::Leave(error);
            }
        }

    void AddPaintDHL(VGPaint aHandle)
        {
        TInt error = AddPaint(aHandle);
        if (error != KErrNone)
            {
            vgDestroyPaint(aHandle);
            User::Leave(error);
            }
        }

    void AddImageDHL(VGImage aHandle)
        {
        TInt error = AddImage(aHandle);
        if (error != KErrNone)
            {
            vgDestroyImage(aHandle);
            User::Leave(error);
            }
        }
    
    TInt AddPath(VGPath aHandle)
        {
        return iHandles.Append(TLVVGHandlePair(aHandle, TLVVGHandlePair::EVGPath));
        }

    TInt AddPaint(VGPaint aHandle)
        {
        return iHandles.Append(TLVVGHandlePair(aHandle, TLVVGHandlePair::EVGPaint));
        }

    TInt AddImage(VGImage aHandle)
        {
        return iHandles.Append(TLVVGHandlePair(aHandle, TLVVGHandlePair::EVGImage));
        }

    TInt AddHandle(VGHandle aHandle, TLVVGHandlePair::THandleType aType)
        {
        return iHandles.Append(TLVVGHandlePair(aHandle, aType));
        }
    
private:
    COpenVGHandleStore();

    void ConstructL();

    RArray<TLVVGHandlePair> iHandles;
    };

#endif
//--------------------------------EndOfFile------------------------------------
