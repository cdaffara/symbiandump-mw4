/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include <s32mem.h>
#include "DownloadDataServ.h"


// CONSTANTS


// ---------------------------------------------------------
// CMediaDataServ::NewL()
// Two-phase Constructor - Creates object from stream
// ---------------------------------------------------------
// 
CMediaDataServ* CMediaDataServ::NewL(RReadStream& aStreamData)
    {
    CMediaDataServ* data = new (ELeave) CMediaDataServ();
    CleanupStack::PushL(data);
    data->InternalizeL(aStreamData);
    CleanupStack::Pop(); //data
    return data;
    }

// ---------------------------------------------------------
// CMediaDataServ::NewL()
// ---------------------------------------------------------
//
CMediaDataServ* CMediaDataServ::NewL(const TDesC8& aStreamData)
    {
    CMediaDataServ* data = new (ELeave) CMediaDataServ();
    CleanupStack::PushL( data );
    RDesReadStream stream(aStreamData);
    CleanupClosePushL(stream);
    data->InternalizeL(stream);
    CleanupStack::PopAndDestroy(); //stream
    CleanupStack::Pop(); //data
    return data;
    }

// ---------------------------------------------------------
// CMediaDataServ::~CMediaDataServ()
// ---------------------------------------------------------
//
CMediaDataServ::~CMediaDataServ()
    {
    delete iName;
    delete iUrl;
    delete iTypes;
    delete iIcon;
    delete iSourceUri;
    delete iRedirUrl;
    delete iDestFilename;
    delete iTempFilename;
    }

// CMediaDataServ::Bytes()
// ---------------------------------------------------------
//
TInt CMediaDataServ::Bytes() const
    {
    return 0;
    }
    
// ---------------------------------------------------------
// CMediaDataServ::SetNameL()
// ---------------------------------------------------------
//
TBool CMediaDataServ::SetNameL( const TDesC& aName )
    {
    return SetStringAttrL( iName, aName, COD_NAME_MAX_LEN );
    }

// ---------------------------------------------------------
// CMediaDataServ::SetUrlL()
// ---------------------------------------------------------
//
TBool CMediaDataServ::SetUrlL( const TDesC8& aUrl )
    {
    return SetStringAttrL( iUrl, aUrl, COD_URL_MAX_LEN );
    }

// ---------------------------------------------------------
// CMediaDataServ::SetSize()
// ---------------------------------------------------------
//
TBool CMediaDataServ::SetSize( TUint aSize )
    {
    if( aSize <= 0 )
        {
        return EFalse;
        }
    iSize = aSize;
    return ETrue;
    }

// ---------------------------------------------------------
// CMediaDataServ::SetTypeL()
// ---------------------------------------------------------
//
TBool CMediaDataServ::SetTypeL( const TDesC8& aType )
    {
    // Backwards-compatible deprecated method.
    iTypes->Reset();    // Replace all, support only one type.
    return AddTypeL( aType );
    }

// ---------------------------------------------------------
// CMediaDataServ::AddTypeL()
// ---------------------------------------------------------
//
TBool CMediaDataServ::AddTypeL( const TDesC8& aType )
    {
    if ( aType.Length() > COD_TYPE_MAX_LEN || !aType.Length() )
        {
        return EFalse;
        }
    HBufC8* type8 = aType.AllocLC();
    iTypes->AppendL( *type8 );
    CleanupStack::PopAndDestroy(); //type8
    return ETrue;
    }
    
// ---------------------------------------------------------
// CMediaDataServ::ResetTypes()
// ---------------------------------------------------------
//
void CMediaDataServ::ResetTypes()
	{
	iTypes->Reset();
	}

// ---------------------------------------------------------
// CMediaDataServ::SetIconL()
// ---------------------------------------------------------
//
TBool CMediaDataServ::SetIconL( const TDesC8& aIcon )
    {
    return SetStringAttrL( iIcon, aIcon, COD_ICON_MAX_LEN );
    }

// ---------------------------------------------------------
// CMediaDataServ::SetSourceUriL()
// ---------------------------------------------------------
//
void CMediaDataServ::SetSourceUriL( const TDesC8& aSourceUri )
    {
    HBufC8* buf = aSourceUri.AllocL();
    delete iSourceUri;
    iSourceUri = buf;
    }

// ---------------------------------------------------------
// CMediaDataServ::SetProgressiveDownload()
// ---------------------------------------------------------
//
void CMediaDataServ::SetProgressiveDownload( TBool aProgressiveDl )
    {
    iProgressiveDl = aProgressiveDl;
    }

// ---------------------------------------------------------
// CMediaDataServ::SetState()
// ---------------------------------------------------------
//
void CMediaDataServ::SetState( TMediaObjectState aState )
    {
    iState = aState;
    }

// ---------------------------------------------------------
// CMediaDataServ::SetResult()
// ---------------------------------------------------------
//
void CMediaDataServ::SetResult( TInt aResult )
    {
    iResult = aResult;
    }
// ---------------------------------------------------------
// CMediaDataServ::SetStatusCode()
// ---------------------------------------------------------
//
void CMediaDataServ::SetStatusCode( TInt aStatusCode )
    {
    iStatusCode = aStatusCode;
    }

// ---------------------------------------------------------
// CMediaDataServ::SetRedirUrlL()
// ---------------------------------------------------------
//
TBool CMediaDataServ::SetRedirUrlL( const TDesC8& aRedirUrl )
	{
	return SetStringAttrL( iRedirUrl, aRedirUrl, COD_URL_MAX_LEN );
	}

// ---------------------------------------------------------
// CMediaDataServ::SetMethod()
// ---------------------------------------------------------
//
void CMediaDataServ::SetMethod( TInt aMethod )
	{
	iMethod = aMethod;
	}

// ---------------------------------------------------------
// CMediaDataServ::SetRedirected()
// ---------------------------------------------------------
//
void CMediaDataServ::SetRedirected( TBool aRedirected )
	{
	iRedirected = aRedirected;
	}

// ---------------------------------------------------------
// CMediaDataServ::SetDestFilenameL()
// ---------------------------------------------------------
//
TBool CMediaDataServ::SetDestFilenameL( const TDesC& aDestFilename )
	{
	return SetStringAttrL( iDestFilename, aDestFilename, KMaxFileName );
	}

// ---------------------------------------------------------
// CMediaDataServ::SetDownloadedSize()
// ---------------------------------------------------------
//
TBool CMediaDataServ::SetDownloadedSize( TInt aDownloadedSize )
	{
	if( aDownloadedSize <= 0 )
        {
        return EFalse;
        }
    iDownloadedSize = aDownloadedSize;
    return ETrue;
	}

// ---------------------------------------------------------
// CMediaDataServ::SetDesRemovable()
// ---------------------------------------------------------
//
void CMediaDataServ::SetDesRemovable( TBool aDesRemovable )
	{
	iDesRemovable = aDesRemovable;
	}

// ---------------------------------------------------------
// CMediaDataServ::SetLastErrorId()
// ---------------------------------------------------------
//
void CMediaDataServ::SetLastErrorId( TInt aLastErrorId )
	{
	iLastErrorId = aLastErrorId;
	}

// ---------------------------------------------------------
// CMediaDataServ::SetGlobalErrorId()
// ---------------------------------------------------------
//
void CMediaDataServ::SetGlobalErrorId( TInt aGlobalErrorId )
	{
	iGlobalErrorId = aGlobalErrorId;
	}

// ---------------------------------------------------------
// CMediaDataServ::SetPausable()
// ---------------------------------------------------------
//
void CMediaDataServ::SetPausable( TBool aPausable )
	{
	iPausable = aPausable;
	}
	
// ---------------------------------------------------------
// CMediaDataServ::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CMediaDataServ::SetStringAttrL
( HBufC*& aBuf, const TDesC& aString, TInt aMaxLength )
    {
    if ( aString.Length() > aMaxLength )
        {
        return EFalse;
        }
    HBufC* buf = aString.AllocL();
    delete aBuf;
    aBuf = buf;
    return ETrue;
    }

// ---------------------------------------------------------
// CMediaDataServ::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CMediaDataServ::SetStringAttrL
( HBufC8*& aBuf, const TDesC& aString, TInt aMaxLength )
    {
    if ( aString.Length() > aMaxLength )
        {
        return EFalse;
        }
        
    HBufC8* buf = HBufC8::NewL( aString.Length() );
    buf->Des().Copy( aString );
    delete aBuf;
    aBuf = buf;
    return ETrue;
    }

// ---------------------------------------------------------
// CMediaDataServ::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CMediaDataServ::SetStringAttrL
( HBufC8*& aBuf, const TDesC8& aString, TInt aMaxLength )
    {
    if ( aString.Length() > aMaxLength )
        {
        return EFalse;
        }
    HBufC8* buf = aString.AllocL();
    delete aBuf;
    aBuf = buf;
    return ETrue;
    }
    
// ---------------------------------------------------------
// CMediaDataServ::ExternalizeL()
// ---------------------------------------------------------
//	
void CMediaDataServ::ExternalizeL(RWriteStream& /*aStream*/) const
    {
    }

// ---------------------------------------------------------
// CMediaDataServ::InternalizeL()
// ---------------------------------------------------------
//	
void CMediaDataServ::InternalizeL(RReadStream& aStream)
    {
    iName = HBufC::NewL(aStream, COD_NAME_MAX_LEN);
    iUrl = HBufC8::NewL(aStream, COD_URL_MAX_LEN);
    iSize = aStream.ReadUint32L();
    
    // Number of elements in iTypes array
    TInt count = aStream.ReadInt32L();
    // Fill elements of iTypes array
    iTypes = new (ELeave) CDesC8ArrayFlat( KTypeGranularity );
    for (TInt index = 0; index < count; ++index)
        {
        HBufC8* des8 = HBufC8::NewLC(aStream, COD_TYPE_MAX_LEN);
        iTypes->AppendL(*des8);
        CleanupStack::Pop(); //des8
        }
    
    iIcon = HBufC8::NewL(aStream, COD_ICON_MAX_LEN);
    iSourceUri = HBufC8::NewL(aStream, COD_URL_MAX_LEN);
    iProgressiveDl = aStream.ReadInt32L();    
    iState = static_cast<TMediaObjectState> (aStream.ReadInt32L());
    iResult = aStream.ReadInt32L();
    iRedirUrl = HBufC8::NewL(aStream, COD_URL_MAX_LEN);
    iMethod = aStream.ReadInt32L();
    iRedirected = aStream.ReadInt32L();
    iDestFilename = HBufC::NewL(aStream, KMaxFileName);
    iDownloadedSize = aStream.ReadInt32L();
    iDesRemovable = aStream.ReadInt32L();
    iLastErrorId = aStream.ReadInt32L();
    iGlobalErrorId = aStream.ReadInt32L();
    iPausable = aStream.ReadInt32L();
    
    
    iTempFilename = HBufC::NewL(KMaxFileName);
    iStatusCode = aStream.ReadInt32L();
    }  
    
// ---------------------------------------------------------
// CMediaDataServ::SetTempFilenameL()
// ---------------------------------------------------------
//
TBool CMediaDataServ::SetTempFilenameL( const TDesC& aTempFilename )
	{
	return SetStringAttrL( iTempFilename, aTempFilename, KMaxFileName );
	}
	    
// ---------------------------------------------------------
// CDownloadDataServ::NewL()
// ---------------------------------------------------------
//
CDownloadDataServ* CDownloadDataServ::NewL(const TDesC8& aStreamData)
    {
    CDownloadDataServ* data = new (ELeave) CDownloadDataServ();
    CleanupStack::PushL( data );
    RDesReadStream stream(aStreamData);
    CleanupClosePushL(stream);
    data->InternalizeL(stream);
    CleanupStack::PopAndDestroy(); //stream
    CleanupStack::Pop(); //data
    return data;
    }

// ---------------------------------------------------------
// CDownloadDataServ::~CDownloadDataServ()
// ---------------------------------------------------------
//
CDownloadDataServ::~CDownloadDataServ()
    {
    delete iName;
    delete iIcon;
	delete iUpdatedDDUri;
    
    iMediaArray.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CDownloadDataServ::operator[]
// ---------------------------------------------------------
//
CMediaDataBase* CDownloadDataServ::operator[]( TInt aIndex )
    {
    //Check if the media object exits at specified index. Return the same if it is.
    if( aIndex > 0 && aIndex <= iMediaArray.Count() )
        {
        return iMediaArray[aIndex-1];
        }
    return NULL;    
    }

// ---------------------------------------------------------
// CDownloadDataServ::operator[]
// ---------------------------------------------------------
//
CMediaDataBase* CDownloadDataServ::operator[]( TInt aIndex ) const
    {
    //Check if the media object exits at specified index. Return the same if it is.
    if( aIndex > 0 && aIndex <= iMediaArray.Count() )
        {
        return iMediaArray[aIndex-1];
        }
    return NULL;    
    }

// ---------------------------------------------------------
// CDownloadDataServ::InternalizeL()
// ---------------------------------------------------------
//
void CDownloadDataServ::InternalizeL(RReadStream& aStream)
    {
    iName = HBufC::NewL(aStream, COD_NAME_MAX_LEN);
    iSize = aStream.ReadUint32L();
    iIcon = HBufC8::NewL(aStream, COD_ICON_MAX_LEN);
    iUpdatedDDUri = HBufC8::NewL(aStream, COD_URL_MAX_LEN);
    
    // iMediaObjects array elements
    iMediaArray.Reset();
    TInt count = aStream.ReadInt32L();
    for (TInt index = 0; index < count; ++index)
        {
        CMediaDataServ* mediaData = CMediaDataServ::NewL(aStream);
        iMediaArray.AppendL(mediaData);
        }
    }

// ---------------------------------------------------------
// CDownloadDataServ::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CDownloadDataServ::SetStringAttrL
( HBufC*& aBuf, const TDesC& aString, TInt aMaxLength )
    {
    if ( aString.Length() > aMaxLength )
        {
        return EFalse;
        }
    HBufC* buf = aString.AllocL();
    delete aBuf;
    aBuf = buf;
    return ETrue;
    }

// ---------------------------------------------------------
// CDownloadDataServ::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CDownloadDataServ::SetStringAttrL
( HBufC8*& aBuf, const TDesC& aString, TInt aMaxLength )
    {
    if ( aString.Length() > aMaxLength )
        {
        return EFalse;
        }
        
    HBufC8* buf = HBufC8::NewL( aString.Length() );
    buf->Des().Copy( aString );
    delete aBuf;
    aBuf = buf;
    return ETrue;
    }

// ---------------------------------------------------------
// CDownloadDataServ::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CDownloadDataServ::SetStringAttrL
( HBufC8*& aBuf, const TDesC8& aString, TInt aMaxLength )
    {
    if ( aString.Length() > aMaxLength )
        {
        return EFalse;
        }
    HBufC8* buf = aString.AllocL();
    delete aBuf;
    aBuf = buf;
    return ETrue;
    }

// ---------------------------------------------------------
// CDownloadDataServ::SetNameL()
// ---------------------------------------------------------
//
TBool CDownloadDataServ::SetNameL( const TDesC& aName )
    {
    return SetStringAttrL( iName, aName, COD_NAME_MAX_LEN );
    }

//  End of File
