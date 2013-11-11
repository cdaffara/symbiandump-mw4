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


#include "pnputils.h"
#include "cupnpservicediscoveryimpl.h"
#include <inetprottextutils.h>
#include <uriutils.h>

const TUint KHttpDefaultPort = 80;

/* Used to resolve the domain name into ipaddress
 @param aResolver   The RHostResolver reference used to resolve the domain name
 @param aUri		The url which has to be resolved into ip address
 @param aSockAddr	The resolved address which will get written to on success
 */
void CUPnPUtils::ResolveHostAddressL ( RHostResolver& aResolver,const TDesC8& aUri, TInetAddr& aSockAddr )
	{
	TUriParser8 parsedUri;
	parsedUri.Parse( aUri );
	_LIT8(KScheme,"http");
	const TDesC8& scheme = parsedUri.Extract( EUriScheme );
	if ( scheme.CompareF( KScheme ))
		User::Leave(KErrCorrupt);
	// Check if the character followed by the scheme eg. http:// is a numeral. If
	// so its already an ip address and hence no need to resolve
	//If the first character following http:// is a number then its already
	// an ipaddress and hence is returned as is		
	const TDesC8& hostname = parsedUri.Extract( EUriHost );
	if ( UriUtils::HostType( hostname ) == UriUtils::EIPv4Host )
		{
		RBuf hostBuf;
		hostBuf.CreateL ( hostname.Length ( ) );
				
		hostBuf.Copy ( hostname );
		aSockAddr.Input ( hostBuf );		
		hostBuf.Close ( );
		}
	else if ( UriUtils::HostType( hostname ) == UriUtils::ETextHost )
		{
		RBuf tempBuf;
		tempBuf.CreateL ( hostname.Length ( ) );		
		tempBuf.Copy ( hostname );
	
		TNameEntry dnsHostEntry;
		// Get the ip address corresponding to the hostname and write it to tempBuf
		aResolver.GetByName( tempBuf, dnsHostEntry );
		TInetAddr address = TInetAddr(dnsHostEntry().iAddr);
		aSockAddr.SetAddress ( address.Address ( ) );
		if ( address.Port () == 0 )
			{			
			aSockAddr.SetPort ( KHttpDefaultPort );
			}
		else
			{
			aSockAddr.SetPort ( address.Port () );
			}	
		
		tempBuf.Close ( );
		}
	else
		User::Leave(KErrCorrupt);
	
	if ( parsedUri.IsPresent ( EUriPort ) )
		{
		const TDesC8& portBuf = parsedUri.Extract( EUriPort );
		TInt port;
		InetProtTextUtils::ConvertDescriptorToInt ( portBuf, port );
		aSockAddr.SetPort ( port );
		}		
	}

/* Used to check whether uri conforms to given type. The uri types it checks are
  1. upnp:rootdevice
  2. urn:domain-name:service:serviceId
  3. urn:domain-name:device:deviceId
  4. uuid:
  
  @param aUri	The url whose validity must be checked
  @param aType	The type of uri to be checked against
  				If aType == EDevice Then aUri is of deviceType
  				else if aType == EService Then aUri is of serviceType
  				else if aType == EAny then aUri is generic
 */
TInt CUPnPUtils::GenericUriValidity ( const TDesC8& aUri , CUPnPUtils::TUriType aType)
	{
	_LIT8(KUrn,"urn");
	_LIT8(KUpnp,"upnp:rootdevice");
	_LIT8(KService,"service");
	_LIT8(KDevice,"device");
	_LIT8(KUuid,"uuid");
	if(aUri.Length()<=5)
		return KErrCorrupt;
	
	if((aType == CUPnPUtils::EDevice) || (aType == CUPnPUtils::EAny))
		{
		if ( aUri.Left(4) == KUuid && aUri[4] == ':')
			return KErrNone;
		// Test whether uri is "upnp:rootdevice"
		if (aUri.CompareF(KUpnp) == NULL)
			return KErrNone;
		}
	TLex8 uriParser(aUri);
	TChar ch = ':';
	uriParser.Mark();
	while((uriParser.Eos() == EFalse) && uriParser.Peek() != ch)
		uriParser.Inc();
	if(uriParser.MarkedToken().CompareF(KUrn) == NULL)
		{
		// Reach the first character after the first delimiter ":"
		uriParser.Inc();
		if( !uriParser.Eos() )
			uriParser.Inc();
		
		// Now move char pointer to the second delimiter
		while((uriParser.Eos() == EFalse) && uriParser.Peek() != ch)
			uriParser.Inc();
		if (uriParser.Eos())
			return KErrCorrupt;
		
		// Reach the the 2nd delimiter ":" and mark it. Now start searching
		// from the next character
		uriParser.Inc();
		uriParser.Mark();
		if( !uriParser.Eos() )
			uriParser.Inc();
		
		// Now move char pointer to the 3rd delimiter
		while((uriParser.Eos() == EFalse) && uriParser.Peek() != ch)
			uriParser.Inc();
		
		// Extract the token in between it should be either "service" or "device"
		const TDesC8& testUri = uriParser.MarkedToken();
		TBool flag = EFalse;
		if(aType == CUPnPUtils::EService && (testUri.CompareF(KService) == NULL))
			flag = ETrue;
		else if(aType == CUPnPUtils::EDevice && (testUri.CompareF(KDevice) == NULL))
			flag = ETrue;
		else if (aType == CUPnPUtils::EAny)
			{
			if ((testUri.CompareF(KDevice) == NULL) || (testUri.CompareF(KService)== NULL))
				flag = ETrue;
			}
		
		if (flag == EFalse )
			return KErrCorrupt;
		
		// Now check for the remaining part of the uri
		uriParser.Inc();
		uriParser.Mark();
		
		if( !uriParser.Eos() )
			uriParser.Inc();
			
		// Now move char pointer to the 4th delimiter
		while((uriParser.Eos() == EFalse) && uriParser.Peek() != ch)
			uriParser.Inc();
		if( uriParser.Eos() )
			return KErrCorrupt;
		// The service/device suffix should be of length <=64
		if((uriParser.MarkedToken()).Length() >64)
			return KErrCorrupt;
		// Now the first character outside the last colon should be a number
		uriParser.Inc();
		ch = uriParser.Peek();
		if( ch >= 48 && ch <= 57 )
			return KErrNone;
		else
			return KErrCorrupt;
	
		}
	else
		return KErrCorrupt;
		
	}
