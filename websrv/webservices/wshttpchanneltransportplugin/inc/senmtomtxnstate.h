/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header declaration
*
*/











#ifndef WSF_MTOM_TXN_STATE_H
#define WSF_MTOM_TXN_STATE_H

//  INCLUDES
#include "sentxnstate.h"
#include "senmultipartutils.h"
#include "senmultiparttxnstate.h"
// CONSTANTS

_LIT8(KDownloadFolder, "SenBlobResponses"); // folder where should be created Blobs from Response
	

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
 * Class for working with MTOM messages - creates MTOM request and parse MTOM response
 * Implements the MHTTPDataSupplier interface.
 *
 */
class CSenMtomTxnState : public CSenMultiPartTxnState
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     */
    static CSenMtomTxnState* NewLC(MSenResponseObserver& aObserver,
                             	   const TDesC8* aUri,
                               	   const TDesC8& aSoapAction,
                               	   CSenSoapEnvelope2& aSoapEnvelope2);

    /**
     * Two-phased constructor.
     *
     */
    static CSenMtomTxnState* NewL(MSenResponseObserver& aObserver,
                              	  const TDesC8* aUri,
                              	  const TDesC8& aSoapAction,
                              	  CSenSoapEnvelope2& aSoapEnvelope2);



    // Functions from base classes



    
    /**
     * From CSenTxnState
     * Creates and sets ContentType in MTOM envelope header
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
    CSenMtomTxnState(MSenResponseObserver& aObserver);

    /**
     * 2nd phase constructor.
     *
     */
    void ConstructL(const TDesC8* aUri,
                    const TDesC8& aSoapAction,
                    CSenSoapEnvelope2& aSoapEnvelope2);
    };

#endif // WSF_MTOM_TXN_STATE_H

// End of File
