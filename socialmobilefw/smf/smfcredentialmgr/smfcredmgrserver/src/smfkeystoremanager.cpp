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
 * This source contains routines handling RSA signing using Unified Key Store 
 */

#ifdef SYMBIAN_V3
#include <unifiedkeystore.h>
#include <asymmetric.h>
#include <mctkeystore.h>
#endif

#include <smfcredmgrclientdatastruct.h>
#include <smfcredmgrcommon.h>
#include <hash.h>

#include "smfkeystoremanager.h"

/**
 * NewL Method
 * @return The constructed CSmfKeyStoreManager instance
 */
CSmfKeyStoreManager* CSmfKeyStoreManager::NewL() {
	CSmfKeyStoreManager* self = CSmfKeyStoreManager::NewLC();
	CleanupStack::Pop( self );
	return self;
}

/**
 * NewLC Method
 * @return The constructed CSmfKeyStoreManager instance
 */
CSmfKeyStoreManager* CSmfKeyStoreManager::NewLC() {
	CSmfKeyStoreManager* self = new( ELeave )CSmfKeyStoreManager();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
}

/**
 * Destructor
 */
CSmfKeyStoreManager::~CSmfKeyStoreManager() {
	iFs.Close();
	
	delete iSignParameters;
	iSignParameters = NULL;
	
	delete iRsaKeyParameters;
	iRsaKeyParameters = NULL;
	
}

/**
 * HandleMessageL
 * @param aMessage
 */
void CSmfKeyStoreManager::HandleMessageL( const RMessage2& aMessage ) 
	{
	RDebug::Printf("SMF: CSmfKeyStoreManager::HandleMessageL");
	iMessages.AppendL( &aMessage );
	if ( !IsActive() ) 
		{
		ContinueMessageHandlingL();
		}
	}

/**
 * RunL
 */
void CSmfKeyStoreManager::RunL() 
	{
	RDebug::Printf("SMF: CSmfKeyStoreManager::RunL, iStatus=%d", iStatus.Int() );
	
	switch( iState )
		{
		case EInitializingKeystore:
			{
			SetPassphraseTimeout();
			break;			
			}
		case ESettingPassphraseTimeout:
			{
			RDebug::Printf("SMF: Key store manager initialized");
			iState = EInitialized;
			break;
			}		
#ifdef SYMBIAN_V3
		case EGeneratingKey:
			{
			iState = EInitialized;
			RDebug::Printf("SMF: Completing generate key message");			
			iMessages[0]->Complete( iStatus.Int() );
			iMessages.Remove(0);
			break;
			}
		case EImportingKey:
			{
			iState = EInitialized;
			TInt num = iStatus.Int();
			if( iStatus.Int() == KErrNone ) 
				{
				iMessages[0]->WriteL( 1, iImportedKey->ID() );
				}
			
			RDebug::Printf("SMF: Completing import key message");
			iMessages[0]->Complete( iStatus.Int() );
			iMessages.Remove(0);
			break;
			}
		case EGettingKeyList:
		case EGettingRSASigner:
			{
			RSA_SHA1_SignMessageL();
			return;
			}
		case ERSASigningMessage:
			{
			iState = EInitialized;
			
			if ( iStatus.Int() == KErrNone ) 
				{
				HBufC8* signature = iRSASignature->S().BufferLC();
				RDebug::Printf("SMF: Signed message=%S", signature);			
				iMessages[0]->WriteL(1, *signature);			
				CleanupStack::PopAndDestroy();
				}
			iMessages[0]->Complete( iStatus.Int() );
			iMessages.Remove(0);
			break;
			}
		case EListingKeys:
			{
			iState = EInitialized;
			RDebug::Printf("Key count: %d", iKeys.Count() );
			for (TInt i = 0; i < iKeys.Count(); i++)
				{
				CCTKeyInfo* info = iKeys[i];
				RDebug::Printf("key label: %S", &info->Label());
				}
			iMessages[0]->Complete( iStatus.Int() );
			iMessages.Remove(0);
			break;
			}
		case EKeyDeleted:
			{
			iState = EInitialized;
			iMessages[0]->Complete(iStatus.Int());
			iMessages.Remove( 0 );
			break;
			}
		case EDeletingKey:
			{
			DeleteKeysL();
			break;
			}
#endif
		default:
			{
			break;
			}
		}
		
	if ( iMessages.Count() )
		{
		ContinueMessageHandlingL();
		}	
	}

/**
 * DoCancel
 */
void CSmfKeyStoreManager::DoCancel() {	
}

/**
 * RunError
 * @param aError
 * @return
 */
TInt CSmfKeyStoreManager::RunError( TInt aError ) {
	RDebug::Printf("SMF: CSmfKeyStoreManager::RunError error=%d", aError);
	
	if ( iMessages.Count() ) 
		{
		iMessages[0]->Complete( aError );
		iMessages.Remove( 0 );
		}

	return KErrNone;
}

/**
 * Constructor
 */
CSmfKeyStoreManager::CSmfKeyStoreManager() 
	:CActive(EPriorityStandard), iState(EInitializingKeystore) 
	{
	}

/**
 * Two-phase constructor
 */
void CSmfKeyStoreManager::ConstructL() {
	RDebug::Printf("SMF: CSmfKeyStoreManager::ConstructL");
	CActiveScheduler::Add( this );
#ifdef	SYMBIAN_V3
	User::LeaveIfError( iFs.Connect() );
	
	iKeyStore = CUnifiedKeyStore::NewL(iFs);
	iKeyStore->Initialize( iStatus );
	iState = EInitializingKeystore;
	SetActive();	
#endif
}

/**
 * ContinueMessageHandlingL
 */
void CSmfKeyStoreManager::ContinueMessageHandlingL() 
	{
	RDebug::Printf("SMF: CSmfKeyStoreManager::ContinueMessageHandling");
	
	if ( IsActive() )
		{
		return;
		}
	
	RMessage2* message = iMessages[0];
	TInt function = message->Function();
	
	switch( function ) 
		{
		case ESmfStoreRSAKey:
			{
			StoreRSAKeyL();
			break;
			}
		case ESmfRSASignMessage:
			{
			RSA_SHA1_SignMessageL();
			break;
			}
		case ESmfHMACSHA1SignMessage:
			{
			HMAC_SHA1_SignMessageL();
			break;
			}
		case ESmfDeleteKeys:
			{
			DeleteKeysL();
			break;
			}
		}
	}

/**
 * StoreRSAKeyL
 */
void CSmfKeyStoreManager::StoreRSAKeyL() 
	{
#ifdef SYMBIAN_V3
	RDebug::Printf("SMF: CSmfKeyStoreManager::StoreRSAKeyL");
	
	ReadRsaKeyParametersL();
	
	RDebug::Printf("SMF: Parameters read");
	
	iKeyStore->ImportKey(
			0, iRsaKeyParameters->KeyData(), EPKCS15UsageSign, iRsaKeyParameters->KeyName(),
			CCTKeyInfo::EExtractable, iRsaKeyParameters->StartDate(), iRsaKeyParameters->EndDate(),
			iImportedKey, iStatus );
	SetActive();
	iState = EImportingKey;
#endif
	}

/**
 * RSA_SHA1_SignMessageL
 */
void CSmfKeyStoreManager::RSA_SHA1_SignMessageL() 
	{
	
	RDebug::Printf("SMF: CSmfKeyStoreManager::RSA_SHA1_SignMessage");
#ifdef SYMBIAN_V3
	switch( iState ) 
		{
		case EInitialized:
			{
			// Get the key
			ReadSignParametersL(); 
			TCTKeyAttributeFilter filter;
			filter.iKeyAlgorithm = CCTKeyInfo::ERSA;
			
			iKeyStore->List( iKeys, filter, iStatus );
			iState = EGettingKeyList;
			SetActive();
			break;
			}
		case EGettingKeyList:
			{
			RDebug::Printf("SMF: key count=%d", iKeys.Count());
			// Find the correct key
			bool keyFound = EFalse;
			for( TInt i = 0; i < iKeys.Count(); i++ ) 
				{
			
				if ( iKeys[i]->ID() == iSignParameters->Key() ) 
					{
					RDebug::Printf("SMF: Correct key found");
					//might panic in CodeScanner, this open returns void
					iKeyStore->Open( *iKeys[i], iRSASigner, iStatus );
					iState = EGettingRSASigner;
					keyFound = ETrue;
					SetActive();
					break;						
					}
				}
			if ( !keyFound ) 
				{
				iMessages[0]->Complete( KErrNotFound );
				iMessages.Remove( 0 );
				}
			break;
			}
		case EGettingRSASigner:
			{
			ASSERT( iRSASigner != NULL );
			iRSASigner->SignMessage(iSignParameters->Message(), iRSASignature, iStatus);
			iState = ERSASigningMessage;
			SetActive();
			break;
			}
		default:
			{
			break;
			}
		}	
#endif
	}

/**
 * HMAC_SHA1_SignMessageL
 */
void CSmfKeyStoreManager::HMAC_SHA1_SignMessageL()
	{	
	ReadSignParametersL();
	
	RDebug::Printf("SMF: CSmfKeyStoreManager::HMAC_SHA1_SignMessage");
	
	CSHA1* sha = CSHA1::NewL();
	CHMAC* hmac = CHMAC::NewL( iSignParameters->Key(), sha );
	TPtrC8 hashedSig( hmac->Hash( iSignParameters->Message() ) );
	
	RDebug::Printf("SMF: hashed message length=%d", hashedSig.Length());
	RDebug::Printf("SMF: hashed message=%S", &hashedSig);
	
	iMessages[0]->WriteL( 1, hashedSig );
	
	RDebug::Printf("SMF: result written");
	
	iMessages[0]->Complete( KErrNone );
	iMessages.Remove(0);
	
	RDebug::Printf("SMF: message completed");
	
	delete iSignParameters;
	iSignParameters = NULL;	
	}

/**
 * DeleteKeysL
 */
void CSmfKeyStoreManager::DeleteKeysL() 
	{
	RDebug::Printf("SMF: CSmfKeyStoreManager::DeleteKeys");
#ifdef SYMBIAN_V3
	switch ( iState ) 
		{
		case EInitialized:
			{
			iKeys.Reset();
			TCTKeyAttributeFilter filter;
			filter.iKeyAlgorithm = CCTKeyInfo::ERSA;
			iKeyStore->List( iKeys, filter, iStatus );
			iState = EDeletingKey;
			SetActive();
			break;
			}
		case EDeletingKey:
			{
			RDebug::Printf("SMF: key count=%d", iKeys.Count());
			//read the key-label to delete from message
			HBufC* KeyBuf = HBufC::NewL(KMaxSignedMsgLength);
			TPtr keyPtr(KeyBuf->Des());
			iMessages[0]->Read(0, keyPtr);

			// Find the correct key
			bool keyFound = EFalse;
			for (TInt i = 0; i < iKeys.Count(); i++)
				{
				if (iKeys[i]->ID() == (keyPtr.Collapse()))
					{
					RDebug::Printf("SMF: Correct key found");
					iKeyStore->DeleteKey(*iKeys[i], iStatus);
					keyFound = ETrue;
					iState = EKeyDeleted;
					SetActive();
					}
				}
			if (!keyFound)
				{
				iMessages[0]->Complete(KErrNotFound);
				iMessages.Remove( 0 );
				}
			
			
			break;
			}
		}
#endif
	}

/**
 * SetPassphraseTimeout
 */
void CSmfKeyStoreManager::SetPassphraseTimeout() 
	{
#ifdef SYMBIAN_V3
	iKeyStore->SetPassphraseTimeout( -1, iStatus );
	iState = ESettingPassphraseTimeout;
	SetActive();
#endif
	}

/**
 * ReadSignParametersL
 */
void CSmfKeyStoreManager::ReadSignParametersL()
	{
	RMessage2* message = iMessages[0];
	RBuf8 dataBuf;
	CleanupClosePushL( dataBuf );
	dataBuf.CreateL( message->GetDesLength( 0 ) );
	message->ReadL( 0, dataBuf, 0 );

	delete iSignParameters;
	iSignParameters = NULL;
	
	iSignParameters = CSmfSignParameters::NewL( dataBuf );
	
	CleanupStack::PopAndDestroy( &dataBuf );
	}

/**
 * ReadRsaKeyParametersL
 */
void CSmfKeyStoreManager::ReadRsaKeyParametersL()
	{
	RMessage2* message = iMessages[0];

	RBuf8 dataBuf;
	CleanupClosePushL( dataBuf );
	dataBuf.CreateL( message->GetDesLength( 0 ) );
	message->ReadL( 0, dataBuf, 0 );

	delete iRsaKeyParameters;
	iRsaKeyParameters = NULL;
	
	iRsaKeyParameters = CSmfRsaKeyParameters::NewL( dataBuf );
	
	CleanupStack::PopAndDestroy( &dataBuf );
	}
