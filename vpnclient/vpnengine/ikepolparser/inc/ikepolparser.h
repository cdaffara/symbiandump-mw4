/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   IKE policy parser main module
*
*/



#ifndef __IKEPOLPARSER_H
#define __IKEPOLPARSER_H

#include <f32file.h>
#include <badesca.h>
#include <in_sock.h>

#define FIRST_SEC_PARSER_VERSION    1
#define SEC_PARSER_VERSION          3

#define IKE_HOST_SEPARATOR  _L("[HOST]")
#define IKE_SEPARATOR  _L("[IKE]")
#define IKE_HOST_SEPARATOR_LEN  6
#define IKE_GENERAL_SEPARATOR   _L("[GENERAL]")
#define IKE_GENERAL_SEPARATOR_LEN   9

#define MAX_INFO_SIZE   1024
#define PIECE_AVG_LENGTH 2048

const TInt KIkePolicyBufferSizeIncrement = 6000;

const TInt KNokiaNattDefaultPort = 9872;

//Security Ike Parser Error codes
// NOTE! The error code values below MUST be kept in sync with
// the corresponding error code values defined together by
// vpnapi/data/vpnerr.rss and vpnapi/data/vpnerr.ra
const TInt KSecParserErrMode                = -5263;
const TInt KSecParserErrNotify              = -5264;
const TInt KSecParserErrCommit              = -5265;
const TInt KSecParserErrIpsecExpire         = -5266;
const TInt KSecParserErrSendCert            = -5267;
const TInt KSecParserErrInitialContact      = -5268;
const TInt KSecParserErrResponderLifetime   = -5269;
const TInt KSecParserErrReplayStatus        = -5270;
const TInt KSecParserErrGroupDesc_II        = -5271;
const TInt KSecParserErrProposal            = -5272;
const TInt KSecParserErrEncrAlg             = -5273;
const TInt KSecParserErrAuthMethod          = -5274;
const TInt KSecParserErrHashAlg             = -5275;
const TInt KSecParserErrGroupDesc           = -5276;
const TInt KSecParserErrGroupType           = -5277;
const TInt KSecParserErrLifeBytes           = -5278;
const TInt KSecParserErrLifeSecs            = -5279;
const TInt KSecParserErrPRF                 = -5280;
const TInt KSecParserErrPreKey              = -5281;
const TInt KSecParserErrPreFormat           = -5282;
const TInt KSecParserErrCA                  = -5283;
const TInt KSecParserErrOwnCerts            = -5284;
const TInt KSecParserErrOwnName             = -5285;
const TInt KSecParserErrOwnKey              = -5286;
const TInt KSecParserErrPeerCerts           = -5287;
const TInt KSecParserErrPeerAddr            = -5288;
const TInt KSecParserErrPeerMask            = -5289;
const TInt KSecParserErrMaxLifetimeSec      = -5290;
const TInt KSecParserErrMaxLifetimeKB       = -5291;
const TInt KSecParserErrMaxRetrans          = -5292;
const TInt KSecParserErrNoSeparator         = -5293;
const TInt KSecParserErrCRACKLAMType        = -5294;
const TInt KSecParserErrUseIntAddr          = -5295;
const TInt KSecParserErrUseNATProbe         = -5296;
const TInt KSecParserErrUnknown             = -5297;



//Values for the choice lists used in the .RSS
enum {IKE_PARSER_MAIN,
      IKE_PARSER_AGGRESSIVE};       //Modes
enum {IKE_PARSER_DES_CBC, 
      IKE_PARSER_DES3_CBC, 
      IKE_PARSER_AES_CBC};
enum {IKE_PARSER_PRE_SHARED, 
      IKE_PARSER_DSS_SIG, 
      IKE_PARSER_RSA_SIG,
      IKE_PARSER_RSA_ENCR, 
      IKE_PARSER_RSA_REV_ENCR, 
      IKE_PARSER_CRACK};
enum {IKE_PARSER_MD5,
      IKE_PARSER_SHA1};
enum {IKE_PARSER_MODP_768 = 1, 
      IKE_PARSER_MODP_1024 = 2, 
      IKE_PARSER_MODP_1536 = 5, 
      IKE_PARSER_MODP_2048 = 14};
enum {IKE_PARSER_DEFAULT,
      IKE_PARSER_MODP};
enum {IKE_PARSER_NONE,
      IKE_PARSER_DES3_CBC_MAC};

//CRACK Legacy Authentication types
#define IKE_PARSER_CRACK_PASSWORD  1

//------------------------------------------------------------------------
//
// IKE Parser
//
//------------------------------------------------------------------------

//Data on a single proposal
struct TProposalData
{
    TUint8 iEncrAlg;
    TUint8 iAuthMeth;
    TUint8 iHashAlg;
    TUint8 iGroupDesc;
    TUint8 iGroupType;
    TUint8 iPRF;
    TUint32 iLifetimeKb;
    TUint32 iLifetimeSec;
    TInt    iEncrKeyLth;
    
    TProposalData *iNext;   //Used for chaining
    TProposalData *iPrev;   //to make easier changing the order

};


static const TUint KMaxPresharedKeyLength = 256;
enum TKeyFormat
{
    STRING_KEY = 0,
    HEX_KEY = 1
};

struct TPresharedKeyData
{
    TBuf<KMaxPresharedKeyLength> iKey;
    TKeyFormat iFormat;
};

enum TCertFormat
{
    PEM_CERT = 0,
    BIN_CERT = 1,
    CA_NAME  = 2,
    KEY_ID   = 3,
    APPL_UID = 4			   
};


//RSA Encryption peer public keys
struct TCertInfo
{
    TCertFormat iFormat;
    TFileName   iData;    
};


enum TIdentityAsRfc822Name
    {
    EIdentityAsRfc822NameUndefined = 0,
    EIdentityAsRfc822NameYes = 1,
    EIdentityAsRfc822NameNo = 2
    };

//Own certificates
struct TOwnCertInfo
{
    TCertFormat iFormat;
    TFileName iData;    //File containing the peer RSA public key
    TBuf<128> iRfc822NameFqdn;
    TIdentityAsRfc822Name iIdentityAsRfc822Name;
    TBuf<256> iSubjectDnSuffix;
    TInt iPrivateKeyLength;
    TInt iOwnCertExists;                     // 0 = no own cert   1 = own cert exists
};

class TStringData
{
	public:
		TStringData() :iBfr(NULL) {}		
		TStringData(HBufC16* aBfr) :iBfr(aBfr) {}
		~TStringData() {delete iBfr;}		
		static inline TStringData* NewL(TInt aLth)
		{
			HBufC16* Databfr = HBufC16::NewL(aLth);
			TStringData* StringData = new (ELeave)TStringData(Databfr);
			return StringData;
		}
		static inline TStringData* NewL(const TPtrC16& aString)
		{
			TStringData* Obj = TStringData::NewL(aString.Length());
			Obj->iBfr->Des().Copy(aString);
			return Obj;
		}
		inline TPtrC16 GetData()
		{
			TPtrC16 DataPtr(NULL, 0);
			if ( iBfr )
				DataPtr.Set(iBfr->Des());
			return DataPtr;
		}
		inline HBufC8* GetAsciiDataL()
		{
			HBufC8* Bfr8 = NULL;
			if ( iBfr )
			{
			   Bfr8	= HBufC8::NewL(iBfr->Des().Length());
			   Bfr8->Des().Copy(iBfr->Des());
			}	
			return Bfr8;
		}	
		
	private:
		HBufC16*  iBfr;                    // Variable length data buffer
		
};


class CIkeData : public CBase
{
public:
    IMPORT_C static CIkeData* NewL();
    IMPORT_C static CIkeData* NewL(const CIkeData *aData);
    IMPORT_C void ConstructL(const CIkeData *aData);
    
    EXPORT_C ~CIkeData();
    IMPORT_C void CopyL(const CIkeData *aData);
    void Empty();
private:
    CIkeData();
public:
	TStringData* iFQDNAddr; //Peer host address in FQDN format
    TInetAddr iAddr;       //Peer host address
    TInetAddr iMask;       //Peer host mask    
    TInetAddr iDnsServer;
    TUint8 iMode;
	TUint8 iIkeVersion;    //Preferred IKE version 
	TUint8 iIdType;        //IKE Phase 1 identity type code. Integer value according to rfc2407
	TUint8 iRemoteIdType;  
	TBool  iSkipRemoteIdCheck; 
    TBuf<128> iFQDN;
	TStringData* iRemoteIdentity;
    TBool iAcceptPartialRemoteId;

	TBool iNotify;	
    TBool iCommit;          //COMMIT bit processing
    TBool iInitialContact;  //INITIAL-CONTACT sending
    TBool iResponderLifetime;   //RESPONDER_LIFETIME sending
    TBool iReplayStatus;    //REPLAY_STATUS sending
    TBool iIpsecExpires;    //IPSEC SAs expire with ISAKMP SAs
    TBool iAlwaysSendCert;  //Sends a Cert without even if NOT receiving a CR

//  Private IKE extension
    TBool  iUseInternalAddr;
    TBool  iUseNatProbing;
    TBool  iUseXauth;       //Use XAUTH defined in <draft-beaulieu-ike-xauth-02.txt>
    TBool  iUseCfgMode;     //Use CFG-MODE defined in <draft-dukes-ike-mode-cfg-01.txt>
	TBool  iUseMobIke;      //Use MOBIKE Ikev2 extension<draft-ietf-mobike-protocol-04.txt>	
    TInt   iEspUdpPort;     //Port value for IPSEC ESP encapsulation (= In  NAT-traversal)
	TInt   iNatKeepAlive;   //NAT keep alive timeout in seconds (if zero not used)
	TUint8 iDscp;           //DSCP tag
	TInt   iDPDHeartBeat;   //DPD Heart beat timeout in seconds (if zero not used)
	TInt   iRekeyingThreshold; //Rekeying threshold value
	TUint8 iGroupDesc_II;   
	TUint8 iCRACKLAMType;   //CRACK Legacy Authentication Method type
	TStringData* iCRACKLAMUserName;
	TStringData* iCRACKLAMPassword;
	TStringData* iClientCertType;
	TUint8 iEAPProtocol;
	TUint8 iEAPReserved;
	TBool  iEAPHideIdentity;
	TStringData* iEAPRealmPrefix;
	TStringData* iEAPManualRealm;
	TStringData* iEAPManualUserName;		
	TBool iCARequired;		//If RSA based authentication method is used then at least one CA is required 
    TBool iCAFound;			//CAs section
    TInt iNumProp;
    TProposalData *iPropList;
    TPresharedKeyData iPresharedKey;
    TCertInfo iCA;                      // Left for backward compatibility    
    CArrayFixFlat<TCertInfo*> *iCAList; // For multiple CAs
    TOwnCertInfo iOwnCert;
    TCertInfo iPrivKey;
    TCertInfo iPeerCert;

};


class CIkeDataArray : public CArrayFixFlat<CIkeData*>
{
public:
    IMPORT_C static CIkeDataArray* NewL(TInt aGranularity);
    IMPORT_C static CIkeDataArray* NewL(CIkeDataArray *aData);
    IMPORT_C void ConstructL(CIkeDataArray *aData);

    IMPORT_C ~CIkeDataArray();
    IMPORT_C void CopyL(CIkeDataArray *aData);
    IMPORT_C void Empty();
private:
    CIkeDataArray(TInt aGranularity);
    CIkeDataArray(CIkeDataArray *aData);
public: 
    //General Data
    TUint32 iMaxLifetimeSec;
    TUint32 iMaxLifetimeKB;
    TInt iMaxRetrans;
    TInt iMaxTraceFileSize;
};

class TIkeParser : public TLex
{
public:
    IMPORT_C TIkeParser();
    IMPORT_C TIkeParser(const TDesC &aStr, TInt aVersion = SEC_PARSER_VERSION);
    IMPORT_C void MainParseL(CIkeDataArray *aIkeList);
    IMPORT_C void ParseL(CIkeData *aConf);
    IMPORT_C static TInt MainWrite(CIkeData *aConf, HBufC8*& aPolBfr);
    IMPORT_C static TInt GeneralInfoWrite(
        TUint32 aMaxLifetimeSec, TUint32 aMaxLifetimeKB, 
        TInt aMaxRetrans, HBufC8*& aPolBfr); 
    IMPORT_C static TInt GeneralInfoWrite(
        TUint32 aMaxLifetimeSec, TUint32 aMaxLifetimeKB, 
        TInt aMaxRetrans, TInt aMaxTraceFileSize, HBufC8*& aPolBfr);
    IMPORT_C static TInt Write(CIkeData *aConf, HBufC8*& aPolBfr);
    IMPORT_C void ParseIKESectionsL(CIkeDataArray *aIkeList);
    IMPORT_C static TInt BufferAppend( HBufC8*& aPolBfr, const TDesC8& aText);

private:
    TInt ParseGeneralData(CIkeDataArray *aIkeList);
    void ParseIkePieceL(CIkeData *aIkeData, TInt aVersion, TPtrC& aSectionName);
    void DoMainParseL(CIkeDataArray *aIkeList, TPtrC& aSectionName);    
    TInt ParseProposals(CIkeData *aConf);
    TInt ParsePresharedKeys(CIkeData *aConf);
    TInt ParseCAs(CIkeData *aConf);
    TInt ParseCAItem(CIkeData *aConf, TCertInfo *aCA, TPtrC& aToken);
    void ParseCAListL(CIkeData *aConf, TInt aCACount, TPtrC& aToken);      
    TInt ParseOwnCerts(CIkeData *aConf);
    TInt ParsePeerCerts(CIkeData *aConf);
	TInt CheckPolicy(CIkeData* aConf);
    TPtrC GetRestOfLine();    
    TInt ParseFileName(TDes& aFileName);
    
    static TBufC<16> TextMode(const TInt aMode);
    static TBufC<16> TextEncrAlg(const TInt aAlg, const TInt aKeyLth);
    static TBufC<16> TextAuthMethod(const TInt aMethod);
    static TBufC<16> TextHashAlg(const TInt aAlg);
    static TBufC<16> TextGroupDesc(const TInt aDesc);
    static TBufC<16> TextGroupType(const TInt aType);
    static TBufC<16> TextPRF(const TInt aPRF);
    static TBufC<16> TextFormat(const TKeyFormat aFormat);
    static TBufC<16> CertFormat(TCertFormat aFormat);
    static TBufC<16> TextLAMType(const TUint8 aLAMType);
private:
     TInt iVersion;
};


class CGeneralData : public CBase
{
public:
    IMPORT_C CGeneralData();
    IMPORT_C CGeneralData(CGeneralData* aData);
public:
    TInt iDeactivationTimeout;
};



//------------------------------------------------------------------------
//Multiple Security Info combined
class CSecurityPiece : public CBase
{
public:
    IMPORT_C void ConstructL(TInt aSize = 64);  //default 64 bytes description
    inline HBufC *Info() {return iInfo;}
    IMPORT_C void SetInfoL(const TDesC &aDes);       //needed to resize it if needed
    inline CIkeData *IkeData() {return iIkeData;}
    inline CGeneralData *GeneralData(){return iGeneralData;}
    IMPORT_C ~CSecurityPiece();

private:
    HBufC* iInfo;
    CIkeData *iIkeData;
    CGeneralData *iGeneralData;
};

typedef CArrayFixFlat<CSecurityPiece *> CPiecesList;

//
//  class TSecParser
//

class TSecParser : public TLex
{
public:
    IMPORT_C TSecParser(const TDesC &aDes);
    IMPORT_C TInt ParseL(CSecurityPiece *aPiece_data);
    IMPORT_C static TInt Write(CSecurityPiece *aPiece_data, HBufC8*& aPolBfr);
    IMPORT_C TInt ParseGeneralData(CGeneralData* aData);
private:
    TBool CheckVersion();
    void ParseInfoL(CSecurityPiece *aPiece_data);  
    void NextTag();
    TInt DoParseL(CSecurityPiece *aPiece_data, TBool aIncludeIKE);
    TInt ParseGeneral(CGeneralData *aGeneralData);
            
    static TInt WriteVersion(HBufC8*& aPolBfr);
    static TInt WriteInfo(CSecurityPiece *aPiece_data, HBufC8*& aPolBfr);
    static TInt WriteIke(CSecurityPiece *aPiece_data, HBufC8*& aPolBfr);
    
private:
    TInt iVersion;  //File version
};

class TGeneralParser : public TLex
{
public:
    IMPORT_C TGeneralParser(const TDesC &aStr);
    IMPORT_C TInt Parse(CGeneralData *aData);
};

class IkeParser
{
	public:
		IMPORT_C static RArray<TUid>* GetApplUidListL(const TDesC16& aApplUidString);
		IMPORT_C static HBufC8* TextToHexOctetsL(const TDesC16& aTextString);
		IMPORT_C static TBool TextToHexOctets(const TDesC16& aTextString, TDes8& aHexOctets);				

	private:
		void  static NextHexaDigit(TLex& aUidString);		
		TBool static DeHex(const TUint16* aUcStr, TInt aStrLen, TUint8* aDstBfr);
		TBool static HexVal(TUint8& c);		
};

#endif
