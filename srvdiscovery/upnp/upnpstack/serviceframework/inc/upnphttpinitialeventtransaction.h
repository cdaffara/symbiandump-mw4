/** @file
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implements algoritms for nonmodereted events
*
*/


#ifndef C_CUPNPHTTPINITIALEVENTTRANSACTION_H
#define C_CUPNPHTTPINITIALEVENTTRANSACTION_H

// INCLUDES

#include "upnphttptransaction.h"

// CLASS DECLARATION

/**
*  CUpnpHttpInitialEventTransaction
* 
*/
NONSHARABLE_CLASS( CUpnpHttpInitialEventTransaction ): public CUpnpHttpTransaction
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CUpnpHttpInitialEventTransaction();

    /**
     * Two-phased constructor.
     */
    static CUpnpHttpInitialEventTransaction* NewL( CUpnpHttpMessage* aRequest );

    /**
     * Two-phased constructor.
     */
    static CUpnpHttpInitialEventTransaction* NewLC( CUpnpHttpMessage* aRequest );
    
    /*
    * Pure virtual method that let child classes to process
    * response in their own specific way.
    */
    void ProcessResponseL();
        
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CUpnpHttpInitialEventTransaction( CUpnpHttpMessage* aRequest );

    };

#endif // C_CUPNPHTTPINITIALEVENTTRANSACTION_H
//end of file
