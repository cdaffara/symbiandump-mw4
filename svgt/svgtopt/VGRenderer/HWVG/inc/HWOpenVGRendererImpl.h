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
* Description:  CHWOpenVGRendererImpl header file
 *
*/

#ifndef HWOPENVGRENDERERIMPL_H_
#define HWOPENVGRENDERERIMPL_H_

#include "HWVG.h"
#include "OpenVGRenderer.h"

class CHWOpenVGRendererImpl : public COpenVGRenderer
    {
    static const TUint KMAJOR_VERSION;
    static const TUint KMINOR_VERSION;
    static const TUint KBUILD_VERSION;
public:
    static CHWOpenVGRendererImpl* NewL();
    static CHWOpenVGRendererImpl* NewLC();
    ~CHWOpenVGRendererImpl();
    
public:
    TVersion Version() const;
    
    virtual TVersion GetVersion() const;
    virtual const TDesC GetName() const;

public:
    
    virtual void ToggleReset();
    
    virtual MVGSurfaceImpl* CreateVGSurfaceL(TInt aOption);
    
private:
    CHWOpenVGRendererImpl();
    void ConstructL();
    };

#endif /*HWOPENVGRENDERERIMPL_H_*/
