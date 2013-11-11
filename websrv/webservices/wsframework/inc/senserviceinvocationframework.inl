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
* Description:           Defines the ECom interface for Sen Service Invocation
*                Framework plug-ins
*
*/








#include <SenCredential2.h>

// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plug-in framework to instantiate appropriate concrete plug-in
// implementation.
inline CSIF* CSIF::NewL(MSenCoreServiceManager& aManager)
    {
    // Caller wants to instantiate the default plug-in - ID-WSF  
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES      
    const TUid KIDWSFInterfaceUid = {0x101F96FA};
#else
    const TUid KIDWSFInterfaceUid = {0x101F9731};
#endif    

    TAny* constructorParameters = reinterpret_cast<TAny*>(&aManager);

    // Find implementation for our interface.
    // - KIDWSFInterfaceUid is the UID of our custom ECOM interface.
    // - This call will leave, if the plugin architecture cannot find any implementation.
    // - The returned pointer points to one of the interface implementations.
    TUid dtor_id_key;
    TAny* intf = REComSession::CreateImplementationL ( KIDWSFInterfaceUid, dtor_id_key, constructorParameters );
    //TAny* intf = REComSession::CreateImplementationL ( KIDWSFUid, _FOFF (CSIF, dtor_id_key), constructorParameters );
    CSIF* sif = reinterpret_cast <CSIF*> (intf);
    if ( sif )
        {
        
        // Resolve the DataType of this plug-in (ECOM cue):
        RImplInfoPtrArray ecomInfoArray;
        REComSession::ListImplementationsL( KIDWSFInterfaceUid, ecomInfoArray );

        TInt infoCount(ecomInfoArray.Count());
        // NOTE: current code uses the plug-in that has highest version number
        //TInt index_with_highest_version(0);
        CImplementationInformation* match = NULL;
        for (TInt i=0; i<infoCount; i++)
            {
            CImplementationInformation* candidate = ecomInfoArray[i];
            if( !match || candidate->Version() > match->Version() )
                {
                match = candidate;
                // index of plug-in with highest version number
                //index_with_highest_version = i;
                }
            }
        CleanupStack::PushL( sif );
        sif->iData = new (ELeave) TSifData();
        sif->iData->iDtor_ID_Key = dtor_id_key;
        if( match )
            {
            sif->iData->iCue = match->DataType().AllocL();
            }
        CleanupStack::Pop( sif );
        ecomInfoArray.ResetAndDestroy();        
        }

    return sif;
    }

// Interface's (abstract base class's) static factory method implementation.
// Asks ECOM plug-in framework to instantiate appropriate concrete plug-in
// implementation.
inline CSIF* CSIF::NewL(const TDesC8& aCue, MSenCoreServiceManager& aManager)
    {
#ifndef RD_SEN_COMPILE_SIS_PACKAGE_FILES    
    const TUid KCSIFInterfaceUid = {0x101F96F9};
#else
    const TUid KCSIFInterfaceUid = {0x101F9730};
#endif    
    //const TUid KCSIFResolverUid = {0x101F614E}; // default resolver used, not needed..

    TEComResolverParams resolverParams;
    resolverParams.SetDataType( aCue );
    //resolverParams.SetWildcardMatch(ETrue); 
    resolverParams.SetWildcardMatch(EFalse); 

    TAny* constructorParameters = reinterpret_cast<TAny*>(&aManager);
    

    // Find implementation for our interface.
    // - XXX is the UID of our custom ECOM
    //   interface. It is defined in EComInterfaceDefinition.h          
    // - This call will leave, if the plugin architecture cannot find implementation.
    // - The returned pointer points to one of the interface implementations
    TUid dtor_id_key;
    TAny* intf = REComSession::CreateImplementationL (KCSIFInterfaceUid, dtor_id_key, constructorParameters, resolverParams); 
    // TAny* intf = REComSession::CreateImplementationL (KCSIFInterfaceUid, _FOFF (CSIF, dtor_id_key), constructorParameters, resolverParams); 

    CSIF* sif = reinterpret_cast<CSIF*> (intf);
    if ( sif )
        {
        CleanupStack::PushL( sif );
        sif->iData = new (ELeave) TSifData();
        sif->iData->iDtor_ID_Key = dtor_id_key;
        sif->iData->iCue = aCue.AllocL();
        CleanupStack::Pop( sif );
        }

    return sif;
    }

// Interface's (abstract base class's) destructor
inline CSIF::~CSIF()
    {
    // If in the NewL some memory is reserved for member data, it must be
    // released here.
    if( iData ) // actually, this MUST always be true
        {
        // Inform the ECOM framework that this specific instance of the
        // interface has been destroyed.
        REComSession::DestroyedImplementation ( iData->iDtor_ID_Key );
        iData->Close(); // de-allocates iData members
        delete iData;
        iData = NULL;
        }
    }

inline TPtrC8 CSIF::Cue() const
    {
    if(iData && iData->iCue)
        return *(iData->iCue);
    else
        return KNullDesC8();
    }
    
inline void CSIF::TSifData::Close()
    {
    delete iCue;
    iCue = NULL;
    }
    

// End of File
