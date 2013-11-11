/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   IKEv2 specifig certificate reading related stuff
*
*/


#ifndef C_IKEV2PKISERVICE_H
#define C_IKEV2PKISERVICE_H

#include <e32base.h>

#include "pkiserviceapi.h"

class CIkeCaElem;
class TCertInfo;
class TCReqPayloadIkev2;
class CIkeData;
class CIkeCaList;
class MIkeV2PkiServiceObserver;
class TOwnCertInfo;
class MIkeDebug;


class CIkeV2PkiService : public CActive
    {
public:
        enum TIkeV2PkiServiceState
            {                
            EPkiServiceIdle = 0,
            EBuildingCaList,
            EReadingCertificate,
            EReadingCertificateChain
            };

   

        IMPORT_C static CIkeV2PkiService* NewL(MIkeV2PkiServiceObserver& aObserver, MIkeDebug& aDebug);
        IMPORT_C ~CIkeV2PkiService();
		
        
        /**
        * Reads all the CA certs defined in the list to the memory.
        *
        * This method is completed by calling:
        * MIkeV2PkiServiceObserver::IkeV2PkiInitComleteL
        *
        * @param aCAList List of CA's, which are retrieved.
        */
        IMPORT_C void InitIkeV2PkiService(const CIkeData* aIkeData);
					
        /*
        * Computes IKEv2 signature with a specified private key.
        * Actually a signature computed happens by referring the related certificate
        * when the PKI store produces the signature with corresponding private key.
        * 
        * @param aTrustedAuthority Trusted CA name coded either in ASN1 (DN) format or ASCII format
        * @param aOwnCertInfo Own cert info from the IKE policy
        * @param aMsgOctets Message data signed. A SHA1 hash is calculated over these message
        *                   octets and result is encoded as PKCS1v15 signature before
        *                   encrypted with private key
        * @result signature length.
        */			
		IMPORT_C TInt Ikev2SignatureL(const TDesC8& aTrustedAuthority, 
		                              const TOwnCertInfo& aOwnCertInfo,
		                              const TDesC8& aMsgOctets, 
		                              TDes8& aSignature, TUint8 aAuthMeth);	
		                              
		                              
        /**
         * Returns a list of trusted CA certificates.
         * If the InitIkeV2PkiService method is not called
         * successfully before this method is called an
         * empty list is returned.
         *
         * @result a list of trusted CA certificates.
         */		                              
		IMPORT_C const CIkeCaList& CaList() const;
		
		
		/**
		 * 
		 */
		IMPORT_C const TDesC8& UserCertificateData() const;
		
		IMPORT_C const TDesC8& I2CertificateData() const;
		IMPORT_C const TDesC8& I1CertificateData() const;
		/**
		 * Returns the name of the trsuted authority of 
		 * user certificate.
		 *
		 * If no user certificates are loaded an empty string is returned.
		 * 
		 * @result the name of the trusted authority of the user certificate.
		 */
		IMPORT_C const TDesC8& TrustedCaName() const;		

protected:		
		void DoCancel();
		void RunL();
	    TInt RunError(TInt aError); 		
	    
private:
	    CIkeV2PkiService(MIkeV2PkiServiceObserver& aObserver, MIkeDebug& aDebug);
	    void ConstructL();
	
	    void ReadTrustedUserCertificateL();					
		void ReadUserCertificateL(const TDesC8& aTrustedAuthority, TBool aGetCACert);	
		void ReadCertificateChainL();
	
	
        void InitIkeV2PkiServiceL();
        void ImportNextCaElemFromIkeDataListL();          
        void BuildingCaListRunL();               
        void ReadUserCertificateRunL();
        void ReadCertificateChainRunL();                
                        
        static void CIkeV2PkiServiceApplUidArrayCleanup(TAny* any);		


        void SignalObserverL(TInt aStatus);
private:
        MIkeV2PkiServiceObserver&   iObserver; 
        MIkeDebug& iDebug;
    
        RPKIServiceAPI              iPkiService; // PKI Service handle 		
		TIkeV2PkiServiceState       iState;      // Current state								
		
		HBufC8*              iCaName;		 // Ca name work buffer			
		const CIkeData*      iIkeData;       // Current policy data object  				
		
		
		HBufC8*              iReadCertificate; // Certificate stream
		TPtr8                iCertPtr;         // For Pkiserviceapi calls

		HBufC8*              iSubjName;      // Subject alt name buffer
		HBufC8*              iRfc822Name;    // RFC822 name buffer
        TPKIKeyIdentifier    iCertKeyId;     // Certficate keyid							

		TAny*                iResArray;      // For Pkiserviceapi calls
						
		RPointerArray<CIkeCaElem> iCasTrustedByPeer;     // CA name list delivered												                
        CArrayFixFlat<TCertInfo>* iIkeDataCAList;		
        CIkeCaList*               iTrustedCAList; // Trusted CA certificate list
        HBufC8*                   iUserCertificate;
        HBufC8*                   i2Certificate; // Intermediate certificate Level 2
        HBufC8*                   i2CertificateName;
        HBufC8*                   i1Certificate; // Intermediate certificate Level 1
    };

    
/** 
* IKE PKI service request complete
* @internalComponent
*/
class MIkeV2PkiServiceObserver
{
	public:
		/** 
         * IKE PKI service operation completed 
         * @internalComponent
         * @param aStatus completion status of operation
         * @param aObject pointer to CIkePkiService object
         * 
         */
		virtual void IkeV2PkiInitCompleteL(TInt aStatus)=0; 
};
    

#endif //C_IKEV2PKISERVICE_H
