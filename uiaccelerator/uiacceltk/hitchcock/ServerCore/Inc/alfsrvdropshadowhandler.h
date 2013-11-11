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
* Description:   Drop shadow handler
*
*/



#ifndef ALFSRVDROPSHADOWHANDLER_H
#define ALFSRVDROPSHADOWHANDLER_H

#include <e32base.h>

class CHuiDropShadow;

/**
 *  Drop shadow handler
 *
 *  @since S60 v5.0.1
 */
NONSHARABLE_CLASS( AlfSrvDropShadowHandler )
    {
public:

    /**
     * Checks if the command can be handled by this class
     *
     * @param aCommand Command to check.
     *
     * @return ETrue if this class can handle the command.
     */
    static TBool CanHandleCommand( TInt aCommand );
    
    /**
     * Handles the command
     *
     * @param aDropShadow Drop shadow handler which is controlled by the command.
     * @param aCommandId Command to execute.
     * @param aInputBuffer Input parameter buffer for the command.
     * @param aResponse Response buffer.
     */
    static void HandleCommandL( 
        CHuiDropShadow& aDropShadow,
        TInt aCommandId, 
        const TDesC8& aInputBuffer, 
        TDes8& aResponse );

    };

#endif // ALFSRVDROPSHADOWHANDLER_H
