// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// upnppublishinfo.inl
// 
//


CUPnPPublishInfoElement* CUPnPPublishInfoElement::NewL( )
	{
	return  new (ELeave) CUPnPPublishInfoElement();
	}

CUPnPPublishInfoElement::~CUPnPPublishInfoElement ()
	{
	iSearchTarget.Close();
	iUsn.Close();
	iServiceDescription.Close();
	iUuid.Close();
	iKey.Close();
	}


void CUPnPPublishInfoElement::SetSearchTargetL( const TDesC8& aSearchTarget )
	{
	iSearchTarget.Close ();
	iSearchTarget.CreateL ( aSearchTarget );
	}

void CUPnPPublishInfoElement::SetUsnL( const TDesC8& aUsn )
	{
	iUsn.Close ();
	iUsn.CreateL ( aUsn );
	}

void CUPnPPublishInfoElement::SetCacheControlL( TInt aCacheControl )
	{
	iCacheControl = aCacheControl;
	}

void CUPnPPublishInfoElement::SetServiceDescriptionL( const TDesC8& aServiceDescription )
	{
	iServiceDescription.Close ();
	iServiceDescription.CreateL ( aServiceDescription );
	}

void CUPnPPublishInfoElement::SetUuidL( const TDesC8& aUuid )
	{
	iUuid.Close ();
	iUuid.CreateL ( aUuid );
	}

void CUPnPPublishInfoElement::SetKeyL( const TDesC8& aKey )
	{
	iKey.Close ();
	iKey.CreateL ( aKey );
	}


const TDesC8& CUPnPPublishInfoElement::SearchTarget( ) const 
	{
	return iSearchTarget;
	}

const TDesC8& CUPnPPublishInfoElement::Usn( ) const 
	{
	return iUsn;
	}

TInt CUPnPPublishInfoElement::CacheControl( ) const 
	{
	return iCacheControl;
	}

const TDesC8& CUPnPPublishInfoElement::ServiceDescription( ) const 
	{
	return iServiceDescription;
	}

const TDesC8& CUPnPPublishInfoElement::Uuid( ) const 
	{
	return iUuid;
	}

const TDesC8& CUPnPPublishInfoElement::Key( ) const 
	{
	return iKey;
	}

void CUPnPPublishInfoElement::SetTimeOut( TInt aTimeOut )
	{
	iTimeOut = aTimeOut;
	}

TInt CUPnPPublishInfoElement::TimeOut( ) const
	{
	return iTimeOut;
	}


