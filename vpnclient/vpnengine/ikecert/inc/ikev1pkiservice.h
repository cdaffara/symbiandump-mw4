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
* Description:   PKI store and Certificate verification interface for IKEv1 
*                plug-in
*
*/

#ifndef C_IKEV1PKISERVICE_H
#define C_IKEV1PKISERVICE_H

#include <e32base.h>
#include <asn1dec.h>

#include "pkiserviceapi.h"
#include "ikepolparser.h"

class CX509Certificate;
class TASN1DecGeneric;

class CIkeData;
class CIkeCaList;
class TCertPayloadIkev2;
class TCReqPayloadIkev2;
class TCertificateISAKMP;
class MIkePlugInServices;
class MIkeDebug;
class CIkeCaElem;
//
// CIkeV1PkiService Class
//
class CIkeV1PkiService : public CBase
{
    public:
        IMPORT_C static CIkeV1PkiService* NewL( CIkeData* aIkeData,
                                                MIkeDebug& aDebug );        
        IMPORT_C ~CIkeV1PkiService();
		
        
		IMPORT_C TBool ImportCACertsL(CArrayFixFlat<TCertInfo*> *aCAList);
		IMPORT_C TInt ReadUserCertWithNameL(const TDesC8& aTrustedCaName, CIkeData* aIkeData, TBool aDnType);
		IMPORT_C TInt Ikev1SignatureL(const TDesC8& aTrustedCaName, CIkeData* aIkeData, const TDesC8& aHashIn, TDes8& aSignature);
		IMPORT_C CIkeCaList* CaList();
		IMPORT_C HBufC8* GetCertificate();
		IMPORT_C HBufC8* GetTrustedCA();		
		IMPORT_C HBufC8* GetTrustedICA1();
		IMPORT_C HBufC8* GetTrustedICA2();
		IMPORT_C TInt ReadChainL(CIkeData* aIkeData, const HBufC8* aCAName);
	    
	private:
	    CIkeV1PkiService( CIkeData* aIkeData,
	                      MIkeDebug& aDebug );
	    void ConstructL();
	
		TInt ComputeSignatureL(const TDesC8& aTrustedAuthority, const TDesC8& aHashIn, TDes8& aSignature, TBool aRsaSignature);
		TInt ReadCertificateL(const TDesC8& aTrustedAuthority, TBool aGetCACert);
		
		/**
		 * Initialized user certificate identification member variables.
		 * The information is taken from the policy file. 
		 * Only the ID data, which is available is set, other
		 * data is zeroed.
		 *
		 * @result the size of the key, if available in the policy, zero otherwise.
		 */
		TUint InitUserCertIdentDataL();		
		TBool GetNextCaElemL();
		TBool AddNextCaElemL(TInt& aStatus);
		TInt GetNextCertificateL();		
		TBool CertificateReadL(TInt& aStatus);				

		TInt ReadCertificateL(const TPKIKeyIdentifier& aKeyIdentifier);
		TInt ReadCertificateListL();		
		TInt GetCertificateWithKeyIdL(const TDesC16& aKeyIdString);
		TBool GetApplUidListL(const TDesC16& aApplUidString);
		TBool ApplUidCertListCompletedL(TInt aStatus);
		TBool ReadNextInListL();

		
#ifdef _DEBUG		
		void CertReadCompleted(TBool aCaCert, TInt aStatus, TInt aLine );
		void HexToString(const TDesC8& aKeyId, TDes16& aKeyIdString);				
#endif

				
    private:
		TInt                 iOperation;     // Current operation ongoing
		RPKIServiceAPI       iPkiService;    // PKI Service handle 		
		CIkeCaList*          iTrustedCAList; // Trusted CA certificate list
		
		TInt                 iCurrIndex;     // Current index in name list 
		CArrayFixFlat<TCertInfo*>* iCaNameList; // CA name list delivered
		HBufC8*              iCaName;		 // Ca name work buffer
		HBufC8*              iCa2Name;       // Level 1 Intermediate Certificate
		HBufC8*              iCa1Name;       // Level 2 Intermediate Certificate
				
		CIkeData*            iIkeData;       // Current policy data object  		
		
		RPointerArray<CIkeCaElem> iCasTrustedByPeer;     // CA name list delivered		
		
		HBufC8*              iReadCertificate; // Certificate stream
		HBufC8*              iReadCertificateOrig; // Certificate stream of original own certificate
		HBufC8*              iSubjName;      // Subject alt name buffer
		HBufC8*              iRfc822Name;    // RFC822 name buffer

		TPtr8                iCertPtr;       // For Pkiserviceapi calls
		TAny*                iResArray;      // For Pkiserviceapi calls
		TBool                iReallocated;   // Certificate buffer enlarged		
		TInt                 iCertBfrSize;   // Certificate buffer size

		TPKIKeyIdentifier    iCertKeyId;     // Certficate keyid
		RArray<TUid>*        iApplUidList;   // Application UID list
		CArrayFix<TCertificateListEntry>*  iCaCertList;   // Applicable CA cert list
		TInt                 iListIndex;     // Current index in CA cert list 		
		TBool                iUserCertDerType; //0=ASCII, 1=DER
		MIkeDebug&           iDebug;
		
};


//
//  CIkePkiService operation codes (iOperation)
//
const TInt KNoOperation              = 0;
const TInt KBuildingCaList           = 1;
const TInt KReadingCertificate       = 2;
const TInt KProcessingApplUidList    = 3;
const TInt KBuildingApplUidList      = 4;
const TInt KSigning                  = 5;
const TInt KDecrypting               = 6;


//
//  Certificate field indicators for GetCertificateFieldDERL()
//

#define IKEV2_CERT_KEYID_SIZE  20
//
//Extra Errors for IkeCert::VerifyCertificateL()
//
const TInt KCertVerifyErrBadType = 1;
const TInt KCertVerifyErrNotValidYet = 2;
const TInt KCertVerifyErrExpired = 3;
const TInt KCertVerifyWithinMargin = 4;
const TInt KCertVerifyCriticalExt   = 5;
const TInt KCertVerifyKeyUsageErr   = 6;
const TInt KCertVerifyCACertificate = 7;

const TInt KDefaultErrorMargin = 3600;  // Default error marginal in Validity

const TInt KSha1hashLth  = 20;          
const TInt KCertKeyIdLth = 20;          // Certificate Key Identifier length = Length of SHA1 hash


#endif // C_IKEV1PKISERVICE_H
