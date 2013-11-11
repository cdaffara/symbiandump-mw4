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
* Description: Header declaration
*
*/








#ifndef SEN_HOSTLET_RESPONSE_H
#define SEN_HOSTLET_RESPONSE_H

#include <e32base.h>

#include "MSenHostletResponse.h"
#include "msenmessagecontext.h"

// FORWARD DECLARES
class CSenMessageContext;

class CSenHostletResponse :public CBase, public MSenHostletResponse
    {
    
    public:
        IMPORT_C static CSenHostletResponse* NewL(const TInt aRequestId);
        IMPORT_C static CSenHostletResponse* NewLC(const TInt aRequestId);
        IMPORT_C virtual ~CSenHostletResponse();
        
    public:
        /** 
        * From MSenHostletResponse
        */
        IMPORT_C virtual void SetResponseUtf8L(const TDesC8& aResponseBodyUtf8,
                                               const TInt aResponseCode = KErrNone,
                                               MSenProperties* apResponseProperties = NULL,
                                               MSenMessage* apMessage = NULL);
        
    public:
        /**
        *  Extraction getter for response data set by the hostlet application
        *  @return reference to response body. Caller takes ownership of the returned object.
        */
        IMPORT_C HBufC8* ExtractResponseUtf8();
        
        /**
        *  Getter for response data set by the hostlet application
        *  @return reference to response body. Ownership is not transferred.
        */
        IMPORT_C virtual TPtrC8 ResponseUtf8();

        /** 
        * Getter for request id
        */
        IMPORT_C virtual TInt RequestId() const;

        /** 
        * Getter for response code set by the hostlet application
        */
        IMPORT_C virtual TInt ResponseCode() const;
        
        /**
        * Getter for response properties
        */
        IMPORT_C virtual MSenProperties* ResponseProperties();

        /**
        * Getter for response as a message object. Note that this message
        * can be safely class-cast further to a concrete subclass via the
        * MSenMessage::IsSafeToCast() helper function.
        * @return message or NULL if the response data is not available
        */
        IMPORT_C virtual MSenMessage* ResponseMessageL();	//codescannerwarnings
        
    protected:
        void BaseConstructL();
    
    private:
        CSenHostletResponse(const TInt aRequestId);
        
        MSenMessageContext& MessageContextL(MSenProperties* apProperties = NULL);
        
    private:
        TInt iRequestId;
        TInt iResponseCode;
        HBufC8* ipOwnedResponseUtf8;
        TAny* iReserved; // not used currently
        CSenMessageContext* ipMessageContext;
    };

#endif
