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
* Description:    Header declaration
*
*/








// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plug-in framework to instantiate appropriate concrete plug-in
// implementation.

// What to do if there are NO providers to be found - should there always be a 
// "dummy" provider?

inline CSenProvider* CSenProvider::NewL()
    {
    // Wish to instantiate the default provider - just a test provider...
    const TUid KTestProviderUid = {0x10273114};
    
    // Find implementation behind KTestProviderUid.
    TAny* intf = REComSession::CreateImplementationL(KTestProviderUid, 
        _FOFF (CSenProvider, 
        iDtor_ID_Key), 
        NULL);

    if(intf)
        {
        CSenProvider* provider = reinterpret_cast <CSenProvider*> (intf);
        CleanupStack::PushL(provider);
        provider->iCriticalSection.CreateLocal();
        _LIT8(KTestProviderCue, "local://com.nokia.provider/TestProvider");
        provider->iCue=KTestProviderCue().AllocL();
        CleanupStack::Pop(); // provider
        }

    return reinterpret_cast <CSenProvider*> (intf);
    }

// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plug-in framework to instantiate appropriate concrete plug-in
// implementation.
inline CSenProvider* CSenProvider::NewL(const TDesC8& aCue)
    {
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES     
    const TUid KUidSenHostletPlugin = { 0x02000502 };
#else
    const TUid KUidSenHostletPlugin = { 0x10282C5D };
#endif    
    
    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aCue);
    resolverParams.SetWildcardMatch(EFalse);
    
    // Find implementation for our interface.
    
    TAny* intf = REComSession::CreateImplementationL(KUidSenHostletPlugin,
        _FOFF (CSenProvider, iDtor_ID_Key), 
        NULL, 
        resolverParams); 
    
    if(intf)
        {
        CSenProvider* provider = reinterpret_cast <CSenProvider*> (intf);
        provider->iCriticalSection.CreateLocal();
        CleanupStack::PushL(provider);
        provider->iCue = aCue.AllocL();
        CleanupStack::Pop(); // provider
        }
    
    return reinterpret_cast<CSenProvider*> (intf);
    }


// Interface's (abstract base class's) destructor
inline CSenProvider::~CSenProvider()
    {
    // If in the NewL some memory is reserved for member data, it must be
    // released here. This interface does not have any instance variables so
    // no need to delete anything.
    
    // Inform the ECOM framework that this specific instance of the
    // interface has been destroyed.
    REComSession::DestroyedImplementation (iDtor_ID_Key);
    delete iCue;
    iCue = NULL;
    iCriticalSection.Close();
    }

inline TPtrC8 CSenProvider::Endpoint() const
    {
    if(iCue)
        return *iCue;
    else
        return KNullDesC8();
    }

// protected
inline void CSenProvider::DescribeServiceL(CSenWSDescription& aSD)
    {
    aSD.SetEndPointL(Endpoint());
    aSD.SetContractL(Contract());
    aSD.SetFrameworkIdL(FrameworkId());
      
    if(Exposed())
        {
        CSenFacet* pExposed = CSenFacet::NewL();
        CleanupStack::PushL(pExposed);
        pExposed->SetNameL(KExposeLocalServiceFacet); // from MSenServiceDescription.h
        aSD.SetFacetL(*pExposed);
        CleanupStack::PopAndDestroy(); // pExposed
        }
    
    if(Sharable())
        {
        CSenFacet* pSharable = CSenFacet::NewL();
        CleanupStack::PushL(pSharable);
        pSharable->SetNameL(KProviderSharableFacet);
        aSD.SetFacetL(*pSharable);
        CleanupStack::PopAndDestroy(); // pSharable

       }
    
    if(CompleteClientMessages())
        {
        CSenFacet* pCompleteClientMsgs = CSenFacet::NewL();
        CleanupStack::PushL(pCompleteClientMsgs);
        pCompleteClientMsgs->SetNameL(KProviderCompleteClientMsgsFacet);
        aSD.SetFacetL(*pCompleteClientMsgs);
        CleanupStack::PopAndDestroy(); // pCompleteClientMsgs
        }

    if(LoadOnStartup())
        {
        CSenFacet* pLoadOnStartup = CSenFacet::NewL();
        CleanupStack::PushL(pLoadOnStartup);
        pLoadOnStartup->SetNameL(KProviderLoadOnStartupFacet);
        aSD.SetFacetL(*pLoadOnStartup);
        CleanupStack::PopAndDestroy(); // pLoadOnStartup
        }

    if(Threadsafe())
        {
        CSenFacet* pThreadsafe = CSenFacet::NewL();
        CleanupStack::PushL(pThreadsafe);
        pThreadsafe->SetNameL(KProviderThreadsafeFacet);
        aSD.SetFacetL(*pThreadsafe);
        CleanupStack::PopAndDestroy(); // pLoadOnStartup
        }
    
    if(Reinitializable())
        {
        CSenFacet* pReinitializable = CSenFacet::NewL();
        CleanupStack::PushL(pReinitializable);
        pReinitializable->SetNameL(KProviderReinitializableFacet);
        aSD.SetFacetL(*pReinitializable);
        CleanupStack::PopAndDestroy(); // pReinitializable
        }
    
    if(StayOnBackground())
        {
        CSenFacet* pStayOnBackgroudnd = CSenFacet::NewL();
        CleanupStack::PushL(pStayOnBackgroudnd);
        pStayOnBackgroudnd->SetNameL(KProviderStayOnBackgroundFacet);
        aSD.SetFacetL(*pStayOnBackgroudnd);
        CleanupStack::PopAndDestroy(); // pStayOnBackgroudnd
        }
    }
  
inline TBool CSenProvider::Exposed() const
    {
    return EFalse;
    }
    
inline TBool CSenProvider::Sharable() const
    {
    return EFalse;
    }

inline TBool CSenProvider::CompleteClientMessages() const
    {
    return EFalse;
    }


inline TBool CSenProvider::LoadOnStartup() const
    {
    return EFalse;
    }

inline TBool CSenProvider::Threadsafe() const
    {
    return ETrue;
    }

inline TBool CSenProvider::Reinitializable() const
    {
    return EFalse;
    }

inline TBool CSenProvider::StayOnBackground() const
    {
    return EFalse;
    }

inline TInt CSenProvider::ReinitL()
    {
    return KErrNotSupported;
    }

// END OF FILE


