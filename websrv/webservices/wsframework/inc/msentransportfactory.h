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








#ifndef M_SEN_TRANSPORT_FACTORY_H
#define M_SEN_TRANSPORT_FACTORY_H

// FORWARD DECLARE
class MSenConsumerPolicy;
class CSenTransportBase;
class CSenWSDescription;
class CSenServiceSession;

// CLASS DECLARATION
class MSenTransportFactory
    {
    public:
    
        /**
        * CreateL() produces new CSenTransportBase instance from this factory.
        * The instance itself may or may not be an ECOM plug-in.
        * @param aInitializer is used to resolve what type of class
        * the new instance will be (the scheme of the endpoint can also
        * indicate which ECOM cue should be used when creating a new instance)
        * Additionally, one can pass some SD subclass, like CSenWSPattern
        * to this method. Consumer and Provider policies may further be included
        * within such SD, and the transport related context may be applied from
        * those policies.
        * @param apSession is a pointer (which ownership will not be changed)
        * to the session which request new transport.
        * @return pointer to new transport instance, a CSenTransportBase*,
        *         or NULL if no transport could be created using input policy.
        */
        virtual CSenTransportBase* CreateL(CSenWSDescription& aInitializer,
                                           CSenServiceSession* apSession) = 0;
    };

#endif // M_SEN_TRANSPORT_FACTORY_H

// End of File
