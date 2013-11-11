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


#ifndef _TLVICONRENDERER_H_
#define _TLVICONRENDERER_H_

#include "TLVRenderer.h"

class CTLVIconRenderer : public CTLVRenderer
    {
public:
    static CTLVIconRenderer * NewL(const TDesC8& aBuf, TInt aWidth, TInt aHeight);
 
    static CTLVIconRenderer * NewLC(const TDesC8& aBuf, TInt aWidth, TInt aHeight);
    
    virtual ~CTLVIconRenderer();

protected:
    TInt DrawPathL();
    virtual TInt ExecuteL(TInt index);

private:
    CTLVIconRenderer(TInt aWidth, TInt aHeight);
    };

#endif
//--------------------------------EndOfFile------------------------------------
