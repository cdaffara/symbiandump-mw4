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
* Description: IKEv1 payload handling.
*
*/

#ifndef IKEV1PAYLOADS_H
#define IKEV1PAYLOADS_H

#include <e32math.h>
#include "ikemsgheader.h"
#include "ikev1isakmpct.h"

//
// GENERIC PAYLOAD HEADER
//
class TPayloadISAKMP
    {
public:
    TPayloadISAKMP() {u.iData32[0] = 0; }
    inline TUint8 GetPayload() const;
    inline void SetPayload(TUint8 aPayload);
    inline TUint16 GetLength() const;
    inline void SetLength(TUint16 aLength);
    inline TUint8 GetReserved() const;
    inline void SetReserved(TUint8 aVal);
    inline const TUint8 *SpecificData() const   
        { return (TUint8 *)((char *)this + sizeof(*this)); } //returns a * to the specific data of this payload
    inline const TUint SpecificDataLen() const  
        { return (GetLength() - sizeof(*this)); } //returns a * to the specific data of this payload
    inline const TPayloadISAKMP *Next() const
        { return (TPayloadISAKMP *)((char *)this + GetLength()); } 

private:
    union
        {
        TUint32 iData32[1];
        TUint16 iData16[2];
        TUint8  iData8[4];
        } u;
    };
inline TUint8 TPayloadISAKMP::GetPayload() const { return u.iData8[0]; }
inline void TPayloadISAKMP::SetPayload(TUint8 aPayload) { u.iData8[0] = aPayload; }
inline TUint16 TPayloadISAKMP::GetLength() const { return (TUint16)GET16(&u.iData8[2]); }
inline void TPayloadISAKMP::SetLength(TUint16 aLength) { PUT16(&u.iData8[2], aLength); }
inline TUint8 TPayloadISAKMP::GetReserved() const { return u.iData8[1]; }
inline void TPayloadISAKMP::SetReserved(TUint8 aVal) { u.iData8[1] = aVal; }

//
// SA PAYLOAD
//


class TProposalISAKMP;

//Plus Variable field (Labeled Domain Identifier) from IPSEC DOI (included in length if present)
//use TSAPayloadISAKMP to decodify its values;

class TSAPayloadISAKMP
{
	public: 
		inline TSAPayloadISAKMP() {iReserved=0;}

	public:
		inline TUint16 GetLength() const { return (TUint16)GET16(&iLength); }
		inline void SetLength(TUint16 aLength) { PUT16(iLength, aLength); }
		inline TUint16 GetReserved() const { return iReserved; }    //No need to swap because always 0
		inline const TSAPayloadISAKMP *Next() const //next payload
				{ return (TSAPayloadISAKMP *)((TUint8 *)this + GetLength()); } 
		inline const TUint32 *Data() const
				{ return (TUint32 *)(this + sizeof(*this)); } //returns the address of the data field

	private:
		TUint16 iLength;
		TUint16 iReserved;
};
//followed by a variable size field of the length indicated

class TSAISAKMP : public TPayloadISAKMP
{
public:
    inline TUint32 Size() const { return sizeof(TSAISAKMP); }   //sizeof OK because aligned
    inline TUint32 GetDOI() const { return GET32(&iDOI); }
    inline void SetDOI(TUint32 aDOI) { PUT32(&iDOI, aDOI); }
    inline TUint32 GetSIT() const { return GET32(&iSIT); }
    inline void SetSIT(TUint32 aSIT) { PUT32(&iSIT, aSIT); }
    inline TUint GetSITLength() const
	{
		if ( GetDOI() == IPSEC_DOI)    //IPSEC DOI has SPI of size 4 octets
			return 4;
		return 0;
	}
    inline TBool HasLDId() const
	{
		TUint32 sit=(IPSEC_SIT_SECRECY | IPSEC_SIT_INTEGRITY);
		if ((GetDOI()==IPSEC_DOI) && (GetSIT() & sit))
			return ETrue;   //DOI and (SECRECY or INTEGRITY) supported

		return EFalse;
	}
    static inline TSAISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TSAISAKMP *)aPayload; }
    inline TUint32 *Data() const
        { return (TUint32 *)(this + sizeof(*this)); } //returns the address of the data field following the header
    TPayloadISAKMP* Payload() const
	{
		TUint size= sizeof(*this);  //fixed size
		if (!HasLDId())
			return (TPayloadISAKMP *) ((TUint8*)this + size);
		size += 4;  //Labeled Domain Identifier payload size
		TUint32 *d=Data();
		TSAPayloadISAKMP *p = (TSAPayloadISAKMP *)++d;
		TInt32 res;

		for (TInt i=0; i < 4; i++)
		{
			if (i % 2==0)   //Fields 1 and 3 in bytes
				Math::Int(res, p->GetLength() / 4);
			else    //Fields 2, 4 in bits
				Math::Int(res, p->GetLength() / 32);

			size += res+1;
		}

		return (TPayloadISAKMP *) ((TUint8 *)this + size);
	}

    TUint32 iDOI;
    TUint32 iSIT;
};

class TTransformISAKMP;
class TProposalISAKMP : public TPayloadISAKMP
    {
public:
    inline TUint32 Size() const { return sizeof(TProposalISAKMP); }//sizeof OK because aligned
    inline TUint8 GetNum() const { return iNum; };
    inline void SetNum(TUint8 aPayload) { iNum = aPayload; };
    inline TUint8 GetProtocol() const { return iProtocol; };
    inline void SetProtocol(TUint8 aPayload) { iProtocol = aPayload; };
    inline TUint8 GetSPISize() const { return iSPISize; };
    inline void SetSPISize(TUint8 aPayload) { iSPISize = aPayload; };
    inline TUint8 GetNumTrans() const { return iNumTrans; };
    inline void SetNumTrans(TUint8 aPayload) { iNumTrans = aPayload; };
    //inline TUint32 GetSPI() const { return iSPI; };
    //inline void SetSPI(TUint32 aSPI) { iSPI = aSPI; };
    static inline TProposalISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TProposalISAKMP *)aPayload; }
    inline const TPayloadISAKMP* Payload() const    //next payload (transform)
        { return (TPayloadISAKMP*)((TUint8*)this + sizeof(*this) + GetSPISize()); } 
    inline const TUint8 *SPI() {return (const TUint8 *)((TUint8*)this + sizeof(*this));} //* to the SPI
    //void String(TDes &aStr) const;

//private:
    TUint8 iNum;        //Proposal Number
    TUint8 iProtocol;   //Protocol ID for the current negotiation PROTO_ISAKMP, PROTO_IPSEC_AH, PROTO_IPSEC_ESP, ...
    TUint8 iSPISize;    //size in octets. For ISAKMP should be from 0 to 16. If >0 SPI ignored
    TUint8 iNumTrans;   //Num of transformations
    //TUint32 iSPI;     //Variable size (depends on protocol). No Padding.
    };


class TDataISAKMP;
class TTransformISAKMP : public TPayloadISAKMP
    {
public:
    inline TTransformISAKMP() {iReserved=0;}
public:
    inline TUint32 Size() const { return sizeof(TTransformISAKMP); }    //sizeof OK because aligned
    inline TUint8 GetNum() const { return iNum; };
    inline void SetNum(TUint8 aPayload) { iNum = aPayload; };
    inline TUint8 GetID() const { return iID; };
    inline void SetID(TUint8 aPayload) { iID = aPayload; };
    inline TUint16 GetReserved() const { return (TUint16)GET16(&iReserved); };
    static inline TTransformISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TTransformISAKMP *)aPayload; }
    inline TDataISAKMP *SAAttrib() const {return (TDataISAKMP *)((TUint8*)this + sizeof(*this)); } //* to SA attributes
    //void String(TDes &aStr) const;

//private:
    //TPayloadISAKMP iHdr;  //Generic part
    TUint8 iNum;        //Transform Number
    TUint8 iID;         //Transform ID for the current negotiation AH, ESP, OSPF, TLS, ...
    TUint16 iReserved;  //always set to 0
};



class TKeyISAKMP : public TPayloadISAKMP
{
public:
    static inline TKeyISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TKeyISAKMP *)aPayload; }
    //void String(TDes &aStr) const;
    inline TUint8 *KeyData() const {return ((TUint8*)this + sizeof(*this)); } //* to keyData

};


//would require a few modifications for DOI != IPSECDOI
class TIdentISAKMP : public TPayloadISAKMP
{
public:
    inline TUint8 GetIDType() const { return iIDType; }
    inline void SetIDType(TUint8 aIDType) { iIDType=aIDType; }
    //IPSEC DOI Specific routines
    inline TUint8 GetProtocol() const { return iProtocolID; }
    inline void SetProtocol(TUint8 aProtocolID) { iProtocolID = aProtocolID; };
    inline TUint16 GetPort() const { return (TUint16)GET16(&iPort); }
    inline void SetPort(TUint16 aPort) { PUT16(&iPort, aPort); }
    //
    static inline TIdentISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TIdentISAKMP *)aPayload; }
    //TBool String(TDes &aStr,TUint aLength) const;
    inline TUint8 *IDData() const {return ((TUint8*)this + sizeof(*this)); } //* to identity data
    inline TUint16 IDDataLen() const {return (TUint16)(GetLength() - sizeof(*this)); } //Length of the payload data
public:
    TUint8 iIDType;
    //IPSEC DOI Specific fields
    TUint8 iProtocolID;
    TUint16 iPort;

};



//Used for certificate 
//WARNING:Do not use sizeof this class, better Size because size of returns aligned size and not the real.
class TCertificateISAKMP: public TPayloadISAKMP
{
public:
    inline TUint8 GetEncoding() const { return iEncoding; }
    inline void SetEncoding(TUint8 aEncoding) { iEncoding = aEncoding; }
    static inline TUint16 Size() {return (sizeof(TPayloadISAKMP) + sizeof(TUint8));}    //TUint8 is iEncoding
    inline TUint16 CertDataLen() const {return (TUint16)(GetLength()-Size()); } //* to certificate data
    inline TUint8 *CertData() const {return ((TUint8*)this + Size()); } 
    //+1 for iEncoding.Cannot use sizeof(*this) because is not aligned and returns 8 instead of 5

    static inline TCertificateISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TCertificateISAKMP *)aPayload; }
	
    TUint8 iEncoding;       // Certificate Encoding

};


                  
//Used for certificate Request payloads
class TCertificateReqISAKMP : public TPayloadISAKMP
{
public:
    inline TUint8 GetEncoding() const { return iEncoding; }
    inline void SetEncoding(TUint8 aEncoding) { iEncoding= aEncoding; }
    static inline TUint16 Size() {return (sizeof(TPayloadISAKMP) + sizeof(TUint8));}    //TUint8 is iEncoding
    static inline TCertificateReqISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TCertificateReqISAKMP *)aPayload; }
    inline TUint16 CertReqDataLen() const {return (TUint16)(GetLength() - Size()); } //* to certificate data
    inline TUint8 *CertReqData() const {return ((TUint8*)this + Size()); }
	
    TUint8 iEncoding;       // Certificate Encoding

};

class THashISAKMP : public TPayloadISAKMP
{
public:
    static inline THashISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (THashISAKMP *)aPayload; }
    inline TUint8 *Data() const {return ((TUint8*)this + sizeof(*this)); } //pointer to hash data
    inline TUint16 DataLen() const {return (TUint16)(GetLength() - sizeof(*this)); } //pointer to hash data
    //TBool String(TDes &aStr,TUint aLength) const;
};


class TSignatureISAKMP : public TPayloadISAKMP
{
public:
    static inline TSignatureISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TSignatureISAKMP *)aPayload; }
    inline TUint16 GetDataLength() const    //Length of the data part in bytes
    { return (TUint16)(GetLength() - sizeof(*this));}
    inline TUint8 *SigData() const {return ((TUint8*)this + sizeof(*this)); } //pointer to signature data

};

class TNonceISAKMP : public TPayloadISAKMP
{
public:
    static inline TNonceISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TNonceISAKMP *)aPayload; }

    inline TUint8 *NonceData() const {return ((TUint8*)this + sizeof(*this)); } //* to Nonce Data
    inline TUint16 NonceDataLen() const {return (TUint16)(GetLength() -  sizeof(*this)); } //* to Nonce Data
};



class TNotificationISAKMP : public TPayloadISAKMP
{
public:
    inline TUint32 GetDOI() const { return GET32(&iDOI); };
    inline void SetDOI(TUint32 aDOI) { PUT32(&iDOI, aDOI); };
    inline TUint8 GetProtocol() const { return iProtocol; };
    inline void SetProtocol(TUint8 aPayload) { iProtocol = aPayload; };
    inline TUint8 GetSPISize() const { return iSPISize; };
    inline void SetSPISize(TUint8 aPayload) { iSPISize = aPayload; };
    inline TUint16 GetMsgType() const { return (TUint16)GET16(&iMsgType); };
    inline void SetMsgType(TUint16 aMsgType) { PUT16(&iMsgType, aMsgType); };
    inline TUint8 *GetSPI() const { return ((TUint8*)this + sizeof(*this)); };  //returns the SPI Data
    inline TUint8 *GetNotifData() const { return ((TUint8*)this + sizeof(*this) + iSPISize); }; //returns the Notification Data
    inline TUint16 GetNotifDataSize() const { return (TUint16)(GetLength() - (sizeof(*this) + iSPISize)); };//returns the Notification Data Length

    static inline TNotificationISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TNotificationISAKMP *)aPayload; }
	
    TUint32 iDOI;       //Domain Of Interpretation
    TUint8 iProtocol;   //Protocol ID for the current notification
    TUint8 iSPISize;    //size in octets. For ISAKMP should be from 0 to 16. If >0 SPI ignored
    TUint16 iMsgType;   //Notify msg type. See constants.
    //Plus the variable fields SPI and Notification Data (both included in length)
private:
    inline void MsgTypeToStr(TDes &aBuf, TUint16 aMsgType) const
	{
#ifdef _DEBUG
		switch (aMsgType)
		{
		//NOTIFY MESSAGES - ERROR TYPES 
			case  INVALID_PAYLOAD_TYPE: 
				aBuf = _L("INVALID_PAYLOAD_TYPE");
				break;
			case  DOI_NOT_SUPPORTED: 
				aBuf = _L("DOI_NOT_SUPPORTED");
				break;
			case  SITUATION_NOT_SUPPORTED: 
				aBuf = _L("SITUATION_NOT_SUPPORTED");
				break;
			case  INVALID_COOKIE: 
				aBuf = _L("INVALID_COOKIE");
				break;
			case  INVALID_MAJOR_VERSION: 
				aBuf = _L("INVALID_MAJOR_VERSION");
				break;
			case  INVALID_MINOR_VERSION: 
				aBuf = _L("INVALID_MINOR_VERSION");
				break;
			case  INVALID_EXCHANGE_TYPE: 
				aBuf = _L("INVALID_EXCHANGE_TYPE");
				break;
			case  INVALID_FLAGS: 
				aBuf = _L("INVALID_FLAGS");
				break;
			case  INVALID_MESSAGE_ID: 
				aBuf = _L("INVALID_MESSAGE_ID");
				break;
			case  INVALID_PROTOCOL_ID: 
				aBuf = _L("INVALID_PROTOCOL_ID");
				break;
			case  INVALID_SPI: 
				aBuf = _L("INVALID_SPI");
				break;
			case  INVALID_TRANSFORM_ID: 
				aBuf = _L("INVALID_SPI");
				break;
			case  ATTRIBUTES_NOT_SUPPORTED: 
				aBuf = _L("ATTRIBUTES_NOT_SUPPORTED");
				break;
			case  NO_PROPOSAL_CHOSEN: 
				aBuf = _L("NO_PROPOSAL_CHOSEN");
				break;
			case  BAD_PROPOSAL_SYNTAX: 
				aBuf = _L("BAD_PROPOSAL_CHOSEN");
				break;
			case  PAYLOAD_MALFORMED: 
				aBuf = _L("PAYLOAD_MALFORMED");
				break;
			case  INVALID_KEY_INFORMATION: 
				aBuf = _L("INVALID_KEY_INFORMATION");
				break;
			case  INVALID_ID_INFORMATION: 
				aBuf = _L("INVALID_ID_INFORMATION");
				break;
			case  INVALID_CERT_ENCODING: 
				aBuf = _L("INVALID_CERT_ENCODING");
				break;
			case  INVALID_CERTIFICATE: 
				aBuf = _L("INVALID_CERTIFICATE");
				break;
			case  CERT_TYPE_UNSUPPORTED: 
				aBuf = _L("CERT_TYPE_UNSUPPORTED");
				break;
			case  INVALID_CERT_AUTHORITY: 
				aBuf = _L("INVALID_CERT_AUTHORITY");
				break;
			case  INVALID_HASH_INFORMATION: 
				aBuf = _L("INVALID_HASH_INFORMATION");
				break;
			case  AUTHENTICATION_FAILED: 
				aBuf = _L("AUTHENTICATION_FAILED");
				break;
			case  INVALID_SIGNATURE: 
				aBuf = _L("INVALID_SIGNATURE");
				break;
			case  ADDRESS_NOTIFICATION: 
				aBuf = _L("ADDRESS_NOTIFICATION");
				break;
			case  NOTIFY_SA_LIFETIME: 
				aBuf = _L("NOTIFY_SA_LIFETIME");
				break;
			case  CERTIFICATE_UNAVAILABLE: 
				aBuf = _L("CERTIFICATE_UNAVAILABLE");
				break;
			case  UNSUPPORTED_EXCHANGE_TYPE: 
				aBuf = _L("UNSUPPORTED_EXCHANGE_TYPE");
				break;
			case  UNEQUAL_PAYLOAD_LENGTHS: 
				aBuf = _L("UNEQUAL_PAYLOAD_LENGTHS");
				break;
			case  CONNECTED: 
				aBuf = _L("CONNECTED");
				break;
			default:
				if ((aMsgType>30) && (aMsgType < 8192))
					aBuf.Format(_L("ERROR RESERVED Future Use (%d)"),aMsgType);
				else if ((aMsgType>8191) && (aMsgType < 16384))
					aBuf.Format(_L("ERROR Private Use (%d)"),aMsgType);
				else if (((aMsgType>16384) && (aMsgType < 24576)) || ((aMsgType>40959) && (aMsgType < 65536)))
					aBuf.Format(_L("STATUS RESERVED Future Use (%d)"),aMsgType);
				else if ((aMsgType>24575) && (aMsgType < 32768))
					aBuf.Format(_L("STATUS DOI Specific (%d)"),aMsgType);
				else if ((aMsgType>32767) && (aMsgType < 40959))
					aBuf.Format(_L("STATUS Private Use (%d)"),aMsgType);
				else
					aBuf.Format(_L("UNKNOWN ERROR (%d)"),aMsgType);
		}
#else
		(void)aBuf;
		(void)aMsgType;
#endif      
	}
			
    inline void ProtocolToStr(TDes& aBuf, TUint16 aProtocol) const
	{		
#ifdef _DEBUG

	switch (aProtocol)
	{
		//Protocol number for IPSEC DOI (=1)
		case PROTO_ISAKMP: 
			aBuf = _L("PROTO_ISAKMP");
			break;
		case PROTO_IPSEC_AH: 
			aBuf = _L("PROTO_IPSEC_AH");
			break;
		case PROTO_IPSEC_ESP: 
			aBuf = _L("PROTO_IPSEC_ESP");
			break;
		case PROTO_IPCOMP: 
			aBuf = _L("PROTO_IPCOMP");
			break;
		default:
			aBuf.Format(_L("Unknown (%d) "),aProtocol);
	}
#else
	(void)aBuf;
	(void)aProtocol;
#endif  
	}
			
    
};

class TDeleteISAKMP : public TPayloadISAKMP
{
public:
    inline TUint32 DOI() const { return GET32(&iDOI); };
    inline void SetDOI(TUint32 aDOI) { PUT32(&iDOI, aDOI); };
    inline TUint8 Protocol() const { return iProtocol; };
    inline void SetProtocol(TUint8 aPayload) { iProtocol = aPayload; };
    inline TUint8 SPISize() const { return iSPISize; };
    inline void SetSPISize(TUint8 aPayload) { iSPISize = aPayload; };
    inline TUint16 NumSPI() const { return (TUint16)GET16(&iNumSPI); };
    inline void SetNumSPI(TUint16 aNumSPI) { PUT16(&iNumSPI, aNumSPI); };

    static inline TDeleteISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TDeleteISAKMP *)aPayload; }
    inline TUint8 *SPIList() const {return ((TUint8*)this + sizeof(*this)); } //* to Vendor ID Data
    inline TUint8 *SPI(TUint i) const {return ((TUint8*)this + sizeof(*this) + i * SPISize()); } //i-th SPI
    inline void String(TDes &aStr) const
	{
#ifdef _DEBUG
		aStr.Append(_L(" Delete payload "));
		aStr.AppendFormat(_L(" DOI= %u"), DOI());   
		aStr.AppendFormat(_L(" Prot=%u "), Protocol());
		aStr.AppendFormat(_L(" SPIsize= %u"), SPISize());
		aStr.AppendFormat(_L(" #SPI=%u "), NumSPI());
	//2 variable size fields

	//TUint8 *p=((TUint8 *)Payload());
		TUint8 *p;
		for (TInt i=0; i < NumSPI(); i++)
		{
			aStr.AppendFormat(_L(" #SPI(%d)="),i);
			p=SPI(i);
			for (TUint j=0 ; j < SPISize() ; j++)
			{
				if (j%4==0)
					aStr.AppendFormat(_L(" "));
				aStr.AppendFormat(_L("%02.2x"), p[j]);  //not sure SPI
			}
		}
#else
		(void)aStr;		
#endif
	}

    TUint32 iDOI;       //Domain Of Interpretation
    TUint8 iProtocol;   //Protocol ID for the current notification
    TUint8 iSPISize;    //size in octets. For ISAKMP should be from 0 to 16. If >0 SPI ignored
    TUint16 iNumSPI;    //# of SPIs to be deleted
    //Plus iNumSPI variable fields SPI
};


class TVendorISAKMP : public TPayloadISAKMP
{
public:
    static inline TVendorISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TVendorISAKMP *)aPayload; }
    inline TUint8 *VIDData() const {return ((TUint8*)this + sizeof(*this)); } //* to Vendor ID Data

};

class TCHREISAKMP : public TPayloadISAKMP
{
public:
    static inline TCHREISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TCHREISAKMP *)aPayload; }
    inline TUint16 GetLAMtype() const { return (TUint16)GET16(&iLAMType); }
    inline void SetLAMtype(TUint16 aLength) { PUT16(&iLAMType, aLength); }  
    inline TUint16 GetCHREReserved() const { return iReserved; }
    inline void SetCHREReserved() { iReserved = 0; }        
    inline TDataISAKMP *CHREAttrib() const {return (TDataISAKMP *)((TUint8*)this + sizeof(*this)); } //* to CHRE attributes 
    //void String(TDes &aStr) const;
    TUint16 iLAMType;   //LAM type          
    TUint16 iReserved;  //reserved must be zero
};

class TINTNETISAKMP : public TPayloadISAKMP
{
public:
    static inline TINTNETISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TINTNETISAKMP *)aPayload; }
    inline TDataISAKMP *INTNETAttrib() const {return (TDataISAKMP *)((TUint8*)this + sizeof(*this)); } //* to INTNET attributes 
    //void String(TDes &aStr) const;
};

class TNATDISAKMP : public TPayloadISAKMP
{
public:
    static inline TNATDISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TNATDISAKMP *)aPayload; }
    inline TUint8 *HashData() const {return ((TUint8*)this + sizeof(*this)); } //* to NAT D hash data
    inline TUint32 HashLth() const {return (GetLength() - sizeof(*this)); };
};


class TNATOaISAKMP : public TPayloadISAKMP
{
public:
    inline TUint8 GetIDType() const { return iIDType; }
    inline void SetIDType(TUint8 aIDType) { iIDType=aIDType; }
    inline void SetReservedFields() { iReserved8=0; PUT16(iReserved16, 0); }    
    //
    static inline TNATOaISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TNATOaISAKMP *)aPayload; }

    inline TUint8 *OAData() const {return ((TUint8*)this + sizeof(*this)); } //* to identity data
    inline TUint16 OADDataLen() const {return (TUint16)(GetLength() - sizeof(*this)); } //Length of the payload data
public:
    TUint8  iIDType;
    //IPSEC DOI Specific fields
    TUint8  iReserved8;
    TUint16 iReserved16;

};

class TAttributeISAKMP : public TPayloadISAKMP
{
public:
    inline TUint8 CfgMsgType() const { return iCfgMsgType; }
    inline void SetCfgMsgType(TUint8 aMsgType) { iCfgMsgType=aMsgType; }
    inline void SetReservedField() { iReserved8=0;}
    inline TUint16 Identifier() const { return (TUint16)GET16(&iIdentifier);}
    inline void SetIdentifier(TUint16 aIdentifier) { PUT16(&iIdentifier, aIdentifier); } 
    //
    static inline TAttributeISAKMP *Ptr(const TPayloadISAKMP *aPayload)
        { return (TAttributeISAKMP *)aPayload; }
    inline TDataISAKMP *AttrData() const {return (TDataISAKMP *)((TUint8*)this + sizeof(*this)); } //* to attribute data
    inline TInt AttrDataLen() const {return (TInt)(GetLength() - sizeof(*this)); } //Length of the payload data
public:
    TUint8  iCfgMsgType;
    TUint8  iReserved8;
    TUint16 iIdentifier;

};


//
// DATA ATTRIBUTES
//
class TDataISAKMP
    {
public:
    TDataISAKMP(){u.iData32[0] = 0;}
    inline TBool IsBasic() const {return ((u.iData8[0] & 0x80) != 0);} //return if basic attrib or variable
    inline void SetBasic(TBool aIsBasic)
	{
		if (aIsBasic)
			 u.iData8[0] |= 0x80;
		else u.iData8[0] &= 0x7F;
	}
    inline TUint16 Type() const	{ return (TUint16)(GET16(&u.iData16[0]) & (TUint16)0x7FFF);} 
    inline void SetType(TUint16 aType)
	{
		u.iData8[1] = (TUint8)(aType & 0x00ff);
		u.iData8[0] &= 0x80;
		u.iData8[0] |= ((TUint8)(aType >> 8)) & 0x7F;
	}
    inline TUint16 Length() const
	{ 
		if (!IsBasic()) 
		   return (TUint16)GET16(&u.iData16[1]);
		return 0;   //No length needed		
	}
    inline void SetLength(TUint16 aLength) { if (!IsBasic()) PUT16(&u.iData16[1], aLength); }
    inline TUint16 Value() const
	{ 
		if (IsBasic()) 
			return  (TUint16)GET16(&u.iData16[1]);
		return 0;
	}
    inline void SetValue(TUint16 aValue) { if (IsBasic()) PUT16(&u.iData16[1], aValue);}
    inline TUint8 *VarValue()  const
	{
		if (!IsBasic()) 
			return ((TUint8*)this + sizeof(*this));
		return NULL;
	}
    inline TUint Size() { return (sizeof(*this) + Length());}   //return attr header full size
    inline TDataISAKMP *Next() {return (TDataISAKMP *)((TUint8*)this + sizeof(*this) + Length());}  //next attribute
public:
    union
        {
        TUint32 iData32[1];
        TUint16 iData16[2];
        TUint8  iData8[4];
        } u;
    };
//Plus Length() bytes if variable attrib;

//
// TLastIKEMsg
// This class is used to detect possible IKE message retransmission from peer.
// The following information is stored to object data in consturctor:
//  -- IKE message length (octets iData8[0], iData8[1] and iData8[2])
//  -- Next payload code (iData8[3])
//  -- IP checksum over entire IKE message (iData32[1])
//
class TLastIKEMsg
    {
public:
    inline TLastIKEMsg() { u.iData32[0] = 0; u.iData32[1] = 0;}
    inline TLastIKEMsg(const ThdrISAKMP& aHdr)
	{
		u.iData32[0] = aHdr.GetLength();
		TUint16 *End = (TUint16*)((TUint8*)&aHdr + (u.iData32[0] & 0xfffffffe));
		TUint16 *Ptr = (TUint16*)&aHdr;
		u.iData8[3]  = aHdr.GetPayload(); // MUST NOT be stored before length value !
		TUint32 sum  = 0;   
		while ( Ptr < End ) sum += *Ptr++;
		u.iData32[1] = sum;
	}
    inline TBool IsUninitialized()
            {
            if (u.iData32[0] == 0 && u.iData32[1] == 0)
                return ETrue;
            return EFalse;
            }
    inline void Store(TLastIKEMsg& aRef)
            { aRef.u.iData32[0] = u.iData32[0];  aRef.u.iData32[1] = u.iData32[1]; }    
    inline TBool IsReTransmit(TLastIKEMsg& aRef)
            { return ((u.iData32[0] == aRef.u.iData32[0]) && (u.iData32[1] == aRef.u.iData32[1]));}
private:	
    union
        {
        TUint32 iData32[2];
        TUint8  iData8[8];
        } u;
    };


#endif // IKEV1PAYLOADS_H
