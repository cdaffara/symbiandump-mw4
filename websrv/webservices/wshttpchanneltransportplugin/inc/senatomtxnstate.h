/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/








#ifndef WSF_ATOM_TXN_STATE_H
#define WSF_ATOM_TXN_STATE_H

//  INCLUDES
#include "sentxnstate.h"
#include "senmultipartutils.h"
#include "senmultiparttxnstate.h"

// CONSTANTS


	

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
 * Class for working with MultiPart messages - creates MultiPart request and parse MultiPart response
 * Implements the MHTTPDataSupplier interface.
 *
 */
class CSenAtomTxnState : public CSenMultiPartTxnState
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     */
    static CSenAtomTxnState* NewLC(MSenResponseObserver& aObserver,
                             	   const TDesC8* aUri,
                               	   CSenAtomEntry& aAtomEntry);

    /**
     * Two-phased constructor.
     *
     */
    static CSenAtomTxnState* NewL(MSenResponseObserver& aObserver,
                              	  const TDesC8* aUri,
                              	  CSenAtomEntry& aAtomEntry);


    // Functions from base classes
    
    /**
     * From CSenTxnState
     * Creates and sets ContentType in MultiPart envelope header
     * @param aSession
     * @param aHeaders
     */
    void SetContentTypeHeaderL(const RHTTPSession aSession, RHTTPHeaders aHeaders);
    void MessageL();
    
private:

    /**
     * C++ default constructor.
     *
     */
    CSenAtomTxnState(MSenResponseObserver& aObserver);

    /**
     * 2nd phase constructor.
     *
     */
    void ConstructL(const TDesC8* aUri,
                    CSenAtomEntry& aAtomEntry);
    };

#endif // WSF_ATOM_TXN_STATE_H

// End of File
