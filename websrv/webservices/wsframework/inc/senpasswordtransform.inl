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
* Description:        Defines the ECom interface for Sen Security Mechanism plug-ins
*
*/








// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plugin framework to instantiate appropriate concret plugin
// implementation.
inline CSenPasswordTransform* CSenPasswordTransform::NewL(MSenCoreServiceManager& aManager)
    {
    // Wish to instantiate the default interface - TruncateTransform
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES        
    const TUid KTruncateTransformPluginUid = {0x101F9708};
#else
    const TUid KTruncateTransformPluginUid = {0x101F973E};
#endif    

    TAny* constructorParameters = reinterpret_cast<TAny*>(&aManager);

    // Find implementation behind KTruncateTransformPluginUid
    TAny* intf = REComSession::CreateImplementationL (KTruncateTransformPluginUid, 
                                                      _FOFF (CSenPasswordTransform, 
                                                      iDtor_ID_Key), 
                                                      constructorParameters);

    return reinterpret_cast <CSenPasswordTransform*> (intf);
    }

// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plugin framework to instantiate appropriate concret plugin
// implementation.
inline CSenPasswordTransform* CSenPasswordTransform::NewL(const TDesC8& aMatch, 
                                                          MSenCoreServiceManager& aManager)
    {
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES    
    const TUid KCSenPasswordTransformInterfaceUid = {0x101F9707};
#else
    const TUid KCSenPasswordTransformInterfaceUid = {0x101F973D};
#endif    

    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aMatch);
    resolverParams.SetWildcardMatch(EFalse);

    TAny* constructorParameters = reinterpret_cast<TAny*>(&aManager);

    // Find implementation for our interface.
    // - KCSenPasswordTransformInterfaceUid is the 
    //   UID of Password Transform ECOM interface.          
    //
    // - This call will leave, if the plugin architecture cannot find
    //   implementation.
    // - The returned pointer points to one of our interface implementation
    //   instances.
    TAny* intf = REComSession::CreateImplementationL (KCSenPasswordTransformInterfaceUid,
                                                      _FOFF (CSenPasswordTransform, 
                                                      iDtor_ID_Key), 
                                                      constructorParameters, 
                                                      resolverParams); 

    return reinterpret_cast<CSenPasswordTransform*> (intf);
    }

// Interface's (abstract base class's) destructor
inline CSenPasswordTransform::~CSenPasswordTransform()
    {
    // If in the NewL some memory is reserved for member data, it must be
    // released here. This interface does not have any instance variables so
    // no need to delete anything.

    // Inform the ECOM framework that this specific instance of the
    // interface has been destroyed.
    REComSession::DestroyedImplementation (iDtor_ID_Key);
    }

inline CSenPasswordTransform::CSenPasswordTransform(MSenCoreServiceManager& aServiceManager)
        :iServiceManager(aServiceManager)
        {
        }

// End of File
