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








#ifndef R_SEN_HOSTLET_CONNECTION_H
#define R_SEN_HOSTLET_CONNECTION_H

// INCLUDES
#include <e32base.h>
#include <flogger.h>

// FORWARD DECLARE
class CSenSoapEnvelope;
class CSenChunk;

// CLASS DECLARATION
class RSenHostletConnection : public RSessionBase
    {
public:
        /**
        * C++ default constructor.
        */
        RSenHostletConnection();
        
        // New functions
        TInt Connect();
        TVersion Version() const;
        
        void CancelSession(TRequestStatus& aStatus);

        TInt Establish(TDesC8& aHostletServiceDescription);
        void AwaitRequest(TRequestStatus& aStatus, TDes8& aErrPtr, TDes8& aTxnPtr);
        TInt AcquireRequestHandle(TDes8& aErrPtr, TDes8& aTxnPtr);

        void ProvideResponse(TRequestStatus& aStatus, 
                             TDes8& aErrPtr, TDes8& aTxnPtr, 
                             CSenChunk& aClientOp, 
                             const TInt aResponseCode);
                             
        TInt ConnectionID();
        TInt RegisterTransferObserver(const TDesC* aServerName, TAny* aConnection);

//        void SetTlsLogChannel(TInt aTlsLogChannel);
        
   private: // Helper functions
        RFileLogger* Log() const;

    private: // Data
        RFileLogger* iLog;
        TInt iTlsLogChannel;
    };

#endif //R_SEN_HOSTLET_CONNECTION_H

