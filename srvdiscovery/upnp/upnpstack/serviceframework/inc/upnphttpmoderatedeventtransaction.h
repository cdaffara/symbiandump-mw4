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
* Description:  Implements transaction for moderated events
*
*/

#ifndef C_CUPNPHTTPMODERATEDEVENTTRANSACTION_H
#define C_CUPNPHTTPMODERATEDEVENTTRANSACTION_H


// INCLUDES
#include "upnphttptransaction.h"
#include "upnpnonmoderatedeventqueuemanager.h"

// CLASS DECLARATION

/**
 *  CUpnpHttpModeratedEventTransaction
 * 
 */
NONSHARABLE_CLASS( CUpnpHttpModeratedEventTransaction ): public CUpnpHttpTransaction
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CUpnpHttpModeratedEventTransaction();

    /**
     * Two-phased constructor.
     */
    static CUpnpHttpModeratedEventTransaction* NewL( CUpnpHttpMessage* aRequest, 
                                                     MTransactionObserver& aObserver );

    /**
     * Two-phased constructor.
     */
    static CUpnpHttpModeratedEventTransaction* NewLC( CUpnpHttpMessage* aRequest, 
                                                      MTransactionObserver& aObserver);
    
    /*
     * Pure virtual method that let child classes to process
     * response in their own specific way.
     */
    void ProcessResponseL();
    
            
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CUpnpHttpModeratedEventTransaction( CUpnpHttpMessage* aRequest, 
                                        MTransactionObserver& aObserver );


private:
    
    MTransactionObserver& iObserver;//not owned
    };

#endif // C_CUPNPHTTPMODERATEDEVENTTRANSACTION_H
//end of file
