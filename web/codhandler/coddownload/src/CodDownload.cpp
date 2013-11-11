/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implementation of class CCodDownload.   
*
*/


// INCLUDE FILES

#include "CodDownload.h"
#include "CodDownloadImpl.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodDownload::NewL()
// ---------------------------------------------------------
//
EXPORT_C CCodDownload* CCodDownload::NewL
        (
        const TDesC8& aBuf,
        const TDesC8& aType,
        MCodDownloadObserver& aObserver,
        CEikonEnv* aEikEnv
        )
    {
    CCodDownload* download = new (ELeave) CCodDownload();
    CleanupStack::PushL( download );
    download->iImpl = CCodDownloadImpl::NewL
        ( *download, aBuf, aType, aObserver, aEikEnv, NULL, NULL );
    CleanupStack::Pop( download );
    return download;
    }

// ---------------------------------------------------------
// CCodDownload::NewL()
// ---------------------------------------------------------
//
EXPORT_C CCodDownload* CCodDownload::NewL
        (
        const TInt aDownloadId,
        const TUid aDlUid,
        const TDesC8& aType,
        MCodDownloadObserver& aObserver,
        CEikonEnv* aEikEnv )
    {
    CCodDownload* download = new (ELeave) CCodDownload();
    CleanupStack::PushL( download );
    download->iImpl = CCodDownloadImpl::NewL
        ( *download, aDownloadId, aDlUid ,aType, aObserver, aEikEnv, NULL, NULL );
    CleanupStack::Pop( download );
    return download;
    }
// ---------------------------------------------------------
// CCodDownload::NewL()
// ---------------------------------------------------------
//
EXPORT_C CCodDownload* CCodDownload::NewL
        (
        const TDesC8& aBuf,
        const TDesC8& aType,
        MCodDownloadObserver& aObserver,
        CEikonEnv* aEikEnv,
        const TDesC8* aMultipartBoundary,
        const TDesC16* aMultipartUrl
        )
    {
    CCodDownload* download = new (ELeave) CCodDownload();
    CleanupStack::PushL( download );
    download->iImpl = CCodDownloadImpl::NewL( *download, aBuf, aType,
        aObserver, aEikEnv, aMultipartBoundary, aMultipartUrl );
    CleanupStack::Pop( download );
    return download;
    }

// ---------------------------------------------------------
// CCodDownload::~CCodDownload()
// ---------------------------------------------------------
//
EXPORT_C CCodDownload::~CCodDownload()
    {
    delete iImpl;
    }

// ---------------------------------------------------------
// CCodDownload::StartL()
// ---------------------------------------------------------
//
EXPORT_C void CCodDownload::StartL( const CAiwGenericParamList* aParams )
    {
    iImpl->StartL( aParams );
    }

// ---------------------------------------------------------
// CCodDownload::Stop()
// ---------------------------------------------------------
//
EXPORT_C void CCodDownload::Stop()
    {
    iImpl->Stop();
    }

// ---------------------------------------------------------
// CCodDownload::Progress()
// ---------------------------------------------------------
//
EXPORT_C const TCodDownloadProgress& CCodDownload::Progress() const
    {
    return iImpl->Progress();
    }

// ---------------------------------------------------------
// CCodDownload::Name()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCodDownload::Name()
    {
    return iImpl->Name();
    }

// ---------------------------------------------------------
// CCodDownload::Type()
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CCodDownload::Type() const
    {
    return iImpl->Type();
    }

// ---------------------------------------------------------
// CCodDownload::GetPath()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCodDownload::GetPath() const
    {
    return iImpl->GetPath();
    }
    
// ---------------------------------------------------------
// CCodDownload::GetDestFilePath()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCodDownload::GetDestFilePath (TInt aMOIndex ) const
    {
    return iImpl->GetDestFilePath(aMOIndex);
    }    

// ---------------------------------------------------------
// CCodDownload::ServiceFlowL()
// ---------------------------------------------------------
//
EXPORT_C void CCodDownload::ServiceFlowL()
    {
    iImpl->ServiceFlowL();
    }

// ---------------------------------------------------------
// CCodDownload::NextUrlL()
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CCodDownload::NextUrlL() const
    {
    return iImpl->NextUrlL();
    }

// ---------------------------------------------------------
// CCodDownload::GetPostResponseUrlL()
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CCodDownload::GetPostResponseUrlL() const
    {
    return iImpl->GetPostResponseUrlL();
    }
    

// ---------------------------------------------------------
// CCodDownload::RemovableMedia()
// ---------------------------------------------------------
//
EXPORT_C TBool CCodDownload::RemovableMedia() const
    {
    return iImpl->RemovableMedia();
    }

// ---------------------------------------------------------
// CCodDownload::UserData()
// ---------------------------------------------------------
//
EXPORT_C TAny* CCodDownload::UserData() const
    {
    return iImpl->UserData();
    }

// ---------------------------------------------------------
// CCodDownload::SetUserData()
// ---------------------------------------------------------
//
EXPORT_C void CCodDownload::SetUserData( TAny* aUserData )
    {
    iImpl->SetUserData( aUserData );
    }

// ---------------------------------------------------------
// CCodDownload::SetCodDlAttached()
// ---------------------------------------------------------
//
EXPORT_C void CCodDownload::SetCodDlAttached(const TBool aValue)
    {
    iImpl->SetCodDlAttached(aValue);
    }
    
// ---------------------------------------------------------
// CCodDownload::UpdatedDDURI()
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CCodDownload::UpdatedDDUriL() const
    {
    return iImpl->UpdatedDDUriL();
    }

// ---------------------------------------------------------
// CCodDownload::UpdatedDownloadDataL()
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CCodDownload::UpdatedDownloadDataL() const
    {
    return iImpl->UpdatedDownloadDataL();
    }
    
// ---------------------------------------------------------
// CCodDownload::UpdatedTrackDataL()
// ---------------------------------------------------------
//
EXPORT_C HBufC8* CCodDownload::UpdatedTrackDataL(TInt& aValue) const
    {
    return iImpl->UpdatedTrackDataL(aValue);
    }

// ---------------------------------------------------------
// CCodDownload::CCodDownload()
// ---------------------------------------------------------
//
CCodDownload::CCodDownload()
    {
    }

// ---------------------------------------------------------
// CCodDownload::Pause()
// ---------------------------------------------------------
//    
EXPORT_C void CCodDownload::Pause()
    {
    iImpl->Pause();
    }


// ---------------------------------------------------------
// CCodDownload::ActiveDownload()
// ---------------------------------------------------------
//        
EXPORT_C TInt CCodDownload::ActiveDownload()
    {
    return iImpl->ActiveDownload();
    }
