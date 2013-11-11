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
*      Implementation of class CCodData.   
*      
*
*/


// INCLUDE FILES

#include "CodData.h"
#include "CodDefs.h"
#include "CodUtil.h"
#include <badesca.h>
#include "FileExt.h"
#include "HttpDownloadData.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodData::NewL()
// ---------------------------------------------------------
//
EXPORT_C CCodData* CCodData::NewL()
    {
    CCodData* data = new (ELeave) CCodData();
    CleanupStack::PushL( data );
    data->ConstructL();
    CleanupStack::Pop( data );
    return data;
    }

// ---------------------------------------------------------
// CCodData::~CCodData()
// ---------------------------------------------------------
//
EXPORT_C CCodData::~CCodData()
    {
    delete iName;
    delete iVendor;
    delete iDescription;
    delete iInstallNotify;
    delete iNextUrl;
    delete iNextUrlAtError;
    delete iInfoUrl;
    delete iPrice;
    delete iIcon;
    delete iVersion;
	delete iUpdatedDDUri;
    delete iSourceUri;
    
    iMediaObjects.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CCodData::IsValid()
// ---------------------------------------------------------
//
EXPORT_C TBool CCodData::IsValid() const
    {
    TInt mediaObjectsValid = EFalse;
    
    //First check if all the media objects are valid
    //Also check total size is valid
    for( TInt i = 0; i < iMediaObjects.Count(); i++ )
        {
        mediaObjectsValid = iMediaObjects[i]->IsValid();
        if( !mediaObjectsValid ) break;
        }
    return ( iSize > 0 && mediaObjectsValid );
    }

// ---------------------------------------------------------
// CCodData::IsPostOrder()
// ---------------------------------------------------------
//
EXPORT_C TBool CCodData::IsPostOrder() const
    {
    return iIsPostOrder;
    }

// ---------------------------------------------------------
// CCodData::Reset()
// ---------------------------------------------------------
//
EXPORT_C void CCodData::Reset()
    {
    iName->Des().Zero();
    iVendor->Des().Zero();
    iDescription->Des().Zero();
    iSize = 0;
    iInstallNotify->Des().Zero();
    iNextUrl->Des().Zero();
    iNextUrlAtError->Des().Zero();
    iInfoUrl->Des().Zero();
    iPrice->Des().Zero();
    iIcon->Des().Zero();
    iVersion->Des().Zero();
	iIsPostOrder = EFalse;
	iUpdatedDDUri->Des().Zero();
	iSourceUri->Des().Zero();
	iActiveDownload = 1;
    
    //Also reset all media objects values
    for( TInt i = 0; i < iMediaObjects.Count(); i++ )
        {
        iMediaObjects[i]->Reset();
        }	
    }

// ---------------------------------------------------------
// CCodData::Name()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCodData::Name() const
    {
    return *iName;
    }

// ---------------------------------------------------------
// CCodData::Vendor()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCodData::Vendor() const
    {
    return *iVendor;
    }

// ---------------------------------------------------------
// CCodData::Description()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCodData::Description() const
    {
    return *iDescription;
    }

// ---------------------------------------------------------
// CCodData::Size()
// ---------------------------------------------------------
//
EXPORT_C TInt CCodData::Size() const
    {
    return iSize;
    }


// ---------------------------------------------------------
// CCodData::SuppressConfirm() 
// ---------------------------------------------------------
//
EXPORT_C TInt CCodData::SuppressConfirm() const
    {
    return iSuppressConfirm;
    }
        
// ---------------------------------------------------------
// CCodData::InstallNotify()
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CCodData::InstallNotify() const
    {
    return *iInstallNotify;
    }

// ---------------------------------------------------------
// CCodData::NextUrl()
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CCodData::NextUrl() const
    {
    return *iNextUrl;
    }

// ---------------------------------------------------------
// CCodData::NextUrlAtError()
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CCodData::NextUrlAtError() const
    {
    return *iNextUrlAtError;
    }

// ---------------------------------------------------------
// CCodData::InfoUrl
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CCodData::InfoUrl() const
    {
    return *iInfoUrl;
    }

// ---------------------------------------------------------
// CCodData::Price()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCodData::Price() const
    {
    return *iPrice;
    }

// ---------------------------------------------------------
// CCodData::Icon()
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CCodData::Icon() const
    {
    return *iIcon;
    }

// ---------------------------------------------------------
// CCodData::Version()
// ---------------------------------------------------------
//
EXPORT_C TPtrC CCodData::Version() const
    {
    return *iVersion;
    }

// ---------------------------------------------------------
// CCodData::SourceUri()
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CCodData::SourceUri() const
    {
    return *iSourceUri;
    }


// ---------------------------------------------------------
// CCodData::UpdatedDDURI()
// ---------------------------------------------------------
//
EXPORT_C TPtrC8 CCodData::UpdatedDDUriL() const
    {
    return *iUpdatedDDUri;
    }


// ---------------------------------------------------------
// CCodData::CCodData()
// ---------------------------------------------------------
//
CCodData::CCodData()
    {
    }

// ---------------------------------------------------------
// CCodData::ConstructL()
// ---------------------------------------------------------
//
void CCodData::ConstructL()
    {
    iName = HBufC::NewL( 0 );
    iVendor = HBufC::NewL( 0 );
    iDescription = HBufC::NewL( 0 );
    iSize = 0;
    iInstallNotify = HBufC8::NewL( 0 );
    iNextUrl = HBufC8::NewL( 0 );
    iNextUrlAtError = HBufC8::NewL( 0 );
    iInfoUrl = HBufC8::NewL( 0 );
    iPrice = HBufC::NewL( 0 );
    iIcon = HBufC8::NewL( 0 );
    iVersion = HBufC::NewL( 0 );
    iSuppressConfirm = 0; 
	iUpdatedDDUri = HBufC8::NewL( 0 );
	iSourceUri = HBufC8::NewL( 0 );
	iActiveDownload = 1;    
    }

// ---------------------------------------------------------
// CCodData::SetNameL()
// ---------------------------------------------------------
//
TBool CCodData::SetNameL( const TDesC& aName )
    {
    return SetStringAttrL( iName, aName, COD_NAME_MAX_LEN );
    }

// ---------------------------------------------------------
// CCodData::SetVendorL()
// ---------------------------------------------------------
//
TBool CCodData::SetVendorL( const TDesC& aVendor )
    {
    return SetStringAttrL( iVendor, aVendor, COD_VENDOR_MAX_LEN );
    }

// ---------------------------------------------------------
// CCodData::SetDescriptionL()
// ---------------------------------------------------------
//
TBool CCodData::SetDescriptionL( const TDesC& aDescription )
    {
    return SetStringAttrL
        ( iDescription, aDescription, COD_DESCRIPTION_MAX_LEN );
    }

// ---------------------------------------------------------
// CCodData::SetSize()
// ---------------------------------------------------------
//
TBool CCodData::SetSize( TUint aSize )
    {
    if( aSize <= 0 )
        {
        return EFalse;
        }
    iSize = aSize;
    return ETrue;
    }

// ---------------------------------------------------------
// CCodData::SetSuppressConfirm()
// ---------------------------------------------------------
//
TBool CCodData::SetSuppressConfirm( TInt aSuppressConfirm )
    {
    if( aSuppressConfirm < ENever || aSuppressConfirm > EAlways )
        {
        return EFalse;
        }
        
    iSuppressConfirm  = aSuppressConfirm;
    return ETrue;
    }

// ---------------------------------------------------------
// CCodData::SetInstallNotifyL()
// ---------------------------------------------------------
//
TBool CCodData::SetInstallNotifyL( const TDesC& aInstallNotify )
    {
    return SetStringAttrL
        ( iInstallNotify, aInstallNotify, COD_INSTALL_NOTIFY_MAX_LEN );
    }

// ---------------------------------------------------------
// CCodData::SetNextUrlL()
// ---------------------------------------------------------
//
TBool CCodData::SetNextUrlL( const TDesC& aNextUrl )
    {
    return SetStringAttrL( iNextUrl, aNextUrl, COD_NEXT_URL_MAX_LEN );
    }

// ---------------------------------------------------------
// CCodData::SetNextUrlAtErrorL()
// ---------------------------------------------------------
//
TBool CCodData::SetNextUrlAtErrorL( const TDesC& aNextUrlAtError )
    {
    return SetStringAttrL
        ( iNextUrlAtError, aNextUrlAtError, COD_NEXT_URL_AT_ERROR_MAX_LEN );
    }

// ---------------------------------------------------------
// CCodData::SetInfoUrlL()
// ---------------------------------------------------------
//
TBool CCodData::SetInfoUrlL( const TDesC& aInfoUrl )
    {
    return SetStringAttrL( iInfoUrl, aInfoUrl, COD_INFO_URL_MAX_LEN );
    }

// ---------------------------------------------------------
// CCodData::SetPriceL()
// ---------------------------------------------------------
//
TBool CCodData::SetPriceL( const TDesC& aPrice )
    {
    return SetStringAttrL( iPrice, aPrice, COD_PRICE_MAX_LEN );
    }

// ---------------------------------------------------------
// CCodData::SetIconL()
// ---------------------------------------------------------
//
TBool CCodData::SetIconL( const TDesC& aIcon )
    {
    return SetStringAttrL( iIcon, aIcon, COD_ICON_MAX_LEN );
    }

// ---------------------------------------------------------
// CCodData::SetVersionL()
// ---------------------------------------------------------
//
TBool CCodData::SetVersionL( const TDesC& aVersion )
    {
    return SetStringAttrL( iVersion, aVersion, COD_VERSION_MAX_LEN );
    }

// ---------------------------------------------------------
// CCodData::SetSourceUriL()
// ---------------------------------------------------------
//
void CCodData::SetSourceUriL( const TDesC8& aSourceUri )
    {
    HBufC8* buf = aSourceUri.AllocL();
    delete iSourceUri;
    iSourceUri = buf;
    }

// ---------------------------------------------------------
// CCodData::SetUpdatedDDURI()
// ---------------------------------------------------------
//
TBool CCodData::SetUpdatedDDURI( const TDesC& aUrl)
    {
    return SetStringAttrL( iUpdatedDDUri, aUrl, COD_URL_MAX_LEN );
    }    
    
// ---------------------------------------------------------
// CCodData::SetActiveDownload()
// ---------------------------------------------------------
//
void CCodData::SetActiveDownload( const TInt aMediaObjectId )
    {
    iActiveDownload = aMediaObjectId;
    }    

// ---------------------------------------------------------
// CCodData::SetOrderIsPost()
// ---------------------------------------------------------
//
void CCodData::SetOrderIsPost( TBool aIsPostOrder )
    {
    iIsPostOrder = aIsPostOrder;
    }

// ---------------------------------------------------------
// CCodData::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CCodData::SetStringAttrL
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
// CCodData::SetStringAttrL()
// ---------------------------------------------------------
//
TBool CCodData::SetStringAttrL
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
// CCodData::AppendStorageInfoL()
// ---------------------------------------------------------
//
void CCodData::AppendStorageInfoL(TPtr8& aBuf)const
	{
	TInt mediaCount = iMediaObjects.Count();
	AppendBufL( aBuf,iName );
	AppendBufL( aBuf,iVendor );
	AppendBufL( aBuf,iNextUrl );
	APPEND_BUF_INT( aBuf,iSize );
	AppendBufL( aBuf,iPrice );
	AppendBufL( aBuf,iSourceUri );
	AppendBufL(aBuf,iInstallNotify);
	AppendBufL( aBuf,iUpdatedDDUri );	
	APPEND_BUF_INT( aBuf, mediaCount );	
	}

// ---------------------------------------------------------
// CCodData::LoadStorageInfoL()
// ---------------------------------------------------------
//
void CCodData::LoadStorageInfoL(RFile& aInFile)
	{
	TInt mediaCount(0);
    ReadHBufCL( aInFile,iName );
    ReadHBufCL( aInFile,iVendor );
    ReadHBufCL( aInFile,iNextUrl );
    READ_INT_L( aInFile,iSize );
    ReadHBufCL( aInFile,iPrice );
	ReadHBufCL( aInFile,iSourceUri );
	ReadHBufCL(aInFile,iInstallNotify);
	ReadHBufCL( aInFile,iUpdatedDDUri );
	READ_INT_L( aInFile, mediaCount );
	
	for( TInt i = 0; i < mediaCount; ++i )
	    {
        CMediaObjectData *mediaObject = CMediaObjectData::NewL();
    	AppendMediaObjectL( mediaObject );
	    }
	}
	
// ---------------------------------------------------------
// CCodData::AppendMediaObjectL()
// ---------------------------------------------------------
//

TInt CCodData::AppendMediaObjectL( CMediaObjectData *aMO )
    {
    //Check if the same media object has already been added to the array.
    //Return index if index is present
    TInt i;

    for( i = 0; i < iMediaObjects.Count(); ++i )
        {
        if( iMediaObjects[i] == aMO )
            {
            return i;
            }
        }

    iMediaObjects.AppendL( aMO );
    iSize += aMO->Size();
    // return item index
    return iMediaObjects.Count();    
    }
    
// ---------------------------------------------------------
// CCodData::operator[]
// ---------------------------------------------------------
//

EXPORT_C CMediaObjectData *CCodData::operator[]( TInt aIndex )
    {
    //Check if the media object exits at specified index. Return the same if it is.
    if( aIndex > 0 && aIndex <= iMediaObjects.Count() )
        {
        return iMediaObjects[aIndex-1];
        }
    return NULL;    
    }

// ---------------------------------------------------------
// CCodData::operator[]
// ---------------------------------------------------------
//

EXPORT_C CMediaObjectData *CCodData::operator[]( TInt aIndex ) const
    {
    //Check if the media object exits at specified index. Return the same if it is.
    if( aIndex > 0 && aIndex <= iMediaObjects.Count() )
        {
        return iMediaObjects[aIndex-1];
        }
    return NULL;    
    }
    
// ---------------------------------------------------------
// CCodData::ActiveDownload
// ---------------------------------------------------------
//        
EXPORT_C TInt CCodData::ActiveDownload() const
	{
	return iActiveDownload;    
	}
