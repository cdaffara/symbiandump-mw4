/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        In case that Service Consumer (owner of SC instance) needs
*                some special, even properietary interface, it can request
*                an implementation of such interface by calling
*                CSenServiceManager::InterfaceByUid(UID) and acquire
*                on of the M-class interfaces defined in this file.
*
*/








#ifndef M_SEN_SERVICE_CONNNECTION_INTERFACES_H
#define M_SEN_SERVICE_CONNNECTION_INTERFACES_H

// CONST
// UIDs for the supported, new *service manager interfaces*:
namespace
    {
    const TUid KSenInterfaceUidInternalServiceManager = { 0xE760F698 }; // MSenInternalServiceManager
    }

typedef RPointerArray<CSenIdentityProvider> RIdentityProviderArray;

// CLASS DECLARATION
/**
 * Internal callback interfaces:
 */
class MSenInternalServiceManager
    {
    public: 
        virtual TInt IdentityProvidersL( RIdentityProviderArray& aList ) = 0;
    };
    
#endif //  M_SEN_SERVICE_CONNNECTION_INTERFACES_H   