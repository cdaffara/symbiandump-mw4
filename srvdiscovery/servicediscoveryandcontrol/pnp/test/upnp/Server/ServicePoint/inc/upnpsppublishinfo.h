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
//

#ifndef __UPNPSPPUBLISHINFO_H_
#define __UPNPSPPUBLISHINFO_H_


class CUPnPPublishInfoElement : public CBase
	{
	private:
	RBuf8 		iSearchTarget;
	RBuf8		iUsn;
	TInt		iCacheControl;
	RBuf8		iServiceDescription;
	RBuf8		iUuid;
	RBuf8		iKey;
	TInt		iTimeOut;

	public:
	// Internal APIs
	inline static CUPnPPublishInfoElement* NewL( );
	// D'tor
	inline ~CUPnPPublishInfoElement ();
	
	// Get and set methods
	inline void SetSearchTargetL( const TDesC8& aSearchTarget );
	inline void SetUsnL( const TDesC8& aUsn );
	inline void SetCacheControlL( TInt aCacheControl );
	inline void SetServiceDescriptionL( const TDesC8& aServiceDescription );
	inline void SetUuidL( const TDesC8& aUuid );
	inline void SetKeyL( const TDesC8& aKey );
	inline void SetTimeOut( TInt aTimeOut );

	inline const TDesC8& SearchTarget( ) const ;
	inline const TDesC8& Usn( ) const ;
	inline TInt CacheControl( ) const ;
	inline const TDesC8& ServiceDescription( ) const ;
	inline const TDesC8& Uuid( ) const ;
	inline const TDesC8& Key( ) const ;
	inline TInt TimeOut( ) const;
	
	};

#include "upnpsppublishinfo.inl"
#endif /*UPNPSPPUBLISHINFO_H_*/
