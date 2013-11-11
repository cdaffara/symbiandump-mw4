/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef M_SEN_TRANSPORT_CONTEXT_H
#define M_SEN_TRANSPORT_CONTEXT_H

// INCLUDES
#include <e32std.h>
#include "msencontext.h"

/**
* This class defines abstract interface for transport plug-in context, 
* which is visible to CSenTransport ECOM plug-in implementations.
*/

// FORWARD DECLARATIONS
class MSenRemoteServiceSession;
class MSenCoreServiceManager;
class CSenWSDescription;
class CSenServiceSession;


// CLASS DECLARATION
class MSenTransportContext : public MSenContext
    {
    public:
        /**
        * Adds new MSenCoreServiceManager& type value into context,
        * typically identified by this key: KSenCtxKeyCoreServiceManager.
        * @aKey identifies new value
        * @aValue is the value being added. Ownership is NEVER transferred
        * to this context.
        * @return KErrNone on success, 
        *         KErrAlreadyExists, if the key is reserved,
        *         or some other system-wide error code.
        */
        // virtual TInt AddCore(MSenCoreServiceManager& aValue) = 0;

        /*
        * Method returns MSenCoreServiceManager* type value.
        * @return reference to Core Service Manager.
        * LeaveCode:
        *        KErrNotFound - If Core is not available in this context
        *        KErrSenTypeMismatch - If KSenCtxKeyCoreServiceManager
        *        illegally points to other than CSenCoreServiceManager
        *        instance (not to happen).
        */
        virtual MSenCoreServiceManager& GetCoreL() = 0;
        //virtual MSenCoreServiceManager* GetCore() = 0;

        /*
        * Adds session description (as CSenWSDescription*) into context,
        * typically identified by following key: KSenCtxKeyServiceSession
        * 
        * @param aValue is the value being added. Ownership is NOT transferred
        * to this context.
        * 
        * @return KErrNone on success, 
        *         KErrAlreadyExists, if the key is reserved,
        *         or some other system-wide error code.
        */
        // virtual TInt AddSession(CSenServiceSession* aValue) = 0; // protected context

        /*
        * Updates new CSenWSDescription* type value into context, typically
        * identified by following key: KSenCtxKeyServiceSession
        * If some value for the same key already exists, it is replaced with the new value.
        * @param aValue is the updated value, which may replace
        *         any existing value, which used same key.
        *         Ownership is NOT transferred to this context.
        * @return KErrNone on success, 
        *         or some other system-wide error code.
        */
        //virtual TInt UpdateSession(CSenServiceSession* aValue) = 0;  // protected context

        /*
        * Method returns the service session which loaded (created) this transport plug-in instance.
        * 
        * @return value behind given key, or NULL, if such is not found.
        * Ownership cannot be transferred, as it is never to be held by this context.   
        * Method leaves with KErrSenTypeMismatch error code,
        * if value at this index is not CSenServiceSession type,
        * or any of it's subclass's.
        */
        virtual const CSenServiceSession* GetSession() = 0;
        //virtual CSenServiceSession& GetSessionL() = 0;

        // virtual TInt AddInitializer(CSenWSDescription* aValue) = 0; // protected context
        // virtual TInt UpdateInitializer(CSenWSDescription* aValue) = 0;  // protected context

        /*
        * Method returns session initializer as CSenWSDescriptionL* type value.
        * 
        * @returns value behind given key, or NULL, if not
        * found.
        * Ownership is NOT transferred, as it is NEVER to be held by this context.
        * Method leaves with KErrSenTypeMismatch error code,
        * if value at this index is not CSenWSDescription*
        * type, or any of it's subclass's.   
        */
        virtual CSenWSDescription& GetInitializerL() = 0;

        //virtual const CSenWSDescription* GetInitializer() = 0;

        /*
        * Adds new CSenWSDescription* type value into context, identified by given key.
        * 
        * @param aKey identifies new value
        * @param aValue is the value being added. Ownership is transferred
        * to this context.
        * 
        * @return KErrNone on success, 
        *         KErrAlreadyExists, if the key is reserved,
        *         or some other system-wide error code.
        */
        //virtual TInt AddServiceDescription(const TDesC8& aKey, CSenWSDescription* aValue) = 0;

        /*
        * Updates session description (as CSenWSDescription*) into context,
        * typically identified by following key: KSenCtxKeyServiceSession
        * If some value for this key already exists, it is destroyed and replaced
        * by the new value.
        * 
        * @param aKey identifies new value
        * @param aValue is the updated value, which may replace
        *         any existing value, which used same key.
        *         Ownership is transferred to this context.
        * @return KErrNone on success, 
        *         or some other system-wide error code.
        */
        //virtual TInt UpdateServiceDescription(const TDesC8& aKey, CSenWSDescription* aValue) = 0;

        /*
        * Method returns CSenWSDescriptionL* type value.
        * 
        * @return value behind given key, or NULL, if not
        * found.
        * Ownership is  not transferred, but remains
        * to be held by this context.   
        * Method leaves with KErrSenTypeMismatch error code,
        * if value at this index is not CSenSoapEnvelope* type.   
        */
        //virtual const CSenWSDescription* GetServiceDescriptionL(const TDesC8& aKey) = 0;

        /*
        * Adds new MSenRemoteServiceSession& type value into context, identified by given key.
        * 
        * @param aKey identifies new value
        * @param aValue is the value being added. Ownership is transferred
        * to this context.
        * 
        * @return KErrNone on success, 
        *         KErrAlreadyExists, if the key is reserved,
        *         or some other system-wide error code.
        */
        //virtual TInt Add(const TDesC8& aKey, MSenRemoteServiceSession& aValue) = 0; // protected context

        /*
        * Updates new MSenRemoteServiceSession& type value into context, identified by given key.
        * If some value for the same key already exists, it is destroyed and replaced
        * by the new value.
        * 
        * @param aKey identifies new value
        * @param aValue is the updated value, which may replace
        *         any existing value, which used same key.
        *         Ownership is transferred to this context.
        * 
        * @return KErrNone on success, 
        *         or some other system-wide error code.
        */
        //virtual TInt Update(const TDesC8& aKey, MSenRemoteServiceSession& aValue) = 0;

        /*
        * Method returns MSenRemoteServiceSession* type value.
        * 
        * @param aKey identifies remote service session
        * 
        * @returns value behind given key, or NULL, if not
        * found.
        * Ownership is  not transferred, but remains
        * to be held by this context.   
        * Method leaves with KErrSenTypeMismatch error code,
        * if value at this index is not MSenRemoteServiceSession* type.   
        */
        //virtual const MSenRemoteServiceSession* GetSenRemoteServiceSessionL(const TDesC8& aKey) = 0;
    };

#endif // M_SEN_SESSION_CONTEXT_H

// End of File
