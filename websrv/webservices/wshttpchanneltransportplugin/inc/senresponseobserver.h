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
* Description:           An interface class for receiving responses to the HTTP
*                requests submitted.
*
*/











#ifndef SEN_RESPONSE_OBSERVER_H
#define SEN_RESPONSE_OBSERVER_H

#include "SenHttpTransportProperties.h"
#include "SenServiceConnection.h"
_LIT8(KDefaultContentType,              "text/xml; charset=UTF-8");


// CLASS DECLARATION

/**
 * An interface class for receiving responses to the HTTP
 * requests submitted.
 *
 */

class MSenResponseObserver
    {
    public:
    
        /**
         * Receive response to a request submitted. This method
         * takes the ownership of the heap buffer aContent.
         *
         * @param aId submit id which identifies the request.
         * @param aContentType incoming response content type.
         * @param aContent incoming response content.
         * @param aHttpProperties - properies of response(e.g.such as file names where BLOBs are saved) or NULL
         * @leave if aContent is NULL.
         */
        virtual void ResponseReceivedL(TInt aId,
                                       const TAny* aContentType,
                                       HBufC8* aContent,
                                       CSenHttpTransportProperties* aHttpProperties = NULL) = 0;
    
        /**
         * Error occurred while receiving response. Error code
         * is either system error code or own error code. 
         *
         * @param aId submit id which identifies the request.
         * @param aError error code.
         */
        virtual void ResponseErrorL(TInt aId,
                                   TInt aError,
                                   HBufC8* aErrorBody,
                                   CSenHttpTransportProperties* aHttpProperties = NULL) = 0;
	    /**
	     * State has changed 
	     * 
	     * @param aId submit id which identifies the request.
	     * @param aState New State of the transaction.
	     */
		virtual void StateChanged(TInt aId, TInt aState) = 0;
        virtual void FileProgress(TInt aTxnId, TBool aIncoming, TBool aIsSoap,
                    const TDesC8& aSoapOrCid, TInt aProgress) = 0;
		virtual void SetTrafficDetails(TSenDataTrafficDetails& aDetails) = 0; 

        virtual MSenProperties& PropertiesL() = 0;

    };



#endif // SEN_RESPONSE_OBSERVER_H

// End of File
