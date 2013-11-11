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
* Description:   Sub-session for control group
*
*/



#ifndef R_ALFCONTROLGROUPSUBSESSION_H
#define R_ALFCONTROLGROUPSUBSESSION_H

#include <e32std.h>
#include "alfclient.h"

/**
 *  Sub-session for control group
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( RAlfControlGroupSubSession ): public RSubSessionBase 
    {

public:

    /**
     * Open subsession for given session.
     *
     * @param aSession Used session.
     * @param aId Control group ID.
     * @return Error code.
     */
    TInt Open( RAlfClient& aSession, TInt aId );
    
    /**
     *  Closes the subsession.
     */
    void Close();
    
    /**
     * Appends a control
     *
     * @param aControlHandle Handle to the server side object. 
     * @return Error code.
     */
    TInt Append( TInt aControlHandle );
    
    /**
     * Removes a control
     *
     * @param aControlHandle Handle to the server side object. 
     * @return Error code.
     */
    TInt Remove( TInt aControlHandle );
    
    /**
     * Enables/disabels transformation
     *
     * @param aTransformationHandle Handle to the server side object.                       
     * @param aIsTransformed ETrue if enabled. 
     * @return Error code.
     */
    TInt EnableTransformation( 
        TInt aTransformationHandle,
        TBool aIsTransformed );
    };


#endif // R_ALFCONTROLGROUPSUBSESSION_H
