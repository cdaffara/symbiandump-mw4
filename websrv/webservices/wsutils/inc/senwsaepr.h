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
* Description: Header declaration
*
*/










#ifndef SEN_WSAEPR_H
#define SEN_WSAEPR_H

#include "SenBaseFragment.h"

namespace
    {
    _LIT8(KName, "EndpointReference");
    _LIT8(KNsPrefix, "wsa");

    // Note: below value is from editor's draft of WS-A spec. 

    _LIT8(KMessageIdNs, "http://www.w3.org/2005/03/addressing") ;
    }


class CSenWsaMessageId : public CSenBaseFragment
    {
    /*
    public:

        IMPORT_C static CSenWsaMessageId* NewL();

        IMPORT_C virtual ~CSenWsaMessageId();

    public:

        IMPORT_C virtual TPtrC8 MessageId();

    private: // Data
        HBufC8* ipMessageId;         // Owned
    */
    };

#endif // WSA_MESSAGEID_H
