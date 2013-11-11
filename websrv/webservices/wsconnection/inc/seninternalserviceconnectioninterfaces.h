/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     In case that Service Consumer (owner of SC instance) needs
*                some special, even properietary interface, it can request
*                an implementation of such interface by calling
*                CSenServiceConnection::InterfaceByUid(UID) and acquire
*                on of the M-class interfaces defined in this file.    
*
*/




#ifndef M_SEN_INTERNAL_SERVICE_CONNNECTION_INTERFACES_H
#define M_SEN_INTERNAL_SERVICE_CONNNECTION_INTERFACES_H

// CONST
// UIDs for the supported, new *service connection interfaces*:
const TUid KSenInterfaceUidInternalServiceConnection = { 0xE760F698 }; // MSenInternalServiceConnection

// CLASS DECLARATION
/**
 * Callback interface for service consumers
 */
class MSenInternalServiceConnection
    {
    public: 
        virtual TInt PendingTrasanctionsCount() = 0;
        /**
        * Sends information about BLOB transfer progres to hostlet.
        *
        * @param aTxnId Transaction ID.
        * @param aIncoming ETrue if it is incoming BLOB, EFalse if outgoing.
        * @param aMessage SOAP message for incoming messages with BLOBs.
        * @param aCid CID of current BLOB.
        * @param aProgress Count of sent/received BLOB bytes.
        */
        virtual TInt SendProgressToHostlet( TInt aTxnId, 
                                            TBool aIncoming,
                                            const TDesC8& aMessage, 
                                            const TDesC8& aCid, 
                                            TInt aProgress ) = 0;
        
    };
   
#endif //  M_SEN_INTERNAL_SERVICE_CONNNECTION_INTERFACES_H   