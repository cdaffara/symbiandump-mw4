/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CUpnpSettings
 *
*/


// INCLUDE FILES
#include "upnpsettings.h"
#include <centralrepository.h>
#include "upnpstring.h"
#include "upnpcompvariant.hrh"
#include <f32file.h> 

const TInt KBufferGranularity = 256;
// =========================== MEMBER FUNCTIONS ===============================
// ----------------------------------------------------------------------------
// CUpnpSettings::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CUpnpSettings::ConstructL(TUid aRepositoryUid)
{
    iRepository = CRepository::NewL(aRepositoryUid);       
}

// ----------------------------------------------------------------------------
// CUpnpSettings::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CUpnpSettings* CUpnpSettings::NewL(TUid aRepositoryUid)
{
    CUpnpSettings* self = new (ELeave) CUpnpSettings();
    CleanupStack::PushL(self);
    self->ConstructL(aRepositoryUid);
    CleanupStack::Pop();
    return self;
}

// ----------------------------------------------------------------------------
// CUpnpSettings::CUpnpSettings
// Destructor
// ----------------------------------------------------------------------------
//
CUpnpSettings::~CUpnpSettings()
{
    delete iRepository;
}

// ----------------------------------------------------------------------------
// CUpnpSettings::Set
// See upnpsettings.h
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSettings::Set(TUint aId, TInt& aValue)
{  
    if ((aId == KUPnPStackFileBufferSize)&&(aValue<0))
        return KErrArgument;
    else  
        return iRepository->Set(aId, aValue);       
}


// ----------------------------------------------------------------------------
// CUpnpSettings::Get
// See upnpsettings.h
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSettings::Get(TUint aId, TInt& aValue)
{
    return iRepository->Get(aId, aValue);
}
    


// ----------------------------------------------------------------------------
// CUpnpSettings::Set
// See upnpsettings.h
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSettings::SetL(TUint aId, const TDesC8& aValue)
{
    HBufC* data = UpnpString::ToUnicodeL(aValue);
    TPtr dataPtr(data->Des());
    TInt err = iRepository->Set(aId, dataPtr);
    delete data;
    return err;
}    

// ----------------------------------------------------------------------------
// CUpnpSettings::Set
// See upnpsettings.h
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSettings::Set(TUint aId, const TDesC& aValue)
{
    return iRepository->Set(aId, aValue);
}

// ----------------------------------------------------------------------------
// CUpnpSettings::Get
// See upnpsettings.h
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSettings::Get(TUint aId, TDes& aValue)
{
    return iRepository->Get(aId, aValue);
}    
// ----------------------------------------------------------------------------
// CUpnpSettings::Get
// See upnpsettings.h
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CUpnpSettings::GetL(TUint aId)
    {
    TInt len(0);
    HBufC* buf(0);
    TInt err(KErrNotFound);
            
    do //until the buffer is large enough
        {
        len += KBufferGranularity;
        delete buf; // first time it is Null
        buf = HBufC::NewL(len);
        TPtr bufPtr(buf->Des());
        err = iRepository->Get(aId, bufPtr);
        } while(err == KErrOverflow );
        
    // handle error
    if(err)
        {
        delete buf;
        User::Leave(err);
        }
        
    // convert to UTF-8
    CleanupStack::PushL(buf);       
    HBufC8* buf8 = UpnpString::FromUnicodeL(*buf);
    CleanupStack::PopAndDestroy(buf);
    return buf8;
    }

// ----------------------------------------------------------------------------
// CUpnpSettings::GetUserAgentL
// See upnpsettings.h
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CUpnpSettings::GetUserAgentL()
    {       
    #if defined(__HN_31__) || defined(__HN_32__)
    CUpnpSettings* usc = CUpnpSettings::NewL( KHnCRUidUPnPStack );
    #else
    CUpnpSettings* usc = CUpnpSettings::NewL( KCRUidUPnPStack );
    #endif
    CleanupStack::PushL(usc);
    HBufC8* tmp = usc->GetL(CUpnpSettings::KUPnPStackCustomizedUserAgent);  
    CleanupStack::PopAndDestroy(usc);  
    return tmp;
    }



// ----------------------------------------------------------------------------
// CUpnpSettings::Get
// See upnpsettings.h
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSettings::GetIapL()
{
    TInt noIap = 0;     
    #if defined(__HN_31__) || defined(__HN_32__)
    CUpnpSettings* usc = CUpnpSettings::NewL( KHnCRUidUPnPStack );
    #else
    CUpnpSettings* usc = CUpnpSettings::NewL( KCRUidUPnPStack );
    #endif
    CleanupStack::PushL(usc);
    usc->Get(CUpnpSettings::KUPnPStackIapId, noIap);  
    CleanupStack::PopAndDestroy(usc);  
    return noIap;
}

// ----------------------------------------------------------------------------
// CUpnpSettings::GetFileBufferSizeL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpSettings::GetFileBufferSizeL()
    {
    TInt size = 0;  
    #if defined(__HN_31__) || defined(__HN_32__)
    CUpnpSettings* usc = CUpnpSettings::NewL( KHnCRUidUPnPStack );
    #else
    CUpnpSettings* usc = CUpnpSettings::NewL( KCRUidUPnPStack );
    #endif
    CleanupStack::PushL(usc);
    User::LeaveIfError(usc->Get(CUpnpSettings::KUPnPStackFileBufferSize, size));  
    CleanupStack::PopAndDestroy(usc);  
    return size;
    }

// ----------------------------------------------------------------------------
// CUpnpSettings::GetServerHeaderL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CUpnpSettings::GetServerHeaderL()
{
    #if defined(__HN_31__) || defined(__HN_32__)
    CUpnpSettings* usc = CUpnpSettings::NewL( KHnCRUidUPnPStack );
    #else
    CUpnpSettings* usc = CUpnpSettings::NewL( KCRUidUPnPStack );
    #endif
    CleanupStack::PushL(usc);
    HBufC8* tmp = usc->GetL(CUpnpSettings::KUPnPStackCustomizedServerHeader);  
    CleanupStack::PopAndDestroy(usc);  
    return tmp;
}

// ----------------------------------------------------------------------------
// CUpnpSettings::GetServerHeaderL
// ---------------------------------------------------------------------------
//
HBufC8* CUpnpSettings::GetMSearchConfigurationL()
{
    #if defined(__HN_31__) || defined(__HN_32__)
    CUpnpSettings* usc = CUpnpSettings::NewL( KHnCRUidUPnPStack );
    #else
    CUpnpSettings* usc = CUpnpSettings::NewL( KCRUidUPnPStack );
    #endif
    CleanupStack::PushL(usc);
    HBufC8* tmp = usc->GetL(CUpnpSettings::KUPnPStackMSearchConfig);  
    CleanupStack::PopAndDestroy(usc);  
    return tmp;
}

// End of File
