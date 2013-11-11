/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This class is used to handle IKEv2 configuration payload attributes.
*
*/

#include "ikedebug.h"
#include "ikev2config.h"
#include "ikev2payloads.h"
#include "ikev2const.h"
#include "internaladdress.h"
#include "ipsecselectors.h"
#include "ikev2acquire.h"
#include "ikev2trafficselector.h"

CIkev2Config* CIkev2Config::NewL(CIkev2Acquire* aAcquire, TInetAddr* aRemoteIp)
{
	CIkev2Config* Config = new (ELeave)CIkev2Config();
	CleanupStack::PushL(Config);
	Config->ConstructL(aAcquire, aRemoteIp);
	CleanupStack::Pop(Config);		
	return Config;
}

CIkev2Config::~CIkev2Config() 
    {
    delete iCp; 
    }


TBool CIkev2Config::ProcessCpL(TCPPayloadIkev2* Cp)
{
    ASSERT(Cp);
   //
   // Process received CP payload according to message type     
   // If CP request is going and reply received parse attributes to get
   // Virtual IP and possible DNS addresses. If INTERNAL_ADDRESS_EXPIRY
   // present save it into object too. (used as IKE SA lifetime).
   // If CP request received sanity check attributes (CFG_REPLY has
   // already been built on constructor ConstructL().
   // All other messages (CFG_SET and CFG_ACK) are silently discarded
   //
    TBool Status = EFalse;
	
    if ( (Cp->GetCFGType() == CFG_REQUEST) || (Cp->GetCFGType() == CFG_REPLY) )
	{
		Status = ETrue;

		TInt AttrLth;
		TUint32   Ipv4Addr;
		TIp6Addr  Ipv6Addr;    //IPV6 raw address
		TDataAttributes* Attribute = Cp->Attributes();		
		TInt Lth = (TInt)(TPayloadIkev2::Cast(Cp)->GetLength()) - TCPPayloadIkev2::Size();
		
		while ( Status && Lth )
		{
			AttrLth = (TInt)Attribute->GetValue();
			switch ( Attribute->GetType() )
			{
				
				case INTERNAL_IP4_ADDRESS:
					if ( (AttrLth == 0) || (AttrLth == 4) )
					{
					   if (AttrLth == 4)
					   {
						  Ipv4Addr = GET32(Attribute->Data()); 
						  iVIP.iVPNIfAddr.SetAddress(Ipv4Addr);
					   }	   
					}	
					else Status = EFalse;	
					break;

				case INTERNAL_IP4_DNS:
					if ( (AttrLth == 0) || (AttrLth == 4) )
					{
						if (AttrLth == 4)
						{	
						   Ipv4Addr = GET32(Attribute->Data());
						   
						   //Only two DNS server addresses supported
						   if (iVIP.iVPNIfDNS1.IsUnspecified())
						       {
						       iVIP.iVPNIfDNS1.SetAddress(Ipv4Addr);
						       }
						   else if (iVIP.iVPNIfDNS2.IsUnspecified())
						       {
						       iVIP.iVPNIfDNS2.SetAddress(Ipv4Addr);
						       }
						}
					}	
					else Status = EFalse;	
					break;

				case INTERNAL_ADDRESS_EXPIRY:
					if ( (AttrLth == 0) || (AttrLth == 4) )
					{
						if ( AttrLth == 4 )
						{
						   iAddressExpiry = GET32(Attribute->Data());
						}	
					}	
					else Status = EFalse;	
					break;
					
				case INTERNAL_IP6_ADDRESS:
					if ( (AttrLth == 0) || (AttrLth == 16) )
					{
						if ( (AttrLth == 16))
						{
						   Mem::Copy(&Ipv6Addr.u.iAddr8, Attribute->Data(), sizeof(Ipv6Addr.u.iAddr8));
						   iVIP.iVPNIfAddr.SetAddress(Ipv6Addr);
						}	   
					}	
					else Status = EFalse;
					break;
					
				default:
					//
					// All other parameters are just ignored
					//
                    break;
					
			}

			AttrLth += Attribute->Size();
			if ( Lth < AttrLth )
			{
			   Status = EFalse;	
			   break;  // Error
			}	 
			else Lth -= AttrLth;
			
			Attribute = Attribute->Next();			
		
		}
		
		if ( !Status &&  Cp->GetCFGType() != CFG_REPLY)
		{
           delete iCp;
           iCp = NULL;
		}	
	}
	
	return Status;
}

void CIkev2Config::ConstructL(CIkev2Acquire* aAcquire, TInetAddr* aRemoteIp)
{
    ASSERT(aAcquire);
   //
   // Build either Config Payload Request or Reply depending on 
   // CIkev2Acquire object "role".
   // If CIkev2Acquire is a request (=not response) build CFG_REQUEST 
   // If CIkev2Acquire is a response) build CFG_REPLY and replace initiator
   // Traffic selector in CIkev2Acquire with Traffic selector containing
   // "dummy"  Virtual IP address built from CIkev2Acquire ID.
   //
	iCp = HBufC8::NewL(80);
	TDataAttributes* Attributes = reinterpret_cast<TDataAttributes*>(const_cast<TUint8*>(iCp->Ptr()));
	TInt AttrLth = 0;

    if ( !aAcquire->Response() )
	{
        iCpType = CFG_REQUEST;
		AttrLth += AddAttribute(Attributes, INTERNAL_IP4_ADDRESS, 0, NULL);
		Attributes = Attributes->Next();
		AttrLth += AddAttribute(Attributes, INTERNAL_IP4_DNS, 0, NULL);
	}
	else
	{
		//
		// Build "dummy" virtual IPv4 
		//
        iCpType = CFG_REPLY;
		TUint32 virtualIp = aAcquire->Id();
		if ( !aRemoteIp ||
		     ((aRemoteIp->Family() == KAfInet6) && !aRemoteIp->IsV4Mapped() && !aRemoteIp->IsV4Compat()))
		{
		   //
		   // "Dummy" virtual IPv4 is created from CIkev2Acquire object Id
		   // data. The address format is the following: 10.x.y.z where bit 1 is always 0
		   //
		   virtualIp  = aAcquire->Id();
		   virtualIp &= 0xfdfdfd;
		   virtualIp |= 0x0a000000;
		}
		else
		{
		   //
		   // "Dummy" virtual IPv4 is created from original peer
		   // address as follows:
		   // Original address: x.y.z.w ==> Virtual IP: 10.y.z.w
		   // If original address: 10.y.z.w ==> Virtual IP: 172.y.z.w
		   //
			virtualIp = aRemoteIp->Address();
			if ( (virtualIp & 0xff000000 ) != 0x0a000000)
				 virtualIp = (virtualIp & 0xffffff) | 0x0a000000;
			else virtualIp = (virtualIp & 0xffffff) | 0xac000000;			
		}	
		TInetAddr Ipv4Addr;
		Ipv4Addr.SetAddress(virtualIp);
		virtualIp = ByteOrder::Swap32(virtualIp);
		AttrLth += AddAttribute(Attributes, INTERNAL_IP4_ADDRESS, 4, (TUint8*)&virtualIp);

		//
		// Replace original Initiator Traffic selector with new based
		// on "virtualIp"
		//
		__ASSERT_DEBUG(aAcquire->TS_i().Count() > 0, User::Invariant());
		TUint8 Protocol = aAcquire->TS_i()[0].ProtocolId();
		
		CArrayFix<TIkeV2TrafficSelector>* newSelectors = 
                            new (ELeave) CArrayFixFlat<TIkeV2TrafficSelector>(1);
		CleanupStack::PushL(newSelectors);
		TIkeV2TrafficSelector selector(Ipv4Addr, Ipv4Addr, Protocol);
		newSelectors->AppendL(selector);
		aAcquire->ReplaceTS_i(newSelectors);
		CleanupStack::Pop(newSelectors);		
	}	
    iCp->Des().SetLength(AttrLth);	
}

TInt CIkev2Config::AddAttribute(TDataAttributes* aAttr, TUint8 aType, TInt aLth, TUint8* aData)
{
    ASSERT(aAttr);
    //
	// Set R-Bit zero by adding attribute as variable length attribute 
	//
	aAttr->SetVariable();
	aAttr->SetType(aType);	
	aAttr->SetValue((TUint16)aLth);	
	if ( aLth && aData )
		Mem::Copy(aAttr->Data(), aData, aLth);

	return (aLth + aAttr->Size());
}

TPtrC8 CIkev2Config::Cp() const 
    { 
    __ASSERT_DEBUG(iCp != NULL, User::Invariant());    
    return *iCp;
    }


TUint8 CIkev2Config::CpType()const
    {
    return iCpType;
    }

TUint32 CIkev2Config::ExpireTime() const 
    { 
    return iAddressExpiry;
    }


TVPNAddress CIkev2Config::VirtualIp()
    {
    TVPNAddress Vip = iVIP;
    iVIP = TVPNAddress(); 
    return Vip;
    }

