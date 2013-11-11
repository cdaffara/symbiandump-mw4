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
* Description:  NVG Decoder source file
 *
*/

#include "OpenVGHandleStore.h"

COpenVGHandleStore::COpenVGHandleStore()
    {
    }

COpenVGHandleStore::~COpenVGHandleStore()
    {
    TInt handleListCount    = iHandles.Count();
    for (TInt i = 0; i < handleListCount; i++)
        {
        if (iHandles[i].iVGHandle)
            {
            switch (iHandles[i].iHandleType)
                {
                case TLVVGHandlePair::EVGPath:
                    vgDestroyPath(iHandles[i].iVGHandle);
                    break;
                case TLVVGHandlePair::EVGPaint:
                    vgDestroyPaint(iHandles[i].iVGHandle);
                    break;
                case TLVVGHandlePair::EVGImage:
                    vgDestroyImage(iHandles[i].iVGHandle);
                    break;
                }
            }
        }
    iHandles.Close();
    }

COpenVGHandleStore * COpenVGHandleStore::NewL()
    {
    COpenVGHandleStore* self    = COpenVGHandleStore::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

COpenVGHandleStore * COpenVGHandleStore::NewLC()
    {
    COpenVGHandleStore* self    = new (ELeave) COpenVGHandleStore;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void COpenVGHandleStore::ConstructL()
    {
    
    }
