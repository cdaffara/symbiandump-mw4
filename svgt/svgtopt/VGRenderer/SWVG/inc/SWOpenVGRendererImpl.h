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
* Description:  CSWOpenVGRendererImpl header file
 *
*/

#ifndef __SWVGRENDERER_H__
#define __SWVGRENDERER_H__

#include "SWVG.h"
#include "OpenVGRenderer.h"


class CSWOpenVGRendererImpl : public COpenVGRenderer
    {
    static const TUint KMAJOR_VERSION;
    static const TUint KMINOR_VERSION;
    static const TUint KBUILD_VERSION;
public:
    static CSWOpenVGRendererImpl* NewL();
    static CSWOpenVGRendererImpl* NewLC();
    ~CSWOpenVGRendererImpl();

public:
    TVersion Version() const;
    virtual TVersion GetVersion() const;
    virtual const TDesC GetName() const;
    
public:

    virtual void ToggleReset();
    
    virtual MVGSurfaceImpl* CreateVGSurfaceL(TInt aOption);

private:
    CSWOpenVGRendererImpl();
    void ConstructL();
   };

#endif

