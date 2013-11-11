/*
* ============================================================================
*  Name        : alfappfwproxyops.h
*  Part of     : uiacceltk / uiaccelerator       *** Info from the SWAD
*  Description : IPC constants for AlfAppFwProxy
*  Version     : %version: 5.1.1 % << Don't touch! Updated by Synergy at check-out.
*
*  Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies).
*  All rights reserved.
*  This component and the accompanying materials are made available
*  under the terms of "Eclipse Public License v1.0"
*  which accompanies this distribution, and is available
*  at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
*  Initial Contributors:
*  Nokia Corporation - initial contribution.
*
*  Contributors:
*  Nokia Corporation
* ============================================================================
* Template version: 4.2
*/

#ifndef ALFAPPFWPROXYOPS_H
#define ALFAPPFWPROXYOPS_H

#include <AknsItemID.h>
const TInt KAlfAppFwProxyUid = 0x20022ef7;

enum AlfProxyCommands
        {
        EAlfIsMirrorred = 101,
        EAlfCbaLocation,
        EAlfLayoutMetricsRect,
        EAlfGetCachedColor, 
        EAlfConvertVisualAndClip,
        EAlfGetSkinBackgroundBitmap,
        EAlfGetSkinBitmap,
        EGetCachedSkinItemData,
        EGetListOfWindowGroups
        };

struct TAlfCommandParams{
        TInt iCommand;
        TInt iInt1;
        TInt iInt2;
        TInt iInt3;        
        };

// oh, so elegant
struct TAlfCommandParams2{
        TInt iCommand;
        TInt iInt1;
        TInt iInt2;
        TInt iInt3;        
        TInt iInt4;        
        TFileName iFileName;
        TSize iSize;
        TScaleMode iScaleMode;        
        };


struct TAlfCachedSkinItemArray
    {
    TInt iCount;
    TAknsItemID iImages[9];
    };
        
struct TInt2
    {
    TInt iInt1;
    TInt iInt2;
    };

    
#endif // ALFAPPFWPROXYOPS_H
