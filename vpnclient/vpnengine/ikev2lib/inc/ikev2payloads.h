/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Payload classes.
*
*/
#ifndef _IKEV2PAYLOADS_H_
#define _IKEV2PAYLOADS_H_
#include "ikev2const.h"
#include "ikemsgheader.h"

//
// GENERIC PAYLOAD HEADER
//
//                     1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ! Next Payload  !C!  RESERVED   !         Payload Length        !
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TPayloadIkev2
{
	public:
		static inline TInt Size() { return sizeof(TPayloadIkev2); }
		static inline TPayloadIkev2* Cast(const TAny* aPayload) { return (TPayloadIkev2*)aPayload; }		
		inline void Init() { PUT32(&u.iData8[0], 0);}   
		inline TUint8 GetNextPayload() { return u.iData8[0]; }
		inline void SetNextPayload(TUint8 aPayload) { u.iData8[0] = aPayload; }
		inline TUint16 GetLength() const { return (TUint16)GET16(&u.iData8[2]); }
		inline void SetLength(TUint16 aLength) { PUT16(&u.iData8[2], aLength); }
		inline TBool GetCritical() const { return ((u.iData8[1] & IKEV2_PL_CRITICAL) == IKEV2_PL_CRITICAL); }		
		inline void SetReserved() { u.iData8[1] &= IKEV2_PL_CRITICAL; }
		inline void SetCritical() { u.iData8[1] |= IKEV2_PL_CRITICAL; }
		inline TUint8* PayloadPtr() { return (TUint8 *)((char *)this); } 
		inline TUint8* PayloadData() const   
				{ return (TUint8 *)((char *)this + sizeof(*this)); } //returns a * to the specific data of this payload
		inline const TUint PlDataLen() const  
				{ if ( GetLength() > sizeof(*this) )
			           return (GetLength() - sizeof(*this));
		          else return 0;
				} //returns a * to the specific data of this payload
		inline TPayloadIkev2 *Next() const
				{ return (TPayloadIkev2 *)((char *)this + GetLength()); }
		//
		// The following methods are used to manage "ENCRYPTED" bit (0)
		// in payload reserved field. This bit is set to 1 when a
		// payload has been received within a encrypted payload.
		// Encrypted bit is NEVER set into transmitted payload.
		//
		inline TBool Encrypted() const { return ((u.iData8[1] & IKEV2_PL_ENCRYPTED) == IKEV2_PL_ENCRYPTED); }		
		inline void SetEncrypted() { u.iData8[1] |= IKEV2_PL_ENCRYPTED; }
		inline void ResetEncrypted() { u.iData8[1] &= ~IKEV2_PL_ENCRYPTED; }		
		
	private:
		union
		{
			TUint32 iData32[1];
			TUint16 iData16[2];
			TUint8  iData8[4];
		} u;
};

#define TSAPayload TPayloadIkev2
#define TNoncePayload TPayloadIkev2
#define TVendorPlIkev2 TPayloadIkev2
#define TEAPPayloadIkev2 TPayloadIkev2
//
// Transform Attributes
//
// In this phase there is only one attribute defined for IKEv2 which is
// encryption key length for transform type Encryption Algorithm
//
//                       1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !A!       Attribute Type        !    AF=0  Attribute Length     !
//	 !F!                             !    AF=1  Attribute Value      !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                   AF=0  Attribute Value                       !
//   !                   AF=1  Not Transmitted                       !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TDataAttributes
{
	public:
		inline TInt Size() const { return sizeof(*this); }		
		inline TBool IsBasic() const {return ((u.iData8[0] & 0x80) != 0);} //return if basic attrib or variable
		inline void SetBasic() { u.iData8[0] |= 0x80;}
		inline void SetVariable() { u.iData8[0] &= 0x7f;}		
		inline TUint16 GetType() { return TUint16(GET16(&u.iData16[0]) & 0x7fff);}
		inline void SetType(TUint16 aType) { PUT16(&u.iData16[0], aType);}				
		inline TUint16 GetValue() { return TUint16(GET16(&u.iData16[1]));}
		inline void SetValue(TUint16 aValue) { PUT16(&u.iData16[1], aValue); }
		inline TUint8* Data() {return (TUint8*)((TUint8*)this + Size());}
		inline TDataAttributes* Next()
		{
			if ( IsBasic() )
			     return (TDataAttributes*)((TUint8*)this + Size());
		    else return (TDataAttributes*)((TUint8*)this + (Size() + (TInt)GetValue()));
        }

	private:
		union
		{
			TUint32 iData32[1];
			TUint16 iData16[2];
			TUint8  iData8[4];
		} u;
};

//
// Transform Payload
//                       1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! 0 (last) or 3 !   RESERVED    !        Transform Length       !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !Transform Type !   RESERVED    !          Transform ID         !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                                                               !
//   ~                      Transform Attributes                     ~
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TTransformIkev2 
{
	public:
		static inline TTransformIkev2* Cast(const TAny* aPayload)	{ return (TTransformIkev2*)aPayload; }
		inline void Init() { PUT32(&u.iData8[0], 0);}   		
		inline TInt Size() const { return sizeof(*this); }
		inline TUint8 GetType() const { return u.iData8[4]; };
		inline void SetType(TUint8 aPayload) { u.iData8[4] = aPayload; };
		inline TUint16 GetID() const { return (TUint16)GET16(&u.iData16[3]); };
		inline void SetID(TUint16 aId) { PUT16(&u.iData16[3], aId); };
		inline void SetReserved() { u.iData8[5] = 0; };
		inline TDataAttributes* Attributes() const {return (TDataAttributes*)((TUint8*)this + Size()); } 
		//
		// The following methods are used to manage "SELECTED" bit (0)
		// in teh Transform payload reserved field. This bit is set to 1 when
		// the transform payload has been selected into acceptable
		// proposal.
		// "SELECTED" is NEVER set into transmitted payload.
		//
		inline TBool IsSelected() { return ((u.iData8[5] & IKEV2_PL_SELECTED) == IKEV2_PL_SELECTED);}		
		inline void Selected()  { u.iData8[5] |= IKEV2_PL_SELECTED; }
		inline void NotSelected()  { u.iData8[5] &= ~IKEV2_PL_SELECTED; }		

	private:
		union
		{
			TUint32 iData32[2];
			TUint16 iData16[4];
			TUint8  iData8[8];
		} u;

};

//
// Proposal Payload
//                       1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! 0 (last) or 2 !   RESERVED    !         Proposal Length       !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Proposal #    !  Protocol ID  !    SPI Size   !# of Transforms!
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ~                        SPI (variable)                         ~
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                                                               !
//   ~                        <Transforms>                           ~
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TProposalIkev2
{
	public:
		static inline TProposalIkev2 *Cast(const TAny* aPayload) { return (TProposalIkev2 *)aPayload;}
		inline TInt Size() const { return sizeof(*this); }
		inline TUint8 GetNum() const { return u.iData8[4]; }
		inline void SetNum(TUint8 aNumber) { u.iData8[4] = aNumber; }
		inline TUint8 GetProtocol() const { return u.iData8[5]; }
		inline void SetProtocol(TUint8 aProtocol) { u.iData8[5] = aProtocol; }
		inline TUint8 GetSPISize() const { return u.iData8[6]; }
		inline void SetSPISize(TUint8 aSpiSize) { u.iData8[6] = aSpiSize; } 
		inline TUint8 GetNumTrans() const { return u.iData8[7]; };
		inline void SetNumTrans(TUint8 aNumber) { u.iData8[7] = aNumber; } 
		inline TTransformIkev2* TransformPl() const    //next payload (transform)
				{ return (TTransformIkev2*)((TUint8*)this + sizeof(*this) + GetSPISize()); } 
		inline TUint8 *SPI() {return (TUint8 *)((TUint8*)this + sizeof(*this));} //*to the SPI
		inline void GetIpsecSPI(TUint32* aSPI) const {Mem::Copy( (TUint8*)aSPI, ((TUint8*)this + sizeof(*this)), 4); }		
		inline void SetIpsecSPI(TUint32 aSPI) { Mem::Copy(((TUint8*)this + sizeof(*this)), (TUint8*)&aSPI, 4); }
		inline TInt PropHdrLth() const { return (Size() + (TInt)GetSPISize()); }
		inline TBool Last() const { return u.iData8[0] == 0;}
		
    private:
		union
		{
			TUint32 iData32[2];
			TUint16 iData16[4];
			TUint8  iData8[8];
		} u;

};

//
// Key Exchange Payload
//                       1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Next Payload  !C!  RESERVED   !         Payload Length        !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !          DH Group #           !           RESERVED            !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                                                               !
//   ~                       Key Exchange Data                       ~
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TKEPayloadIkev2 
{
	public:
		static inline TKEPayloadIkev2 *Cast(const TAny* aPayload) { return (TKEPayloadIkev2 *)aPayload;}		
		static inline TInt Size() { return sizeof(TKEPayloadIkev2); }
		inline TUint16 GetDHGroup() const { return (TUint16)GET16(&u.iData16[2]); };
		inline void SetDHGroup(TUint16 aGroup) { PUT16(&u.iData16[2], aGroup); };
		inline void SetReserved() { PUT16(&u.iData16[3], 0); };		
		inline TUint8 *DHPublic() {return (TUint8 *)((TUint8*)this + sizeof(*this));}
		inline TUint8 GetNextPayload() { return u.iData8[0]; }

	private:
		union
		{
			TUint32 iData32[2];
			TUint16 iData16[4];
			TUint8  iData8[8];
		} u;
		
};

//
// Identification Payload
//                       1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Next Payload  !C!  RESERVED   !         Payload Length        !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !   ID Type     !                 RESERVED                      |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                                                               !
//   ~                   Identification Data                         ~
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TIDPayloadIkev2 
{
	public:
		static inline TIDPayloadIkev2 *Cast(const TAny* aPayload) { return (TIDPayloadIkev2 *)aPayload;}		
		static inline TInt Size() { return sizeof(TIDPayloadIkev2); }
		inline TUint8 GetIdType() { return u.iData8[4]; };
		inline void SetIdType(TUint8 aIdType) { u.iData8[4] = aIdType; };
		inline void SetReserved() { PUT16(&u.iData16[3], 0); u.iData8[5] = 0; };		
		inline TUint8 *IdData() {return (TUint8*)((TUint8*)this + sizeof(*this));}
		inline TUint8 GetNextPayload() { return u.iData8[0]; }

	private:
		union
		{
			TUint32 iData32[2];
			TUint16 iData16[4];
			TUint8  iData8[8];
		} u;

};

//
// Authentication Payload
//                       1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Next Payload  !C!  RESERVED   !         Payload Length        !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Auth Method   !                RESERVED                       !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                                                               !
//   ~                      Authentication Data                      ~
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TAuthPayloadIkev2 
{
	public:
		static inline TAuthPayloadIkev2 *Cast(const TAny* aPayload) { return (TAuthPayloadIkev2 *)aPayload;}		
		static inline TInt Size() { return sizeof(TAuthPayloadIkev2); }
		inline TUint8 GetAuthMethod() { return u.iData8[4]; };
		inline void SetAuthMethod(TUint8 aMethod) { u.iData8[4] = aMethod; };
		inline void SetReserved() { PUT16(&u.iData16[3], 0); u.iData8[5] = 0; };		
		inline TUint8 *AuthData() {return (TUint8*)((TUint8*)this + sizeof(*this));}

	private:
		union
		{
			TUint32 iData32[2];
			TUint16 iData16[4];
			TUint8  iData8[8];
		} u;

};

//
//   Traffic Selector Payload
//                       1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Next Payload  !C!  RESERVED   !         Payload Length        !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Number of TSs !                 RESERVED                      !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                                                               !
//   ~                       <Traffic Selectors>                     ~
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TTSPayloadIkev2 
{
	   public:
		   static inline TTSPayloadIkev2 *Cast(const TAny* aPayload) { return (TTSPayloadIkev2 *)aPayload;}		
		   static inline TInt Size() { return sizeof(TTSPayloadIkev2); }
		   inline TUint8 GetNumberOfTs() { return u.iData8[4]; };
		   inline void SetNumberOfTs(TUint8 aNumber) { u.iData8[4] = aNumber; };
		   inline void SetReserved() { PUT16(&u.iData16[3], 0); u.iData8[5] = 0; };		
		   inline TUint8* TrafficSelectors() {return (TUint8*)((TUint8*)this + sizeof(*this));}
		   inline TUint8 GetNextPayload() { return u.iData8[0]; }

	   private:
		   union
		   {
			   TUint32 iData32[2];
			   TUint16 iData16[4];
			   TUint8  iData8[8];
		   } u;

};

//
//   Traffic Selector
//                       1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !   TS Type     !IP Protocol ID*|       Selector Length         |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |           Start Port*         |           End Port*           |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                                                               !
//   ~                         Starting Address*                     ~
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                                                               !
//   ~                         Ending Address*                       ~
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TTrafficSelector
{
	public:
		static inline TTrafficSelector* Cast(const TAny* aPayload) { return (TTrafficSelector *)aPayload;}				
		static inline TInt Size() { return sizeof(TTrafficSelector); }
		inline void Init() { PUT32(&u.iData8[0], 0); PUT32(&u.iData8[4], 0); };		
		inline void SetLength(TUint16 aLth) { PUT16(&u.iData8[2], aLth);  };
		inline void SetType(TUint8 aType) { u.iData8[0] = aType;  };
		inline void SetProtocol(TUint8 aProt) { u.iData8[1] = aProt;  };
		inline void SetStartPort(TUint16 aPort) { PUT16(&u.iData8[4], aPort); };
		inline void SetEndPort(TUint16 aPort) { PUT16(&u.iData8[6], aPort); };				
		inline TUint16 Length() const { return (TUint16)GET16(&u.iData8[2]);  };
		inline TUint16 StartPort() const { return (TUint16)GET16(&u.iData8[4]);  };
		inline TUint16 EndPort() const { return (TUint16)GET16(&u.iData8[6]);  };				
		inline TUint8  Protocol() const { return u.iData8[1];  };
		inline TUint8  Type() const { return u.iData8[0];  };						
		inline const TUint8 *Addresses() const {return (TUint8*)((TUint8*)this + sizeof(*this));}
	private:
		union
		{
			TUint32 iData32[2];
			TUint16 iData16[4];
			TUint8  iData8[8];
		} u;

};


//
//   Certificate Payload
//                       1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Next Payload  !C!  RESERVED   !         Payload Length        !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Cert Encoding !                                               !
//   +-+-+-+-+-+-+-+-+                                               !
//   ~                       Certificate Data                        ~
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TCertPayloadIkev2 
{
	public:
		static inline TCertPayloadIkev2 *Cast(const TAny* aPayload) { return (TCertPayloadIkev2 *)aPayload;}		
		static inline TInt Size() { return sizeof(TPayloadIkev2) + sizeof(char); }
		inline TUint8 GetEncoding() const { return u.iData8[4]; };
		inline void SetEncoding(TUint8 aEncoding) { u.iData8[4] = aEncoding; };
		inline TUint8 *Certificate() const {return (TUint8*)((TUint8*)this + Size());}
		inline TUint8 GetNextPayload() const { return u.iData8[0]; }

	private:
		union
		{
			TUint8  iData8[5];
		} u;
};
//
//   Certificate Request Payload
//		                 1                   2                   3
//	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Next Payload  !C!  RESERVED   !         Payload Length        !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Cert Encoding !                                               !
//   +-+-+-+-+-+-+-+-+                                               !
//	 ~                    Certification Authority                    ~
//	 !                                                               !
//	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TCReqPayloadIkev2 
{
	public:
		static inline TCReqPayloadIkev2 *Cast(const TAny* aPayload) { return (TCReqPayloadIkev2 *)aPayload;}		
		static inline TInt Size() { return sizeof(TPayloadIkev2) + sizeof(char); }
		inline TUint8 GetEncoding() const { return u.iData8[4]; };		
		inline void SetEncoding(TUint8 aEncoding) { u.iData8[4] = aEncoding; };		
		inline TUint8* Authority() const {return (TUint8*)((TUint8*)this + Size());}
		inline TUint8 GetNextPayload() { return u.iData8[0]; }

	private:
		union
		{
			TUint8  iData8[5];
		} u;
};

//
//   Notify Payload
//                       1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Next Payload  !C!  RESERVED   !         Payload Length        !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !  Protocol ID  !   SPI Size    !      Notify Message Type      !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                                                               !
//   ~                Security Parameter Index (SPI)                 ~
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                                                               !
//   ~                       Notification Data                       ~
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TNotifPayloadIkev2 
{
	public:
		static inline TNotifPayloadIkev2 *Cast(const TAny* aPayload) { return (TNotifPayloadIkev2 *)aPayload;}		
		static inline TInt Size() { return sizeof(TNotifPayloadIkev2); }
		inline TUint8 GetProtocolId() const { return u.iData8[4]; };
		inline void SetProtocolId(TUint8 aProtId) { u.iData8[4] = aProtId; };
		inline TUint8 GetSPISize() const { return u.iData8[5]; }
		inline void SetSPISize(TUint8 aSpiSize) { u.iData8[5] = aSpiSize; }
		inline TUint16 GetMsgType() const { return (TUint16)GET16(&u.iData16[3]); };
		inline void SetMsgType(TUint16 aType) { PUT16(&u.iData16[3], aType); };
		inline TUint8* SPI() {return (TUint8*)((TUint8*)this + sizeof(*this));} 
		inline TUint8* NotifData() const {return (TUint8*)((TUint8*)this + Size() + GetSPISize());}
		inline TUint NotifDataLength() const { return TPayloadIkev2::Cast(this)->GetLength() - Size() - GetSPISize(); }
	private:
		union
		{
			TUint32 iData32[2];
			TUint16 iData16[4];
			TUint8  iData8[8];
		} u;
		
};

//
//   Delete Payload
//                       1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Next Payload  !C!  RESERVED   !         Payload Length        !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Protocol ID   !   SPI Size    !           # of SPIs           !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                                                               !
//   ~               Security Parameter Index(es) (SPI)              ~
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TDeletePlIkev2 
{
	public:
		static inline TDeletePlIkev2 *Cast(const TAny* aPayload) { return (TDeletePlIkev2 *)aPayload;}		
		static inline TInt Size() { return sizeof(TDeletePlIkev2); }
		inline TUint8 GetProtocolId() const { return u.iData8[4]; };
		inline void SetProtocolId(TUint8 aProtId) { u.iData8[4] = aProtId; };
		inline TUint8 GetSPISize() const { return u.iData8[5]; }
		inline void SetSPISize(TUint8 aSpiSize) { u.iData8[5] = aSpiSize; }
		inline TUint16 GetNbrOfSpis() const { return (TUint16)GET16(&u.iData16[3]); };
		inline void SetNbrOfSpis(TUint16 aType) { PUT16(&u.iData16[3], aType); };
		inline const TUint8* SPIs() const {return (TUint8*)((TUint8*)this + sizeof(*this));} 
	private:
		union
		{
			TUint32 iData32[2];
			TUint16 iData16[4];
			TUint8  iData8[8];
		} u;

};

//
//   Configuration Payload
//                       1                   2                   3
//   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   ! Next Payload  !C! RESERVED    !         Payload Length        !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !   CFG Type    !                    RESERVED                   !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   !                                                               !
//   ~                   Configuration Attributes                    ~
//   !                                                               !
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
class TCPPayloadIkev2
{
	public:
		static inline TCPPayloadIkev2 *Cast(const TAny* aPayload) { return (TCPPayloadIkev2 *)aPayload;}		
		static inline TInt Size() { return sizeof(TCPPayloadIkev2); }
		inline TUint8 GetCFGType() { return u.iData8[4]; };
		inline void SetCFGType(TUint8 aCFGType) { u.iData8[4] = aCFGType; };
		inline void SetReserved() { PUT16(&u.iData16[3], 0); u.iData8[5] = 0; };				
		inline TDataAttributes* Attributes() {return (TDataAttributes*)((TUint8*)this + sizeof(*this));}
		
	private:
		union
		{
			TUint32 iData32[2];
			TUint16 iData16[4];
			TUint8  iData8[8];
		} u;

};

#endif
