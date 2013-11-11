/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Declares a interface for session-attached objects
*
*/



#ifndef __HUISESSIONOBJECT_H__
#define __HUISESSIONOBJECT_H__

#include <e32base.h>

/**
 * Interface for session-aware objects, providing a setter and getter for session id.
 * Object type information is included too.
 */
class MHuiSessionObject
    {
public:
    enum TType
        {
        ETypeVisual,
        ETypeControl,
        ETypeControlGroup,
        ETypeBrush,
        ETypeTexture,
        ETypeRenderSurface,
        ETypeMesh,
        };

    /**
     * Gets the object type.
     */
    virtual TType Type() const = 0;

    /**
     * Gets the session id for object.
     */
    virtual TInt SessionId() const = 0;

    /**
     * Sets the session id for object.
     */
    virtual void SetSessionId(TInt aSessionId) = 0;
    };

#endif // __HUISESSIONOBJECT_H__

