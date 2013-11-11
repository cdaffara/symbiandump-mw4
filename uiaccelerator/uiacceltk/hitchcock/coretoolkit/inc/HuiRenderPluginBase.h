/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Abstract base class for renderer plugins
*
*/



#ifndef HUIRENDERERPLUGINBASE_H
#define HUIRENDERERPLUGINBASE_H

#include <e32base.h>

const TUid KUidHuiRenderPluginBase = { 0x200113D4 };

/**
 *  Abstract base class for renderer plugins
 *  Renderer plugins are implemented as ECom plug-ins derived from this interface.
 *  Each plugin specifies in its ECom IMPLEMENTATION_INFO default_data field the
 *  API type it handles. For Excample OpenGLES11
 *
 *  @lib //?library
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CHuiRenderPluginBase ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     * @param aMatchData The type of the renderer plugin for example OpenGLES11
     */
    inline static CHuiRenderPluginBase* NewL( const TDesC& aMatchData );
    
    /**
    * Destructor.
    */
    inline virtual ~CHuiRenderPluginBase();

protected:

    CHuiRenderPluginBase();

private: // data

    /**
     * A unique UID used in interface destruction
     */
	TUid	iDtor_ID_Key;

    };

#include "HuiRenderPluginBase.inl"

#endif // HUIRENDERERPLUGINBASE_H
