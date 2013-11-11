/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        Inlines for the Transport ECOM plug-ins
*
*/








namespace
    {
    _LIT(KPanic, "SenTransport");
    }

// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plugin framework to instantiate appropriate concrete plugin
// implementation.
//
// Instantiates the default WSF transport: HTTP Channel plug-in
inline CSenTransport* CSenTransport::NewL(CSenTransportContext* apCtx)
    {
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
    const TUid KHttpChannelTransportPluginUid = {0x10274C74};
#else
    const TUid KHttpChannelTransportPluginUid = {0x101F974A};
#endif

    TAny* constructorParameters = reinterpret_cast<TAny*>(apCtx);

    // Find implementation behind KHttpChannelTransportPluginUid
    TAny* intf = REComSession::CreateImplementationL(KHttpChannelTransportPluginUid, 
                                                     _FOFF (CSenTransport, 
                                                     iDtor_ID_Key), 
                                                     constructorParameters);


    return reinterpret_cast <CSenTransport*> (intf);
    }

// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plugin framework to instantiate appropriate concret plugin
// implementation.
inline CSenTransport* CSenTransport::NewL(const TDesC8& aCue, 
                                          CSenTransportContext* apCtx)
    {
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
    const TUid KCSenTransportInterfaceUid = {0x10274C72};
#else
    const TUid KCSenTransportInterfaceUid = {0x101F9749};
#endif

    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aCue);
    resolverParams.SetWildcardMatch(EFalse);

    TAny* constructorParameters = reinterpret_cast<TAny*>(apCtx);

    // Find implementation for our interface.
    // - KCSenTransportInterfaceUid is the UID of Transport ECOM plugin.
    // - This call will leave, if the plugin architecture cannot find
    //   implementation.
    // - The returned pointer points to one of our interface implementation
    //   instances.
    TAny* intf = REComSession::CreateImplementationL (KCSenTransportInterfaceUid,
                                                      _FOFF (CSenTransport, 
                                                      iDtor_ID_Key), 
                                                      constructorParameters, 
                                                      resolverParams); 

    return reinterpret_cast<CSenTransport*> (intf);
    }


// Interface's (abstract base class's) destructor
inline CSenTransport::~CSenTransport()
    {
    // If in the NewL some memory is reserved for member data, it must be
    // released here.
    delete ipCtx;
    ipCtx = NULL;

    // Inform the ECOM framework that this specific instance of the
    // interface has been destroyed.
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

inline CSenTransport::CSenTransport(CSenTransportContext* apCtx,
        TBool aSessionOwned):
        CSenTransportBase(aSessionOwned), ipCtx(apCtx)
        {
        }

inline MSenTransportContext& CSenTransport::Context()
    {
    __ASSERT_ALWAYS(ipCtx != NULL, User::Panic(KPanic, ETransportCtxNotInitialized));

    return *ipCtx;
    }

// End of File
