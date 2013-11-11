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
*      Implementation of class CMediaObjectData.   
*      
*
*/


// INCLUDE FILES

#include "MediaObject.h"
#include "CodDefs.h"
#include "CodUtil.h"
#include <badesca.h>
#include "FileExt.h"
#include "CodStatus.h"
#include "HeaderField.h"
#include <apmstd.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMediaObjectData::NewL()
// ---------------------------------------------------------
//
EXPORT_C CMediaObjectData* CMediaObjectData::NewL()
    {
    CMediaObjectData* data = new (ELeave) CMediaObjectData();
    CleanupStack::PushL( data );
    data->ConstructL();
    CleanupStack::Pop( data );
    return data;
    }

// ---------------------------------------------------------
// CMediaObjectData::~CMediaObjectData()
// ---------------------------------------------------------
//
EXPORT_C CMediaObjectData::~CMediaObjectData()
    {
    delete iIcon;
    delete iPrice;
    delete iInfoUrl;
    delete iInstallNotify;
    delete iTypes;
    delete iUrl;
    delete iDescription;
    delete iName;
    delete iSourceUri;
    
    delete iFullName;
    }

// ---------------------------------------------------------
// CMediaObjectData::IsValid()
// ---------------------------------------------------------
//
EXPORT_C TBool CMediaObjectData::IsValid() const
    {
    return ( iUrl->Length() && iSize > 0 && iTypes->MdcaCount() );
    }

// ---------------------------------------------------------
// CMediaObjectData::IsPostOrder()
// ---------------------------------------------------------
//
EXPORT_C TBool CMediaObjectData::IsPostOrder() const
    {
    return iIsPostOrder;
    }

// ---------------------------------------------------------
// CMediaObjectData::Reset()
// ---------------------------------------------------------
//
EXPORT_C void CMediaObjectData::Reset()
    {
    iName->Des().Zero();
    iDescription->Des().Zero();
    iUrl->Des().Zero();
    iSize = 0;
    iTypes->Reset();
    iInstallNotify->Des().Zero();
    iInfoUrl->Des().Zero();
    iPrice->Des().Zero();
    iIcon->Des().Zero();
    iSourceUri->Des().Zero();
    iProgressiveDl = EFalse;
	iIsPostOrder = EFalse;
	
	iDownloadedSize = 0;
	iState = EQueued;
	iStatusCode = KHttp902UserCancelled;
	iResult = KErrGeneral;
	iFileName.SetLength( 0 );
	iPausable = EFalse;
	iUniqueFilenameSet = EFalse;
    }

// ---------------------------------------------------------
// CMediaObjectData::Name()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMediaObjectData::Name() const
    {
    return *iName;
    }

// ---------------------------------------------------------
// CMediaObjectData::Description()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMediaObjectData::Description() const
    {
    return *iDescription;
    }

// ---------------------------------------------------------
// CMediaObjectData::Url()
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CMediaObjectData::Url() const
    {
    return *iUrl;
    }

// ---------------------------------------------------------
// CMediaObjectData::Size()
// ---------------------------------------------------------
//
EXPORT_C TInt CMediaObjectData::Size() const
    {
    return iSize;
    }

// ---------------------------------------------------------
// CMediaObjectData::DownloadedSize()
// ---------------------------------------------------------
//
EXPORT_C TInt CMediaObjectData::DownloadedSize() const
    {
    return iDownloadedSize;
    }

// ---------------------------------------------------------
// CMediaObjectData::State()
// ---------------------------------------------------------
//
EXPORT_C TMediaObjectState CMediaObjectData::State() const
    {
    return iState;
    }
    
// ---------------------------------------------------------
// CMediaObjectData::Types()
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CMediaObjectData::Type() const
    {
    if ( iTypes->MdcaCount() )
        {
        return iTypes->MdcaPoint( 0 );
        }
    return TPtrC8( KNullDesC8 );
    }

// ---------------------------------------------------------
// CMediaObjectData::Types()
// ---------------------------------------------------------
//
EXPORT_C const MDesC8Array& CMediaObjectData::Types() const
    {
    return *iTypes;
    }


// ---------------------------------------------------------
// CMediaObjectData::InstallNotify()
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CMediaObjectData::InstallNotify() const
    {
    return *iInstallNotify;
    }

// ---------------------------------------------------------
// CMediaObjectData::InfoUrl
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CMediaObjectData::InfoUrl() const
    {
    return *iInfoUrl;
    }

// ---------------------------------------------------------
// CMediaObjectData::Price()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMediaObjectData::Price() const
    {
    return *iPrice;
    }

// ---------------------------------------------------------
// CMediaObjectData::Icon()
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CMediaObjectData::Icon() const
    {
    return *iIcon;
    }

// ---------------------------------------------------------
// CMediaObjectData::SourceUri()
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CMediaObjectData::SourceUri() const
    {
    return *iSourceUri;
    }

// ---------------------------------------------------------
// CMediaObjectData::ProgressiveDownload()
// ---------------------------------------------------------
//
EXPORT_C TBool CMediaObjectData::ProgressiveDownload() const
	{
    return iProgressiveDl;
	}

// ---------------------------------------------------------
// CMediaObjectData::HasType()
// ---------------------------------------------------------
//
TInt CMediaObjectData::HasType( const TDesC8& aType ) const
    {
    TInt i;
    return 0 == iTypes->Find( aType, i, ECmpNormal );
    }

// ---------------------------------------------------------
// CMediaObjectData::TypesCount()
// ---------------------------------------------------------
//
TInt CMediaObjectData::TypesCount() const
    {
    return iTypes->MdcaCount();
    }

// ---------------------------------------------------------
// CMediaObjectData::FullName()
// ---------------------------------------------------------
//
TPtrC CMediaObjectData::FullName() const
	{
	return *iFullName;
	}

// ---------------------------------------------------------
// CMediaObjectData::Pausable()
// ---------------------------------------------------------
//
TBool CMediaObjectData::Pausable() const
	{
	return iPausable;
	}

// ---------------------------------------------------------
// CMediaObjectData::IsUniqueFilenameSet()
// ---------------------------------------------------------
//
TBool CMediaObjectData::IsUniqueFilenameSet() const
	{
	return iUniqueFilenameSet;
	}

// ---------------------------------------------------------
// CMediaObjectData::CMediaObjectData()
// ---------------------------------------------------------
//
CMediaObjectData::CMediaObjectData()
    {
    }

// ---------------------------------------------------------
// CMediaObjectData::ConstructL()
// ---------------------------------------------------------
//
void CMediaObjectData::ConstructL()
    {
    iName = HBufC::NewL( 0 );
    iDescription = HBufC::NewL( 0 );
    iUrl = HBufC8::NewL( 0 );
    iSize = 0;
    iTypes = new (ELeave) CDesC8ArrayFlat( KTypeGranularity );
    iInstallNotify = HBufC8::NewL( 0 );
    iInfoUrl = HBufC8::NewL( 0 );
    iPrice = HBufC::NewL( 0 );
    iIcon = HBufC8::NewL( 0 );
    iSourceUri = HBufC8::NewL( 0 );
    
    iDownloadedSize = 0;
	iState = EQueued;
    iStatusCode = KHttp902UserCancelled;
    iResult = KErrGeneral;
    iFileName.SetLength( 0 );
    iPausable = EFalse;
    iUniqueFilenameSet = EFalse;
    iTempPath = KNullDesC;
    iRootPath = KNullDesC;
    }

// ---------------------------------------------------------
// CMediaObjectData::SetNameL()
// ---------------------------------------------------------
//
TBool CMediaObjectData::SetNameL( const TDesC& aName )
    {
    return SetStringAttrL( iName, aName, COD_NAME_MAX_LEN );
    }

// ---------------------------------------------------------
// CMediaObjectData::SetDescriptionL()
// ---------------------------------------------------------
//
TBool CMediaObjectData::SetDescriptionL( const TDesC& aDescription )
    {
    return SetStringAttrL
        ( iDescription, aDescription, COD_DESCRIPTION_MAX_LEN );
    }

// ---------------------------------------------------------
// CMediaObjectData::SetUrlL()
// ---------------------------------------------------------
//
TBool CMediaObjectData::SetUrlL( const TDesC& aUrl )
    {
    return SetStringAttrL( iUrl, aUrl, COD_URL_MAX_LEN );
    }

// ---------------------------------------------------------
// CMediaObjectData::SetSize()
// ---------------------------------------------------------
//
TBool CMediaObjectData::SetSize( TInt aSize )
    {
    if( aSize <= 0 )
        {
        return EFalse;
        }
    iSize = aSize;
    return ETrue;
    }

// ---------------------------------------------------------
// CMediaObjectData::SetDownloadedSize()
// ---------------------------------------------------------
//
TBool CMediaObjectData::SetDownloadedSize( TInt aSize )
    {
    if( aSize < 0 )
        {
        return EFalse;
        }
    iDownloadedSize = aSize;
    return ETrue;
    }
    
// ---------------------------------------------------------
// CMediaObjectData::SetState()
// ---------------------------------------------------------
//
void CMediaObjectData::SetState( TMediaObjectState aState )
    {
    iState = aState;
    }
        
// ---------------------------------------------------------
// CMediaObjectData::SetTypeL()
// ---------------------------------------------------------
//
TBool CMediaObjectData::SetTypeL( const TDesC& aType )
    {
    // Backwards-compatible deprecated method.
    iTypes->Reset();    // Replace all, support only one type.
    return AddTypeL( aType );
    }

// ---------------------------------------------------------
// CMediaObjectData::AddTypeL()
// ---------------------------------------------------------
//
TBool CMediaObjectData::AddTypeL( const TDesC& aType )
    {
    if ( aType.Length() > COD_TYPE_MAX_LEN || !aType.Length() )
        {
        return EFalse;
        }
    HBufC8* type8 = CodUtil::ConvertLC( aType );
    iTypes->AppendL( *type8 );
    CleanupStack::PopAndDestroy( type8 );
    return ETrue;
    }

// ---------------------------------------------------------
// CMediaObjectData::SetInstallNotifyL()
// ---------------------------------------------------------
//
TBool CMediaObjectData::SetInstallNotifyL( const TDesC& aInstallNotify )
    {
    return SetStringAttrL
        ( iInstallNotify, aInstallNotify, COD_INSTALL_NOTIFY_MAX_LEN );
    }

// ---------------------------------------------------------
// CMediaObjectData::SetInfoUrlL()
// ---------------------------------------------------------
//
TBool CMediaObjectData::SetInfoUrlL( const TDesC& aInfoUrl )
    {
    return SetStringAttrL( iInfoUrl, aInfoUrl, COD_INFO_URL_MAX_LEN );
    }

// ---------------------------------------------------------
// CMediaObjectData::SetPriceL()
// ---------------------------------------------------------
//
TBool CMediaObjectData::SetPriceL( const TDesC& aPrice )
    {
    return SetStringAttrL( iPrice, aPrice, COD_PRICE_MAX_LEN );
    }

// ---------------------------------------------------------
// CMediaObjectData::SetIconL()
// ---------------------------------------------------------
//
TBool CMediaObjectData::SetIconL( const TDesC& aIcon )
    {
    return SetStringAttrL( iIcon, aIcon, COD_ICON_MAX_LEN );
    }

// ---------------------------------------------------------
// CMediaObjectData::SetSourceUriL()
// ---------------------------------------------------------
//
void CMediaObjectData::SetSourceUriL( const TDesC8& aSourceUri )
    {
    HBufC8* buf = aSourceUri.AllocL();
    delete iSourceUri;
    iSourceUri = buf;
    }

// ---------------------------------------------------------
// CMediaObjectData::SetProgressiveDownload()
// ---------------------------------------------------------
//
void CMediaObjectData::SetProgressiveDownload( TBool aProgressiveDl )
    {
    iProgressiveDl = aProgressiveDl;
    }

// ---------------------------------------------------------
// CMediaObjectData::UniqueFilenameSet()
// ---------------------------------------------------------
//
void CMediaObjectData::UniqueFilenameSet( TBool aUniqueFilenameSet )
    {
    iUniqueFilenameSet = aUniqueFilenameSet;
    }

// ---------------------------------------------------------
// CMediaObjectData::SetOrderIsPost()
// ---------------------------------------------------------
//
void CMediaObjectData::SetOrderIsPost( TBool aIsPostOrder )
    {
    iIsPostOrder = aIsPostOrder;
    }

// ---------------------------------------------------------
// CMediaObjectData::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CMediaObjectData::SetStringAttrL
( HBufC*& aBuf, const TDesC& aString, TInt aMaxLength )
    {
    HBufC* buf;
    if ( aString.Length() > aMaxLength )
        {
        buf = aString.Mid(0,aMaxLength).AllocL();
        }
    else
        {
        buf = aString.AllocL();        
        }
  
    delete aBuf;
    aBuf = buf;
    return ETrue;
    }

// ---------------------------------------------------------
// CMediaObjectData::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CMediaObjectData::SetStringAttrL
( HBufC8*& aBuf, const TDesC& aString, TInt /*aMaxLength*/ )
    {
    HBufC8* buf = CodUtil::ConvertL( aString );
    delete aBuf;
    aBuf = buf;
    return ETrue;
    }

// ---------------------------------------------------------
// CMediaObjectData::StoreMediaInfoL()
// ---------------------------------------------------------
//
void CMediaObjectData::StoreMediaInfoL( TPtr8& aBuf, const CArrayPtrFlat<CHeaderField>* aHeaders ) const
	{
	AppendBufL( aBuf, iName );
	AppendBufL( aBuf, iUrl );
	APPEND_BUF_INT( aBuf, iSize );
	AppendBufL( aBuf, iPrice );
	APPEND_BUF_INT( aBuf, iProgressiveDl );
	AppendBufL( aBuf, iSourceUri );
	AppendBufL( aBuf, iInstallNotify );
	AppendContentTypesL( aBuf, iTypes );
	
	APPEND_BUF_INT( aBuf, iState );
    APPEND_BUF_INT( aBuf, iStatusCode );
    APPEND_BUF_INT( aBuf, iResult );
    APPEND_BUF_INT( aBuf, iPausable );    
    APPEND_BUF_INT( aBuf, iUniqueFilenameSet );

    AppendBufL( aBuf, iFullName );

    HBufC* buf1 = iTempPath.AllocLC();
	AppendBufL(aBuf,buf1);
	
	HBufC* buf2 = iRootPath.AllocLC();
	AppendBufL(aBuf,buf2);
	
	CleanupStack::PopAndDestroy(2,buf1);
		
	AppendHeadersL(aBuf, aHeaders );
	}

// ---------------------------------------------------------
// CMediaObjectData::AppendHeadersL()
// ---------------------------------------------------------
//
void CMediaObjectData::AppendHeadersL( TPtr8& aBuf,
                                    const CArrayPtrFlat<CHeaderField>* aHeaders ) const
    {
    
    if(!aHeaders)
        {
        TInt hdrCnt = 0;
        APPEND_BUF_INT( aBuf, hdrCnt );
        return;
        }
        
    TInt headers = aHeaders->Count();
    APPEND_BUF_INT( aBuf, headers );

    HBufC8* fieldName = NULL;
    HBufC8* fieldRawData = NULL;

    for( TInt i = 0; i < headers; ++i )
        {
        
        fieldName = (*aHeaders)[i]->FieldName();
        fieldRawData = (*aHeaders)[i]->FieldRawData();
        AppendBufL( aBuf, fieldName );
        AppendBufL( aBuf, fieldRawData );
        
        }
    return;    
    }     

// ---------------------------------------------------------
// CMediaObjectData::LoadHeadersL()
// ---------------------------------------------------------
//
void CMediaObjectData::LoadHeadersL( RFile& aFile, CArrayPtrFlat<CHeaderField>*& aHeaders )
	{

    if(!aHeaders)
        {
        return;
        }	

    TInt headers;
    READ_INT_L( aFile, headers );

    aHeaders->ResetAndDestroy();

    for( TInt i = 0; i < headers; ++i )
        {
        CHeaderField* field = CHeaderField::NewL();
        CleanupStack::PushL( field );

        field->LoadHeaderInfoL( aFile );
        aHeaders->AppendL( field );

        CleanupStack::Pop( field );
        }	
	}

// ---------------------------------------------------------
// CMediaObjectData::LoadStorageInfoL()
// ---------------------------------------------------------
//	
void CMediaObjectData::LoadMediaInfoL( RFile& aInFile, RFs &aFs, CArrayPtrFlat<CHeaderField>*& aHeaders )
	{
    ReadHBufCL( aInFile, iName );
    ReadHBufCL( aInFile, iUrl );
    READ_INT_L( aInFile, iSize );
    ReadHBufCL( aInFile, iPrice );
    READ_INT_L( aInFile, iProgressiveDl );
    ReadHBufCL( aInFile, iSourceUri );
    ReadHBufCL( aInFile, iInstallNotify );
    LoadContentTypesL( aInFile, iTypes );
    
    READ_INT_L( aInFile, iState );
    READ_INT_L( aInFile, iStatusCode );
    READ_INT_L( aInFile, iResult );
    READ_INT_L( aInFile, iPausable );  
    READ_INT_L( aInFile, iUniqueFilenameSet );
    
    ReadHBufCL( aInFile,iFullName );
    
    HBufC* buf1 = NULL;
    HBufC* buf2 = NULL;	
    
    ReadHBufCL(aInFile,buf1);
    if( buf1->Compare( KNullDesC ))
        {
        iTempPath = *buf1;    
        }    

    ReadHBufCL(aInFile,buf2);
    if( buf2->Compare( KNullDesC ))
        {
        iRootPath = *buf2;    
        }      
    delete buf1;
    delete buf2;
          	  
    LoadHeadersL( aInFile, aHeaders );
    SetDownloadedFileSizeL( aFs );
	}

// ---------------------------------------------------------
// CMediaObjectData::SetDownloadedFileSize()
// ---------------------------------------------------------
//	
void CMediaObjectData::SetDownloadedFileSizeL( RFs &aFs )
    {    
    if( !iFullName )
        {
        return;
        }
        
    RFile mediaFile;
    CleanupClosePushL<RFile>( mediaFile );    
    TInt err = mediaFile.Open(aFs, 
                           *iFullName, 
                           EFileShareReadersOnly | 
                           EFileRead ) ;	

	//KErrPathNotFound will occur when file is downloaded to MMC
	//And MMC is removed when next time you launch Browser.
                  //Other possible errors,  KErrNotFound, KErrBadName
	if( err )
	    {
	    CleanupStack::PopAndDestroy();
	    return;
	    }
		
    err = mediaFile.Size( iDownloadedSize ) ;
    User::LeaveIfError ( err );
    CleanupStack::PopAndDestroy(); // mediaFile
    }
    
// ---------------------------------------------------------
// CMediaObjectData::AppendContentTypes()
// ---------------------------------------------------------
//	
void CMediaObjectData::AppendContentTypesL( TPtr8& aBuf, CDesC8ArrayFlat* aTypes ) const
	{
	
	TInt count = aTypes->MdcaCount();
	APPEND_BUF_INT( aBuf,count );

	for( TInt i =0 ; i < count ; i++ )
		{
		HBufC8* buf = HBufC8::NewL( COD_TYPE_MAX_LEN );
    	
    	CleanupStack::PushL( buf );

		TPtr8 ptr= buf->Des();
		ptr = aTypes->MdcaPoint( i );
	 	AppendBufL( aBuf, buf );

		CleanupStack::PopAndDestroy(/*buf*/);
		}
	}

// ---------------------------------------------------------
// CMediaObjectData::LoadContentTypes()
// ---------------------------------------------------------
//	
void CMediaObjectData::LoadContentTypesL( RFile& aInFile, CDesC8ArrayFlat* aTypes )
	{
	TInt numTypes( 0 );
	READ_INT_L( aInFile, numTypes );
  aTypes->Reset();
	for( TInt i = 0 ; i < numTypes ; i++ )
	    {
    	HBufC8* buf = NULL;
    	ReadHBufCL( aInFile, buf );
    	CleanupStack::PushL( buf );
    	aTypes->AppendL( *buf );
    	CleanupStack::PopAndDestroy(/*buf*/);
	    }
	}


// ---------------------------------------------------------
// CMediaObjectData::ReArrangeTypesL()
// ---------------------------------------------------------
//	
void CMediaObjectData::ReArrangeTypesL( TInt aIndex )
    {
    const TDataType& type( iTypes->MdcaPoint( aIndex ) );
    iTypes->Delete(aIndex);
    iTypes->InsertL(0,type.Des8());	
    }