/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*      Implementation of class CDownloadDataClient.   
*      
*
*/


// INCLUDE FILES

#include <s32mem.h>
#include "DownloadDataClient.h"
#include "CodData.h"
#include "CodUtil.h"

// ================= CONSTANTS =======================


// ================= MEMBER FUNCTIONS =======================

    
// ---------------------------------------------------------
// CMediaDataClient::NewL()
// ---------------------------------------------------------
//
CMediaDataClient* CMediaDataClient::NewL()
    {
    CMediaDataClient* data = new (ELeave) CMediaDataClient();
    CleanupStack::PushL( data );
    data->ConstructL();
    CleanupStack::Pop(); //data
    return data;
    }
    
// ---------------------------------------------------------
// CMediaDataClient::NewLC()
// ---------------------------------------------------------
//
CMediaDataClient* CMediaDataClient::NewLC()
    {
    CMediaDataClient* data = new (ELeave) CMediaDataClient();
    CleanupStack::PushL( data );
    data->ConstructL();
    return data;
    }

// ---------------------------------------------------------
// CMediaDataClient::~CMediaDataClient()
// ---------------------------------------------------------
//    
CMediaDataClient::~CMediaDataClient()
    {
    delete iName;
    delete iUrl;
    delete iIcon;
    delete iSourceUri;
    delete iRedirUrl;
    delete iDestFilename;
    delete iTempFilename;
    }
    
// ---------------------------------------------------------
// CMediaDataClient::ConstructL()
// ---------------------------------------------------------
//
void CMediaDataClient::ConstructL()
    {
    iName = HBufC::NewL( 0 );
    iUrl = HBufC8::NewL( 0 );
    iTypes = new (ELeave) CDesC8ArrayFlat( KTypeGranularity );
    iIcon = HBufC8::NewL( 0 );
    iSourceUri = HBufC8::NewL( 0 );
    iRedirUrl = HBufC8::NewL( 0 );
    iDestFilename = HBufC::NewL( 0 );
    iTempFilename = HBufC::NewL( 0 );
    }
    
// ---------------------------------------------------------
// CMediaDataClient::Bytes()
// ---------------------------------------------------------
//
TInt CMediaDataClient::Bytes() const
    {
    TInt bytes = 0;
    
    bytes += iName->Size();
    bytes += iUrl->Size();
    bytes += sizeof(iSize);
    bytes += iIcon->Size();
    bytes += iSourceUri->Size();
    bytes += sizeof(iProgressiveDl);
    bytes += sizeof(iState);
    
    bytes += iRedirUrl->Size();
    bytes += sizeof(iMethod);
    bytes += sizeof(iRedirected);
    bytes += iDestFilename->Size();
    bytes += sizeof(iDownloadedSize);
    bytes += sizeof(iDesRemovable);
    bytes += sizeof(iLastErrorId);
    bytes += sizeof(iGlobalErrorId);
    bytes += sizeof(iPausable);
    bytes += iTempFilename->Size();
    
    // iTypes array elements
    for (TInt index = 0; index < iTypes->MdcaCount(); ++index)
        bytes += iTypes->MdcaPoint(index).Size();
    
    return bytes;
    }

// ---------------------------------------------------------
// CMediaDataClient::SetNameL()
// ---------------------------------------------------------
//
TBool CMediaDataClient::SetNameL( const TDesC& aName )
    {
    return SetStringAttrL( iName, aName, COD_NAME_MAX_LEN );
    }

// ---------------------------------------------------------
// CMediaDataClient::SetUrlL()
// ---------------------------------------------------------
//
TBool CMediaDataClient::SetUrlL( const TDesC8& aUrl )
    {
    return SetStringAttrL( iUrl, aUrl, COD_URL_MAX_LEN );
    }

// ---------------------------------------------------------
// CMediaDataClient::SetSize()
// ---------------------------------------------------------
//
TBool CMediaDataClient::SetSize( TUint aSize )
    {
    if( aSize <= 0 )
        {
        return EFalse;
        }
    iSize = aSize;
    return ETrue;
    }

// ---------------------------------------------------------
// CMediaDataClient::SetTypeL()
// ---------------------------------------------------------
//
TBool CMediaDataClient::SetTypeL( const TDesC8& aType )
    {
    // Backwards-compatible deprecated method.
    iTypes->Reset();    // Replace all, support only one type.
    return AddTypeL( aType );
    }

// ---------------------------------------------------------
// CMediaDataClient::AddTypeL()
// ---------------------------------------------------------
//
TBool CMediaDataClient::AddTypeL( const TDesC8& aType )
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
// CMediaDataClient::ResetTypes()
// ---------------------------------------------------------
//
void CMediaDataClient::ResetTypes()
	{
	iTypes->Reset();
	}

// ---------------------------------------------------------
// CMediaDataClient::SetIconL()
// ---------------------------------------------------------
//
TBool CMediaDataClient::SetIconL( const TDesC8& aIcon )
    {
    return SetStringAttrL( iIcon, aIcon, COD_ICON_MAX_LEN );
    }

// ---------------------------------------------------------
// CMediaDataClient::SetSourceUriL()
// ---------------------------------------------------------
//
void CMediaDataClient::SetSourceUriL( const TDesC8& aSourceUri )
    {
    HBufC8* buf = aSourceUri.AllocL();
    delete iSourceUri;
    iSourceUri = buf;
    }

// ---------------------------------------------------------
// CMediaDataClient::SetProgressiveDownload()
// ---------------------------------------------------------
//
void CMediaDataClient::SetProgressiveDownload( TBool aProgressiveDl )
    {
    iProgressiveDl = aProgressiveDl;
    }

// ---------------------------------------------------------
// CMediaDataClient::SetState()
// ---------------------------------------------------------
//
void CMediaDataClient::SetState( TMediaObjectState aState )
    {
    iState = aState;
    }

// ---------------------------------------------------------
// CMediaDataClient::SetResult()
// ---------------------------------------------------------
//
void CMediaDataClient::SetResult( TInt aResult )
    {
    iResult = aResult;
    }
// ---------------------------------------------------------
// CMediaDataClient::SetStatusCode()
// ---------------------------------------------------------
//
void CMediaDataClient::SetStatusCode( TInt aStatusCode )
    {
    iStatusCode = aStatusCode;
    }

// ---------------------------------------------------------
// CMediaDataClient::SetRedirUrlL()
// ---------------------------------------------------------
//
TBool CMediaDataClient::SetRedirUrlL( const TDesC8& aRedirUrl )
	{
	return SetStringAttrL( iRedirUrl, aRedirUrl, COD_URL_MAX_LEN );
	}

// ---------------------------------------------------------
// CMediaDataClient::SetMethod()
// ---------------------------------------------------------
//
void CMediaDataClient::SetMethod( TInt aMethod )
	{
	iMethod = aMethod;
	}

// ---------------------------------------------------------
// CMediaDataClient::SetRedirected()
// ---------------------------------------------------------
//
void CMediaDataClient::SetRedirected( TBool aRedirected )
	{
	iRedirected = aRedirected;
	}

// ---------------------------------------------------------
// CMediaDataClient::SetDestFilenameL()
// ---------------------------------------------------------
//
TBool CMediaDataClient::SetDestFilenameL( const TDesC& aDestFilename )
	{
	return SetStringAttrL( iDestFilename, aDestFilename, KMaxFileName );
	}
	
// ---------------------------------------------------------
// CMediaDataClient::SetTempFilenameL()
// ---------------------------------------------------------
//
TBool CMediaDataClient::SetTempFilenameL( const TDesC& aTempFilename )
	{
	return SetStringAttrL( iTempFilename, aTempFilename, KMaxFileName );
	}

// ---------------------------------------------------------
// CMediaDataClient::SetDownloadedSize()
// ---------------------------------------------------------
//
TBool CMediaDataClient::SetDownloadedSize( TInt aDownloadedSize )
	{
	if( aDownloadedSize <= 0 )
        {
        return EFalse;
        }
    iDownloadedSize = aDownloadedSize;
    return ETrue;
	}

// ---------------------------------------------------------
// CMediaDataClient::SetDesRemovable()
// ---------------------------------------------------------
//
void CMediaDataClient::SetDesRemovable( TBool aDesRemovable )
	{
	iDesRemovable = aDesRemovable;
	}

// ---------------------------------------------------------
// CMediaDataClient::SetLastErrorId()
// ---------------------------------------------------------
//
void CMediaDataClient::SetLastErrorId( TInt aLastErrorId )
	{
	iLastErrorId = aLastErrorId;
	}

// ---------------------------------------------------------
// CMediaDataClient::SetGlobalErrorId()
// ---------------------------------------------------------
//
void CMediaDataClient::SetGlobalErrorId( TInt aGlobalErrorId )
	{
	iGlobalErrorId = aGlobalErrorId;
	}

// ---------------------------------------------------------
// CMediaDataClient::SetPausable()
// ---------------------------------------------------------
//
void CMediaDataClient::SetPausable( TBool aPausable )
	{
	iPausable = aPausable;
	}
	
// ---------------------------------------------------------
// CMediaDataClient::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CMediaDataClient::SetStringAttrL
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
// CMediaDataClient::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CMediaDataClient::SetStringAttrL
( HBufC8*& aBuf, const TDesC& aString, TInt aMaxLength )
    {
    if ( aString.Length() > aMaxLength )
        {
        return EFalse;
        }
        
    HBufC8* buf = CodUtil::ConvertL( aString );
    delete aBuf;
    aBuf = buf;
    return ETrue;
    }

// ---------------------------------------------------------
// CMediaDataClient::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CMediaDataClient::SetStringAttrL
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
// CMediaDataClient::MarshalDataL()
// ---------------------------------------------------------
//
HBufC8* CMediaDataClient::MarshalDataL() const
    {
    TInt bufLen = Bytes();      // Size of class including iMediaArray elements.
                                //  Note that this includes actual bytes occupied by
                                //  contents of descriptors and pointers.
                                
    bufLen += sizeof(TInt);  	// We include the count of elements in iTypes array
                                //  while externalizing.
                                
    
    // Dynamic data buffer
    CBufFlat* buf = CBufFlat::NewL(bufLen);
    CleanupStack::PushL(buf);
    // Stream over the buffer
    RBufWriteStream stream(*buf);
    CleanupClosePushL(stream);
    
    ExternalizeL(stream);
    CleanupStack::PopAndDestroy(); //stream
    
    // Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewL(buf->Size());
    TPtr8 ptr(des->Des());
    buf->Read(0, ptr, buf->Size());
    CleanupStack::PopAndDestroy(); //buf
    
    return des;
    }

// ---------------------------------------------------------
// CMediaDataClient::ExternalizeL()
// ---------------------------------------------------------
//	
void CMediaDataClient::ExternalizeL(RWriteStream& aStream) const
    {
    // iName
    if (iName)
        aStream << *iName;
    else
        aStream << KNullDesC;
    
    // iUrl
    if (iUrl)
        aStream << *iUrl;
    else
        aStream << KNullDesC;
    
    // iSize
    aStream.WriteUint32L(iSize);
    
    // Number of elements in iTypes array
    if (iTypes)
        {
        TInt count = iTypes->MdcaCount();
        aStream.WriteInt32L(count);
        // Elements of iTypes array
        for (TInt index = 0; index < count; ++index)
            aStream << iTypes->MdcaPoint(index);
        }
    else
        {
        aStream.WriteInt32L(0);
        }
    
    // iIcon
    if (iIcon)
        aStream << *iIcon;
    else
        aStream << KNullDesC;
    
    // iSourceUri
    if (iSourceUri)
        aStream << *iSourceUri;
    else
        aStream << KNullDesC;
    
    // iProgressiveDl
    aStream.WriteInt32L(iProgressiveDl);
    
    // iState
    aStream.WriteInt32L(iState);
    
    // iResult
    aStream.WriteInt32L(iResult);
    
    // iRedirUrl
    if (iRedirUrl)
        aStream << *iRedirUrl;
    else
        aStream << KNullDesC;
    
    // iMethod
    aStream.WriteInt32L(iMethod);
    
    // iRedirected
    aStream.WriteInt32L(iRedirected);
    
    // iDestFilename
    if (iIcon)
        aStream << *iDestFilename;
    else
        aStream << KNullDesC;
    
    // iDownloadedSize
    aStream.WriteInt32L(iDownloadedSize);
    
    // iDesRemovable
    aStream.WriteInt32L(iDesRemovable);
    
    // iLastErrorId
    aStream.WriteInt32L(iLastErrorId);
    
    // iGlobalErrorId
    aStream.WriteInt32L(iGlobalErrorId);
    
    // iPausable
    aStream.WriteInt32L(iPausable);
    // iStatusCode
    aStream.WriteInt32L(iStatusCode);
    }

void CMediaDataClient::InternalizeL(RReadStream& /*aStream*/)
    {
    }

// ---------------------------------------------------------
// CDownloadDataClient::NewL()
// ---------------------------------------------------------
//
CDownloadDataClient* CDownloadDataClient::NewL()
    {
    CDownloadDataClient* data = new (ELeave) CDownloadDataClient();
    CleanupStack::PushL( data );
    data->ConstructL();
    CleanupStack::Pop(); //data
    return data;
    }

// ---------------------------------------------------------
// CDownloadDataClient::NewLC()
// ---------------------------------------------------------
//
CDownloadDataClient* CDownloadDataClient::NewLC()
    {
    CDownloadDataClient* data = new (ELeave) CDownloadDataClient();
    CleanupStack::PushL( data );
    data->ConstructL();
    return data;
    }

// ---------------------------------------------------------
// CDownloadDataClient::~CDownloadDataClient()
// ---------------------------------------------------------
//
CDownloadDataClient::~CDownloadDataClient()
    {
    delete iName;
    delete iIcon;
	delete iUpdatedDDUri;
    
    iMediaArray.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CDownloadDataClient::ConstructL()
// ---------------------------------------------------------
//
void CDownloadDataClient::ConstructL()
    {
    iName = HBufC::NewL( 0 );
    iSize = 0;
    iIcon = HBufC8::NewL( 0 );
	iUpdatedDDUri = HBufC8::NewL( 0 );
    }

// ---------------------------------------------------------
// CDownloadDataClient::Bytes()
// ---------------------------------------------------------
//
TInt CDownloadDataClient::Bytes() const
    {
    TInt bytes = 0;
    
    bytes += iName->Size();
    bytes += sizeof(iSize);
    bytes += iIcon->Size();
    bytes += iUpdatedDDUri->Size();
    
    // iMediaArray elements 
    for (TInt index = 0; index < iMediaArray.Count(); ++index)
        bytes += iMediaArray[index]->Bytes();
    
    return bytes;
    }

// ---------------------------------------------------------
// CDownloadDataClient::SetNameL()
// ---------------------------------------------------------
//
TBool CDownloadDataClient::SetNameL( const TDesC& aName )
    {
    return SetStringAttrL( iName, aName, COD_NAME_MAX_LEN );
    }

// ---------------------------------------------------------
// CDownloadDataClient::SetSize()
// ---------------------------------------------------------
//
TBool CDownloadDataClient::SetSize( TUint aSize )
    {
    if( aSize <= 0 )
        {
        return EFalse;
        }
    iSize = aSize;
    return ETrue;
    }

// ---------------------------------------------------------
// CDownloadDataClient::SetIconL()
// ---------------------------------------------------------
//
TBool CDownloadDataClient::SetIconL( const TDesC8& aIcon )
    {
    return SetStringAttrL( iIcon, aIcon, COD_ICON_MAX_LEN );
    }
    
// ---------------------------------------------------------
// CDownloadDataClient::SetUpdatedDDURI()
// ---------------------------------------------------------
//
TBool CDownloadDataClient::SetUpdatedDDURI( const TDesC8& aUrl)
    {
    return SetStringAttrL( iUpdatedDDUri, aUrl, COD_URL_MAX_LEN );
    }    

// ---------------------------------------------------------
// CDownloadDataClient::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CDownloadDataClient::SetStringAttrL
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
// CDownloadDataClient::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CDownloadDataClient::SetStringAttrL
( HBufC8*& aBuf, const TDesC& aString, TInt aMaxLength )
    {
    if ( aString.Length() > aMaxLength )
        {
        return EFalse;
        }
    HBufC8* buf = CodUtil::ConvertL( aString );
    delete aBuf;
    aBuf = buf;
    return ETrue;
    }

// ---------------------------------------------------------
// CDownloadDataClient::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CDownloadDataClient::SetStringAttrL
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
// CDownloadDataClient::AppendMediaData()
// ---------------------------------------------------------
//
TInt CDownloadDataClient::AppendMediaData( CMediaDataClient *aMO )
    {
    iMediaArray.AppendL( aMO );
    // return item index
    return iMediaArray.Count();    
    }
    
// ---------------------------------------------------------
// CDownloadDataClient::operator[]
// ---------------------------------------------------------
//

CMediaDataBase* CDownloadDataClient::operator[]( TInt aIndex )
    {
    //Check if the media object exits at specified index. Return the same if it is.
    if( aIndex >= 0 && aIndex < iMediaArray.Count() )
        {
        return iMediaArray[aIndex];
        }
    return NULL;    
    }

// ---------------------------------------------------------
// CDownloadDataClient::operator[]
// ---------------------------------------------------------
//

CMediaDataBase* CDownloadDataClient::operator[]( TInt aIndex ) const
    {
    //Check if the media object exits at specified index. Return the same if it is.
    if( aIndex >= 0 && aIndex < iMediaArray.Count() )
        {
        return iMediaArray[aIndex];
        }
    return NULL;    
    }

// ---------------------------------------------------------
// CDownloadDataClient::MarshalDataL()
// ---------------------------------------------------------
//
HBufC8* CDownloadDataClient::MarshalDataL() const
    {
    TInt bufLen = Bytes();      // Size of class including iMediaArray elements.
                                //  Note that this includes actual bytes occupied by
                                //  contents of descriptors and pointers.
                                
    bufLen += sizeof(TInt);  // We include the count of elements in iMediaArray
                                //  while externalizing.
                                
    bufLen += sizeof(TInt) * iMediaArray.Count();
                                // iMediaArray has an array iTypes. We are including
                                //  count of elements in iTypes array while externalizing
                                //  each element of iMediaArray.
    
    // Dynamic data buffer
    CBufFlat* buf = CBufFlat::NewL(bufLen);
    CleanupStack::PushL(buf);
    // Stream over the buffer
    RBufWriteStream stream(*buf);
    CleanupClosePushL(stream);
    
    ExternalizeL(stream);
    CleanupStack::PopAndDestroy(); //stream
    
    // Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewL(buf->Size());
    TPtr8 ptr(des->Des());
    buf->Read(0, ptr, buf->Size());
    CleanupStack::PopAndDestroy(); //buf
    
    return des;
    }

// ---------------------------------------------------------
// CDownloadDataClient::ExternalizeL()
// ---------------------------------------------------------
//
void CDownloadDataClient::ExternalizeL(RWriteStream& aStream) const
    {
    // iName
    if (iName)
        aStream << *iName;
    else
        aStream << KNullDesC;
    
    // iSize
    aStream.WriteUint32L(iSize);
    
    // iIcon
    if (iIcon)
        aStream << *iIcon;
    else
        aStream << KNullDesC;
    
    // iUpdatedDDUri
    if (iUpdatedDDUri)
        aStream << *iUpdatedDDUri;
    else
        aStream << KNullDesC;
    
    // iMediaArray elements 
    TInt count = iMediaArray.Count();
    aStream.WriteInt32L(count);
    for (TInt index = 0; index < count; ++index)
        iMediaArray[index]->ExternalizeL(aStream);
   
    }
