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
* Description:        Defines the ECom interface for Sen Security Mechanism plug-ins
*
*/








// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plugin framework to instantiate appropriate concrete plugin
// implementation.
inline CSenSecurityMechanism* CSenSecurityMechanism::NewL(MSenCoreServiceManager& aManager)
    {
    // Instantiate the default SASL plugin - AnonymousSasl
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES    
    const TUid KAnonymousSaslPluginUid = {0x101F96FD};
#else
    const TUid KAnonymousSaslPluginUid = {0x101F9734};
#endif    

    TAny* constructorParameters = reinterpret_cast<TAny*>(&aManager);

    // Find implementation behind KAnonymousSaslPluginUid
    TAny* intf = REComSession::CreateImplementationL (KAnonymousSaslPluginUid, 
                                                      _FOFF (CSenSecurityMechanism, 
                                                      iDtor_ID_Key), 
                                                      constructorParameters);

    return reinterpret_cast <CSenSecurityMechanism*> (intf);
    }

// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plugin framework to instantiate appropriate concret plugin
// implementation.
inline CSenSecurityMechanism* CSenSecurityMechanism::NewL(const TDesC8& aMatch, 
                                                          MSenCoreServiceManager& aManager)
    {
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES
    const TUid KCSenSecurityMechanismInterfaceUid = {0x101F96FC};
#else
    const TUid KCSenSecurityMechanismInterfaceUid = {0x101F9733};
#endif

    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aMatch);
    resolverParams.SetWildcardMatch(EFalse);

    TAny* constructorParameters = reinterpret_cast<TAny*>(&aManager);

    // Find implementation for our interface.
    // - KCSenSecurityMechanismInterfaceUid is the UID of
    //  SASL SECURITY MECHANISM ECOM interface.         
    //
    // - This call will leave, if the plugin architecture 
    // cannot find implementation.
    // - The returned pointer points to one of our interface implementation
    //   instances.
    TAny* intf = REComSession::CreateImplementationL (KCSenSecurityMechanismInterfaceUid,
                                                      _FOFF (CSenSecurityMechanism, 
                                                      iDtor_ID_Key), 
                                                      constructorParameters, 
                                                      resolverParams); 

    return reinterpret_cast<CSenSecurityMechanism*> (intf);
    }


// Interface's (abstract base class's) destructor
inline CSenSecurityMechanism::~CSenSecurityMechanism()
    {
    // we need to call this, because destroying is too late
    // in virtual destructor in super class (virtual destructor & ECOM)
    // If superclass resets the contents, the memory will be in illegal
    // state in case that there is ONE or MORE password transform ECOM
    // instancies inside this array(!) -> this will cause crash in 
    // unknown location, typically in next destructor.
    iTransforms.ResetAndDestroy(); // were owned

    // Inform the ECOM framework that this specific instance of the
    // interface has been destroyed.
    REComSession::DestroyedImplementation (iDtor_ID_Key);
    }

inline CSenSecurityMechanism::CSenSecurityMechanism(MSenCoreServiceManager& aServiceManager)
        :iServiceManager(aServiceManager)
        {
        }

// End of File
