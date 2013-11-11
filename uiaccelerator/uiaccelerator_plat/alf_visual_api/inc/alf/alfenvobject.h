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
* Description:   An type for holding virtually releasable objects
*
*/



#ifndef ALFENVOBJECT_H
#define ALFENVOBJECT_H

/**
 * Base class for objects that required virtual release of heap objects
 *  
 *  @lib alfclient.lib
 *  @since S60 v5.0.1
 */
class MAlfEnvObject
    {
public:
    /**
     * Method to release the owned object. 
     * This method should be implemented in the derived classes to delete
     * the heap object.
     */
    virtual void Release() = 0;
    };

#endif // ALFENVOBJECT_H
