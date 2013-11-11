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
* Description:        Inlines for the Handler ECOM plug-ins
*
*/








// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plugin framework to instantiate appropriate concrete plug-in
// implementation.
//
// Instantiates the default message handler plug-in
inline CSenSessionHandler* CSenSessionHandler::NewL(MSenHandlerContext& aCtx)
    {
    const TUid KSenDefaultSessionHandlerPluginUid = {0x20000395};

    TAny* constructorParameters = reinterpret_cast<TAny*>(&aCtx);

    // Find implementation behind KTestHandlerPluginUid
    TAny* intf = REComSession::CreateImplementationL(KSenDefaultSessionHandlerPluginUid, 
                                                     _FOFF (CSenSessionHandler, 
                                                     iDtor_ID_Key), 
                                                     constructorParameters);

    if(intf)
        {
        _LIT8(KSenDefaultSessionHandlerPluginName, "urn:com.nokia.serene:handlers:session:default");
        CSenSessionHandler* handler = reinterpret_cast <CSenSessionHandler*> (intf);
        CleanupStack::PushL(handler);
        handler->iName = KSenDefaultSessionHandlerPluginName().AllocL();
        handler->InitL(aCtx);
        CleanupStack::Pop(); // handler
        }

    return reinterpret_cast <CSenSessionHandler*> (intf);
    }

// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plugin framework to instantiate appropriate concret plug-in
// implementation.
inline CSenSessionHandler* CSenSessionHandler::NewL(const TDesC8& aCue, 
                                                    MSenHandlerContext& aCtx)
    {
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES    
    const TUid KSenSessionHandlerInterfaceUid = {0x20000392};
#else
    const TUid KSenSessionHandlerInterfaceUid = {0x10282C5B};
#endif    

    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aCue);
    resolverParams.SetWildcardMatch(EFalse);

    TAny* constructorParameters = reinterpret_cast<TAny*>(&aCtx);

    // Find implementation for our interface.
    // - KCSenSessionHandlerInterfaceUid is the UID of Handler ECOM interface.
    // - This call will leave, if the plugin architecture cannot find
    //   implementation.
    // - The returned pointer points to one of our interface implementation
    //   instances.
    TAny* intf = REComSession::CreateImplementationL(KSenSessionHandlerInterfaceUid ,
                                                     _FOFF (CSenSessionHandler, 
                                                     iDtor_ID_Key), 
                                                     constructorParameters, 
                                                     resolverParams); 

    if(intf)
        {
        CSenSessionHandler* handler = reinterpret_cast <CSenSessionHandler*> (intf);
        CleanupStack::PushL(handler);
        handler->iName = aCue.AllocL();
        handler->InitL(aCtx);
        CleanupStack::Pop(); // handler
        }
    return reinterpret_cast<CSenSessionHandler*> (intf);
    }


// Interface's (abstract base class's) destructor
inline CSenSessionHandler::~CSenSessionHandler()
    {
    // Any member data must be released here. 
    delete iName; 

    // Inform the ECOM framework that this specific instance of the
    // interface has been destroyed.
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

inline CSenSessionHandler::CSenSessionHandler(MSenHandlerContext& aCtx)
        :iHandlerContext(aCtx)        
        {
        }

inline SenHandler::THandlerType CSenSessionHandler::Type() const
    {
    return SenHandler::ESessionHandler;
    }

inline TPtrC8 CSenSessionHandler::Name() const
    {
    if(iName)
        return *iName;
    else
        return KNullDesC8();
    }


// End of File
