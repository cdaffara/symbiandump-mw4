/**
 * Copyright (c) 2010 Sasken Communication Technologies Ltd.
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html"
 *
 * Initial Contributors:
 * Lasse Laasonen, Sasken Communication Technologies Ltd - Initial contribution
 *
 * Description:
 * This header contains routines handling RSA signing using Unified Key Store 
 */

#ifndef SMFKEYSTOREMANAGER_H_
#define SMFKEYSTOREMANAGER_H_

// Include files
#include <e32base.h>
#include <f32file.h>

#ifdef	SYMBIAN_V3
#include <rmpointerarray.h>
#include <platform/mkeystore.h>
#include <platform/flogger.h>

class CUnifiedKeyStore;
class CCTKeyInfo;
class CRSASignature;
#endif

class CSmfSignParameters;
class CSmfRsaKeyParameters;

class CSmfKeyStoreManager : public CActive 
	{
public:
	/**
	 * NewL Method
	 * @return The constructed CSmfKeyStoreManager instance
	 */
	static CSmfKeyStoreManager* NewL();
	
	/**
	 * NewLC Method
	 * @return The constructed CSmfKeyStoreManager instance
	 */
	static CSmfKeyStoreManager* NewLC();
	
	/**
	 * Destructor
	 */
	~CSmfKeyStoreManager();
	
	/**
	 * HandleMessageL
	 * @param aMessage
	 */
	void HandleMessageL( const RMessage2& aMessage );
	
//	void GenerateKeyL( const RMessage2& aMessage );
//	void StoreRSAKeyL( const RMessage2& aMessage );
	
private: // from CActive
	/**
	 * RunL
	 */
	void RunL();
	
	/**
	 * DoCancel
	 */
	void DoCancel();
	
	/**
	 * RunError
	 * @param aError
	 * @return
	 */
	TInt RunError(TInt aError);
	
private:
	/**
	 * Constructor
	 */
	CSmfKeyStoreManager();
	
	/**
	 * Two-phase constructor
	 */
	void ConstructL();
	
	/**
	 * ContinueMessageHandlingL
	 */
	void ContinueMessageHandlingL();
	
	/**
	 * StoreRSAKeyL
	 */
	void StoreRSAKeyL();
	
	/**
	 * RSA_SHA1_SignMessageL
	 */
	void RSA_SHA1_SignMessageL();
	
	/**
	 * HMAC_SHA1_SignMessageL
	 */
	void HMAC_SHA1_SignMessageL();
		
	/**
	 * DeleteKeysL
	 */
	void DeleteKeysL();
	
	/**
	 * SetPassphraseTimeout
	 */
	void SetPassphraseTimeout();
	
	/**
	 * ReadSignParametersL
	 */
	void ReadSignParametersL();
	
	/**
	 * ReadRsaKeyParametersL
	 */
	void ReadRsaKeyParametersL();
	
private:
	RFs iFs;
	
	RPointerArray<RMessage2> iMessages;
	CSmfSignParameters* iSignParameters;
	CSmfRsaKeyParameters* iRsaKeyParameters;
#ifdef	SYMBIAN_V3	
	CCTKeyInfo*	iImportedKey;
	CUnifiedKeyStore* iKeyStore;
	RMPointerArray<CCTKeyInfo> iKeys;
	MRSASigner* iRSASigner;
	CRSASignature* iRSASignature;
#endif
	
	enum {
		EInitializingKeystore,
		ESettingPassphraseTimeout,
		EInitialized,
		EGeneratingKey,
		EImportingKey,
		EGettingKeyList,
		EGettingRSASigner,
		ERSASigningMessage,
		EListingKeys,
		EDeletingKey,
		EKeyDeleted		//state after the key has been deleted
	} iState;
	
};

#endif /* SMFKEYSTOREMANAGER_H_ */
