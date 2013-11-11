/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*
*/

#ifndef CUPNPCPBINITIALEVENTRETRYHANDLER_H
#define CUPNPCPBINITIALEVENTRETRYHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
class CUpnpGenaMessage;
class CUpnpControlPoint;
class CUpnpCpbInitialEventRetry;

/**
 *  CUpnpCpbInitialEventRetryHandler
 * 
 */
NONSHARABLE_CLASS( CUpnpCpbInitialEventRetryHandler ) : public CBase
    {
public:        
    
    // Constructors and destructor
    CUpnpCpbInitialEventRetryHandler( CUpnpControlPoint& aCp );
    
    ~CUpnpCpbInitialEventRetryHandler();
   
    /**
     * Adds Gena message to handler, ownership of the argument is  passed through     
     * @param aMsg Gena message
     */ 
    void AddL( CUpnpGenaMessage* aMsg ); 

    /**
     * Method to be invoked by CUpnpCpbInitialEventRetry      
     * @param aTimer retry timer
     */ 
    void TimerExpired( CUpnpCpbInitialEventRetry* aEventRetry );

private:
     
    RPointerArray<CUpnpCpbInitialEventRetry> iList;
    CUpnpControlPoint& iControlPoint;
    };

#endif // CUPNPCPBINITIALEVENTRETRYHANDLER_H
